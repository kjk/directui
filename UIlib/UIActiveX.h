#if !defined(AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_)
#define AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_

#pragma once

class CActiveXCtrl;
struct IOleObject;

class UILIB_API CActiveXUI : public ControlUI, public IMessageFilterUI
{
friend CActiveXCtrl;
public:
   CActiveXUI();
   virtual ~CActiveXUI();

   const TCHAR* GetClass() const;

   bool CreateControl(const CLSID clsid);
   bool CreateControl(const TCHAR* pstrCLSID);
   HRESULT GetControl(const IID iid, LPVOID* ppRet);

   void SetWidth(int cx);
   void SetHeight(int cy);

   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(const TCHAR* name, const TCHAR* value);

   LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
   void ReleaseControl();
   bool DelayedControlCreation();

protected:
   CLSID m_clsid;
   bool m_bCreated;
   IOleObject* m_pUnk;
   CActiveXCtrl* m_ctrl;
   HWND m_hwndHost;
   SIZE m_szFixed;
};


#endif // !defined(AFX_UIACTIVEX_H__20060223_0330_20DB_3F74_0080AD509054__INCLUDED_)
