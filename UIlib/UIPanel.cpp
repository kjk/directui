#include "StdAfx.h"
#include "UIPanel.h"

NavigatorPanelUI::NavigatorPanelUI() : m_curSel(-1)
{
}

const char* NavigatorPanelUI::GetClass() const
{
    return "NavigatorPanelUI";
}

void* NavigatorPanelUI::GetInterface(const char* name)
{
    if (str::Eq(name, "ListOwner")) return static_cast<IListOwnerUI*>(this);
    return VerticalLayoutUI::GetInterface(name);
}

int NavigatorPanelUI::GetCurSel() const
{
    return m_curSel;
}

void NavigatorPanelUI::Event(TEventUI& event)
{
    VerticalLayoutUI::Event(event);
}

bool NavigatorPanelUI::SelectItem(int idx)
{
    if (idx == m_curSel)  return true;
    if (m_curSel >= 0)  {
        ControlUI* ctrl = GetItem(m_curSel);
        IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
        if (listItem != NULL)  listItem->Select(false);
    }
    m_curSel = idx;
    if (m_curSel >= 0)  {
        ControlUI* ctrl = GetItem(m_curSel);
        IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
        if (listItem == NULL)  return false;
        listItem->Select(true);
        if (m_mgr != NULL)  m_mgr->SendNotify(ctrl, "itemclick");
    }
    if (m_mgr != NULL)  m_mgr->SendNotify(this, "itemselect");
    Invalidate();
    return true;
}

bool NavigatorPanelUI::Add(ControlUI* ctrl)
{
    IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
    if (listItem != NULL)  {
        listItem->SetOwner(this);
        listItem->SetIndex(m_items.Count());
    }
    return ContainerUI::Add(ctrl);
}

SIZE NavigatorPanelUI::EstimateSize(SIZE szAvailable)
{
    return CSize(0, 0);
}

void NavigatorPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    COLORREF clrFirst, clrSecond;
    m_mgr->GetThemeColorPair(UICOLOR_NAVIGATOR_BACKGROUND, clrFirst, clrSecond);
    BlueRenderEngineUI::DoPaintGradient(hDC, m_mgr, m_rcItem, clrFirst, clrSecond, false, 64);
    VerticalLayoutUI::DoPaint(hDC, rcPaint);
}


NavigatorButtonUI::NavigatorButtonUI() : m_uButtonState(0)
{
}

const char* NavigatorButtonUI::GetClass() const
{
    return "NavigatorButton";
}

void NavigatorButtonUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
    {
        RECT rcButton = GetButtonRect(m_rcItem);
        if (::PtInRect(&rcButton, event.ptMouse))  {
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        }
    }
    if (event.type == UIEVENT_MOUSEMOVE) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            RECT rcButton = GetButtonRect(m_rcItem);
            if (::PtInRect(&rcButton, event.ptMouse))  m_uButtonState |= UISTATE_PUSHED;
            else m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
    }
    if (event.type == UIEVENT_BUTTONUP) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            RECT rcButton = GetButtonRect(m_rcItem);
            if (::PtInRect(&rcButton, event.ptMouse))  {
                m_mgr->SendNotify(this, "link");
                Select();
            }
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
        }
    }
}

SIZE NavigatorButtonUI::EstimateSize(SIZE szAvailable)
{
    return CSize(0, 18 + m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void NavigatorButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    DrawItem(hDC, m_rcItem, 0);
}

void NavigatorButtonUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
{
    RECT rcButton = GetButtonRect(m_rcItem);

    if (FlSet(m_uButtonState, UISTATE_PUSHED))  {
        BlueRenderEngineUI::DoFillRect(hDC, m_mgr, rcButton, UICOLOR_NAVIGATOR_BUTTON_PUSHED);
    } else if (m_selected)  {
        BlueRenderEngineUI::DoFillRect(hDC, m_mgr, rcButton, UICOLOR_NAVIGATOR_BUTTON_SELECTED);
    } else if (FlSet(m_uButtonState, UISTATE_PUSHED))  {
        BlueRenderEngineUI::DoFillRect(hDC, m_mgr, rcButton, UICOLOR_NAVIGATOR_BUTTON_HOVER);
    }
    ::SelectObject(hDC, m_mgr->GetThemePen(m_selected ? UICOLOR_NAVIGATOR_BORDER_SELECTED : UICOLOR_NAVIGATOR_BORDER_NORMAL));
    POINT ptTemp = { 0 };
    ::MoveToEx(hDC, rcButton.left, rcButton.top, &ptTemp);
    ::LineTo(hDC, rcButton.left, rcButton.bottom);
    ::LineTo(hDC, rcButton.right, rcButton.bottom);

    UITYPE_COLOR iTextColor = UICOLOR_NAVIGATOR_TEXT_NORMAL;
    if (FlSet(m_uButtonState, UISTATE_PUSHED))  iTextColor = UICOLOR_NAVIGATOR_TEXT_PUSHED;
    else if (m_selected)  iTextColor = UICOLOR_NAVIGATOR_TEXT_SELECTED;

    RECT rcText = rcButton;
    ::OffsetRect(&rcText, 0, 1);
    ::InflateRect(&rcText, -8, 0);
    if (FlSet(m_uButtonState, UISTATE_PUSHED))  {
        rcText.left++; 
        rcText.top += 2;
    }
    int nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcText, m_txt, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
}

RECT NavigatorButtonUI::GetButtonRect(RECT rc) const
{
    int cy = m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 8;
    return CRect(rc.left + 10, rc.top, rc.right, rc.top + cy);
}


TaskPanelUI::TaskPanelUI() : m_hFadeBitmap(NULL)
{
    SetInset(CSize(8, 8));
    SetPadding(10);
    SetWidth(165);  // By default it gets a fixed 165 pixel width
}

TaskPanelUI::~TaskPanelUI()
{
    ::DeleteObject(m_hFadeBitmap);
    if (m_mgr != NULL)  m_mgr->KillTimer(this, FADE_TIMERID);
}

const char* TaskPanelUI::GetClass() const
{
    return "TaskPanelUI";
}

SIZE TaskPanelUI::EstimateSize(SIZE szAvailable)
{
    // The TaskPanel dissapears if the windows size becomes too small
    // Currently it is set to vasnish when its width gets below 1/4 of window.
    SIZE sz = m_cxyFixed;
    if (m_mgr->GetClientSize().cx <= m_cxyFixed.cx * 4)  {
        // Generate a bitmap so we can paint this panel as slowly fading out.
        // Only do this when the control's size suddenly go below the threshold.
        if (m_rcItem.right - m_rcItem.left > 1 && m_hFadeBitmap == NULL)  {
            ::DeleteObject(m_hFadeBitmap);
            m_hFadeBitmap = BlueRenderEngineUI::GenerateAlphaBitmap(m_mgr, this, m_rcItem, UICOLOR_DIALOG_BACKGROUND);
            // If we successfully created the 32bpp bitmap we'll set off the
            // timer so we can get animating...
            if (m_hFadeBitmap != NULL)  m_mgr->SetTimer(this, FADE_TIMERID, 50U);
            m_dwFadeTick = ::timeGetTime();
            m_rcFade = m_rcItem;
        }
        sz.cx = 1;
    }
    return sz;
}

void TaskPanelUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_TIMER && event.wParam == FADE_TIMERID) 
    {
        // The fading animation runs for 500ms. Then we kill
        // the bitmap which in turn disables the animation.
        if (event.timestamp - m_dwFadeTick > FADE_DELAY)  {
            m_mgr->KillTimer(this, FADE_TIMERID);
            ::DeleteObject(m_hFadeBitmap);
            m_hFadeBitmap = NULL;
        }
        m_mgr->Invalidate(m_rcFade);
        return;
    }
    VerticalLayoutUI::Event(event);
}

void TaskPanelUI::SetPos(RECT rc)
{
    int cyFont = m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight;
    RECT rcClient = { rc.left, rc.top + cyFont + 6, rc.right, rc.bottom };
    VerticalLayoutUI::SetPos(rcClient);
    m_rcItem = rc;
}

void TaskPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // Handling gracefull fading of panel
    if (m_hFadeBitmap != NULL)  {
        DWORD dwTimeDiff = ::timeGetTime() - m_dwFadeTick;
        TPostPaintUI job;
        job.rc = m_rcFade;
        job.hBitmap = m_hFadeBitmap;
        job.iAlpha = (BYTE) CLAMP(255 - (long)(dwTimeDiff / (FADE_DELAY / 255.0)), 0, 255);
        m_mgr->AddPostPaintBlit(job);
    }
    // A tiny panel (see explaination in EstimateSize()) is invisible
    if (RectDx(m_rcItem) < 2) return;
    // Paint caption
    int cyFont = m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight;
    RECT rcArc = { m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + cyFont + 6 };
    BlueRenderEngineUI::DoPaintArcCaption(hDC, m_mgr, rcArc, m_txt, UIARC_GRIPPER);
    // Paint background
    RECT rcClient = { m_rcItem.left, rcArc.bottom, m_rcItem.right, m_rcItem.bottom };
    COLORREF clrFirst, clrSecond;
    m_mgr->GetThemeColorPair(UICOLOR_TASK_BACKGROUND, clrFirst, clrSecond);
    BlueRenderEngineUI::DoPaintGradient(hDC, m_mgr, rcClient, clrFirst, clrSecond, false, 128);
    BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcClient, UICOLOR_TASK_CAPTION, UICOLOR_TASK_CAPTION, UICOLOR__INVALID, 0);
    // Paint elements
    VerticalLayoutUI::DoPaint(hDC, rcPaint);
}

SearchTitlePanelUI::SearchTitlePanelUI() : m_iconIdx(-1)
{
    SetInset(CSize(0, 0));
}

const char* SearchTitlePanelUI::GetClass() const
{
    return "SearchTitlePanelUI";
}

void SearchTitlePanelUI::SetImage(int idx)
{
    m_iconIdx = idx;
}

void SearchTitlePanelUI::SetPos(RECT rc)
{
    RECT rcClient = { rc.left + 1, rc.top + 35, rc.right - 1, rc.bottom - 1 };
    HorizontalLayoutUI::SetPos(rcClient);
    m_rcItem = rc;
}

void SearchTitlePanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcFrame = { m_rcItem.left, m_rcItem.top + 34, m_rcItem.right, m_rcItem.bottom };
    BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcFrame, UICOLOR_HEADER_SEPARATOR, UICOLOR_HEADER_SEPARATOR, UICOLOR_HEADER_BACKGROUND);
    RECT rcArc = { m_rcItem.left, m_rcItem.top + 14, m_rcItem.right, m_rcItem.top + 34 };
    RECT rcTemp = { 0 };
    if (::IntersectRect(&rcTemp, &rcPaint, &rcArc))  {
        BlueRenderEngineUI::DoPaintArcCaption(hDC, m_mgr, rcArc, "", 0);
    }
    if (m_iconIdx >= 0)  {
        HICON hIcon = m_mgr->GetThemeIcon(m_iconIdx, 50);
        ::DrawIconEx(hDC, m_rcItem.left + 5, m_rcItem.top, hIcon, 50, 50, 0, NULL, DI_NORMAL);
        ::DestroyIcon(hIcon);
    }
    HorizontalLayoutUI::DoPaint(hDC, rcPaint);
}

void SearchTitlePanelUI::SetAttribute(const char* name, const char* value)
{
    if (str::Eq(name, "image"))  SetImage(atoi(value));
    else HorizontalLayoutUI::SetAttribute(name, value);
}


PaddingPanelUI::PaddingPanelUI()
{
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
}

PaddingPanelUI::PaddingPanelUI(int cx, int cy)
{
    m_cxyFixed.cx = cx;
    m_cxyFixed.cy = cy;
}

void PaddingPanelUI::SetWidth(int cxWidth)
{
    m_cxyFixed.cx = cxWidth;
    UpdateLayout();
}

void PaddingPanelUI::SetHeight(int cyHeight)
{
    m_cxyFixed.cy = cyHeight;
    UpdateLayout();
}

void PaddingPanelUI::SetAttribute(const char* name, const char* value)
{
    int n;
    if (ParseWidth(name, value, n))
        SetWidth(n);
    else if (ParseHeight(name, value, n))
        SetHeight(n);
    else ControlUI::SetAttribute(name, value);
}

const char* PaddingPanelUI::GetClass() const
{
    return "PaddingPanel";
}

SIZE PaddingPanelUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void PaddingPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
}


ImagePanelUI::ImagePanelUI() : m_hBitmap(NULL)
{
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
}

ImagePanelUI::~ImagePanelUI()
{
    ::DeleteObject(m_hBitmap);
}

bool ImagePanelUI::SetImage(const char* pstrBitmap)
{
    ::DeleteObject(m_hBitmap);
    m_hBitmap = (HBITMAP) ::LoadImageA(m_mgr->GetResourceInstance(), pstrBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    ASSERT(m_hBitmap);
    if (m_hBitmap == NULL)  return false;
    BITMAP bm = { 0 };
    ::GetObject(m_hBitmap, sizeof(BITMAP), &bm);
    SetWidth(bm.bmWidth);
    SetHeight(bm.bmHeight);
    return true;
}

void ImagePanelUI::SetWidth(int cxWidth)
{
    m_cxyFixed.cx = cxWidth;
    UpdateLayout();
}

void ImagePanelUI::SetHeight(int cyHeight)
{
    m_cxyFixed.cy = cyHeight;
    UpdateLayout();
}

void ImagePanelUI::SetAttribute(const char* name, const char* value)
{
    int n;
    if (ParseWidth(name, value, n))
        SetWidth(n);
    else if (ParseHeight(name, value, n))
        SetHeight(n);
    else if (str::Eq(name, "image"))  SetImage(value);
    else ControlUI::SetAttribute(name, value);
}

const char* ImagePanelUI::GetClass() const
{
    return "ImagePanel";
}

SIZE ImagePanelUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void ImagePanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    BlueRenderEngineUI::DoPaintBitmap(hDC, m_mgr, m_hBitmap, m_rcItem);
}


TextPanelUI::TextPanelUI() : m_nLinks(0), m_uButtonState(0), m_textColor(UICOLOR_EDIT_TEXT_NORMAL), m_backColor(UICOLOR__INVALID)
{
    m_uTextStyle = DT_WORDBREAK;
    ::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
}

const char* TextPanelUI::GetClass() const
{
    return "TextPanelUI";
}

bool TextPanelUI::Activate()
{
    if (!LabelPanelUI::Activate())  return false;
    if (m_nLinks > 0)  m_mgr->SendNotify(this, "link");
    return true;
}

void TextPanelUI::SetTextColor(UITYPE_COLOR textColor)
{
    m_textColor = textColor;
    Invalidate();
}

void TextPanelUI::SetBkColor(UITYPE_COLOR backColor)
{
    m_backColor = backColor;
    Invalidate();
}

UINT TextPanelUI::GetControlFlags() const
{
    return m_nLinks > 0 ? UIFLAG_SETCURSOR : 0;
}

void TextPanelUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_SETCURSOR)  {
        for (int i = 0; i < m_nLinks; i++)  {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
                return;
            }
        }      
    }
    if (event.type == UIEVENT_BUTTONDOWN && IsEnabled())  {
        for (int i = 0; i < m_nLinks; i++)  {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
                m_uButtonState |= UISTATE_PUSHED;
                Invalidate();
                return;
            }
        }      
    }
    if (event.type == UIEVENT_BUTTONUP)  {
        for (int i = 0; i < m_nLinks; i++)  {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse))  Activate();
        }      
        m_uButtonState &= ~UISTATE_PUSHED;
        Invalidate();
        return;
    }
    // When you move over a link
    if (m_nLinks > 0 && event.type == UIEVENT_MOUSEMOVE)  Invalidate();
    if (m_nLinks > 0 && event.type == UIEVENT_MOUSELEAVE)  Invalidate();
    LabelPanelUI::Event(event);
}

void TextPanelUI::SetAttribute(const char* name, const char* value)
{
    if (str::Eq(name, "textColor"))  SetTextColor((UITYPE_COLOR)atoi(value));
    else if (str::Eq(name, "backColor"))  SetBkColor((UITYPE_COLOR)atoi(value));
    else LabelPanelUI::SetAttribute(name, value);
}

SIZE TextPanelUI::EstimateSize(SIZE szAvailable)
{
    RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxWidth), 9999 };
    m_nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(m_mgr->GetPaintDC(), m_mgr, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, NULL, m_nLinks, DT_CALCRECT | m_uTextStyle);
    return CSize(RectDx(rcText), RectDy(rcText));
}

void TextPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcText = m_rcItem;
    m_nLinks = dimof(m_rcLinks);
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcText, m_txt, m_textColor, m_backColor, m_rcLinks, m_nLinks, m_uTextStyle);
}

WarningPanelUI::WarningPanelUI() : m_backColor(UICOLOR_STANDARD_YELLOW)
{
}

const char* WarningPanelUI::GetClass() const
{
    return "WarningPanelUI";
}

void WarningPanelUI::SetWarningType(UINT uType)
{
    switch (uType)  {
    case MB_ICONERROR:
        m_backColor = UICOLOR_STANDARD_RED;
        break;
    case MB_ICONWARNING:
        m_backColor = UICOLOR_STANDARD_YELLOW;
        break;
    default:
        m_backColor = UICOLOR_WINDOW_BACKGROUND;
        break;
    }
}

void WarningPanelUI::SetAttribute(const char* name, const char* value)
{
    if (str::Eq(name, "type"))  {
        if (str::Eq(value, "error"))  SetWarningType(MB_ICONERROR);
        if (str::Eq(value, "warning"))  SetWarningType(MB_ICONWARNING);
    }
    else TextPanelUI::SetAttribute(name, value);
}

SIZE WarningPanelUI::EstimateSize(SIZE szAvailable)
{
    RECT rcText = { 0, 0, szAvailable.cx, szAvailable.cy };
    ::InflateRect(&rcText, -6, -4);
    int nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(m_mgr->GetPaintDC(), m_mgr, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_WORDBREAK | DT_CALCRECT);
    return CSize(0, RectDy(rcText) + 16);
}

void WarningPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcSign = m_rcItem;
    rcSign.bottom -= 8;
    BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcSign, UICOLOR_STANDARD_GREY, UICOLOR_STANDARD_GREY, m_backColor);
    RECT rcText = rcSign;
    ::InflateRect(&rcText, -6, -4);
    m_nLinks = dimof(m_rcLinks);
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcText, m_txt, UICOLOR_BUTTON_TEXT_NORMAL, UICOLOR__INVALID, m_rcLinks, m_nLinks, DT_WORDBREAK);
}
