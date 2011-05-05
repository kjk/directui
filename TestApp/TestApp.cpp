#include "stdafx.h"
#include "resource.h"
#include "UIlib.h"
#include "Views.h"
#include "UIManager.h"

#if 0
static const char* mainWindowPageDialog = "\
Dialog\n\r\n\
  HorizontalLayout\n\
    VerticalLayout width=150\n\
      Toolbar\n\
        ToolGripper\n\
      NavigatorPanel\n\
        PaddingPanel height=18\n\
        NavigatorButton name=page_start text='<i 0> Start' selected=true tooltip='Vis start siden'\n\
        NavigatorButton name=page_registers text='<i 4> Registre' tooltip='Vis forskellige registre'\n\
        NavigatorButton name=page_systems text='<i 4> Systemer'\n\
        NavigatorButton name=page_configure text='<i 4> Opsætning'\n\
        NavigatorButton name=page_reports text='<i 4> Rapporter'\n\
    VerticalLayout\n\
      Toolbar\n\
        LabelPanel align=right text='<f 6><c #fffe28>Start Side</c></f>'\n\
      ToolbarTitlePanel text=\"<f 7>Bjarke's Test Program</f>\"\n\
      TitleShadow\n\
      Canvas bgCol=winbg watermark=StartWatermark\n\
        VerticalLayout\n\
          TextPanel text='<f 8>Vælg startområde?</f>'\n\
          FadedLine\n\
          TileLayout scrollbar=true\n\
            TextPanel name=link_registers shortcut=R text='<i 7 50><a><f 6>&Registre</f></a>\\n<h>\\n<c #444540>Vælg denne menu for at rette i diverse registre i systemet.\\n\\nDu kan rette i kunde, vogn og chauffør-reigsteret.'\n\
            TextPanel name=link_systems shortcut=S text='<i 9 50><a><f 6>&Systemer</f></a>\\n<h>\\n<c #444540>Gennem denne menu kan du opsætte diverse ting.'\n\
            TextPanel name=link_configure text='<i 6 50><a><f 6>Opsætning</f></a>\\n<h>\\n<c #444540>Opsætning giver adgang til konfiguration af de mange kørsels-systemer og regler.'\n\
            TextPanel name=link_reports text='<i 5 50><a><f 6>Rapporter</f></a\\n<h>\\n<c #444540>Rapporter giver dig overblik over registre samt hverdagens ture og bestillinger.\\n\\nGennem statistik og lister kan du hurtigt få præsenteret historiske data fra systemet.'\n\
";
#else
static const char* mainWindowPageDialog = "\
Dialog\n\
  VerticalLayout bgCol=winbg\n\
      TextPanel backColor=3 text='<x 16><c #f00000>h&ello'\n\
      List header=hidden footer=hidden\n\
        TextPanel text='<x 16><c #585ebf><b>Click one of the items below:</b>\\n<h>'\n\
        ListLabelElement name=test_old text='<x 14>Old test window'\n\
        ListLabelElement name=login_window text='<x 14>Login Window'\n\
        ListLabelElement name=test_controls text='<x 14>Test controls'\n\
        #TextPanel text='<x 16><c #F00000><b>A cool effect</b>\\n<h>'\n\
      Button text=&Exit name=exit\n\
";

static const char* loginWindowPageDialog = "\
Dialog\n\
  VerticalLayout     inset=12 bgCol=winbg\n\
    LabelPanel       text='&Search'\n\
    SingleLineEdit   name=navn\n\
    LabelPanel       text=&Type\n\
    SingleLineEdit   name=type\n\
    Button           name=ok text=Login\n\
";
#endif

class BaseWindowWnd : public WindowWnd, public INotifyUI
{
protected:
    const char *m_className;
    HWND        m_hWndClient;

public:
    BaseWindowWnd(const char *className) : m_className(className), m_hWndClient(NULL)
    {
    }

    virtual const char *GetWindowClassName() const {
        return m_className;
    }

    virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME; };
    virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_SETFOCUS)  {
            ::SetFocus(m_hWndClient);
        }
        if (uMsg == WM_SIZE)  {
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::MoveWindow(m_hWndClient, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE);
            return 0;
        }
        if (uMsg == WM_ERASEBKGND)  {
            return 1;
        }
        if (uMsg == WM_PAINT)  {
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(m_hWnd, &ps);
            ::EndPaint(m_hWnd, &ps);
            return 0;
        }
        return WindowWnd::HandleMessage(uMsg, wParam, lParam);
    }
};

class FrameWindowWnd : public BaseWindowWnd
{
public:
    FrameWindowWnd() : BaseWindowWnd("OldMainWindowWnd") { };

    virtual void Notify(TNotifyUI& msg)
    {
        if (str::Eq(msg.type,"click") || str::Eq(msg.type, "link"))
            _CreatePage(msg.sender->GetName());
        if (str::Eq(msg.type, "itemactivate"))
            _CreatePage("page_search");
    }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)  {
            SetIcon(IDR_MAINFRAME);
            _CreatePage("page_start");
            return 0;
        }
        return BaseWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    void _CreatePage(const char* sName)
    {
        StandardPageWnd* win = NULL;
        if (str::Eq(sName, "page_start"))      win = new StartPageWnd;
        if (str::Eq(sName, "page_configure"))  win = new ConfigurePageWnd;
        if (str::Eq(sName, "page_reports"))    win = new ReportsPageWnd;
        if (str::Eq(sName, "page_systems"))    win = new SystemsPageWnd;
        if (str::Eq(sName, "page_registers"))  win = new RegistersPageWnd;
        if (str::Eq(sName, "link_start"))      win = new StartPageWnd;
        if (str::Eq(sName, "link_configure"))  win = new ConfigurePageWnd;
        if (str::Eq(sName, "link_reports"))    win = new ReportsPageWnd;
        if (str::Eq(sName, "link_systems"))    win = new SystemsPageWnd;
        if (str::Eq(sName, "link_registers"))  win = new RegistersPageWnd;
        if (win != NULL)  {
            if (m_hWndClient != NULL)  ::PostMessage(m_hWndClient, WM_CLOSE, 0, 0L);
            win->m_pm.AddNotifier(this);
            m_hWndClient = win->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0);
            PostMessage(WM_SIZE);
            return;
        }
        if (str::Eq(sName, "page_search"))  win = new SearchPageWnd;
        if (str::Eq(sName, "page_edit"))    win = new EditPageWnd;
        if (win != NULL)  {
            win->Create(m_hWnd, NULL, UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
            return;
        }
    }
};

class LoginWindowPageWnd : public WindowWnd, public INotifyUI
{
public:
    virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_CHILD; }
    virtual const char* GetWindowClassName() const { return "UILoginWindow"; };
    virtual void OnFinalMessage(HWND hWnd) { delete this; }
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void Init() {}
    virtual void Notify(TNotifyUI& msg) {}

public:
    PaintManagerUI m_pm;
};

LRESULT LoginWindowPageWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)  {     
        m_pm.Init(m_hWnd);
        ControlUI* root = CreateDialogFromSimple(loginWindowPageDialog);
        ASSERT(root && "Failed to parse resource");
        m_pm.AttachDialog(root);
        m_pm.AddNotifier(this);
        Init();
        return 0;
    }
    LRESULT lRes = 0;
	if (WM_PAINT == uMsg)
		lRes = 0;
    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
        return lRes;
    return WindowWnd::HandleMessage(uMsg, wParam, lParam);
}

class LoginWindowFrame : public BaseWindowWnd
{
public:
    LoginWindowFrame() : BaseWindowWnd("LoginWindowFrame") { };

    virtual void Notify(TNotifyUI& msg)
    {
#if 0
        ControlUI *sender = msg.sender;
        const char *name = sender->GetName();
        if (str::Eq(msg.type, "itemclick")) {
            if (str::Eq(name, "test_old")) {
                CreateOldTestWindow();
            } else if (str::Eq(name, "login_window")) {
                CreateLoginWindow();
            }
        } else if (str::Eq(msg.type, "click")) {
            if (str::Eq(name, "exit")) {
                PostMessage(WM_CLOSE);
            }
        }
#endif
    }

    void CreateUI()
    {
        LoginWindowPageWnd *win = new LoginWindowPageWnd();
        if (m_hWndClient != NULL)  ::PostMessage(m_hWndClient, WM_CLOSE, 0, 0L);
        win->m_pm.AddNotifier(this);
        m_hWndClient = win->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0);
        PostMessage(WM_SIZE);
    }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)  {
            SetIcon(IDR_MAINFRAME);
            CreateUI();
            return 0;
        }
        return BaseWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }
};

class MainWindowPageWnd : public WindowWnd, public INotifyUI
{
public:
    virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_CHILD; }
    virtual const char* GetWindowClassName() const { return "UIMainChild"; };
    virtual void OnFinalMessage(HWND hWnd) { delete this; }
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void Init() {}
    virtual void Notify(TNotifyUI& msg) {}
    virtual const char* GetDialogResourceSimple() const { return mainWindowPageDialog; }

public:
    PaintManagerUI m_pm;
};

LRESULT MainWindowPageWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)  {     
        m_pm.Init(m_hWnd);
        const char *s = GetDialogResourceSimple();
        ControlUI* root = CreateDialogFromSimple(s);
        ASSERT(root && "Failed to parse resource");
        m_pm.AttachDialog(root);
        m_pm.AddNotifier(this);
        Init();
        return 0;
    }
    LRESULT lRes = 0;
    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))  return lRes;
    return WindowWnd::HandleMessage(uMsg, wParam, lParam);
}

class MainWindowFrame : public BaseWindowWnd
{
public:
    MainWindowFrame() : BaseWindowWnd("MainWindowFrame") { };
    void CreateOldTestWindow() {
        FrameWindowWnd* frame = new FrameWindowWnd();
        frame->Create(NULL, "OldTest", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    }

    void CreateLoginWindow() {
        LoginWindowFrame* frame = new LoginWindowFrame();
        frame->Create(NULL, "Login window test", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE,
            CW_USEDEFAULT, CW_USEDEFAULT, 320, 200);
    }

    void CreateControlsWindow() {
#if 0
		ControlsWindowFrame* frame = new ControlsWindowFrame();
        frame->Create(NULL, "Test controls", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE,
            CW_USEDEFAULT, CW_USEDEFAULT, 649, 480);
#endif
	}

    virtual void Notify(TNotifyUI& msg)
    {
        ControlUI *sender = msg.sender;
        const char *name = sender->GetName();
        if (str::Eq(msg.type, "itemclick")) {
            if (str::Eq(name, "test_old")) {
                CreateOldTestWindow();
            } else if (str::Eq(name, "login_window")) {
                CreateLoginWindow();
            } else if (str::Eq(name, "test_controls")) {
                CreateControlsWindow();
            }
        } else if (str::Eq(msg.type, "click")) {
            if (str::Eq(name, "exit")) {
                PostMessage(WM_CLOSE);
            }
        }
    }

    void CreateUI()
    {
        MainWindowPageWnd *win = new MainWindowPageWnd();
        if (m_hWndClient != NULL)  ::PostMessage(m_hWndClient, WM_CLOSE, 0, 0L);
        win->m_pm.AddNotifier(this);
        m_hWndClient = win->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0);
        PostMessage(WM_SIZE);
    }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)  {
            SetIcon(IDR_MAINFRAME);
            CreateUI();
            return 0;
        }
        if (uMsg == WM_DESTROY)  {
            ::PostQuitMessage(0L);
        }
        return BaseWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    PaintManagerUI::SetResourceInstance(hInstance);
    InitCommonControls();

    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr))  return 0;

    if (::LoadLibraryA("d3d9.dll") == NULL)
        ::MessageBoxA(NULL, "DirectX 9 not installed!", "Test", MB_ICONINFORMATION);
    MainWindowFrame* frame = new MainWindowFrame();
    if (frame == NULL)
        return 0;
    frame->Create(NULL, "Test", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, CW_USEDEFAULT, CW_USEDEFAULT, 480, 640);

    PaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
