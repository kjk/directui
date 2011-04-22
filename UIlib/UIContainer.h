#if !defined(AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_)
#define AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class IContainerUI
{
public:
   virtual ControlUI* GetItem(int idx) const = 0;
   virtual int GetCount() const = 0;
   virtual bool Add(ControlUI* ctrl) = 0;
   virtual bool Remove(ControlUI* ctrl) = 0;
   virtual void RemoveAll() = 0;
};

class UILIB_API ContainerUI : public ControlUI, public IContainerUI
{
public:
   ContainerUI();
   virtual ~ContainerUI();

public:
   const TCHAR* GetClass() const;
   LPVOID GetInterface(const TCHAR* name);

   ControlUI* GetItem(int idx) const;
   int GetCount() const;
   bool Add(ControlUI* ctrl);
   bool Remove(ControlUI* ctrl);
   void RemoveAll();

   void Event(TEventUI& event);
   void SetVisible(bool bVisible);

   virtual void SetInset(SIZE szInset);
   virtual void SetInset(RECT rcInset);
   virtual void SetPadding(int iPadding);
   virtual void SetWidth(int cx);
   virtual void SetHeight(int cy);
   virtual void SetAutoDestroy(bool bAuto);

   virtual int FindSelectable(int idx, bool bForward = true) const;

   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(const TCHAR* name, const TCHAR* value);

   void SetManager(PaintManagerUI* manager, ControlUI* parent);
   ControlUI* FindControl(FINDCONTROLPROC Proc, void* data, UINT uFlags);

   virtual int GetScrollPos() const;
   virtual int GetScrollPage() const;
   virtual SIZE GetScrollRange() const;
   virtual void SetScrollPos(int pos);
   virtual void EnableScrollBar(bool bEnable = true);

protected:
   virtual void ProcessScrollbar(RECT rc, int cyRequired);

protected:
   CStdPtrArray m_items;
   RECT m_rcInset;
   int m_iPadding;
   SIZE m_cxyFixed;
   bool m_bAutoDestroy;
   bool m_bAllowScrollbars;
   HWND m_hwndScroll;
   int m_iScrollPos;
};

class UILIB_API CanvasUI : public ContainerUI
{
public:
   CanvasUI();
   virtual ~CanvasUI();

   const TCHAR* GetClass() const;

   bool SetWatermark(UINT iBitmapRes, int iOrientation = HTBOTTOMRIGHT);
   bool SetWatermark(const TCHAR* pstrBitmap, int iOrientation = HTBOTTOMRIGHT);

   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   COLORREF m_clrBack;
   HBITMAP m_hBitmap;
   BITMAP m_BitmapInfo;
   int m_iOrientation;
};


class UILIB_API WindowCanvasUI : public CanvasUI
{
public:
   WindowCanvasUI();
   const TCHAR* GetClass() const;
};


class UILIB_API ControlCanvasUI : public CanvasUI
{
public:
   ControlCanvasUI();
   const TCHAR* GetClass() const;
};


class UILIB_API WhiteCanvasUI : public CanvasUI
{
public:
   WhiteCanvasUI();
   const TCHAR* GetClass() const;
};


class UILIB_API DialogCanvasUI : public CanvasUI
{
public:
   DialogCanvasUI();
   const TCHAR* GetClass() const;
};


class UILIB_API TabFolderCanvasUI : public CanvasUI
{
public:
   TabFolderCanvasUI();
   const TCHAR* GetClass() const;
};


class UILIB_API VerticalLayoutUI : public ContainerUI
{
public:
   VerticalLayoutUI();

   const TCHAR* GetClass() const;
   void SetPos(RECT rc);

protected:
   int m_cyNeeded;
   int m_nPrevItems;
};


class UILIB_API HorizontalLayoutUI : public ContainerUI
{
public:
   HorizontalLayoutUI();

   const TCHAR* GetClass() const;
   void SetPos(RECT rc);
};


class UILIB_API TileLayoutUI : public ContainerUI
{
public:
   TileLayoutUI();

   const TCHAR* GetClass() const;

   void SetPos(RECT rc);
   void SetColumns(int nCols);

protected:
   int m_nColumns;
   int m_cyNeeded;
};


class UILIB_API DialogLayoutUI : public ContainerUI
{
public:
   DialogLayoutUI();

   const TCHAR* GetClass() const;
   void* GetInterface(const TCHAR* name);

   void SetStretchMode(ControlUI* ctrl, UINT uMode);

   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);

protected:
   void RecalcArea();

protected:  
   typedef struct 
   {
      ControlUI* ctrl;
      UINT uMode;
      RECT rcItem;
   } STRETCHMODE;

   RECT m_rcDialog;
   RECT m_rcOriginal;
   bool m_bFirstResize;
   CStdValArray m_aModes;
};


#endif // !defined(AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_)

