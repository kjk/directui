
#include "StdAfx.h"
#include "UIDecoration.h"


const TCHAR* TitleShadowUI::GetClass() const
{
   return _T("TitleShadowUI");
}

SIZE TitleShadowUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 3);
}

void TitleShadowUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   COLORREF clrBack1 = m_manager->GetThemeColor(UICOLOR_TITLE_BACKGROUND);
   COLORREF clrBack2 = m_manager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
   RECT rcTop = { m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + 4 };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack1, clrBack2, true, 4);
}


const TCHAR* CListHeaderShadowUI::GetClass() const
{
   return _T("ListHeaderShadowUI");
}

SIZE CListHeaderShadowUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 3);
}

void CListHeaderShadowUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   COLORREF clrBack1, clrBack2;
   m_manager->GetThemeColorPair(UICOLOR_HEADER_BACKGROUND, clrBack1, clrBack2);
   RECT rcTop = { m_rcItem.left + 1, m_rcItem.top, m_rcItem.right - 1, m_rcItem.top + 4 };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack2, clrBack1, true, 8);
}


const TCHAR* FadedLineUI::GetClass() const
{
   return _T("FadedLineUI");
}

SIZE FadedLineUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 10);
}

void FadedLineUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   COLORREF clrLine = m_manager->GetThemeColor(UICOLOR_NAVIGATOR_BORDER_NORMAL);
   COLORREF clrDialog = m_manager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
   RECT rc1 = { m_rcItem.left, m_rcItem.top + 3, m_rcItem.right - 120, m_rcItem.top + 3 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_NAVIGATOR_BORDER_NORMAL);
   RECT rc2 = { m_rcItem.right - 120, m_rcItem.top + 3, m_rcItem.right - 40, m_rcItem.top + 4 };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rc2, clrLine, clrDialog, false, 16);
}


const TCHAR* SeparatorLineUI::GetClass() const
{
   return _T("SeparatorLineUI");
}

SIZE SeparatorLineUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 12);
}

void SeparatorLineUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   RECT rc1 = { m_rcItem.left, m_rcItem.top + 3, m_rcItem.right - 1, m_rcItem.top + 3 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_EDIT_TEXT_DISABLED);
   RECT rc2 = { m_rcItem.left, m_rcItem.top + 4, m_rcItem.right, m_rcItem.top + 4 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_EDIT_TEXT_DISABLED);
}

