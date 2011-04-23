#if !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)
#define AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_

class UILIB_API LabelPanelUI : public ControlUI
{
public:
   LabelPanelUI();

   const char* GetClass() const;

   void SetText(const TCHAR* txt);

   void SetWidth(int cxWidth);
   void SetTextStyle(UINT uStyle);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(const char* name, const char* value);

protected:
   int m_cxWidth;
   UINT m_uTextStyle;
};

class UILIB_API GreyTextHeaderUI : public ControlUI
{
public:
   const char* GetClass() const;
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};

#endif // !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)

