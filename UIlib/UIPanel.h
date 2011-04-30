#if !defined(AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_)
#define AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_

#include "UILabel.h"

class UILIB_API TaskPanelUI : public VerticalLayoutUI
{
public:
    TaskPanelUI();
    ~TaskPanelUI();

    enum { FADE_TIMERID = 10 };
    enum { FADE_DELAY = 500UL };

    virtual const char* GetClass() const;

    virtual void Event(TEventUI& event);
    virtual void SetPos(RECT rc);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
    HBITMAP  m_hFadeBitmap;
    DWORD    m_dwFadeTick;
    RECT     m_rcFade;
};

class UILIB_API NavigatorPanelUI : public VerticalLayoutUI, public IListOwnerUI
{
public:
    NavigatorPanelUI();

    virtual const char* GetClass() const;   
    void* GetInterface(const char* name);

    bool Add(ControlUI* ctrl);

    virtual int GetCurSel() const;
    virtual bool SelectItem(int idx);

    virtual void Event(TEventUI& event);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
    int m_curSel;
};

class UILIB_API NavigatorButtonUI : public ListElementUI
{
public:
    NavigatorButtonUI();

    virtual const char* GetClass() const;
    virtual void Event(TEventUI& event);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

    RECT GetButtonRect(RECT rc) const;

protected:
    UINT m_uButtonState;
};

class UILIB_API SearchTitlePanelUI : public HorizontalLayoutUI
{
public:
    SearchTitlePanelUI();

    const char* GetClass() const;

    void SetImage(int idx);

    void SetPos(RECT rc);
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void SetAttribute(const char* name, const char* value);

protected:
    int m_iconIdx;
};

class UILIB_API PaddingPanelUI : public ControlUI
{
public:
    PaddingPanelUI();
    PaddingPanelUI(int cx, int cy);

    virtual const char* GetClass() const;

    void SetWidth(int cx);
    void SetHeight(int cy);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void SetAttribute(const char* name, const char* value);

protected:
    SIZE m_cxyFixed;
};

class UILIB_API ImagePanelUI : public ControlUI
{
public:
    ImagePanelUI();
    virtual ~ImagePanelUI();

    virtual const char* GetClass() const;

    bool SetImage(const char* pstrImage);
    void SetWidth(int cx);
    void SetHeight(int cy);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void SetAttribute(const char* name, const char* value);

protected:
    HBITMAP m_hBitmap;
    SIZE m_cxyFixed;
};

class UILIB_API TextPanelUI : public LabelPanelUI
{
public:
    TextPanelUI();

    virtual const char* GetClass() const;
    virtual UINT GetControlFlags() const;

    virtual bool Activate();

    void SetTextColor(UITYPE_COLOR TextColor);
    void SetBkColor(UITYPE_COLOR BackColor);

    virtual void Event(TEventUI& event);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    virtual void SetAttribute(const char* name, const char* value);

protected:
    int          m_nLinks;
    RECT         m_rcLinks[8];
    UINT         m_uButtonState;
    UITYPE_COLOR m_TextColor;
    UITYPE_COLOR m_BackColor;
};

class UILIB_API WarningPanelUI : public TextPanelUI
{
public:
    WarningPanelUI();

    virtual const char* GetClass() const;

    void SetWarningType(UINT uType);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);  
    virtual void SetAttribute(const char* name, const char* value);

protected:
    UITYPE_COLOR m_BackColor;
};

#endif // !defined(AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_)

