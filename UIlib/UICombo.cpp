#include "StdAfx.h"
#include "UICombo.h"

SingleLinePickUI::SingleLinePickUI() : m_cxWidth(0), m_nLinks(0), m_uButtonState(0)
{
    ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
    ::ZeroMemory(&m_rcButton, sizeof(m_rcButton));
}

const char* SingleLinePickUI::GetClass() const
{
    return "SinglePrettyEditUI";
}

UINT SingleLinePickUI::GetControlFlags() const
{
    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void SingleLinePickUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_SETCURSOR) 
    {
        for (int i = 0; i < m_nLinks; i++)  {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
                return;
            }
        }      
    }
    if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
    {
        if (::PtInRect(&m_rcButton, event.ptMouse))  {
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        } else {
            // Check for link press
            for (int i = 0; i < m_nLinks; i++)  {
                if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
                    m_mgr->SendNotify(this, "link");
                    return;
                }
            }      
        }
        return;
    }
    if (event.type == UIEVENT_MOUSEMOVE) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            if (::PtInRect(&m_rcButton, event.ptMouse))  m_uButtonState |= UISTATE_PUSHED;
            else m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
    }
    if (event.type == UIEVENT_BUTTONUP) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            if (::PtInRect(&m_rcButton, event.ptMouse))  m_mgr->SendNotify(this, "browse");
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
        }
    }
    if (event.type == UIEVENT_KEYDOWN)  
    {
        if (event.chKey == VK_SPACE && m_nLinks > 0)  m_mgr->SendNotify(this, "link");
        if (event.chKey == VK_F4 && IsEnabled())  m_mgr->SendNotify(this, "browse");
    }
    ControlUI::Event(event);
}

void SingleLinePickUI::SetWidth(int cxWidth)
{
    m_cxWidth = cxWidth;
    UpdateLayout();
}

SIZE SingleLinePickUI::EstimateSize(SIZE /*szAvailable*/)
{
    SIZE sz = { 0, 12 + m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
    if (m_cxWidth > 0)  {
        sz.cx = m_cxWidth;
        RECT rcText = m_rcItem;
        ::InflateRect(&rcText, -4, -2);
        m_nLinks = dimof(m_rcLinks);
        BlueRenderEngineUI::DoPaintPrettyText(m_mgr->GetPaintDC(), m_mgr, rcText, m_txt, UICOLOR_EDIT_TEXT_NORMAL, UICOLOR__INVALID, m_rcLinks, m_nLinks, DT_SINGLELINE | DT_CALCRECT);
        sz.cy = RectDy(rcText);
    }
    return sz;
}

void SingleLinePickUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    int cy = RectDy(m_rcItem);
    ::SetRect(&m_rcButton, m_rcItem.right - cy, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    RECT rcText = { m_rcItem.left, m_rcItem.top, m_rcButton.left - 4, m_rcItem.bottom };
    UITYPE_COLOR iTextColor = UICOLOR_EDIT_TEXT_NORMAL;
    UITYPE_COLOR iBorderColor = UICOLOR_CONTROL_BORDER_NORMAL;
    UITYPE_COLOR iBackColor = UICOLOR_CONTROL_BACKGROUND_NORMAL;
    if (IsFocused())  {
        iTextColor = UICOLOR_EDIT_TEXT_NORMAL;
        iBorderColor = UICOLOR_CONTROL_BORDER_NORMAL;
        iBackColor = UICOLOR_CONTROL_BACKGROUND_HOVER;
    }
    if (!IsEnabled())  {
        iTextColor = UICOLOR_EDIT_TEXT_DISABLED;
        iBorderColor = UICOLOR_CONTROL_BORDER_DISABLED;
        iBackColor = UICOLOR__INVALID;
    }
    BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcText, iBorderColor, iBorderColor, iBackColor);
    ::InflateRect(&rcText, -4, -2);
    m_nLinks = dimof(m_rcLinks);
    BlueRenderEngineUI::DoPaintPrettyText(hDC, m_mgr, rcText, m_txt, iTextColor, UICOLOR__INVALID, m_rcLinks, m_nLinks, DT_SINGLELINE);
    RECT rcPadding = { 0 };
    BlueRenderEngineUI::DoPaintButton(hDC, m_mgr, m_rcButton, "<i 4>", rcPadding, m_uButtonState, 0);
}


class DropDownWnd : public WindowWnd
{
public:
    void Init(DropDownUI* owner);
    virtual const char* GetWindowClassName() const;
    virtual void OnFinalMessage(HWND hWnd);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    PaintManagerUI m_pm;
    DropDownUI*    m_owner;
    int            m_oldSel;
};

void DropDownWnd::Init(DropDownUI* owner)
{
    m_owner = owner;
    m_oldSel = m_owner->GetCurSel();
    // Position the popup window in absolute space
    SIZE szDrop = m_owner->GetDropDownSize();
    RECT rc = owner->GetPos();
    rc.top = rc.bottom;
    rc.bottom = rc.top + szDrop.cy;
    if (szDrop.cx > 0)
        rc.right = rc.left + szDrop.cx;
    MapWindowRect(owner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    Create(owner->GetManager()->GetPaintWindow(), NULL, WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW, rc);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while (::GetParent(hWndParent) != NULL)  hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

const char* DropDownWnd::GetWindowClassName() const
{
    return "DropDownWnd";
}

void DropDownWnd::OnFinalMessage(HWND hWnd)
{
    delete this;
}

LRESULT DropDownWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)  {     
        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        ControlCanvasUI* win = new ControlCanvasUI;
        VerticalLayoutUI* pLayout = new VerticalLayoutUI;
        for (int i = 0; i < m_owner->GetCount(); i++)
            pLayout->Add((ControlUI*)m_owner->GetItem(i));
        pLayout->SetAutoDestroy(false);
        pLayout->EnableScrollBar();
        win->Add(pLayout);
        m_pm.AttachDialog(win);
        return 0;
    } else if (uMsg == WM_CLOSE)  {
        m_owner->SetManager(m_owner->GetManager(), m_owner->GetParent());
        m_owner->SetPos(m_owner->GetPos());
        m_owner->SetFocus();
    } else if (uMsg == WM_LBUTTONUP)  {
        PostMessage(WM_KILLFOCUS);
    } else if (uMsg == WM_KEYDOWN)  {
        switch (wParam)  {
        case VK_ESCAPE:
            m_owner->SelectItem(m_oldSel);
            // FALL THROUGH...
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.type = UIEVENT_KEYDOWN;
            event.chKey = wParam;
            m_owner->Event(event);
            return 0;
        }
    } else if (uMsg == WM_KILLFOCUS)  {
        if (m_hWnd != (HWND) wParam)  PostMessage(WM_CLOSE);
    }
    LRESULT lRes = 0;
    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))  return lRes;
    return WindowWnd::HandleMessage(uMsg, wParam, lParam);
}

DropDownUI::DropDownUI() : 
m_curSel(-1), 
    m_uButtonState(0)
{
    m_szDropBox = CSize(0, 150);
    ::ZeroMemory(&m_rcButton, sizeof(RECT));
}

const char* DropDownUI::GetClass() const
{
    return "DropDownUI";
}

void* DropDownUI::GetInterface(const char* name)
{
    if (str::Eq(name, "ListOwner"))
        return static_cast<IListOwnerUI*>(this);
    return ContainerUI::GetInterface(name);
}

UINT DropDownUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

void DropDownUI::Init()
{
    if (m_curSel < 0)  SelectItem(0);
}

int DropDownUI::GetCurSel() const
{
    return m_curSel;
}

bool DropDownUI::SelectItem(int idx)
{
    if (idx == m_curSel)  return true;
    if (m_curSel >= 0)  {
        ControlUI* ctrl = static_cast<ControlUI*>(m_items[m_curSel]);
        IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
        if (listItem != NULL)  listItem->Select(false);
        m_curSel = -1;
    }
    if (m_items.GetSize() == 0)  return false;
    if (idx < 0)  idx = 0;
    if (idx >= m_items.GetSize())  idx = m_items.GetSize() - 1;
    ControlUI* ctrl = static_cast<ControlUI*>(m_items[idx]);
    if (!ctrl->IsVisible())  return false;
    if (!ctrl->IsEnabled())  return false;
    IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
    if (listItem == NULL)  return false;
    m_curSel = idx;
    ctrl->SetFocus();
    listItem->Select(true);
    if (m_mgr != NULL)  m_mgr->SendNotify(ctrl, "itemclick");
    if (m_mgr != NULL)  m_mgr->SendNotify(this, "itemselect");
    Invalidate();
    return true;
}

bool DropDownUI::Add(ControlUI* ctrl)
{
    IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
    if (listItem != NULL)  {
        listItem->SetOwner(this);
        listItem->SetIndex(m_items.GetSize());
    }
    return ContainerUI::Add(ctrl);
}

bool DropDownUI::Remove(ControlUI* ctrl)
{
    ASSERT(!"Not supported");
    return false;
}

void DropDownUI::RemoveAll()
{
    m_curSel = -1;
    ContainerUI::RemoveAll();
}

void DropDownUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
    {
        Activate();
        m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
    }
    if (event.type == UIEVENT_MOUSEMOVE) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            if (::PtInRect(&m_rcButton, event.ptMouse))  m_uButtonState |= UISTATE_PUSHED;
            else m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
    }
    if (event.type == UIEVENT_BUTTONUP) 
    {
        if (FlSet(m_uButtonState, UISTATE_CAPTURED))  {
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
        }
    }
    if (event.type == UIEVENT_KEYDOWN) 
    {
        switch (event.chKey)  {
        case VK_F4:
            Activate();
            return;
        case VK_UP:
            SelectItem(FindSelectable(m_curSel - 1, false));
            return;
        case VK_DOWN:
            SelectItem(FindSelectable(m_curSel + 1, true));
            return;
        case VK_PRIOR:
            SelectItem(FindSelectable(m_curSel - 10, false));
            return;
        case VK_NEXT:
            SelectItem(FindSelectable(m_curSel + 10, true));
            return;
        case VK_HOME:
            SelectItem(FindSelectable(0, false));
            return;
        case VK_END:
            SelectItem(FindSelectable(GetCount() - 1, true));
            return;
        }
    }
    if (event.type == UIEVENT_SCROLLWHEEL) 
    {
        bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
        SelectItem(FindSelectable(m_curSel + (bDownward ? 1 : -1), bDownward));
        return;
    }
    ControlUI::Event(event);
}

bool DropDownUI::Activate()
{
    if (!ControlUI::Activate())  return false;
    DropDownWnd* win = new DropDownWnd;
    win->Init(this);
    if (m_mgr != NULL)  m_mgr->SendNotify(this, "dropdown");
    Invalidate();
    return true;
}

const char* DropDownUI::GetText() const
{
    if (m_curSel < 0)  return "";
    ControlUI* ctrl = static_cast<ControlUI*>(m_items[m_curSel]);
    return ctrl->GetText();
}

SIZE DropDownUI::GetDropDownSize() const
{
    return m_szDropBox;
}

void DropDownUI::SetDropDownSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

void DropDownUI::SetPos(RECT rc)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for (int i = 0; i < m_items.GetSize(); i++)  static_cast<ControlUI*>(m_items[i])->SetPos(rcNull);
    // Position this control
    ControlUI::SetPos(rc);
}

SIZE DropDownUI::EstimateSize(SIZE /*szAvailable*/)
{
    SIZE sz = { 0, 12 + m_mgr->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
    // Once there is an element in the list, we'll use the first one to
    // determine the size of the dropdown base control.
    if (m_cxyFixed.cx > 0 && !m_items.IsEmpty())  {
        RECT rcText = m_rcItem;
        ::InflateRect(&rcText, -4, -2);
        sz = static_cast<ControlUI*>(m_items[0])->EstimateSize(CSize(RectDx(rcText), 0));
    }
    return sz;
}

void DropDownUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // Paint the nice frame
    int cy = RectDy(m_rcItem);
    ::SetRect(&m_rcButton, m_rcItem.right - cy, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    RECT rcText = { m_rcItem.left, m_rcItem.top, m_rcButton.left + 1, m_rcItem.bottom };
    if (!IsEnabled())  {
        BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcText, UICOLOR_CONTROL_BORDER_DISABLED, UICOLOR__INVALID, UICOLOR__INVALID);
    } else {
        BlueRenderEngineUI::DoPaintFrame(hDC, m_mgr, rcText, UICOLOR_CONTROL_BORDER_NORMAL, UICOLOR_CONTROL_BORDER_NORMAL, UICOLOR__INVALID);
    }
    // Paint dropdown edit box
    ::InflateRect(&rcText, -1, -1);
    if (m_curSel >= 0)  {
        ControlUI* ctrl = static_cast<ControlUI*>(m_items[m_curSel]);
        IListItemUI* pElement = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
        if (pElement != NULL)  {
            // Render item with specific draw-style
            pElement->DrawItem(hDC, rcText, UIDRAWSTYLE_INPLACE | (m_focused ? UIDRAWSTYLE_FOCUS : 0));
        } else {
            // Allow non-listitems to render as well.
            RECT rcOldPos = ctrl->GetPos();
            ctrl->SetPos(rcText);
            ctrl->DoPaint(hDC, rcText);
            ctrl->SetPos(rcOldPos);
        }
    } else {
        BlueRenderEngineUI::DoFillRect(hDC, m_mgr, rcText, UICOLOR_CONTROL_BACKGROUND_NORMAL);
    }
    // Paint dropdown button
    RECT rcPadding = { 0 };
    BlueRenderEngineUI::DoPaintButton(hDC, m_mgr, m_rcButton, "<i 6>", rcPadding, m_uButtonState, 0);
}

