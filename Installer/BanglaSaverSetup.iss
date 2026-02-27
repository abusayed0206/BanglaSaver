; BanglaSaver Installer Script
; Inno Setup 6.x
; Produces a single EXE installer with /SILENT and /VERYSILENT support
; for Microsoft Store EXE submission.

#define MyAppName "BanglaSaver"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "BanglaSaver"
#define MyAppURL "https://github.com/abusayed0206/BanglaSaver"
#define MyAppExeName "BanglaSaver.exe"

[Setup]
AppId={{B4A3C7E8-9F21-4D6B-A582-1E3F0C8D7A9B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=..\Installer\Output
OutputBaseFilename=BanglaSaverSetup-{#MyAppVersion}
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
; Minimum Windows 10
MinVersion=10.0
UninstallDisplayIcon={app}\{#MyAppExeName}
; SetupIconFile=..\BanglaSaver\BanglaSaver.ico
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
SetupMutex=BanglaSaverSetupMutex

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main launcher
Source: "..\BanglaSaver\x64\Release\BanglaSaver.exe"; DestDir: "{app}"; Flags: ignoreversion
; Screensaver binary
Source: "..\BanglaSaver\x64\Release\bsaver.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallRun]
; Clean up registry on uninstall � remove screensaver entry if it points to our app
Filename: "reg.exe"; Parameters: "delete ""HKCU\Control Panel\Desktop"" /v SCRNSAVE.EXE /f"; Flags: runhidden; RunOnceId: "RemoveScrReg"

[Code]
// Exit codes for Microsoft Store EXE submission:
//   0 = Installation successful (default)
//   1 = Installation already in progress (SetupMutex)
//   2 = Installation cancelled by user (default)
//   4 = Disk space full (checked below)

function GetSpaceOnDisk(const DriveRoot: String; var FreeSpace, TotalSpace: Int64): Boolean;
  external 'GetDiskFreeSpaceExW@kernel32.dll stdcall';

function InitializeSetup(): Boolean;
var
  FreeBytes, TotalBytes: Int64;
  RequiredBytes: Int64;
  Drive: String;
begin
  Result := True;
  RequiredBytes := 10 * 1024 * 1024;
  Drive := ExtractFileDrive(ExpandConstant('{autopf}')) + '\';
  if GetSpaceOnDisk(Drive, FreeBytes, TotalBytes) then
  begin
    if FreeBytes < RequiredBytes then
    begin
      MsgBox('Disk space is full. Please free up space and try again.', mbError, MB_OK);
      Result := False;
      Exit;
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
  if CurUninstallStep = usPostUninstall then
  begin
    Exec('rundll32.exe', 'user32.dll,SystemParametersInfoW 17 0 0 3', '', SW_HIDE, ewNoWait, ResultCode);
  end;
end;
