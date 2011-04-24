#if !defined(AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)
#define AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_

class UILIB_API ButtonUI : public ControlUI
{
public:
   ButtonUI();

   virtual const char* GetClass() const;
   virtual UINT GetControlFlags() const;

   virtual bool Activate();

   void SetText(const TCHAR* txt);

   void SetWidth(int cxWidth);
   void SetPadding(int cx, int cy);

   virtual void Event(TEventUI& event);

   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);
   virtual void SetAttribute(const char* name, const char* value);

protected:
   int  m_cxWidth;
   SIZE m_szPadding;
   UINT m_uTextStyle;
   UINT m_uButtonState;
};


class UILIB_API OptionUI : public ControlUI
{
public:
   OptionUI();

   virtual const char* GetClass() const;
   virtual UINT GetControlFlags() const;

   virtual bool Activate();

   bool IsChecked() const;
   void SetCheck(bool bSelected);
   void SetWidth(int cxWidth);
   
   virtual void Event(TEventUI& event);

   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);
   virtual void SetAttribute(const char* name, const char* value);

protected:
   bool m_bSelected;
   UINT m_uStyle;
   UINT m_uButtonState;
   int  m_cxWidth;
};


#endif // !defined(AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)

