/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef BaseUtil_h
#define BaseUtil_h

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <gdiplus.h>

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#ifdef DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Ugly name, but the whole point is to make things shorter.
   SAZA = Struct Allocate and Zero memory for Array
   (note: use operator new for single structs/classes) */
#define SAZA(struct_name, n) (struct_name *)calloc((n), sizeof(struct_name))
#define dimof(X)    (sizeof(X)/sizeof((X)[0]))

/* compile-time assert */
#define CASSERT(exp, name) typedef int assert_##name [(exp) != FALSE]

inline void *memdup(void *data, size_t len)
{
    void *dup = malloc(len);
    if (dup)
        memcpy(dup, data, len);
    return dup;
}

// auto-free memory for arbitrary malloc()ed memory of type T*
template <typename T>
class ScopedMem
{
    T *obj;
public:
    ScopedMem() : obj(NULL) {}
    explicit ScopedMem(T* obj) : obj(obj) {}
    ~ScopedMem() { free((void*)obj); }
    void Set(T *o) {
        free((void*)obj);
        obj = o;
    }
    T *Get() const { return obj; }
    T *StealData() {
        T *tmp = obj;
        obj = NULL;
        return tmp;
    }
    operator T*() const { return obj; }
};

inline bool FlSet(UINT val, UINT flag) {
    return (val & flag) != 0;
}

#endif
