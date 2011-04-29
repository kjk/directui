#include "StdAfx.h"
#include "UIDlgBuilder.h"
#include "UIMarkup2.h"

static ControlUI *CreateKnown(const char *cls)
{
    switch (str::Len(cls))  {
    case 4:
       if (str::Eq(cls, "List")) return new ListUI;
       break;
    case 6:
       if (str::Eq(cls, "Canvas"))         return new CanvasUI;
       else if (str::Eq(cls, "Button"))    return new ButtonUI;
       else if (str::Eq(cls, "Option"))    return new OptionUI;
       break;
    case 7:
       if (str::Eq(cls, "Toolbar"))        return new ToolbarUI;
       else if (str::Eq(cls, "TabPage"))   return new TabPageUI;
       else if (str::Eq(cls, "ActiveX"))   return new ActiveXUI;
       break;
    case 8:
       if (str::Eq(cls, "DropDown"))       return new DropDownUI;
       break;
    case 9:
       if (str::Eq(cls, "FadedLine"))      return new FadedLineUI;      
       else if (str::Eq(cls, "TaskPanel")) return new TaskPanelUI;
       else if (str::Eq(cls, "Statusbar")) return new StatusbarUI;
       else if (str::Eq(cls, "TabFolder")) return new TabFolderUI;
       else if (str::Eq(cls, "TextPanel")) return new TextPanelUI;
       break;
    case 10:
       if (str::Eq(cls, "ListHeader"))        return new ListHeaderUI;
       else if (str::Eq(cls, "ListFooter"))   return new ListFooterUI;
       else if (str::Eq(cls, "TileLayout"))   return new TileLayoutUI;
       else if (str::Eq(cls, "ToolButton"))   return new ToolButtonUI;
       else if (str::Eq(cls, "ImagePanel"))   return new ImagePanelUI;
       else if (str::Eq(cls, "LabelPanel"))   return new LabelPanelUI;
       break;
    case 11:
       if (str::Eq(cls, "ToolGripper"))       return new ToolGripperUI;
       else if (str::Eq(cls, "WhiteCanvas"))  return new WhiteCanvasUI;
       else if (str::Eq(cls, "TitleShadow"))  return new TitleShadowUI;
       break;
    case 12:
       if (str::Eq(cls, "WindowCanvas"))      return new WindowCanvasUI;
       else if (str::Eq(cls, "DialogCanvas")) return new DialogCanvasUI;
       else if (str::Eq(cls, "DialogLayout")) return new DialogLayoutUI;
       else if (str::Eq(cls, "PaddingPanel")) return new PaddingPanelUI;
       else if (str::Eq(cls, "WarningPanel")) return new WarningPanelUI;
       break;
    case 13:
       if (str::Eq(cls, "SeparatorLine"))      return new SeparatorLineUI;
       else if (str::Eq(cls, "ControlCanvas")) return new ControlCanvasUI;
       else if (str::Eq(cls, "MultiLineEdit")) return new MultiLineEditUI;
       else if (str::Eq(cls, "ToolSeparator")) return new ToolSeparatorUI;
       break;
    case 14:
       if (str::Eq(cls, "VerticalLayout"))      return new VerticalLayoutUI;
       else if (str::Eq(cls, "SingleLineEdit")) return new SingleLineEditUI;
       else if (str::Eq(cls, "SingleLinePick")) return new SingleLinePickUI;
       else if (str::Eq(cls, "NavigatorPanel")) return new NavigatorPanelUI;
       else if (str::Eq(cls, "ListHeaderItem")) return new ListHeaderItemUI;
       else if (str::Eq(cls, "GreyTextHeader")) return new GreyTextHeaderUI;
       break;
    case 15:
       if (str::Eq(cls, "ListTextElement"))      return new ListTextElementUI;
       else if (str::Eq(cls, "NavigatorButton")) return new NavigatorButtonUI;      
       else if (str::Eq(cls, "TabFolderCanvas")) return new TabFolderCanvasUI;      
       break;
    case 16:
       if (str::Eq(cls, "ListHeaderShadow"))      return new ListHeaderShadowUI; 
       else if (str::Eq(cls, "HorizontalLayout")) return new HorizontalLayoutUI;
       else if (str::Eq(cls, "ListLabelElement")) return new ListLabelElementUI;
       else if (str::Eq(cls, "SearchTitlePanel")) return new SearchTitlePanelUI;
       break;
    case 17:
       if (str::Eq(cls, "ToolbarTitlePanel"))   return new ToolbarTitlePanelUI;
       else if (str::Eq(cls, "ListExpandElement")) return new ListExpandElementUI;
       break;
    }
    return NULL;
}

class UIBuilderParserCallback : MarkupParserCallback {
    ControlUI*                first;
    IDialogBuilderCallback *  cb;
    DialogLayoutUI*           stretched;

public:
    UIBuilderParserCallback() : first(NULL), stretched(NULL) {}
    ~UIBuilderParserCallback() {}

    ControlUI *ParseXml(const char *xml, IDialogBuilderCallback* cb);
    ControlUI *ParseSimple(const char *s, IDialogBuilderCallback* cb);
    virtual void NewNode(MarkupNode2 *node);
};

static UINT GetStretchMode(const char *val)
{
    UINT mode = 0;
    if (str::Eq(val, "move_x"))  mode |= UISTRETCH_MOVE_X;
    if (str::Eq(val, "move_y"))  mode |= UISTRETCH_MOVE_Y;
    if (str::Eq(val, "move_xy")) mode |= UISTRETCH_MOVE_X | UISTRETCH_MOVE_Y;
    if (str::Eq(val, "size_x"))  mode |= UISTRETCH_SIZE_X;
    if (str::Eq(val, "size_y"))  mode |= UISTRETCH_SIZE_Y;
    if (str::Eq(val, "size_xy")) mode |= UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y;
    if (str::Eq(val, "group"))   mode |= UISTRETCH_NEWGROUP;
    if (str::Eq(val, "line"))    mode |= UISTRETCH_NEWLINE;
    return mode;
}

void UIBuilderParserCallback::NewNode(MarkupNode2 *node)
{
    ControlUI* parent = NULL;
    const char *cls = node->name;

    // for compat with the old code, skip the Dialog node if it's the first node
    if (str::Eq(cls, "Dialog") && !node->Parent())
        return;

    ControlUI* ctrl = CreateKnown(cls);
    if (ctrl == NULL && cb != NULL)  {
       ctrl = cb->CreateControl(cls);
    }
    if (!ctrl)
        return;
    if (NULL == first)
        first = ctrl;

    assert(NULL == node->user);
    node->user = (void*)ctrl;

    if (node->Parent()) {
        parent = (ControlUI*) node->Parent()->user;
        if (parent) {
            IContainerUI* container = (IContainerUI*)parent->GetInterface("Container");
            if (container)
                container->Add(ctrl);
        }
    }

    if (NULL == node->attributes)
        return;
    int n = node->attributes->Count() / 2;
    for (int i = 0; i < n; i++) {
        char *name = node->attributes->At(i*2);
        char *val = node->attributes->At(i*2+1);
        if (str::Eq(name, "stretch")) {
            if (stretched == NULL)
                stretched = (DialogLayoutUI*)parent->GetInterface("DialogLayout");
            ASSERT(stretched);
            if (stretched) {
                UINT mode = GetStretchMode(val);
                stretched->SetStretchMode(ctrl, mode);
            }
        } else {
            ctrl->SetAttribute(name, val);
        }
    }
}

ControlUI *UIBuilderParserCallback::ParseXml(const char *xml, IDialogBuilderCallback* cb)
{
    this->cb = cb;
    ParseMarkupXml(xml, this);
    return first;
}

ControlUI *UIBuilderParserCallback::ParseSimple(const char *s, IDialogBuilderCallback* cb)
{
    this->cb = cb;
    ParseMarkupSimple(s, this);
    return first;
}

ControlUI* CreateDialogFromXml(const char* xml, IDialogBuilderCallback* cb)
{
    UIBuilderParserCallback *p = new UIBuilderParserCallback();
    ControlUI* res = p->ParseXml(xml, cb);
    delete p;
    return res;
}

ControlUI* CreateDialogFromSimple(const char* s, IDialogBuilderCallback* cb)
{
    UIBuilderParserCallback *p = new UIBuilderParserCallback();
    ControlUI* res = p->ParseSimple(s, cb);
    delete p;
    return res;
}

