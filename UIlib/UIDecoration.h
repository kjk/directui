#if !defined(AFX_UIDECORATION_H__20060218_5471_B259_0B2E_0080AD509054__INCLUDED_)
#define AFX_UIDECORATION_H__20060218_5471_B259_0B2E_0080AD509054__INCLUDED_

class UILIB_API TitleShadowUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API ListHeaderShadowUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API SeparatorLineUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API FadedLineUI : public ControlUI
{
public:
    virtual const char* GetClass() const;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
};

#endif // !defined(AFX_UIDECORATION_H__20060218_5471_B259_0B2E_0080AD509054__INCLUDED_)
