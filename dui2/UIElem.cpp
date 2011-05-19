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


void WinHwnd::OnPaint()
{
    MillisecondTimer t; t.Start();
    painter.PaintBegin(hwnd, Color::Blue);
    painter.PaintUIElem(uiRoot);
    painter.PaintEnd();
    double ms = t.GetCurrTimeInMs();
    str::d("OnPaint(): %.2f ms\n", ms);
}

void WinHwnd::OnSize(int dx, int dy)
{
    RECT r = {0, 0, dx, dy};
    InflateRect(&r, -20, -20);
    uiRoot->SetPosition(r);
}

void WinHwnd::OnSettingsChanged()
{
    ULONG scrollLines;
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
    // scrollLines usually equals 3 or 0 (for no scrolling)
    // WHEEL_DELTA equals 120, so deltaPerLine will be 40
    deltaPerLine = 0;
    if (scrollLines)
        deltaPerLine = WHEEL_DELTA / scrollLines;
}

void WinHwnd::OnFinalMessage(HWND hwnd)
{
    PostQuitMessage(0);
}

LRESULT CALLBACK WinHwnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WinHwnd* win = NULL;

    if (WM_ERASEBKGND == uMsg)
        return TRUE;

    if (WM_NCCREATE == uMsg) {
        CREATESTRUCT *lpcs = (CREATESTRUCT*)lParam;
        win = (WinHwnd*)lpcs->lpCreateParams;
        win->hwnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LPARAM)win);
        win->OnSettingsChanged();
        goto Exit;
    }

    win = (WinHwnd*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (!win)
        goto Exit;

    if (WM_NCDESTROY == uMsg) {
        LRESULT res = ::CallWindowProc(win->oldWndProc, hWnd, uMsg, wParam, lParam);
        ::SetWindowLongPtr(win->hwnd, GWLP_USERDATA, 0L);
        //if (win->m_subclassed)  win->Unsubclass();
        win->hwnd = NULL;
        win->OnFinalMessage(hWnd);
        return res;
    }

    if (WM_PAINT == uMsg) {
        win->OnPaint();
        return 0;
    }

    if (WM_SIZE == uMsg) {
        if (SIZE_MINIMIZED != wParam) {
            win->OnSize(LOWORD(lParam), HIWORD(lParam));
        }
        goto Exit;
    }

    if (WM_SETTINGCHANGE == uMsg) {
        win->OnSettingsChanged();
        return 0;
    }

Exit:
    if (win) {
        return win->HandleMessage(uMsg, wParam, lParam);
    } else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT WinHwnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::CallWindowProc(oldWndProc, hwnd, uMsg, wParam, lParam);
}

void WinHwnd::Show(bool show, bool takeFocus)
{
    ::ShowWindow(hwnd, show ? (takeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
    if (show)
        ::UpdateWindow(hwnd);
}

WinHwnd::~WinHwnd()
{
    delete uiRoot;
}

void WinHwnd::SetUIRoot(UIElem *root)
{
    delete uiRoot;
    uiRoot = root;
    uiRoot->SetHwndParent(hwnd);
}

static void RegisterWinHwndClass(HINSTANCE hinst)
{
    WNDCLASSA wc = { 0 };
    // TODO: don't set CS_*REDRAW and instead schedule repaint in WM_SIZE
    // to improve fluidity during resize?
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = WinHwnd::WndProc;
    wc.hInstance = hinst;
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WIN_HWND_CLS_NAME;
    // it's safe to register a class twice (it'll return an error
    // and GetLastError()will be ERROR_CLASS_ALREADY_EXISTS)
    ::RegisterClassA(&wc);
}

void WinHwnd::Create(UIElem *root, const char* name, DWORD dwStyle, DWORD dwExStyle, int x, int y, int dx, int dy, HMENU hMenu)
{
    RegisterWinHwndClass(hinst);
    hwnd = ::CreateWindowExUtf8(dwExStyle, WIN_HWND_CLS_NAME, name, dwStyle, x, y, dx, dy, NULL, hMenu, hinst, this);
    if (root)
        SetUIRoot(root);
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
