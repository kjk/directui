#if !defined(AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_)
#define AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_

class ActiveXCtrl;
struct IOleObject;

class UILIB_API ActiveXUI : public ControlUI, public IMessageFilterUI
{
    friend ActiveXCtrl;
public:
    ActiveXUI();
    virtual ~ActiveXUI();

    virtual const char* GetClass() const;

    bool CreateControl(const CLSID clsid);
    bool CreateControl(const char* pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID* ppRet);

    void SetWidth(int cx);
    void SetHeight(int cy);

    virtual void SetPos(RECT rc);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);

    virtual void SetAttribute(const char* name, const char* value);

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    void ReleaseControl();
    bool DelayedControlCreation();

protected:
    CLSID        m_clsid;
    bool         m_bCreated;
    IOleObject*  m_pUnk;
    ActiveXCtrl* m_ctrl;
    HWND         m_hwndHost;
    SIZE         m_szFixed;
};

#endif // !defined(AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_)
