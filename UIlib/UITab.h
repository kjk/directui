#if !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)
#define AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_

#include "UIlist.h"

class UILIB_API TabFolderUI : public ContainerUI, public IListOwnerUI
{
public:
   TabFolderUI();

   const TCHAR* GetClass() const;

   void Init();

   bool Add(ControlUI* ctrl);

   int GetCurSel() const;
   bool SelectItem(int idx);

   void Event(TEventUI& Event);

   void SetPos(RECT rc);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(const TCHAR* name, const TCHAR* value);

protected:
   int m_curSel;
   RECT m_rcPage;
   RECT m_rcClient;
   ControlUI* m_curPage;
   StdValArray m_tabAreas;
};

class UILIB_API TabPageUI : public ContainerUI
{
public:
   TabPageUI();
   const TCHAR* GetClass() const;

   bool Activate();
};

#endif // !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)

