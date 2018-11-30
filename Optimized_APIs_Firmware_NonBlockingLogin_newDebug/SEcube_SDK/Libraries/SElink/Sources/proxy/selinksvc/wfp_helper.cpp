/**
 *  \file wfp_helper.cpp
 *  \author Nicola Ferri
 *  \brief Windows Filtering Platform helper functions
 */

#pragma comment(lib, "fwpuclnt")
#pragma comment(lib, "psapi")

#include "selinksvc/wfp_helper.h"

#include <fwpmu.h>
#include <guiddef.h>
#include <psapi.h>
#include <boost/log/trivial.hpp>

namespace selink { namespace service { namespace wfp { namespace helper {

    // descriptors for the WFP provider context which holds the service PID
    static const std::wstring FILTER_PROVIDER_CONTEXT_NAME = L"FilterSvcPid";
    static const std::wstring FILTER_PROVIDER_CONTEXT_DESC = L"Filter Service PID";
    static const GUID FILTER_PROVIDER_CONTEXT_KEY =
        { 0x971c2ed0, 0x9439, 0x42dc, { 0x95, 0x10, 0x7b, 0xed, 0x32, 0x9a, 0x77, 0xba } };

    enum {
        REDIRECT_RECORDS_SIZE = (2048),
        REDIRECT_CONTEXT_SIZE = (sizeof(SOCKADDR_STORAGE)*3)
    };

    bool redirect_setup(
        boost::asio::ip::tcp::socket& fa,
        boost::asio::ip::tcp::socket& fb,
        wfp_socket_info* info )
    {
        // allocate required structures
        BYTE** ppRedirectRecords = (BYTE**)calloc(REDIRECT_RECORDS_SIZE, sizeof(BYTE*));
        BYTE** ppRedirectContext = (BYTE**)calloc(REDIRECT_CONTEXT_SIZE, sizeof(BYTE*));
        SIZE_T redirectRecordsSize = 0;
        SIZE_T redirectRecordsSet = 0;
        SIZE_T redirectContextSize = 0;
        SOCKADDR_STORAGE localAddressAndPort, remoteAddressAndPort, localPid;
        int status;
        bool success = false;

        // break on fail
        do {
            // read redirect records
            status = WSAIoctl(
                fa.native_handle(),
                SIO_QUERY_WFP_CONNECTION_REDIRECT_RECORDS,
                0,
                0,
                (BYTE*)ppRedirectRecords,
                REDIRECT_RECORDS_SIZE,
                (LPDWORD)&redirectRecordsSize,
                0,
                0);

            if (status != ERROR_SUCCESS) {
                break;
            }

            // read redirect context
            status = WSAIoctl(
                fa.native_handle(),
                SIO_QUERY_WFP_CONNECTION_REDIRECT_CONTEXT,
                0,
                0,
                (BYTE*)ppRedirectContext,
                REDIRECT_CONTEXT_SIZE,
                (LPDWORD)&redirectContextSize,
                0,
                0);

            if (status != ERROR_SUCCESS) {
                break;
            }

            // read the redirect context values into a wfp_info struct
            RtlCopyMemory(&localAddressAndPort,
                &(((SOCKADDR_STORAGE*)ppRedirectContext)[0]),
                sizeof(SOCKADDR_STORAGE));
            RtlCopyMemory(&remoteAddressAndPort,
                &(((SOCKADDR_STORAGE*)ppRedirectContext)[1]),
                sizeof(SOCKADDR_STORAGE));
            RtlCopyMemory(&localPid,
                &(((SOCKADDR_STORAGE*)ppRedirectContext)[2]),
                sizeof(SOCKADDR_STORAGE));
            info->family = remoteAddressAndPort.ss_family;
            info->local_host = sockaddr_storage_host(&localAddressAndPort);
            info->remote_host = sockaddr_storage_host(&remoteAddressAndPort);
            info->local_port = sockaddr_storage_port(&localAddressAndPort);
            info->remote_port = sockaddr_storage_port(&remoteAddressAndPort);
            info->local_pid = (DWORD)*((UINT64*)&localPid);

            // check if the socket family is ipv4 or ipv6
            if (info->family != AF_INET && info->family != AF_INET6) {
                BOOST_LOG_TRIVIAL(error) <<
                    "redirect_setup address family " << 
                    (unsigned)info->family << " not supported";
                break;
            }

            // create a new socket, with overlapped i/o enabled
            SOCKET fb_native = WSASocketW(
                info->family,
                SOCK_STREAM,
                IPPROTO_TCP,
                NULL,
                0,
                WSA_FLAG_OVERLAPPED);
            
            // attach the REDIRECT_RECORDS to the new socket
            status = WSAIoctl(
                fb_native,
                SIO_SET_WFP_CONNECTION_REDIRECT_RECORDS,
                (BYTE*)ppRedirectRecords,
                (DWORD)redirectRecordsSize,
                0,
                0,
                (LPDWORD)&redirectRecordsSet,
                0,
                0);

            if (status != ERROR_SUCCESS) {
                BOOST_LOG_TRIVIAL(error) <<
                    "redirect_setup WSAIoctl failed with error code " <<
                    (unsigned)WSAGetLastError();
                closesocket(fb_native);
                break;
            }

            // assign the new socket to a boost::asio socket object
            try {
                fb.assign(
                    ((info->family == AF_INET) ?
                    (boost::asio::ip::tcp::v4()) : (boost::asio::ip::tcp::v6())),
                    fb_native);
            }
            catch (boost::system::system_error&) {
                break;
            }

            // everything OK
            success = true;
        } while (false);

        // cleanup
        free(ppRedirectRecords);
        free(ppRedirectContext);

        return success;
    }


    std::string address_string(std::vector<uint8_t> const& a)
    {
        char tmp[64];
        tmp[0] = '\0';
        if (a.size() == 4) {
            sprintf_s(
                tmp,
                "%u.%u.%u.%u",
                (unsigned)a[0],
                (unsigned)a[1],
                (unsigned)a[2],
                (unsigned)a[3]);
        }
        else if (a.size() == 16) {
            sprintf_s(
                tmp,
                "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                (unsigned)a[0], (unsigned)a[1],
                (unsigned)a[2], (unsigned)a[3],
                (unsigned)a[4], (unsigned)a[5],
                (unsigned)a[6], (unsigned)a[7],
                (unsigned)a[8], (unsigned)a[9],
                (unsigned)a[10], (unsigned)a[11],
                (unsigned)a[12], (unsigned)a[13],
                (unsigned)a[14], (unsigned)a[15]);
        }
        return std::string(tmp);
    }

    std::vector<uint8_t> sockaddr_storage_host(SOCKADDR_STORAGE* sa)
    {
        if (sa->ss_family == AF_INET) {
            struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(sa);
            uint8_t* addr_data = (uint8_t*)&(in4->sin_addr.s_addr);
            return std::vector<uint8_t>(addr_data, addr_data + 4);
        }
        else if (sa->ss_family == AF_INET6) {
            struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(sa);
            uint8_t* addr_data = in6->sin6_addr.s6_addr;
            return std::vector<uint8_t>(addr_data, addr_data + 16);
        }
        return{};
    }

    uint16_t sockaddr_storage_port(SOCKADDR_STORAGE* sa)
    {
        uint16_t tmp = 0;
        if (sa->ss_family == AF_INET) {
            struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(sa);
            tmp = ntohs(in4->sin_port);
        }
        else if (sa->ss_family == AF_INET6) {
            struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(sa);
            tmp = ntohs(in6->sin6_port);
        }
        return tmp;
    }

    bool add_pid_to_wfp_context(DWORD pid)
    {
        // get a handle to perform WFP operations
        FWPM_SESSION Session = { 0 };
        HANDLE EngineHandle;
        DWORD Status = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &Session, &EngineHandle);
        if (Status != ERROR_SUCCESS) {
            return false;
        }
        
        // prepare the provider context (descriptor and data)
        FWPM_PROVIDER_CONTEXT Context = { 0 };
        Context.providerContextKey = FILTER_PROVIDER_CONTEXT_KEY;
        Context.displayData.name = (wchar_t*)FILTER_PROVIDER_CONTEXT_NAME.c_str();
        Context.displayData.description = (wchar_t*)FILTER_PROVIDER_CONTEXT_DESC.c_str();
        Context.type = FWPM_GENERAL_CONTEXT;

        FWP_BYTE_BLOB* ProviderContextData = (FWP_BYTE_BLOB*)malloc(sizeof(FWP_BYTE_BLOB));
        ProviderContextData->size = 8;
        ProviderContextData->data = (UINT8*)malloc(ProviderContextData->size);
        *((UINT64*)(ProviderContextData->data)) = (UINT64)pid;
        Context.dataBuffer = ProviderContextData;

        // add the new provider context containing the PID
        Status = FwpmProviderContextAdd(
            EngineHandle, &Context, 0, &Context.providerContextId);

        // if a provider context already exists, delete it and create a new one
        if (Status == FWP_E_ALREADY_EXISTS) {
            Status = FwpmProviderContextDeleteByKey(
                EngineHandle, &FILTER_PROVIDER_CONTEXT_KEY);

            if (Status == ERROR_SUCCESS) {
                Status = FwpmProviderContextAdd(
                    EngineHandle, &Context, 0, &Context.providerContextId);
            }
        }

        // cleanup
        free(ProviderContextData->data);
        free(ProviderContextData);

        return (Status == ERROR_SUCCESS);
    }

    boost::filesystem::path pid_to_path(DWORD pid)
    {
        HANDLE hProcess = NULL;
        WCHAR path[MAX_PATH];

        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess != NULL) {
            DWORD r = GetModuleFileNameExW(hProcess, NULL, path, MAX_PATH);
            CloseHandle(hProcess);
            if (r > 0) {
                _wcslwr_s(path);
                return boost::filesystem::path(std::wstring(path));
            }
        }
        return boost::filesystem::path("");
    }

} } } }  // namespace selink::service::wfp::helper
