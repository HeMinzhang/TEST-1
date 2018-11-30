#include "Globals.h"

FILTER_GLOBALS Globals;

void InitializeGlobals()
{
    RtlZeroMemory(&Globals, sizeof(FILTER_GLOBALS));
    Globals.PortFirst = 8000;
    Globals.PortLast = 8999;
    Globals.ServicePort = 10200;
    Globals.Stopping = FALSE;
}
