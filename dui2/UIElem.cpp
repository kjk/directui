#include "UIElem.h"

namespace dui {

static bool PreTranslateMessage(const MSG* pMsg)
{
#if 0
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
            if (pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes))
                return true;
        }
    }
#endif
    return false;
}

void MessageLoop()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0))  {
        if (!PreTranslateMessage(&msg))  {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

}
