
#include "StdAfx.h"
#include "UIBlue.h"

const char* StatusbarUI::GetClass() const
{
    return "StatusbarUI";
}

SIZE StatusbarUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(0, 10 + m_mgr->GetThemeFontInfo(UIFONT_CAPTION).tmHeight);
}

void StatusbarUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    BlueRenderEngineUI::DoPaintPanel(hDC, m_mgr, m_rcItem);
    RECT rcMessage = m_rcItem;
    ::InflateRect(&rcMessage, -8, -2);
    int nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcMessage, m_txt, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
}


ToolbarTitlePanelUI::ToolbarTitlePanelUI() : m_iPadding(6)
{
}

const char* ToolbarTitlePanelUI::GetClass() const
{
    return "ToolbarTitlePanelUI";
}

SIZE ToolbarTitlePanelUI::EstimateSize(SIZE szAvailable)
{
    SIZE sz = { 0 };
    RECT rcText = { 0, 0, szAvailable.cx, szAvailable.cy };
    ::InflateRect(&rcText, -m_iPadding, -m_iPadding);
    int nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(m_mgr->GetPaintDC(), m_mgr, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_WORDBREAK | DT_CALCRECT);
    sz.cy = RectDy(rcText) + (m_iPadding * 2);
    return sz;
}

void ToolbarTitlePanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    BlueRenderEngineUI::DoPaintPanel(hDC, m_mgr, m_rcItem);
    RECT rcText = m_rcItem;
    ::InflateRect(&rcText, -m_iPadding, -m_iPadding);
    int nLinks = 0;
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcText, m_txt, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_WORDBREAK | DT_NOPREFIX);
}

ToolbarUI::ToolbarUI()
{
    SetInset(CSize(2, 2));
    SetPadding(2);
}

const char* ToolbarUI::GetClass() const
{
    return "ToolbarUI";
}

SIZE ToolbarUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(0, 10 + m_mgr->GetThemeFontInfo(UIFONT_CAPTION).tmHeight);
}

void ToolbarUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    BlueRenderEngineUI::DoPaintPanel(hDC, m_mgr, m_rcItem);
    ContainerUI::DoPaint(hDC, rcPaint);
}

ToolButtonUI::ToolButtonUI()
{
}

const char* ToolButtonUI::GetClass() const
{
    return "ToolButtonUI";
}

void ToolButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    UINT uState = 0;
    if (IsFocused())  uState |= UISTATE_FOCUSED;
    if (!IsEnabled())  uState |= UISTATE_DISABLED;
    BlueRenderEngineUI::DoPaintToolbarButton(hDC, m_mgr, m_rcItem, m_txt, m_szPadding, m_uButtonState | uState);
}

const char* ToolSeparatorUI::GetClass() const
{
    return "ToolSeparatorUI";
}

SIZE ToolSeparatorUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(4, 0);
}

void ToolSeparatorUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rc1 = { m_rcItem.left + 1, m_rcItem.top + 2, m_rcItem.left + 1, m_rcItem.bottom - 3 };
    BlueRenderEngineUI::DoPaintLine(hDC, m_mgr, rc1, UICOLOR_TITLE_BORDER_DARK);
    RECT rc2 = { m_rcItem.left + 2, m_rcItem.top + 2, m_rcItem.left + 2, m_rcItem.bottom - 3 };
    BlueRenderEngineUI::DoPaintLine(hDC, m_mgr, rc2, UICOLOR_TITLE_BORDER_LIGHT);
}

const char* ToolGripperUI::GetClass() const
{
    return "ToolGripperUI";
}

SIZE ToolGripperUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(12, 0);
}

void ToolGripperUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcLine = { m_rcItem.left + 5, m_rcItem.top + 6, m_rcItem.left + 5 + 3, m_rcItem.top + 6 };
    for (int i = m_rcItem.top + 6; i <= m_rcItem.bottom - 6; i += 2)  {
        BlueRenderEngineUI::DoPaintLine(hDC, m_mgr, rcLine, UICOLOR_TITLE_BORDER_DARK);
        ::OffsetRect(&rcLine, 0, 2);
    }
}

