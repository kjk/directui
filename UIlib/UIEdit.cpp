
#include "StdAfx.h"
#include "UIEdit.h"


class CSingleLineEditWnd : public CWindowWnd
{
public:
   CSingleLineEditWnd();

   void Init(CSingleLineEditUI* owner);

   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetSuperClassName() const;
   void OnFinalMessage(HWND hWnd);

   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   CSingleLineEditUI* m_owner;
};


CSingleLineEditWnd::CSingleLineEditWnd() : m_owner(NULL)
{
}

void CSingleLineEditWnd::Init(CSingleLineEditUI* owner)
{
   CRect rcPos = owner->GetPos();
   rcPos.Deflate(1, 3);
   Create(owner->GetManager()->GetPaintWindow(), NULL, WS_CHILD | owner->m_uEditStyle, 0, rcPos);
   SetWindowFont(m_hWnd, owner->GetManager()->GetThemeFont(UIFONT_NORMAL), TRUE);
   Edit_SetText(m_hWnd, owner->GetText());
   Edit_SetModify(m_hWnd, FALSE);
   SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
   Edit_SetSel(m_hWnd, 0, -1);
   Edit_Enable(m_hWnd, owner->IsEnabled() == true);
   Edit_SetReadOnly(m_hWnd, owner->IsReadOnly() == true);
   ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
   ::SetFocus(m_hWnd);
   m_owner = owner;
}

const TCHAR* CSingleLineEditWnd::GetWindowClassName() const
{
   return _T("SingleLineEditWnd");
}

const TCHAR* CSingleLineEditWnd::GetSuperClassName() const
{
   return WC_EDIT;
}

void CSingleLineEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
   // Clear reference and die
   m_owner->m_win = NULL;
   delete this;
}

LRESULT CSingleLineEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lRes = 0;
   BOOL bHandled = TRUE;
   if (uMsg == WM_KILLFOCUS)  lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
   else if (uMsg == OCM_COMMAND && GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)  lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
   else bHandled = FALSE;
   if (!bHandled)  return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
   return lRes;
}

LRESULT CSingleLineEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
   PostMessage(WM_CLOSE);
   return lRes;
}

LRESULT CSingleLineEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (m_owner == NULL)  return 0;
   // Copy text back
   int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
   TCHAR* pstr = static_cast<TCHAR*>(_alloca(cchLen * sizeof(TCHAR)));
   ASSERT(pstr);
   if (pstr == NULL)  return 0;
   ::GetWindowText(m_hWnd, pstr, cchLen);
   m_owner->SetText(pstr);
   return 0;
}


CSingleLineEditUI::CSingleLineEditUI() : m_win(NULL), m_uEditStyle(ES_AUTOHSCROLL), m_bReadOnly(false)
{
}

const TCHAR* CSingleLineEditUI::GetClass() const
{
   return _T("SingleLineEditUI");
}

UINT CSingleLineEditUI::GetControlFlags() const
{
   return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void CSingleLineEditUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_SETCURSOR) 
   {
      if (IsEnabled())  {
         ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
         return;
      }
   }
   if (event.Type == UIEVENT_WINDOWSIZE) 
   {
      if (m_win != NULL)  m_manager->SetFocus(NULL);
   }
   if (event.Type == UIEVENT_SETFOCUS)  
   {
      if (IsEnabled())  {
         m_win = new CSingleLineEditWnd();
         ASSERT(m_win);
         m_win->Init(this);
      }
   }
   if (event.Type == UIEVENT_BUTTONDOWN && IsFocused() && m_win == NULL)  
   {
      // FIX: In the case of window having lost focus, editor is gone, but
      //      we don't get another SetFocus when we click on the control again.
      m_win = new CSingleLineEditWnd();
      ASSERT(m_win);
      m_win->Init(this);
      return;
   }
   ControlUI::Event(event);
}

void CSingleLineEditUI::SetText(const TCHAR* txt)
{
   m_txt = txt;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("changed"));
   Invalidate();
}

void CSingleLineEditUI::SetReadOnly(bool bReadOnly)
{
   m_bReadOnly = bReadOnly;
   Invalidate();
}

bool CSingleLineEditUI::IsReadOnly() const
{
   return m_bReadOnly;
}

void CSingleLineEditUI::SetEditStyle(UINT uStyle)
{
   m_uEditStyle = uStyle;
   Invalidate();
}

SIZE CSingleLineEditUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 12 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void CSingleLineEditUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   UINT uState = 0;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (IsReadOnly())  uState |= UISTATE_READONLY;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   CBlueRenderEngineUI::DoPaintEditBox(hDC, m_manager, m_rcItem, m_txt, uState, m_uEditStyle, false);
}

class CMultiLineEditWnd : public CWindowWnd
{
public:
   void Init(CMultiLineEditUI* owner);

   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetSuperClassName() const;
   void OnFinalMessage(HWND hWnd);

   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   CMultiLineEditUI* m_owner;
};


void CMultiLineEditWnd::Init(CMultiLineEditUI* owner)
{
   RECT rcPos = owner->GetPos();
   ::InflateRect(&rcPos, -1, -3);
   Create(owner->m_manager->GetPaintWindow(), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 0, rcPos);
   SetWindowFont(m_hWnd, owner->m_manager->GetThemeFont(UIFONT_NORMAL), TRUE);
   Edit_SetText(m_hWnd, owner->m_txt);
   Edit_SetModify(m_hWnd, FALSE);
   SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
   Edit_SetReadOnly(m_hWnd, owner->IsReadOnly() == true);
   Edit_Enable(m_hWnd, owner->IsEnabled() == true);
   if (owner->IsVisible())  ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
   m_owner = owner;
}

const TCHAR* CMultiLineEditWnd::GetWindowClassName() const
{
   return _T("MultiLineEditWnd");
}

const TCHAR* CMultiLineEditWnd::GetSuperClassName() const
{
   return WC_EDIT;
}

void CMultiLineEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
   m_owner->m_win = NULL;
   delete this;
}

LRESULT CMultiLineEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lRes = 0;
   BOOL bHandled = TRUE;
   if (uMsg == OCM_COMMAND && GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)  lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
   else bHandled = FALSE;
   if (!bHandled)  return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
   return lRes;
}

LRESULT CMultiLineEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (m_owner == NULL)  return 0;
   // Copy text back
   int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
   TCHAR* pstr = static_cast<TCHAR*>(_alloca(cchLen * sizeof(TCHAR)));
   ASSERT(pstr);
   ::GetWindowText(m_hWnd, pstr, cchLen);
   m_owner->m_txt = pstr;
   m_owner->GetManager()->SendNotify(m_owner, _T("changed"));
   return 0;
}


CMultiLineEditUI::CMultiLineEditUI() : m_win(NULL)
{
}

CMultiLineEditUI::~CMultiLineEditUI()
{
   if (m_win != NULL && ::IsWindow(*m_win))  m_win->Close();
}

void CMultiLineEditUI::Init()
{
   m_win = new CMultiLineEditWnd();
   ASSERT(m_win);
   m_win->Init(this);
}

const TCHAR* CMultiLineEditUI::GetClass() const
{
   return _T("MultiLineEditUI");
}

UINT CMultiLineEditUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void CMultiLineEditUI::SetText(const TCHAR* txt)
{
   m_txt = txt;
   if (m_win != NULL)  SetWindowText(*m_win, txt);
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("changed"));
   Invalidate();
}

CStdString CMultiLineEditUI::GetText() const
{
   if (m_win != NULL)  {
      int cchLen = ::GetWindowTextLength(*m_win) + 1;
      TCHAR* pstr = static_cast<TCHAR*>(_alloca(cchLen * sizeof(TCHAR)));
      ASSERT(pstr);
      ::GetWindowText(*m_win, pstr, cchLen);
      return CStdString(pstr);
   }
   return m_txt;
}

void CMultiLineEditUI::SetVisible(bool bVisible)
{
   ControlUI::SetVisible(bVisible);
   if (m_win != NULL)  ::ShowWindow(*m_win, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void CMultiLineEditUI::SetEnabled(bool bEnabled)
{
   ControlUI::SetEnabled(bEnabled);
   if (m_win != NULL)  ::EnableWindow(*m_win, bEnabled == true);
}

void CMultiLineEditUI::SetReadOnly(bool bReadOnly)
{
   if (m_win != NULL)  Edit_SetReadOnly(*m_win, bReadOnly == true);
   Invalidate();
}

bool CMultiLineEditUI::IsReadOnly() const
{
   return (GetWindowStyle(*m_win) & ES_READONLY) != 0;
}

SIZE CMultiLineEditUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_rcItem);
}

void CMultiLineEditUI::SetPos(RECT rc)
{
   if (m_win != NULL)  {
      CRect rcEdit = rc;
      rcEdit.Deflate(3, 3);
      ::SetWindowPos(*m_win, HWND_TOP, rcEdit.left, rcEdit.top, rcEdit.GetWidth(), rcEdit.GetHeight(), SWP_NOACTIVATE);
   }
   ControlUI::SetPos(rc);
}

void CMultiLineEditUI::SetPos(int left, int top, int right, int bottom)
{
   SetPos(CRect(left, top, right, bottom));
}

void CMultiLineEditUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_WINDOWSIZE) 
   {
      if (m_win != NULL)  m_manager->SetFocus(NULL);
   }
   if (event.Type == UIEVENT_SETFOCUS)  
   {
      if (m_win != NULL)  ::SetFocus(*m_win);
   }
   ControlUI::Event(event);
}

void CMultiLineEditUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   UINT uState = 0;
   if (IsFocused())  uState |= UISTATE_FOCUSED;
   if (IsReadOnly())  uState |= UISTATE_READONLY;
   if (!IsEnabled())  uState |= UISTATE_DISABLED;
   CBlueRenderEngineUI::DoPaintEditBox(hDC, m_manager, m_rcItem, _T(""), uState, 0, true);
}

