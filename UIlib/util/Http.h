/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#ifndef Http_h
#define Http_h

#include "StrUtil.h"
#include "Vec.h"

bool HttpPost(const char *url, str::Str<char> *headers, str::Str<char> *data, str::Str<char>& respOut);
bool HttpGet(const char *url, str::Str<char> *dataOut);
//bool  HttpGetToFile(const char *url, const char *destFilePath);

class HttpRsp;

class HttpRspDoneCallback {
    virtual void OnDone(HttpRsp *rsp) = 0;
};

class HttpRsp {
    HttpRspDoneCallback *cb;
    const char *url;
public:
    static HttpRsp *StartHttpGet(const char *url, HttpRspDoneCallback *cb);
};

#if 0
class HttpReqCtx {
    HANDLE          hThread;

public:
    // the callback to execute when the download is complete
    CallbackFunc *  callback;

    TCHAR *         url;
    Str::Str<char> *data;
    DWORD           error;

    HttpReqCtx(const TCHAR *url, CallbackFunc *callback=NULL);
    ~HttpReqCtx() {
        free(url);
        delete data;
        CloseHandle(hThread);
    }
};

#endif

#endif
