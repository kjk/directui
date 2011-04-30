#if !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)
#define AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_

class UILIB_API LabelPanelUI : public ControlUI
{
public:
    LabelPanelUI();

    virtual const char* GetClass() const;

    virtual void SetText(const char* txt);

    void SetWidth(int cxWidth);
    void SetTextStyle(UINT uStyle);

    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void SetAttribute(const char* name, const char* value);

protected:
    int  m_cxWidth;
    UINT m_uTextStyle;
};

class UILIB_API GreyTextHeaderUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

#endif // !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)

