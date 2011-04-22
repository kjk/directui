#if !defined(AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_)
#define AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_

class IDialogBuilderCallback
{
public:
   virtual ControlUI* CreateControl(const char* pstrClass) = 0;
};

class UILIB_API DialogBuilder
{
public:
   ControlUI* Create(const char* xml, IDialogBuilderCallback* pCallback = NULL);
   ControlUI* CreateFromResource(UINT nRes, IDialogBuilderCallback* pCallback = NULL);

private:
   ControlUI* _Parse(MarkupNode* root, ControlUI* parent = NULL);

   MarkupParser m_xml;
   IDialogBuilderCallback* m_pCallback;
};

#endif // !defined(AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_)

