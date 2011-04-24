#if !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

class UILIB_API SingleLinePickUI : public ControlUI
{
public:
   SingleLinePickUI();

   void SetWidth(int cxWidth);

   virtual const char* GetClass() const;
   virtual UINT GetControlFlags() const;
   virtual void Event(TEventUI& event);

   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   RECT m_rcButton;
   RECT m_rcLinks[8];
   int  m_nLinks;
   int  m_cxWidth;
   UINT m_uButtonState;
};

class UILIB_API DropDownUI : public ContainerUI, public IListOwnerUI
{
public:
   DropDownUI();

   virtual const char* GetClass() const;
   virtual void* GetInterface(const char* name);
   
   void Init();
   virtual UINT GetControlFlags() const;

   StdString GetText() const;

   SIZE GetDropDownSize() const;
   void SetDropDownSize(SIZE szDropBox);

   int GetCurSel() const;  
   bool SelectItem(int idx);

   bool Add(ControlUI* ctrl);
   bool Remove(ControlUI* ctrl);
   void RemoveAll();

   virtual bool Activate();

   virtual void SetPos(RECT rc);
   virtual void Event(TEventUI& event);
   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   int  m_curSel;
   int  m_cxWidth;
   SIZE m_szDropBox;
   RECT m_rcButton;
   UINT m_uButtonState;
};


#endif // !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

