#ifndef UIMarkup_h
#define UIMarkup_h

#include "BaseUtil.h"
#include "Vec.h"

class ParserState;

class MarkupNode {
    friend ParserState;
    int             parentIdx;
    ParserState *   parserState;
public:
    const char *    name;
    Vec<char*> *    attributes;
    void *          user;

    MarkupNode *    Parent();
};

class MarkupParserCallback
{
public:
    virtual void NewNode(MarkupNode *node) = 0;
};

bool ParseMarkupXml(const char *xml, MarkupParserCallback *cb);
bool ParseMarkupSimple(const char *s, MarkupParserCallback *cb);

#endif
