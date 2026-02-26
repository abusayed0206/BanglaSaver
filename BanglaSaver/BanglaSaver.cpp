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
HFONT hFontSubtitle;
HFONT hFontNormal;
HFONT hFontBtn;
HFONT hFontFooter;
HBRUSH hBrushBg;
HBRUSH hBrushBtnNormal;
HBRUSH hBrushBtnHover;
HPEN hPenSeparator;

// Bangladesh flag colors
static const COLORREF CLR_BG         = RGB(22, 27, 34);
static const COLORREF CLR_CARD       = RGB(30, 36, 44);
static const COLORREF CLR_TEXT       = RGB(220, 225, 232);
static const COLORREF CLR_TITLE      = RGB(0, 106, 78);     // flag green
static const COLORREF CLR_ACCENT     = RGB(244, 42, 65);    // flag red
static const COLORREF CLR_OK         = RGB(60, 179, 113);
static const COLORREF CLR_WARN       = RGB(230, 170, 50);
static const COLORREF CLR_NEUTRAL    = RGB(140, 148, 160);
static const COLORREF CLR_SEPARATOR  = RGB(55, 62, 72);
static const COLORREF CLR_FOOTER     = RGB(100, 110, 125);

static COLORREF statusColor = CLR_NEUTRAL;

// Forward declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

std::wstring GetScrPath()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathRemoveFileSpecW(path);
    return std::wstring(path) + L"\\bsaver.exe";
}

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

bool IsOurScreensaverSet()
{
    std::wstring current = GetCurrentScreensaver();
    std::wstring ours = GetScrPath();
    return _wcsicmp(current.c_str(), ours.c_str()) == 0;
}

void UpdateStatus(HWND hWnd)
{
    std::wstring scrPath = GetScrPath();
    bool scrExists = PathFileExistsW(scrPath.c_str()) == TRUE;

    if (!scrExists)
    {
        SetWindowTextW(hStatus, L"\u26A0  \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09AB\x09BE\x0987\x09B2 \x09AA\x09BE\x0993\x09AF\x09BC\x09BE \x09AF\x09BE\x09AF\x09BC\x09A8\x09BF");
        statusColor = CLR_WARN;
        EnableWindow(hBtnSet, FALSE);
        EnableWindow(hBtnPreview, FALSE);
    }
    else if (IsOurScreensaverSet())
    {
        SetWindowTextW(hStatus, L"\u2714  \x09AC\x09BE\x0982\x09B2\x09BE\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x0995\x09CD\x09B0\x09BF\x09AF\x09BC \x0986\x099B\x09C7");
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
            SetWindowTextW(hStatus, L"\u2022  \x0995\x09CB\x09A8\x09CB \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09C7\x099F \x0995\x09B0\x09BE \x09A8\x09C7\x0987");
            statusColor = CLR_NEUTRAL;
        }
        else
        {
            SetWindowTextW(hStatus, L"\u2022  \x0985\x09A8\x09CD\x09AF \x098F\x0995\x099F\x09BF \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x0995\x09CD\x09B0\x09BF\x09AF\x09BC \x0986\x099B\x09C7");
            statusColor = CLR_WARN;
        }
        EnableWindow(hBtnSet, TRUE);
        EnableWindow(hBtnRemove, FALSE);
        EnableWindow(hBtnPreview, TRUE);
    }
    InvalidateRect(hWnd, NULL, TRUE);
}

void SetScreensaver(HWND hWnd)
{
    std::wstring scrPath = GetScrPath();

    HKEY hKey;
    LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd,
            L"\x09B0\x09C7\x099C\x09BF\x09B8\x09CD\x099F\x09CD\x09B0\x09BF \x0996\x09CB\x09B2\x09BE \x09AF\x09BE\x09AF\x09BC\x09A8\x09BF\x0964",
            L"\x09B8\x09AE\x09B8\x09CD\x09AF\x09BE", MB_ICONERROR);
        return;
    }

    res = RegSetValueExW(hKey, L"SCRNSAVE.EXE", 0, REG_SZ,
        (BYTE*)scrPath.c_str(), (DWORD)((scrPath.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd,
            L"\x09B0\x09C7\x099C\x09BF\x09B8\x09CD\x099F\x09CD\x09B0\x09BF \x09B2\x09C7\x0996\x09BE \x09AF\x09BE\x09AF\x09BC\x09A8\x09BF\x0964",
            L"\x09B8\x09AE\x09B8\x09CD\x09AF\x09BE", MB_ICONERROR);
        return;
    }

    SystemParametersInfoW(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_SENDCHANGE);
    UpdateStatus(hWnd);
    MessageBoxW(hWnd,
        L"\x09AC\x09BE\x0982\x09B2\x09BE\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B9\x09BF\x09B8\x09C7\x09AC\x09C7 \x09B8\x09C7\x099F \x09B9\x09AF\x09BC\x09C7\x099B\x09C7!",
        L"\x09B8\x09AB\x09B2", MB_ICONINFORMATION);
}

void RemoveScreensaver(HWND hWnd)
{
    HKEY hKey;
    LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS)
    {
        MessageBoxW(hWnd,
            L"\x09B0\x09C7\x099C\x09BF\x09B8\x09CD\x099F\x09CD\x09B0\x09BF \x0996\x09CB\x09B2\x09BE \x09AF\x09BE\x09AF\x09BC\x09A8\x09BF\x0964",
            L"\x09B8\x09AE\x09B8\x09CD\x09AF\x09BE", MB_ICONERROR);
        return;
    }

    RegDeleteValueW(hKey, L"SCRNSAVE.EXE");
    RegCloseKey(hKey);

    SystemParametersInfoW(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDCHANGE);
    UpdateStatus(hWnd);
    MessageBoxW(hWnd,
        L"\x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09B0\x09BF\x09AF\x09BC\x09C7 \x09A6\x09C7\x0993\x09AF\x09BC\x09BE \x09B9\x09AF\x09BC\x09C7\x099B\x09C7\x0964",
        L"\x09B8\x09B0\x09BE\x09A8\x09CB \x09B9\x09AF\x09BC\x09C7\x099B\x09C7", MB_ICONINFORMATION);
}

void OpenScreensaverSettings()
{
    ShellExecuteW(NULL, L"open", L"control.exe", L"desk.cpl,,@screensaver", NULL, SW_SHOW);
}

void PreviewScreensaver()
{
    std::wstring scrPath = GetScrPath();
    ShellExecuteW(NULL, L"open", scrPath.c_str(), L"/s", NULL, SW_SHOW);
}

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
    if (hFontSubtitle) DeleteObject(hFontSubtitle);
    if (hFontNormal) DeleteObject(hFontNormal);
    if (hFontBtn) DeleteObject(hFontBtn);
    if (hFontFooter) DeleteObject(hFontFooter);
    if (hBrushBg) DeleteObject(hBrushBg);
    if (hBrushBtnNormal) DeleteObject(hBrushBtnNormal);
    if (hBrushBtnHover) DeleteObject(hBrushBtnHover);
    if (hPenSeparator) DeleteObject(hPenSeparator);

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_BANGLASAVER));
    wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground  = NULL;
    wcex.lpszClassName  = L"BanglaSaverLauncher";
    wcex.hIconSm        = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int wndW = 480, wndH = 520;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    // Window title in Bangla: ??????????
    HWND hWnd = CreateWindowExW(
        0, L"BanglaSaverLauncher",
        L"\x09AC\x09BE\x0982\x09B2\x09BE\x09B8\x09C7\x09AD\x09BE\x09B0",
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hBrushBg = CreateSolidBrush(CLR_BG);
        hBrushBtnNormal = CreateSolidBrush(CLR_CARD);
        hBrushBtnHover = CreateSolidBrush(RGB(42, 50, 60));
        hPenSeparator = CreatePen(PS_SOLID, 1, CLR_SEPARATOR);

        hFontTitle = CreateFontW(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontSubtitle = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontNormal = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontBtn = CreateFontW(19, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontFooter = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int clientW = clientRect.right;

        int btnW = 340;
        int btnH = 44;
        int leftMargin = (clientW - btnW) / 2;
        int y = 180;
        int gap = 14;

        // Status label — centered
        hStatus = CreateWindowExW(0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            leftMargin - 20, y, btnW + 40, 28, hWnd, (HMENU)IDC_STATUS, hInst, NULL);
        SendMessageW(hStatus, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
        y += 46;

        // ???????????? ??? ????
        hBtnSet = CreateWindowExW(0, L"BUTTON",
            L"\x25B6  \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09C7\x099F \x0995\x09B0\x09C1\x09A8",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_SET, hInst, NULL);
        SendMessageW(hBtnSet, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        // ???????????? ????
        hBtnRemove = CreateWindowExW(0, L"BUTTON",
            L"\x2716  \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09B0\x09BE\x09A8",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_REMOVE, hInst, NULL);
        SendMessageW(hBtnRemove, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        // ???????????? ??????
        hBtnSettings = CreateWindowExW(0, L"BUTTON",
            L"\x2699  \x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09B8\x09C7\x099F\x09BF\x0982\x09B8",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            leftMargin, y, btnW, btnH, hWnd, (HMENU)IDC_BTN_SETTINGS, hInst, NULL);
        SendMessageW(hBtnSettings, WM_SETFONT, (WPARAM)hFontBtn, TRUE);
        y += btnH + gap;

        // ??????? ?????
        hBtnPreview = CreateWindowExW(0, L"BUTTON",
            L"\x25B7  \x09AA\x09CD\x09B0\x09BF\x09AD\x09BF\x0989 \x09A6\x09C7\x0996\x09C1\x09A8",
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

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int clientW = clientRect.right;

        SetBkMode(hdc, TRANSPARENT);

        // Draw red accent circle (small decorative element)
        HBRUSH hRedBrush = CreateSolidBrush(CLR_ACCENT);
        int circleSize = 10;
        int circleY = 30;
        RECT rcCircle = { (clientW - circleSize) / 2, circleY,
                          (clientW + circleSize) / 2, circleY + circleSize };
        HRGN hCircleRgn = CreateEllipticRgnIndirect(&rcCircle);
        FillRgn(hdc, hCircleRgn, hRedBrush);
        DeleteObject(hCircleRgn);
        DeleteObject(hRedBrush);

        // Title: ?????????? — centered
        RECT rcTitle = { 0, 50, clientW, 100 };
        SetTextColor(hdc, CLR_TITLE);
        SelectObject(hdc, hFontTitle);
        DrawTextW(hdc,
            L"\x09AC\x09BE\x0982\x09B2\x09BE\x09B8\x09C7\x09AD\x09BE\x09B0",
            -1, &rcTitle, DT_CENTER | DT_SINGLELINE);

        // Subtitle: ???????????? ????????? — centered
        RECT rcSub = { 0, 100, clientW, 130 };
        SetTextColor(hdc, CLR_NEUTRAL);
        SelectObject(hdc, hFontSubtitle);
        DrawTextW(hdc,
            L"\x09B8\x09CD\x0995\x09CD\x09B0\x09BF\x09A8\x09B8\x09C7\x09AD\x09BE\x09B0 \x09AE\x09CD\x09AF\x09BE\x09A8\x09C7\x099C\x09BE\x09B0",
            -1, &rcSub, DT_CENTER | DT_SINGLELINE);

        // Separator line
        SelectObject(hdc, hPenSeparator);
        int sepMargin = 50;
        MoveToEx(hdc, sepMargin, 145, NULL);
        LineTo(hdc, clientW - sepMargin, 145);

        // Footer — centered
        RECT rcFooter = { 0, clientRect.bottom - 32, clientW, clientRect.bottom - 8 };
        SetTextColor(hdc, CLR_FOOTER);
        SelectObject(hdc, hFontFooter);
        DrawTextW(hdc,
            L"\x09AE\x09BE\x09A4\x09C3\x09AD\x09C2\x09AE\x09BF \x0985\x09A5\x09AC\x09BE \x09AE\x09C3\x09A4\x09CD\x09AF\x09C1",
            -1, &rcFooter, DT_CENTER | DT_SINGLELINE);

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
