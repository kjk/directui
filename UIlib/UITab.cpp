
#include "StdAfx.h"
#include "UITab.h"


CTabFolderUI::CTabFolderUI() : m_iCurSel(-1), m_pCurPage(NULL), m_aTabAreas(sizeof(RECT))
{
   m_chShortcut = VK_NEXT;
}

const TCHAR* CTabFolderUI::GetClass() const
{
   return _T("TabFolderUI");
}

void CTabFolderUI::Init()
{
   if( m_iCurSel == -1 ) SelectItem(0);
}

bool CTabFolderUI::Add(CControlUI* ctrl)
{
   ctrl->SetVisible(false);
   return CContainerUI::Add(ctrl);
}

int CTabFolderUI::GetCurSel() const
{
   return m_iCurSel;
}

bool CTabFolderUI::SelectItem(int idx)
{
   int iPrevSel = m_iCurSel;
   if( idx < 0 || idx >= m_items.GetSize() ) return false;
   if( idx == m_iCurSel ) return true;
   // Assign page to internal pointers
   if( m_pCurPage != NULL ) m_pCurPage->SetVisible(false);
   m_iCurSel = idx;
   m_pCurPage = static_cast<CControlUI*>(m_items[idx]);
   if( m_manager != NULL ) m_manager->SendNotify(this, _T("itemselect"));
   m_pCurPage->SetVisible(true);
   // Need to re-think the layout
   if( m_manager != NULL ) m_manager->UpdateLayout();
   // Set focus on page
   m_pCurPage->SetFocus();
   if( m_manager != NULL ) m_manager->SetNextTabControl();
   return true;
}

void CTabFolderUI::Event(TEventUI& event)
{
   if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
   {
      for( int i = 0; i < m_items.GetSize() && i < m_aTabAreas.GetSize(); i++ ) {
         if( ::PtInRect( static_cast<LPRECT>(m_aTabAreas[i]), event.ptMouse) ) {
            SelectItem(i);
            return;
         }
      }
   }
   if( event.Type == UIEVENT_SYSKEY && IsEnabled() )
   {
      if( event.chKey == VK_PRIOR && (event.wKeyState & MK_ALT) != 0 ) {
         SelectItem(m_iCurSel - 1);
         return;
      }
      if( event.chKey == VK_NEXT && (event.wKeyState & MK_ALT) != 0 ) {
         SelectItem(m_iCurSel + 1);
         return;
      }
   }
   CContainerUI::Event(event);
}

void CTabFolderUI::SetPos(RECT rc)
{
   CControlUI::SetPos(rc);
   // Determine size of embedded page and place it there
   int cyFont = m_manager->GetThemeFontInfo(UIFONT_BOLD).tmHeight;
   ::SetRect(&m_rcClient, rc.left + m_rcInset.left, rc.top + m_rcInset.top + cyFont + 8, rc.right - m_rcInset.right, rc.bottom - m_rcInset.bottom);
   m_rcPage = m_rcClient;
   ::InflateRect(&m_rcPage, -8, -8);
   if( m_pCurPage != NULL ) m_pCurPage->SetPos(m_rcPage);
}

void CTabFolderUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   {
      CRenderClip clip;
      CBlueRenderEngineUI::GenerateClip(hDC, rcPaint, clip);

      // Fill client area background
      RECT rcFill = { 0 };
      ::IntersectRect(&rcFill, &rcPaint, &m_rcClient);
      CBlueRenderEngineUI::DoFillRect(hDC, m_manager, rcFill, UICOLOR_TAB_BACKGROUND_NORMAL);

      // Frame around client area
      CBlueRenderEngineUI::DoPaintRectangle(hDC, m_manager, m_rcClient, UICOLOR_TAB_BORDER, UICOLOR__INVALID);

      // Paint tab strip
      RECT rcTabs = m_rcItem;
      rcTabs.left += m_rcInset.left;
      rcTabs.top += m_rcInset.top;
      rcTabs.right -= m_rcInset.right;
      rcTabs.bottom = m_rcClient.top;
      RECT rcTemp = { 0 };
      if( ::IntersectRect(&rcTemp, &rcPaint, &rcTabs) ) 
      {
         int posX = 1;
         m_aTabAreas.Empty();
         for( int i = 0; i < GetCount(); i++ ) 
         {
            const CControlUI* pPage = GetItem(i);
            const CStdString& strText = pPage->GetText();
            RECT rcTab = { rcTabs.left + posX, rcTabs.top, rcTabs.right, m_rcClient.top };
            UINT uState = 0;
            if( IsFocused() ) uState |= UISTATE_FOCUSED;
            if( !IsEnabled() ) uState |= UISTATE_DISABLED;
            if( m_iCurSel == i ) uState = UISTATE_PUSHED;
            CBlueRenderEngineUI::DoPaintTabFolder(hDC, m_manager, rcTab, strText, uState);
            posX += (rcTab.right - rcTab.left) + 2;
            m_aTabAreas.Add(&rcTab);
         }
      }
   }

   if( m_pCurPage != NULL ) m_pCurPage->DoPaint(hDC, rcPaint);
}

void CTabFolderUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if( _tcscmp(name, _T("select")) == 0 ) SelectItem(_ttoi(value));
   else CContainerUI::SetAttribute(name, value);
}


CTabPageUI::CTabPageUI()
{
   SetInset(CSize(6, 6));
}

const TCHAR* CTabPageUI::GetClass() const
{
   return _T("TabPageUI");
}

bool CTabPageUI::Activate()
{
   if( !CContainerUI::Activate() ) return false;
   CControlUI* pParent = GetParent();
   if( pParent == NULL || pParent->GetInterface(_T("ListOwner")) == NULL ) return false;
   return static_cast<IListOwnerUI*>(pParent->GetInterface(_T("ListOwner")))->SelectItem(0 /*m_idx*/);
}
