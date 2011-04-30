#if !defined(AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_)
#define AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_

#include "UIButton.h"

class UILIB_API ToolbarUI : public HorizontalLayoutUI
{
public:
    ToolbarUI();

    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ToolbarTitlePanelUI : public ControlUI
{
public:
    ToolbarTitlePanelUI();

    void SetPadding(int iPadding);

    virtual const char* GetClass() const;   
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
    int m_iPadding;
};

class UILIB_API ToolSeparatorUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ToolGripperUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ToolButtonUI : public ButtonUI
{
public:
    ToolButtonUI();

    const char* GetClass() const;

    void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API StatusbarUI : public ContainerUI
{
public:
    virtual const char* GetClass() const;

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

#endif // !defined(AFX_UITOOL_H__20060218_57EB_12A7_9A10_0080AD509054__INCLUDED_)
