/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef StrUtil_h
#define StrUtil_h

namespace str {

inline size_t Len(const char *s) { return strlen(s); }
inline size_t Len(const WCHAR *s) { return wcslen(s); }

inline char * Dup(const char *s) { return _strdup(s); }
char *        DupN(const char *s, size_t lenCch);

bool Eq(const char *s1, const char *s2);
bool EqI(const char *s1, const char *s2);

inline bool EqN(const char *s1, const char *s2, size_t len) {
    return 0 == strncmp(s1, s2, len);
}

const char *Find(const char *s, const char *subs);

char *  FmtV(const char *fmt, va_list args);
char *  Format(const char *fmt, ...);

char *  Join(const char *s1, const char *s2, const char *s3=NULL);

char *  ToMultiByte(const WCHAR *txt, UINT CodePage);
char *  ToMultiByte(const char *src, UINT CodePageSrc, UINT CodePageDest);
WCHAR * ToWideChar(const char *src, UINT CodePage);


inline void Replace(const char*& s, const char *replacement) {
    free((void*)s);
    if (NULL != replacement)
        s = str::Dup(replacement);
    else
        s = NULL;
}

inline char *UniToUtf8(const WCHAR *src) { return ToMultiByte(src, CP_UTF8); }
inline WCHAR *Utf8ToUni(const char *src) { return ToWideChar(src, CP_UTF8); }

} // namespace str

#endif
