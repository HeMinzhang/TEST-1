#pragma once
//#define NDIS61 1

// Kernel headers
#include <ntifs.h>
#include <ntddk.h>
#include <wdf.h>

// Windows Filtering Platform
#pragma warning(push)
#pragma warning(disable: 4201)    // Disable "Nameless struct/union" compiler warning for fwpsk.h only!
#include <fwpsk.h>                // Functions and enumerated types used to implement callouts in kernel mode
#pragma warning(pop)            // Re-enable "Nameless struct/union" compiler warning
#include <fwpmk.h>
#include <fwpvi.h>

// GUID
#include <guiddef.h>
#include <initguid.h>
#include <devguid.h>

// SOCKADDR_STORAGE
#include <mstcpip.h>

#ifdef _DEBUG
// Print ipv6 address
#define PRINT_ADDRESS_V6(Msg, IpAddress) do { DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, \
    "SF: %s %08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x\n", \
    Msg, \
    (UINT32)(IpAddress)->byteArray16[0], \
    (UINT32)(IpAddress)->byteArray16[1], \
    (UINT32)(IpAddress)->byteArray16[2], \
    (UINT32)(IpAddress)->byteArray16[3], \
    (UINT32)(IpAddress)->byteArray16[4], \
    (UINT32)(IpAddress)->byteArray16[5], \
    (UINT32)(IpAddress)->byteArray16[6], \
    (UINT32)(IpAddress)->byteArray16[7], \
    (UINT32)(IpAddress)->byteArray16[8], \
    (UINT32)(IpAddress)->byteArray16[9], \
    (UINT32)(IpAddress)->byteArray16[10], \
    (UINT32)(IpAddress)->byteArray16[11], \
    (UINT32)(IpAddress)->byteArray16[12], \
    (UINT32)(IpAddress)->byteArray16[13], \
    (UINT32)(IpAddress)->byteArray16[14], \
    (UINT32)(IpAddress)->byteArray16[15] ); } while(0)

// Print ipv4 address
#define PRINT_ADDRESS_V4(Msg, IpAddress) do{ DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, \
    "SF: %s %u.%u.%u.%u\n", \
    Msg, \
    (UINT32)((IpAddress) >> 24), \
    (UINT32)(((IpAddress) >> 16) & 0xFF), \
    (UINT32)(((IpAddress) >> 8) & 0xFF), \
    (UINT32)((IpAddress) & 0xFF) ); } while(0)

#else
#define PRINT_ADDRESS_V4(Msg, IpAddress)
#define PRINT_ADDRESS_V6(Msg, IpAddress)
#endif  // _DEBUG

#define DEVICE_NAME L"selinkflt"

// For memory allocation
#define FILTER_POOL_TAG 'tfLS'

// Redirect context will contain 3 SOCKADDR_STORAGE structures
#define REDIRECT_CONTEXT_SIZE (3*sizeof(SOCKADDR_STORAGE))

// WFP Redirect Provider
DEFINE_GUID( FILTER_REDIRECT_PROVIDER ,
    0x60df82b9, 0xb675, 0x4a5a, 0x8e, 0xda, 0xfa, 0xf0, 0x61, 0x10, 0x63, 0x3);

// WFP Sublayer
#define SUBLAYER_A_NAME L"SELinkSublayer"
#define SUBLAYER_A_DESC L"Default Sublayer"
DEFINE_GUID(SUBLAYER_A_GUID,
    0xcb566ee7, 0x2a4a, 0x4b6d, 0x9b, 0xcb, 0x8b, 0x7e, 0x1d, 0x10, 0xc9, 0x90);

// WFP Connect Redirect Callout (ipv4)
#define CALLOUT_C4_NAME        L"SELinkCalloutC4"
#define CALLOUT_C4_DESC    L"Callout for Connect Redirect v4"
DEFINE_GUID(CALLOUT_C4_GUID,
    0xcb566ee7, 0x2a4a, 0x4b6d, 0x9b, 0xcb, 0x8b, 0x7e, 0x1d, 0x10, 0xc9, 0xa0);

// WFP Connect Redirect Callout (ipv6)
#define CALLOUT_C6_NAME        L"SELinkCalloutC6"
#define CALLOUT_C6_DESC    L"Callout for Connect Redirect v6"
DEFINE_GUID(CALLOUT_C6_GUID,
    0xcb566ee7, 0x2a4a, 0x4b6d, 0x9b, 0xcb, 0x8b, 0x7e, 0x1d, 0x10, 0xc9, 0xa1);

// Driver class
__declspec (selectany) const GUID WFP_DRIVER_CLASS_GUID =
{ 0xc1ea91dc, 0xa37f, 0x453e,{ 0xbf, 0xd5, 0xef, 0x68, 0xe3, 0x6e, 0xed, 0xaa } };

// Context provider for service PID
DEFINE_GUID(FILTER_PROVIDER_CONTEXT_KEY,
    0x971c2ed0, 0x9439, 0x42dc, 0x95, 0x10, 0x7b, 0xed, 0x32, 0x9a, 0x77, 0xba);


