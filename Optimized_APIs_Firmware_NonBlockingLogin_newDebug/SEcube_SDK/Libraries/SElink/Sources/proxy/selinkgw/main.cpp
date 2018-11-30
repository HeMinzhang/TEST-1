/**
 *  \file main.cpp
 *  \author Nicola Ferri
 *  \brief SElink gateway configuration and startup
 */

#include "util/os.h"

#include <ctime>
#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>

#include "util/filesystem.h"
#include "util/arguments.h"
#include "selinkgw/selinkgw.h"

namespace service = selink::service::portmap;

static const char log_format[]{ "[%TimeStamp%]: %Message%" };
#ifdef __linux__
static void handle_signal(int sig);
static bool daemon_main();
static void daemonize();
static boost::filesystem::path pid_file{ "/var/run/selinkgw.pid" };
static boost::filesystem::path log_file{ "/var/log/selinkgw.log" };
static pid_t pid_fd = -1;
#else
static boost::filesystem::path log_file{ "selinkgw.log" };
#endif  // __linux__

static boost::filesystem::path ini_file{ "selinkgw.ini" };

static boost::filesystem::path path_format(std::string arg);
static bool fg_main();
static bool initialize_log_file();
static bool read_serial_number(std::string value,
    std::array<uint8_t, selink::provider::provider::sn_size>& sn);
static bool read_pin(boost::filesystem::path path,
    std::vector<uint8_t>& pin);

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
            po::value<std::string>()->default_value(":selinkgw.json"),
            "Configuration file")
        ("provider,p",
            po::value<std::string>()->default_value("soft"),
            "Encryption provider type (soft|secube)")
        ("keys,k",
            po::value<std::string>()->default_value(":keys.json"),
            "Key file for provider \"soft\"")
        ("serial-number,d",
            po::value<std::string>(),
            "Device serial for provider \"secube\" (as hex)")
        ("pin,z",
            po::value<std::string>(),
            "Pin file for provider \"secube\"")
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
            std::cout << "SELink PortMap proxy service" << std::endl;
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

        service::globals.key_file_path =
            path_format(options["keys"].as<std::string>());

        if (options.count("serial-number")) {
            if (!read_serial_number(
                options["serial-number"].as<std::string>(),
                service::globals.device_sn))
            {
                throw po::error("Serial number is not valid. Please use a 32 byte hex string");
            }
        }
        else {
            if (service::globals.encryption_provider_type ==
                selink::provider::provider_type::secube)
            {
                throw po::error("Provider \"secube\" requires serial number");
            }
        }

        if (options.count("pin")) {
            if (!read_pin(
                path_format(options["pin"].as<std::string>()),
                service::globals.device_pin))
            {
                throw po::error("Cannot read device pin from file");
            }
        }
        else {
            if (service::globals.encryption_provider_type ==
                selink::provider::provider_type::secube)
            {
                throw po::error("Provider \"secube\" requires pin file");
            }
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


#if defined(_WIN32)
    service::globals.is_service = false;
    BOOST_LOG_TRIVIAL(error) << "Daemon mode not available: running in foreground";
#endif  // _WIN32

    if (service::globals.is_service) {
#if defined(_WIN32)
        return 1;
#elif defined(__linux__)
        bool r = daemon_main();
        return (r) ? (0) : (1);
#endif  // __linux__
    }
    else {
        if (log_to_file) {
            if (!initialize_log_file()) {
                return 1;
            }
        }
        if (!service::initialize_app()) {
            return 1;
        }
        // run in foreground
        bool r = fg_main();
        return (r) ? (0) : (1);
    }
    return 0;
}

static bool fg_main()
{
    boost::thread network_thread{ service::network_main };

    BOOST_LOG_TRIVIAL(info) << "Press RETURN to stop...";
    std::getchar();
    // delete all acceptors BEFORE stopping the service
    service::globals.proxy_server->stop();
    service::globals.io.stop();

    network_thread.join();
    return true;
}


#ifdef __linux__

static bool daemon_main()
{
    daemonize();

    if (!initialize_log_file()) {
        // cannot write to log file
        if (pid_fd != -1) {
            lockf(pid_fd, F_ULOCK, 0);
            close(pid_fd);
        }
        unlink(pid_file.native().c_str());
        signal(SIGINT, SIG_DFL);
        return false;
    }
    if (!service::initialize_app()) {
        return 1;
    }
    boost::thread network_thread{ service::network_main };
    network_thread.join();
    return true;
}

static void handle_signal(int sig)
{
    if (sig == SIGINT) {
        BOOST_LOG_TRIVIAL(trace) << "SIGINT received";

        // unlock
        if (pid_fd != -1) {
            lockf(pid_fd, F_ULOCK, 0);
            close(pid_fd);
        }

        unlink(pid_file.native().c_str());
        // reset signal handling
        signal(SIGINT, SIG_DFL);
        service::globals.io.stop();
    }
    else if (sig == SIGHUP) {
        // reload
        BOOST_LOG_TRIVIAL(trace) << "SIGHUP received";
        service::proxy_server_update();
    }
    else if (sig == SIGCHLD) {
        BOOST_LOG_TRIVIAL(trace) << "SIGCHLD received";
    }
}

static void daemonize()
{
    int fd;

    pid_t pid = fork();

    // on error, abort
    if (pid < 0) {
        BOOST_LOG_TRIVIAL(fatal) << "fork error";
        exit(1);
    }

    // terminate parent
    if (pid > 0) exit(0);

    if (setsid() < 0)exit(1);  // process group leader
    signal(SIGCHLD, SIG_IGN);  // ignore sig from children

    // fork again
    pid = fork();
    if (pid < 0)exit(1);
    if (pid > 0)exit(0);

    umask(0);
    chdir(selink::util::filesystem::get_exe_path().native().c_str());

    // close all open file descriptors
    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--)
    {
        close(fd);
    }
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");

    // write lockfile

    char str[256];
    pid_fd = open(pid_file.native().c_str(), O_RDWR | O_CREAT, 0640);
    if (pid_fd < 0)exit(1);
    if (lockf(pid_fd, F_TLOCK, 0) < 0)exit(1);
    // write pid
    snprintf(str, sizeof(str), "%d\n", getpid());
    write(pid_fd, str, strlen(str));

    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);
}
#endif  // __linux__


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

static bool read_serial_number(std::string value,
    std::array<uint8_t, selink::provider::provider::sn_size>& sn)
{
    try {
        boost::algorithm::unhex(
            value,
            sn.begin());
    }
    catch (std::exception&) {
        return false;
    }
    return true;
}
static bool read_pin(boost::filesystem::path path,
    std::vector<uint8_t>& pin)
{
    boost::filesystem::basic_ifstream<char> pinfile(path, std::ios::binary | std::ios::in);
    if (!pinfile.is_open()) {
        return false;
    }
    pinfile.seekg(0, std::ios::end);
    std::streampos pos = pinfile.tellg();
    if((size_t)pos == 0 || pos > 32){
        BOOST_LOG_TRIVIAL(fatal) << "Pin file must contain 1 to 32 bytes";
        return false;
    }
    BOOST_LOG_TRIVIAL(trace) << (size_t)pos;
    std::vector<uint8_t> buf((size_t)pos);
    pinfile.seekg(0, std::ios::beg);
    pinfile.read((char*)&buf[0], (size_t)pos);
    BOOST_LOG_TRIVIAL(trace) << "pin " << (unsigned)buf[0];
    std::streampos count = pinfile.gcount();
    pinfile.close();
    if (count != pos) {
        BOOST_LOG_TRIVIAL(fatal) << count;
        return false;
    }
    pin.resize((size_t)count);
    memcpy(&pin[0], &buf[0], (size_t)count);
    return true;
}
