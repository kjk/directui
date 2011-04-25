/* Copyright 2011 the  Wdl Extra project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#include "BaseUtil.h"
#include "StrUtil.h"
#include "FileUtil.h"

bool DeleteFileUtf8(const char* path)
{
    ScopedMem<WCHAR> pathW(str::conv::Utf8ToUni(path));
    return !!DeleteFileW(pathW);
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

namespace path {

static inline bool IsSep(char c)
{
    return '\\' == c || '/' == c;
}

// Note: returns pointer inside <path>, do not free
const char *GetBaseName(const char *path)
{
    const char *fileBaseName = path + str::Len(path);
    for (; fileBaseName > path; fileBaseName--)
        if (IsSep(fileBaseName[-1]))
            break;
    return fileBaseName;
}

char *GetDir(const char *path)
{
    const char *baseName = GetBaseName(path);
    int dirLen;
    if (baseName <= path + 1)
        dirLen = str::Len(path);
    else if (baseName[-2] == ':')
        dirLen = baseName - path;
    else
        dirLen = baseName - path - 1;
    return str::DupN(path, dirLen);
}

char *Join(const char *path, const char *filename)
{
    if (IsSep(*filename))
        filename++;
    char *sep = NULL;
    if (!IsSep(path[str::Len(path) - 1]))
        sep = "\\";
    return str::Join(path, sep, filename);
}

// Normalize a file path.
//  remove relative path component (..\ and .\),
//  replace slashes by backslashes,
//  convert to long form.
//
// Returns a pointer to a memory allocated block containing the normalized string.
//   The caller is responsible for freeing the block.
//   Returns NULL if the file does not exist or if a memory allocation fails.
//
// Precondition: the file must exist on the file system.
//
// Note:
//   - the case of the root component is preserved
//   - the case of rest is set to the way it is stored on the file system
//
// e.g. suppose the a file "C:\foo\Bar.Pdf" exists on the file system then
//    "c:\foo\bar.pdf" becomes "c:\foo\Bar.Pdf"
//    "C:\foo\BAR.PDF" becomes "C:\foo\Bar.Pdf"
char *Normalize(const char *path)
{
    // convert to absolute path, change slashes into backslashes
    char *normPath = GetFullPathNameUtf8(path);
    if (!normPath)
        return NULL;

    char *longPath = GetLongPathNameUtf8(normPath);
    free(normPath);
    return longPath;
}

// Code adapted from http://stackoverflow.com/questions/562701/best-way-to-determine-if-two-path-reference-to-same-file-in-c-c/562830#562830
// Determine if 2 paths point ot the same file...
bool IsSame(const char *path1, const char *path2)
{
    bool isSame = false, needFallback = true;
    HANDLE handle1 = CreateFileUtf8(path1, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    HANDLE handle2 = CreateFileUtf8(path2, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (handle1 != INVALID_HANDLE_VALUE && handle2 != INVALID_HANDLE_VALUE) {
        BY_HANDLE_FILE_INFORMATION fi1, fi2;
        if (GetFileInformationByHandle(handle1, &fi1) && GetFileInformationByHandle(handle2, &fi2)) {
            isSame = fi1.dwVolumeSerialNumber == fi2.dwVolumeSerialNumber &&
                     fi1.nFileIndexHigh == fi2.nFileIndexHigh &&
                     fi1.nFileIndexLow == fi2.nFileIndexLow;
            needFallback = false;
        }
    }

    CloseHandle(handle1);
    CloseHandle(handle2);

    if (!needFallback)
        return isSame;

    ScopedMem<char> npath1(Normalize(path1));
    ScopedMem<char> npath2(Normalize(path2));
    // consider the files different, if their paths can't be normalized
    if (!npath1 || !npath2)
        return false;
    return str::EqI(npath1, npath2);
}

}

namespace file {

bool Exists(const char *filePathUtf8)
{
    if (NULL == filePathUtf8)
        return false;

    ScopedMem<WCHAR> filePath(str::conv::Utf8ToUni(filePathUtf8));
    if (!filePath)
        return false;
    WIN32_FILE_ATTRIBUTE_DATA   fileInfo;
    BOOL res = GetFileAttributesExW(filePath, GetFileExInfoStandard, &fileInfo);
    if (0 == res)
        return false;

    if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return false;
    return true;
}

size_t GetSize(const char *filePathUtf8)
{
    WIN32_FILE_ATTRIBUTE_DATA   fileInfo;

    if (NULL == filePathUtf8)
        return INVALID_FILE_SIZE;

    ScopedMem<WCHAR> filePath(str::conv::Utf8ToUni(filePathUtf8));
    if (!filePath)
        return false;
    BOOL res = GetFileAttributesExW(filePath, GetFileExInfoStandard, &fileInfo);
    if (0 == res)
        return INVALID_FILE_SIZE;

    size_t size = fileInfo.nFileSizeLow;
#ifdef _WIN64
    size += fileInfo.nFileSizeHigh << 32;
#else
    if (fileInfo.nFileSizeHigh > 0)
        return INVALID_FILE_SIZE;
#endif

    return size;
}

char *ReadAll(const char *filePathUtf8, size_t *fileSizeOut)
{
    size_t size = GetSize(filePathUtf8);
    if (INVALID_FILE_SIZE == size)
        return NULL;

    /* allocate one byte more and 0-terminate just in case it's a text
       file we'll want to treat as C string. Doesn't hurt for binary
       files */
    char *data = SAZA(char, size + 1);
    if (!data)
        return NULL;

    if (!ReadAll(filePathUtf8, data, size)) {
        free(data);
        return NULL;
    }

    if (fileSizeOut)
        *fileSizeOut = size;
    return data;
}

bool ReadAll(const char *filePathUtf8, char *buffer, size_t bufferLen)
{
    HANDLE h = CreateFileUtf8(filePathUtf8, GENERIC_READ, FILE_SHARE_READ, NULL,  
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  NULL); 
    if (h == INVALID_HANDLE_VALUE)
        return false;

    DWORD sizeRead;
    BOOL ok = ReadFile(h, buffer, bufferLen, &sizeRead, NULL);
    CloseHandle(h);

    return ok && sizeRead == bufferLen;
}

bool WriteAll(const char *filePathUtf8, void *data, size_t dataLen)
{
    HANDLE h = CreateFileUtf8(filePathUtf8, GENERIC_WRITE, FILE_SHARE_READ, NULL,  
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,  NULL); 
    if (h == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD size;
    BOOL ok = WriteFile(h, data, (DWORD)dataLen, &size, NULL);
    assert(!ok || (dataLen == size));
    CloseHandle(h);

    return ok && dataLen == size;
}

// Return true if the file wasn't there or was successfully deleted
bool Delete(const char *filePathUtf8)
{
    bool ok = DeleteFileUtf8(filePathUtf8);
    if (ok)
        return true;
    DWORD err = GetLastError();
    return ((ERROR_PATH_NOT_FOUND == err) || (ERROR_FILE_NOT_FOUND == err));
}

FILETIME GetModificationTime(const char *filePathUtf8)
{
    FILETIME lastMod = { 0 };
    HANDLE h = CreateFileUtf8(filePathUtf8, GENERIC_READ, FILE_SHARE_READ, NULL,  
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  NULL); 
    if (h != INVALID_HANDLE_VALUE)
        GetFileTime(h, NULL, NULL, &lastMod);
    CloseHandle(h);
    return lastMod;
}

}

namespace dir {

bool Exists(const char *dirUtf8)
{
    if (!dirUtf8)
        return false;

    WIN32_FILE_ATTRIBUTE_DATA   fileInfo;
    BOOL res = GetFileAttributesExUtf8(dirUtf8, GetFileExInfoStandard, &fileInfo);
    if (0 == res)
        return false;

    if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

// Return true if a directory already exists or has been successfully created
bool Create(const char *dirUtf8)
{
    if (!dirUtf8)
        return false;
    BOOL ok = CreateDirectoryUtf8(dirUtf8, NULL);
    if (ok)
        return true;
    return ERROR_ALREADY_EXISTS == GetLastError();
}

}
