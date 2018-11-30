/**
 *  \file wfp_helper.h
 *  \author Nicola Ferri
 *  \brief Windows Filtering Platform helper functions
 */

#pragma once

#include "util/os.h"
#include "util/os_tcpip.h"

#include <cstdint>
#include <vector>
#include <string>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

namespace selink { namespace service { namespace wfp {

    /** Redirection information from the driver */
    struct wfp_socket_info {
        uint16_t family;
        std::vector<uint8_t> local_host;
        std::vector<uint8_t> remote_host;
        uint16_t local_port;
        uint16_t remote_port;
        DWORD local_pid;
    };

    namespace helper {

        /** Address family constants */
        enum {
            family_ipv4 = AF_INET,
            family_ipv6 = AF_INET6
        };

        /** \brief Configure the new socket for a redirected connection
         *  \param fa socket for the incoming connection (program to filter)
         *  \param fb socket for the outgoing connection (filter to destination)
         *  \param info output, structure which will be filled with the redirection information
         *       coming from the driver
         *  \return true on success
         *  \remarks this function may fail if the current process is not elevated
         *  
         *  Redirection information coming from the driver is extracted from socket 'fa' and
         *  relevant fields are injected in socket 'fb' for the driver to keep track of the
         *  redirection. Information for filtering and redirection is stored in 'info'
         */
        bool redirect_setup(
            boost::asio::ip::tcp::socket& fa,
            boost::asio::ip::tcp::socket& fb,
            wfp_socket_info* info);

        /** \brief Get the IP address from a SOCKADDR_STORAGE
         *  \param sa the SOCKADDR_STORAGE structure
         *  \return IP address as byte vector
         *  
         *  The returned vector contains for bytes for an ipv4 address, or
         *  16 bytes for an ipv6 address.
         */
        std::vector<uint8_t> sockaddr_storage_host(SOCKADDR_STORAGE* sa);

        /** \brief Convert IP address to string
         *  \param vector containing the IP address. see sockaddr_storage_host()
         *  \return string representation of the IP address
         */
        std::string address_string(std::vector<uint8_t> const& a);

        /** \brief Get the port from a SOCKADDR_STORAGE
         *  \param sa the SOCKADDR_STORAGE structure
         *  \return the port
         */
        uint16_t sockaddr_storage_port(SOCKADDR_STORAGE* sa);

        /** \brief Get the executable's file name from a process id
         *  \param pid process id
         *  \return a boost path object
         */
        boost::filesystem::path pid_to_path(DWORD pid);

        /** \brief Store a proccess id in a dedicated structure readable from the driver
         *  \param pid the process id
         *  \return true on success
         *  \remarks this function may fail if the current process is not elevated
         *  
         *  The driver needs to know the filter service's process id in order to redirect
         *  the connections. This function makes the process id available for use within the driver.
         */
        bool add_pid_to_wfp_context(DWORD pid);

    }  // namespace helper

} } }  // namespace selink::service::wfp
