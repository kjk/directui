/* Copyright 2011 the  Wdl Extra project authors (see ../AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef FileUtil_h
#define FileUtil_h

// more convenient wrappers around win32 functions that use utf8 as paths
bool DeleteFileUtf8(const char* path);
char *GetFullPathNameUtf8(const char* lpFileNameUtf8);
char *GetLongPathNameUtf8(const char* lpszShortPathUtf8);
char *SHGetSpecialFolderPathUtf8(HWND hwndOwner, int csidl, BOOL fCreate);
HANDLE CreateFileUtf8(const char *fileNameUtf8, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile = NULL);
bool CreateDirectoryUtf8(const char* lpPathNameUtf8, LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
bool GetFileAttributesExUtf8(const char* lpFileNameUtf8, GET_FILEEX_INFO_LEVELS fInfoLevelId, void* lpFileInformation);

namespace path {

const char * GetBaseName(const char *path);

char *       GetDir(const char *path);
char *       Join(const char *path, const char *filename);
char *       Normalize(const char *path);
bool         IsSame(const char *path1, const char *path2);

}

namespace file {

bool         Exists(const char *filePath);
char *       ReadAll(const char *filePath, size_t *fileSizeOut);
bool         ReadAll(const char *filePath, char *buffer, size_t bufferLen);
bool         WriteAll(const char *filePath, void *data, size_t dataLen);
size_t       GetSize(const char *filePath);
bool         Delete(const char *filePath);
FILETIME     GetModificationTime(const char *filePath);

}

namespace dir {

bool         Exists(const char *dir);
bool         Create(const char *dir);

}

#endif
