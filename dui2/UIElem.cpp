#include "UIElem.h"

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
