#if !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
#define AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_

#pragma once

class CPaintManagerUI;


#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


#define ASSERT(expr)  _ASSERTE(expr)

#ifdef _DEBUG
   #define TRACE __Trace
   #define TRACEMSG __TraceMsg
#else
   #define TRACE
   #define TRACEMSG _T("")
#endif

void UILIB_API __Trace(const TCHAR* pstrFormat, ...);
const TCHAR* __TraceMsg(UINT uMsg);


class UILIB_API CRect : public tagRECT
{
public:
   CRect();
   CRect(const RECT& src);
   CRect(int iLeft, int iTop, int iRight, int iBottom);

   int GetWidth() const;
   int GetHeight() const;
   void Empty();
   void Join(const RECT& rc);
   void ResetOffset();
   void Normalize();
   void Offset(int cx, int cy);
   void Inflate(int cx, int cy);
   void Deflate(int cx, int cy);
   void Union(CRect& rc);
};

class UILIB_API CSize : public tagSIZE
{
public:
   CSize();
   CSize(const SIZE& src);
   CSize(const RECT rc);
   CSize(int cx, int cy);
};


class UILIB_API CPoint : public tagPOINT
{
public:
   CPoint();
   CPoint(const POINT& src);
   CPoint(int x, int y);
   CPoint(LPARAM lParam);
};



class UILIB_API CStdPtrArray
{
public:
   CStdPtrArray(int iPreallocSize = 0);
   virtual ~CStdPtrArray();

   void Empty();
   void Resize(int iSize);
   bool IsEmpty() const;
   int Find(void* idx) const;
   bool Add(void* data);
   bool SetAt(int idx, void* data);
   bool InsertAt(int idx, void* data);
   bool Remove(int idx);
   int GetSize() const;
   void** GetData();

   void* GetAt(int idx) const;
   void* operator[] (int nIndex) const;

protected:
   void** m_ppVoid;
   int m_nCount;
   int m_nAllocated;
};


class UILIB_API CStdValArray
{
public:
   CStdValArray(int iElementSize, int iPreallocSize = 0);
   virtual ~CStdValArray();

   void Empty();
   bool IsEmpty() const;
   bool Add(LPCVOID data);
   bool Remove(int idx);
   int GetSize() const;
   void* GetData();

   void* GetAt(int idx) const;
   void* operator[] (int nIndex) const;

protected:
   LPBYTE m_pVoid;
   int m_iElementSize;
   int m_nCount;
   int m_nAllocated;
};

class UILIB_API CWindowWnd
{
public:
   CWindowWnd();

   HWND GetHWND() const;
   operator HWND() const;

   bool RegisterWindowClass();
   bool RegisterSuperclass();

   HWND Create(HWND hwndParent, const TCHAR* name, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
   HWND Create(HWND hwndParent, const TCHAR* name, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
   HWND Subclass(HWND hWnd);
   void Unsubclass();
   void ShowWindow(bool bShow = true, bool bTakeFocus = true);
   bool ShowModal();
   void Close();
   void CenterWindow();
   void SetIcon(UINT nRes);

protected:
   virtual const TCHAR* GetWindowClassName() const = 0;
   virtual const TCHAR* GetSuperClassName() const;
   virtual UINT GetClassStyle() const;

   LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
   LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
   void ResizeClient(int cx = -1, int cy = -1);

   virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   virtual void OnFinalMessage(HWND hWnd);

   static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
   HWND m_hWnd;
   WNDPROC m_OldWndProc;
   bool m_bSubclassed;
};

class UILIB_API CWaitCursor
{
public:
   CWaitCursor();
   ~CWaitCursor();

protected:
   HCURSOR m_hOrigCursor;
};

class UILIB_API CStdString
{
public:
   enum { MAX_LOCAL_STRING_LEN = 63 };

   CStdString();
   CStdString(const TCHAR ch);
   CStdString(const CStdString& src);
   CStdString(const TCHAR* lpsz, int nLen = -1);
   virtual ~CStdString();

   static CStdString RES(UINT nRes);

   void Empty();
   int GetLength() const;
   bool IsEmpty() const;
   TCHAR GetAt(int nIndex) const;
   void Append(const TCHAR* pstr);
   void Assign(const TCHAR* pstr, int nLength = -1);
   const TCHAR* GetData();
   
   void SetAt(int nIndex, TCHAR ch);
   operator LPCTSTR() const;

   TCHAR operator[] (int nIndex) const;
   const CStdString& operator=(const CStdString& src);
   const CStdString& operator=(const TCHAR ch);
   const CStdString& operator=(const TCHAR* pstr);
#ifndef _UNICODE
   const CStdString& CStdString::operator=(LPCWSTR lpwStr);
#endif
   CStdString operator+(const CStdString& src);
   CStdString operator+(const TCHAR* pstr);
   const CStdString& operator+=(const CStdString& src);
   const CStdString& operator+=(const TCHAR* pstr);
   const CStdString& operator+=(const TCHAR ch);

   bool operator == (const TCHAR* str) const;
   bool operator != (const TCHAR* str) const;
   bool operator <= (const TCHAR* str) const;
   bool operator <  (const TCHAR* str) const;
   bool operator >= (const TCHAR* str) const;
   bool operator >  (const TCHAR* str) const;

   int Compare(const TCHAR* pstr) const;
   int CompareNoCase(const TCHAR* pstr) const;
   
   void MakeUpper();
   void MakeLower();

   CStdString Left(int nLength) const;
   CStdString Mid(int pos, int nLength = -1) const;
   CStdString Right(int nLength) const;

   int Find(TCHAR ch, int pos = 0) const;
   int Find(const TCHAR* pstr, int pos = 0) const;
   int ReverseFind(TCHAR ch) const;
   int Replace(const TCHAR* pstrFrom, const TCHAR* pstrTo);
   
   void ProcessResourceTokens();
   int __cdecl Format(const TCHAR* pstrFormat, ...);

protected:
   TCHAR* m_pstr;
   TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};


#endif // !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
