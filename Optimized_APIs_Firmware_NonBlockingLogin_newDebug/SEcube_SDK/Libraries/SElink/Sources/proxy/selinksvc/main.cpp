#include "util/os.h"
#include "selinksvc/selinksvc.h"

#include <iostream>
#include <locale>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/program_options.hpp>

#include "util/arguments.h"

namespace service = selink::service::wfp;

static bool initialize_log_file();
static boost::filesystem::path log_file;
static const char log_format[] { "[%TimeStamp%]: %Message%" };

static boost::filesystem::path ini_file{ "selinksvc.ini" };

static boost::filesystem::path path_format(std::string arg);

static bool foreground_main();

static VOID WINAPI service_ctrl_handler(DWORD);
static VOID WINAPI service_main(DWORD argc, LPTSTR *argv);

int main(int argc, const char** argv)
{
    std::locale::global(boost::locale::generator().generate(""));
    boost::filesystem::path::imbue(std::locale());

    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help,h",
            "Print help")
        ("log,l",
            po::value<std::string>(),
            "Log file")
        ("config,c",
            po::value<std::string>()->default_value(":selinksvc.json"),
            "Configuration file (default :selinksvc.json)")
        ("provider,p",
            po::value<std::string>()->default_value("se3"),
            "Encryption provider type (soft|secube|se3)")
        ("keys,k",
            po::value<std::string>(),
            "Key file for soft provider")
        ("port,w",
            po::value<uint16_t>()->default_value(10200),
            "Driver connection redirection port")
        ("foreground,f",
            "Run in foreground");

    po::variables_map options;
    bool log_to_file = false;
    try {
        // read options from command line (command line takes precedence)
        po::store(selink::util::arguments::parse_command_line_utf8(argc, argv, desc), options);

        // read options from file
        boost::filesystem::ifstream config_file(selink::util::filesystem::get_exe_path().parent_path() / ini_file);
        if (config_file.is_open()) {
            po::store(po::parse_config_file(config_file, desc), options);
        }

        if (options.count("help")) {
            std::cout << "SELink WFP proxy service" << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }

        if (options.count("log")) {
            log_to_file = true;
            log_file =
                path_format(options["log"].as<std::string>());
        }
        else {
            log_to_file = false;
            log_file = path_format(":selinksvc.log");
        }

        service::globals.config_path =
            path_format(options["config"].as<std::string>());

        std::string provider = options["provider"].as<std::string>();
        if (provider == std::string("soft")) {
            service::globals.encryption_provider_type =
                selink::provider::provider_type::soft;
        }
        else if (provider == std::string("secube")) {
            service::globals.encryption_provider_type =
                selink::provider::provider_type::secube;
        }
        else {
            throw po::error("Invalid provider type");
        }

        if (options.count("keys")) {
            service::globals.key_file_path =
				path_format(options["keys"].as<std::string>());
        }
        else {
            if (service::globals.encryption_provider_type ==
                selink::provider::provider_type::soft)
            {
                throw po::error("Provider \"soft\" requires key file");
            }
        }

        if (options.count("port")) {
            service::globals.wfp_port = options["port"].as<uint16_t>();
        }

        if (options.count("foreground")) {
            service::globals.is_service = false;
        }
        else {
            service::globals.is_service = true;
        }
    }
    catch (po::error& e) {
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        BOOST_LOG_TRIVIAL(fatal) << desc;
        return 1;
    }

    if (service::globals.is_service) {
        // always log to file if possible
        initialize_log_file();

        if (!service::initialize_app()) {
            return 1;
        }

        // run as service, with entry point 'service_main'
        SERVICE_TABLE_ENTRYW ServiceTable[] = {
            { (LPWSTR)service::globals.service_name.c_str(),
                (LPSERVICE_MAIN_FUNCTIONW)service_main },
            { NULL, NULL } };
        if (StartServiceCtrlDispatcherW(ServiceTable) == FALSE) {
            BOOST_LOG_TRIVIAL(fatal) <<
                "StartServiceCtrlDispatcherW failed with error " <<
                (unsigned)GetLastError();
            return 1;
        }
        return 0;
    }
    else {
        if (log_to_file) {
            // log to file if log file path was supplied
            if (!initialize_log_file()) {
                return 1;
            }
        }
        if (!service::initialize_app()) {
            return 1;
        }
        // run in foreground
        bool r = foreground_main();
        return (r) ? (0) : (1);
    }
    return 0;
}

static bool foreground_main()
{
    boost::thread network_thread{ service::network_main };

    BOOST_LOG_TRIVIAL(info) << "Press RETURN to quit...";
    std::getchar();


    if (service::globals.proxy_server) {
        service::globals.proxy_server->stop();
    }
    service::globals.io.stop();

    network_thread.join();
    return true;
}

VOID WINAPI service_main(DWORD argc, LPTSTR *argv)
{
    // register control handler
    service::globals.service_status_handle =
        RegisterServiceCtrlHandlerW(service::globals.service_name.c_str(),
            service_ctrl_handler);
    if (service::globals.service_status_handle == NULL) {
        BOOST_LOG_TRIVIAL(fatal) << "RegisterServiceCtrlHandler failed";
        return;
    }

    // STATUS = START_PENDING
    SERVICE_STATUS* ServiceStatus = &service::globals.service_status;
    ZeroMemory(ServiceStatus, sizeof(SERVICE_STATUS));
    ServiceStatus->dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus->dwControlsAccepted = 0;
    ServiceStatus->dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus->dwWin32ExitCode = 0;
    ServiceStatus->dwServiceSpecificExitCode = 0;
    ServiceStatus->dwCheckPoint = 0;
    if (SetServiceStatus(service::globals.service_status_handle, ServiceStatus) == FALSE) {
        BOOST_LOG_TRIVIAL(error) << "SetServiceStatus(START_PENDING) failed";
    }

    // STATUS = STARTED
    ServiceStatus->dwControlsAccepted = SERVICE_ACCEPT_STOP;
    ServiceStatus->dwCurrentState = SERVICE_RUNNING;
    ServiceStatus->dwWin32ExitCode = 0;
    ServiceStatus->dwCheckPoint = 0;

    if (SetServiceStatus(service::globals.service_status_handle, ServiceStatus) == FALSE) {
        BOOST_LOG_TRIVIAL(fatal) << "SetServiceStatus(RUNNING) failed";
        return;
    }

    // Start worker thread
    boost::thread network_thread{ service::network_main };

    // Wait for worker thread
    network_thread.join();

    // STATUS = STOPPED
    ServiceStatus->dwControlsAccepted = 0;
    ServiceStatus->dwCurrentState = SERVICE_STOPPED;
    ServiceStatus->dwWin32ExitCode = 0;
    ServiceStatus->dwCheckPoint = 3;

    if (SetServiceStatus(service::globals.service_status_handle, ServiceStatus) == FALSE) {
        BOOST_LOG_TRIVIAL(error) << "SetServiceStatus(STOPPED) failed";
    }
}

VOID WINAPI service_ctrl_handler(DWORD CtrlCode)
{
    SERVICE_STATUS* ServiceStatus = &service::globals.service_status;
    switch (CtrlCode) {
    case SERVICE_CONTROL_STOP:
        if (ServiceStatus->dwCurrentState == SERVICE_RUNNING) {
            ServiceStatus->dwControlsAccepted = 0;
            ServiceStatus->dwCurrentState = SERVICE_STOP_PENDING;
            ServiceStatus->dwWin32ExitCode = 0;
            ServiceStatus->dwCheckPoint = 4;

            if (SetServiceStatus(service::globals.service_status_handle, ServiceStatus) == FALSE) {
                BOOST_LOG_TRIVIAL(error) << "SetServiceStatus(STOP_PENDING) failed";
            }

            // make the network thread exit
            if (service::globals.proxy_server) {
                service::globals.proxy_server->stop();
            }
            service::globals.io.stop();
        }
        break;
    default:
        break;
    }
}

static bool initialize_log_file()
{
    BOOST_LOG_TRIVIAL(info) << "Logging to: " << log_file;
    try {
        // log to file
        boost::log::add_common_attributes();
        boost::log::add_file_log(
            boost::log::keywords::file_name = log_file,
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::format = log_format);
    }
    catch (boost::system::system_error&) {
        BOOST_LOG_TRIVIAL(fatal) << "Cannot open log file";
        return false;
    }
    return true;
}

static boost::filesystem::path path_format(std::string arg)
{
    if (arg.length() > 0 && arg[0] == ':') {
        return selink::util::filesystem::get_exe_path().parent_path() /
            selink::util::filesystem::path_from_utf8(arg.substr(1));
    }
    return selink::util::filesystem::path_from_utf8(arg);
}

