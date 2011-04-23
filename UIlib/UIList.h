#if !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
#define AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_

class ListHeaderUI;
class ListFooterUI;

#define UILIST_MAX_COLUMNS 10

typedef struct tagTListInfoUI
{
   int nColumns;
   RECT rcColumn[UILIST_MAX_COLUMNS];
   UITYPE_COLOR Text;
   UITYPE_COLOR Background;
   UITYPE_COLOR SelText;
   UITYPE_COLOR SelBackground;
   UITYPE_COLOR HotText;
   UITYPE_COLOR HotBackground;
   bool expandable;
} TListInfoUI;

class IListCallbackUI
{
public:
   virtual const TCHAR* GetItemText(ControlUI* pList, int iItem, int iSubItem) = 0;
   virtual int CompareItem(ControlUI* pList, ControlUI* item1, ControlUI* item2) = 0;
};

class IListOwnerUI
{
public:
   virtual int GetCurSel() const = 0;
   virtual bool SelectItem(int idx) = 0;
   virtual void Event(TEventUI& event) = 0;
};

class IListUI : public IListOwnerUI
{
public:
   virtual ListHeaderUI* GetHeader() const = 0;
   virtual ListFooterUI* GetFooter() const = 0; 
   virtual ContainerUI* GetList() const = 0;
   virtual const TListInfoUI* GetListInfo() const = 0;
   virtual IListCallbackUI* GetTextCallback() const = 0;
   virtual void SetTextCallback(IListCallbackUI* cb) = 0;
   virtual bool ExpandItem(int idx, bool bExpand = true) = 0;
   virtual int GetExpandedItem() const = 0;
};

class IListItemUI
{
public:
   virtual int GetIndex() const = 0;
   virtual void SetIndex(int idx) = 0;
   virtual void SetOwner(ControlUI* owner) = 0;
   virtual bool IsSelected() const = 0;
   virtual bool Select(bool bSelect = true) = 0;
   virtual bool IsExpanded() const = 0;
   virtual bool Expand(bool bExpand = true) = 0;
   virtual void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle) = 0;
};

class UILIB_API ListElementUI : public ControlUI, public IListItemUI
{
public:
   ListElementUI();

   UINT GetControlFlags() const;
   void* GetInterface(const TCHAR* name);

   int GetIndex() const;
   void SetIndex(int idx);

   void SetOwner(ControlUI* owner);

   bool IsSelected() const;
   bool Select(bool bSelect = true);
   bool IsExpanded() const;
   bool Expand(bool bExpand = true);

   bool Activate();

   void Event(TEventUI& event);
   void SetAttribute(const char* name, const char* value);

protected:
   int m_idx;
   bool m_bSelected;
   IListOwnerUI* m_owner;
};

class UILIB_API ListHeaderUI : public HorizontalLayoutUI
{
public:
   ListHeaderUI();

   const char* GetClass() const;
   void* GetInterface(const TCHAR* name);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ListHeaderItemUI : public ControlUI
{
public:
   ListHeaderItemUI();

   const char* GetClass() const;
   UINT GetControlFlags() const;
   
   void SetText(const TCHAR* txt);

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetWidth(int cxWidth);
   void SetAttribute(const char* name, const char* value);

   RECT GetThumbRect(RECT rc) const;

protected:
   int m_cxWidth;
   POINT ptLastMouse;
   UINT m_uDragState;
};

class UILIB_API ListFooterUI : public HorizontalLayoutUI
{
public:
   ListFooterUI();

   const char* GetClass() const;
   void* GetInterface(const TCHAR* name);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ListUI : public VerticalLayoutUI, public IListUI
{
public:
   ListUI();

   const char* GetClass() const;
   UINT GetControlFlags() const;
   void* GetInterface(const TCHAR* name);

   int GetCurSel() const;
   bool SelectItem(int idx);

   ListHeaderUI* GetHeader() const;
   ListFooterUI* GetFooter() const;   
   ContainerUI* GetList() const;
   const TListInfoUI* GetListInfo() const;

   ControlUI* GetItem(int idx) const;
   int GetCount() const;
   bool Add(ControlUI* ctrl);
   bool Remove(ControlUI* ctrl);
   void RemoveAll();

   void EnsureVisible(int idx);
   void Scroll(int dx, int dy);

   void SetExpanding(bool expandable);

   int GetExpandedItem() const;
   bool ExpandItem(int idx, bool bExpand = true);

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   void SetAttribute(const char* name, const char* value);

   IListCallbackUI* GetTextCallback() const;
   void SetTextCallback(IListCallbackUI* cb);

protected:
   int m_curSel;
   int m_expandedItem;
   IListCallbackUI* m_cb;
   VerticalLayoutUI* m_list;
   ListHeaderUI* m_header;
   ListFooterUI* m_footer;
   TListInfoUI m_listInfo;
};

class UILIB_API ListLabelElementUI : public ListElementUI
{
public:
   ListLabelElementUI();

   const char* GetClass() const;

   void SetWidth(int cxWidth);
   void SetTextStyle(UINT uStyle);

   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

   void SetAttribute(const char* name, const char* value);

protected:
   int m_cxWidth;
   UINT m_uTextStyle;
   UINT m_uButtonState;
};

class UILIB_API ListTextElementUI : public ListLabelElementUI
{
public:
   ListTextElementUI();

   const char* GetClass() const;
   UINT GetControlFlags() const;

   void SetOwner(ControlUI* owner);

   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

protected:
   int m_cyItem;
   int m_nLinks;
   RECT m_rcLinks[8];
   IListUI* m_owner;
};

class UILIB_API ListExpandElementUI : public ListTextElementUI, public IContainerUI
{
public:
   ListExpandElementUI();
   virtual ~ListExpandElementUI();

   const char* GetClass() const;

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetManager(PaintManagerUI* manager, ControlUI* parent);
   ControlUI* FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags);

   ControlUI* GetItem(int idx) const;
   int GetCount() const;
   bool Add(ControlUI* ctrl);
   bool Remove(ControlUI* ctrl);
   void RemoveAll();

   bool IsExpanded() const;
   bool Expand(bool bExpand = true);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

protected:
   bool m_bExpanded;
   int m_cyExpanded;
   ContainerUI* m_pContainer;
};

#endif // !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
