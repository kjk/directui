#if !defined(AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_)
#define AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UILabel.h"


class UILIB_API CTaskPanelUI : public CVerticalLayoutUI
{
public:
   CTaskPanelUI();
   ~CTaskPanelUI();

   enum { FADE_TIMERID = 10 };
   enum { FADE_DELAY = 500UL };

   const TCHAR* GetClass() const;

   void Event(TEventUI& event);
   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   
protected:
   HBITMAP m_hFadeBitmap;
   DWORD m_dwFadeTick;
   RECT m_rcFade;
};

class UILIB_API CNavigatorPanelUI : public CVerticalLayoutUI, public IListOwnerUI
{
public:
   CNavigatorPanelUI();

   const TCHAR* GetClass() const;   
   void* GetInterface(const TCHAR* name);

   bool Add(ControlUI* ctrl);

   int GetCurSel() const;
   bool SelectItem(int idx);

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   int m_curSel;
};

class UILIB_API CNavigatorButtonUI : public CListElementUI
{
public:
   CNavigatorButtonUI();

   const TCHAR* GetClass() const;
   void Event(TEventUI& event);
   
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

   RECT GetButtonRect(RECT rc) const;

protected:
   UINT m_uButtonState;
};

class UILIB_API CSearchTitlePanelUI : public CHorizontalLayoutUI
{
public:
   CSearchTitlePanelUI();

   const TCHAR* GetClass() const;

   void SetImage(int idx);

   void SetPos(RECT rc);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   int m_iconIdx;
};


class UILIB_API CPaddingPanelUI : public ControlUI
{
public:
   CPaddingPanelUI();
   CPaddingPanelUI(int cx, int cy);

   const TCHAR* GetClass() const;

   void SetWidth(int cx);
   void SetHeight(int cy);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   SIZE m_cxyFixed;
};

class UILIB_API CImagePanelUI : public ControlUI
{
public:
   CImagePanelUI();
   virtual ~CImagePanelUI();

   const TCHAR* GetClass() const;

   bool SetImage(const TCHAR* pstrImage);
   void SetWidth(int cx);
   void SetHeight(int cy);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   HBITMAP m_hBitmap;
   SIZE m_cxyFixed;
};

class UILIB_API CTextPanelUI : public CLabelPanelUI
{
public:
   CTextPanelUI();

   const TCHAR* GetClass() const;
   UINT GetControlFlags() const;

   bool Activate();

   void SetTextColor(UITYPE_COLOR TextColor);
   void SetBkColor(UITYPE_COLOR BackColor);

   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   int m_nLinks;
   RECT m_rcLinks[8];
   UINT m_uButtonState;
   UITYPE_COLOR m_TextColor;
   UITYPE_COLOR m_BackColor;
};


class UILIB_API CWarningPanelUI : public CTextPanelUI
{
public:
   CWarningPanelUI();

   const TCHAR* GetClass() const;

   void SetWarningType(UINT uType);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);  
   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   UITYPE_COLOR m_BackColor;
};


#endif // !defined(AFX_UIPANEL_H__20060218_451A_298B_7A16_0080AD509054__INCLUDED_)

