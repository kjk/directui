
#include "StdAfx.h"
#include "UIButton.h"


ButtonUI::ButtonUI() : m_cxWidth(0), m_uButtonState(0), m_uTextStyle(DT_SINGLELINE | DT_CENTER)
{
   m_szPadding.cx = 4;
   m_szPadding.cy = 0;
}

const TCHAR* ButtonUI::GetClass() const
{
   return _T("ButtonUI");
}

UINT ButtonUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void ButtonUI::Event(TEventUI& event)
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
   ControlUI::Event(event);
}

void ButtonUI::SetText(const TCHAR* txt)
{
   ControlUI::SetText(txt);
   // Automatic assignment of keyboard shortcut
   if (_tcschr(txt, '&') != NULL)  m_chShortcut = *(_tcschr(txt, '&') + 1);
}

bool ButtonUI::Activate()
{
   if (!ControlUI::Activate())  return false;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("click"));
   return true;
}

void ButtonUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void ButtonUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttoi(value));
   else if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("center")) != NULL)  m_uTextStyle |= DT_CENTER;
      if (_tcsstr(value, _T("right")) != NULL)  m_uTextStyle |= DT_RIGHT;
   }
   else ControlUI::SetAttribute(name, value);
}

void ButtonUI::SetPadding(int cx, int cy)
{
   m_szPadding.cx = cx;
   m_szPadding.cy = cy;
   UpdateLayout();
}

SIZE ButtonUI::EstimateSize(SIZE /*szAvailable*/)
{
   SIZE sz = { m_cxWidth, 12 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
   if (m_cxWidth == 0 && m_manager != NULL)  {
      RECT rcText = { 0, 0, 9999, 20 };
      int nLinks = 0;
      BlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, m_txt, UICOLOR_STANDARD_BLACK, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT);
      sz.cx = rcText.right - rcText.left;
   }
   sz.cx += m_szPadding.cx * 2;
   sz.cy += m_szPadding.cy * 2;
   return sz;
}

void ButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = 0;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   RECT rcPadding = { m_szPadding.cx, m_szPadding.cy, m_szPadding.cx, m_szPadding.cy };
   BlueRenderEngineUI::DoPaintButton(hDC, m_manager, m_rcItem, m_txt, rcPadding, m_uButtonState | uState, m_uTextStyle);
}


OptionUI::OptionUI() : m_cxWidth(0), m_uButtonState(0), m_uStyle(DT_LEFT), m_bSelected(false)
{
}

const TCHAR* OptionUI::GetClass() const
{
   return _T("OptionUI");
}

UINT OptionUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void OptionUI::Event(TEventUI& event)
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
   ControlUI::Event(event);
}

bool OptionUI::IsChecked() const
{
   return m_bSelected;
}

void OptionUI::SetCheck(bool bSelected)
{
   if (m_bSelected == bSelected)  return;
   m_bSelected = bSelected;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("changed"));
   Invalidate();
}

bool OptionUI::Activate()
{
   if (!ControlUI::Activate())  return false;
   SetCheck(true);
   return true;
}

void OptionUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void OptionUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttoi(value));
   else if (_tcscmp(name, _T("selected")) == 0)  SetCheck(_tcscmp(value, _T("true")) == 0);
   else if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("right")) != NULL)  m_uStyle |= DT_RIGHT;
   }
   else ControlUI::SetAttribute(name, value);
}

SIZE OptionUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, 18 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void OptionUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = 0;
   if (m_bSelected)  uState |= UISTATE_CHECKED;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   BlueRenderEngineUI::DoPaintOptionBox(hDC, m_manager, m_rcItem, m_txt, m_uButtonState | uState, m_uStyle);
}

