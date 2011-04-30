#if !defined(AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_)
#define AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_

class IDialogBuilderCallback
{
public:
    virtual ControlUI* CreateControl(const char* pstrClass) = 0;
};

ControlUI* CreateDialogFromXml(const char* xml, IDialogBuilderCallback* cb = NULL);
ControlUI* CreateDialogFromSimple(const char* s, IDialogBuilderCallback* cb = NULL);

#endif // !defined(AFX_BLUEBUILDER_H__20050505_A1C5_1D19_C2BA_0080AD509054__INCLUDED_)
