#include "UIElem.h"
#include "WinUtf8.h"

namespace dui {

static Graphics *SetupGraphics(Graphics *g)
{
    g->SetPageUnit(UnitPixel);
    g->SetCompositingMode(CompositingModeSourceOver);
    g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g->SetSmoothingMode(SmoothingModeHighQuality);
    g->SetTextRenderingHint(TextRenderingHintAntiAlias);
    g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
    return g;
}

void UIPainter::PaintBegin(HWND hwnd, ARGB bgColor)
{
    this->hwnd = hwnd;
    BeginPaint(hwnd,&ps);
    RECT rc;
    GetClientRect(hwnd, &rc);

    if (!bmp ||
        (RectDx(rc) > (int)bmp->GetWidth()) ||
        (RectDy(rc) > (int)bmp->GetHeight()))
    {
        // TODO: use CachedBitmap() ?
        delete bmp;
        delete gfx;
        bmp = new Bitmap(RectDx(rc), RectDy(rc), PixelFormat32bppARGB);
        gfx = new Graphics(bmp);
        SetupGraphics(gfx);
    }

    if (bgColor != Color::Transparent) {
        gfx->FillRectangle(&SolidBrush(Color(bgColor)), RectFromRECT(rc));
    }
}

void UIPainter::PaintUIElem(UIElem* el)
{
    if (gfx && el && el->IsVisible()) {
        el->Draw(gfx);
    }
}

void UIPainter::PaintEnd()
{
    // TODO: clip child hwnd windows

    // TODO: only blit the part in ps.rcPaint
    Graphics g(ps.hdc);
    g.DrawImage(bmp, 0, 0);
    EndPaint(hwnd, &ps);
    hwnd = 0;
}

#define WIN_HWND_CLS_NAME "WinHwndCls"

static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void RegisterWinHwndClassName(HINSTANCE hinst)
{
    static bool registered = false;
    if (registered)
        return;

    ScopedMem<WCHAR> clsName(str::Utf8ToUni(WIN_HWND_CLS_NAME));

    WNDCLASSW wc = { 0 };
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = __WndProc;
    wc.hInstance = hinst;
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = clsName;
    ::RegisterClassW(&wc);
    registered = true;
}

void WinHwnd::Create(UIElem *root, const char* name, DWORD dwStyle, DWORD dwExStyle, int x, int y, int dx, int dy, HMENU hMenu)
{
    RegisterWinHwndClassName(hinst);
    hwnd = ::CreateWindowExUtf8(dwExStyle, WIN_HWND_CLS_NAME, name, dwStyle, x, y, dx, dy, NULL, hMenu, hinst, this);
}

void WinHwnd::Create(UIElem *root, const char* name, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    Create(root, name, dwStyle, dwExStyle, rc.left, rc.top, RectDx(rc), RectDy(rc), hMenu);
}

UIElem *UIElem::ChildFromPoint(int x, int y)
{
    if (!children)
        return NULL;

    for (size_t i=0; i<children->Count(); i++)
    {
        UIElem *el = children->At(i);
        if (!el->IsVisible())
            continue;
        if (PointIn(el->pos, x, y)) {
            UIElem *el2 = el->ChildFromPoint(el->pos.left + x, el->pos.top + y);
            if (el2)
                return el2;
            return el;
        }
    }
    return NULL;
}

void UIElem::DrawChildren(Graphics *g)
{
    if (!children)
        return;

    for (size_t i=0; i<children->Count(); i++) {
        UIElem *el = children->At(i);
        if (el->IsVisible()) {
            el->Draw(g);
        }
    }
}

void UIRectangle::Draw(Graphics *g)
{
    Rect r(RectFromRECT(pos));
    ARGB c = Color::MakeARGB(75,255,255,255);
    g->FillRectangle(&SolidBrush(c), r);
    DrawChildren(g);
}

static bool PreTranslateMessage(const MSG* pMsg)
{
#if 0
    // Pretranslate Message takes care of system-wide messages, such as
    // tabbing and shortcut key-combos. We'll look for all messages for
    // each window and any child control attached.
    HWND hwndParent = ::GetParent(pMsg->hwnd);
    UINT uStyle = GetWindowStyle(pMsg->hwnd);
    LRESULT lRes = 0;
    for (int i = 0; i < m_preMessages.GetSize(); i++)  {
        PaintManagerUI* pT = static_cast<PaintManagerUI*>(m_preMessages[i]);
        if (pMsg->hwnd == pT->GetPaintWindow()
            || (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0))) 
        {
            if (pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes))
                return true;
        }
    }
#endif
    return false;
}

void MessageLoop()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0))  {
        if (!PreTranslateMessage(&msg))  {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

}
