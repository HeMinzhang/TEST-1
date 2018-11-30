/**
 *  \file ipc.h
 *  \author Nicola Ferri
 *  \brief Named pipe server for IPC
 */

#pragma once
#include "util/os.h"

#include <cstdint>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <chrono>

namespace selink { namespace service { namespace wfp {

    /** Server for inter process communication, using a named pipe.
     */
    class ipc
    {
    public:
        /** Base protocol parameters */
        enum : uint32_t {
            request_max = 64,
            request_cmd_stop = 0xFFFFFFFF,
            response_max = (16 * 1024),

            response_status_error = 0xFFFFFFFF,
            response_status_ok = 0,
            response_status_not_available = 1
        };

        /** Request packet structure, fixed size payload */
        struct request {
            uint32_t command{ 0 };
            uint32_t size{ 0 };
            std::array<uint8_t, request_max> data;
        };

        /** Response packet structure, variable size payload */
        struct response {
            uint32_t status{ 0 };
            std::vector<uint8_t> data;
        };

        ipc();
        ~ipc();

        /** \brief Wait for a request
         *  \return the request packet
         *  
         *  The function returns either when a request is received, or when the server
         *  is stopped from a different thread. In the latter case, the request command
         *  field is set to request_cmd_stop.
         */
        request run_once();
        void stop();
        void reply();
        void reply(response msg);

    private:
        enum {
            request_packet_size = (4 + 4 + request_max),
            io_timeout = 3000
        };

        HANDLE pipe_handle;
        HANDLE stop_event;
        OVERLAPPED pipe_ol;
        bool stopping;
        bool wait_reply;

        /** asynchronous io call result type*/
        enum class async_result {
            stopping, complete, failed, fatal, timeout
        };

        /** Helper for asynchronous io (Windows overlapped) */
        static async_result async_wait(
            BOOL result, HANDLE hfile, OVERLAPPED& ol,
            DWORD& bytes_transferred, HANDLE server_stop_event, DWORD timeout_ms);

        /** Utility to set a global timeout for multiple operations
         *  Usage example:
         *  \code{.cpp}
         *      multi_timeout to(timeout_ms);
         *      while(operation_must_run_again){
         *          to.before();
         *          operation_with_timeout(to.value());
         *          to.after();
         *      }
         *  \endcode
         */
        class multi_timeout {
        public:
            /** \brief multi-timeout constructor
             *  \param start_value the value of the global timeout, in milliseconds
             */
            explicit multi_timeout(unsigned int start_value);

            /** \brief Begin timing an operation
             *  
             *  Run this before running an operation subject to timeout
             */
            void before();

            /** \brief Finish timing an operation
             *  
             *  Run this after running an operation subject to timeout
             */
            void after();

            /** \brief Get remaining time
             *  \return remaining time until the deadline
             *  
             *  Use this value as the timeout value to pass to the operation.
             *  Always returns 1 after the deadline
             */
            unsigned int value();
        private:
            uint64_t value_;
            std::chrono::time_point<std::chrono::system_clock> beg;
            enum : unsigned int {
                grace_period = 1
            };
        };
    };

} } }  // namespace selink::service::wfp
