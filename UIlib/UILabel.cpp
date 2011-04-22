
#include "StdAfx.h"
#include "UILabel.h"

LabelPanelUI::LabelPanelUI() : m_cxWidth(0), m_uTextStyle(DT_VCENTER)
{
}

const TCHAR* LabelPanelUI::GetClass() const
{
   return _T("LabelPanelUI");
}

void LabelPanelUI::SetText(const TCHAR* txt)
{
   // Automatic assignment of keyboard shortcut
   if (_tcschr(txt, '&') != NULL)  m_chShortcut = *(_tcschr(txt, '&') + 1);
   ControlUI::SetText(txt);
}

void LabelPanelUI::SetWidth(int cx)
{
   m_cxWidth = cx;
   UpdateLayout();
}

void LabelPanelUI::SetTextStyle(UINT uStyle)
{
   m_uTextStyle = uStyle;
   Invalidate();
}

void LabelPanelUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("center")) != NULL)  m_uTextStyle |= DT_CENTER;
      if (_tcsstr(value, _T("right")) != NULL)  m_uTextStyle |= DT_RIGHT;
   }
   else if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttoi(value));
   else ControlUI::SetAttribute(name, value);
}

SIZE LabelPanelUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 4);
}

void LabelPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   RECT rcText = m_rcItem;
   int nLinks = 0;
   CBlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
}

const TCHAR* GreyTextHeaderUI::GetClass() const
{
   return _T("GreyTextHeaderUI");
}

SIZE GreyTextHeaderUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 12 + m_manager->GetThemeFontInfo(UIFONT_BOLD).tmHeight + 12);
}

void GreyTextHeaderUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   COLORREF clrDarkText = m_manager->GetThemeColor(UICOLOR_DIALOG_TEXT_DARK);
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 6, m_rcItem.right, m_rcItem.bottom - 5 };
   CBlueRenderEngineUI::DoFillRect(hDC, m_manager, rcLine, UICOLOR_DIALOG_TEXT_DARK);
   CBlueRenderEngineUI::DoPaintQuickText(hDC, m_manager, m_rcItem, m_txt, UICOLOR_DIALOG_TEXT_DARK, UIFONT_BOLD, DT_SINGLELINE);
}

