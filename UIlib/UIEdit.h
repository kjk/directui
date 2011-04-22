#if !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)
#define AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SingleLineEditWnd;

class UILIB_API SingleLineEditUI : public ControlUI
{
friend SingleLineEditWnd;
public:
   SingleLineEditUI();

   const TCHAR* GetClass() const;
   UINT GetControlFlags() const;

   void SetText(const TCHAR* txt);

   void SetEditStyle(UINT uStyle);
   void SetReadOnly(bool bReadOnly);
   bool IsReadOnly() const;

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   SingleLineEditWnd* m_win;

   bool m_bReadOnly;
   UINT m_uEditStyle;
};

class MultiLineEditWnd;

class UILIB_API MultiLineEditUI : public ControlUI
{
friend MultiLineEditWnd;
public:
   MultiLineEditUI();
   ~MultiLineEditUI();

   const TCHAR* GetClass() const;
   UINT GetControlFlags() const;

   void Init();
   
   CStdString GetText() const;
   void SetText(const TCHAR* txt);

   void SetEnabled(bool bEnabled);
   void SetVisible(bool bVisible);
   void SetReadOnly(bool bReadOnly);
   void SetFocus(bool bReadOnly);
   bool IsReadOnly() const;

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void SetPos(RECT rc);
   void SetPos(int left, int top, int right, int bottom);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   MultiLineEditWnd* m_win;
};


#endif // !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)

