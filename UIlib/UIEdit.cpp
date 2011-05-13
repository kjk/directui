
#include "StdAfx.h"
#include "UIEdit.h"


class SingleLineEditWnd : public WindowWnd
{
public:
    SingleLineEditWnd();

    void Init(SingleLineEditUI* owner);

    virtual const char* GetWindowClassName() const;
    virtual const char* GetSuperClassName() const;
    virtual void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    SingleLineEditUI* m_owner;
};


SingleLineEditWnd::SingleLineEditWnd() : m_owner(NULL)
{
}

void SingleLineEditWnd::Init(SingleLineEditUI* owner)
{
    CRect rcPos = owner->GetPos();
    rcPos.Deflate(1, 3);
    Create(owner->GetManager()->GetPaintWindow(), NULL, WS_CHILD | owner->m_uEditStyle, 0, rcPos);
    SetWindowFont(m_hWnd, owner->GetManager()->GetThemeFont(UIFONT_NORMAL), TRUE);
    Edit_SetTextUtf8(m_hWnd, owner->GetText());
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
    Edit_SetSel(m_hWnd, 0, -1);
    Edit_Enable(m_hWnd, owner->IsEnabled() == true);
    Edit_SetReadOnly(m_hWnd, owner->IsReadOnly() == true);
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);
    m_owner = owner;
}

const char* SingleLineEditWnd::GetWindowClassName() const
{
    return "SingleLineEditWnd";
}

const char* SingleLineEditWnd::GetSuperClassName() const
{
    return WC_EDITA;
}

void SingleLineEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
    // Clear reference and die
    m_owner->m_win = NULL;
    delete this;
}

LRESULT SingleLineEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    if (uMsg == WM_KILLFOCUS)  lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
    else if (uMsg == OCM_COMMAND && GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)  lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
    else bHandled = FALSE;
    if (!bHandled)  return WindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT SingleLineEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    PostMessage(WM_CLOSE);
    return lRes;
}

LRESULT SingleLineEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if (m_owner == NULL)  return 0;
    char *s = GetWindowTextUtf8(m_hWnd);
    if (s)
        m_owner->SetText(s);
    free(s);
    return 0;
}

SingleLineEditUI::SingleLineEditUI() : m_win(NULL), m_uEditStyle(ES_AUTOHSCROLL), m_bReadOnly(false)
{
}

const char* SingleLineEditUI::GetClass() const
{
    return "SingleLineEditUI";
}

UINT SingleLineEditUI::GetControlFlags() const
{
    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void SingleLineEditUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_SETCURSOR) 
    {
        if (IsEnabled())  {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
            return;
        }
    }
    if (event.type == UIEVENT_WINDOWSIZE) 
    {
        if (m_win != NULL)  m_mgr->SetFocus(NULL);
    }
    if (event.type == UIEVENT_SETFOCUS)  
    {
        if (IsEnabled())  {
            m_win = new SingleLineEditWnd();
            ASSERT(m_win);
            m_win->Init(this);
        }
    }
    if (event.type == UIEVENT_BUTTONDOWN && IsFocused() && m_win == NULL)  
    {
        // FIX: In the case of window having lost focus, editor is gone, but
        //      we don't get another SetFocus when we click on the control again.
        m_win = new SingleLineEditWnd();
        ASSERT(m_win);
        m_win->Init(this);
        return;
    }
    ControlUI::Event(event);
}

void SingleLineEditUI::SetText(const char* txt)
{
    str::Replace(m_txt, txt);
    if (m_mgr != NULL)  m_mgr->SendNotify(this, "changed");
    Invalidate();
}

void SingleLineEditUI::SetReadOnly(bool bReadOnly)
{
    m_bReadOnly = bReadOnly;
    Invalidate();
}

bool SingleLineEditUI::IsReadOnly() const
{
    return m_bReadOnly;
}

void SingleLineEditUI::SetEditStyle(UINT uStyle)
{
    m_uEditStyle = uStyle;
    Invalidate();
}

SIZE SingleLineEditUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(0, 12 + m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void SingleLineEditUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
    UINT uState = 0;
    if (IsFocused())  uState |= UISTATE_FOCUSED;
    if (IsReadOnly())  uState |= UISTATE_READONLY;
    if (!IsEnabled())  uState |= UISTATE_DISABLED;
    BlueRenderEngineUI::DoPaintEditBox(hDC, m_mgr, m_rcItem, m_txt, uState, m_uEditStyle, false);
}

class MultiLineEditWnd : public WindowWnd
{
public:
    void Init(MultiLineEditUI* owner);

    virtual const char* GetWindowClassName() const;
    virtual const char* GetSuperClassName() const;
    virtual void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    MultiLineEditUI* m_owner;
};


void MultiLineEditWnd::Init(MultiLineEditUI* owner)
{
    RECT rcPos = owner->GetPos();
    ::InflateRect(&rcPos, -1, -3);
    Create(owner->m_mgr->GetPaintWindow(), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 0, rcPos);
    SetWindowFont(m_hWnd, owner->m_mgr->GetThemeFont(UIFONT_NORMAL), TRUE);
    Edit_SetTextUtf8(m_hWnd, owner->m_txt);
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
    Edit_SetReadOnly(m_hWnd, owner->IsReadOnly() == true);
    Edit_Enable(m_hWnd, owner->IsEnabled() == true);
    if (owner->IsVisible())  ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    m_owner = owner;
}

const char* MultiLineEditWnd::GetWindowClassName() const
{
    return "MultiLineEditWnd";
}

const char* MultiLineEditWnd::GetSuperClassName() const
{
    return WC_EDITA;
}

void MultiLineEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
    m_owner->m_win = NULL;
    delete this;
}

LRESULT MultiLineEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    if (uMsg == OCM_COMMAND && GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)  lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
    else bHandled = FALSE;
    if (!bHandled)  return WindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT MultiLineEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if (m_owner == NULL)  return 0;
    char *s = GetWindowTextUtf8(m_hWnd);
    if (s) {
        m_owner->SetText(s);
        m_owner->GetManager()->SendNotify(m_owner, "changed");
        free(s);
    }
    return 0;
}

MultiLineEditUI::MultiLineEditUI() : m_win(NULL)
{
}

MultiLineEditUI::~MultiLineEditUI()
{
    if (m_win != NULL && ::IsWindow(*m_win))  m_win->Close();
}

void MultiLineEditUI::Init()
{
    m_win = new MultiLineEditWnd();
    ASSERT(m_win);
    m_win->Init(this);
}

const char* MultiLineEditUI::GetClass() const
{
    return "MultiLineEditUI";
}

UINT MultiLineEditUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

void MultiLineEditUI::SetText(const char* txt)
{
    str::Replace(m_txt, txt);
    if (m_win != NULL)  SetWindowTextUtf8(*m_win, txt);
    if (m_mgr != NULL)  m_mgr->SendNotify(this, "changed");
    Invalidate();
}

const char * MultiLineEditUI::GetText() const
{
    if (m_win != NULL)  {
        // TDOO: this leak
        char *s = GetWindowTextUtf8(*m_win);
        return (const char*)s;
    }
    return m_txt;
}

void MultiLineEditUI::SetVisible(bool visible)
{
    ControlUI::SetVisible(visible);
    if (m_win != NULL)  ::ShowWindow(*m_win, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void MultiLineEditUI::SetEnabled(bool enabled)
{
    ControlUI::SetEnabled(enabled);
    if (m_win != NULL)  ::EnableWindow(*m_win, enabled == true);
}

void MultiLineEditUI::SetReadOnly(bool bReadOnly)
{
    if (m_win != NULL)  Edit_SetReadOnly(*m_win, bReadOnly == true);
    Invalidate();
}

bool MultiLineEditUI::IsReadOnly() const
{
    return IsFlSet(GetWindowStyle(*m_win), ES_READONLY);
}

SIZE MultiLineEditUI::EstimateSize(SIZE /*szAvailable*/)
{
    return CSize(m_rcItem);
}

void MultiLineEditUI::SetPos(RECT rc)
{
    if (m_win != NULL)  {
        CRect rcEdit = rc;
        rcEdit.Deflate(3, 3);
        ::SetWindowPos(*m_win, HWND_TOP, rcEdit.left, rcEdit.top, rcEdit.GetWidth(), rcEdit.GetHeight(), SWP_NOACTIVATE);
    }
    ControlUI::SetPos(rc);
}

void MultiLineEditUI::SetPos(int left, int top, int right, int bottom)
{
    SetPos(CRect(left, top, right, bottom));
}

void MultiLineEditUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_WINDOWSIZE) 
    {
        if (m_win != NULL)  m_mgr->SetFocus(NULL);
    }
    if (event.type == UIEVENT_SETFOCUS)  
    {
        if (m_win != NULL)  ::SetFocus(*m_win);
    }
    ControlUI::Event(event);
}

void MultiLineEditUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
    UINT uState = 0;
    if (IsFocused())  uState |= UISTATE_FOCUSED;
    if (IsReadOnly())  uState |= UISTATE_READONLY;
    if (!IsEnabled())  uState |= UISTATE_DISABLED;
    BlueRenderEngineUI::DoPaintEditBox(hDC, m_mgr, m_rcItem, "", uState, 0, true);
}
