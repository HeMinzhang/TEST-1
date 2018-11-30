/**
 *  \file ipc.cpp
 *  \author Nicola Ferri
 *  \brief Named pipe server for IPC
 */

#include "selinksvc/ipc.h"

#include <stdexcept>

#include <boost/log/trivial.hpp>

namespace selink { namespace service { namespace wfp {

    // throws std::runtime_error
    ipc::ipc()
        : stopping(true), stop_event(NULL)
        , pipe_handle(INVALID_HANDLE_VALUE)
        , pipe_ol({ 0 })
        , wait_reply(false)
    {
        // pipe creation may fail right after restart
        for (int ntry = 0; ntry < 8; ntry++) {
            pipe_handle = CreateNamedPipeW(
                L"\\\\.\\pipe\\b6fb44de3444904803bd4ee1039a2af942ebbede24ddc680f3bb80090b6b7dc7",
                (PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND | PIPE_TYPE_BYTE
                    | PIPE_READMODE_BYTE | FILE_FLAG_OVERLAPPED),
                PIPE_WAIT,
                1,
                1024,
                1024,
                NMPWAIT_USE_DEFAULT_WAIT,
                NULL);
            if (pipe_handle != INVALID_HANDLE_VALUE) {
                break;
            }
            Sleep(50);
        }

        if (pipe_handle == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Pipe creation failed");
        }

        pipe_ol.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (pipe_ol.hEvent != NULL) {
            stop_event = CreateEventW(NULL, TRUE, FALSE, NULL);
            if (stop_event != NULL) {
                stopping = false;
                BOOST_LOG_TRIVIAL(trace) << "Pipe created successfully";
                return;  // success
            }
        }

        // cleanup on fail
        if (pipe_handle != INVALID_HANDLE_VALUE)CloseHandle(pipe_handle);
        if (stop_event != NULL)CloseHandle(stop_event);
        if (pipe_ol.hEvent != NULL)CloseHandle(pipe_ol.hEvent);
    }

    ipc::~ipc()
    {
        if (pipe_handle != INVALID_HANDLE_VALUE)CloseHandle(pipe_handle);
        if (stop_event != NULL)CloseHandle(stop_event);
        if (pipe_ol.hEvent != NULL)CloseHandle(pipe_ol.hEvent);
    }

    ipc::request ipc::run_once()
    {
        auto message_stop = []() {
            request req;
            req.command = request_cmd_stop;
            return req;
        };

        if (stopping || wait_reply) {
            // fatal: run_once must not be called in such states
            return message_stop();
        }

        DWORD dwIgnore;
        BOOL res;
        async_result ares;
        uint8_t buffer[request_packet_size];

        for (;;) {
            res = ConnectNamedPipe(pipe_handle, &pipe_ol);
            ares = async_wait(res, pipe_handle, pipe_ol, dwIgnore, stop_event, INFINITE);
            switch (ares) {
            case async_result::failed:
                continue;  // for(;;)
            case async_result::fatal:
            case async_result::stopping:
                return message_stop();
            case async_result::timeout:
                DisconnectNamedPipe(pipe_handle);
                continue;  // for(;;)
            case async_result::complete:
                // client connected to pipe
                break;  // switch
            }

            // read request
            DWORD bytes_read;

            res = ReadFile(pipe_handle, buffer, request_packet_size, NULL, &pipe_ol);
            ares = async_wait(res, pipe_handle, pipe_ol, bytes_read, stop_event, io_timeout);
            switch (ares) {
            case async_result::failed:
                continue;  // for(;;)
            case async_result::fatal:
            case async_result::stopping:
                return message_stop();
            case async_result::timeout:
                DisconnectNamedPipe(pipe_handle);
                continue;  // for(;;)
            case async_result::complete:
                // request read
                break;  // switch
            }

            // validate request length
            if (bytes_read != request_packet_size) {
                DisconnectNamedPipe(pipe_handle);
                continue;  // for(;;)
            }

            // fill request struct
            request m;
            memcpy(&m.command, buffer, 4);
            memcpy(&m.size, buffer + 4, 4);
            memcpy(&m.data[0], buffer + 8, request_max);

            // validate request fields
            if (m.command == request_cmd_stop || m.size > request_max) {
                DisconnectNamedPipe(pipe_handle);
                continue;  // for(;;)
            }

            // the request is valid
            wait_reply = true;
            return m;
        }
        return message_stop();
    }

    void ipc::reply()
    {
        response msg;
        msg.status = response_status_error;
        reply(msg);
    }

    void ipc::reply(ipc::response msg)
    {
        if (!wait_reply || stopping)return;

        if (msg.data.size() <= response_max) {
            DWORD bytes_written = 0;
            uint8_t header[8];
            BOOL res;
            async_result ares;

            uint32_t data_size = (uint32_t)msg.data.size();
            memcpy(header, &msg.status, 4);
            memcpy(header + 4, &data_size, 4);

            multi_timeout to(io_timeout);

            to.before();
            res = WriteFile(pipe_handle, header, 8, NULL, &pipe_ol);
            ares = async_wait(
                res, pipe_handle, pipe_ol, bytes_written, stop_event, (DWORD)to.value());
            to.after();
            switch (ares) {
            case async_result::failed:
            case async_result::fatal:
            case async_result::stopping:
            case async_result::timeout:
                DisconnectNamedPipe(pipe_handle);
                wait_reply = false;
                return;
            case async_result::complete:
                // response sent
                break;  // switch
            }
            if (bytes_written != 8) {
                DisconnectNamedPipe(pipe_handle);
                wait_reply = false;
                return;
            }
            if (msg.data.empty()) {
                // no data. success
                FlushFileBuffers(pipe_handle);
                DisconnectNamedPipe(pipe_handle);
                wait_reply = false;
                return;
            }

            // send response data
            res = WriteFile(pipe_handle, &msg.data[0], (DWORD)msg.data.size(), NULL, &pipe_ol);
            ares = async_wait(
                res, pipe_handle, pipe_ol, bytes_written, stop_event, (DWORD)to.value());
            to.after();
            if (bytes_written != (DWORD)msg.data.size()) {
                DisconnectNamedPipe(pipe_handle);
                wait_reply = false;
                return;
            }

            FlushFileBuffers(pipe_handle);
        }

        DisconnectNamedPipe(pipe_handle);
        wait_reply = false;
    }

    void ipc::stop()
    {
        stopping = true;
        SetEvent(stop_event);
    }


    ipc::multi_timeout::multi_timeout(unsigned int start_value)
        : value_(start_value)
    { }

    void ipc::multi_timeout::before()
    {
        beg = std::chrono::system_clock::now();
    }

    void ipc::multi_timeout::after()
    {
        uint64_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - beg).count();
        if ((diff > value_) || (value_ - diff < grace_period)) {
            value_ = grace_period;
        }
        else {
            value_ -= diff;
        }
    }

    unsigned int ipc::multi_timeout::value()
    {
        return (unsigned int)value_;
    }


    ipc::async_result ipc::async_wait(
        BOOL result, HANDLE hfile, OVERLAPPED& ol,
        DWORD& bytes_transferred, HANDLE server_stop_event, DWORD timeout_ms)
    {
        // operation completed
        if (result) {
            if (!GetOverlappedResult(hfile, &ol, &bytes_transferred, FALSE)) {
                return async_result::failed;
            }
            return async_result::complete;
        }

        // operation did not complete
        if (GetLastError() != ERROR_IO_PENDING) {
            // ... it actually failed
            return async_result::failed;
        }

        // wait for overlapped io completion or server stop signal
        HANDLE handles[2] = { ol.hEvent, server_stop_event };
        DWORD status = WaitForMultipleObjects(2, handles, FALSE, timeout_ms);

        if (status >= WAIT_OBJECT_0 && status < WAIT_OBJECT_0 + 2) {
            // one of the two events was signaled
            int evt_index = status - WAIT_OBJECT_0;
            if (evt_index == 0) {
                // operation completed
                if (!GetOverlappedResult(hfile, &ol, &bytes_transferred, FALSE)) {
                    return async_result::failed;
                }
                return async_result::complete;
            }
            else if (status == WAIT_TIMEOUT) {
                return async_result::timeout;
            }
            else {
                // server stopping
                return async_result::stopping;
            }
        }
        else {
            // wait failed
            return async_result::fatal;
        }
    }


} } }  // namespace selink::service::wfp

