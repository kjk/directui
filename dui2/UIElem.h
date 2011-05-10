#ifndef UIElem_h
#define UIElem_h

#include "BaseUtil.h"
#include "Vec.h"

class UIElem;

namespace dui {
void MessageLoop();
}

struct MeasureInfo {
    // in
    SIZE        availSize;
    // out
    SIZE        idealSize;
    SIZE        size;
};

class ILayout {
public:
    virtual void Measure(UIElem *el, MeasureInfo& mi);
};

class IGfx {
public:
};

class UIElem {
public:
    UIElem() : parent(NULL), children(NULL), layout(NULL), hwndParent(NULL)
        , visible(true)
    {
    }

    virtual ~UIElem() {
        if (children)
            DeleteVecMembers(*children);
    }

    // parent in the visual hierarchy
    UIElem *       parent;
    Vec<UIElem*> * children;
    ILayout *      layout;
    HWND           hwndParent;

    bool           visible;
    RECT           pos;

    UIElem *GetParent() {
        return parent;
    }

    void SetParent(UIElem *p) {
        parent=p;
    }

    void AddChild(UIElem *el) {
        if (!children)
            children = new Vec<UIElem*>();
        children->Add(el);
        el->SetParent(this);
    }

    HWND GetHwndParent() {
        if (hwndParent)
            return hwndParent;
        if (GetParent())
            return GetParent()->GetHwndParent();
        return 0;
    }

    void SetVisible(bool v) {
        visible = v;
    }

    bool IsVisible() {
        return visible;
    }

    void SetHwndParent(HWND p) {
        hwndParent = p;
    }

    virtual void Draw(IGfx *g) {
    }

    virtual void Measure(MeasureInfo& mi) {
        if (layout) {
            layout->Measure(this, mi);
        }
    }

};

#endif
