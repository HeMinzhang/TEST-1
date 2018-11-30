#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <wdf.h>

/** \brief Load the configuration from registry
 *  \param driver WDF driver handle
 *  \remarks Values that cannot be read are left unchanged
 *  
 *  The following fields are configured:
 *  - Globals.PortFirst
 *  - Globals.PortLast
 *  - Globals.ServicePort
 */
void LoadConfigFromRegistry(WDFDRIVER driver);
