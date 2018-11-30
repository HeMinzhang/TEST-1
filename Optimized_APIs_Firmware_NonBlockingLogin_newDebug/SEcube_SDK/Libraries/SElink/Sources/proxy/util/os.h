/**
 *  \file os.h
 *  \author Nicola Ferri
 *  \brief Platform-specific includes and definitions
 */

#pragma once

#if !defined(_WIN32) && !defined(__linux__)
#error "OS not supported"
#endif

// Include os-specific headers

#ifdef _WIN32

#define WINVER 0x0603
#define _WIN32_WINNT 0x0603

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN

#include <Windows.h>

#elif defined(__linux__)

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>

#endif  // _WIN32
