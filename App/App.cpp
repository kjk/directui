// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "UIlib.h"
#include "Views.h"

class FrameWindowWnd : public WindowWnd, public INotifyUI
{
public:
   FrameWindowWnd() : m_hWndClient(NULL) { };
   virtual const char* GetWindowClassName() const { return "UIMainFrame"; };
   virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME; };
   virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; };

   void Notify(TNotifyUI& msg)
   {
      if (str::Eq(msg.type,"click") || str::Eq(msg.type, "link"))
         _CreatePage(msg.pSender->GetName());
      if (str::Eq(msg.type, "itemactivate"))
         _CreatePage("page_search");
   }

   virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
      if (uMsg == WM_CREATE)  {
         SetIcon(IDR_MAINFRAME);
         _CreatePage("page_start");
      }
      if (uMsg == WM_DESTROY)  {
         ::PostQuitMessage(0L);
      }
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

protected:
   HWND m_hWndClient;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
   PaintManagerUI::SetResourceInstance(hInstance);

   HRESULT Hr = ::CoInitialize(NULL);
   if (FAILED(Hr))  return 0;

   if (::LoadLibraryA("d3d9.dll") == NULL)  ::MessageBoxA(NULL, "Denne applikation vises bedst med DirectX 9 installeret!", "Test", MB_ICONINFORMATION);

   FrameWindowWnd* pFrame = new FrameWindowWnd();
   if (pFrame == NULL)  return 0;
   pFrame->Create(NULL, _T("Test"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

   PaintManagerUI::MessageLoop();

   ::CoUninitialize();
   return 0;
}
