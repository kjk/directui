#ifndef WinUtf8_h
#define WinUtf8_h

/* Those are wrappers around Windows Unicode functions that convert between
   utf8 (used internally by appilctions) and utf16 unicode format used by
   *W version of win32 functions. */

#include "BaseUtil.h"

HANDLE CreateFileUtf8(const char *fileNameUtf8, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile = NULL);
bool   CreateDirectoryUtf8(const char* lpPathNameUtf8, LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
bool   DeleteFileUtf8(const char* path);
bool   GetFileAttributesExUtf8(const char* lpFileNameUtf8, GET_FILEEX_INFO_LEVELS fInfoLevelId, void* lpFileInformation);
char * GetFullPathNameUtf8(const char* lpFileNameUtf8);
char * GetLongPathNameUtf8(const char* lpszShortPathUtf8);
char * SHGetSpecialFolderPathUtf8(HWND hwndOwner, int csidl, BOOL fCreate);

HWND   CreateWindowExUtf8(DWORD dwExStyle, const char* lpClassName, const char *lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

char * GetWindowTextUtf8(HWND hwnd);
void   SetWindowTextUtf8(HWND hwnd, const char *s);
void   Edit_SetTextUtf8(HWND hwnd, const char *s);

int    DrawTextUtf8(HDC hdc, const char* lpchText, int cchText, LPRECT lprc, UINT format);

BOOL   GetTextExtentPoint32Utf8(HDC hdc, const char *lpString, int cch, LPSIZE lpSize);

#if !defined(DONT_HIDE_WIN32)

#undef CreateFile
#define CreateFile Use_CreateFileUtf8_Not_CreateFile

#undef CreateDirectory
#define CreateDirectory Use_CreateDirectoryUtf8_Not_CreateDirectory

#undef DeleteFile
#define DeleteFile Use_DeleteFileUtf8_Not_DeleteFile

#undef GetFileAttributesEx
#define GetFileAttributesEx Use_GetFileAttributesExUtf8_Not_GetFileAttributesEx

#undef GetFullPathName
#define GetFullPathName Use_GetFullPathNameUtf8_Not_GetFullPathName

#undef GetLongPathName
#define GetLongPathName Use_GetLongPathNameUtf8_Not_GetLongPathName

#undef SHGetSpecialFolderPath
#define SHGetSpecialFolderPath Use_SHGetSpecialFolderPathUtf8_Not_SHGetSpecialFolderPath

#undef CreateWindowEx
#define CreateWindowEx Use_CreateWindowExUtf8_Not_CreateWindowEx

#undef GetWindowText
#define Use_GetWindowTextUtf_Not_GetWindowText

#undef SetWindowText
#define SetWindowText Use_SetWindowTextUtf8_Not_SetWindowText

#undef Edit_SetText
#define Edit_SetText Use_Edit_SetTextUtf8_Not_Edit_SetText

#undef DrawText
#define DrawText Use_DrawTextUtf8_Not_DrawText

#undef GetTextExtentPoint32
#define GetTextExtentPoint32 Use_GetTextExtentPoint32Utf_Not_GetTextExtentPoint32

#endif // !defined(DONT_HIDE_WIN32)

#endif
