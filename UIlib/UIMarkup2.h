#ifndef UIMarkup2_h
#define UIMarkup2_h

#include "BaseUtil.h"
#include "Vec.h"

class ParserState;

class MarkupNode2 {
    friend ParserState;
    int             parentIdx;
    ParserState *   parserState;
public:
    const char *    name;
    Vec<char*> *    attributes;
    void *          user;

    MarkupNode2 *   Parent();
};

class MarkupParserCallback
{
public:
    virtual void NewNode(MarkupNode2 *node) = 0;
};

bool ParseMarkupXml(const char *xml, MarkupParserCallback *cb);
bool ParseMarkupSimple(const char *s, MarkupParserCallback *cb);

#endif
