
#include "StdAfx.h"
#include "UIButton.h"


CButtonUI::CButtonUI() : m_cxWidth(0), m_uButtonState(0), m_uTextStyle(DT_SINGLELINE | DT_CENTER)
{
   m_szPadding.cx = 4;
   m_szPadding.cy = 0;
}

const TCHAR* CButtonUI::GetClass() const
{
   return _T("ButtonUI");
}

UINT CButtonUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void CButtonUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) 
   {
      if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())  {
         m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
         Invalidate();
      }
   }
   if (event.Type == UIEVENT_MOUSEMOVE) 
   {
      if ((m_uButtonState & UISTATE_CAPTURED) != 0)  {
         if (::PtInRect(&m_rcItem, event.ptMouse))  m_uButtonState |= UISTATE_PUSHED;
         else m_uButtonState &= ~UISTATE_PUSHED;
         Invalidate();
      }
   }
   if (event.Type == UIEVENT_BUTTONUP) 
   {
      if ((m_uButtonState & UISTATE_CAPTURED) != 0)  {
         if (::PtInRect(&m_rcItem, event.ptMouse))  Activate();
         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
         Invalidate();
      }
   }
   if (event.Type == UIEVENT_MOUSEENTER)
   {
      m_uButtonState |= UISTATE_HOT;
      Invalidate();
   }
   if (event.Type == UIEVENT_MOUSELEAVE)
   {
      m_uButtonState &= ~UISTATE_HOT;
      Invalidate();
   }
   CControlUI::Event(event);
}

void CButtonUI::SetText(const TCHAR* txt)
{
   CControlUI::SetText(txt);
   // Automatic assignment of keyboard shortcut
   if (_tcschr(txt, '&') != NULL)  m_chShortcut = *(_tcschr(txt, '&') + 1);
}

bool CButtonUI::Activate()
{
   if (!CControlUI::Activate())  return false;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("click"));
   return true;
}

void CButtonUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void CButtonUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttoi(value));
   else if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("center")) != NULL)  m_uTextStyle |= DT_CENTER;
      if (_tcsstr(value, _T("right")) != NULL)  m_uTextStyle |= DT_RIGHT;
   }
   else CControlUI::SetAttribute(name, value);
}

void CButtonUI::SetPadding(int cx, int cy)
{
   m_szPadding.cx = cx;
   m_szPadding.cy = cy;
   UpdateLayout();
}

SIZE CButtonUI::EstimateSize(SIZE /*szAvailable*/)
{
   SIZE sz = { m_cxWidth, 12 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
   if (m_cxWidth == 0 && m_manager != NULL)  {
      RECT rcText = { 0, 0, 9999, 20 };
      int nLinks = 0;
      CBlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, m_sText, UICOLOR_STANDARD_BLACK, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT);
      sz.cx = rcText.right - rcText.left;
   }
   sz.cx += m_szPadding.cx * 2;
   sz.cy += m_szPadding.cy * 2;
   return sz;
}

void CButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = 0;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   RECT rcPadding = { m_szPadding.cx, m_szPadding.cy, m_szPadding.cx, m_szPadding.cy };
   CBlueRenderEngineUI::DoPaintButton(hDC, m_manager, m_rcItem, m_sText, rcPadding, m_uButtonState | uState, m_uTextStyle);
}


COptionUI::COptionUI() : m_cxWidth(0), m_uButtonState(0), m_uStyle(DT_LEFT), m_bSelected(false)
{
}

const TCHAR* COptionUI::GetClass() const
{
   return _T("OptionUI");
}

UINT COptionUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void COptionUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) 
   {
      if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())  {
         m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
         Invalidate();
      }
   }
   if (event.Type == UIEVENT_MOUSEMOVE) 
   {
      if ((m_uButtonState & UISTATE_CAPTURED) != 0)  {
         if (::PtInRect(&m_rcItem, event.ptMouse))  m_uButtonState |= UISTATE_PUSHED;
         else m_uButtonState &= ~UISTATE_PUSHED;
         Invalidate();
      }
   }
   if (event.Type == UIEVENT_BUTTONUP) 
   {
      if ((m_uButtonState & UISTATE_CAPTURED) != 0)  {
         if (::PtInRect(&m_rcItem, event.ptMouse))  Activate();
         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
         Invalidate();
      }
   }
   CControlUI::Event(event);
}

bool COptionUI::IsChecked() const
{
   return m_bSelected;
}

void COptionUI::SetCheck(bool bSelected)
{
   if (m_bSelected == bSelected)  return;
   m_bSelected = bSelected;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("changed"));
   Invalidate();
}

bool COptionUI::Activate()
{
   if (!CControlUI::Activate())  return false;
   SetCheck(true);
   return true;
}

void COptionUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void COptionUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttoi(value));
   else if (_tcscmp(name, _T("selected")) == 0)  SetCheck(_tcscmp(value, _T("true")) == 0);
   else if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("right")) != NULL)  m_uStyle |= DT_RIGHT;
   }
   else CControlUI::SetAttribute(name, value);
}

SIZE COptionUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, 18 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void COptionUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = 0;
   if (m_bSelected)  uState |= UISTATE_CHECKED;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   CBlueRenderEngineUI::DoPaintOptionBox(hDC, m_manager, m_rcItem, m_sText, m_uButtonState | uState, m_uStyle);
}

