/* Copyright 2011 the Wdl Extra authors (see AUTHORS file).
   License: Simplified BSD (see COPYING) */

#include "SettingsParser.h"

/* This is a parser for a simple, human-readable format for storing (potentially nested)
   objects and their properties. The format looks like:

Window
  Size
    Width: 100
    Height: 200
  Pos
    X: 356
    Y: 118

It shows a serialized Window object with embedded Size object (which has Widht/Height properties)
and embedded Pos object (with X and Y properties). (In real life we would encode Size and Pos in
more compact form as: "Size: 100x200", "Pos: 356x118").

Indentation is used to indicate which object owns the values/child objects. 

Arrays can be stored by repeating objects or values with the same name, e.g.:

AppState
  RecentlyOpenedFile: c:\file1.pdf
  RecentlyOpenedFile: c:\file2.pdf

*/

struct LineInfo {
    char *     name;   // always present
    char *     value;  // only for values
    int        indent;
};

static char* SkipSpaces(char *s)
{
    while (*s == ' ')
        ++s;
    return s;
}

static inline bool IsNewLine(char c)
{
    return c == '\r' || c == '\n';
}

static char *ParseLineIter(char *s, size_t& sLenInOut, LineInfo& liOut)
{
NextLine:
    if (0 == sLenInOut)
        return NULL;
    char *sStart = s;
    char *sEnd = sStart + sLenInOut;
    while ((s < sEnd) && !IsNewLine(*s)) {
        ++s;
    }
    bool emptyLine = (s == sStart);

    while ((s < sEnd) && IsNewLine(*s)) {
        *s++ = 0;
    }
    size_t sLen = s - sStart;
    sLenInOut -= sLen;
    if (emptyLine)
        goto NextLine;

    liOut.name = SkipSpaces(sStart);
    liOut.indent = liOut.name - sStart;
    liOut.value = NULL;
    char *value = (char*)str::Find(liOut.name, ":");
    if (NULL != value) {
        // null out ':"
        *value++ = 0;
        liOut.value = SkipSpaces(value);
    }
    return s;
}

static void SplitIntoLines(char *s, size_t sLen, Vec<LineInfo>& lines)
{
    lines.Reset();
    for (;;) {
        LineInfo li;
        s = ParseLineIter(s, sLen, li);
        if (NULL == s)
            break;
        lines.Append(li);
    }
}

class SettingsParserImpl {
    char         *  s;

    Vec<LineInfo>   lines;
    size_t          pos;

public:
    SettingsParserImpl() {
        pos = 0;
        s = NULL;
    }

    ~SettingsParserImpl() {
        free(s);
    }
    bool InitializeWithString(const char *s, size_t sLen=-1);
    void Parse(ISettingsParserDelegate *newValue, ISettingsParserDelegate *newObject);
};

bool SettingsParserImpl::InitializeWithString(const char *s, size_t sLen)
{
    if (-1 == sLen)
        sLen = str::Len(s);
    this->s = (char*) calloc(sLen + 1, 1); // +1 to ensure terminating zero
    if (!this->s)
        return false;
    memcpy(this->s, s, sLen);
    SplitIntoLines(this->s, sLen, lines);
    return true;
}

void SettingsParserImpl::Parse(ISettingsParserDelegate *newValue, ISettingsParserDelegate *newObject)
{
    int objectIndent = -1;
    if (pos > 0)
        objectIndent = lines.At(pos - 1).indent;
    while (pos < lines.Count())
    {
        LineInfo li = lines.At(pos);
        if (-1 == objectIndent)
            objectIndent = li.indent;
        else if (li.indent <= objectIndent)
            break;
        ++pos;

        if (li.value) {
            if (newValue)
                newValue->NewValue(li.name, li.value);
        } else {
            if (newObject)
                newObject->NewObject(li.name);
        }
    }
}

SettingsParser::SettingsParser()
{
    impl = new SettingsParserImpl();
}

SettingsParser::~SettingsParser()
{
    delete impl;
}

SettingsParser *SettingsParser::CreateFromString(const char *s, size_t sLen)
{
    SettingsParser *sp = new SettingsParser();
    bool ok = sp->impl->InitializeWithString(s);
    if (ok)
        return sp;
    delete sp;
    return NULL;
}

void SettingsParser::Parse(ISettingsParserDelegate *newValue, ISettingsParserDelegate *newObject)
{
    impl->Parse(newValue, newObject);
}

void AddIndentSpaces(str::Str<char>& s, size_t indent)
{
    for (size_t i=0; i<indent; i++) {
        // 2 spaces for each level for readability
        s.Append("  ");
    }
}

void SerializeObjectName(const char *name, size_t indent, str::Str<char>& s)
{
    AddIndentSpaces(s, indent);
    s.Append(name);
    s.Append("\r\n");
}

void SerializeValue(const char *name, const char *value, size_t indent, str::Str<char>& s)
{
    AddIndentSpaces(s, indent);
    s.AppendFmt("%s: %s\r\n", name, value);
}

void SerializeValueObfuscated(const char *name, const char *value, size_t indent, str::Str<char>& s)
{
    // TODO: obfuscate value
    SerializeValue(name, value, indent, s);
}

void SerializePos(int x, int y, size_t indent, str::Str<char>& s)
{
    AddIndentSpaces(s, indent);
    s.AppendFmt("Pos: %dx%d\r\n", x, y);
}

void SerializeSize(int dx, int dy, size_t indent, str::Str<char>& s)
{
    AddIndentSpaces(s, indent);
    s.AppendFmt("Size: %dx%d\r\n", dx, dy);
}

bool DeserializePos(const char *s, int& x, int& y)
{
    ScopedMem<char> xStr(str::Dup(s));
    char *yStr = (char*)str::Find(xStr, "x");
    if (!yStr)
        return false;
    *yStr = 0; yStr++;
    x = atoi(xStr);
    y = atoi(yStr);
    return true;
}

bool DeserializeSize(const char *s, int& dx, int& dy)
{
    return DeserializePos(s, dx, dy);
}
