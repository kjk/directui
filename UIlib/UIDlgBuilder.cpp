
#include "StdAfx.h"
#include "UIDlgBuilder.h"


ControlUI* DialogBuilder::Create(const char* xml, IDialogBuilderCallback* cb /*= NULL*/)
{
   m_cb = cb;
   if (!m_xml.Load(xml))  return NULL;
   // NOTE: The root element is actually discarded since the _Parse() methods is
   //       parsing children and attaching to the current node.
   MarkupNode root = m_xml.GetRoot();
   return _Parse(&root);
}

ControlUI* DialogBuilder::CreateFromResource(UINT nRes, IDialogBuilderCallback* cb /*= NULL*/)
{
   ASSERT(0); // TODO: not tested
   HINSTANCE hinst = PaintManagerUI::GetResourceInstance();
   HRSRC hResource = ::FindResourceA(hinst, MAKEINTRESOURCEA(nRes), "XML");
   if (hResource == NULL)  return NULL;
   HGLOBAL hGlobal = ::LoadResource(hinst, hResource);
   if (hGlobal == NULL)  {
      FreeResource(hResource);
      return NULL;
   }
   void *tmp = LockResource(hGlobal);
   size_t len = SizeofResource(hinst, hResource);
   char * s = (char*)memdup(tmp, len);
   //TODO: write me
   //str::Replace(s, "\\n", "\n");
   FreeResource(hResource);
   return Create(s, cb);
}

ControlUI *CreateKnown(const char *cls)
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

ControlUI* DialogBuilder::_Parse(MarkupNode* root, ControlUI* parent)
{
   DialogLayoutUI* pStretched = NULL;
   IContainerUI* pContainer = NULL;
   ControlUI* pReturn = NULL;
   for (MarkupNode node = root->GetChild() ; node.IsValid(); node = node.GetSibling())  {
      const char* cls = node.GetName();

      ControlUI* ctrl = CreateKnown(cls);
      // User-supplied control factory
      if (ctrl == NULL && m_cb != NULL)  {
         ctrl = m_cb->CreateControl(cls);
      }
      ASSERT(ctrl);
      if (ctrl == NULL)  return NULL;
      // Add children
      if (node.HasChildren())  {
         _Parse(&node, ctrl);
      }
      // Attach to parent
      if (parent != NULL)  {
         if (pContainer == NULL)  pContainer = static_cast<IContainerUI*>(parent->GetInterface(_T("Container")));
         ASSERT(pContainer);
         if (pContainer == NULL)  return NULL;
         pContainer->Add(ctrl);
      }
      // Process attributes
      if (node.HasAttributes())  {
         char val[500] = { 0 };
         SIZE_T cchLen = dimof(val) - 1;
         // Set ordinary attributes
         int nAttributes = node.GetAttributeCount();
         for (int i = 0; i < nAttributes; i++)  {
            ctrl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
         }

         // Very custom attributes
         if (node.GetAttributeValue("stretch", val, cchLen))  {
            if (pStretched == NULL)  pStretched = static_cast<DialogLayoutUI*>(parent->GetInterface(_T("DialogLayout")));
            ASSERT(pStretched);
            if (pStretched == NULL)  return NULL;
            UINT uMode = 0;
            if (str::Eq(val, "move_x"))  uMode |= UISTRETCH_MOVE_X;
            if (str::Eq(val, "move_y"))  uMode |= UISTRETCH_MOVE_Y;
            if (str::Eq(val, "move_xy"))  uMode |= UISTRETCH_MOVE_X | UISTRETCH_MOVE_Y;
            if (str::Eq(val, "size_x"))  uMode |= UISTRETCH_SIZE_X;
            if (str::Eq(val, "size_y"))  uMode |= UISTRETCH_SIZE_Y;
            if (str::Eq(val, "size_xy"))  uMode |= UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y;
            if (str::Eq(val, "group"))  uMode |= UISTRETCH_NEWGROUP;
            if (str::Eq(val, "line"))  uMode |= UISTRETCH_NEWLINE;
            pStretched->SetStretchMode(ctrl, uMode);
         }
      }
      // Return first item
      if (pReturn == NULL)  pReturn = ctrl;
   }
   return pReturn;
}

