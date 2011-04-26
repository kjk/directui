/* Copyright 2011 the  Wdl Extra project authors (see ../AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef FileUtil_h
#define FileUtil_h

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
