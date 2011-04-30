
#include "StdAfx.h"
#include "UIDecoration.h"


const char* TitleShadowUI::GetClass() const
{
    return "TitleShadowUI";
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
    BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack1, clrBack2, true, 4);
}


const char* ListHeaderShadowUI::GetClass() const
{
    return "ListHeaderShadowUI";
}

SIZE ListHeaderShadowUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(0, 3);
}

void ListHeaderShadowUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
    COLORREF clrBack1, clrBack2;
    m_manager->GetThemeColorPair(UICOLOR_HEADER_BACKGROUND, clrBack1, clrBack2);
    RECT rcTop = { m_rcItem.left + 1, m_rcItem.top, m_rcItem.right - 1, m_rcItem.top + 4 };
    BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack2, clrBack1, true, 8);
}


const char* FadedLineUI::GetClass() const
{
    return "FadedLineUI";
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
    BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_NAVIGATOR_BORDER_NORMAL);
    RECT rc2 = { m_rcItem.right - 120, m_rcItem.top + 3, m_rcItem.right - 40, m_rcItem.top + 4 };
    BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rc2, clrLine, clrDialog, false, 16);
}


const char* SeparatorLineUI::GetClass() const
{
    return "SeparatorLineUI";
}

SIZE SeparatorLineUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(0, 12);
}

void SeparatorLineUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
    RECT rc1 = { m_rcItem.left, m_rcItem.top + 3, m_rcItem.right - 1, m_rcItem.top + 3 };
    BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_EDIT_TEXT_DISABLED);
    RECT rc2 = { m_rcItem.left, m_rcItem.top + 4, m_rcItem.right, m_rcItem.top + 4 };
    BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_EDIT_TEXT_DISABLED);
}

