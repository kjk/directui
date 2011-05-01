#include "StdAfx.h"
#include "UIManager.h"
#include "UIAnim.h"

#include <zmouse.h>

static UINT GetNameHash(const char* name)
{
    UINT i = 0;
    while (*name) {
        i = (i << 5) + i + *name++;
    }
    return i;
}

static UINT MapKeyState()
{
    UINT uState = 0;
    if (::GetKeyState(VK_CONTROL) < 0)  uState |= MK_CONTROL;
    if (::GetKeyState(VK_RBUTTON) < 0)  uState |= MK_LBUTTON;
    if (::GetKeyState(VK_LBUTTON) < 0)  uState |= MK_RBUTTON;
    if (::GetKeyState(VK_SHIFT) < 0)  uState |= MK_SHIFT;
    if (::GetKeyState(VK_MENU) < 0)  uState |= MK_ALT;
    return uState;
}

#define IDB_ICONS16 200
#define IDB_ICONS24 201
#define IDB_ICONS32 202
#define IDB_ICONS50 203

typedef struct
{
    ControlUI* focus;
    ControlUI* pLast;
    bool       bForward;
    bool       bNextIsIt;
} FINDTABINFO;

typedef struct
{
    char ch;
    bool bPickNext;
} FINDSHORTCUT;

typedef struct
{
    ControlUI* sender;
    UINT       localID;
    HWND       hWnd;
    UINT       uWinTimer;
} TIMERINFO;

AnimationSpooler m_anim;
HPEN m_hPens[UICOLOR__LAST] = { 0 };
HFONT m_hFonts[UIFONT__LAST] = { 0 };
HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
COLORREF m_colors[UICOLOR__LAST][2] = { 0 };
TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };
HIMAGELIST m_himgIcons16 = NULL;
HIMAGELIST m_himgIcons24 = NULL;
HIMAGELIST m_himgIcons32 = NULL;
HIMAGELIST m_himgIcons50 = NULL;

HINSTANCE PaintManagerUI::m_hInstance = NULL;
HINSTANCE PaintManagerUI::m_hLangInst = NULL;
StdPtrArray PaintManagerUI::m_preMessages;

PaintManagerUI::PaintManagerUI() :
m_hWndPaint(NULL),
    m_hDcPaint(NULL),
    m_hDcOffscreen(NULL),
    m_hbmpOffscreen(NULL),
    m_hwndTooltip(NULL),
    m_timerID(0x1000),
    m_root(NULL),
    m_focus(NULL),
    m_eventHover(NULL),
    m_eventClick(NULL),
    m_eventKey(NULL),
    m_firstLayout(true),
    m_focusNeeded(false),
    m_resizeNeeded(false),
    m_mouseTracking(false),
    m_offscreenPaint(true),
    m_postPaint(sizeof(TPostPaintUI))
{
    if (m_hFonts[1] == NULL)  
    {
        // Fill in default font information
        LOGFONTA lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        strcpy(lf.lfFaceName, "Tahoma");
        // TODO: Handle "large fonts" or other font sizes when
        //       screen resolution changes!!!
        lf.lfHeight = -12;
        m_aLogFonts[UIFONT_NORMAL] = lf;
        m_aLogFonts[UIFONT_CAPTION] = lf;
        LOGFONT lfBold = lf;
        lfBold.lfWeight += FW_BOLD;
        m_aLogFonts[UIFONT_BOLD] = lfBold;
        lfBold.lfHeight -= 2;
        m_aLogFonts[UIFONT_TITLE] = lfBold;
        lfBold.lfHeight -= 4;
        m_aLogFonts[UIFONT_HEADLINE] = lfBold;
        LOGFONT lfSubscript = lf;
        lfSubscript.lfHeight -= 4;
        m_aLogFonts[UIFONT_SUBSCRIPT] = lfSubscript;
        LOGFONT lfLink = lf;
        lfLink.lfUnderline = TRUE;
        m_aLogFonts[UIFONT_LINK] = lfLink;

        m_colors[UICOLOR_WINDOW_BACKGROUND][0]            = RGB(239,239,235);
        m_colors[UICOLOR_DIALOG_BACKGROUND][0]            = RGB(238,238,238);
        m_colors[UICOLOR_DIALOG_TEXT_NORMAL][0]           = RGB(0,0,0);
        m_colors[UICOLOR_DIALOG_TEXT_DARK][0]             = RGB(96,96,80);
        m_colors[UICOLOR_TITLE_BACKGROUND][0]             = RGB(114,136,172);
        m_colors[UICOLOR_TITLE_TEXT][0]                   = RGB(255,255,255);
        m_colors[UICOLOR_TITLE_BORDER_LIGHT][0]           = RGB(171,192,231);
        m_colors[UICOLOR_TITLE_BORDER_DARK][0]            = RGB(0,55,122);
        m_colors[UICOLOR_BUTTON_BACKGROUND_NORMAL][0]     = RGB(250,250,252);
        m_colors[UICOLOR_BUTTON_BACKGROUND_NORMAL][1]     = RGB(215,215,227);
        m_colors[UICOLOR_BUTTON_BACKGROUND_DISABLED][0]   = RGB(248,248,248);
        m_colors[UICOLOR_BUTTON_BACKGROUND_DISABLED][1]   = RGB(214,214,214);
        m_colors[UICOLOR_BUTTON_BACKGROUND_PUSHED][0]     = RGB(215,215,227);
        m_colors[UICOLOR_BUTTON_BACKGROUND_PUSHED][1]     = RGB(250,250,252);
        m_colors[UICOLOR_BUTTON_TEXT_NORMAL][0]           = RGB(0,0,0);
        m_colors[UICOLOR_BUTTON_TEXT_PUSHED][0]           = RGB(0,0,20);
        m_colors[UICOLOR_BUTTON_TEXT_DISABLED][0]         = RGB(204,204,204);
        m_colors[UICOLOR_BUTTON_BORDER_LIGHT][0]          = RGB(123,158,189);
        m_colors[UICOLOR_BUTTON_BORDER_DARK][0]           = RGB(123,158,189);
        m_colors[UICOLOR_BUTTON_BORDER_DISABLED][0]       = RGB(204,204,204);
        m_colors[UICOLOR_BUTTON_BORDER_FOCUS][0]          = RGB(140,140,140);
        m_colors[UICOLOR_TOOL_BACKGROUND_NORMAL][0]       = RGB(114,136,172);
        m_colors[UICOLOR_TOOL_BACKGROUND_DISABLED][0]     = RGB(100,121,156);
        m_colors[UICOLOR_TOOL_BACKGROUND_HOVER][0]        = RGB(100,121,156);
        m_colors[UICOLOR_TOOL_BACKGROUND_PUSHED][0]       = RGB(80,101,136);
        m_colors[UICOLOR_TOOL_BORDER_NORMAL][0]           = RGB(0,55,122);
        m_colors[UICOLOR_TOOL_BORDER_DISABLED][0]         = RGB(0,55,122);
        m_colors[UICOLOR_TOOL_BORDER_HOVER][0]            = RGB(0,55,122);
        m_colors[UICOLOR_TOOL_BORDER_PUSHED][0]           = RGB(0,55,122);
        m_colors[UICOLOR_EDIT_BACKGROUND_DISABLED][0]     = RGB(255,251,255);
        m_colors[UICOLOR_EDIT_BACKGROUND_READONLY][0]     = RGB(255,251,255);
        m_colors[UICOLOR_EDIT_BACKGROUND_NORMAL][0]       = RGB(255,255,255);
        m_colors[UICOLOR_EDIT_BACKGROUND_HOVER][0]        = RGB(255,251,255);
        m_colors[UICOLOR_EDIT_TEXT_NORMAL][0]             = RGB(0,0,0);
        m_colors[UICOLOR_EDIT_TEXT_DISABLED][0]           = RGB(167,166,170);
        m_colors[UICOLOR_EDIT_TEXT_READONLY][0]           = RGB(167,166,170);      
        m_colors[UICOLOR_NAVIGATOR_BACKGROUND][0]         = RGB(229,217,213);
        m_colors[UICOLOR_NAVIGATOR_BACKGROUND][1]        = RGB(201,199,187);
        m_colors[UICOLOR_NAVIGATOR_TEXT_NORMAL][0]        = RGB(102,102,102);
        m_colors[UICOLOR_NAVIGATOR_TEXT_SELECTED][0]      = RGB(0,0,0);
        m_colors[UICOLOR_NAVIGATOR_TEXT_PUSHED][0]        = RGB(0,0,0);       
        m_colors[UICOLOR_NAVIGATOR_BORDER_NORMAL][0]      = RGB(131,133,116);
        m_colors[UICOLOR_NAVIGATOR_BORDER_SELECTED][0]    = RGB(159,160,144);
        m_colors[UICOLOR_NAVIGATOR_BUTTON_HOVER][0]       = RGB(200,200,200);
        m_colors[UICOLOR_NAVIGATOR_BUTTON_PUSHED][0]      = RGB(184,184,183);
        m_colors[UICOLOR_NAVIGATOR_BUTTON_SELECTED][0]    = RGB(238,238,238);
        m_colors[UICOLOR_TAB_BACKGROUND_NORMAL][0]        = RGB(255,251,255);
        m_colors[UICOLOR_TAB_FOLDER_NORMAL][0]            = RGB(255,251,255);
        m_colors[UICOLOR_TAB_FOLDER_NORMAL][1]            = RGB(233,231,215);
        m_colors[UICOLOR_TAB_FOLDER_SELECTED][0]          = RGB(255,251,255);
        m_colors[UICOLOR_TAB_BORDER][0]                   = RGB(148,166,181);
        m_colors[UICOLOR_TAB_TEXT_NORMAL][0]              = RGB(0,0,0);
        m_colors[UICOLOR_TAB_TEXT_SELECTED][0]            = RGB(0,0,0);
        m_colors[UICOLOR_TAB_TEXT_DISABLED][0]            = RGB(0,0,0);      
        m_colors[UICOLOR_HEADER_BACKGROUND][0]            = RGB(233,231,215);
        m_colors[UICOLOR_HEADER_BACKGROUND][1]            = RGB(150,150,147);
        m_colors[UICOLOR_HEADER_BORDER][0]                = RGB(218,219,201);
        m_colors[UICOLOR_HEADER_SEPARATOR][0]             = RGB(197,193,177);
        m_colors[UICOLOR_HEADER_TEXT][0]                  = RGB(0,0,0);
        m_colors[UICOLOR_TASK_BACKGROUND][0]              = RGB(230,243,255);
        m_colors[UICOLOR_TASK_BACKGROUND][1]              = RGB(255,255,255);
        m_colors[UICOLOR_TASK_BORDER][0]                  = RGB(140,158,198);
        m_colors[UICOLOR_TASK_CAPTION][0]                 = RGB(140,158,198);
        m_colors[UICOLOR_TASK_TEXT][0]                    = RGB(65,65,110);
        m_colors[UICOLOR_LINK_TEXT_NORMAL][0]             = RGB(0,0,255);
        m_colors[UICOLOR_LINK_TEXT_HOVER][0]              = RGB(0,0,100);
        m_colors[UICOLOR_CONTROL_BACKGROUND_NORMAL][0]    = RGB(255,255,255);
        m_colors[UICOLOR_CONTROL_BACKGROUND_SELECTED][0]  = RGB(173,195,231);
        m_colors[UICOLOR_CONTROL_BACKGROUND_READONLY][0]  = RGB(255,255,255);
        m_colors[UICOLOR_CONTROL_BACKGROUND_DISABLED][0]  = RGB(255,255,255);
        m_colors[UICOLOR_CONTROL_BACKGROUND_HOVER][0]     = RGB(233,245,255);
        m_colors[UICOLOR_CONTROL_BACKGROUND_SORTED][0]    = RGB(242,242,246);
        m_colors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][0]  = RGB(255,255,255);
        m_colors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][1]  = RGB(236,242,255);
        m_colors[UICOLOR_CONTROL_BORDER_NORMAL][0]        = RGB(123,158,189);
        m_colors[UICOLOR_CONTROL_BORDER_SELECTED][0]      = RGB(123,158,189);
        m_colors[UICOLOR_CONTROL_BORDER_DISABLED][0]      = RGB(204,204,204);
        m_colors[UICOLOR_CONTROL_TEXT_NORMAL][0]          = RGB(0,0,0);
        m_colors[UICOLOR_CONTROL_TEXT_SELECTED][0]        = RGB(0,0,0);
        m_colors[UICOLOR_CONTROL_TEXT_DISABLED][0]        = RGB(204,204,204);
        m_colors[UICOLOR_STANDARD_BLACK][0]               = RGB(0,0,0);
        m_colors[UICOLOR_STANDARD_YELLOW][0]              = RGB(255,255,204);
        m_colors[UICOLOR_STANDARD_RED][0]                 = RGB(255,204,204);
        m_colors[UICOLOR_STANDARD_GREY][0]                = RGB(145,146,119);
        m_colors[UICOLOR_STANDARD_LIGHTGREY][0]           = RGB(195,196,179);
        m_colors[UICOLOR_STANDARD_WHITE][0]               = RGB(255,255,255);

        // Boot Windows Common Controls (for the ToolTip control)
        ::InitCommonControls();
    }
    m_szMinWindow.cx = 140;
    m_szMinWindow.cy = 200;
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
    m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
    // System Config
    m_SystemConfig.bShowKeyboardCues = false;
    m_SystemConfig.bScrollLists = false;
    // System Metrics
    m_SystemMetrics.cxvscroll = (INT) ::GetSystemMetrics(SM_CXVSCROLL);
}

PaintManagerUI::~PaintManagerUI()
{
    // Delete the control-tree structures
    int i;
    for (i = 0; i < m_delayedCleanup.GetSize(); i++)
        delete static_cast<ControlUI*>(m_delayedCleanup[i]);
    delete m_root;
    // Release other collections
    for (i = 0; i < m_timers.GetSize(); i++)  delete static_cast<TIMERINFO*>(m_timers[i]);
    // Reset other parts...
    ::DestroyWindow(m_hwndTooltip);
    ::DeleteDC(m_hDcOffscreen);
    ::DeleteObject(m_hbmpOffscreen);
    ::ReleaseDC(m_hWndPaint, m_hDcPaint);
    m_preMessages.Remove(m_preMessages.Find(this));
}

void PaintManagerUI::Init(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    // Remember the window context we came from
    m_hWndPaint = hWnd;
    m_hDcPaint = ::GetDC(hWnd);
    // We'll want to filter messages globally too
    m_preMessages.Add(this);
}

HINSTANCE PaintManagerUI::GetResourceInstance()
{
    return m_hInstance;
}

HINSTANCE PaintManagerUI::GetLanguageInstance()
{
    return m_hLangInst;
}

void PaintManagerUI::SetResourceInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
    if (m_hLangInst == NULL)  m_hLangInst = hInst;
}

void PaintManagerUI::SetLanguageInstance(HINSTANCE hInst)
{
    m_hLangInst = hInst;
}

HWND PaintManagerUI::GetPaintWindow() const
{
    return m_hWndPaint;
}

HDC PaintManagerUI::GetPaintDC() const
{
    return m_hDcPaint;
}

POINT PaintManagerUI::GetMousePos() const
{
    return m_ptLastMousePos;
}

SIZE PaintManagerUI::GetClientSize() const
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    return CSize(RectDx(rcClient), RectDy(rcClient));
}

void PaintManagerUI::SetMinMaxInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMinWindow.cx = cx;
    m_szMinWindow.cy = cy;
}

bool PaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
    switch (uMsg)  {
    case WM_KEYDOWN:
        {
            // Tabbing between controls
            if (wParam == VK_TAB)  {
                SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
                m_SystemConfig.bShowKeyboardCues = true;
                ::InvalidateRect(m_hWndPaint, NULL, FALSE);
                return true;
            }
            // Handle default dialog controls OK and CANCEL.
            // If there are controls named "ok" or "cancel" they
            // will be activated on keypress.
            if (wParam == VK_RETURN)  {
                ControlUI* ctrl = FindControl("ok");
                if (ctrl != NULL && m_focus != ctrl)  {
                    if (m_focus == NULL || (m_focus->GetControlFlags() & UIFLAG_WANTRETURN) == 0)  {
                        ctrl->Activate();
                        return true;
                    }
                }
            }
            if (wParam == VK_ESCAPE)  {
                ControlUI* ctrl = FindControl("cancel");
                if (ctrl != NULL)  {
                    ctrl->Activate();
                    return true;
                }
            }
        }
        break;
    case WM_SYSCHAR:
        {
            // Handle ALT-shortcut key-combinations
            FINDSHORTCUT fs = { 0 };
            fs.ch = toupper(wParam);
            ControlUI* ctrl = m_root->FindControl(__FindControlFromShortcut, &fs, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
            if (ctrl != NULL)  {
                ctrl->SetFocus();
                ctrl->Activate();
                return true;
            }
        }
        break;
    case WM_SYSKEYDOWN:
        {
            // Press ALT once and the shortcuts will be shown in view
            if (wParam == VK_MENU && !m_SystemConfig.bShowKeyboardCues)  {
                m_SystemConfig.bShowKeyboardCues = true;
                ::InvalidateRect(m_hWndPaint, NULL, FALSE);
            }
            if (m_focus != NULL)  {
                TEventUI event = { 0 };
                event.type = UIEVENT_SYSKEY;
                event.chKey = wParam;
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
                event.timestamp = ::GetTickCount();
                m_focus->Event(event);
            }
        }
        break;
    }
    return false;
}

bool PaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
#ifdef _DEBUG
    switch (uMsg)  {
    case WM_NCPAINT:
    case WM_NCHITTEST:
    case WM_SETCURSOR:
        break;
    default:
        TRACE("MSG: %-20s (%08ld)", TRACEMSG(uMsg), ::GetTickCount());
    }
#endif
    // Not ready yet?
    if (m_hWndPaint == NULL)
        return false;
    // Cycle through listeners
    for (int i = 0; i < m_messageFilters.GetSize(); i++)  
    {
        bool bHandled = false;
        LRESULT lResult = static_cast<IMessageFilterUI*>(m_messageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
        if (bHandled)  {
            lRes = lResult;
            return true;
        }
    }
    // Custom handling of events
    switch (uMsg)  {
    case WM_APP + 1:
        {
            // Delayed control-tree cleanup. See AttachDialog() for details.
            for (int i = 0; i < m_delayedCleanup.GetSize(); i++)  delete static_cast<ControlUI*>(m_delayedCleanup[i]);
            m_delayedCleanup.Empty();
        }
        break;
    case WM_CLOSE:
        {
            // Make sure all matching "closing" events are sent
            TEventUI event = { 0 };
            event.ptMouse = m_ptLastMousePos;
            event.timestamp = ::GetTickCount();
            if (m_eventHover != NULL)  {
                event.type = UIEVENT_MOUSELEAVE;
                event.sender = m_eventHover;
                m_eventHover->Event(event);
            }
            if (m_eventClick != NULL)  {
                event.type = UIEVENT_BUTTONUP;
                event.sender = m_eventClick;
                m_eventClick->Event(event);
            }
            SetFocus(NULL);
            // Hmmph, the usual Windows tricks to avoid
            // focus loss...
            HWND hwndParent = GetWindowOwner(m_hWndPaint);
            if (hwndParent != NULL)  ::SetFocus(hwndParent);
        }
        break;
    case WM_ERASEBKGND:
        {
            // We'll do the painting here...
            lRes = 1;
        }
        return true;
    case WM_PAINT:
        {
            // Should we paint?
            RECT rcPaint = { 0 };
            if (!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE))  return true;
            // Do we need to resize anything?
            // This is the time where we layout the controls on the form.
            // We delay this even from the WM_SIZE messages since resizing can be
            // a very expensize operation.
            if (m_resizeNeeded)  {
                RECT rcClient = { 0 };
                ::GetClientRect(m_hWndPaint, &rcClient);
                if (!::IsRectEmpty(&rcClient))  {
                    HDC hDC = ::CreateCompatibleDC(m_hDcPaint);
                    m_root->SetPos(rcClient);
                    ::DeleteDC(hDC);
                    m_resizeNeeded = false;
                    // We'll want to notify the window when it is first initialized
                    // with the correct layout. The window form would take the time
                    // to submit swipes/animations.
                    if (m_firstLayout)  {
                        m_firstLayout = false;
                        SendNotify(m_root, "windowinit");
                    }
                }
                // Reset offscreen device
                ::DeleteDC(m_hDcOffscreen);
                ::DeleteObject(m_hbmpOffscreen);
                m_hDcOffscreen = NULL;
                m_hbmpOffscreen = NULL;
            }
            // Set focus to first control?
            if (m_focusNeeded)  {
                SetNextTabControl();
            }

            // Render screen
            if (m_anim.IsAnimating()) 
            {
                // 3D animation in progress
                m_anim.Render();
                // Do a minimum paint loop
                // Keep the client area invalid so we generate lots of
                // WM_PAINT messages. Cross fingers that Windows doesn't
                // batch these somehow in the future.
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint, &ps);
                ::EndPaint(m_hWndPaint, &ps);
                ::InvalidateRect(m_hWndPaint, NULL, FALSE);
            } else if (m_anim.IsJobScheduled())  {
                // Animation system needs to be initialized
                m_anim.Init(m_hWndPaint);
                // A 3D animation was scheduled; allow the render engine to
                // capture the window content and repaint some other time
                if (!m_anim.PrepareAnimation(m_hWndPaint))  m_anim.CancelJobs();
                ::InvalidateRect(m_hWndPaint, NULL, TRUE);
            } else {
                // Standard painting of control-tree - no 3D animation now.
                // Prepare offscreen bitmap?
                if (m_offscreenPaint && m_hbmpOffscreen == NULL) 
                {
                    RECT rcClient = { 0 };
                    ::GetClientRect(m_hWndPaint, &rcClient);
                    m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
                    m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, RectDx(rcClient), RectDy(rcClient)); 
                    ASSERT(m_hDcOffscreen);
                    ASSERT(m_hbmpOffscreen);
                }
                // Begin Windows paint
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint, &ps);
                if (m_offscreenPaint) 
                {
                    // We have an offscreen device to paint on for flickerfree display.
                    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
                    // Paint the image on the offscreen bitmap
                    int iSaveDC = ::SaveDC(m_hDcOffscreen);
                    m_root->DoPaint(m_hDcOffscreen, ps.rcPaint);
                    ::RestoreDC(m_hDcOffscreen, iSaveDC);
                    // Draw alpha bitmaps on top?
                    for (int i = 0; i < m_postPaint.GetSize(); i++)  {
                        TPostPaintUI* pBlit = static_cast<TPostPaintUI*>(m_postPaint[i]);
                        BlueRenderEngineUI::DoPaintAlphaBitmap(m_hDcOffscreen, this, pBlit->hBitmap, pBlit->rc, pBlit->iAlpha);
                    }
                    m_postPaint.Empty();
                    // Blit offscreen bitmap back to display
                    ::BitBlt(ps.hdc, 
                        ps.rcPaint.left, 
                        ps.rcPaint.top, 
                        RectDx(ps.rcPaint),
                        RectDy(ps.rcPaint),
                        m_hDcOffscreen,
                        ps.rcPaint.left,
                        ps.rcPaint.top,
                        SRCCOPY);
                    ::SelectObject(m_hDcOffscreen, hOldBitmap);
                } else {
                    // A standard paint job
                    int iSaveDC = ::SaveDC(ps.hdc);
                    m_root->DoPaint(ps.hdc, ps.rcPaint);
                    ::RestoreDC(ps.hdc, iSaveDC);
                }
                ::EndPaint(m_hWndPaint, &ps);
            }
        }
        // If any of the painting requested a resize again, we'll need
        // to invalidate the entire window once more.
        if (m_resizeNeeded)
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return true;
    case WM_PRINTCLIENT:
        {
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            HDC hDC = (HDC) wParam;
            int save = ::SaveDC(hDC);
            m_root->DoPaint(hDC, rcClient);
            // Check for traversing children. The crux is that WM_PRINT will assume
            // that the DC is positioned at frame coordinates and will paint the child
            // control at the wrong position. We'll simulate the entire thing instead.
            if ((lParam & PRF_CHILDREN) != 0)  {
                HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                while (hWndChild != NULL)  {
                    RECT rcPos = { 0 };
                    ::GetWindowRect(hWndChild, &rcPos);
                    ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
                    ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
                    // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
                    //       since the latter will not print the nonclient correctly for
                    //       EDIT controls.
                    ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
                    hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                }
            }
            ::RestoreDC(hDC, save);
        }
        break;
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
            lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
        }
        break;
    case WM_SIZE:
        {
            if (m_focus != NULL)  {
                TEventUI event = { 0 };
                event.type = UIEVENT_WINDOWSIZE;
                event.timestamp = ::GetTickCount();
                m_focus->Event(event);
            }
            if (m_anim.IsAnimating())  m_anim.CancelJobs();
            m_resizeNeeded = true;
        }
        return true;
    case WM_TIMER:
        {
            for (int i = 0; i < m_timers.GetSize(); i++)  {
                const TIMERINFO* timer = static_cast<TIMERINFO*>(m_timers[i]);
                if (timer->hWnd == m_hWndPaint && timer->uWinTimer == LOWORD(wParam))  {
                    TEventUI event = { 0 };
                    event.type = UIEVENT_TIMER;
                    event.wParam = timer->localID;
                    event.timestamp = ::GetTickCount();
                    timer->sender->Event(event);
                    break;
                }
            }
        }
        break;
    case WM_MOUSEHOVER:
        {
            m_mouseTracking = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ControlUI* hover = FindControl(pt);
            if (hover == NULL)  break;
            // Generate mouse hover event
            if (m_eventHover != NULL)  {
                TEventUI event = { 0 };
                event.ptMouse = pt;
                event.type = UIEVENT_MOUSEHOVER;
                event.sender = hover;
                event.timestamp = ::GetTickCount();
                m_eventHover->Event(event);
            }
            // Create tooltip information
            StdString sToolTip = hover->GetToolTip();
            if (sToolTip.IsEmpty())  return true;
            sToolTip.ProcessResourceTokens();
            ::ZeroMemory(&m_toolTip, sizeof(TOOLINFO));
            m_toolTip.cbSize = sizeof(TOOLINFO);
            m_toolTip.uFlags = TTF_IDISHWND;
            m_toolTip.hwnd = m_hWndPaint;
            m_toolTip.uId = (UINT) m_hWndPaint;
            m_toolTip.hinst = m_hInstance;
            m_toolTip.lpszText = const_cast<char*>( (const char*) sToolTip) ;
            m_toolTip.rect = hover->GetPos();
            if (m_hwndTooltip == NULL)  {
                m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
                ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_toolTip);
            }
            ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_toolTip);
            ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_toolTip);
        }
        return true;
    case WM_MOUSELEAVE:
        {
            if (m_hwndTooltip != NULL)
                ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_toolTip);
            if (m_mouseTracking)
                ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
            m_mouseTracking = false;
        }
        break;
    case WM_MOUSEMOVE:
        {
            // Start tracking this entire window again...
            if (!m_mouseTracking)  {
                TRACKMOUSEEVENT tme = { 0 };
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = m_hWndPaint;
                tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                _TrackMouseEvent(&tme);
                m_mouseTracking = true;
            }
            // Generate the appropriate mouse messages
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            ControlUI* pNewHover = FindControl(pt);
            if (pNewHover != NULL && pNewHover->GetManager() != this)  break;
            TEventUI event = { 0 };
            event.ptMouse = pt;
            event.timestamp = ::GetTickCount();
            if (pNewHover != m_eventHover && m_eventHover != NULL)  {
                event.type = UIEVENT_MOUSELEAVE;
                event.sender = pNewHover;
                m_eventHover->Event(event);
                m_eventHover = NULL;
                if (m_hwndTooltip != NULL)  ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_toolTip);
            }
            if (pNewHover != m_eventHover && pNewHover != NULL)  {
                event.type = UIEVENT_MOUSEENTER;
                event.sender = m_eventHover;
                pNewHover->Event(event);
                m_eventHover = pNewHover;
            }
            if (m_eventClick != NULL)  {
                event.type = UIEVENT_MOUSEMOVE;
                event.sender = NULL;
                m_eventClick->Event(event);
            } else if (pNewHover != NULL)  {
                event.type = UIEVENT_MOUSEMOVE;
                event.sender = NULL;
                pNewHover->Event(event);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            // We alway set focus back to our app (this helps
            // when Win32 child windows are placed on the dialog
            // and we need to remove them on focus change).
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            ControlUI* ctrl = FindControl(pt);
            if (ctrl == NULL)  break;
            if (ctrl->GetManager() != this)  break;
            m_eventClick = ctrl;
            ctrl->SetFocus();
            TEventUI event = { 0 };
            event.type = UIEVENT_BUTTONDOWN;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = wParam;
            event.timestamp = ::GetTickCount();
            ctrl->Event(event);
            // No need to burden user with 3D animations
            m_anim.CancelJobs();
            // We always capture the mouse
            ::SetCapture(m_hWndPaint);
        }
        break;
    case WM_LBUTTONUP:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            if (m_eventClick == NULL)  break;
            ::ReleaseCapture();
            TEventUI event = { 0 };
            event.type = UIEVENT_BUTTONUP;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = wParam;
            event.timestamp = ::GetTickCount();
            m_eventClick->Event(event);
            m_eventClick = NULL;
        }
        break;
    case WM_LBUTTONDBLCLK:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            ControlUI* ctrl = FindControl(pt);
            if (ctrl == NULL)  break;
            if (ctrl->GetManager() != this)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_DBLCLICK;
            event.ptMouse = pt;
            event.wKeyState = wParam;
            event.timestamp = ::GetTickCount();
            ctrl->Event(event);
            m_eventClick = ctrl;
            // We always capture the mouse
            ::SetCapture(m_hWndPaint);
        }
        break;
    case WM_CHAR:
        {
            if (m_focus == NULL)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_CHAR;
            event.chKey = wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.timestamp = ::GetTickCount();
            m_focus->Event(event);
        }
        break;
    case WM_KEYDOWN:
        {
            if (m_focus == NULL)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_KEYDOWN;
            event.chKey = wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.timestamp = ::GetTickCount();
            m_focus->Event(event);
            m_eventKey = m_focus;
        }
        break;
    case WM_KEYUP:
        {
            if (m_eventKey == NULL)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_KEYUP;
            event.chKey = wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.timestamp = ::GetTickCount();
            m_eventKey->Event(event);
            m_eventKey = NULL;
        }
        break;
    case WM_SETCURSOR:
        {
            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_hWndPaint, &pt);
            ControlUI* ctrl = FindControl(pt);
            if (ctrl == NULL)  break;
            if ((ctrl->GetControlFlags() & UIFLAG_SETCURSOR) == 0)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_SETCURSOR;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = MapKeyState();
            event.timestamp = ::GetTickCount();
            ctrl->Event(event);
        }
        return true;
    case WM_CTLCOLOREDIT:
        {
            ::DefWindowProc(m_hWndPaint, uMsg, wParam, lParam);
            HDC hDC = (HDC) wParam;
            ::SetTextColor(hDC, GetThemeColor(UICOLOR_EDIT_TEXT_NORMAL));
            ::SetBkColor(hDC, GetThemeColor(UICOLOR_EDIT_BACKGROUND_NORMAL));
            lRes = (LRESULT) GetThemeBrush(UICOLOR_EDIT_BACKGROUND_NORMAL);
        }
        return true;
    case WM_MEASUREITEM:
        {
            if (wParam == 0)  break;
            HWND hWndChild = ::GetDlgItem(m_hWndPaint, ((LPMEASUREITEMSTRUCT) lParam)->CtlID);
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_DRAWITEM:
        {
            if (wParam == 0)  break;
            HWND hWndChild = ((LPDRAWITEMSTRUCT) lParam)->hwndItem;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_VSCROLL:
        {
            if (lParam == NULL)  break;
            ContainerUI* container = static_cast<ContainerUI*>(::GetPropA((HWND) lParam, "WndX"));
            if (container == NULL)  break;
            TEventUI event = { 0 };
            event.type = UIEVENT_VSCROLL;
            event.wParam = wParam;
            event.lParam = lParam;
            event.timestamp = ::GetTickCount();
            container->Event(event);
        }
        break;
    case WM_NOTIFY:
        {
            LPNMHDR lpNMHDR = (LPNMHDR) lParam;
            if (lpNMHDR != NULL)  lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if (lParam == 0)  break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    default:
        // Handle WM_MOUSEWHEEL
        if ((uMsg == m_uMsgMouseWheel || uMsg == 0x020A) && m_focus != NULL) 
        {
            int zDelta = (int) (short) HIWORD(wParam);
            TEventUI event = { 0 };
            event.type = UIEVENT_SCROLLWHEEL;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
            event.timestamp = ::GetTickCount();
            m_focus->Event(event);
            // Simulate regular scrolling by sending scroll events
            event.type = UIEVENT_VSCROLL;
            for (int i = 0; i < abs(zDelta); i += 40)  m_focus->Event(event);
            // Let's make sure that the scroll item below the cursor is the same as before...
            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
        }
        break;
    }
    return false;
}

void PaintManagerUI::UpdateLayout()
{
    m_resizeNeeded = true;
    ::InvalidateRect(m_hWndPaint, NULL, FALSE);
}

void PaintManagerUI::Invalidate(RECT rcItem)
{
    ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool PaintManagerUI::AttachDialog(ControlUI* ctrl)
{
    ASSERT(::IsWindow(m_hWndPaint));
    // Reset any previous attachment
    SetFocus(NULL);
    m_eventKey = NULL;
    m_eventHover = NULL;
    m_eventClick = NULL;
    m_nameHash.Empty();
    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if (m_root != NULL)  {
        m_delayedCleanup.Add(m_root);
        ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
    }
    // Set the dialog root element
    m_root = ctrl;
    // Go ahead...
    m_resizeNeeded = true;
    m_firstLayout = true;
    m_focusNeeded = true;
    // Initiate all control
    return InitControls(ctrl);
}

bool PaintManagerUI::InitControls(ControlUI* ctrl, ControlUI* parent /*= NULL*/)
{
    ASSERT(ctrl);
    if (ctrl == NULL)  return false;
    ctrl->SetManager(this, parent != NULL ? parent : ctrl->GetParent());
    // We're usually initializing the control after adding some more of them to the tree,
    // and thus this would be a good time to request the name-map rebuilt.
    m_nameHash.Empty();
    return true;
}

void PaintManagerUI::ReapObjects(ControlUI* ctrl)
{
    if (ctrl == m_eventKey)  m_eventKey = NULL;
    if (ctrl == m_eventHover)  m_eventHover = NULL;
    if (ctrl == m_eventClick)  m_eventClick = NULL;
    // TODO: Do something with name-hash-map
    //m_nameHash.Empty();
}

void PaintManagerUI::MessageLoop()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0))  {
        if (!PaintManagerUI::TranslateMessage(&msg))  {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

bool PaintManagerUI::TranslateMessage(const MSG* pMsg)
{
    // Pretranslate Message takes care of system-wide messages, such as
    // tabbing and shortcut key-combos. We'll look for all messages for
    // each window and any child control attached.
    HWND hwndParent = ::GetParent(pMsg->hwnd);
    UINT uStyle = GetWindowStyle(pMsg->hwnd);
    LRESULT lRes = 0;
    for (int i = 0; i < m_preMessages.GetSize(); i++)  {
        PaintManagerUI* pT = static_cast<PaintManagerUI*>(m_preMessages[i]);
        if (pMsg->hwnd == pT->GetPaintWindow()
            || (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0))) 
        {
            if (pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes))  return true;
        }
    }
    return false;
}

bool PaintManagerUI::AddAnimJob(const AnimJobUI& job)
{
    AnimJobUI* jobTmp = new AnimJobUI(job);
    if (jobTmp == NULL)  return false;
    ::InvalidateRect(m_hWndPaint, NULL, FALSE);
    return m_anim.AddJob(jobTmp);
}

bool PaintManagerUI::AddPostPaintBlit(const TPostPaintUI& job)
{
    return m_postPaint.Add(&job);
}

ControlUI* PaintManagerUI::GetFocus() const
{
    return m_focus;
}

void PaintManagerUI::SetFocus(ControlUI* ctrl)
{
    // Paint manager window has focus?
    if (::GetFocus() != m_hWndPaint)  ::SetFocus(m_hWndPaint);
    // Already has focus?
    if (ctrl == m_focus)  return;
    // Remove focus from old control
    if (m_focus != NULL)  
    {
        TEventUI event = { 0 };
        event.type = UIEVENT_KILLFOCUS;
        event.sender = ctrl;
        event.timestamp = ::GetTickCount();
        m_focus->Event(event);
        SendNotify(m_focus, "killfocus");
        m_focus = NULL;
    }
    // Set focus to new control
    if (ctrl != NULL 
        && ctrl->GetManager() == this 
        && ctrl->IsVisible() 
        && ctrl->IsEnabled())  
    {
        m_focus = ctrl;
        TEventUI event = { 0 };
        event.type = UIEVENT_SETFOCUS;
        event.sender = ctrl;
        event.timestamp = ::GetTickCount();
        m_focus->Event(event);
        SendNotify(m_focus, "setfocus");
    }
}

bool PaintManagerUI::SetTimer(ControlUI* ctrl, UINT timerID, UINT uElapse)
{
    ASSERT(ctrl!=NULL);
    ASSERT(uElapse>0);
    m_timerID = (++m_timerID) % 0xFF;
    if (!::SetTimer(m_hWndPaint, m_timerID, uElapse, NULL))  return FALSE;
    TIMERINFO* timer = new TIMERINFO;
    if (timer == NULL)  return FALSE;
    timer->hWnd = m_hWndPaint;
    timer->sender = ctrl;
    timer->localID = timerID;
    timer->uWinTimer = m_timerID;
    return m_timers.Add(timer);
}

bool PaintManagerUI::KillTimer(ControlUI* ctrl, UINT timerID)
{
    ASSERT(ctrl!=NULL);
    for (int i = 0; i< m_timers.GetSize(); i++)  {
        TIMERINFO* timer = static_cast<TIMERINFO*>(m_timers[i]);
        if (timer->sender == ctrl
            && timer->hWnd == m_hWndPaint
            && timer->localID == timerID) 
        {
            ::KillTimer(timer->hWnd, timer->uWinTimer);
            delete timer;
            return m_timers.Remove(i);
        }
    }
    return false;
}

bool PaintManagerUI::SetNextTabControl(bool bForward)
{
    // If we're in the process of restructuring the layout we can delay the
    // focus calulation until the next repaint.
    if (m_resizeNeeded && bForward)  {
        m_focusNeeded = true;
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return true;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.focus = m_focus;
    info1.bForward = bForward;
    ControlUI* ctrl = m_root->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if (ctrl == NULL)  {  
        if (bForward)  {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.focus = bForward ? NULL : info1.pLast;
            info2.bForward = bForward;
            ctrl = m_root->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        } else {
            ctrl = info1.pLast;
        }
    } else {
        SetFocus(ctrl);
    }
    m_focusNeeded = false;
    return true;
}

TSystemSettingsUI PaintManagerUI::GetSystemSettings() const
{
    return m_SystemConfig;
}

void PaintManagerUI::SetSystemSettings(const TSystemSettingsUI Config)
{
    m_SystemConfig = Config;
}

TSystemMetricsUI PaintManagerUI::GetSystemMetrics() const
{
    return m_SystemMetrics;
}

bool PaintManagerUI::AddNotifier(INotifyUI* pNotifier)
{
    ASSERT(m_notifiers.Find(pNotifier)<0);
    return m_notifiers.Add(pNotifier);
}

bool PaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
{
    for (int i = 0; i < m_notifiers.GetSize(); i++)  {
        if (static_cast<INotifyUI*>(m_notifiers[i]) == pNotifier)  {
            return m_notifiers.Remove(i);
        }
    }
    return false;
}

bool PaintManagerUI::AddMessageFilter(IMessageFilterUI* filter)
{
    ASSERT(m_messageFilters.Find(filter)<0);
    return m_messageFilters.Add(filter);
}

bool PaintManagerUI::RemoveMessageFilter(IMessageFilterUI* filter)
{
    for (int i = 0; i < m_messageFilters.GetSize(); i++)  {
        if (static_cast<IMessageFilterUI*>(m_messageFilters[i]) == filter)  {
            return m_messageFilters.Remove(i);
        }
    }
    return false;
}

// Note: we assume that msg has infinite lifetime
// TODO: replace msg with msgId, have a bunch of known ids and ability
// to register new ones (as a string)
void PaintManagerUI::SendNotify(ControlUI* ctrl, const char* msg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    TNotifyUI Msg;
    Msg.sender = ctrl;
    Msg.type = msg;
    Msg.wParam = 0;
    Msg.lParam = 0;
    SendNotify(Msg);
}

void PaintManagerUI::SendNotify(TNotifyUI& Msg)
{
    // Pre-fill some standard members
    Msg.ptMouse = m_ptLastMousePos;
    Msg.timestamp = ::GetTickCount();
    // Allow sender control to react
    Msg.sender->Notify(Msg);
    // Send to all listeners
    for (int i = 0; i < m_notifiers.GetSize(); i++)  {
        static_cast<INotifyUI*>(m_notifiers[i])->Notify(Msg);
    }
}

HFONT PaintManagerUI::GetThemeFont(UITYPE_FONT idx) const
{
    if (idx <= UIFONT__FIRST || idx >= UIFONT__LAST)  return NULL;
    if (m_hFonts[idx] == NULL)  m_hFonts[idx] = ::CreateFontIndirect(&m_aLogFonts[idx]);
    return m_hFonts[idx];
}

HICON PaintManagerUI::GetThemeIcon(int idx, int cxySize) const
{
    if (m_himgIcons16 == NULL)  {
        m_himgIcons16 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
        m_himgIcons24 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
        m_himgIcons32 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
        m_himgIcons50 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS50), 50, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
    }
    if (cxySize == 16)  return ImageList_GetIcon(m_himgIcons16, idx, ILD_NORMAL);
    else if (cxySize == 24)  return ImageList_GetIcon(m_himgIcons24, idx, ILD_NORMAL);
    else if (cxySize == 32)  return ImageList_GetIcon(m_himgIcons32, idx, ILD_NORMAL);
    else if (cxySize == 50)  return ImageList_GetIcon(m_himgIcons50, idx, ILD_NORMAL);
    return NULL;
}

HPEN PaintManagerUI::GetThemePen(UITYPE_COLOR idx) const
{
    if (idx <= UICOLOR__FIRST || idx >= UICOLOR__LAST)  return NULL;
    if (m_hPens[idx] == NULL)  m_hPens[idx] = ::CreatePen(PS_SOLID, 1, m_colors[idx][0]);
    return m_hPens[idx];
}

HBRUSH PaintManagerUI::GetThemeBrush(UITYPE_COLOR idx) const
{
    if (idx <= UICOLOR__FIRST || idx >= UICOLOR__LAST)  return NULL;
    if (m_hBrushes[idx] == NULL)  m_hBrushes[idx] = ::CreateSolidBrush(m_colors[idx][0]);
    return m_hBrushes[idx];
}

const TEXTMETRIC& PaintManagerUI::GetThemeFontInfo(UITYPE_FONT idx) const
{
    if (idx <= UIFONT__FIRST || idx >= UIFONT__LAST)  return m_aTextMetrics[0];
    if (m_aTextMetrics[idx].tmHeight == 0)  {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, GetThemeFont(idx));
        ::GetTextMetrics(m_hDcPaint, &m_aTextMetrics[idx]);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return m_aTextMetrics[idx];
}

COLORREF PaintManagerUI::GetThemeColor(UITYPE_COLOR idx) const
{
    if (idx <= UICOLOR__FIRST || idx >= UICOLOR__LAST)  return RGB(0,0,0);
    return m_colors[idx][0];
}

bool PaintManagerUI::GetThemeColorPair(UITYPE_COLOR idx, COLORREF& clr1, COLORREF& clr2) const
{
    if (idx <= UICOLOR__FIRST || idx >= UICOLOR__LAST)  return false;
    clr1 = m_colors[idx][0];
    clr2 = m_colors[idx][1];
    return true;
}

ControlUI* PaintManagerUI::FindControl(const char* name)
{
    ASSERT(m_root);
    // First time here? Build hash array...
    if (m_nameHash.GetSize() == 0)  {
        int nCount = 0;
        m_root->FindControl(__FindControlFromCount, &nCount, UIFIND_ALL);
        m_nameHash.Resize(nCount + (nCount / 10));
        m_root->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
    }
    // Find name in the hash array
    int nCount = 0;
    int nSize = m_nameHash.GetSize();
    int iNameHash = (int) (GetNameHash(name) % nSize);
    while (m_nameHash[iNameHash] != NULL)  {
        const char *ctrlName = static_cast<ControlUI*>(m_nameHash[iNameHash])->GetName();
        if (str::Eq(ctrlName, name))
            return static_cast<ControlUI*>(m_nameHash[iNameHash]);
        iNameHash = (iNameHash + 1) % nSize;
        if (++nCount >= nSize)  break;
    }
    return NULL;
}

ControlUI* PaintManagerUI::FindControl(POINT pt) const
{
    ASSERT(m_root);
    return m_root->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST);
}

ControlUI* CALLBACK PaintManagerUI::__FindControlFromCount(ControlUI* /*pThis*/, void* data)
{
    int* pnCount = static_cast<int*>(data);
    (*pnCount)++;
    return NULL;  // Count all controls
}

ControlUI* CALLBACK PaintManagerUI::__FindControlFromTab(ControlUI* pThis, void* data)
{
    FINDTABINFO* info = static_cast<FINDTABINFO*>(data);
    if (info->focus == pThis)  {
        if (info->bForward)  info->bNextIsIt = true;
        return info->bForward ? NULL : info->pLast;
    }
    if ((pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0)  return NULL;
    info->pLast = pThis;
    if (info->bNextIsIt)  return pThis;
    if (info->focus == NULL)  return pThis;
    return NULL;  // Examine all controls
}

ControlUI* CALLBACK PaintManagerUI::__FindControlFromNameHash(ControlUI* pThis, void* data)
{
    PaintManagerUI* manager = static_cast<PaintManagerUI*>(data);
    // No name?
    const char *name = pThis->GetName();
    if (NULL == name)  return NULL;
    // Add this control to the hash list
    int nCount = 0;
    int nSize = manager->m_nameHash.GetSize();
    int iNameHash = (int) (GetNameHash(name) % nSize);
    while (manager->m_nameHash[iNameHash] != NULL)  {
        iNameHash = (iNameHash + 1) % nSize;
        if (++nCount == nSize)  return NULL;
    }
    manager->m_nameHash.SetAt(iNameHash, pThis);
    return NULL; // Attempt to add all controls
}

ControlUI* CALLBACK PaintManagerUI::__FindControlFromShortcut(ControlUI* pThis, void* data)
{
    FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(data);
    if (pFS->ch == toupper(pThis->GetShortcut()))  pFS->bPickNext = true;
    if (str::Find(pThis->GetClass(), "Label") != NULL)  return NULL;   // Labels never get focus!
    return pFS->bPickNext ? pThis : NULL;
}

ControlUI* CALLBACK PaintManagerUI::__FindControlFromPoint(ControlUI* pThis, void* data)
{
    LPPOINT pPoint = static_cast<LPPOINT>(data);
    return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
}

ControlUI::ControlUI() : 
m_manager(NULL), 
    m_parent(NULL), 
    m_name(NULL),
    m_txt(NULL),
    m_toolTip(NULL),
    m_tag(NULL),
    m_chShortcut('\0'),
    m_visible(true), 
    m_focused(false),
    m_enabled(true)
{
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
}

ControlUI::~ControlUI()
{
    if (m_manager != NULL)  m_manager->ReapObjects(this);
    free((void*)m_name);
    free((void*)m_txt);
    free((void*)m_toolTip);
}

bool ControlUI::IsVisible() const
{
    return m_visible;
}

bool ControlUI::IsEnabled() const
{
    return m_enabled;
}

bool ControlUI::IsFocused() const
{
    return m_focused;
}

UINT ControlUI::GetControlFlags() const
{
    return 0;
}

void ControlUI::SetVisible(bool visible)
{
    if (m_visible == visible)  return;
    m_visible = visible;
    if (m_manager != NULL)  m_manager->UpdateLayout();
}

void ControlUI::SetEnabled(bool enabled)
{
    m_enabled = enabled;
    Invalidate();
}

bool ControlUI::Activate()
{
    if (!IsVisible())  return false;
    if (!IsEnabled())  return false;
    return true;
}

ControlUI* ControlUI::GetParent() const
{
    return m_parent;
}

void ControlUI::SetFocus()
{
    if (m_manager != NULL)  m_manager->SetFocus(this);
}

void ControlUI::SetShortcut(char ch)
{
    m_chShortcut = ch;
}

char ControlUI::GetShortcut() const
{
    return m_chShortcut;
}

const char * ControlUI::GetText() const
{
    return m_txt;
}

void ControlUI::SetText(const char* txt)
{
    str::Replace(m_txt, txt);
    Invalidate();
}

UINT_PTR ControlUI::GetTag() const
{
    return m_tag;
}

void ControlUI::SetTag(UINT_PTR pTag)
{
    m_tag = pTag;
}

void ControlUI::SetToolTip(const char* txt)
{
    str::Replace(m_toolTip, txt);
}

const char* ControlUI::GetToolTip() const
{
    return m_toolTip;
}

void ControlUI::Init()
{
}

PaintManagerUI* ControlUI::GetManager() const
{
    return m_manager;
}

void ControlUI::SetManager(PaintManagerUI* manager, ControlUI* parent)
{
    bool bInit = (m_manager == NULL);
    m_manager = manager;
    m_parent = parent;
    if (bInit)
        Init();
}

const char* ControlUI::GetName() const
{
    return m_name;
}

void ControlUI::SetName(const char* name)
{
    str::Replace(m_name, name);
}

void* ControlUI::GetInterface(const char* name)
{
    if (str::Eq(name, "Control"))
        return this;
    return NULL;
}

ControlUI* ControlUI::FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags)
{
    if (FlSet(uFlags, UIFIND_VISIBLE) && !IsVisible())  return NULL;
    if (FlSet(uFlags, UIFIND_ENABLED) && !IsEnabled())  return NULL;
    if (FlSet(uFlags, UIFIND_HITTEST) && !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(data)))  return NULL;
    return Proc(this, data);
}

RECT ControlUI::GetPos() const
{
    return m_rcItem;
}

void ControlUI::SetPos(RECT rc)
{
    m_rcItem = rc;
    // NOTE: SetPos() is usually called during the WM_PAINT cycle where all controls are
    //       being laid out. Calling UpdateLayout() again would be wrong. Refreshing the
    //       window won't hurt (if we're already inside WM_PAINT we'll just validate it out).
    Invalidate();
}

void ControlUI::Invalidate()
{
    if (m_manager != NULL)  m_manager->Invalidate(m_rcItem);
}

void ControlUI::UpdateLayout()
{
    if (m_manager != NULL)  m_manager->UpdateLayout();
}

void ControlUI::Event(TEventUI& event)
{
    if (event.type == UIEVENT_SETCURSOR) 
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }
    if (event.type == UIEVENT_SETFOCUS)  
    {
        m_focused = true;
        Invalidate();
        return;
    }
    if (event.type == UIEVENT_KILLFOCUS)  
    {
        m_focused = false;
        Invalidate();
        return;
    }
    if (event.type == UIEVENT_TIMER) 
    {
        m_manager->SendNotify(this, "timer", event.wParam, event.lParam);
        return;
    }
    if (m_parent != NULL)  m_parent->Event(event);
}

void ControlUI::Notify(TNotifyUI& /*msg*/)
{
}

void ControlUI::SetAttribute(const char* name, const char* value)
{
    if (str::Eq(name, "pos"))  {
        RECT rcPos = { 0 };
        char* pstr = NULL;
        rcPos.left = strtol(value, &pstr, 10);      ASSERT(pstr);    
        rcPos.top = strtol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = strtol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = strtol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetPos(rcPos);
    }
    else if (str::Eq(name, "name"))     SetName(value);
    else if (str::Eq(name, "text"))     SetText(value);
    else if (str::Eq(name, "tooltip"))  SetToolTip(value);
    else if (str::Eq(name, "enabled"))  SetEnabled(str::Eq(value, "true"));
    else if (str::Eq(name, "visible"))  SetVisible(str::Eq(value, "true"));
    else if (str::Eq(name, "shortcut")) SetShortcut(value[0]);
}

// handle xml attribute lists in the form:
// foo="value of foo",bar="value of bar"
ControlUI* ControlUI::ApplyAttributeList(const char* attrList)
{
    ScopedMem<char> attrListCopy(str::Dup(attrList));
    char *s = attrListCopy;
    char *name, *value;
    while (*s)  {
        while (*s == ' ')
            ++s;
        name= s;
        while (*s && *s != '=')
            ++s;
        ASSERT(*s=='=');
        if (*s++ != '=')
            break;
        s[-1] = 0; // zero-out '='
        ASSERT(*s=='\"');
        if (*s++ != '\"')
            break;
        value = s;
        while (*s && *s != '\"')
            ++s;
        ASSERT(*s=='\"');
        if (*s++ != '\"')
            break;
        s[-1] = 0; // zero-out '"'
        SetAttribute(name, value);
        if (*s++ != ',')
            break;
    }
    return this;
}
