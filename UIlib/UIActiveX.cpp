#include "StdAfx.h"
#include "UIActiveX.h"

class ActiveXCtrl;

class ActiveXWnd : public WindowWnd
{
public:
    HWND Init(ActiveXCtrl* owner, HWND hWndParent);

    virtual const char* GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void DoVerb(LONG iVerb);

    LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    ActiveXCtrl* m_owner;
};

class ActiveXEnum : public IEnumUnknown
{
public:
    ActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_pos(0)
    {
        m_pUnk->AddRef();
    }
    ~ActiveXEnum()
    {
        m_pUnk->Release();
    }

    LONG m_pos;
    ULONG m_dwRef;
    IUnknown* m_pUnk;

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        LONG lRef = --m_dwRef;
        if (lRef == 0)  delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if (riid == IID_IUnknown)  *ppvObject = static_cast<IEnumUnknown*>(this);
        else if (riid == IID_IEnumUnknown)  *ppvObject = static_cast<IEnumUnknown*>(this);
        if (*ppvObject != NULL)  AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }
    STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
    {
        if (pceltFetched != NULL)  *pceltFetched = 0;
        if (++m_pos > 1)  return S_FALSE;
        *rgelt = m_pUnk;
        (*rgelt)->AddRef();
        if (pceltFetched != NULL)  *pceltFetched = 1;
        return S_OK;
    }
    STDMETHOD(Skip)(ULONG celt)
    {
        m_pos += celt;
        return S_OK;
    }
    STDMETHOD(Reset)(void)
    {
        m_pos = 0;
        return S_OK;
    }
    STDMETHOD(Clone)(IEnumUnknown **ppenum)
    {
        return E_NOTIMPL;
    }
};

class ActiveXFrameWnd : public IOleInPlaceFrame
{
public:
    ActiveXFrameWnd(ActiveXUI* owner) : m_dwRef(1), m_owner(owner), m_pActiveObject(NULL)
    {
    }
    ~ActiveXFrameWnd()
    {
        if (m_pActiveObject != NULL)  m_pActiveObject->Release();
    }

    ULONG m_dwRef;
    ActiveXUI* m_owner;
    IOleInPlaceActiveObject* m_pActiveObject;

    // IUnknown
    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        ULONG lRef = --m_dwRef;
        if (lRef == 0)  delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if (riid == IID_IUnknown)  *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if (riid == IID_IOleWindow)  *ppvObject = static_cast<IOleWindow*>(this);
        else if (riid == IID_IOleInPlaceFrame)  *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if (riid == IID_IOleInPlaceUIWindow)  *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
        if (*ppvObject != NULL)  AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }  
    // IOleInPlaceFrameWindow
    STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
    {
        return S_OK;
    }
    STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
    {
        return S_OK;
    }
    STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
    {
        return S_OK;
    }
    STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
    {
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(MSG* /*lpMsg*/, WORD /*wID*/)
    {
        return S_FALSE;
    }
    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if (m_owner == NULL)  return E_UNEXPECTED;
        *phwnd = m_owner->GetManager()->GetPaintWindow();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
    {
        return S_OK;
    }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return INPLACE_E_NOTOOLSPACE;
    }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
    {
        if (pActiveObject != NULL)  pActiveObject->AddRef();
        if (m_pActiveObject != NULL)  m_pActiveObject->Release();
        m_pActiveObject = pActiveObject;
        return S_OK;
    }
};

class ActiveXCtrl :
    public IOleClientSite,
    public IOleInPlaceSiteWindowless,
    public IOleControlSite,
    public IObjectWithSite,
    public IOleContainer
{
    friend ActiveXUI;
    friend ActiveXWnd;
public:
    ActiveXCtrl();
    ~ActiveXCtrl();

    // IUnknown
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

    // IObjectWithSite
    STDMETHOD(SetSite)(IUnknown *pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    // IOleClientSite
    STDMETHOD(SaveObject)(void);       
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
    STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
    STDMETHOD(ShowObject)(void);        
    STDMETHOD(OnShowWindow)(BOOL fShow);        
    STDMETHOD(RequestNewObjectLayout)(void);

    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void);
    STDMETHOD(GetCapture)(void);
    STDMETHOD(SetCapture)(BOOL fCapture);
    STDMETHOD(GetFocus)(void);
    STDMETHOD(SetFocus)(BOOL fFocus);
    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
    STDMETHOD(ReleaseDC)(HDC hDC);
    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
    STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
    STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
    STDMETHOD(RequestUIActivate)(void);

    // IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)(void);       
    STDMETHOD(OnInPlaceActivate)(void);        
    STDMETHOD(OnUIActivate)(void);
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtant);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)(void);
    STDMETHOD(DiscardUndoState)( void);
    STDMETHOD(DeactivateAndUndo)( void);
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void);      
    STDMETHOD(LockInPlaceActive)(BOOL fLock);       
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
    STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
    STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
    STDMETHOD(OnFocus)(BOOL fGotFocus);
    STDMETHOD(ShowPropertyFrame)(void);

    // IOleContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
    STDMETHOD(LockContainer)(BOOL fLock);

    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

protected:
    HRESULT CreateActiveXWnd();

protected:
    LONG m_dwRef;
    ActiveXUI* m_owner;
    ActiveXWnd* m_win;
    IUnknown* m_pUnkSite;
    IViewObject* m_pViewObject;
    IOleInPlaceObjectWindowless* m_pInPlaceObject;
    bool m_bLocked;
    bool m_focused;
    bool m_bCaptured;
    bool m_bUIActivated;
    bool m_bInPlaceActive;
    bool m_bWindowless;
};


ActiveXCtrl::ActiveXCtrl() : 
m_dwRef(1), 
    m_owner(NULL), 
    m_win(NULL),
    m_pUnkSite(NULL), 
    m_pViewObject(NULL),
    m_pInPlaceObject(NULL),
    m_bLocked(false), 
    m_focused(false),
    m_bCaptured(false),
    m_bWindowless(true),
    m_bUIActivated(false),
    m_bInPlaceActive(false)
{
}

ActiveXCtrl::~ActiveXCtrl()
{
    if (m_win != NULL)  ::DestroyWindow(*m_win);
    if (m_pUnkSite != NULL)  m_pUnkSite->Release();
    if (m_pViewObject != NULL)  m_pViewObject->Release();
    if (m_pInPlaceObject != NULL)  m_pInPlaceObject->Release();
}

STDMETHODIMP ActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    *ppvObject = NULL;
    if (riid == IID_IUnknown)                        *ppvObject = static_cast<IOleWindow*>(this);
    else if (riid == IID_IOleClientSite)             *ppvObject = static_cast<IOleClientSite*>(this);
    else if (riid == IID_IOleInPlaceSiteWindowless)  *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    else if (riid == IID_IOleInPlaceSiteEx)          *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
    else if (riid == IID_IOleInPlaceSite)            *ppvObject = static_cast<IOleInPlaceSite*>(this);
    else if (riid == IID_IOleWindow)                 *ppvObject = static_cast<IOleWindow*>(this);
    else if (riid == IID_IOleControlSite)            *ppvObject = static_cast<IOleControlSite*>(this);
    else if (riid == IID_IOleContainer)              *ppvObject = static_cast<IOleContainer*>(this);
    else if (riid == IID_IObjectWithSite)            *ppvObject = static_cast<IObjectWithSite*>(this);
    if (*ppvObject != NULL)  AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) ActiveXCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) ActiveXCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if (lRef == 0)  delete this;
    return lRef;
}

STDMETHODIMP ActiveXCtrl::SetSite(IUnknown *pUnkSite)
{
    TRACE("AX: ActiveXCtrl::SetSite");
    if (m_pUnkSite != NULL)  {
        m_pUnkSite->Release();
        m_pUnkSite = NULL;
    }
    if (pUnkSite != NULL)  {
        m_pUnkSite = pUnkSite;
        m_pUnkSite->AddRef();
    }
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
    TRACE("AX: ActiveXCtrl::GetSite");
    if (ppvSite == NULL)  return E_POINTER;
    *ppvSite = NULL;
    if (m_pUnkSite == NULL)  return E_FAIL;
    return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP ActiveXCtrl::SaveObject(void)
{
    TRACE("AX: ActiveXCtrl::SaveObject");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    TRACE("AX: ActiveXCtrl::GetMoniker");
    if (ppmk != NULL)  *ppmk = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::GetContainer(IOleContainer** ppContainer)
{
    TRACE("AX: ActiveXCtrl::GetContainer");
    if (ppContainer == NULL)  return E_POINTER;
    *ppContainer = NULL;
    HRESULT Hr = E_NOTIMPL;
    if (m_pUnkSite != NULL)  Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    if (FAILED(Hr))  Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    return Hr;
}

STDMETHODIMP ActiveXCtrl::ShowObject(void)
{
    TRACE("AX: ActiveXCtrl::ShowObject");
    if (m_owner == NULL)  return E_UNEXPECTED;
    HDC hDC = ::GetDC(m_owner->m_hwndHost);
    if (hDC == NULL)  return E_FAIL;
    if (m_pViewObject != NULL)  m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_owner->m_rcItem, (RECTL*) &m_owner->m_rcItem, NULL, NULL);
    ::ReleaseDC(m_owner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnShowWindow(BOOL fShow)
{
    TRACE("AX: ActiveXCtrl::OnShowWindow");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::RequestNewObjectLayout(void)
{
    TRACE("AX: ActiveXCtrl::RequestNewObjectLayout");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::CanWindowlessActivate(void)
{
    TRACE("AX: ActiveXCtrl::CanWindowlessActivate");
    return S_OK;  // Yes, we can!!
}

STDMETHODIMP ActiveXCtrl::GetCapture(void)
{
    TRACE("AX: ActiveXCtrl::GetCapture");
    if (m_owner == NULL)  return E_UNEXPECTED;
    return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP ActiveXCtrl::SetCapture(BOOL fCapture)
{
    TRACE("AX: ActiveXCtrl::SetCapture");
    if (m_owner == NULL)  return E_UNEXPECTED;
    m_bCaptured = (fCapture == TRUE);
    if (fCapture)  ::SetCapture(m_owner->m_hwndHost); else ::ReleaseCapture();
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::GetFocus(void)
{
    TRACE("AX: ActiveXCtrl::GetFocus");
    if (m_owner == NULL)  return E_UNEXPECTED;
    return m_focused ? S_OK : S_FALSE;
}

STDMETHODIMP ActiveXCtrl::SetFocus(BOOL fFocus)
{
    TRACE("AX: ActiveXCtrl::SetFocus");
    if (m_owner == NULL)  return E_UNEXPECTED;
    if (fFocus)  m_owner->SetFocus();
    m_focused = (fFocus == TRUE);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
    TRACE("AX: ActiveXCtrl::GetDC");
    if (phDC == NULL)  return E_POINTER;
    if (m_owner == NULL)  return E_UNEXPECTED;
    *phDC = ::GetDC(m_owner->m_hwndHost);
    if (IsFlSet(grfFlags, OLEDC_PAINTBKGND))  {
        CRect rcItem = m_owner->GetPos();
        if (!m_bWindowless)  rcItem.ResetOffset();
        ::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
    }
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::ReleaseDC(HDC hDC)
{
    TRACE("AX: ActiveXCtrl::ReleaseDC");
    if (m_owner == NULL)  return E_UNEXPECTED;
    ::ReleaseDC(m_owner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    TRACE("AX: ActiveXCtrl::InvalidateRect");
    if (m_owner == NULL)  return E_UNEXPECTED;
    if (m_owner->m_hwndHost == NULL)  return E_FAIL;
    return ::InvalidateRect(m_owner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP ActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    TRACE("AX: ActiveXCtrl::InvalidateRgn");
    if (m_owner == NULL)  return E_UNEXPECTED;
    return ::InvalidateRgn(m_owner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP ActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    TRACE("AX: ActiveXCtrl::ScrollRect");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::AdjustRect(LPRECT prc)
{
    TRACE("AX: ActiveXCtrl::AdjustRect");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    TRACE("AX: ActiveXCtrl::OnDefWindowMessage");
    if (m_owner == NULL)  return E_UNEXPECTED;
    *plResult = ::DefWindowProc(m_owner->m_hwndHost, msg, wParam, lParam);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    TRACE("AX: ActiveXCtrl::OnInPlaceActivateEx");
    ASSERT(m_pInPlaceObject==NULL);
    if (m_owner == NULL)  return E_UNEXPECTED;
    if (m_owner->m_pUnk == NULL)  return E_UNEXPECTED;
    ::OleLockRunning(m_owner->m_pUnk, TRUE, FALSE);
    HWND hWndFrame = m_owner->GetManager()->GetPaintWindow();
    HRESULT Hr = E_FAIL;
    if (IsFlSet(dwFlags, ACTIVATE_WINDOWLESS))  {
        m_bWindowless = true;
        Hr = m_owner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
        m_owner->m_hwndHost = hWndFrame;
        m_owner->GetManager()->AddMessageFilter(m_owner);
    }
    if (FAILED(Hr))  {
        m_bWindowless = false;
        Hr = CreateActiveXWnd();
        if (FAILED(Hr))  return Hr;
        Hr = m_owner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
    }
    if (m_pInPlaceObject != NULL)  {
        CRect rcItem = m_owner->m_rcItem;
        if (!m_bWindowless)  rcItem.ResetOffset();
        m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    m_bInPlaceActive = SUCCEEDED(Hr);
    return Hr;
}

STDMETHODIMP ActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    TRACE("AX: ActiveXCtrl::OnInPlaceDeactivateEx");
    m_bInPlaceActive = false;
    if (m_pInPlaceObject != NULL)  {
        m_pInPlaceObject->Release();
        m_pInPlaceObject = NULL;
    }
    if (m_win != NULL)  {
        ::DestroyWindow(*m_win);
        m_win = NULL;
    }
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::RequestUIActivate(void)
{
    TRACE("AX: ActiveXCtrl::RequestUIActivate");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::CanInPlaceActivate(void)       
{
    TRACE("AX: ActiveXCtrl::CanInPlaceActivate");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnInPlaceActivate(void)
{
    TRACE("AX: ActiveXCtrl::OnInPlaceActivate");
    BOOL bDummy = FALSE;
    return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP ActiveXCtrl::OnUIActivate(void)
{
    TRACE("AX: ActiveXCtrl::OnUIActivate");
    m_bUIActivated = true;
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    TRACE("AX: ActiveXCtrl::GetWindowContext");
    if (ppDoc == NULL)  return E_POINTER;
    if (ppFrame == NULL)  return E_POINTER;
    if (lprcPosRect == NULL)  return E_POINTER;
    if (lprcClipRect == NULL)  return E_POINTER;
    *ppFrame = new ActiveXFrameWnd(m_owner);
    *ppDoc = NULL;
    ACCEL ac = { 0 };
    HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = m_owner->GetManager()->GetPaintWindow();
    lpFrameInfo->haccel = hac;
    lpFrameInfo->cAccelEntries = 1;
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::Scroll(SIZE scrollExtant)
{
    TRACE("AX: ActiveXCtrl::Scroll");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
{
    TRACE("AX: ActiveXCtrl::OnUIDeactivate");
    m_bUIActivated = false;
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnInPlaceDeactivate(void)
{
    TRACE("AX: ActiveXCtrl::OnInPlaceDeactivate");
    return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP ActiveXCtrl::DiscardUndoState(void)
{
    TRACE("AX: ActiveXCtrl::DiscardUndoState");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::DeactivateAndUndo(void)
{
    TRACE("AX: ActiveXCtrl::DeactivateAndUndo");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
    TRACE("AX: ActiveXCtrl::OnPosRectChange");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::GetWindow(HWND* phwnd)
{
    TRACE("AX: ActiveXCtrl::GetWindow");
    if (m_owner == NULL)  return E_UNEXPECTED;
    if (m_owner->m_hwndHost == NULL)  CreateActiveXWnd();
    if (m_owner->m_hwndHost == NULL)  return E_FAIL;
    *phwnd = m_owner->m_hwndHost;
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
    TRACE("AX: ActiveXCtrl::ContextSensitiveHelp");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::OnControlInfoChanged(void)      
{
    TRACE("AX: ActiveXCtrl::OnControlInfoChanged");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::LockInPlaceActive(BOOL fLock)       
{
    TRACE("AX: ActiveXCtrl::LockInPlaceActive");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
    TRACE("AX: ActiveXCtrl::GetExtendedControl");
    if (ppDisp == NULL)  return E_POINTER;   
    if (m_owner == NULL)  return E_UNEXPECTED;
    if (m_owner->m_pUnk == NULL)  return E_UNEXPECTED;
    return m_owner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP ActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    TRACE("AX: ActiveXCtrl::TransformCoords");
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    TRACE("AX: ActiveXCtrl::TranslateAccelerator");
    return S_FALSE;
}

STDMETHODIMP ActiveXCtrl::OnFocus(BOOL fGotFocus)
{
    TRACE("AX: ActiveXCtrl::OnFocus");
    m_focused = (fGotFocus == TRUE);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::ShowPropertyFrame(void)
{
    TRACE("AX: ActiveXCtrl::ShowPropertyFrame");
    return E_NOTIMPL;
}

STDMETHODIMP ActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    TRACE("AX: ActiveXCtrl::EnumObjects");
    if (ppenum == NULL)  return E_POINTER;
    if (m_owner == NULL)  return E_UNEXPECTED;
    *ppenum = new ActiveXEnum(m_owner->m_pUnk);
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::LockContainer(BOOL fLock)
{
    TRACE("AX: ActiveXCtrl::LockContainer");
    m_bLocked = fLock != FALSE;
    return S_OK;
}

STDMETHODIMP ActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    TRACE("AX: ActiveXCtrl::ParseDisplayName");
    return E_NOTIMPL;
}

HRESULT ActiveXCtrl::CreateActiveXWnd()
{
    if (m_win != NULL)  return S_OK;
    m_win = new ActiveXWnd;
    if (m_win == NULL)  return E_OUTOFMEMORY;
    m_owner->m_hwndHost = m_win->Init(this, m_owner->GetManager()->GetPaintWindow());
    return S_OK;
}

HWND ActiveXWnd::Init(ActiveXCtrl* owner, HWND hWndParent)
{
    m_owner = owner;
    UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    Create(hWndParent, "UIActiveX", uStyle, 0L, 0,0,0,0, NULL);
    return m_hWnd;
}

const char* ActiveXWnd::GetWindowClassName() const
{
    return "ActiveXWnd";
}

void ActiveXWnd::OnFinalMessage(HWND hWnd)
{
    delete this;
}

void ActiveXWnd::DoVerb(LONG iVerb)
{
    if (m_owner == NULL)  return;
    if (m_owner->m_owner == NULL)  return;
    IOleObject* pUnk = NULL;
    m_owner->m_owner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if (pUnk == NULL)  return;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    IOleClientSite* pOleClientSite = NULL;
    m_owner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_owner->m_owner->GetPos());
}

LRESULT ActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes;
    BOOL bHandled = TRUE;
    switch (uMsg)  {
    case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
    case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
    case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
    default:
        bHandled = FALSE;
    }
    if (!bHandled)  return WindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT ActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_owner->m_pViewObject == NULL)  bHandled = FALSE;
    return 1;
}

LRESULT ActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IOleObject* pUnk = NULL;
    m_owner->m_owner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if (pUnk == NULL)  return 0;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    DWORD dwMiscStatus = 0;
    pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    if (IsFlSet(dwMiscStatus, OLEMISC_NOUIACTIVATE))  return 0;
    if (!m_owner->m_bInPlaceActive)  DoVerb(OLEIVERB_INPLACEACTIVATE);
    bHandled = FALSE;
    return 0;
}

LRESULT ActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_owner->m_focused = true;
    if (!m_owner->m_bUIActivated)  DoVerb(OLEIVERB_UIACTIVATE);
    return 0;
}

LRESULT ActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_owner->m_focused = false;
    return 0;
}

LRESULT ActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    return 1;
}

ActiveXUI::ActiveXUI() : m_pUnk(NULL), m_ctrl(NULL), m_hwndHost(NULL), m_bCreated(false)
{
    m_szFixed.cx = m_szFixed.cy = 0;
}

ActiveXUI::~ActiveXUI()
{
    ReleaseControl();
}

const char* ActiveXUI::GetClass() const
{
    return "ActiveXUI";
}

SIZE ActiveXUI::EstimateSize(SIZE szAvailable)
{
    return m_szFixed;
}

static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(NULL);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

void ActiveXUI::SetPos(RECT rc)
{
    if (!m_bCreated)  DelayedControlCreation();

    if (m_pUnk == NULL)  return;
    if (m_ctrl == NULL)  return;

    m_rcItem = rc;

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = RectDx(m_rcItem);
    pxSize.cy = RectDy(m_rcItem);
    PixelToHiMetric(&pxSize, &hmSize);

    if (m_pUnk != NULL)  {
        m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
    }
    if (m_ctrl->m_pInPlaceObject != NULL)  {
        CRect rcItem = m_rcItem;
        if (!m_ctrl->m_bWindowless)  rcItem.ResetOffset();
        m_ctrl->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    if (!m_ctrl->m_bWindowless)  {
        ASSERT(m_ctrl->m_win);
        ::MoveWindow(*m_ctrl->m_win, m_rcItem.left, m_rcItem.top, RectDx(m_rcItem), RectDy(m_rcItem), TRUE);
    }
}

void ActiveXUI::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
    if (m_ctrl != NULL && m_ctrl->m_bWindowless && m_ctrl->m_pViewObject != NULL) 
    {
        m_ctrl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_rcItem, (RECTL*) &m_rcItem, NULL, NULL); 
    }
}

void ActiveXUI::SetAttribute(const char* name, const char* value)
{
    int n;
    if (str::Eq(name, "clsid"))  CreateControl(value);
    else if (ParseWidth(name, value, n))
        SetWidth(n);
    else if (ParseHeight(name, value, n))
        SetHeight(n);
    else ControlUI::SetAttribute(name, value);
}

LRESULT ActiveXUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (m_ctrl == NULL)  return 0;
    ASSERT(m_ctrl->m_bWindowless);
    if (!m_ctrl->m_bInPlaceActive)  return 0;
    if (m_ctrl->m_pInPlaceObject == NULL)  return 0;
    bool bWasHandled = true;
    if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR) 
    {
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_ctrl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if (dwHitResult == HITRESULT_OUTSIDE && m_ctrl->m_pViewObject != NULL)  {
            IViewObjectEx* pViewEx = NULL;
            m_ctrl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
            if (pViewEx != NULL)  {
                POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
                pViewEx->Release();
            }
        }
        if (dwHitResult != HITRESULT_HIT)  return 0;
        if (uMsg == WM_SETCURSOR)  bWasHandled = false;
    }
    else if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) 
    {
        // Keyboard messages just go when we have focus
        if (!m_ctrl->m_focused)  return 0;
    }
    else
    {
        switch (uMsg)  {
        case WM_HELP:
        case WM_CONTEXTMENU:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    HRESULT Hr = m_ctrl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
    if (Hr == S_OK)  bHandled = bWasHandled;
    return lResult;
}

void ActiveXUI::SetWidth(int cx)
{
    m_szFixed.cx = cx;
}

void ActiveXUI::SetHeight(int cy)
{
    m_szFixed.cy = cy;
}

bool ActiveXUI::CreateControl(const char* pstrCLSID)
{
    CLSID clsid = { 0 };
    OLECHAR szCLSID[100] = { 0 };
    ASSERT(0);
#if 0 // TODO: impolement for unicode
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, dimof(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, pstrCLSID, dimof(szCLSID) - 1);
#endif
    if (pstrCLSID[0] == '{')  ::CLSIDFromString(szCLSID, &clsid);
    else ::CLSIDFromProgID(szCLSID, &clsid);
#endif
    return CreateControl(clsid);
}

bool ActiveXUI::CreateControl(const CLSID clsid)
{
    ASSERT(clsid!=IID_NULL);
    if (clsid == IID_NULL)  return false;
    // NOTE: We do *not* actually create the ActiveX at this point; we merely
    //       make a note of the CLSID and delays the creating until we know for 
    //       sure that the entire user-interface is running.
    m_bCreated = false;
    m_clsid = clsid;
    return true;
}

void ActiveXUI::ReleaseControl()
{
    m_hwndHost = NULL;
    if (m_pUnk != NULL)  {
        IObjectWithSite* pSite = NULL;
        m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
        if (pSite != NULL)  {
            pSite->SetSite(NULL);
            pSite->Release();
        }
        m_pUnk->Close(OLECLOSE_NOSAVE);
        m_pUnk->SetClientSite(NULL);
        m_pUnk->Release(); 
        m_pUnk = NULL;
    }
    if (m_ctrl != NULL)  {
        m_ctrl->m_owner = NULL;
        m_ctrl->Release();
        m_ctrl = NULL;
    }
    m_mgr->RemoveMessageFilter(this);
}

bool ActiveXUI::DelayedControlCreation()
{
    ReleaseControl();
    // At this point we'll create the ActiveX control
    m_bCreated = true;
    IOleControl* pOleControl = NULL;
    HRESULT Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*) &pOleControl);
    ASSERT(SUCCEEDED(Hr));
    if (FAILED(Hr))  return false;
    pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
    pOleControl->Release();
    if (m_pUnk == NULL)  return false;
    // Create the host too
    m_ctrl = new ActiveXCtrl();
    m_ctrl->m_owner = this;
    // More control creation stuff
    DWORD dwMiscStatus = 0;
    m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    IOleClientSite* pOleClientSite = NULL;
    m_ctrl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    // Initialize control
    if (IsFlSet(dwMiscStatus, OLEMISC_SETCLIENTSITEFIRST))  m_pUnk->SetClientSite(pOleClientSite);
    IPersistStreamInit* pPersistStreamInit = NULL;
    m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
    if (pPersistStreamInit != NULL)  {
        Hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }
    if (FAILED(Hr))  return false;
    if ((dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0)  m_pUnk->SetClientSite(pOleClientSite);
    // Grab the view...
    Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_ctrl->m_pViewObject);
    if (FAILED(Hr))  Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_ctrl->m_pViewObject);
    if (FAILED(Hr))  Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_ctrl->m_pViewObject);
    // Activate and done...
    m_pUnk->SetHostNames(OLESTR("UIActiveX"), NULL);
    if ((dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0)  {
        Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, m_mgr->GetPaintWindow(), &m_rcItem);
        ::RedrawWindow(m_mgr->GetPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
    }
    IObjectWithSite* pSite = NULL;
    m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
    if (pSite != NULL)  {
        pSite->SetSite(static_cast<IOleClientSite*>(m_ctrl));
        pSite->Release();
    }
    return SUCCEEDED(Hr);
}

HRESULT ActiveXUI::GetControl(const IID iid, LPVOID* ppRet)
{
    ASSERT(ppRet!=NULL);
    ASSERT(*ppRet==NULL);
    if (ppRet == NULL)  return E_POINTER;
    if (m_pUnk == NULL)  return E_PENDING;
    return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}

