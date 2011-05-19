#include "resource.h"
#include "BaseUtil.h"
#include "WinUtil.h"
#include "WinUtf8.h"
#include "UIElem.h"

using namespace Gdiplus;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    InitCommonControls();
    ::CoInitialize(NULL);
    SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
    ScopedGdiPlus gdiPlus;

    dui::UIElem *uiRoot = new dui::UIRectangle();
    dui::WinHwnd *winMain = new dui::WinHwnd(hInst);
    winMain->Create(uiRoot, "hello", WS_OVERLAPPEDWINDOW, 0, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480);
    winMain->Show(true, true);
    dui::MessageLoop();
    delete winMain;
    ::CoUninitialize();
    return 0;
}
