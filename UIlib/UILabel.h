#if !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)
#define AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UILIB_API CLabelPanelUI : public CControlUI
{
public:
   CLabelPanelUI();

   const TCHAR* GetClass() const;

   void SetText(const TCHAR* pstrText);

   void SetWidth(int cxWidth);
   void SetTextStyle(UINT uStyle);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   int m_cxWidth;
   UINT m_uTextStyle;
};

class UILIB_API CGreyTextHeaderUI : public CControlUI
{
public:
   const TCHAR* GetClass() const;
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
};


#endif // !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)

