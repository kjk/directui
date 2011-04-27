#include "UIMarkup2.h"
#include "StrUtil.h"

class XmlState
{
public:
    XmlState(const char *s, MarkupParserCallback *cb) {
        this->xml = str::Dup(s);
        this->curr = this->xml;
        this->cb = cb;
    }
    ~XmlState() { free(xml); }

    MarkupNode2 *AllocNode() {
        return nodes.MakeSpaceAt(nodes.Count());
    }

    char *                  xml;
    char *                  curr;
    Vec<MarkupNode2>        nodes;
    MarkupParserCallback *  cb;
};

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

// parse xml tag information i.e. extract tag name and attributes
// until closing '>'. We've already consumed opening '<' in the caller.
static bool ParseTag(char *& s, XmlTagInfo& tagInfo)
{
    tagInfo.type = TAG_OPEN;
    if (*s == '/') { // '</foo>
        ++s;
        tagInfo.type = TAG_OPEN_CLOSE;
    }
    tagInfo.name = s;
    SkipIdentifier(s);
    if (!*s)
        return false;
    char *e = (char*)str::Find(s, ">"); // TODO: use more efficient str::Find(s, '>')
    if (!e)
        return false;
    *e = 0;
    char *tmp = s;
    s = e + 1;
    if (e[-1] == '/') { // <foo/>
        if (tagInfo.type != TAG_OPEN) // but not </foo/>
            return false;
        tagInfo.type = TAG_CLOSE;
        --*e = 0;
    }
    return ParseAttributes(tmp, &tagInfo);
}

static bool ParseXmlRecur(XmlState *state, MarkupNode2 *parent)
{
    char *s = state->curr;
    for (;;)
    {
        XmlTagInfo tagInfo;

        SkipWhitespace(s);
        if (!*s)
            return parent != NULL;

        if (*s != '<')
            return false;
        s++;

        bool skipped;
        if (!SkipCommentOrProcesingInstr(s, skipped))
            return false;
        if (skipped)
            continue;

        if (!ParseTag(s, tagInfo))
            return false;

        if (TAG_CLOSE == tagInfo.type) {
            if (NULL != tagInfo.attributes) {
                delete tagInfo.attributes;
                return false;
            }
            return str::Eq(parent->name, tagInfo.name);
        }

        MarkupNode2 *node = state->AllocNode();
        node->parent = parent;
        node->attributes = tagInfo.attributes;
        node->name = tagInfo.name;
        node->user = NULL;

        state->cb->NewNode(node);
        if (TAG_OPEN_CLOSE == tagInfo.type)
            continue;

        assert(TAG_OPEN == tagInfo.type);
        state->curr = s;
        if (!ParseXmlRecur(state, node))
            return false;
    }
}

bool ParseMarkupXml(const char *xml, MarkupParserCallback *cb)
{
    XmlState *state = new XmlState(xml, cb);
    bool ok = ParseXmlRecur(state, NULL);
    delete state;
    return ok;
}

bool ParseMarkupSimple(const char *s, MarkupParserCallback *cb)
{

    return true;
}

