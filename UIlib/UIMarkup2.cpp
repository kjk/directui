#include "UIMarkup2.h"
#include "StrUtil.h"

class XmlState
{
public:
    XmlState(const char *s, MarkupParserCallback *cb) {
        this->s = str::Dup(s);
        this->curr = this->s;
        this->cb = cb;
    }
    ~XmlState() { free(s); }

    MarkupNode2 *AllocNode() {
        return nodes.MakeSpaceAt(nodes.Count());
    }

    char *                  s;
    char *                  curr;
    Vec<MarkupNode2>        nodes;
    MarkupParserCallback *  cb;
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

static bool ParseAttributes(char*& s, MarkupNode2 *node)
{
    if (*s == '>')
        return true;
    *s++ = 0;
    SkipWhitespace(s);
    while (*s && *s != '>' && *s != '/')  {
        char *name = s;
        SkipIdentifier(s);
        if (*s != '=')
            return false;
        *s++ = 0;
        char chQuote = *s++;
        if (chQuote != '\"' && chQuote != '\'')
            return false;
        char *value = s;
        char* dst = s;
        if (!ParseXmlData(s, dst, chQuote))
            return false;
        if (!*s)
            return false;
        *dst = '\0';
        *s++ = '\0';
        if (!node->attributes)
            node->attributes = new Vec<char*>();
        node->attributes->Append(name);
        node->attributes->Append(value);        
        SkipWhitespace(s);
    }
    return true;
}

static bool ParseXmlRecur(XmlState *state, MarkupNode2 *parent)
{
    char *s = state->s;
    for (;;)
    {
        if (!*s) return parent != NULL;
        if (*s != '<')
            return false;
        *s++ = 0;
        
        // Skip comment or processing directive
        if (*s == '!' || *s == '?')  {
            char chEnd = *s == '!' ? '-' : '?';
            while (*s && !(*s == chEnd && *(s + 1) == '>')) {
                s = ::CharNextA(s);
            }
            if (*s)\
                s += 2;
            SkipWhitespace(s);
            continue;
        }
        char *name = s;
        SkipIdentifier(s);
        if (!*s)
            return false;
        size_t nameLen = s - name;

        MarkupNode2 *node = state->AllocNode();
        node->parent = parent;
        node->attributes = NULL;
        node->name = name;
        node->user = NULL;

        if (!ParseAttributes(s, node))
            return false;

        SkipWhitespace(s);
        if (s[0] == '/' && s[1] == '>') 
        {
            *s = 0;
            s += 2;
            state->cb->NewNode(node);
        }
        else
        {
            if (*s != '>')
                return false;
            char* dst = s;
            if (!ParseXmlData(s, dst, '<'))
                return false;

            if (!*s && !parent)
                return true;
            if (*s != '<')
                return false;

            char tmp = name[nameLen];
            name[nameLen] = 0;
            state->cb->NewNode(node);
            name[nameLen] = tmp;

            if (s[0] == '<' && s[1] != '/')  
            {
                state->s = s;
                if (!ParseXmlRecur(state, node))
                    return false;
            }

            if (s[0] == '<' && s[1] == '/')  
            {
                *dst = 0;
                *s = 0;
                s += 2;
                if (!str::EqN(s, name, nameLen))
                    return false;
                if (s[nameLen] != '>')
                    return false;
                s += nameLen + 1;
            }
        }
        SkipWhitespace(s);
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

