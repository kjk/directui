/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef WinUtil_h
#define WinUtil_h

#include "BaseUtil.h"

using namespace Gdiplus;

class GdiPlusScope {
protected:
    GdiplusStartupInput si;
    ULONG_PTR           token;
public:
    GdiPlusScope() { GdiplusStartup(&token, &si, NULL); }
    ~GdiPlusScope() { GdiplusShutdown(token); }
};

class ComScope {
public:
    ComScope() { CoInitialize(NULL); }
    ~ComScope() { CoUninitialize(); }
};

inline void InitAllCommonControls()
{
    INITCOMMONCONTROLSEX cex = {0};
    cex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    cex.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES ;
    InitCommonControlsEx(&cex);
}

inline void FillWndClassEx(WNDCLASSEX &wcex, HINSTANCE hInstance) 
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

inline int RectDx(RECT& r)
{
    return r.right - r.left;
}

inline int RectDy(RECT& r)
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

#endif

