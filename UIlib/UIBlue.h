#if !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
#define AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_

class UILIB_API RenderClip
{
public:
    ~RenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;
};

class UILIB_API BlueRenderEngineUI
{
public:
    static void DoFillRect(HDC hDC, PaintManagerUI* manager, RECT rc, UITYPE_COLOR Color);
    static void DoFillRect(HDC hDC, PaintManagerUI* manager, RECT rc, COLORREF colFill);
    static void DoPaintLine(HDC hDC, PaintManagerUI* manager, RECT rc, UITYPE_COLOR Color);
    static void DoPaintRectangle(HDC hDC, PaintManagerUI* manager, RECT rcItem, UITYPE_COLOR Border, UITYPE_COLOR Fill);
    static void DoPaintPanel(HDC hDC, PaintManagerUI* manager, RECT rc);
    static void DoPaintFrame(HDC hDC, PaintManagerUI* manager, RECT rc, UITYPE_COLOR Light, UITYPE_COLOR Dark, UITYPE_COLOR Background = UICOLOR__INVALID, UINT uStyle = 0);
    static void DoPaintBitmap(HDC hDC, PaintManagerUI* manager, HBITMAP hBmp, RECT rcBitmap);
    static void DoPaintArcCaption(HDC hDC, PaintManagerUI* manager, RECT rc, const char* txt, UINT uStyle);
    static void DoPaintButton(HDC hDC, PaintManagerUI* manager, RECT rc, const char* txt, RECT rcPadding, UINT uState, UINT uDrawStyle);
    static void DoPaintEditBox(HDC hDC, PaintManagerUI* manager, RECT rcItem, const char* txt, UINT uState, UINT uDrawStyle, bool bPaintFrameOnly);
    static void DoPaintOptionBox(HDC hDC, PaintManagerUI* manager, RECT rcItem, const char* txt, UINT uState, UINT uStyle);
    static void DoPaintTabFolder(HDC hDC, PaintManagerUI* manager, RECT& rc, const char* txt, UINT uState);
    static void DoPaintToolbarButton(HDC hDC, PaintManagerUI* manager, RECT rc, const char* txt, SIZE szPadding, UINT uState);
    static void DoPaintQuickText(HDC hDC, PaintManagerUI* manager, RECT& rc, const char* txt, UITYPE_COLOR iTextColor, UITYPE_FONT iFont, UINT uStyle);
    static void DoPaintPrettyText(HDC hDC, PaintManagerUI* manager, RECT& rc, const char* txt, UITYPE_COLOR iTextColor, UITYPE_COLOR iBackColor, RECT* pLinks, int& nLinkRects, UINT uStyle);
    static void DoPaintGradient(HDC hDC, PaintManagerUI* manager, RECT rc, COLORREF clrFirst, COLORREF clrSecond, bool bVertical, int nSteps);
    static void DoPaintAlphaBitmap(HDC hDC, PaintManagerUI* manager, HBITMAP hBitmap, RECT rc, BYTE iAlpha);
    static void DoAnimateWindow(HWND hWnd, UINT uStyle, DWORD dwTime = 200);
    static void GenerateClip(HDC hDC, RECT rcItem, RenderClip& clip);
    static HBITMAP GenerateAlphaBitmap(PaintManagerUI* manager, ControlUI* ctrl, RECT rc, UITYPE_COLOR Background);
};

#endif // !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
