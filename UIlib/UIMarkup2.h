#ifndef UIMarkup2_h
#define UIMarkup2_h

#include "BaseUtil.h"
#include "Vec.h"

class XmlState;

class MarkupNode2 {
    friend XmlState;
    int             parentIdx;
    XmlState *      xmlState;
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
