
#include "StdAfx.h"
#include "UIlist.h"

#include "UILabel.h"
#include "UIPanel.h"

ListElementUI::ListElementUI() : 
   m_idx(-1),
   m_owner(NULL), 
   m_bSelected(false)
{
}

UINT ListElementUI::GetControlFlags() const
{
   return UIFLAG_WANTRETURN;
}

void* ListElementUI::GetInterface(const TCHAR* name)
{
   if (_tcscmp(name, _T("ListItem")) == 0)  return static_cast<IListItemUI*>(this);
   return ControlUI::GetInterface(name);
}

void ListElementUI::SetOwner(ControlUI* owner)
{
   m_owner = static_cast<IListOwnerUI*>(owner->GetInterface(_T("ListOwner")));
}

int ListElementUI::GetIndex() const
{
   return m_idx;
}

void ListElementUI::SetIndex(int idx)
{
   m_idx = idx;
}

bool ListElementUI::Activate()
{
   if (!ControlUI::Activate())  return false;
   if (m_manager != NULL)  m_manager->SendNotify(this, _T("itemactivate"));
   return true;
}

bool ListElementUI::IsSelected() const
{
   return m_bSelected;
}

bool ListElementUI::Select(bool bSelect)
{
   if (!IsEnabled())  return false;
   if (bSelect == m_bSelected)  return true;
   m_bSelected = bSelect;
   if (bSelect && m_owner != NULL)  m_owner->SelectItem(m_idx);
   Invalidate();
   return true;
}

bool ListElementUI::IsExpanded() const
{
   return false;
}

bool ListElementUI::Expand(bool /*bExpand = true*/)
{
   return false;
}

void ListElementUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_DBLCLICK && IsEnabled()) 
   {
      Activate();
      Invalidate();
      return;
   }
   if (event.Type == UIEVENT_KEYDOWN && IsEnabled()) 
   {
      if (event.chKey == VK_RETURN)  {
         Activate();
         Invalidate();
         return;
      }
   }
   // An important twist: The list-item will send the event not to its immediate
   // parent but to the "attached" list. A list may actually embed several components
   // in its path to the item, but key-presses etc. needs to go to the actual list.
   if (m_owner != NULL)  m_owner->Event(event); else ControlUI::Event(event);
}

void ListElementUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("selected")) == 0)  Select();
   else ControlUI::SetAttribute(name, value);
}


ListHeaderUI::ListHeaderUI()
{
   SetInset(CSize(0, 0));
}

const TCHAR* ListHeaderUI::GetClass() const
{
   return _T("ListHeaderUI");
}

void* ListHeaderUI::GetInterface(const TCHAR* name)
{
   if (_tcscmp(name, _T("ListHeader")) == 0)  return this;
   return HorizontalLayoutUI::GetInterface(name);
}

SIZE ListHeaderUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 6 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void ListHeaderUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw background
   COLORREF clrBack1, clrBack2;
   m_manager->GetThemeColorPair(UICOLOR_HEADER_BACKGROUND, clrBack1, clrBack2);
   CBlueRenderEngineUI::DoPaintFrame(hDC, m_manager, m_rcItem, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BACKGROUND, 0);
   RECT rcBottom = { m_rcItem.left + 1, m_rcItem.bottom - 3, m_rcItem.right - 1, m_rcItem.bottom };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcBottom, clrBack1, clrBack2, true, 4);
   // Draw headers too...
   HorizontalLayoutUI::DoPaint(hDC, rcPaint);
}


CListHeaderItemUI::CListHeaderItemUI() : m_uDragState(0)
{
}

const TCHAR* CListHeaderItemUI::GetClass() const
{
   return _T("ListHeaderItemUI");
}

void CListHeaderItemUI::SetText(const TCHAR* txt)
{
   m_txt = txt;
   UpdateLayout();
}

void CListHeaderItemUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void CListHeaderItemUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  SetWidth(_ttol(value));
   else ControlUI::SetAttribute(name, value);
}

UINT CListHeaderItemUI::GetControlFlags() const
{
   return UIFLAG_SETCURSOR;
}

void CListHeaderItemUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled()) 
   {
      RECT rcSeparator = GetThumbRect(m_rcItem);
      if (::PtInRect(&rcSeparator, event.ptMouse))  {
         m_uDragState |= UISTATE_CAPTURED;
         ptLastMouse = event.ptMouse;
         m_manager->SendNotify(this, _T("headerdragging"));
      }
      else {
         m_manager->SendNotify(this, _T("headerclick"));
      }
   }
   if (event.Type == UIEVENT_BUTTONUP) 
   {
      if ((m_uDragState & UISTATE_CAPTURED) != 0)  {
         m_uDragState &= ~UISTATE_CAPTURED;
         m_manager->SendNotify(this, _T("headerdragged"));
         m_manager->UpdateLayout();
      }
   }
   if (event.Type == UIEVENT_MOUSEMOVE) 
   {
      if ((m_uDragState & UISTATE_CAPTURED) != 0)  {
         RECT rc = m_rcItem;
         rc.right -= ptLastMouse.x - event.ptMouse.x;
         const int MIN_DRAGSIZE = 40;
         if (rc.right - rc.left > MIN_DRAGSIZE)  {
            m_rcItem = rc;
            m_cxWidth = rc.right - rc.left;
            ptLastMouse = event.ptMouse;
            m_parent->Invalidate();
         }
      }
   }
   if (event.Type == UIEVENT_SETCURSOR) 
   {
      RECT rcSeparator = GetThumbRect(m_rcItem);
      if (IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse))  {
         ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
         return;
      }
   }
   ControlUI::Event(event);
}

SIZE CListHeaderItemUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, 14 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void CListHeaderItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Paint text (with some indent)
   RECT rcMessage = m_rcItem;
   rcMessage.left += 6;
   rcMessage.bottom -= 1;
   int nLinks = 0;
   CBlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcMessage, m_txt, UICOLOR_HEADER_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
   // Draw gripper
   POINT ptTemp = { 0 };
   RECT rcThumb = GetThumbRect(m_rcItem);
   RECT rc1 = { rcThumb.left + 2, rcThumb.top + 4, rcThumb.left + 2, rcThumb.bottom - 1 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_HEADER_SEPARATOR);
   RECT rc2 = { rcThumb.left + 3, rcThumb.top + 4, rcThumb.left + 3, rcThumb.bottom - 1 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc2, UICOLOR_STANDARD_WHITE);
}

RECT CListHeaderItemUI::GetThumbRect(RECT rc) const
{
   return CRect(rc.right - 4, rc.top, rc.right, rc.bottom - 3);
}


ListFooterUI::ListFooterUI()
{
   LabelPanelUI* pLabel = new LabelPanelUI;
   Add(pLabel);
   PaddingPanelUI* pPadding = new PaddingPanelUI;
   Add(pPadding);
}

const TCHAR* ListFooterUI::GetClass() const
{
   return _T("ListFooterUI");
}

void* ListFooterUI::GetInterface(const TCHAR* name)
{
   if (_tcscmp(name, _T("ListFooter")) == 0)  return this;
   return HorizontalLayoutUI::GetInterface(name);
}

SIZE ListFooterUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(0, 8 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void ListFooterUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   COLORREF clrBack1, clrBack2;
   m_manager->GetThemeColorPair(UICOLOR_HEADER_BACKGROUND, clrBack1, clrBack2);
   CBlueRenderEngineUI::DoPaintFrame(hDC, m_manager, m_rcItem, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BACKGROUND, 0);
   RECT rcTop = { m_rcItem.left + 1, m_rcItem.top, m_rcItem.right - 1, m_rcItem.top + 1 };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack2, clrBack1, true, 2);
   RECT rcBottom = { m_rcItem.left + 1, m_rcItem.bottom - 3, m_rcItem.right - 1, m_rcItem.bottom };
   CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcBottom, clrBack1, clrBack2, true, 4);
   // Paint items as well...
   HorizontalLayoutUI::DoPaint(hDC, rcPaint);
}


ListUI::ListUI() : m_pCallback(NULL), m_curSel(-1), m_iExpandedItem(-1)
{
   m_pList = new VerticalLayoutUI;
   m_pHeader = new ListHeaderUI;
   m_pFooter = new ListFooterUI;

   WhiteCanvasUI* pWhite = new WhiteCanvasUI;
   pWhite->Add(m_pList);

   m_pFooter->Add(new LabelPanelUI);
   m_pFooter->Add(new PaddingPanelUI);

   Add(m_pHeader);
   VerticalLayoutUI::Add(pWhite);
   Add(m_pFooter);

   m_pList->EnableScrollBar();

   ::ZeroMemory(&m_ListInfo, sizeof(TListInfoUI));
}

const TCHAR* ListUI::GetClass() const
{
   return _T("ListUI");
}

UINT ListUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void* ListUI::GetInterface(const TCHAR* name)
{
   if (_tcscmp(name, _T("List")) == 0)  return static_cast<IListUI*>(this);
   if (_tcscmp(name, _T("ListOwner")) == 0)  return static_cast<IListOwnerUI*>(this);
   return VerticalLayoutUI::GetInterface(name);
}

bool ListUI::Add(ControlUI* ctrl)
{
   // Override the Add() method so we can add items specifically to
   // the intended widgets. Headers and footers are assumed to be
   // answer the correct interface so we can add multiple list headers/footers.
   if (ctrl->GetInterface(_T("ListHeader")) != NULL)  return VerticalLayoutUI::Add(ctrl);
   if (ctrl->GetInterface(_T("ListFooter")) != NULL)  return VerticalLayoutUI::Add(ctrl);
   // We also need to recognize header sub-items
   if (_tcsstr(ctrl->GetClass(), _T("Header")) != NULL)  return m_pHeader->Add(ctrl);
   if (_tcsstr(ctrl->GetClass(), _T("Footer")) != NULL)  return m_pFooter->Add(ctrl);
   // The list items should know about us
   IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface(_T("ListItem")));
   if (listItem != NULL)  {
      listItem->SetOwner(this);
      listItem->SetIndex(GetCount());
   }
   return m_pList->Add(ctrl);
}

bool ListUI::Remove(ControlUI* ctrl)
{
   ASSERT(!"Not supported yet");
   return false; 
}

void ListUI::RemoveAll()
{
   m_curSel = -1;
   m_iExpandedItem = -1;
   m_pList->RemoveAll();
}

ControlUI* ListUI::GetItem(int idx) const
{
   return m_pList->GetItem(idx);
}

int ListUI::GetCount() const
{
   return m_pList->GetCount();
}

void ListUI::SetPos(RECT rc)
{
   VerticalLayoutUI::SetPos(rc);
   if (m_pHeader == NULL)  return;
   // Determine general list information and the size of header columns
   m_ListInfo.Text = UICOLOR_CONTROL_TEXT_NORMAL;
   m_ListInfo.Background = UICOLOR__INVALID;
   m_ListInfo.SelText = UICOLOR_CONTROL_TEXT_SELECTED;
   m_ListInfo.SelBackground = UICOLOR_CONTROL_BACKGROUND_SELECTED;
   m_ListInfo.HotText = UICOLOR_CONTROL_TEXT_NORMAL;
   m_ListInfo.HotBackground = UICOLOR_CONTROL_BACKGROUND_HOVER;   
   m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
   // The header/columns may or may not be visible at runtime. In either case
   // we should determine the correct dimensions...
   if (m_pHeader->IsVisible())  {
      for (int i = 0; i < m_ListInfo.nColumns; i++)  m_ListInfo.rcColumn[i] = m_pHeader->GetItem(i)->GetPos();
   }
   else {
      RECT rcCol = { rc.left, 0, rc.left, 0 };
      for (int i = 0; i < m_ListInfo.nColumns; i++)  {
         SIZE sz = m_pHeader->GetItem(i)->EstimateSize(CSize(rc.right - rc.left, rc.bottom - rc.top));
         rcCol.right += sz.cx;
         m_ListInfo.rcColumn[i] = rcCol;
         ::OffsetRect(&rcCol, sz.cx, 0);
      }
   }
}

void ListUI::Event(TEventUI& event)
{
   switch( event.Type)  {
   case UIEVENT_KEYDOWN:
      switch( event.chKey)  {
      case VK_UP:
         SelectItem(FindSelectable(m_curSel - 1, false));
         EnsureVisible(m_curSel);
         return;
      case VK_DOWN:
         SelectItem(FindSelectable(m_curSel + 1, true));
         EnsureVisible(m_curSel);
         return;
      case VK_PRIOR:
         SelectItem(FindSelectable(m_curSel - 10, false));
         EnsureVisible(m_curSel);
         return;
      case VK_NEXT:
         SelectItem(FindSelectable(m_curSel + 10, true));
         EnsureVisible(m_curSel);
         return;
      case VK_HOME:
         SelectItem(FindSelectable(0, false));
         EnsureVisible(m_curSel);
         return;
      case VK_END:
         SelectItem(FindSelectable(GetCount() - 1, true));
         EnsureVisible(m_curSel);
         return;
      case VK_RETURN:
         if (m_curSel != -1)  GetItem(m_curSel)->Activate();
         return;
      }
      break;
   case UIEVENT_SCROLLWHEEL:
      {
         switch( LOWORD(event.wParam))  {
         case SB_LINEUP:
            SelectItem(FindSelectable(m_curSel - 1, false));
            EnsureVisible(m_curSel);
            return;
         case SB_LINEDOWN:
            SelectItem(FindSelectable(m_curSel + 1, true));
            EnsureVisible(m_curSel);
            return;
         }
      }
      break;
   }
   ControlUI::Event(event);
}

ListHeaderUI* ListUI::GetHeader() const
{
   return m_pHeader;
}

ListFooterUI* ListUI::GetFooter() const
{
   return m_pFooter;
}

ContainerUI* ListUI::GetList() const
{
   return m_pList;
}

int ListUI::GetCurSel() const
{
   return m_curSel;
}

bool ListUI::SelectItem(int idx)
{
   if (idx == m_curSel)  return true;
   // We should first unselect the currently selected item
   if (m_curSel >= 0)  {
      ControlUI* ctrl = GetItem(m_curSel);
      if (ctrl != NULL)  {
         IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface(_T("ListItem")));
         if (listItem != NULL)  listItem->Select(false);
      }
   }
   // Now figure out if the control can be selected
   // TODO: Redo old selected if failure
   ControlUI* ctrl = GetItem(idx);
   if (ctrl == NULL)  return false;
   if (!ctrl->IsVisible())  return false;
   if (!ctrl->IsEnabled())  return false;
   IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface(_T("ListItem")));
   if (listItem == NULL)  return false;
   m_curSel = idx;
   if (!listItem->Select(true))  {
      m_curSel = -1;
      return false;
   }
   ctrl->SetFocus();
   if (m_manager != NULL)  {
      m_manager->SendNotify(ctrl, _T("itemclick"));
      m_manager->SendNotify(this, _T("itemselect"));
   }
   Invalidate();
   return true;
}

const TListInfoUI* ListUI::GetListInfo() const
{
   return &m_ListInfo;
}

void ListUI::SetExpanding(bool bExpandable)
{
   m_ListInfo.bExpandable = bExpandable;
}

bool ListUI::ExpandItem(int idx, bool bExpand /*= true*/)
{
   if (!m_ListInfo.bExpandable)  return false;
   if (m_iExpandedItem >= 0)  {
      ControlUI* ctrl = GetItem(m_iExpandedItem);
      if (ctrl != NULL)  {
         IListItemUI* item = static_cast<IListItemUI*>(ctrl->GetInterface(_T("ListItem")));
         if (item != NULL)  item->Expand(false);
      }
      m_iExpandedItem = -1;
   }
   if (bExpand)  {
      ControlUI* ctrl = GetItem(idx);
      if (ctrl == NULL)  return false;
      if (!ctrl->IsVisible())  return false;
      IListItemUI* item = static_cast<IListItemUI*>(ctrl->GetInterface(_T("ListItem")));
      if (item == NULL)  return false;
      m_iExpandedItem = idx;
      if (!item->Expand(true))  {
         m_iExpandedItem = -1;
         return false;
      }
   }
   UpdateLayout();
   return true;
}

int ListUI::GetExpandedItem() const
{
   return m_iExpandedItem;
}

void ListUI::EnsureVisible(int idx)
{
   if (m_curSel < 0)  return;
   RECT rcItem = m_pList->GetItem(idx)->GetPos();
   RECT rcList = m_pList->GetPos();
   int pos = m_pList->GetScrollPos();
   if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom)  return;
   int dx = 0;
   if (rcItem.top < rcList.top)  dx = rcItem.top - rcList.top;
   if (rcItem.bottom > rcList.bottom)  dx = rcItem.bottom - rcList.bottom;
   Scroll(0, dx);
}

void ListUI::Scroll(int dx, int dy)
{
   if (dx == 0 && dy == 0)  return;
   m_pList->SetScrollPos(m_pList->GetScrollPos() + dy);
}

void ListUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("header")) == 0)  GetHeader()->SetVisible(_tcscmp(value, _T("hidden")) != 0);
   else if (_tcscmp(name, _T("footer")) == 0)  GetFooter()->SetVisible(_tcscmp(value, _T("hidden")) != 0);
   else if (_tcscmp(name, _T("expanding")) == 0)  SetExpanding(_tcscmp(value, _T("true")) == 0);
   else VerticalLayoutUI::SetAttribute(name, value);
}

IListCallbackUI* ListUI::GetTextCallback() const
{
   return m_pCallback;
}

void ListUI::SetTextCallback(IListCallbackUI* pCallback)
{
   m_pCallback = pCallback;
}


CListLabelElementUI::CListLabelElementUI() : m_cxWidth(0), m_uTextStyle(DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS), m_uButtonState(0)
{
}

const TCHAR* CListLabelElementUI::GetClass() const
{
   return _T("ListLabelElementUI");
}

void CListLabelElementUI::SetWidth(int cx)
{
   m_cxWidth = cx;
   Invalidate();
}

void CListLabelElementUI::SetTextStyle(UINT uStyle)
{
   m_uTextStyle = uStyle;
   Invalidate();
}

void CListLabelElementUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled()) 
   {
      Select();
      Invalidate();
   }
   if (event.Type == UIEVENT_MOUSEENTER) 
   {
      m_uButtonState |= UISTATE_HOT;
      Invalidate();
   }
   if (event.Type == UIEVENT_MOUSELEAVE) 
   {
      if ((m_uButtonState & UISTATE_HOT) != 0)  {
         m_uButtonState &= ~UISTATE_HOT;
         Invalidate();
      }
   }
   ListElementUI::Event(event);
}

void CListLabelElementUI::SetAttribute(const TCHAR* name, const TCHAR* value)
{
   if (_tcscmp(name, _T("width")) == 0)  {
      SetWidth(_ttoi(value));
   }
   else if (_tcscmp(name, _T("align")) == 0)  {
      if (_tcsstr(value, _T("center")) != NULL)  m_uTextStyle |= DT_CENTER;
      if (_tcsstr(value, _T("right")) != NULL)  m_uTextStyle |= DT_RIGHT;
   }
   else ListElementUI::SetAttribute(name, value);
}

SIZE CListLabelElementUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 8);
}

void CListLabelElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, GetPos(), 0);
}

void CListLabelElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uDrawStyle)
{
   UITYPE_COLOR iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
   UITYPE_COLOR iBackColor = UICOLOR__INVALID;
   if ((m_uButtonState & UISTATE_HOT) != 0)  {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_HOVER;
   }
   if (m_bSelected)  {
      iTextColor = UICOLOR_CONTROL_TEXT_SELECTED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_SELECTED;
   }
   if ((uDrawStyle & UIDRAWSTYLE_INPLACE) != 0) 
   {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_NORMAL;
   }
   if ((uDrawStyle & UIDRAWSTYLE_FOCUS) != 0) 
   {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_HOVER;
   }
   if (!m_bEnabled)  {
      iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
   }
   // Paint background (because we're vertically centering the text area
   // so it cannot paint the entire item rectangle)
   if (iBackColor != UICOLOR__INVALID)  {
      CBlueRenderEngineUI::DoFillRect(hDC, m_manager, rcItem, iBackColor);
   }
   // Paint text
   RECT rcText = rcItem;
   ::InflateRect(&rcText, -4, 0);
   int nLinks = 0;
   CBlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcText, m_txt, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
}


CListTextElementUI::CListTextElementUI() : m_cyItem(0), m_nLinks(0), m_owner(NULL)
{
   ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

const TCHAR* CListTextElementUI::GetClass() const
{
   return _T("ListTextElementUI");
}

UINT CListTextElementUI::GetControlFlags() const
{
   return UIFLAG_WANTRETURN | (m_nLinks > 0 ? UIFLAG_SETCURSOR : 0);
}

void CListTextElementUI::SetOwner(ControlUI* owner)
{
   ListElementUI::SetOwner(owner);
   m_owner = static_cast<IListUI*>(owner->GetInterface("List"));
}

void CListTextElementUI::Event(TEventUI& event)
{
   // When you hover over a link
   if (event.Type == UIEVENT_SETCURSOR)  {
      for (int i = 0; i < m_nLinks; i++)  {
         if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
            return;
         }
      }      
   }
   if (m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE)  Invalidate();
   if (m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE)  Invalidate();
   CListLabelElementUI::Event(event);
}

SIZE CListTextElementUI::EstimateSize(SIZE szAvailable)
{
   if (m_owner == NULL)  return CSize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if (m_cyItem == 0)  {
      const TCHAR* txt = _T("XXX");
      RECT rcText = { 0, 0, 9999, 9999 };
      int nLinks = 0;
      CBlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, txt, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
      m_cyItem = rcText.bottom - rcText.top;
   }
   return CSize(m_cxWidth, m_cyItem + 9);
}

void CListTextElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, m_rcItem, 0);
}

void CListTextElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
{
   ASSERT(m_owner);
   if (m_owner == NULL)  return;
   const TListInfoUI* pInfo = m_owner->GetListInfo();
   UITYPE_COLOR iTextColor = pInfo->Text;
   UITYPE_COLOR iBackColor = pInfo->Background;
   if ((m_uButtonState & UISTATE_HOT) != 0)  {
      iBackColor = pInfo->HotBackground;
      iTextColor = pInfo->HotText;
   }
   if (IsSelected())  {
      iTextColor = pInfo->SelText;
      iBackColor = pInfo->SelBackground;
   }
   if (!IsEnabled())  {
      iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
   }
   if (iBackColor != UICOLOR__INVALID)  {
      CBlueRenderEngineUI::DoFillRect(hDC, m_manager, m_rcItem, iBackColor);
   }
   IListCallbackUI* pCallback = m_owner->GetTextCallback();
   ASSERT(pCallback);
   if (pCallback == NULL)  return;
   m_nLinks = 0;
   int nLinks = lengthof(m_rcLinks);
   for (int i = 0; i < pInfo->nColumns; i++) 
   {
      // Paint text
      RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom - 1 };
      const TCHAR* txt = pCallback->GetItemText(this, m_idx, i);
      ::InflateRect(&rcItem, -4, 0);
      CBlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcItem, txt, iTextColor, UICOLOR__INVALID, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if (nLinks > 0)  m_nLinks = nLinks, nLinks = 0; else nLinks = lengthof(m_rcLinks);
   }
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_DIALOG_BACKGROUND);
}


CListExpandElementUI::CListExpandElementUI() : m_bExpanded(false), m_cyExpanded(0), m_pContainer(NULL)
{
}

CListExpandElementUI::~CListExpandElementUI()
{
   delete m_pContainer;
}

const TCHAR* CListExpandElementUI::GetClass() const
{
   return _T("ListExpandElementUI");
}

bool CListExpandElementUI::Expand(bool bExpand)
{
   ASSERT(m_owner);
   if (m_owner == NULL)  return false;  
   if (bExpand == m_bExpanded)  return true;
   m_bExpanded = bExpand;
   if (!m_owner->ExpandItem(m_idx, bExpand))  return false;
   // We need to manage the "expanding items", which are actually embedded controls
   // that we selectively display or not.
   if (bExpand)  
   {
      delete m_pContainer;
      TileLayoutUI* pTile = new TileLayoutUI;
      pTile->SetPadding(4);
      m_pContainer = pTile;
      if (m_manager != NULL)  m_manager->SendNotify(this, _T("itemexpand"));
      m_manager->InitControls(m_pContainer, this);
   }
   else
   {
      if (m_manager != NULL)  m_manager->SendNotify(this, _T("itemcollapse"));
   }
   m_cyExpanded = 0;
   return true;
}

bool CListExpandElementUI::IsExpanded() const
{
   return m_bExpanded;
}

void CListExpandElementUI::Event(TEventUI& event)
{
   if (event.Type == UIEVENT_BUTTONUP) 
   {
      if (m_owner == NULL)  return;
      const TListInfoUI* pInfo = m_owner->GetListInfo();
      RECT rcExpander = { m_rcItem.left, m_rcItem.top, m_rcItem.left + 20, m_rcItem.bottom };;
      if (pInfo->bExpandable && ::PtInRect(&rcExpander, event.ptMouse))  Expand(!m_bExpanded);
   }
   if (event.Type == UIEVENT_KEYDOWN) 
   {
      switch( event.chKey)  {
      case VK_LEFT:
         Expand(false);
         return;
      case VK_RIGHT:
         Expand(true);
         return;
      }
   }
   CListTextElementUI::Event(event);
}

SIZE CListExpandElementUI::EstimateSize(SIZE szAvailable)
{
   if (m_owner == NULL)  return CSize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if (m_cyItem == 0)  {
      const TCHAR* txt = _T("XXX");
      RECT rcText = { 0, 0, 9999, 9999 };
      int nLinks = 0;
      CBlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, txt, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
      m_cyItem = (rcText.bottom - rcText.top) + 8;
   }
   int cyItem = m_cyItem;
   if (m_bExpanded && m_cyExpanded == 0 && m_pContainer != NULL && m_pContainer->GetCount() > 0)  {
      RECT rcItem = m_pContainer->GetItem(0)->GetPos();
      // HACK: Here we boldy assume that the item height is the same as the listitem.
      m_cyExpanded = ((rcItem.bottom - rcItem.top) + 14) * ((m_pContainer->GetCount() + 1) / 2);
      m_cyExpanded += 4 * ((m_pContainer->GetCount() - 1) / 2);
      m_cyExpanded += 26;
   }
   return CSize(m_cxWidth, cyItem + m_cyExpanded + 1);
}

void CListExpandElementUI::SetPos(RECT rc)
{
   if (m_pContainer != NULL)  {
      RECT rcSubItems = { rc.left + 14, rc.top + m_cyItem, rc.right - 8, rc.bottom - 6 };
      m_pContainer->SetPos(rcSubItems);
   }
   CListTextElementUI::SetPos(rc);
}

bool CListExpandElementUI::Add(ControlUI* ctrl)
{
   ASSERT(m_pContainer);
   if (m_pContainer == NULL)  return false;
   return m_pContainer->Add(ctrl);
}

bool CListExpandElementUI::Remove(ControlUI* ctrl)
{
   ASSERT(!"Not supported yet");
   return false; 
}

void CListExpandElementUI::RemoveAll()
{
   if (m_pContainer != NULL)  m_pContainer->RemoveAll();
}

ControlUI* CListExpandElementUI::GetItem(int idx) const
{
   if (m_pContainer == NULL)  return NULL;
   return m_pContainer->GetItem(idx);
}

int CListExpandElementUI::GetCount() const
{
   if (m_pContainer == NULL)  return 0;
   return m_pContainer->GetCount();
}

void CListExpandElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   CListTextElementUI::DoPaint(hDC, rcPaint);
   // Paint the expanded items?
   if (m_bExpanded && m_pContainer != NULL)  {
      // Paint gradient box for the items
      RECT rcFrame = m_pContainer->GetPos();
      COLORREF clr1, clr2;
      m_manager->GetThemeColorPair(UICOLOR_CONTROL_BACKGROUND_EXPANDED, clr1, clr2);
      CBlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcFrame, clr1, clr2, true, 64);
      CBlueRenderEngineUI::DoPaintRectangle(hDC, m_manager, rcFrame, UICOLOR_HEADER_BORDER, UICOLOR__INVALID);
      RECT rcLine = { m_rcItem.left, rcFrame.top, m_rcItem.right, rcFrame.top };
      CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_STANDARD_BLACK);
      // We'll draw the items then...
      m_pContainer->DoPaint(hDC, rcPaint);
   }
}

void CListExpandElementUI::SetManager(PaintManagerUI* manager, ControlUI* parent)
{
   if (m_pContainer != NULL)  m_pContainer->SetManager(manager, parent);
   CListTextElementUI::SetManager(manager, parent);
}

ControlUI* CListExpandElementUI::FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags)
{
   ControlUI* pResult = NULL;
   if (m_pContainer != NULL)  pResult = m_pContainer->FindControl(Proc, data, uFlags);
   if (pResult == NULL)  pResult = CListTextElementUI::FindControl(Proc, data, uFlags);
   return pResult;
}

void CListExpandElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
{
   ASSERT(m_owner);
   if (m_owner == NULL)  return;
   const TListInfoUI* pInfo = m_owner->GetListInfo();
   UITYPE_COLOR iTextColor = pInfo->Text;
   UITYPE_COLOR iBackColor = pInfo->Background;
   if ((m_uButtonState & UISTATE_HOT) != 0)  {
      iBackColor = pInfo->HotBackground;
      iTextColor = pInfo->HotText;
   }
   if (IsSelected())  {
      iTextColor = pInfo->SelText;
      iBackColor = pInfo->SelBackground;
   }
   if (!IsEnabled())  {
      iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
   }
   if (iBackColor != UICOLOR__INVALID)  {
      RECT rcItem = { m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + m_cyItem };
      CBlueRenderEngineUI::DoFillRect(hDC, m_manager, rcItem, iBackColor);
   }
   IListCallbackUI* pCallback = m_owner->GetTextCallback();
   ASSERT(pCallback);
   if (pCallback == NULL)  return;
   m_nLinks = 0;
   CStdString sColText;
   int nLinks = lengthof(m_rcLinks);
   for (int i = 0; i < pInfo->nColumns; i++) 
   {
      // Paint text
      RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.top + m_cyItem };
      const TCHAR* txt = pCallback->GetItemText(this, m_idx, i);
      // If list control is expandable then we'll automaticially draw
      // the expander-box at the first column.
      if (i == 0 && pInfo->bExpandable)  {
         sColText = (m_bExpanded ? _T("<i 14> ") : _T("<i 13> "));
         sColText += txt;
         txt = sColText;
      }
      ::InflateRect(&rcItem, -4, 0);
      CBlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcItem, txt, iTextColor, UICOLOR__INVALID, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if (nLinks > 0)  m_nLinks = nLinks, nLinks = 0; else nLinks = lengthof(m_rcLinks);
   }
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
   CBlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_DIALOG_BACKGROUND);
}
