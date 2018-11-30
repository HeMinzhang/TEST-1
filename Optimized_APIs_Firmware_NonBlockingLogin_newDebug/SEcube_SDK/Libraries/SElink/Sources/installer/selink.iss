; SELink installation file

[Setup]
OutputBaseFilename=selink-setup
AppName=SELink
AppVersion=0.1-a1
AppPublisher=SElink
DefaultDirName={pf}\SElink
DefaultGroupName=SElink
UninstallDisplayIcon={app}\SElink.exe
Compression=lzma2
SolidCompression=yes
OutputDir=Output
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Files]
; GUI
Source: "..\proxy\x64\Release\SElink.exe"; DestDir: "{app}"; DestName: "SElink.exe"
Source: "..\proxy\x64\Release\SElink.exe.config"; DestDir: "{app}"; DestName: "SElink.exe.config"
; Service
Source: "..\proxy\x64\Release\selinksvc.exe"; DestDir: "{app}"; DestName: "selinksvc.exe"
Source: "..\proxy\x64\Release\selinkgw.exe"; DestDir: "{app}"; DestName: "selinkgw.exe"
Source: "..\proxy\example\selinksvc-port8888.json"; DestDir: "{app}"; DestName: "selinksvc.json"
Source: "..\proxy\example\selinksvc-soft.ini"; DestDir: "{app}"; DestName: "selinksvc.ini"
Source: "..\proxy\example\keys.json"; DestDir: "{app}"; DestName:"keys.json"
; Driver
Source: "..\filter\x64\Release\selinkflt\selinkflt.inf"; DestDir: "{app}\sys"; DestName: "selinkflt.inf"
Source: "..\filter\x64\Release\selinkflt\selinkflt.cat"; DestDir: "{app}\sys"; DestName: "selinkflt.cat"
Source: "..\filter\x64\Release\selinkflt\selinkflt.sys"; DestDir: "{app}\sys"; DestName: "selinkflt.sys"
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\SElink GUI"; Filename: "{app}\SElink.exe"

[Run]
; Service
Filename: "{sys}\sc.exe"; Parameters: "create selinksvc start= auto binPath= ""{app}\selinksvc.exe""" ; Flags: runhidden; StatusMsg: "Installing service..."
Filename: "{sys}\sc.exe"; Parameters: "start selinksvc"; Flags: runhidden; StatusMsg: "Starting service..."
; Driver
Filename: "{sys}\InfDefaultInstall.exe"; Parameters: """{app}\sys\selinkflt.inf"""; Flags: runhidden; StatusMsg: "Installing driver..."
Filename: "{sys}\net.exe"; Parameters: "start selinkflt"; Flags: runhidden; StatusMsg: "Starting driver..."

[UninstallDelete]
Type: files; Name: "{app}\selinksvc.log"

[UninstallRun]
; Driver
Filename: "{sys}\net.exe"; Parameters: "stop selinkflt"; Flags: runhidden; StatusMsg: "Stopping driver..."
Filename: "{sys}\rundll32.exe"; Parameters: "SETUPAPI.DLL,InstallHinfSection DefaultUninstall 132 {app}\sys\selinkflt.inf"; Flags: runhidden; StatusMsg: "Uninstalling driver..."
; Service
Filename: {sys}\sc.exe; Parameters: "stop selinksvc" ; Flags: runhidden; StatusMsg: "Stopping service..."
Filename: {sys}\sc.exe; Parameters: "delete selinksvc" ; Flags: runhidden; StatusMsg: "Uninstalling service..."
