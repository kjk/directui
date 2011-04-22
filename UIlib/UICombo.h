#if !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UILIB_API SingleLinePickUI : public ControlUI
{
public:
   SingleLinePickUI();

   void SetWidth(int cxWidth);

   const TCHAR* GetClass() const;
   UINT GetControlFlags() const;
   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   RECT m_rcButton;
   RECT m_rcLinks[8];
   int m_nLinks;
   int m_cxWidth;
   UINT m_uButtonState;
};

class UILIB_API DropDownUI : public ContainerUI, public IListOwnerUI
{
public:
   DropDownUI();

   const TCHAR* GetClass() const;
   void* GetInterface(const TCHAR* name);
   
   void Init();
   UINT GetControlFlags() const;

   CStdString GetText() const;

   SIZE GetDropDownSize() const;
   void SetDropDownSize(SIZE szDropBox);

   int GetCurSel() const;  
   bool SelectItem(int idx);

   bool Add(ControlUI* ctrl);
   bool Remove(ControlUI* ctrl);
   void RemoveAll();

   bool Activate();

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   int m_curSel;
   int m_cxWidth;
   SIZE m_szDropBox;
   RECT m_rcButton;
   UINT m_uButtonState;
};


#endif // !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

