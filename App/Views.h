#if !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)
#define AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_

class StandardPageWnd : public WindowWnd, public INotifyUI
{
public:
   UINT GetClassStyle() const;
   void OnFinalMessage(HWND hWnd);
   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

   virtual void Init();
   virtual void OnPrepareAnimation();
   virtual void Notify(TNotifyUI& msg);
   virtual const char* GetDialogResource() const = 0;

public:
   PaintManagerUI m_pm;
};

class StartPageWnd : public StandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void OnPrepareAnimation();
   void Init();
};

class ConfigurePageWnd : public StandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void OnPrepareAnimation();
};

class RegistersPageWnd : public StandardPageWnd, public IListCallbackUI
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void OnPrepareAnimation();
   // IListCallbackUI
   const TCHAR* GetItemText(ControlUI* ctrl, int idx, int iSubItem);
   int CompareItem(ControlUI* pList, ControlUI* item1, ControlUI* item2);
};

class ReportsPageWnd : public StandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void OnPrepareAnimation();
};

class SystemsPageWnd : public StandardPageWnd, public IListCallbackUI
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void OnPrepareAnimation();
   void OnExpandItem(ControlUI* ctrl);
   // IListCallbackUI
   const TCHAR* GetItemText(ControlUI* ctrl, int idx, int iSubItem);
   int CompareItem(ControlUI* pList, ControlUI* item1, ControlUI* item2);
};

class SearchPageWnd : public StandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void Init();
};

class EditPageWnd : public StandardPageWnd
{
public:
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void OnPrepareAnimation();
   void Init();
};

class PopupWnd : public StandardPageWnd
{
public:
   UINT GetClassStyle() const;
   const TCHAR* GetWindowClassName() const;
   const char* GetDialogResource() const;
   void Notify(TNotifyUI& msg);
   void Init();
};

#endif // !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)

