#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <wdf.h>
#include <minwindef.h>

/** global values for the driver */
typedef struct _FILTER_GLOBALS {
    // Driver handles
    WDFDEVICE Device;
    DEVICE_OBJECT* DeviceObject;

    // Driver is stopping?
    BOOL Stopping;

    // WFP handles
    HANDLE EngineHandle;
    HANDLE InjectionHandle;
    HANDLE RedirectHandle;
    UINT32 CalloutC4;
    UINT32 CalloutC6;

    // Service PID
    WDFTIMER Timer;
    DWORD RedirectTargetPID;
    
    // Configuration
    UINT16 PortFirst;
    UINT16 PortLast;
    UINT16 ServicePort;

} FILTER_GLOBALS;

extern FILTER_GLOBALS Globals;

#ifdef _DEBUG
#define PRINTK(f_, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "SF: " f_ "\n", __VA_ARGS__)
#else
#define PRINTK
#endif

/** Set the global fields to default values */
void InitializeGlobals();
