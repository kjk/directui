
#include "StdAfx.h"
#include "UIBlue.h"


const TCHAR* StatusbarUI::GetClass() const
{
   return _T("StatusbarUI");
}

SIZE StatusbarUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 10 + m_manager->GetThemeFontInfo(UIFONT_CAPTION).tmHeight);
}

void StatusbarUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   BlueRenderEngineUI::DoPaintPanel(hDC, m_manager, m_rcItem);
   RECT rcMessage = m_rcItem;
   ::InflateRect(&rcMessage, -8, -2);
   int nLinks = 0;
   BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcMessage, m_txt, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
}


ToolbarTitlePanelUI::ToolbarTitlePanelUI() : m_iPadding(6)
{
}

const TCHAR* ToolbarTitlePanelUI::GetClass() const
{
   return _T("ToolbarTitlePanelUI");
}

SIZE ToolbarTitlePanelUI::EstimateSize(SIZE szAvailable)
{
   SIZE sz = { 0 };
   RECT rcText = { 0, 0, szAvailable.cx, szAvailable.cy };
   ::InflateRect(&rcText, -m_iPadding, -m_iPadding);
   int nLinks = 0;
   BlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_WORDBREAK | DT_CALCRECT);
   sz.cy = (rcText.bottom - rcText.top) + (m_iPadding * 2);
   return sz;
}

void ToolbarTitlePanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   BlueRenderEngineUI::DoPaintPanel(hDC, m_manager, m_rcItem);
   RECT rcText = m_rcItem;
   ::InflateRect(&rcText, -m_iPadding, -m_iPadding);
   int nLinks = 0;
   BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcText, m_txt, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_WORDBREAK | DT_NOPREFIX);
}

ToolbarUI::ToolbarUI()
{
   SetInset(CSize(2, 2));
   SetPadding(2);
}

const TCHAR* ToolbarUI::GetClass() const
{
   return _T("ToolbarUI");
}

SIZE ToolbarUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 10 + m_manager->GetThemeFontInfo(UIFONT_CAPTION).tmHeight);
}

void ToolbarUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   BlueRenderEngineUI::DoPaintPanel(hDC, m_manager, m_rcItem);
   ContainerUI::DoPaint(hDC, rcPaint);
}

ToolButtonUI::ToolButtonUI()
{
}

const TCHAR* ToolButtonUI::GetClass() const
{
   return _T("ToolButtonUI");
}

void ToolButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   UINT uState = 0;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   BlueRenderEngineUI::DoPaintToolbarButton(hDC, m_manager, m_rcItem, m_txt, m_szPadding, m_uButtonState | uState);
}

const TCHAR* ToolSeparatorUI::GetClass() const
{
   return _T("ToolSeparatorUI");
}

SIZE ToolSeparatorUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(4, 0);
}

void ToolSeparatorUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   RECT rc1 = { m_rcItem.left + 1, m_rcItem.top + 2, m_rcItem.left + 1, m_rcItem.bottom - 3 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_TITLE_BORDER_DARK);
   RECT rc2 = { m_rcItem.left + 2, m_rcItem.top + 2, m_rcItem.left + 2, m_rcItem.bottom - 3 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc2, UICOLOR_TITLE_BORDER_LIGHT);
}

const TCHAR* ToolGripperUI::GetClass() const
{
   return _T("ToolGripperUI");
}

SIZE ToolGripperUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(12, 0);
}

void ToolGripperUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   RECT rcLine = { m_rcItem.left + 5, m_rcItem.top + 6, m_rcItem.left + 5 + 3, m_rcItem.top + 6 };
   for (int i = m_rcItem.top + 6; i <= m_rcItem.bottom - 6; i += 2)  {
      BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_TITLE_BORDER_DARK);
      ::OffsetRect(&rcLine, 0, 2);
   }
}

