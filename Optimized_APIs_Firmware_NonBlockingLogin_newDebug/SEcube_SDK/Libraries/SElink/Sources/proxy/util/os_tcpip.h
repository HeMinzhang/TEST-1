/**
 *  \file os_tcpip.h
 *  \author Nicola Ferri
 *  \brief Platform-specific includes and definitions for networking
 */

#pragma once

#include "util/os.h"

// Include os-specific headers for networking

#ifdef _WIN32

#include <Winsock2.h>
#include <mstcpip.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#endif  // _WIN32
