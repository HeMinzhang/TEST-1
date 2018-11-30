#include "RegistryHelper.h"
#include "Globals.h"

/** \brief read port from registry
 *  
 *  \param hKey registry key handle
 *  \param valueName value name
 *  \param value out read value
 *  \return TRUE on success
 *  \remarks value will remain unchanged if the function fails
 */
static BOOL RegistryReadPort(WDFKEY hKey, PCUNICODE_STRING valueName, UINT16* value);

void LoadConfigFromRegistry(WDFDRIVER driver)
{
    NTSTATUS Status = STATUS_SUCCESS;
    DECLARE_CONST_UNICODE_STRING(valPortFirst, L"PortFirst");
    DECLARE_CONST_UNICODE_STRING(valPortLast, L"PortLast");
    DECLARE_CONST_UNICODE_STRING(valServicePort, L"ServicePort");
    UINT16 PortFirst = 0, PortLast = 0, Port = 0;
    WDFKEY hKey;

    // Open registry key
    Status = WdfDriverOpenParametersRegistryKey(driver, KEY_READ, WDF_NO_OBJECT_ATTRIBUTES, &hKey);
    if (NT_SUCCESS(Status)) {
        // Read port range and validate
        if (RegistryReadPort(hKey, &valPortFirst, &PortFirst)) {
            if (RegistryReadPort(hKey, &valPortLast, &PortLast)) {
                if (PortFirst <= PortLast) {
                    Globals.PortFirst = PortFirst;
                    Globals.PortLast = PortLast;
                }
            }
        }

        // Read service port
        if (RegistryReadPort(hKey, &valServicePort, &Port)) {
            Globals.ServicePort = Port;
        }

        // Close registry key
        WdfRegistryClose(hKey);
    }
    else {
        PRINTK("WdfDriverOpenParametersRegistryKey failed with %u", (unsigned)Status);
    }
}


static BOOL RegistryReadPort(WDFKEY hKey, PCUNICODE_STRING valueName, UINT16* value)
{
    ULONG ulongValue;
    if (NT_SUCCESS(WdfRegistryQueryULong(hKey, valueName, &ulongValue))) {
        if (ulongValue > 0 && ulongValue < 65536) {
            *value = (UINT16)ulongValue;
            return TRUE;
        }
    }
    return FALSE;
}

