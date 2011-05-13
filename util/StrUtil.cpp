/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#include "BaseUtil.h"
#include "StrUtil.h"

namespace str {

#define EntryCheck(arg1, arg2) \
    if (arg1 == arg2) \
        return true; \
    if (!arg1 || !arg2) \
        return false

bool Eq(const char *s1, const char *s2)
{
    EntryCheck(s1, s2);
    return 0 == strcmp(s1, s2);
}

bool EqI(const char *s1, const char *s2)
{
    EntryCheck(s1, s2);
    return 0 == _stricmp(s1, s2);
}

#undef EntryCheck

char *DupN(const char *s, size_t lenCch)
{
    if (!s)
        return NULL;
    char *res = (char *)memdup((void *)s, lenCch + 1);
    if (res)
        res[lenCch] = 0;
    return res;
}

const char *Find(const char *s, const char *subs)
{
    if (NULL == subs || NULL == s)
        return NULL;

    if (s == subs)
        return s;

    return strstr(s, subs);
}

char *FmtV(const char *fmt, va_list args)
{
    char    message[256];
    size_t  bufCchSize = dimof(message);
    char  * buf = message;
    for (;;)
    {
        int written = vsnprintf_s(buf, bufCchSize, bufCchSize-1, fmt, args);
        if (written > 0)
            break;
        /* we have to make the buffer bigger. The algorithm used to calculate
           the new size is arbitrary (aka. educated guess) */
        if (buf != message)
            free(buf);
        if (bufCchSize < 4*1024)
            bufCchSize += bufCchSize;
        else
            bufCchSize += 1024;
        buf = SAZA(char, bufCchSize);
        if (!buf)
            break;
    }

    if (buf == message)
        buf = str::Dup(message);

    return buf;
}

char *Format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char *res = FmtV(fmt, args);
    va_end(args);
    return res;
}

/* Concatenate 2 strings. Any string can be NULL.
   Caller needs to free() memory. */
char *Join(const char *s1, const char *s2, const char *s3)
{
    if (!s1) s1= "";
    if (!s2) s2 = "";
    if (!s3) s3 = "";

    return Format("%s%s%s", s1, s2, s3);
}

void d(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ScopedMem<char> buf(FmtV(format, args));
    OutputDebugStringA(buf);
    va_end(args);
}

/* Caller needs to free() the result */
char *ToMultiByte(const WCHAR *txt, UINT CodePage)
{
    assert(txt);
    if (!txt) return NULL;

    int requiredBufSize = WideCharToMultiByte(CodePage, 0, txt, -1, NULL, 0, NULL, NULL);
    char *res = SAZA(char, requiredBufSize);
    if (!res)
        return NULL;
    WideCharToMultiByte(CodePage, 0, txt, -1, res, requiredBufSize, NULL, NULL);
    return res;
}

/* Caller needs to free() the result */
char *ToMultiByte(const char *src, UINT CodePageSrc, UINT CodePageDest)
{
    assert(src);
    if (!src) return NULL;

    if (CodePageSrc == CodePageDest)
        return str::Dup(src);

    ScopedMem<WCHAR> tmp(ToWideChar(src, CodePageSrc));
    if (!tmp)
        return NULL;

    return ToMultiByte(tmp.Get(), CodePageDest);
}

/* Caller needs to free() the result */
WCHAR *ToWideChar(const char *src, UINT CodePage)
{
    if (!src) return NULL;

    int requiredBufSize = MultiByteToWideChar(CodePage, 0, src, -1, NULL, 0);
    WCHAR *res = SAZA(WCHAR, requiredBufSize);
    if (!res)
        return NULL;
    MultiByteToWideChar(CodePage, 0, src, -1, res, requiredBufSize);
    return res;
}

} // namespace str
