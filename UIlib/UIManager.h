#if !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)
#define AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_

class ControlUI;

typedef enum EVENTTYPE_UI
{
   UIEVENT__FIRST = 0,
   UIEVENT_MOUSEMOVE,
   UIEVENT_MOUSELEAVE,
   UIEVENT_MOUSEENTER,
   UIEVENT_MOUSEHOVER,
   UIEVENT_KEYDOWN,
   UIEVENT_KEYUP,
   UIEVENT_CHAR,
   UIEVENT_SYSKEY,
   UIEVENT_KILLFOCUS,
   UIEVENT_SETFOCUS,
   UIEVENT_BUTTONDOWN,
   UIEVENT_BUTTONUP,
   UIEVENT_DBLCLICK,
   UIEVENT_CONTEXTMENU,
   UIEVENT_VSCROLL,
   UIEVENT_HSCROLL,
   UIEVENT_SCROLLWHEEL,
   UIEVENT_WINDOWSIZE,
   UIEVENT_SETCURSOR,
   UIEVENT_MEASUREITEM,
   UIEVENT_DRAWITEM,
   UIEVENT_TIMER,
   UIEVENT_NOTIFY,
   UIEVENT_COMMAND,
   UIEVENT__LAST
};

typedef enum
{
   UIFONT__FIRST = 0,
   UIFONT_NORMAL,
   UIFONT_BOLD,
   UIFONT_CAPTION,
   UIFONT_MENU,
   UIFONT_LINK,
   UIFONT_TITLE,
   UIFONT_HEADLINE,
   UIFONT_SUBSCRIPT,
   UIFONT__LAST,
} UITYPE_FONT;

typedef enum
{
   UICOLOR__FIRST = 0,
   UICOLOR_WINDOW_BACKGROUND,
   UICOLOR_WINDOW_TEXT,
   UICOLOR_DIALOG_BACKGROUND,
   UICOLOR_DIALOG_TEXT_NORMAL,
   UICOLOR_DIALOG_TEXT_DARK,
   UICOLOR_MENU_BACKGROUND,
   UICOLOR_MENU_TEXT_NORMAL,
   UICOLOR_MENU_TEXT_HOVER,
   UICOLOR_MENU_TEXT_SELECTED,
   UICOLOR_TEXTCOLOR_NORMAL,
   UICOLOR_TEXTCOLOR_HOVER,
   UICOLOR_TAB_BACKGROUND_NORMAL,
   UICOLOR_TAB_BACKGROUND_SELECTED,
   UICOLOR_TAB_FOLDER_NORMAL,
   UICOLOR_TAB_FOLDER_SELECTED,
   UICOLOR_TAB_BORDER,
   UICOLOR_TAB_TEXT_NORMAL,
   UICOLOR_TAB_TEXT_SELECTED,
   UICOLOR_TAB_TEXT_DISABLED,
   UICOLOR_NAVIGATOR_BACKGROUND,
   UICOLOR_NAVIGATOR_BUTTON_HOVER,
   UICOLOR_NAVIGATOR_BUTTON_PUSHED,
   UICOLOR_NAVIGATOR_BUTTON_SELECTED,
   UICOLOR_NAVIGATOR_BORDER_NORMAL,
   UICOLOR_NAVIGATOR_BORDER_SELECTED,
   UICOLOR_NAVIGATOR_TEXT_NORMAL,
   UICOLOR_NAVIGATOR_TEXT_SELECTED,
   UICOLOR_NAVIGATOR_TEXT_PUSHED,
   UICOLOR_BUTTON_BACKGROUND_NORMAL,
   UICOLOR_BUTTON_BACKGROUND_DISABLED,
   UICOLOR_BUTTON_BACKGROUND_PUSHED,
   UICOLOR_BUTTON_TEXT_NORMAL,
   UICOLOR_BUTTON_TEXT_PUSHED,
   UICOLOR_BUTTON_TEXT_DISABLED,
   UICOLOR_BUTTON_BORDER_LIGHT,
   UICOLOR_BUTTON_BORDER_DARK,
   UICOLOR_BUTTON_BORDER_DISABLED,
   UICOLOR_BUTTON_BORDER_FOCUS,
   UICOLOR_CONTROL_BACKGROUND_NORMAL,
   UICOLOR_CONTROL_BACKGROUND_SELECTED,
   UICOLOR_CONTROL_BACKGROUND_DISABLED,
   UICOLOR_CONTROL_BACKGROUND_READONLY,
   UICOLOR_CONTROL_BACKGROUND_HOVER,
   UICOLOR_CONTROL_BACKGROUND_SORTED,
   UICOLOR_CONTROL_BACKGROUND_EXPANDED,
   UICOLOR_CONTROL_BORDER_NORMAL,
   UICOLOR_CONTROL_BORDER_SELECTED,
   UICOLOR_CONTROL_BORDER_DISABLED,
   UICOLOR_CONTROL_TEXT_NORMAL,
   UICOLOR_CONTROL_TEXT_SELECTED,
   UICOLOR_CONTROL_TEXT_DISABLED,
   UICOLOR_CONTROL_TEXT_READONLY,
   UICOLOR_TOOL_BACKGROUND_NORMAL,
   UICOLOR_TOOL_BACKGROUND_DISABLED,
   UICOLOR_TOOL_BACKGROUND_HOVER,
   UICOLOR_TOOL_BACKGROUND_PUSHED,
   UICOLOR_TOOL_BORDER_NORMAL,
   UICOLOR_TOOL_BORDER_DISABLED,
   UICOLOR_TOOL_BORDER_HOVER,
   UICOLOR_TOOL_BORDER_PUSHED,
   UICOLOR_EDIT_BACKGROUND_NORMAL,
   UICOLOR_EDIT_BACKGROUND_HOVER,
   UICOLOR_EDIT_BACKGROUND_DISABLED,
   UICOLOR_EDIT_BACKGROUND_READONLY,
   UICOLOR_EDIT_TEXT_NORMAL,
   UICOLOR_EDIT_TEXT_DISABLED,
   UICOLOR_EDIT_TEXT_READONLY,
   UICOLOR_TITLE_BACKGROUND,
   UICOLOR_TITLE_TEXT,
   UICOLOR_TITLE_BORDER_LIGHT,
   UICOLOR_TITLE_BORDER_DARK,
   UICOLOR_HEADER_BACKGROUND,
   UICOLOR_HEADER_BORDER,
   UICOLOR_HEADER_SEPARATOR,
   UICOLOR_HEADER_TEXT,
   UICOLOR_TASK_BACKGROUND,
   UICOLOR_TASK_CAPTION,
   UICOLOR_TASK_BORDER,
   UICOLOR_TASK_TEXT,
   UICOLOR_LINK_TEXT_HOVER,
   UICOLOR_LINK_TEXT_NORMAL,
   UICOLOR_STANDARD_BLACK,
   UICOLOR_STANDARD_YELLOW,
   UICOLOR_STANDARD_RED,
   UICOLOR_STANDARD_GREY,
   UICOLOR_STANDARD_LIGHTGREY,
   UICOLOR_STANDARD_WHITE,
   UICOLOR__LAST,
   UICOLOR__INVALID,
} UITYPE_COLOR;

// Styles for the DoPaintFrame() helper
#define UIFRAME_ROUND        0x00000001
#define UIFRAME_FOCUS        0x00000002

// Styles for the DoPaintArcCaption() helper
#define UIARC_GRIPPER        0x00000001
#define UIARC_EXPANDBUTTON   0x00000002
#define UIARC_COLLAPSEBUTTON 0x00000004

// Flags for ControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_ME_FIRST      0x80000000

// Flags for Draw Style
#define UIDRAWSTYLE_INPLACE  0x00000001
#define UIDRAWSTYLE_FOCUS    0x00000002

// Flags for DoAnumateWindow()
#define UIANIM_FADE          0x00000001
#define UIANIM_HIDE          0x00000002

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_CHECKED      0x00000020
#define UISTATE_READONLY     0x00000040
#define UISTATE_CAPTURED     0x00000080

// Structure for notifications from the system
// to the control implementation.
typedef struct
{
   int Type;
   ControlUI* pSender;
   DWORD dwTimestamp;
   POINT ptMouse;
   TCHAR chKey;
   WORD wKeyState;
   WPARAM wParam;
   LPARAM lParam;
} TEventUI;

// Structure for notifications to the outside world
typedef struct 
{
   const char*  type;
   ControlUI*   pSender;
   DWORD        dwTimestamp;
   POINT        ptMouse;
   WPARAM       wParam;
   LPARAM       lParam;
} TNotifyUI;

// Structure for adding alpha bitmaps on top of the window
typedef struct
{
   HBITMAP hBitmap;
   RECT rc;
   BYTE iAlpha;
} TPostPaintUI;

// System settings
typedef struct
{
   bool bShowKeyboardCues;
   bool bScrollLists;
} TSystemSettingsUI;

// Various system settings
typedef struct
{
   INT cxvscroll;
} TSystemMetricsUI;

// Listener interface
class INotifyUI
{
public:
   virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
   virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class UILIB_API PaintManagerUI
{
public:
   PaintManagerUI();
   ~PaintManagerUI();

public:
   void Init(HWND hWnd);
   void UpdateLayout();
   void Invalidate(RECT rcItem);

   HDC GetPaintDC() const;
   HWND GetPaintWindow() const;

   POINT GetMousePos() const;
   SIZE GetClientSize() const;

   void SetMinMaxInfo(int cx, int cy);

   static HINSTANCE GetResourceInstance();
   static HINSTANCE GetLanguageInstance();
   static void SetResourceInstance(HINSTANCE hInst);
   static void SetLanguageInstance(HINSTANCE hInst);

   HPEN GetThemePen(UITYPE_COLOR idx) const;
   HFONT GetThemeFont(UITYPE_FONT idx) const;
   HBRUSH GetThemeBrush(UITYPE_COLOR idx) const;
   COLORREF GetThemeColor(UITYPE_COLOR idx) const;
   HICON GetThemeIcon(int idx, int cxySize) const;
   const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT idx) const;
   bool GetThemeColorPair(UITYPE_COLOR idx, COLORREF& clr1, COLORREF& clr2) const;

   bool AttachDialog(ControlUI* ctrl);
   bool InitControls(ControlUI* ctrl, ControlUI* parent = NULL);
   void ReapObjects(ControlUI* ctrl);

   ControlUI* GetFocus() const;
   void SetFocus(ControlUI* ctrl);

   bool SetNextTabControl(bool bForward = true);

   bool SetTimer(ControlUI* ctrl, UINT timerID, UINT uElapse);
   bool KillTimer(ControlUI* ctrl, UINT timerID);

   bool AddNotifier(INotifyUI* ctrl);
   bool RemoveNotifier(INotifyUI* ctrl);   
   void SendNotify(TNotifyUI& Msg);
   void SendNotify(ControlUI* ctrl, const char* msg, WPARAM wParam = 0, LPARAM lParam = 0);

   bool AddMessageFilter(IMessageFilterUI* filter);
   bool RemoveMessageFilter(IMessageFilterUI* filter);

   bool AddAnimJob(const AnimJobUI& job);
   bool AddPostPaintBlit(const TPostPaintUI& job);

   ControlUI* FindControl(POINT pt) const;
   ControlUI* FindControl(const char* name);

   static void MessageLoop();
   static bool TranslateMessage(const MSG* pMsg);

   bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
   bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

   TSystemMetricsUI GetSystemMetrics() const;
   TSystemSettingsUI GetSystemSettings() const;
   void SetSystemSettings(const TSystemSettingsUI Config);

private:
   static ControlUI* CALLBACK __FindControlFromNameHash(ControlUI* pThis, void* data);
   static ControlUI* CALLBACK __FindControlFromCount(ControlUI* pThis, void* data);
   static ControlUI* CALLBACK __FindControlFromPoint(ControlUI* pThis, void* data);
   static ControlUI* CALLBACK __FindControlFromTab(ControlUI* pThis, void* data);
   static ControlUI* CALLBACK __FindControlFromShortcut(ControlUI* pThis, void* data);

private:
   HWND     m_hWndPaint;
   HDC      m_hDcPaint;
   HDC      m_hDcOffscreen;
   HBITMAP  m_hbmpOffscreen;
   HWND     m_hwndTooltip;
   TOOLINFO m_toolTip;
   //
   ControlUI* m_root;
   ControlUI* m_focus;
   ControlUI* m_eventHover;
   ControlUI* m_eventClick;
   ControlUI* m_eventKey;
   //
   POINT m_ptLastMousePos;
   SIZE m_szMinWindow;
   UINT m_uMsgMouseWheel;
   UINT m_timerID;
   bool m_firstLayout;
   bool m_resizeNeeded;
   bool m_focusNeeded;
   bool m_offscreenPaint;
   bool m_mouseTracking;

   TSystemMetricsUI m_SystemMetrics;
   TSystemSettingsUI m_SystemConfig;

   StdPtrArray m_notifiers;
   StdPtrArray m_nameHash;
   StdPtrArray m_timers;
   StdValArray m_postPaint;
   StdPtrArray m_messageFilters;
   StdPtrArray m_delayedCleanup;

   static HINSTANCE m_hLangInst;
   static HINSTANCE m_hInstance;
   static StdPtrArray m_preMessages;
};

typedef ControlUI* (CALLBACK* FINDCONTROLPROC)(ControlUI*, void*);

class UILIB_API ControlUI : public INotifyUI
{
public:
   ControlUI();
   virtual ~ControlUI();

public:
   const char* GetName() const;
   void        SetName(const char* name);

   virtual void* GetInterface(const char* name);

   virtual bool Activate();
   virtual ControlUI* GetParent() const;

   virtual StdString GetText() const;
#ifdef UNICODE
   virtual void SetText(const TCHAR* txt);
#endif
   virtual void SetText(const char* txt);

           StdString GetToolTip() const;
#ifdef UNICODE
           void SetToolTip(const TCHAR* txt);
#endif
           void SetToolTip(const char* txt);

           TCHAR GetShortcut() const;
           void SetShortcut(TCHAR ch);

           UINT_PTR GetTag() const;
           void SetTag(UINT_PTR pTag);

   virtual void SetFocus();

           bool IsVisible() const;
           bool IsEnabled() const;
           bool IsFocused() const;
   virtual void SetVisible(bool visible = true);
   virtual void SetEnabled(bool bEnable = true);

   virtual ControlUI* FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags);

           PaintManagerUI* GetManager() const;
   virtual void SetManager(PaintManagerUI* manager, ControlUI* parent);

   virtual RECT GetPos() const;
   virtual void SetPos(RECT rc);
   virtual UINT GetControlFlags() const;
   
   void Invalidate();
   void UpdateLayout();

   virtual void Init();
   virtual void Event(TEventUI& event);
   virtual void Notify(TNotifyUI& msg);

   virtual void SetAttribute(const char* name, const char* value);
   ControlUI* ApplyAttributeList(const char* attrList);

   virtual const char* GetClass() const = 0;
   virtual SIZE EstimateSize(SIZE szAvailable) = 0;
   virtual void DoPaint(HDC hDC, const RECT& rcPaint) = 0;

protected:
   PaintManagerUI*  m_manager;
   ControlUI*       m_parent;
   TCHAR            m_chShortcut;
   const char*      m_name;
   StdString        m_txt;
   StdString        m_toolTip;
   UINT_PTR         m_tag;
   RECT             m_rcItem;
   bool             m_visible;
   bool             m_enabled;
   bool             m_focused;
};

#endif // !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)

