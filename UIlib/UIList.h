#if !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
#define AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_

class ListHeaderUI;
class ListFooterUI;

#define UILIST_MAX_COLUMNS 10

typedef struct
{
    int          nColumns;
    RECT         rcColumn[UILIST_MAX_COLUMNS];
    UITYPE_COLOR Text;
    UITYPE_COLOR Background;
    UITYPE_COLOR SelText;
    UITYPE_COLOR SelBackground;
    UITYPE_COLOR HotText;
    UITYPE_COLOR HotBackground;
    bool         expandable;
} TListInfoUI;

class IListCallbackUI
{
public:
    virtual const char* GetItemText(ControlUI* list, int iItem, int subItem) = 0;
    virtual int CompareItem(ControlUI* list, ControlUI* item1, ControlUI* item2) = 0;
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

    virtual UINT GetControlFlags() const;
    virtual void* GetInterface(const char* name);

    int GetIndex() const;
    void SetIndex(int idx);

    void SetOwner(ControlUI* owner);

    bool IsSelected() const;
    bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    virtual bool Activate();

    virtual void Event(TEventUI& event);
    virtual void SetAttribute(const char* name, const char* value);

protected:
    int              m_idx;
    bool             m_bSelected;
    IListOwnerUI*    m_owner;
};

class UILIB_API ListHeaderUI : public HorizontalLayoutUI
{
public:
    ListHeaderUI();

    virtual const char* GetClass() const;
    virtual void* GetInterface(const char* name);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ListHeaderItemUI : public ControlUI
{
public:
    ListHeaderItemUI();

    virtual const char* GetClass() const;
    virtual UINT GetControlFlags() const;

    virtual void SetText(const char* txt);

    virtual void Event(TEventUI& event);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetWidth(int cxWidth);
    virtual void SetAttribute(const char* name, const char* value);

    RECT GetThumbRect(RECT rc) const;

protected:
    int   m_cxWidth;
    POINT m_ptLastMouse;
    UINT  m_uDragState;
};

class UILIB_API ListFooterUI : public HorizontalLayoutUI
{
public:
    ListFooterUI();

    virtual const char* GetClass() const;
    virtual void* GetInterface(const char* name);

    SIZE EstimateSize(SIZE szAvailable);
    void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ListUI : public VerticalLayoutUI, public IListUI
{
public:
    ListUI();

    virtual const char* GetClass() const;
    virtual UINT GetControlFlags() const;
    virtual void* GetInterface(const char* name);

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

    virtual void SetPos(RECT rc);
    virtual void Event(TEventUI& event);
    virtual void SetAttribute(const char* name, const char* value);

    IListCallbackUI* GetTextCallback() const;
    void SetTextCallback(IListCallbackUI* cb);

protected:
    int               m_curSel;
    int               m_expandedItem;
    IListCallbackUI*  m_cb;
    VerticalLayoutUI* m_list;
    ListHeaderUI*     m_header;
    ListFooterUI*     m_footer;
    TListInfoUI       m_listInfo;
};

class UILIB_API ListLabelElementUI : public ListElementUI
{
public:
    ListLabelElementUI();

    virtual const char* GetClass() const;

    void SetWidth(int cxWidth);
    void SetTextStyle(UINT uStyle);

    virtual void Event(TEventUI& event);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    virtual void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

    virtual void SetAttribute(const char* name, const char* value);

protected:
    int  m_cxWidth;
    UINT m_uTextStyle;
    UINT m_uButtonState;
};

class UILIB_API ListTextElementUI : public ListLabelElementUI
{
public:
    ListTextElementUI();

    virtual const char* GetClass() const;
    virtual UINT GetControlFlags() const;

    void SetOwner(ControlUI* owner);

    virtual void Event(TEventUI& event);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    virtual void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

protected:
    int      m_cyItem;
    int      m_nLinks;
    RECT     m_rcLinks[8];
    IListUI* m_owner;
};

class UILIB_API ListExpandElementUI : public ListTextElementUI, public IContainerUI
{
public:
    ListExpandElementUI();
    virtual ~ListExpandElementUI();

    virtual const char* GetClass() const;

    virtual void SetPos(RECT rc);
    virtual void Event(TEventUI& event);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetManager(PaintManagerUI* manager, ControlUI* parent);
    virtual ControlUI* FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags);

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
    ContainerUI* m_container;
};

#endif // !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
