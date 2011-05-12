/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef WinUtil_h
#define WinUtil_h

#include "BaseUtil.h"

using namespace Gdiplus;

class ScopedGdiPlus {
protected:
    Gdiplus::GdiplusStartupInput si;
    Gdiplus::GdiplusStartupOutput so;
    ULONG_PTR token, hookToken;

public:
    // suppress the GDI+ background thread when initiating in WinMain,
    // as that thread causes DDE messages to be sent too early and
    // thus unexpected timeouts
    ScopedGdiPlus() {
        si.SuppressBackgroundThread = true;
        Gdiplus::GdiplusStartup(&token, &si, &so);
        so.NotificationHook(&hookToken);
    }
    ~ScopedGdiPlus() {
        so.NotificationUnhook(hookToken);
        Gdiplus::GdiplusShutdown(token);
    }
};

class ScopedCom {
public:
    ScopedCom() { CoInitialize(NULL); }
    ~ScopedCom() { CoUninitialize(); }
};

inline void InitAllCommonControls()
{
    INITCOMMONCONTROLSEX cex = {0};
    cex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    cex.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES ;
    InitCommonControlsEx(&cex);
}

inline void FillWndClassEx(WNDCLASSEXW &wcex, HINSTANCE hInstance) 
{
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = NULL;
    wcex.lpszMenuName   = NULL;
    wcex.hIconSm        = 0;
}

inline int RectDx(const RECT& r)
{
    return r.right - r.left;
}

inline int RectDy(const RECT& r)
{
    return r.bottom - r.top;
}

inline int RectDx(RECT *r)
{
    return r->right - r->left;
}

inline int RectDy(RECT *r)
{
    return r->bottom - r->top;
}

inline void RectZero(RECT *r)
{
    r->left = r->right = r->top = r->bottom = 0;
}

inline void RectSet(RECT *r, int x, int y, int dx, int dy)
{
    r->left = x;
    r->right = x + dx;
    r->top = y;
    r->bottom = y + dy;
}

inline void RectSet(RECT& r, int x, int y, int dx, int dy)
{
    r.left = x;
    r.right = x + dx;
    r.top = y;
    r.bottom = y + dy;
}

inline void RectSetSize(RECT& r, int dx, int dy)
{
    r.left = r.top = 0;
    r.right = dx;
    r.bottom = dy;
}

inline void RectSetDx(RECT& r, int dx)
{
    r.right = r.left + dx;
}

inline void RectMove(RECT& r, int dx, int dy)
{
    r.left += dx; r.right += dx;
    r.top += dy; r.bottom += dy;
}

inline bool PointIn(const RECT&r, int x, int y)
{
    if (x < r.left || x > r.right)
        return false;
    if (y < r.top || y > r.bottom)
        return false;
    return true;
}


inline const PointF PointFFromRECT(const RECT& r)
{
    PointF ret;
    ret.X = (REAL)r.left;
    ret.Y = (REAL)r.top;
    return ret;
}

namespace win {

inline void ModifyStyleEx(HWND hwnd, LONG styleExSet)
{
    LONG old = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, old | styleExSet);
}

} // namespace win

#endif

