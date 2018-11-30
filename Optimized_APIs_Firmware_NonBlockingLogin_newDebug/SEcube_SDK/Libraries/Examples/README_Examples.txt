SEcube(tm) Open Source SDK

"Examples" Folder:
    Contains the source code of simple applications that exploit the libraries provided


Folders:
Add_Keys
    Contains the source code showing how to add key to a SEcube(tm) device

Device_Initialisation
    Contains the source code showing how a SEcube(tm) device should be initialised

Encrypt_Data
    Contains the source code showing how a SEcube(tm) device can encrypt data

FunctionalTest
    Contains the source code showing how to use basic functionalities of a SEcube(tm) device

secube-host
    Contains the source code of the host-side SEcube(tm) library

TestFPGA
    Contains the source code for an example of LED Blink software using the FPGA


Note:
1) All the examples here (a part from TestFPGA), requires to be linked with the SEcube(tm) host library which is in the folder "secube-host". If you move a project or compile in a different location, you should correct the <<#include>> statements.
2) After initialisation of the device the PINs are set to zero. The initialisation of the device can be only performed once. Any further attempt of initialisation will fail. In order to reinitialise the device, its flash must be erased.
