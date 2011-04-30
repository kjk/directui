#if !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)
#define AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_

#include "UIlist.h"

class UILIB_API TabFolderUI : public ContainerUI, public IListOwnerUI
{
public:
    TabFolderUI();

    virtual const char* GetClass() const;

    void Init();

    bool Add(ControlUI* ctrl);

    int GetCurSel() const;
    bool SelectItem(int idx);

    virtual void Event(TEventUI& Event);

    virtual void SetPos(RECT rc);
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void SetAttribute(const char* name, const char* value);

protected:
    int         m_curSel;
    RECT        m_rcPage;
    RECT        m_rcClient;
    ControlUI*  m_curPage;
    StdValArray m_tabAreas;
};

class UILIB_API TabPageUI : public ContainerUI
{
public:
    TabPageUI();
    virtual const char* GetClass() const;

    virtual bool Activate();
};

#endif // !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)
