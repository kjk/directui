#ifndef WinUtf8_h
#define WinUtf8_h

/* Those are wrappers around Windows Unicode functions that convert between
   utf8 (used internally by appilctions) and utf16 unicode format used by
   *W version of win32 functions. */

#include "BaseUtil.h"

bool DeleteFileUtf8(const char* path);
char *GetFullPathNameUtf8(const char* lpFileNameUtf8);
char *GetLongPathNameUtf8(const char* lpszShortPathUtf8);
char *SHGetSpecialFolderPathUtf8(HWND hwndOwner, int csidl, BOOL fCreate);
HANDLE CreateFileUtf8(const char *fileNameUtf8, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile = NULL);
bool CreateDirectoryUtf8(const char* lpPathNameUtf8, LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
bool GetFileAttributesExUtf8(const char* lpFileNameUtf8, GET_FILEEX_INFO_LEVELS fInfoLevelId, void* lpFileInformation);

HWND CreateWindowExUtf8(DWORD dwExStyle, const char* lpClassName, const char *lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

void Edit_SetTextUtf8(HWND hwnd, const char *s);
void SetWindowTextUtf8(HWND hwnd, const char *s);
char *GetWindowTextUtf8(HWND hwnd);

int DrawTextUtf8(HDC hdc, const char* lpchText, int cchText, LPRECT lprc, UINT format);

#if !defined(DONT_HIDE_WIN32)

#ifdef DrawText
#undef DrawText
#define DrawText Use_DrawTextUtf8_Not_DrawText
#endif

#ifdef CreateWindowEx
#undef CreateWindowEx
#define CreateWindowEx Use_CreateWindowExUtf8_Not_CreateWindowEx
#endif

#ifdef Edit_SetText
#undef Edit_SetText
#define Edit_SetText Use_Edit_SetTextUtf8_Not_Edit_SetText
#endif

#ifdef GetWindowText
#undef GetWindowText
#define Use_GetWindowTextUtf_Not_GetWindowText
#endif

#ifdef SetWindowText
#undef SetWindowText
#define SetWindowText Use_SetWindowTextUtf8_Not_SetWindowText
#endif

#endif // !defined(DONT_HIDE_WIN32)

#endif
