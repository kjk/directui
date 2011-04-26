#ifndef UIMarkup2_h
#define UIMarkup2_h

#include "BaseUtil.h"
#include "Vec.h"

struct MarkupNode2 {
    MarkupNode2 *   parent;
    const char *    name;
    Vec<char*> *    attributes;
    void *          user;
};

class MarkupParserCallback
{
public:
    virtual void NewNode(MarkupNode2 *node) = 0;
};

bool ParseMarkupXml(const char *xml, MarkupParserCallback *cb);
bool ParseMarkupSimple(const char *s, MarkupParserCallback *cb);

#endif
