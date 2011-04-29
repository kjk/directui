
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

void* ListElementUI::GetInterface(const char* name)
{
   if (str::Eq(name, "ListItem"))  return static_cast<IListItemUI*>(this);
   return ControlUI::GetInterface(name);
}

void ListElementUI::SetOwner(ControlUI* owner)
{
   m_owner = static_cast<IListOwnerUI*>(owner->GetInterface("ListOwner"));
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
   if (m_manager != NULL)  m_manager->SendNotify(this, "itemactivate");
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
   if (event.type == UIEVENT_DBLCLICK && IsEnabled()) 
   {
      Activate();
      Invalidate();
      return;
   }
   if (event.type == UIEVENT_KEYDOWN && IsEnabled()) 
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

void ListElementUI::SetAttribute(const char* name, const char* value)
{
   if (str::Eq(name, "selected"))  Select();
   else ControlUI::SetAttribute(name, value);
}

ListHeaderUI::ListHeaderUI()
{
   SetInset(CSize(0, 0));
}

const char* ListHeaderUI::GetClass() const
{
   return "ListHeaderUI";
}

void* ListHeaderUI::GetInterface(const char* name)
{
   if (str::Eq(name, "ListHeader"))  return this;
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
   BlueRenderEngineUI::DoPaintFrame(hDC, m_manager, m_rcItem, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BACKGROUND, 0);
   RECT rcBottom = { m_rcItem.left + 1, m_rcItem.bottom - 3, m_rcItem.right - 1, m_rcItem.bottom };
   BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcBottom, clrBack1, clrBack2, true, 4);
   // Draw headers too...
   HorizontalLayoutUI::DoPaint(hDC, rcPaint);
}


ListHeaderItemUI::ListHeaderItemUI() : m_uDragState(0)
{
}

const char* ListHeaderItemUI::GetClass() const
{
   return "ListHeaderItemUI";
}

void ListHeaderItemUI::SetText(const char* txt)
{
   str::Replace(m_txt, txt);
   UpdateLayout();
}

void ListHeaderItemUI::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void ListHeaderItemUI::SetAttribute(const char* name, const char* value)
{
   int n;
   if (ParseWidth(name, value, n))
      SetWidth(n);
   else ControlUI::SetAttribute(name, value);
}

UINT ListHeaderItemUI::GetControlFlags() const
{
   return UIFLAG_SETCURSOR;
}

void ListHeaderItemUI::Event(TEventUI& event)
{
   if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
   {
      RECT rcSeparator = GetThumbRect(m_rcItem);
      if (::PtInRect(&rcSeparator, event.ptMouse))  {
         m_uDragState |= UISTATE_CAPTURED;
         m_ptLastMouse = event.ptMouse;
         m_manager->SendNotify(this, "headerdragging");
      }
      else {
         m_manager->SendNotify(this, "headerclick");
      }
   }
   if (event.type == UIEVENT_BUTTONUP) 
   {
      if ((m_uDragState & UISTATE_CAPTURED) != 0)  {
         m_uDragState &= ~UISTATE_CAPTURED;
         m_manager->SendNotify(this, "headerdragged");
         m_manager->UpdateLayout();
      }
   }
   if (event.type == UIEVENT_MOUSEMOVE) 
   {
      if ((m_uDragState & UISTATE_CAPTURED) != 0)  {
         RECT rc = m_rcItem;
         rc.right -= m_ptLastMouse.x - event.ptMouse.x;
         const int MIN_DRAGSIZE = 40;
         if (rc.right - rc.left > MIN_DRAGSIZE)  {
            m_rcItem = rc;
            m_cxWidth = rc.right - rc.left;
            m_ptLastMouse = event.ptMouse;
            m_parent->Invalidate();
         }
      }
   }
   if (event.type == UIEVENT_SETCURSOR) 
   {
      RECT rcSeparator = GetThumbRect(m_rcItem);
      if (IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse))  {
         ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
         return;
      }
   }
   ControlUI::Event(event);
}

SIZE ListHeaderItemUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, 14 + m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void ListHeaderItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Paint text (with some indent)
   RECT rcMessage = m_rcItem;
   rcMessage.left += 6;
   rcMessage.bottom -= 1;
   int nLinks = 0;
   BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcMessage, m_txt, UICOLOR_HEADER_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
   // Draw gripper
   POINT ptTemp = { 0 };
   RECT rcThumb = GetThumbRect(m_rcItem);
   RECT rc1 = { rcThumb.left + 2, rcThumb.top + 4, rcThumb.left + 2, rcThumb.bottom - 1 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc1, UICOLOR_HEADER_SEPARATOR);
   RECT rc2 = { rcThumb.left + 3, rcThumb.top + 4, rcThumb.left + 3, rcThumb.bottom - 1 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rc2, UICOLOR_STANDARD_WHITE);
}

RECT ListHeaderItemUI::GetThumbRect(RECT rc) const
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

const char* ListFooterUI::GetClass() const
{
   return "ListFooterUI";
}

void* ListFooterUI::GetInterface(const char* name)
{
   if (str::Eq(name, "ListFooter"))  return this;
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
   BlueRenderEngineUI::DoPaintFrame(hDC, m_manager, m_rcItem, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BACKGROUND, 0);
   RECT rcTop = { m_rcItem.left + 1, m_rcItem.top, m_rcItem.right - 1, m_rcItem.top + 1 };
   BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcTop, clrBack2, clrBack1, true, 2);
   RECT rcBottom = { m_rcItem.left + 1, m_rcItem.bottom - 3, m_rcItem.right - 1, m_rcItem.bottom };
   BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcBottom, clrBack1, clrBack2, true, 4);
   // Paint items as well...
   HorizontalLayoutUI::DoPaint(hDC, rcPaint);
}


ListUI::ListUI() : m_cb(NULL), m_curSel(-1), m_expandedItem(-1)
{
   m_list = new VerticalLayoutUI;
   m_header = new ListHeaderUI;
   m_footer = new ListFooterUI;

   WhiteCanvasUI* pWhite = new WhiteCanvasUI;
   pWhite->Add(m_list);

   m_footer->Add(new LabelPanelUI);
   m_footer->Add(new PaddingPanelUI);

   Add(m_header);
   VerticalLayoutUI::Add(pWhite);
   Add(m_footer);

   m_list->EnableScrollBar();

   ::ZeroMemory(&m_listInfo, sizeof(TListInfoUI));
}

const char* ListUI::GetClass() const
{
   return "ListUI";
}

UINT ListUI::GetControlFlags() const
{
   return UIFLAG_TABSTOP;
}

void* ListUI::GetInterface(const char* name)
{
   if (str::Eq(name, "List"))  return static_cast<IListUI*>(this);
   if (str::Eq(name, "ListOwner"))  return static_cast<IListOwnerUI*>(this);
   return VerticalLayoutUI::GetInterface(name);
}

bool ListUI::Add(ControlUI* ctrl)
{
   // Override the Add() method so we can add items specifically to
   // the intended widgets. Headers and footers are assumed to be
   // answer the correct interface so we can add multiple list headers/footers.
   if (ctrl->GetInterface("ListHeader") != NULL)  return VerticalLayoutUI::Add(ctrl);
   if (ctrl->GetInterface("ListFooter") != NULL)  return VerticalLayoutUI::Add(ctrl);
   // We also need to recognize header sub-items
   if (str::Find(ctrl->GetClass(), "Header") != NULL)  return m_header->Add(ctrl);
   if (str::Find(ctrl->GetClass(), "Footer") != NULL)  return m_footer->Add(ctrl);
   // The list items should know about us
   IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
   if (listItem != NULL)  {
      listItem->SetOwner(this);
      listItem->SetIndex(GetCount());
   }
   return m_list->Add(ctrl);
}

bool ListUI::Remove(ControlUI* ctrl)
{
   ASSERT(!"Not supported yet");
   return false; 
}

void ListUI::RemoveAll()
{
   m_curSel = -1;
   m_expandedItem = -1;
   m_list->RemoveAll();
}

ControlUI* ListUI::GetItem(int idx) const
{
   return m_list->GetItem(idx);
}

int ListUI::GetCount() const
{
   return m_list->GetCount();
}

void ListUI::SetPos(RECT rc)
{
   VerticalLayoutUI::SetPos(rc);
   if (m_header == NULL)  return;
   // Determine general list information and the size of header columns
   m_listInfo.Text = UICOLOR_CONTROL_TEXT_NORMAL;
   m_listInfo.Background = UICOLOR__INVALID;
   m_listInfo.SelText = UICOLOR_CONTROL_TEXT_SELECTED;
   m_listInfo.SelBackground = UICOLOR_CONTROL_BACKGROUND_SELECTED;
   m_listInfo.HotText = UICOLOR_CONTROL_TEXT_NORMAL;
   m_listInfo.HotBackground = UICOLOR_CONTROL_BACKGROUND_HOVER;   
   m_listInfo.nColumns = MIN(m_header->GetCount(), UILIST_MAX_COLUMNS);
   // The header/columns may or may not be visible at runtime. In either case
   // we should determine the correct dimensions...
   if (m_header->IsVisible())  {
      for (int i = 0; i < m_listInfo.nColumns; i++)  m_listInfo.rcColumn[i] = m_header->GetItem(i)->GetPos();
   }
   else {
      RECT rcCol = { rc.left, 0, rc.left, 0 };
      for (int i = 0; i < m_listInfo.nColumns; i++)  {
         SIZE sz = m_header->GetItem(i)->EstimateSize(CSize(rc.right - rc.left, rc.bottom - rc.top));
         rcCol.right += sz.cx;
         m_listInfo.rcColumn[i] = rcCol;
         ::OffsetRect(&rcCol, sz.cx, 0);
      }
   }
}

void ListUI::Event(TEventUI& event)
{
   switch (event.type)  {
   case UIEVENT_KEYDOWN:
      switch (event.chKey)  {
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
         switch (LOWORD(event.wParam))  {
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
   return m_header;
}

ListFooterUI* ListUI::GetFooter() const
{
   return m_footer;
}

ContainerUI* ListUI::GetList() const
{
   return m_list;
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
         IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
         if (listItem != NULL)  listItem->Select(false);
      }
   }
   // Now figure out if the control can be selected
   // TODO: Redo old selected if failure
   ControlUI* ctrl = GetItem(idx);
   if (ctrl == NULL)  return false;
   if (!ctrl->IsVisible())  return false;
   if (!ctrl->IsEnabled())  return false;
   IListItemUI* listItem = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
   if (listItem == NULL)  return false;
   m_curSel = idx;
   if (!listItem->Select(true))  {
      m_curSel = -1;
      return false;
   }
   ctrl->SetFocus();
   if (m_manager != NULL)  {
      m_manager->SendNotify(ctrl, "itemclick");
      m_manager->SendNotify(this, "itemselect");
   }
   Invalidate();
   return true;
}

const TListInfoUI* ListUI::GetListInfo() const
{
   return &m_listInfo;
}

void ListUI::SetExpanding(bool expandable)
{
   m_listInfo.expandable = expandable;
}

bool ListUI::ExpandItem(int idx, bool bExpand /*= true*/)
{
   if (!m_listInfo.expandable)  return false;
   if (m_expandedItem >= 0)  {
      ControlUI* ctrl = GetItem(m_expandedItem);
      if (ctrl != NULL)  {
         IListItemUI* item = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
         if (item != NULL)  item->Expand(false);
      }
      m_expandedItem = -1;
   }
   if (bExpand)  {
      ControlUI* ctrl = GetItem(idx);
      if (ctrl == NULL)  return false;
      if (!ctrl->IsVisible())  return false;
      IListItemUI* item = static_cast<IListItemUI*>(ctrl->GetInterface("ListItem"));
      if (item == NULL)  return false;
      m_expandedItem = idx;
      if (!item->Expand(true))  {
         m_expandedItem = -1;
         return false;
      }
   }
   UpdateLayout();
   return true;
}

int ListUI::GetExpandedItem() const
{
   return m_expandedItem;
}

void ListUI::EnsureVisible(int idx)
{
   if (m_curSel < 0)  return;
   RECT rcItem = m_list->GetItem(idx)->GetPos();
   RECT rcList = m_list->GetPos();
   int pos = m_list->GetScrollPos();
   if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom)  return;
   int dx = 0;
   if (rcItem.top < rcList.top)  dx = rcItem.top - rcList.top;
   if (rcItem.bottom > rcList.bottom)  dx = rcItem.bottom - rcList.bottom;
   Scroll(0, dx);
}

void ListUI::Scroll(int dx, int dy)
{
   if (dx == 0 && dy == 0)  return;
   m_list->SetScrollPos(m_list->GetScrollPos() + dy);
}

void ListUI::SetAttribute(const char* name, const char* value)
{
   if (str::Eq(name, "header"))  GetHeader()->SetVisible(!str::Eq(value, "hidden"));
   else if (str::Eq(name, "footer"))  GetFooter()->SetVisible(!str::Eq(value, "hidden"));
   else if (str::Eq(name, "expanding"))  SetExpanding(str::Eq(value, "true"));
   else VerticalLayoutUI::SetAttribute(name, value);
}

IListCallbackUI* ListUI::GetTextCallback() const
{
   return m_cb;
}

void ListUI::SetTextCallback(IListCallbackUI* cb)
{
   m_cb = cb;
}


ListLabelElementUI::ListLabelElementUI() : m_cxWidth(0), m_uTextStyle(DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS), m_uButtonState(0)
{
}

const char* ListLabelElementUI::GetClass() const
{
   return "ListLabelElementUI";
}

void ListLabelElementUI::SetWidth(int cx)
{
   m_cxWidth = cx;
   Invalidate();
}

void ListLabelElementUI::SetTextStyle(UINT uStyle)
{
   m_uTextStyle = uStyle;
   Invalidate();
}

void ListLabelElementUI::Event(TEventUI& event)
{
   if (event.type == UIEVENT_BUTTONDOWN && IsEnabled()) 
   {
      Select();
      Invalidate();
   }
   if (event.type == UIEVENT_MOUSEENTER) 
   {
      m_uButtonState |= UISTATE_HOT;
      Invalidate();
   }
   if (event.type == UIEVENT_MOUSELEAVE) 
   {
      if ((m_uButtonState & UISTATE_HOT) != 0)  {
         m_uButtonState &= ~UISTATE_HOT;
         Invalidate();
      }
   }
   ListElementUI::Event(event);
}

void ListLabelElementUI::SetAttribute(const char* name, const char* value)
{
   int n;
   if (ParseWidth(name, value, n)) {
      SetWidth(n);
   } else if (str::Eq(name, "align"))  {
      if (str::Find(value, "center") != NULL)  m_uTextStyle |= DT_CENTER;
      if (str::Find(value, "right") != NULL)  m_uTextStyle |= DT_RIGHT;
   }
   else ListElementUI::SetAttribute(name, value);
}

SIZE ListLabelElementUI::EstimateSize(SIZE /*szAvailable*/)
{
   return CSize(m_cxWidth, m_manager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 8);
}

void ListLabelElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, GetPos(), 0);
}

void ListLabelElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uDrawStyle)
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
   if (!m_enabled)  {
      iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
   }
   // Paint background (because we're vertically centering the text area
   // so it cannot paint the entire item rectangle)
   if (iBackColor != UICOLOR__INVALID)  {
      BlueRenderEngineUI::DoFillRect(hDC, m_manager, rcItem, iBackColor);
   }
   // Paint text
   RECT rcText = rcItem;
   ::InflateRect(&rcText, -4, 0);
   int nLinks = 0;
   BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcText, m_txt, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
}


ListTextElementUI::ListTextElementUI() : m_cyItem(0), m_nLinks(0), m_owner(NULL)
{
   ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

const char* ListTextElementUI::GetClass() const
{
   return "ListTextElementUI";
}

UINT ListTextElementUI::GetControlFlags() const
{
   return UIFLAG_WANTRETURN | (m_nLinks > 0 ? UIFLAG_SETCURSOR : 0);
}

void ListTextElementUI::SetOwner(ControlUI* owner)
{
   ListElementUI::SetOwner(owner);
   m_owner = static_cast<IListUI*>(owner->GetInterface("List"));
}

void ListTextElementUI::Event(TEventUI& event)
{
   // When you hover over a link
   if (event.type == UIEVENT_SETCURSOR)  {
      for (int i = 0; i < m_nLinks; i++)  {
         if (::PtInRect(&m_rcLinks[i], event.ptMouse))  {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
            return;
         }
      }      
   }
   if (m_nLinks > 0 && event.type == UIEVENT_MOUSEMOVE)  Invalidate();
   if (m_nLinks > 0 && event.type == UIEVENT_MOUSELEAVE)  Invalidate();
   ListLabelElementUI::Event(event);
}

SIZE ListTextElementUI::EstimateSize(SIZE szAvailable)
{
   if (m_owner == NULL)  return CSize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if (m_cyItem == 0)  {
      const char* txt = "XXX";
      RECT rcText = { 0, 0, 9999, 9999 };
      int nLinks = 0;
      BlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, txt, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
      m_cyItem = rcText.bottom - rcText.top;
   }
   return CSize(m_cxWidth, m_cyItem + 9);
}

void ListTextElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, m_rcItem, 0);
}

void ListTextElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
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
      BlueRenderEngineUI::DoFillRect(hDC, m_manager, m_rcItem, iBackColor);
   }
   IListCallbackUI* cb = m_owner->GetTextCallback();
   ASSERT(cb);
   if (cb == NULL)  return;
   m_nLinks = 0;
   int nLinks = dimof(m_rcLinks);
   for (int i = 0; i < pInfo->nColumns; i++) 
   {
      // Paint text
      RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom - 1 };
      const char* txt = cb->GetItemText(this, m_idx, i);
      ::InflateRect(&rcItem, -4, 0);
      BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcItem, txt, iTextColor, UICOLOR__INVALID, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if (nLinks > 0)  m_nLinks = nLinks, nLinks = 0; else nLinks = dimof(m_rcLinks);
   }
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_DIALOG_BACKGROUND);
}


ListExpandElementUI::ListExpandElementUI() : m_bExpanded(false), m_cyExpanded(0), m_container(NULL)
{
}

ListExpandElementUI::~ListExpandElementUI()
{
   delete m_container;
}

const char* ListExpandElementUI::GetClass() const
{
   return "ListExpandElementUI";
}

bool ListExpandElementUI::Expand(bool bExpand)
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
      delete m_container;
      TileLayoutUI* pTile = new TileLayoutUI;
      pTile->SetPadding(4);
      m_container = pTile;
      if (m_manager != NULL)  m_manager->SendNotify(this, "itemexpand");
      m_manager->InitControls(m_container, this);
   }
   else
   {
      if (m_manager != NULL)  m_manager->SendNotify(this, "itemcollapse");
   }
   m_cyExpanded = 0;
   return true;
}

bool ListExpandElementUI::IsExpanded() const
{
   return m_bExpanded;
}

void ListExpandElementUI::Event(TEventUI& event)
{
   if (event.type == UIEVENT_BUTTONUP) 
   {
      if (m_owner == NULL)  return;
      const TListInfoUI* pInfo = m_owner->GetListInfo();
      RECT rcExpander = { m_rcItem.left, m_rcItem.top, m_rcItem.left + 20, m_rcItem.bottom };;
      if (pInfo->expandable && ::PtInRect(&rcExpander, event.ptMouse))  Expand(!m_bExpanded);
   }
   if (event.type == UIEVENT_KEYDOWN) 
   {
      switch (event.chKey)  {
      case VK_LEFT:
         Expand(false);
         return;
      case VK_RIGHT:
         Expand(true);
         return;
      }
   }
   ListTextElementUI::Event(event);
}

SIZE ListExpandElementUI::EstimateSize(SIZE szAvailable)
{
   if (m_owner == NULL)  return CSize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if (m_cyItem == 0)  {
      const char* txt = "XXX";
      RECT rcText = { 0, 0, 9999, 9999 };
      int nLinks = 0;
      BlueRenderEngineUI::DoPaintPrettyText(m_manager->GetPaintDC(), m_manager, rcText, txt, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
      m_cyItem = (rcText.bottom - rcText.top) + 8;
   }
   int cyItem = m_cyItem;
   if (m_bExpanded && m_cyExpanded == 0 && m_container != NULL && m_container->GetCount() > 0)  {
      RECT rcItem = m_container->GetItem(0)->GetPos();
      // HACK: Here we boldy assume that the item height is the same as the listitem.
      m_cyExpanded = ((rcItem.bottom - rcItem.top) + 14) * ((m_container->GetCount() + 1) / 2);
      m_cyExpanded += 4 * ((m_container->GetCount() - 1) / 2);
      m_cyExpanded += 26;
   }
   return CSize(m_cxWidth, cyItem + m_cyExpanded + 1);
}

void ListExpandElementUI::SetPos(RECT rc)
{
   if (m_container != NULL)  {
      RECT rcSubItems = { rc.left + 14, rc.top + m_cyItem, rc.right - 8, rc.bottom - 6 };
      m_container->SetPos(rcSubItems);
   }
   ListTextElementUI::SetPos(rc);
}

bool ListExpandElementUI::Add(ControlUI* ctrl)
{
   ASSERT(m_container);
   if (m_container == NULL)  return false;
   return m_container->Add(ctrl);
}

bool ListExpandElementUI::Remove(ControlUI* ctrl)
{
   ASSERT(!"Not supported yet");
   return false; 
}

void ListExpandElementUI::RemoveAll()
{
   if (m_container != NULL)  m_container->RemoveAll();
}

ControlUI* ListExpandElementUI::GetItem(int idx) const
{
   if (m_container == NULL)  return NULL;
   return m_container->GetItem(idx);
}

int ListExpandElementUI::GetCount() const
{
   if (m_container == NULL)  return 0;
   return m_container->GetCount();
}

void ListExpandElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   ListTextElementUI::DoPaint(hDC, rcPaint);
   // Paint the expanded items?
   if (m_bExpanded && m_container != NULL)  {
      // Paint gradient box for the items
      RECT rcFrame = m_container->GetPos();
      COLORREF clr1, clr2;
      m_manager->GetThemeColorPair(UICOLOR_CONTROL_BACKGROUND_EXPANDED, clr1, clr2);
      BlueRenderEngineUI::DoPaintGradient(hDC, m_manager, rcFrame, clr1, clr2, true, 64);
      BlueRenderEngineUI::DoPaintRectangle(hDC, m_manager, rcFrame, UICOLOR_HEADER_BORDER, UICOLOR__INVALID);
      RECT rcLine = { m_rcItem.left, rcFrame.top, m_rcItem.right, rcFrame.top };
      BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_STANDARD_BLACK);
      // We'll draw the items then...
      m_container->DoPaint(hDC, rcPaint);
   }
}

void ListExpandElementUI::SetManager(PaintManagerUI* manager, ControlUI* parent)
{
   if (m_container != NULL)  m_container->SetManager(manager, parent);
   ListTextElementUI::SetManager(manager, parent);
}

ControlUI* ListExpandElementUI::FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags)
{
   ControlUI* pResult = NULL;
   if (m_container != NULL)  pResult = m_container->FindControl(Proc, data, uFlags);
   if (pResult == NULL)  pResult = ListTextElementUI::FindControl(Proc, data, uFlags);
   return pResult;
}

void ListExpandElementUI::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
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
      BlueRenderEngineUI::DoFillRect(hDC, m_manager, rcItem, iBackColor);
   }
   IListCallbackUI* cb = m_owner->GetTextCallback();
   ASSERT(cb);
   if (cb == NULL)  return;
   m_nLinks = 0;
   StdString sColText;
   int nLinks = dimof(m_rcLinks);
   for (int i = 0; i < pInfo->nColumns; i++) 
   {
      // Paint text
      RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.top + m_cyItem };
      const char* txt = cb->GetItemText(this, m_idx, i);
      // If list control is expandable then we'll automaticially draw
      // the expander-box at the first column.
      if (i == 0 && pInfo->expandable)  {
         sColText = (m_bExpanded ? "<i 14> " : "<i 13> ");
         sColText += txt;
         txt = sColText;
      }
      ::InflateRect(&rcItem, -4, 0);
      BlueRenderEngineUI::DoPaintPrettyText(hDC, m_manager, rcItem, txt, iTextColor, UICOLOR__INVALID, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if (nLinks > 0)  m_nLinks = nLinks, nLinks = 0; else nLinks = dimof(m_rcLinks);
   }
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
   BlueRenderEngineUI::DoPaintLine(hDC, m_manager, rcLine, UICOLOR_DIALOG_BACKGROUND);
}
