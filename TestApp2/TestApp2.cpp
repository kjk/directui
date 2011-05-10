#include "resource.h"
#include "BaseUtil.h"
#include "WinUtil.h"
#include "UIElem.h"

using namespace Gdiplus;

#define CLS_NAME L"TestApp2Frame"

static HINSTANCE ghinst;

static int  gDeltaPerLine = 0;         // for mouse wheel logic

static void OnSize(HWND hwnd, int dx, int dy)
{

}

static void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);

    Graphics g(hdc);
    Color white(255, 255, 255);
    Color red(255, 0, 0);

    Rect rc2(rc.left, rc.top, RectDx(rc), RectDy(rc));
    g.FillRectangle(&SolidBrush(white), rc2);
    rc2.Inflate(-10, -10);
    g.FillRectangle(&SolidBrush(red), rc2);
    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK WndProcFrame(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ULONG           ulScrollLines;                   // for mouse wheel logic

    switch (message)
    {
        case WM_CREATE:
            // do nothing
            goto InitMouseWheelInfo;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            if (SIZE_MINIMIZED != wParam) {
                int dx = LOWORD(lParam);
                int dy = HIWORD(lParam);
                OnSize(hwnd, dx, dy);
            }
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT:
            OnPaint(hwnd);
            break;

        case WM_SETTINGCHANGE:
InitMouseWheelInfo:
            SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
            // ulScrollLines usually equals 3 or 0 (for no scrolling)
            // WHEEL_DELTA equals 120, so iDeltaPerLine will be 40
            if (ulScrollLines)
                gDeltaPerLine = WHEEL_DELTA / ulScrollLines;
            else
                gDeltaPerLine = 0;
            return 0;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

static bool RegisterWinClass(HINSTANCE hInst)
{
    WNDCLASSEXW  wcex;
    FillWndClassEx(wcex, hInst);
    wcex.lpfnWndProc    = WndProcFrame;
    wcex.lpszClassName  = CLS_NAME;
   // wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUMATRAPDF));
    //wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    ATOM atom = RegisterClassExW(&wcex);
    if (!atom)
        return false;
    return true;
}

static bool InstanceInit(HINSTANCE hInst, int nCmdShow)
{
    ghinst = hInst;
    return true;
}

static HWND CreateMainWindow()
{
    HWND hwnd = CreateWindowW(
            CLS_NAME, L"TestApp",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
            NULL, NULL, ghinst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    return hwnd;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    InitCommonControls();

    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr))
        return 0;
    SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
    ScopedGdiPlus gdiPlus;

    if (!RegisterWinClass(hInst))
        goto Exit;
    if (!InstanceInit(hInst, nCmdShow))
        goto Exit;
    CreateMainWindow();
    dui::MessageLoop();

Exit:
    ::CoUninitialize();
    return 0;
}
