#if !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
#define AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_

#include "BaseUtil.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "Vec.h"

class PaintManagerUI;

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
   #define TRACEMSG ""
#endif

void UILIB_API __Trace(const char* fmt, ...);
const char* __TraceMsg(UINT uMsg);

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

class UILIB_API StdPtrArray
{
public:
   StdPtrArray(int iPreallocSize = 0);
   virtual ~StdPtrArray();

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
   void* operator[] (int idx) const;

protected:
   void** m_ppVoid;
   int m_nCount;
   int m_nAllocated;
};

class UILIB_API StdValArray
{
public:
   StdValArray(int iElementSize, int iPreallocSize = 0);
   virtual ~StdValArray();

   void Empty();
   bool IsEmpty() const;
   bool Add(LPCVOID data);
   bool Remove(int idx);
   int GetSize() const;
   void* GetData();

   void* GetAt(int idx) const;
   void* operator[] (int idx) const;

protected:
   LPBYTE m_pVoid;
   int m_iElementSize;
   int m_nCount;
   int m_nAllocated;
};

class UILIB_API WindowWnd
{
public:
   WindowWnd();

   HWND GetHWND() const;
   operator HWND() const;

   bool RegisterWindowClass();
   bool RegisterSuperclass();

   HWND Create(HWND hwndParent, const char* name, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
   HWND Create(HWND hwndParent, const char* name, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
   HWND Subclass(HWND hWnd);
   void Unsubclass();
   void ShowWindow(bool bShow = true, bool takeFocus = true);
   bool ShowModal();
   void Close();
   void CenterWindow();
   void SetIcon(UINT nRes);

protected:
   virtual const char* GetWindowClassName() const = 0;
   virtual const char* GetSuperClassName() const;
   virtual UINT GetClassStyle() const;

   LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
   LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
   void ResizeClient(int cx = -1, int cy = -1);

   virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   virtual void OnFinalMessage(HWND hWnd);

   static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
   HWND     m_hWnd;
   WNDPROC  m_OldWndProc;
   bool     m_subclassed;
};

class UILIB_API WaitCursor
{
public:
   WaitCursor();
   ~WaitCursor();

protected:
   HCURSOR m_hOrigCursor;
};

class UILIB_API StdString
{
public:
   enum { MAX_LOCAL_STRING_LEN = 63 };

   StdString();
   StdString(const char ch);
   StdString(const StdString& src);
   StdString(const char* lpsz, int nLen = -1);
   virtual ~StdString();

   static StdString RES(UINT nRes);

   void Empty();
   int GetLength() const;
   bool IsEmpty() const;
   char GetAt(int idx) const;
   void Append(const char* pstr);
   void Assign(const char* pstr, int len = -1);
   const char* GetData();
   
   void SetAt(int idx, char ch);
   operator LPCSTR() const;

   char operator[] (int idx) const;
   const StdString& operator=(const StdString& src);
   const StdString& operator=(const char ch);
   const StdString& operator=(const char* pstr);
#ifndef _UNICODE
   const StdString& StdString::operator=(LPCWSTR lpwStr);
#endif
   StdString operator+(const StdString& src);
   StdString operator+(const char* pstr);
   const StdString& operator+=(const StdString& src);
   const StdString& operator+=(const char* pstr);
   const StdString& operator+=(const char ch);

   bool operator == (const char* str) const;
   bool operator != (const char* str) const;
   bool operator <= (const char* str) const;
   bool operator <  (const char* str) const;
   bool operator >= (const char* str) const;
   bool operator >  (const char* str) const;

   int Compare(const char* pstr) const;
   int CompareNoCase(const char* pstr) const;
   
   void MakeUpper();
   void MakeLower();

   StdString Left(int len) const;
   StdString Mid(int pos, int len = -1) const;
   StdString Right(int len) const;

   int Find(char ch, int pos = 0) const;
   int Find(const char* pstr, int pos = 0) const;
   int ReverseFind(char ch) const;
   int Replace(const char* pstrFrom, const char* pstrTo);
   
   void ProcessResourceTokens();
   int __cdecl Format(const char* pstrFormat, ...);

protected:
   char* m_pstr;
   char  m_buf[MAX_LOCAL_STRING_LEN + 1];
};

bool ParseInt(const char *name, const char *val, const char *expectedName, int& ret);
bool ParseWidth(const char *name, const char *val, int& dx);
bool ParseHeight(const char *name, const char *val, int& dy);

#endif // !defined(AFX_UIBASE_H__20050509_3DFB_5C7A_C897_0080AD509054__INCLUDED_)
