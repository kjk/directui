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

#endif
