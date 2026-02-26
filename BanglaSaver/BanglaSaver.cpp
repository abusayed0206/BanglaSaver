// BanglaSaver.cpp : Screensaver Launcher / Manager
//

#include "framework.h"
#include "BanglaSaver.h"

// Global Variables
HINSTANCE hInst;
HWND hStatus;
HWND hBtnSet;
HWND hBtnRemove;
HWND hBtnSettings;
HWND hBtnPreview;
HFONT hFontTitle;
HFONT hFontNormal;
HFONT hFontBtn;
HBRUSH hBrushBg;

static const COLORREF CLR_BG       = RGB(32, 32, 38);
static const COLORREF CLR_TEXT     = RGB(220, 220, 230);
static const COLORREF CLR_TITLE    = RGB(100, 180, 255);
static const COLORREF CLR_OK       = RGB(60, 179, 113);
static const COLORREF CLR_WARN     = RGB(200, 160, 60);
static const COLORREF CLR_NEUTRAL  = RGB(160, 160, 170);

static COLORREF statusColor = CLR_NEUTRAL;

// Forward declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// ?? Helper: get path to bsaver.exe next to this launcher ??
std::wstring GetScrPath()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathRemoveFileSpecW(path);
    return std::wstring(path) + L"\\bsaver.exe";
}

// ?? Helper: read current SCRNSAVE.EXE from registry ??
std::wstring GetCurrentScreensaver()
{
    HKEY hKey;
    wchar_t value[MAX_PATH] = {};
    DWORD size = sizeof(value);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueExW(hKey, L"SCRNSAVE.EXE", NULL, NULL, (BYTE*)value, &size);
        RegCloseKey(hKey);
    }
    return std::wstring(value);
}

// ?? Helper: check if our screensaver is the active one ??
bool IsOurScreensaverSet()
{
    std::wstring current = GetCurrentScreensaver();
    std::wstring ours = GetScrPath();
    return _wcsicmp(current.c_str(), ours.c_str()) == 0;
}

// ?? Update status label and button states ??
void UpdateStatus(HWND hWnd)
{
    std::wstring scrPath = GetScrPath();
    bool scrExists = PathFileExistsW(scrPath.c_str()) == TRUE;

    if (!scrExists)
    {
        SetWindowTextW(hStatus, L"  \u26A0  bsaver.exe not found next to launcher");
        statusColor = CLR_WARN;
        EnableWindow(hBtnSet, FALSE);
        EnableWindow(hBtnPreview, FALSE);
    }
    else if (IsOurScreensaverSet())
    {
        SetWindowTextW(hStatus, L"  \u2714  BanglaSaver is set as your screensaver");
        statusColor = CLR_OK;
        EnableWindow(hBtnSet, FALSE);
        EnableWindow(hBtnRemove, TRUE);
        EnableWindow(hBtnPreview, TRUE);
    }
    else
    {
        std::wstring current = GetCurrentScreensaver();
        if (current.empty())
        {
            SetWindowTextW(hStatus, L"  \u2022  No screensaver is currently set");
            statusColor = CLR_NEUTRAL;
        }
        else
        {
            SetWindowTextW(hStatus, L"  \u2022  Another screensaver is currently active");
            statusColor = CLR_WARN;
        }
        EnableWindow(hBtnSet, TRUE);
        EnableWindow(hBtnRemove, FALSE);
        EnableWindow(hBtnPreview, TRUE);
    }
    InvalidateRect(hWnd, NULL, TRUE);
}

// ?? Set our screensaver in the registry ??
void SetScreensaver(HWND hWnd)
{
    std::wstring scrPath = GetScrPath();

    HKEY hKey;
    LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd, L"Failed to open registry key.", L"Error", MB_ICONERROR);
        return;
    }

    res = RegSetValueExW(hKey, L"SCRNSAVE.EXE", 0, REG_SZ,
        (BYTE*)scrPath.c_str(), (DWORD)((scrPath.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd, L"Failed to write registry value.", L"Error", MB_ICONERROR);
        return;
    }

    SystemParametersInfoW(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_SENDCHANGE);
    UpdateStatus(hWnd);
    MessageBoxW(hWnd, L"BanglaSaver has been set as your screensaver!", L"Success", MB_ICONINFORMATION);
}

// ?? Remove our screensaver from the registry ??
void RemoveScreensaver(HWND hWnd)
{
    HKEY hKey;
    LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd, L"Failed to open registry key.", L"Error", MB_ICONERROR);
        return;
    }

    RegDeleteValueW(hKey, L"SCRNSAVE.EXE");
    RegCloseKey(hKey);

    SystemParametersInfoW(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
    UpdateStatus(hWnd);
    MessageBoxW(hWnd, L"Screensaver has been removed.", L"Removed", MB_ICONINFORMATION);
}

// ?? Open Windows screensaver settings ??
void OpenScreensaverSettings()
{
    ShellExecuteW(NULL, L"open", L"control.exe", L"desk.cpl,,@screensaver", NULL, SW_SHOW);
}

// ?? Preview the screensaver ??
void PreviewScreensaver()
{
    std::wstring scrPath = GetScrPath();
    ShellExecuteW(NULL, L"open", scrPath.c_str(), L"/s", NULL, SW_SHOW);
}

// ?? Entry point ??
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (hFontTitle) DeleteObject(hFontTitle);
    if (hFontNormal) DeleteObject(hFontNormal);
    if (hFontBtn) DeleteObject(hFontBtn);
    if (hBrushBg) DeleteObject(hBrushBg);

    return (int)msg.wParam;
}

// ?? Register window class ??
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_BANGLASAVER));
    wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground  = NULL; // we paint manually
    wcex.lpszClassName  = L"BanglaSaverLauncher";
    wcex.hIconSm        = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

// ?? Create & show main window ??
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int wndW = 440, wndH = 380;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = CreateWindowExW(
        0, L"BanglaSaverLauncher",
        L"BanglaSaver",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (screenW - wndW) / 2, (screenH - wndH) / 2,
        wndW, wndH,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

// ?? Window procedure ??
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hBrushBg = CreateSolidBrush(CLR_BG);

        hFontTitle = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontNormal = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontBtn = CreateFontW(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

        int leftMargin = 30;
        int btnW = 370;
        int btnH = 40;
        int y = 100;
        int gap = 12;

        // Status label
        hStatus = CreateWindowExW(0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            leftMargin, y, btnW, 24, hWnd, (HMENU)IDC_STATUS, hInst, NULL);
        SendMessageW(hStatus, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
        y += 40;

        // Buttons
        hBtnSet = CreateWindowExW(0, L"BUTTON", L"\u25B6  Set as Screensaver",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_SET, hInst, NULL);
        SendMessageW(hBtnSet, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        hBtnRemove = CreateWindowExW(0, L"BUTTON", L"\u2716  Remove Screensaver",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_REMOVE, hInst, NULL);
        SendMessageW(hBtnRemove, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        hBtnSettings = CreateWindowExW(0, L"BUTTON", L"\u2699  Open Screensaver Settings",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_SETTINGS, hInst, NULL);
        SendMessageW(hBtnSettings, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        hBtnPreview = CreateWindowExW(0, L"BUTTON", L"\u25B7  Preview Screensaver",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_PREVIEW, hInst, NULL);
        SendMessageW(hBtnPreview, WM_SETFONT, (WPARAM)hFontBtn, TRUE);

        UpdateStatus(hWnd);
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BTN_SET:
            SetScreensaver(hWnd);
            break;
        case IDC_BTN_REMOVE:
            RemoveScreensaver(hWnd);
            break;
        case IDC_BTN_SETTINGS:
            OpenScreensaverSettings();
            break;
        case IDC_BTN_PREVIEW:
            PreviewScreensaver();
            break;
        }
        break;
    }

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, hBrushBg);
        return 1;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        SetBkMode(hdc, TRANSPARENT);

        // Draw title
        RECT rcTitle = { 30, 20, 410, 60 };
        SetTextColor(hdc, CLR_TITLE);
        SelectObject(hdc, hFontTitle);
        DrawTextW(hdc, L"BanglaSaver", -1, &rcTitle, DT_LEFT | DT_SINGLELINE);

        // Draw subtitle
        RECT rcSub = { 32, 55, 410, 80 };
        SetTextColor(hdc, CLR_TEXT);
        SelectObject(hdc, hFontNormal);
        DrawTextW(hdc, L"Screensaver Manager", -1, &rcSub, DT_LEFT | DT_SINGLELINE);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;
        SetBkMode(hdcStatic, TRANSPARENT);
        if (hCtrl == hStatus)
            SetTextColor(hdcStatic, statusColor);
        else
            SetTextColor(hdcStatic, CLR_TEXT);
        return (LRESULT)hBrushBg;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}
