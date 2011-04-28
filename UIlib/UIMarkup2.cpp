#include "UIMarkup2.h"
#include "StrUtil.h"

class ParserState
{
public:
    ParserState(const char *s, MarkupParserCallback *cb) {
        this->txt = str::Dup(s);
        this->curr = this->txt;
        this->cb = cb;
    }

    ~ParserState() { 
        for (size_t i=0; i<nodes.Count(); i++) {
            delete nodes.At(i).attributes;
        }
        free(txt);
    }

    MarkupNode2 *AllocNode(int& idx, int parentIdx) {
        idx = nodes.Count();
        MarkupNode2 *ret = nodes.MakeSpaceAt(idx);
        ret->parserState = this;
        ret->parentIdx = parentIdx;
        return ret;
    }

    MarkupNode2 *Node(int idx) {
        return &nodes.At(idx);
    }

    char *                  txt;
    char *                  curr;
    Vec<MarkupNode2>        nodes;
    MarkupParserCallback *  cb;
};

MarkupNode2 *MarkupNode2::Parent()
{
    if (parentIdx >= 0)
        return parserState->Node(parentIdx);
    return NULL;
}

enum XmlTagType {
    TAG_INVALID,
    TAG_OPEN,           // <foo>
    TAG_CLOSE,          // </foo>
    TAG_OPEN_CLOSE      // <foo/>
};

class XmlTagInfo {
public:
    XmlTagInfo() : type(TAG_INVALID), name(NULL), attributes(NULL) {}
    // we assume that the ownership of only data XmlTagInfo owns (attributes)
    // will be transferred to the client, so destructor does nothing
    ~XmlTagInfo() {}
    XmlTagType   type;
    char *       name;
    Vec<char*> * attributes;
};

static void SkipWhitespace(char*& s)
{
   while (*s && *s <= ' ')
      s++;
}

static void SkipIdentifier(char*& s)
{
   while (*s && (*s == '_' || *s == ':' || isalnum(*s)))
      s++;
}

static void ParseEntity(char*& s, char*& dst)
{
   if (s[0] == 'a' && s[1] == 'm' && s[2] == 'p' && s[3] == ';')  {
      *dst++ = '&';
      s += 4;
   } else if (s[0] == 'l' && s[1] == 't' && s[2] == ';')  {
      *dst++ = '<';
      s += 3;
   } else if (s[0] == 'g' && s[1] == 't' && s[2] == ';')  {
      *dst++ = '>';
      s += 3;
   } else if (s[0] == 'q' && s[1] == 'u' && s[2] == 'o' && s[3] == 't' && s[4] == ';')  {
      *dst++ = '\"';
      s += 5;
   } else if (s[0] == 'a' && s[1] == 'p' && s[2] == 'o' && s[3] == 's' && s[4] == ';')  {
      *dst++ = '\'';
      s += 5;
   } else {
      *dst++ = '&';
   }
}

static bool ParseXmlData(char*& s, char*& dst, char until)
{
   while (*s && *s != until)  {
      if (*s == '&')  {
         ParseEntity(++s, dst);
      }
      if (*s == ' ')  {
         *dst++ = *s++;
         //if (!m_bPreserveWhitespace)  SkipWhitespace(s);
      } else {
         *dst++ = *s++;
      }
   }
   // Make sure that MapAttributes() works correctly when it parses
   // over a value that has been transformed.
   char* sfill = dst + 1;
   while (sfill < s)
      *sfill++ = ' ';
   return true;
}

static bool ParseAttributes(char* s, XmlTagInfo *tagInfo)
{
    for (;;) {
        SkipWhitespace(s);
        if (!*s)
            return true;
        char *name = s;
        SkipIdentifier(s);
        if (*s != '=')
            return false;
        *s++ = 0;
        char quoteChar = *s++;
        if (quoteChar != '\"' && quoteChar != '\'')
            return false;
        char *value = s;
        char* dst = s;
        if (!ParseXmlData(s, dst, quoteChar))
            return false;
        if (!*s)
            return false;
        *dst = '\0';
        *s++ = '\0';
        if (!tagInfo->attributes)
            tagInfo->attributes = new Vec<char*>();
        tagInfo->attributes->Append(name);
        tagInfo->attributes->Append(value);        
    }
    return true;
}

static bool SkipCommentOrProcesingInstr(char *& s, bool& skipped)
{
    skipped = false;
    if (!(*s == '!' || *s == '?'))
        return true;
    char end = (*s == '!') ? '-' : '?';
    while (*s) {
        if (*s == end) {
            if (s[1] == '>')
                goto Exit;
            return false;
        }
        // TODO: this should be utf8-aware
        s++;
    }
Exit:
    if (*s)
        s += 2;
    skipped = true;
    return true;
}

// The xml variant used by directui allows unescaped '<' and '>' inside attribute
// values, so as work-around, find closing '>' by looking for first unbalanced '>'
char *FindTagClose(char *s)
{
    int nest = 0;
    while (*s) {
        if (*s == '<')
            ++nest;
        else if (*s == '>')
        {
            if (0 == nest)
                return s;
            --nest;
        }
        ++s;
    }
    return NULL;
}

// parse xml tag information i.e. extract tag name and attributes
// until closing '>'. We've already consumed opening '<' in the caller.
static bool ParseTag(char *& s, XmlTagInfo& tagInfo)
{
    tagInfo.type = TAG_OPEN;
    if (*s == '/') { // '</foo>
        ++s;
        tagInfo.type = TAG_CLOSE;
    }
    tagInfo.name = s;
    SkipIdentifier(s);
    if (!*s)
        return false;
    char *e = FindTagClose(s);
    if (!e)
        return false;
    *e = 0;
    char *tmp = s;
    s = e + 1;
    if (e[-1] == '/') { // <foo/>
        if (tagInfo.type != TAG_OPEN) // but not </foo/>
            return false;
        tagInfo.type = TAG_OPEN_CLOSE;
        e[-1] = 0;
    }
    bool ok = ParseAttributes(tmp, &tagInfo);
    *tmp = 0;
    return ok;
}

static bool ParseXmlRecur(ParserState *state, int parentIdx=-1)
{
    for (;;)
    {
        char *s = state->curr;
        XmlTagInfo tagInfo;

        SkipWhitespace(s);
        if (!*s) {
            if (parentIdx == -1)
                return true;
            return false;
        }

        if (*s != '<')
            return false;
        s++;

        bool skipped;
        if (!SkipCommentOrProcesingInstr(s, skipped))
            return false;
        if (skipped) {
            state->curr = s;
            continue;
        }

        if (!ParseTag(s, tagInfo))
            return false;

        if (TAG_CLOSE == tagInfo.type) {
            if (NULL != tagInfo.attributes) {
                delete tagInfo.attributes;
                return false;
            }
            state->curr = s;
            MarkupNode2 *parent = state->Node(parentIdx);
            if (!str::Eq(parent->name, tagInfo.name))
                return false;
            return true;
        }

        int nodeIdx;
        MarkupNode2 *node = state->AllocNode(nodeIdx, parentIdx);
        node->name = tagInfo.name;
        node->attributes = tagInfo.attributes;
        node->user = NULL;

        state->cb->NewNode(node);
        if (TAG_OPEN_CLOSE == tagInfo.type) {
            state->curr = s;
            continue;
        }

        assert(TAG_OPEN == tagInfo.type);
        state->curr = s;
        if (!ParseXmlRecur(state, nodeIdx))
            return false;
    }
}

bool ParseMarkupXml(const char *xml, MarkupParserCallback *cb)
{
    ParserState *state = new ParserState(xml, cb);
    bool ok = ParseXmlRecur(state);
    delete state;
    return ok;
}

static int Indent(char *s)
{
    char *e = s;
    while (*e == ' ')
        e++;
    return e - s;
}

static inline bool IsNewline(char c)
{
    return c == '\n' || c == '\t';
}

static char *FindEndLine(char *s)
{
    while (*s && !IsNewline(*s))
        ++s;
    if (!*s)
        return s;
    *s++ = 0;
    while (IsNewline(*s))
        ++s;
    return s;
}

static bool IsQuoteChar(char c)
{
    return c == '\'' || c == '\"';
}

static char *FindQuotedEnd(char *s, int& len)
{
    char *start = s;
    if (IsQuoteChar(*s)) {
        char c = *s++;
        char *res = s;
        while (*s && *s != c)
            ++s;
        if (!*s)
            return NULL;
        *s++ = 0;
        len = s - start;
        return res;
    }

    while (*s && *s != ' ')
        ++s;
    if (*s == ' ')
        *s++ = 0;
    len = s - start;
    return start;
}

static Vec<char*> *ParseAttributesSimple(char* s, bool& ok)
{
    Vec<char*> *attributes = NULL;
    while (*s)
    {
        SkipWhitespace(s);
        if (!*s)
            return attributes;
        char *name = s;
        SkipIdentifier(s);
        if (*s++ != '=')
            goto Error;
        int len;
        char *val = FindQuotedEnd(s, len);
        if (NULL == len)
            goto Error;
        if (NULL == attributes)
            attributes = new Vec<char*>();
        attributes->Append(name);
        attributes->Append(val);
        s += len;
    }
    return NULL;
Error:
    ok = false;
    delete attributes;
    return NULL;
}

static bool ParseSimpleRecur(ParserState *state, int parentIdx=-1)
{
    int prevIndent = -1;
    char *s = state->curr;
    for (;;) {
        if (!*s)
            return true;
        char *s = state->curr;
        char *e = FindEndLine(s);
        int indent = Indent(s);
        s += indent;
        char *name = s;
        SkipIdentifier(s);
        bool ok;
        Vec<char*> *attributes = ParseAttributesSimple(s, ok);
        if (!ok)
            return false;
        int nodeIdx;
        MarkupNode2 *node = state->AllocNode(nodeIdx, parentIdx);
        node->name = name;
        node->attributes = attributes;
        node->user = NULL;
        s = e;
        // TODO: write me, my friend
    }
}

bool ParseMarkupSimple(const char *s, MarkupParserCallback *cb)
{
    ParserState *state = new ParserState(s, cb);
    bool ok = ParseSimpleRecur(state);
    delete state;
    return ok;
}
