# BanglaSaver

A Windows screensaver manager that installs and registers [bsaver](https://github.com/abusayed0206/bsaver) - a Bangla screensaver for Windows 10/11.

This repository contains the **launcher UI + installer** for distributing BanglaSaver through the Microsoft Store and GitHub Releases.

> Looking for the screensaver itself? See [abusayed0206/bsaver](https://github.com/abusayed0206/bsaver).

---

## What It Does

BanglaSaver is a lightweight Win32 app that provides a simple interface to manage the bsaver screensaver:

| Button | Action |
|---|---|
| **Set as Screensaver** | Registers `bsaver.exe` in `HKCU\Control Panel\Desktop\SCRNSAVE.EXE` |
| **Remove Screensaver** | Deletes the registry entry and deactivates the screensaver |
| **Open Screensaver Settings** | Opens Windows screensaver settings (`desk.cpl`) |
| **Preview Screensaver** | Launches `bsaver.exe /s` for a full-screen preview |

**Key design decisions:**
- Per-user registry only (`HKCU`) - no admin required
- Dynamic path detection - survives updates/reinstalls
- Clean uninstall - registry entry removed automatically
- Store-compliant - no `HKLM`, no `System32`, no elevation

---

## Architecture

```
BanglaSaver/
├── BanglaSaver/              # Win32 launcher (C++, Visual Studio)
│   ├── BanglaSaver.cpp       # Main application - UI + registry logic
│   ├── BanglaSaver.vcxproj   # Visual Studio project
│   └── bsaver.exe            # Screensaver binary (from abusayed0206/bsaver)
│   
├── Installer/
│   └── BanglaSaverSetup.iss  # Inno Setup script - produces the EXE installer
├── .github/workflows/
│   ├── build.yml             # CI: builds on every push/PR
│   └── release.yml           # CD: builds installer + creates GitHub Release on tag
├── docs/
│   └── STORE_SUBMISSION.md   # Guide for publishing to Microsoft Store
└── BanglaSaver.slnx          # Visual Studio solution
```

---

## Download & Install

### From GitHub Releases

1. Go to [Releases](https://github.com/abusayed0206/BanglaSaver/releases)
2. Download `BanglaSaverSetup-x.x.x.exe`
3. Run the installer
4. Open **BanglaSaver** from the Start Menu
5. Click **Set as Screensaver**

### From Microsoft Store

*(Coming soon)*

### Silent Install (for automation / Store)

```
BanglaSaverSetup-1.0.0.exe /VERYSILENT /SUPPRESSMSGBOXES /NORESTART
```

### Silent Uninstall

```
"%LOCALAPPDATA%\Programs\BanglaSaver\unins000.exe" /VERYSILENT /SUPPRESSMSGBOXES
```

---

## Build From Source

### Prerequisites

| Tool | Version | Purpose |
|---|---|---|
| Visual Studio 2022+ | with "Desktop development with C++" workload | Build the launcher |
| Inno Setup 6 | [Download](https://jrsoftware.org/isdl.php) | Build the installer |
| Windows 10 SDK | 10.0.19041.0+ | Win32 API headers |

### Build the Launcher

```powershell
# Clone
git clone https://github.com/abusayed0206/BanglaSaver.git
cd BanglaSaver

# Build via MSBuild (Release x64)
msbuild BanglaSaver\BanglaSaver.vcxproj /p:Configuration=Release /p:Platform=x64

# Output:
#   BanglaSaver\x64\Release\BanglaSaver.exe   (launcher)
#   BanglaSaver\x64\Release\bsaver.exe        (screensaver, copied by build)
```

Or open `BanglaSaver.slnx` in Visual Studio ? Build ? Release x64.

### Build the Installer

```powershell
# Using Inno Setup CLI
iscc Installer\BanglaSaverSetup.iss

# Output:
#   Installer\Output\BanglaSaverSetup-1.0.0.exe
```

### Test Locally

```powershell
# Install silently
.\Installer\Output\BanglaSaverSetup-1.0.0.exe /VERYSILENT /SUPPRESSMSGBOXES

# Verify installed files
dir "$env:LOCALAPPDATA\Programs\BanglaSaver"

# Run the launcher
& "$env:LOCALAPPDATA\Programs\BanglaSaver\BanglaSaver.exe"

# Uninstall silently
& "$env:LOCALAPPDATA\Programs\BanglaSaver\unins000.exe" /VERYSILENT /SUPPRESSMSGBOXES
```

---

## CI/CD

### Build Workflow (`.github/workflows/build.yml`)

Runs on every push and pull request. Builds the launcher and installer, uploads the installer as an artifact.

### Release Workflow (`.github/workflows/release.yml`)

Triggered when you push a version tag:

```bash
git tag v1.0.0
git push origin v1.0.0
```

This will:
1. Build the launcher (Release x64)
2. Build the Inno Setup installer
3. Create a GitHub Release with the installer EXE attached
4. Generate a public download URL for Store submission

---

## Publishing to Microsoft Store

See [docs/STORE_SUBMISSION.md](docs/STORE_SUBMISSION.md) for the full step-by-step guide.

**Quick summary:**
1. Push a version tag ? GitHub Actions creates a Release
2. Copy the installer download URL from the Release
3. In Partner Center ? Packages ? paste the URL
4. Set installer params: `/VERYSILENT /SUPPRESSMSGBOXES /NORESTART`
5. Submit for certification

---

## Updating the Screensaver

To update `bsaver.exe`:

1. Build the latest version from [abusayed0206/bsaver](https://github.com/abusayed0206/bsaver)
2. Copy the new `bsaver.exe` into `BanglaSaver/bsaver.exe`
3. Bump the version in `Installer/BanglaSaverSetup.iss` (`MyAppVersion`)
4. Commit and tag:
   ```bash
   git add -A
   git commit -m "Update bsaver to vX.Y.Z"
   git tag vX.Y.Z
   git push origin master --tags
   ```
5. GitHub Actions will build and release automatically

---

## License

[MIT](LICENSE.txt)
