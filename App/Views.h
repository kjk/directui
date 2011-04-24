#if !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)
#define AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_

class StandardPageWnd : public WindowWnd, public INotifyUI
{
public:
   virtual UINT GetClassStyle() const;
   virtual void OnFinalMessage(HWND hWnd);
   virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

   virtual void Init();
   virtual void OnPrepareAnimation();
   virtual void Notify(TNotifyUI& msg);
   virtual const char* GetDialogResourceXml() const = 0;

public:
   PaintManagerUI m_pm;
};

class StartPageWnd : public StandardPageWnd
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void OnPrepareAnimation();
   virtual void Init();
};

class ConfigurePageWnd : public StandardPageWnd
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void OnPrepareAnimation();
};

class RegistersPageWnd : public StandardPageWnd, public IListCallbackUI
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void OnPrepareAnimation();
   // IListCallbackUI
   virtual const char* GetItemText(ControlUI* ctrl, int idx, int subItem);
   virtual int CompareItem(ControlUI* list, ControlUI* item1, ControlUI* item2);
};

class ReportsPageWnd : public StandardPageWnd
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void OnPrepareAnimation();
};

class SystemsPageWnd : public StandardPageWnd, public IListCallbackUI
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void Notify(TNotifyUI& msg);
   virtual void OnPrepareAnimation();
   void OnExpandItem(ControlUI* ctrl);
   // IListCallbackUI
   virtual const char* GetItemText(ControlUI* ctrl, int idx, int subItem);
   virtual int CompareItem(ControlUI* list, ControlUI* item1, ControlUI* item2);
};

class SearchPageWnd : public StandardPageWnd
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void Notify(TNotifyUI& msg);
   virtual void Init();
};

class EditPageWnd : public StandardPageWnd
{
public:
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void Notify(TNotifyUI& msg);
   virtual void OnPrepareAnimation();
   virtual void Init();
};

class PopupWnd : public StandardPageWnd
{
public:
   virtual UINT GetClassStyle() const;
   virtual const char* GetWindowClassName() const;
   virtual const char* GetDialogResourceXml() const;
   virtual void Notify(TNotifyUI& msg);
   virtual void Init();
};

#endif // !defined(AFX_VIEWS_H__20050510_6D37_02C3_39BC_0080AD509054__INCLUDED_)

