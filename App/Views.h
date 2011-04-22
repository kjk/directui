#if !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)
#define AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStandardPageWnd : public CWindowWnd, public INotifyUI
{
public:
   UINT GetClassStyle() const;
   void OnFinalMessage(HWND hWnd);
   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

   virtual void Init();
   virtual void OnPrepareAnimation();
   virtual void Notify(TNotifyUI& msg);
   virtual const TCHAR* GetDialogResource() const = 0;

public:
   CPaintManagerUI m_pm;
};

class CStartPageWnd : public CStandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void OnPrepareAnimation();
   void Init();
};

class CConfigurePageWnd : public CStandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void OnPrepareAnimation();
};

class CRegistersPageWnd : public CStandardPageWnd, public IListCallbackUI
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void OnPrepareAnimation();
   // IListCallbackUI
   const TCHAR* GetItemText(ControlUI* ctrl, int idx, int iSubItem);
   int CompareItem(ControlUI* pList, ControlUI* item1, ControlUI* item2);
};

class CReportsPageWnd : public CStandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void OnPrepareAnimation();
};

class CSystemsPageWnd : public CStandardPageWnd, public IListCallbackUI
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void OnPrepareAnimation();
   void OnExpandItem(ControlUI* ctrl);
   // IListCallbackUI
   const TCHAR* GetItemText(ControlUI* ctrl, int idx, int iSubItem);
   int CompareItem(ControlUI* pList, ControlUI* item1, ControlUI* item2);
};

class CSearchPageWnd : public CStandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void Init();
};

class CEditPageWnd : public CStandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void OnPrepareAnimation();
   void Init();
};

class CPopupWnd : public CStandardPageWnd
{
public:
   UINT GetClassStyle() const;
   const TCHAR* GetWindowClassName() const;
   const TCHAR* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void Init();
};


#endif // !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)

