#ifndef UIElem_h
#define UIElem_h

#include "BaseUtil.h"
#include "Vec.h"
#include "WinUtil.h"

using namespace Gdiplus;

namespace dui {

inline const Rect RectFromRECT(const RECT& rc)
{
    Rect ret(rc.left, rc.top, RectDx(rc), RectDy(rc));
    return ret;
}

class UIElem;

void MessageLoop();

struct MeasureInfo {
    // in
    SIZE        availSize;
    // out
    SIZE        idealSize;
    SIZE        size;
};

class ILayout {
public:
    virtual void Measure(UIElem *el, Graphics *g, MeasureInfo& mi) = 0;
};

class LayoutVertical : public ILayout {
    virtual void Measure(UIElem *el, Graphics *g, MeasureInfo& mi);
};

class WinHwnd {
    HINSTANCE   hinst;
    HWND        hwnd;
public:
    WinHwnd(HINSTANCE hinst) : hinst(hinst), hwnd(NULL)
    {}

    void Create(UIElem *root, const char* name, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int dx = CW_USEDEFAULT, int dy = CW_USEDEFAULT, HMENU hMenu = NULL);
    void Create(UIElem *root, const char* name, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
};

class UIPainter {
    HWND          hwnd;
    PAINTSTRUCT   ps;
    Bitmap *      bmp;
    Graphics *    gfx;

public:
    UIPainter() : bmp(NULL), gfx(NULL)
    {}

    ~UIPainter() {
        Reset();
    }

    void Reset() {
        delete bmp;
        bmp = NULL;
        delete gfx;
        gfx = NULL;
    }

    void PaintBegin(HWND hwnd, ARGB bgColor);
    void PaintUIElem(UIElem* el);
    void PaintEnd();
};

class UIElem {
public:
    UIElem() : parent(NULL), children(NULL), layout(NULL), hwndParent(NULL)
        , visible(true)
    {
    }

    virtual ~UIElem() {
        if (children) {
            DeleteVecMembers(*children);
            delete children;
        }
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

    void SetHwndParent(HWND p) {
        hwndParent = p;
    }

    HWND GetHwndParent() {
        if (hwndParent)
            return hwndParent;
        if (parent)
            return parent->GetHwndParent();
        return 0;
    }

    void SetPosition(const RECT& p) {
        pos = p;
    }

    UIElem *ChildFromPoint(int x, int y);

    void SetVisible(bool v) {
        visible = v;
    }

    bool IsVisible() {
        return visible;
    }

    // part that needs to be redrawn can be different
    // (usually bigger) than the size of the element
    // (e.g. to show shadow but not make it an active area
    // of the element)
    virtual void PaintRect(RECT *r)
    {
        *r = pos;
    }

    void DrawChildren(Graphics *g);

    virtual void Draw(Graphics *g) {
        DrawChildren(g);
    }

    virtual void Measure(Graphics *g, MeasureInfo& mi) {
        if (layout) {
            layout->Measure(this, g, mi);
        }
    }

};

class UIRectangle : public UIElem {
public:
    UIRectangle()
    {}

    virtual ~UIRectangle()
    {}

    virtual void Draw(Graphics *g);
};

class UIText : public UIElem {
    const char *text;

public:
    UIText() : UIElem(), text(NULL)
    {}

    virtual ~UIText()
    {}

    const char *GetText() {
        return text;
    }

    void SetText(const char *txt) {
        str::Replace(text, txt);
    }

    virtual void Measure(Graphics *g, MeasureInfo& mi) {
        if (!text) {
            static SIZE zeroSize = { 0, 0 };
            mi.idealSize = zeroSize;
            mi.size = zeroSize;
            return;
        }
        //ScopedMem<WCHAR> s(str::Utf8ToUni(text));
        //g->MeasureString(
    }

    virtual void Draw(Graphics *g) {
        if (!text)
            return;
        ScopedMem<WCHAR> s(str::Utf8ToUni(text));
        PointF o = PointFFromRECT(pos);
        g->DrawString(s, str::Len(s), NULL, o, NULL);
    }
};

} // namespace dui
#endif
