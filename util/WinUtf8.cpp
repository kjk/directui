
#define DONT_HIDE_WIN32
#include "WinUtf8.h"

#include "StrUtil.h"

bool DeleteFileUtf8(const char* path)
{
    ScopedMem<WCHAR> pathW(str::conv::Utf8ToUni(path));
    return DeleteFileW(pathW) != 0;
}

char *GetFullPathNameUtf8(const char* lpFileNameUtf8)
{
    WCHAR tmp[1024];
    WCHAR *buf = tmp;
    DWORD cchBuf = dimof(tmp);
    ScopedMem<WCHAR> fileName(str::conv::Utf8ToUni(lpFileNameUtf8));
OnceMore:
    DWORD ret = GetFullPathNameW(fileName, cchBuf, buf, NULL);
    if (0 == ret)
        goto Error;
    if (ret >= cchBuf) {
        if (buf != tmp)
            goto Error;
        cchBuf = ret + 8;
        buf = SAZA(WCHAR, cchBuf);
        goto OnceMore;
    }
    char *p = str::conv::UniToUtf8(tmp);
    if (buf != tmp)
        free(buf);
    return p;

Error:
    if (buf != tmp)
        free(buf);
    return NULL;
}

char *GetLongPathNameUtf8(const char* lpszShortPathUtf8)
{
    WCHAR tmp[1024];
    WCHAR *buf = tmp;
    DWORD cchBuf = dimof(tmp);
    ScopedMem<WCHAR> shortPath(str::conv::Utf8ToUni(lpszShortPathUtf8));

OnceMore:
    DWORD ret = GetLongPathNameW(shortPath, buf, cchBuf);
    if (0 == ret)
        goto Error;
    if (ret >= cchBuf) {
        if (buf != tmp)
            goto Error;
        cchBuf = ret + 8;
        buf = SAZA(WCHAR, cchBuf);
        goto OnceMore;
    }

    char *p = str::conv::UniToUtf8(buf);
    if (buf != tmp)
        free(buf);
    return p;
Error:
    if (buf != tmp)
        free(buf);
    return NULL;
}

char *SHGetSpecialFolderPathUtf8(HWND hwndOwner, int csidl, BOOL fCreate)
{
    WCHAR dir[MAX_PATH] = {0};
    BOOL ok = SHGetSpecialFolderPathW(hwndOwner, dir, csidl, fCreate);
    if (!ok)
        return NULL;
    return str::conv::UniToUtf8(dir);
}

HANDLE CreateFileUtf8(const char *fileNameUtf8, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    ScopedMem<WCHAR> fileName(str::conv::Utf8ToUni(fileNameUtf8));
    HANDLE h = CreateFileW(fileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);
    return h;
}

bool CreateDirectoryUtf8(const char* lpPathNameUtf8, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    ScopedMem<WCHAR> dir(str::conv::Utf8ToUni(lpPathNameUtf8));
    return !!CreateDirectoryW(dir, lpSecurityAttributes);
}

bool GetFileAttributesExUtf8(const char* lpFileNameUtf8, GET_FILEEX_INFO_LEVELS fInfoLevelId, void* lpFileInformation)
{
    ScopedMem<WCHAR> fileName(str::conv::Utf8ToUni(lpFileNameUtf8));
    return !!GetFileAttributesExW(fileName, fInfoLevelId, lpFileInformation);
}

HWND CreateWindowExUtf8(DWORD dwExStyle, const char * lpClassName, const char *lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    ScopedMem<WCHAR> className(str::conv::Utf8ToUni(lpClassName));
    ScopedMem<WCHAR> windowName(str::conv::Utf8ToUni(lpWindowName));
    return CreateWindowExW(dwExStyle, className, windowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
}

void SetWindowTextUtf8(HWND hwnd, const char *s)
{
    ScopedMem<WCHAR> tmp(str::conv::Utf8ToUni(s));
    SetWindowTextW(hwnd, tmp);
}

void Edit_SetTextUtf8(HWND hwnd, const char *s)
{
    SetWindowTextUtf8(hwnd, s);
}

char *GetWindowTextUtf8(HWND hwnd)
{
    WCHAR buf[1024];
    WCHAR *tmp = buf;
    int cch = GetWindowTextLength(hwnd);
    if (cch + 1 > dimof(buf)) {
        cch += 1;
        tmp = (WCHAR*)malloc(cch * sizeof(WCHAR));
    } else {
        cch = dimof(buf);
    }
    BOOL ok = ::GetWindowTextW(hwnd, tmp, cch);
    if (ok == 0)
        return NULL;
    return str::conv::UniToUtf8(buf);
}

int DrawTextUtf8(HDC hdc, const char* lpchText, int cchText, LPRECT lprc, UINT format)
{
    // TODO: not sure how cchText translates from utf8 to WCHAR
    ScopedMem<WCHAR> s(str::conv::Utf8ToUni(lpchText));
    return DrawTextW(hdc, s, cchText, lprc, format);
}

BOOL GetTextExtentPoint32Utf8(HDC hdc, const char *lpString, int cch, LPSIZE lpSize)
{
    // TODO: not sure how cchText translates from utf8 to WCHAR
    ScopedMem<WCHAR> s(str::conv::Utf8ToUni(lpString));
    return GetTextExtentPoint32(hdc, s, cch, lpSize);
}

