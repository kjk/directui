
#include "StdAfx.h"
#include "UITab.h"


TabFolderUI::TabFolderUI() : m_curSel(-1), m_curPage(NULL), m_tabAreas(sizeof(RECT))
{
    m_chShortcut = VK_NEXT;
}

const char* TabFolderUI::GetClass() const
{
    return "TabFolderUI";
}

void TabFolderUI::Init()
{
    if (m_curSel == -1)  SelectItem(0);
}

bool TabFolderUI::Add(ControlUI* ctrl)
{
    ctrl->SetVisible(false);
    return ContainerUI::Add(ctrl);
}

int TabFolderUI::GetCurSel() const
{
    return m_curSel;
}

bool TabFolderUI::SelectItem(int idx)
{
    int iPrevSel = m_curSel;
    if (idx < 0 || idx >= m_items.GetSize())  return false;
    if (idx == m_curSel)  return true;
    // Assign page to internal pointers
    if (m_curPage != NULL)  m_curPage->SetVisible(false);
    m_curSel = idx;
    m_curPage = static_cast<ControlUI*>(m_items[idx]);
    if (m_manager != NULL)  m_manager->SendNotify(this, "itemselect");
    m_curPage->SetVisible(true);
    // Need to re-think the layout
    if (m_manager != NULL)  m_manager->UpdateLayout();
    // Set focus on page
    m_curPage->SetFocus();
    if (m_manager != NULL)  m_manager->SetNextTabControl();
    return true;
}

void TabFolderUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
    {
        for (int i = 0; i < m_items.GetSize() && i < m_tabAreas.GetSize(); i++)  {
            if (::PtInRect( static_cast<LPRECT>(m_tabAreas[i]), event.ptMouse))  {
                SelectItem(i);
                return;
            }
        }
    }
    if (event.type == UIEVENT_SYSKEY && IsEnabled()) 
    {
        if (event.chKey == VK_PRIOR && (event.wKeyState & MK_ALT) != 0)  {
            SelectItem(m_curSel - 1);
            return;
        }
        if (event.chKey == VK_NEXT && (event.wKeyState & MK_ALT) != 0)  {
            SelectItem(m_curSel + 1);
            return;
        }
    }
    ContainerUI::Event(event);
}

void TabFolderUI::SetPos(RECT rc)
{
    ControlUI::SetPos(rc);
    // Determine size of embedded page and place it there
    int cyFont = m_manager->GetThemeFontInfo(UIFONT_BOLD).tmHeight;
    ::SetRect(&m_rcClient, rc.left + m_rcInset.left, rc.top + m_rcInset.top + cyFont + 8, rc.right - m_rcInset.right, rc.bottom - m_rcInset.bottom);
    m_rcPage = m_rcClient;
    ::InflateRect(&m_rcPage, -8, -8);
    if (m_curPage != NULL)  m_curPage->SetPos(m_rcPage);
}

void TabFolderUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    {
        RenderClip clip;
        BlueRenderEngineUI::GenerateClip(hDC, rcPaint, clip);

        // Fill client area background
        RECT rcFill = { 0 };
        ::IntersectRect(&rcFill, &rcPaint, &m_rcClient);
        BlueRenderEngineUI::DoFillRect(hDC, m_manager, rcFill, UICOLOR_TAB_BACKGROUND_NORMAL);

        // Frame around client area
        BlueRenderEngineUI::DoPaintRectangle(hDC, m_manager, m_rcClient, UICOLOR_TAB_BORDER, UICOLOR__INVALID);

        // Paint tab strip
        RECT rcTabs = m_rcItem;
        rcTabs.left += m_rcInset.left;
        rcTabs.top += m_rcInset.top;
        rcTabs.right -= m_rcInset.right;
        rcTabs.bottom = m_rcClient.top;
        RECT rcTemp = { 0 };
        if (::IntersectRect(&rcTemp, &rcPaint, &rcTabs))  
        {
            int posX = 1;
            m_tabAreas.Empty();
            for (int i = 0; i < GetCount(); i++)  
            {
                const ControlUI* page = GetItem(i);
                const char *txt = page->GetText();
                RECT rcTab = { rcTabs.left + posX, rcTabs.top, rcTabs.right, m_rcClient.top };
                UINT uState = 0;
                if (IsFocused())  uState |= UISTATE_FOCUSED;
                if (!IsEnabled())  uState |= UISTATE_DISABLED;
                if (m_curSel == i)  uState = UISTATE_PUSHED;
                BlueRenderEngineUI::DoPaintTabFolder(hDC, m_manager, rcTab, txt, uState);
                posX += (rcTab.right - rcTab.left) + 2;
                m_tabAreas.Add(&rcTab);
            }
        }
    }

    if (m_curPage != NULL)  m_curPage->DoPaint(hDC, rcPaint);
}

void TabFolderUI::SetAttribute(const char* name, const char* value)
{
    if (str::Eq(name, "select"))  SelectItem(atoi(value));
    else ContainerUI::SetAttribute(name, value);
}

TabPageUI::TabPageUI()
{
    SetInset(CSize(6, 6));
}

const char* TabPageUI::GetClass() const
{
    return "TabPageUI";
}

bool TabPageUI::Activate()
{
    if (!ContainerUI::Activate())  return false;
    ControlUI* parent = GetParent();
    if (parent == NULL || parent->GetInterface("ListOwner") == NULL)  return false;
    return static_cast<IListOwnerUI*>(parent->GetInterface("ListOwner"))->SelectItem(0 /*m_idx*/);
}
