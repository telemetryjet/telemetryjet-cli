#include "environment.iss"

[Setup]
AppName=TelemetryJet CLI
AppVerName=TelemetryJet CLI {#ApplicationVersion}
VersionInfoVersion={#ApplicationVersion}
WizardStyle=modern
DefaultDirName={autopf}\TelemetryJet CLI
DefaultGroupName=TelemetryJet CLI
UninstallDisplayIcon={app}\jet.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
OutputBaseFilename=telemetryjet-cli-windows_x86-64_{#ApplicationVersion}
ArchitecturesInstallIn64BitMode=x64 arm64 ia64
ArchitecturesAllowed=x64 arm64 ia64
ChangesEnvironment=yes
SetupLogging=yes
PrivilegesRequired=admin

[Files]
Source: "jet.exe"; DestDir: "{app}\bin"
Source: "LICENSE.txt"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\TelemetryJet Documentation"; Filename: "https://docs.telemetryjet.com/cli/"
Name: "{group}\TelemetryJet Homepage"; Filename: "https://www.telemetryjet.com/"
Name: "{group}\Uninstall TelemetryJet CLI"; Filename: "{uninstallexe}"

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
    if CurStep = ssPostInstall
     then EnvAddPath(ExpandConstant('{app}') +'\bin');
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
    if CurUninstallStep = usPostUninstall
    then EnvRemovePath(ExpandConstant('{app}') +'\bin');
end;