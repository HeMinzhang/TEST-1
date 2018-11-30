#include "Filter.h"
#include "Globals.h"
#include "RegistryHelper.h"

/** Driver entry point */
DRIVER_INITIALIZE DriverEntry;
/** Driver exit point */
EVT_WDF_DRIVER_UNLOAD EvtUnload;

/** Initializes required WDFDriver and WDFDevice objects */
static NTSTATUS InitializeDriverObjects(
    DRIVER_OBJECT * DriverObject, UNICODE_STRING * RegistryPath,
    WDFDRIVER * pDriver, WDFDEVICE * pDevice);

/** Register all callouts to redirect connections */
static NTSTATUS RegisterCallouts(DEVICE_OBJECT * DeviceObject);

/** \brief Register a single callout
 *  
 *  \param DeviceObject WDM device object
 *  \param LayerKey GUID identifying the layer
 *  \param CalloutKey GUID identifying the callout
 *  \param DisplayData WPF display data, including name and description
 *  \param CalloutId out ID of the newly created callout
 *  \param IsV6 create callout for ipv6
 */
static NTSTATUS RegisterConnectRedirectCallout(
    DEVICE_OBJECT * DeviceObject,
    const GUID* LayerKey,
    const GUID* CalloutKey,
    FWPM_DISPLAY_DATA* DisplayData,
    UINT32* CalloutId,
    BOOL IsV6);

/** \brief Unregister all callouts
 */
static VOID UnregisterCallouts();

/** \brief Classify function for outgoing ipv4 connection
 */
VOID NTAPI ClassifyC4(
    _In_ const FWPS_INCOMING_VALUES* InFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    _In_ PVOID LayerData,
    _In_ const VOID* ClassifyContext,
    _In_ const FWPS_FILTER* Filter,
    _In_ UINT64 InFlowContext,
    _Inout_ FWPS_CLASSIFY_OUT* ClassifyOut);

/** \brief Classify function for outgoing ipv6 connection
*/
VOID NTAPI ClassifyC6(
    _In_ const FWPS_INCOMING_VALUES* InFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    _In_ PVOID LayerData,
    _In_ const VOID* ClassifyContext,
    _In_ const FWPS_FILTER* Filter,
    _In_ UINT64 InFlowContext,
    _Inout_ FWPS_CLASSIFY_OUT* ClassifyOut);

/** \brief Classify logic
 *
 * Actual logic of classify function, for both ipv4 and ipv6 outgoing connections.
 */
static VOID Classify(
    const FWPS_INCOMING_VALUES* InFixedValues,
    const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    const VOID* ClassifyContext,
    const FWPS_FILTER* Filter,
    FWPS_CLASSIFY_OUT* ClassifyOut,
    BOOL IsV6);

/** \brief Notify function for outgoing connection
*/
NTSTATUS NotifyC(
    _In_ FWPS_CALLOUT_NOTIFY_TYPE NotifyType,
    _In_ const GUID* FilterKey,
    _In_ const FWPS_FILTER* Filter);

/** \brief Check wheter an ipv4 address is localhost
 */
static BOOL IsLocalV4(UINT32 IpAddress);

/** \brief Check wheter an ipv6 address is localhost
 */
static BOOL IsLocalV6(FWP_BYTE_ARRAY16* IpAddress);

/** \brief Try to retrieve the service's PID
 *  
 *  Read the provider context which should store the service's PID.
 *  Update the value in Globals if it changed.
 */
static BOOL UpdateServicePID();

/** \brief Start polling the service's PID
 *  
 *  Starts a timer which periodically reads and updates the service PID.
 *  See also UpdateServicePid().
 */
static BOOL StartPeriodicTask();

/** Service PID polling task callback function */
EVT_WDF_TIMER EvtPeriodicTask;

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS Status = STATUS_SUCCESS;
    WDFDRIVER driver = { 0 };
    WDFDEVICE device = { 0 };
    DEVICE_OBJECT * DeviceObject = NULL;

    PRINTK("DriverEntry");
    InitializeGlobals();

    do {
        Status = InitializeDriverObjects(DriverObject, RegistryPath, &driver, &device);
        if (!NT_SUCCESS(Status))break;

        LoadConfigFromRegistry(driver);

        Globals.Device = device;
        DeviceObject = WdfDeviceWdmGetDeviceObject(device);
        Globals.DeviceObject = DeviceObject;
        
        UpdateServicePID();
        if (!StartPeriodicTask()) {
            Status = STATUS_FAILED_DRIVER_ENTRY;
            break;
        }

        Status = FwpsRedirectHandleCreate(
            &FILTER_REDIRECT_PROVIDER, 0, &Globals.RedirectHandle);
        if (!NT_SUCCESS(Status))break;
        
        Status = RegisterCallouts(DeviceObject);
        if (!NT_SUCCESS(Status))break;

    } while (FALSE);
    
    PRINTK("DriverEntry %08x", Status);

    if (!NT_SUCCESS(Status)) {
        if (Globals.RedirectHandle != NULL) {
            FwpsRedirectHandleDestroy(Globals.RedirectHandle);
        }
        Status = STATUS_FAILED_DRIVER_ENTRY;
    }

    return Status;
}

static NTSTATUS InitializeDriverObjects(
    DRIVER_OBJECT * DriverObject, 
    UNICODE_STRING * RegistryPath,
    WDFDRIVER * pDriver,
    WDFDEVICE * pDevice)
{
    NTSTATUS Status;
    WDF_DRIVER_CONFIG config;
    //UNICODE_STRING DeviceName = { 0 };
    //UNICODE_STRING DosDeviceName = { 0 };
    PWDFDEVICE_INIT pInit = NULL;

    //RtlInitUnicodeString(&DeviceName, L"\\Devices\\" DEVICE_NAME);
    //RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\" DEVICE_NAME);

    // Initialize WDF device
    WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);
    config.DriverInitFlags |= WdfDriverInitNonPnpDriver;
    config.EvtDriverUnload = EvtUnload; // <-- Necessary for this driver to unload correctly
    Status = WdfDriverCreate(
        DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, pDriver);
    if (NT_SUCCESS(Status)) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        pInit = WdfControlDeviceInitAllocate(*pDriver, &SDDL_DEVOBJ_KERNEL_ONLY);

        if (pInit) {
            //WdfDeviceInitSetDeviceType(pInit, FILE_DEVICE_NETWORK);
            WdfDeviceInitSetCharacteristics(pInit, FILE_AUTOGENERATED_DEVICE_NAME, TRUE);
            WdfDeviceInitSetDeviceClass(pInit, &WFP_DRIVER_CLASS_GUID);
            WdfDeviceInitSetCharacteristics(pInit, FILE_DEVICE_SECURE_OPEN, TRUE);
            //WdfDeviceInitAssignName(pInit, &DeviceName);
            //IoCreateSymbolicLink(&DosDeviceName, &DeviceName);

            Status = WdfDeviceCreate(&pInit, WDF_NO_OBJECT_ATTRIBUTES, pDevice);

            if (NT_SUCCESS(Status)) {
                WdfControlFinishInitializing(*pDevice);
            }
        }
    }

    if (!NT_SUCCESS(Status)) {
        if (pInit)
            WdfDeviceInitFree(pInit);
    }

    return Status;
}


static NTSTATUS RegisterCallouts(DEVICE_OBJECT * DeviceObject)
{
    NTSTATUS Status;
    BOOL EngineOpened=FALSE;

    FWPM_SESSION Session = { 0 };
    FWPM_SUBLAYER0 Sublayer = { 0 };
    FWPM_DISPLAY_DATA DisplayData = { 0 };

    Session.flags = FWPM_SESSION_FLAG_DYNAMIC;  // Release callouts on driver unload

    Status = FwpmEngineOpen(
        NULL,
        RPC_C_AUTHN_WINNT,
        NULL,
        &Session,
        &Globals.EngineHandle);

    if (NT_SUCCESS(Status)) {
        EngineOpened = TRUE;
        Status = FwpmTransactionBegin(Globals.EngineHandle, 0);
        if (NT_SUCCESS(Status)) {
            do {
                // Register sublayer 

                Sublayer.subLayerKey = SUBLAYER_A_GUID;
                Sublayer.displayData.name = SUBLAYER_A_NAME;
                Sublayer.displayData.description = SUBLAYER_A_DESC;
                Sublayer.weight = 0x40;

                Status = FwpmSubLayerAdd(Globals.EngineHandle, &Sublayer, NULL);
                PRINTK("FwpmSubLayerAdd %08x", Status);
                if (!NT_SUCCESS(Status)) break;

                // Register ipv4 connect redirect callout
                DisplayData.name = CALLOUT_C4_NAME;
                DisplayData.description = CALLOUT_C4_DESC;
                Status = RegisterConnectRedirectCallout(
                    DeviceObject,
                    &FWPM_LAYER_ALE_CONNECT_REDIRECT_V4,
                    &CALLOUT_C4_GUID,
                    &DisplayData,
                    &Globals.CalloutC4,
                    FALSE);
                PRINTK("RegisterConnectRedirectCallout v4 %08x", Status);
                if (!NT_SUCCESS(Status)) break;

                // Register ipv6 connect redirect callout
                DisplayData.name = CALLOUT_C6_NAME;
                DisplayData.description = CALLOUT_C6_DESC;
                Status = RegisterConnectRedirectCallout(
                    DeviceObject,
                    &FWPM_LAYER_ALE_CONNECT_REDIRECT_V6,
                    &CALLOUT_C6_GUID,
                    &DisplayData,
                    &Globals.CalloutC6,
                    TRUE);
                PRINTK("RegisterConnectRedirectCallout v6 %08x", Status);
                if (!NT_SUCCESS(Status)) break;

            } while (FALSE);
            
            if (!NT_SUCCESS(Status)) {
                PRINTK("Aborting Transaction");
                FwpmTransactionAbort(Globals.EngineHandle);
            }
            else {
                Status = FwpmTransactionCommit(Globals.EngineHandle);
            }
        }
    }

    if (!NT_SUCCESS(Status)) {
        if (EngineOpened) {
            PRINTK("Closing Engine");
            FwpmEngineClose(Globals.EngineHandle);
            Globals.EngineHandle = NULL;
        }
    }

    return Status;
}

static NTSTATUS RegisterConnectRedirectCallout(
    DEVICE_OBJECT * DeviceObject,
    const GUID* LayerKey,
    const GUID* CalloutKey,
    FWPM_DISPLAY_DATA* DisplayData,
    UINT32* CalloutId,
    BOOL IsV6)
{
    NTSTATUS Status = STATUS_SUCCESS;
    FWPS_CALLOUT sCallout = { 0 };
    FWPM_CALLOUT mCallout = { 0 };
    BOOL CalloutRegistered = FALSE;

    FWPM_FILTER filter = { 0 };
    FWPM_FILTER_CONDITION filterConditions[4] = { 0 };
    FWP_RANGE portRange;
    UINT32 ci = 0;

    if (Globals.EngineHandle == NULL)
        return STATUS_INVALID_HANDLE;

    //Register callout
    sCallout.calloutKey = *CalloutKey;
    sCallout.classifyFn = ((IsV6)?(ClassifyC6):(ClassifyC4));
    sCallout.notifyFn = NotifyC;
    Status = FwpsCalloutRegister((void *)DeviceObject, &sCallout, CalloutId);
    PRINTK("FwpsCalloutRegister %08x", Status);

    if (NT_SUCCESS(Status)) {
        CalloutRegistered = TRUE;

        //Add callout
        mCallout.calloutKey = *CalloutKey;
        mCallout.displayData = *DisplayData;
        mCallout.applicableLayer = *LayerKey;
        //mCallout.flags = 0;

        Status = FwpmCalloutAdd(Globals.EngineHandle, &mCallout, NULL, NULL);
        PRINTK("FwpmCalloutAdd %08x", Status);

        if (NT_SUCCESS(Status)) {

            //Add filters
            filter.layerKey = *LayerKey;
            filter.displayData.name = L"SELinkFilter";
            filter.displayData.description = L"SELinkFilter";
            filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
            filter.action.calloutKey = *CalloutKey;
            filter.filterCondition = filterConditions;

            // TCP only
            filterConditions[ci].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
            filterConditions[ci].matchType = FWP_MATCH_EQUAL;
            filterConditions[ci].conditionValue.type = FWP_UINT8;
            filterConditions[ci].conditionValue.uint8 = IPPROTO_TCP;
            ci++;

            // Port range
            portRange.valueLow.type = FWP_UINT16;
            portRange.valueLow.uint16 = Globals.PortFirst;
            portRange.valueHigh.type = FWP_UINT16;
            portRange.valueHigh.uint16 = Globals.PortLast;
            filterConditions[ci].fieldKey = FWPM_CONDITION_IP_REMOTE_PORT;
            filterConditions[ci].matchType = FWP_MATCH_RANGE;
            filterConditions[ci].conditionValue.type = FWP_RANGE_TYPE;
            filterConditions[ci].conditionValue.rangeValue = &portRange;
            ci++;

            filter.numFilterConditions = ci;

            filter.subLayerKey = SUBLAYER_A_GUID;
            filter.weight.type = FWP_EMPTY;

            Status = FwpmFilterAdd(Globals.EngineHandle, &filter, NULL, NULL);
            PRINTK("FwpmFilterAdd %08x", Status);
        }
    }

    if (!NT_SUCCESS(Status)) {
        if (CalloutRegistered) {
            FwpsCalloutUnregisterById(*CalloutId);
        }
    }
    PRINTK("RegisterFlowEstablishedCallout %08x", Status);

    return Status;
}

VOID UnregisterCallouts()
{
    NTSTATUS Status;
    PRINTK("UnregisterCallouts (EngineHandle==NULL) -> %u",
        (UINT32)(Globals.EngineHandle==NULL));
    if (Globals.EngineHandle != NULL) {
        Status=FwpsCalloutUnregisterById(Globals.CalloutC4);
        PRINTK("FwpsCalloutUnregisterById %08x", Status);
        Status=FwpsCalloutUnregisterById(Globals.CalloutC6);
        PRINTK("FwpsCalloutUnregisterById %08x", Status);
        //Status=FwpmSubLayerDeleteByKey(Globals.EngineHandle, &SUBLAYER_A_GUID);
        //PRINTK("FwpmSubLayerDeleteByKey %08x", Status);
        Status=FwpmEngineClose(Globals.EngineHandle);
        PRINTK("FwpmEngineClose %08x", Status);
        Globals.EngineHandle = NULL;
    }
}

VOID EvtUnload(WDFDRIVER Driver)
{
    UNREFERENCED_PARAMETER(Driver);
    Globals.Stopping = TRUE;

    UnregisterCallouts();

    if (Globals.RedirectHandle != NULL) {
        FwpsRedirectHandleDestroy(Globals.RedirectHandle);
    }

    if (Globals.Timer != NULL) {
        WdfTimerStop(Globals.Timer, FALSE);
    }

    PRINTK("EvtUnload OK");
    return;
}

static BOOL StartPeriodicTask()
{
    WDF_TIMER_CONFIG TimerConfig = { 0 };
    WDF_OBJECT_ATTRIBUTES Attributes = { 0 };
    NTSTATUS Status;

    WDF_TIMER_CONFIG_INIT(&TimerConfig, EvtPeriodicTask);
    TimerConfig.Period = 0;
    TimerConfig.AutomaticSerialization = FALSE;
    TimerConfig.TolerableDelay = TolerableDelayUnlimited;
    TimerConfig.UseHighResolutionTimer = WdfFalse;

    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
    Attributes.Size = sizeof(WDF_OBJECT_ATTRIBUTES);
    Attributes.ExecutionLevel = WdfExecutionLevelPassive;
    Attributes.ParentObject = Globals.Device;
    // The timer event handler is run at passive level
    Status = WdfTimerCreate(&TimerConfig, &Attributes, &Globals.Timer);

    PRINTK("WdfTimerCreate %08x", (UINT32)Status);
    if (NT_SUCCESS(Status)) {
        Status = WdfTimerStart(Globals.Timer, -1);
		// negative time is relative to the current time
        PRINTK("WdfTimerStart %08x", (UINT32)Status);
        if (NT_SUCCESS(Status)) {
            return TRUE;
        }
    }
    return FALSE;
}

VOID EvtPeriodicTask(_In_ WDFTIMER Timer)
{
    //PRINTK("EvtPeriodicTask");
    UpdateServicePID();
    
    // Schedule next update in 5 seconds
    WdfTimerStart(Timer, -50000000);
}

static BOOL UpdateServicePID()
{
    NTSTATUS Status;
    FWPM_PROVIDER_CONTEXT* Context;
    DWORD pid = 0;
    BOOL ret = FALSE;
    
    Status = FwpmProviderContextGetByKey(
        Globals.EngineHandle, &FILTER_PROVIDER_CONTEXT_KEY, &Context);
    if (NT_SUCCESS(Status)) {
        if (Context->type == FWPM_GENERAL_CONTEXT) {
            if (Context->dataBuffer->size == 8) {
                pid = (DWORD)*((UINT64*)Context->dataBuffer->data);
                ret = TRUE;
            }
        }
        FwpmFreeMemory(&Context);
    }
    
    if (pid != Globals.RedirectTargetPID) {
        Globals.RedirectTargetPID = pid;
        PRINTK("Service PID: %u", (unsigned)pid);
    }
    return ret;
}

VOID NTAPI ClassifyC4(
    _In_ const FWPS_INCOMING_VALUES* InFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    _In_ PVOID LayerData,
    _In_ const VOID* ClassifyContext,
    _In_ const FWPS_FILTER* Filter,
    _In_ UINT64 InFlowContext,
    _Inout_ FWPS_CLASSIFY_OUT* ClassifyOut)
{
    UNREFERENCED_PARAMETER(LayerData);
    UNREFERENCED_PARAMETER(InFlowContext);

    Classify(InFixedValues, InMetaValues, ClassifyContext, Filter, ClassifyOut, FALSE);
}


VOID NTAPI ClassifyC6(
    _In_ const FWPS_INCOMING_VALUES* InFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    _In_ PVOID LayerData,
    _In_ const VOID* ClassifyContext,
    _In_ const FWPS_FILTER* Filter,
    _In_ UINT64 InFlowContext,
    _Inout_ FWPS_CLASSIFY_OUT* ClassifyOut)
{
    UNREFERENCED_PARAMETER(LayerData);
    UNREFERENCED_PARAMETER(InFlowContext);

    Classify(InFixedValues, InMetaValues, ClassifyContext, Filter, ClassifyOut, TRUE);
}


static VOID Classify(
    const FWPS_INCOMING_VALUES* InFixedValues,
    const FWPS_INCOMING_METADATA_VALUES* InMetaValues,
    const VOID* ClassifyContext,
    const FWPS_FILTER* Filter,
    FWPS_CLASSIFY_OUT* ClassifyOut,
    BOOL IsV6)
{
    UINT16 Port = 0;
    NL_ADDRESS_TYPE AddressType;
    UINT64 ClassifyHandle = 0;
    NTSTATUS Status = 1;
    FWPS_CONNECT_REQUEST* ConnectRequest = NULL;
    SOCKADDR_STORAGE* RedirectContext = NULL;
    FWPS_CONNECTION_REDIRECT_STATE RedirectState;
    UINT32 IpAddressV4 = 0;
    UINT64 ProcessId = 0;
    FWP_BYTE_ARRAY16* IpAddressV6 = NULL;
    BOOL IsLocal = FALSE;
    
    ClassifyOut->actionType = FWP_ACTION_PERMIT;

    // Read address type and port
    if (!IsV6) {
        AddressType = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V4_IP_DESTINATION_ADDRESS_TYPE].value.uint32;
        IpAddressV4 = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V4_IP_REMOTE_ADDRESS].value.uint32;
        if (IsLocalV4(IpAddressV4)) {
            IsLocal = TRUE;
        }
        Port = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V4_IP_REMOTE_PORT].value.uint16;
    }
    else {
        AddressType = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V6_IP_DESTINATION_ADDRESS_TYPE].value.uint32;
        IpAddressV6 = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V6_IP_REMOTE_ADDRESS].value.byteArray16;
        if (IsLocalV6(IpAddressV6)) {
            IsLocal = TRUE;
        }
        Port = InFixedValues->incomingValue[
            FWPS_FIELD_ALE_CONNECT_REDIRECT_V6_IP_REMOTE_PORT].value.uint16;
    }

    // Check whether the connection was already redirected by this driver
    RedirectState = FwpsQueryConnectionRedirectState(
        InMetaValues->redirectRecords, Globals.RedirectHandle, NULL);
    if (RedirectState == FWPS_CONNECTION_REDIRECTED_BY_SELF ||
        RedirectState == FWPS_CONNECTION_PREVIOUSLY_REDIRECTED_BY_SELF)
    {
        // PRINTK("[Classify] connection already redirected. Continue");
        return;
    }
    
    if (IsLocal) {
        // Bypass for localhost
        PRINTK("[Classify] address is localhost. CONTINUE");
        return;
    }
    else {
        if (!IsV6) {
            PRINT_ADDRESS_V4("Address", IpAddressV4);
        }
        else if (IpAddressV6 != NULL) {
            PRINT_ADDRESS_V6("Address", IpAddressV6);
        }
    }

    PRINTK("Port %u, PID %u", (UINT32)Port, (UINT32)InMetaValues->processId);

    Status = FwpsAcquireClassifyHandle((void*)ClassifyContext, 0, &ClassifyHandle);
    if (NT_SUCCESS(Status)) {
        // Start editing connection request fields
        Status = FwpsAcquireWritableLayerDataPointer(
            ClassifyHandle, Filter->filterId, 0, (PVOID*)&ConnectRequest, ClassifyOut);
        if (NT_SUCCESS(Status)) {
            // Allocate the redirect context. The kernel will take care of cleanup
            ConnectRequest->localRedirectContext = ExAllocatePoolWithTag(
                NonPagedPool, REDIRECT_CONTEXT_SIZE, FILTER_POOL_TAG);
            if (ConnectRequest->localRedirectContext == NULL) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
            }
            else {
                // Fill the redirect context
                ConnectRequest->localRedirectContextSize = REDIRECT_CONTEXT_SIZE;
                RedirectContext = (SOCKADDR_STORAGE*)ConnectRequest->localRedirectContext;
                RedirectContext[0] = ConnectRequest->localAddressAndPort;
                RedirectContext[1] = ConnectRequest->remoteAddressAndPort;
                RtlZeroMemory(&RedirectContext[2], sizeof(SOCKADDR_STORAGE));
                ProcessId = (UINT64)InMetaValues->processId;
                RtlCopyMemory(&RedirectContext[2], &ProcessId, sizeof(UINT64));

                // Redirect to localhost
                INETADDR_SETLOOPBACK((PSOCKADDR)&(ConnectRequest->remoteAddressAndPort));
                INETADDR_SET_PORT((PSOCKADDR)&(ConnectRequest->remoteAddressAndPort),
                    RtlUshortByteSwap(Globals.ServicePort));

                ConnectRequest->localRedirectHandle = Globals.RedirectHandle;
                ConnectRequest->localRedirectTargetPID = Globals.RedirectTargetPID;

                FwpsApplyModifiedLayerData(ClassifyHandle, (PVOID)ConnectRequest, 0);
                PRINTK("Redirected to pid %u", (unsigned)Globals.RedirectTargetPID);
            }
        }
    }
    if (ClassifyHandle != 0) {
        FwpsReleaseClassifyHandle(ClassifyHandle);
    }
    if (!NT_SUCCESS(Status)) {
        PRINTK("Error redirecting %08x", Status);
    }
}


NTSTATUS NotifyC(
    _In_ FWPS_CALLOUT_NOTIFY_TYPE NotifyType,
    _In_ const GUID* FilterKey,
    _In_ const FWPS_FILTER* Filter)
{
    UNREFERENCED_PARAMETER(NotifyType);
    UNREFERENCED_PARAMETER(FilterKey);
    UNREFERENCED_PARAMETER(Filter);
    return STATUS_SUCCESS;
}


static BOOL IsLocalV4(UINT32 IpAddress)
{
    return (IpAddress == 0x7F000001);
}

static BOOL IsLocalV6(FWP_BYTE_ARRAY16* IpAddress)
{
    SIZE_T i;
    UINT8* byteArray = IpAddress->byteArray16;
    for (i = 0; i < 15; i++) {
        if (byteArray[i] != 0) {
            return FALSE;
        }
    }
    return (byteArray[15] == 0x01);
}

