/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#include "BaseUtil.h"
#include <Wininet.h>

#include "Http.h"
//#include "FileUtil.h"
#include "WinUtil.h"

// per RFC 1945 10.15 and 3.7, a user agent product token shouldn't contain whitespace
#define USER_AGENT "BaseHTTP"

// returns ERROR_SUCCESS or an error code
bool HttpGet(const char *url, str::Str<char> *dataOut)
{
    bool ok = false;

    HINTERNET hFile = NULL;
    HINTERNET hInet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInet)
        goto Exit;

    hFile = InternetOpenUrlA(hInet, url, NULL, 0, 0, 0);
    if (!hFile)
        goto Exit;

    static const DWORD readAtATime = 1024;
    DWORD dwRead;
    do {
        char *buf = dataOut->MakeSpaceAtNoLenIncrease(dataOut->Count(), readAtATime);
        if (!InternetReadFile(hFile, buf, readAtATime, &dwRead))
            goto Exit;
        dataOut->LenIncrease(dwRead);
    } while (dwRead > 0);

    ok = true;

Exit:
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInet);
    return ok;
}

#if 0
// Download content of a url to a file
bool HttpGetToFile(const char *url, const char *destFilePath)
{
    bool ok = false;
    char buf[1024];
    HINTERNET hFile = NULL, hInet = NULL;

    HANDLE hf = CreateFile(destFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,  
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,  NULL); 
    if (INVALID_HANDLE_VALUE == hf) {
        SeeLastError();
        goto Exit;
    }

    hInet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInet)
        goto Exit;

    hFile = InternetOpenUrlA(hInet, url, NULL, 0, 0, 0);
    if (!hFile)
        goto Exit;

    DWORD dwRead;
    for (;;) {
        if (!InternetReadFile(hFile, buf, sizeof(buf), &dwRead))
            goto Exit;
        if (dwRead == 0)
            break;

        DWORD size;
        BOOL ok = WriteFile(hf, buf, (DWORD)dwRead, &size, NULL);
        if (!ok) {
            SeeLastError();
            goto Exit;
        }
        if (size != dwRead)
            goto Exit;
    }

    ok = true;
Exit:
    CloseHandle(hf);
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInet);
    if (!ok)
        File::Delete(destFilePath);
    return ok;
}
#endif

// Given an url https://myserver.com:39/my/url.html :
struct ParsedUrl {
    str::Str<char> protocol; // http
    str::Str<char> server;   // myserver.com
    int port;                // 39 (-1 if not given)
    str::Str<char> url;      // /my/url.html
};

static int DefaultPortForProtocol(const char *proto)
{
    if (NULL == proto) return -1;
    if (str::EqI(proto, "http"))
        return INTERNET_DEFAULT_HTTP_PORT;
    if (str::EqI(proto, "https"))
        return INTERNET_DEFAULT_HTTPS_PORT;
    return -1;
}

// parse urls in the form:
// http://foo.com:port/my/url.html
// The protocol (http://) and port parts are optional
static bool ParseUrl(const char *url, ParsedUrl& pu)
{
    size_t len;
    const char *server = str::Find(url, "://");
    if (server) {
        len = url - server;
        if (0 == len)
            return false;
        pu.protocol.Set(url, len);
        server += 3;
    } else {
        server = url;
    }

    const char *serverEnd = NULL;
    const char *urlPart = str::Find(server, "/");
    if (NULL == urlPart)
        return false;
    const char *port = str::Find(server, ":");
    if (NULL == port) {
        pu.port = DefaultPortForProtocol(pu.protocol.Get());
        serverEnd = urlPart;
    } else {
        serverEnd = port;
        len = urlPart - port - 1;
        if (0 == len)
            return false;
        ScopedMem<char> portStr(str::DupN(port + 1, len));
        pu.port = atoi(portStr);
    }
    len = serverEnd - server;
    if (0 == len)
        return false;
    pu.server.Set(server, len);
    pu.url.Set(urlPart);
    return true;
}

bool HttpPost(const char *url, str::Str<char> *headers, str::Str<char> *data, str::Str<char>& respOut)
{
    ParsedUrl parsedUrl;
    bool ok = ParseUrl(url, parsedUrl);
    if (!ok)
        return false;

    int port = parsedUrl.port;
    if (-1 == port)
        port = INTERNET_DEFAULT_HTTP_PORT;

    HINTERNET hInet = NULL, hConn = NULL, hReq = NULL;
    hInet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInet)
        goto Exit;
    hConn = InternetConnectA(hInet, parsedUrl.server.Get(), parsedUrl.port,
                            NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    if (!hConn)
        goto Exit;

    DWORD flags = INTERNET_FLAG_KEEP_CONNECTION;
    hReq = HttpOpenRequestA(hConn, "POST", parsedUrl.url.Get(), NULL, NULL, NULL, flags, NULL);
    if (!hReq)
        goto Exit;
    char *hdr = NULL;
    DWORD hdrLen = 0;
    if (headers && headers->Count() > 0) {
        hdr = headers->Get();
        hdrLen = headers->Count();
    }
    void *d = NULL;
    DWORD dLen = 0;
    if (data && data->Count() > 0) {
        d = data->Get();
        dLen = data->Count();
    }

    unsigned int timeoutMs = 15 * 1000;
    InternetSetOption(hReq, INTERNET_OPTION_SEND_TIMEOUT,
                      &timeoutMs, sizeof(timeoutMs));

    InternetSetOption(hReq, INTERNET_OPTION_RECEIVE_TIMEOUT,
                      &timeoutMs, sizeof(timeoutMs));

    if (!HttpSendRequestA(hReq, hdr, hdrLen, d, dLen)) {
        //SeeLastError();
        goto Exit;
    }

    // Get the response status.
    DWORD respHttpCode = 0;
    DWORD respHttpCodeSize = sizeof(respHttpCode);
    if (!HttpQueryInfoA(hReq, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &respHttpCode, &respHttpCodeSize, 0)) {
        //SeeLastError();
    }

    DWORD dwRead;
    respOut.Reset();
    static const DWORD readAtATime = 1024;
    do {
        char *buf = respOut.MakeSpaceAtNoLenIncrease(respOut.Count(), readAtATime);
        if (!InternetReadFile(hReq, buf, readAtATime, &dwRead))
            goto Exit;
        respOut.LenIncrease(dwRead);
    } while (dwRead > 0);

#if 0
    // it looks like I should be calling HttpEndRequest(), but it always claims
    // a timeout even though the data has been sent, received and we get HTTP 200
    if (!HttpEndRequest(hReq, NULL, 0, 0)) {
        SeeLastError();
        goto Exit;
    }
#endif
    ok = (200 == respHttpCode);
Exit:
    InternetCloseHandle(hReq);
    InternetCloseHandle(hConn);
    InternetCloseHandle(hInet);
    return ok;
}

#if 0
static DWORD WINAPI HttpDownloadThread(LPVOID data)
{
    HttpReqCtx *ctx = (HttpReqCtx *)data;
    ctx->error = HttpGet(ctx->url, ctx->data);
    if (ctx->callback)
        ctx->callback->Callback(ctx);
    return 0;
}

HttpReqCtx::HttpReqCtx(const TCHAR *url, CallbackFunc *callback)
    : callback(callback), error(0)
{
    assert(url);
    this->url = str::Dup(url);
    data = new str::Str<char>(256);
    if (callback)
        hThread = CreateThread(NULL, 0, HttpDownloadThread, this, 0, 0);
    else
        HttpDownloadThread(this);
}
#endif
