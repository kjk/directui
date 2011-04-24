#if !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)
#define AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_

class SingleLineEditWnd;

class UILIB_API SingleLineEditUI : public ControlUI
{
friend SingleLineEditWnd;
public:
   SingleLineEditUI();

   virtual const char* GetClass() const;
   virtual UINT GetControlFlags() const;

   virtual void SetText(const char* txt);

   void SetEditStyle(UINT uStyle);
   void SetReadOnly(bool bReadOnly);
   bool IsReadOnly() const;

   virtual void Event(TEventUI& event);

   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   SingleLineEditWnd* m_win;

   bool m_bReadOnly;
   UINT m_uEditStyle;
};

class MultiLineEditWnd;

class UILIB_API MultiLineEditUI : public ControlUI
{
friend MultiLineEditWnd;
public:
   MultiLineEditUI();
   ~MultiLineEditUI();

   virtual const char* GetClass() const;
   virtual UINT GetControlFlags() const;

   virtual void Init();
   
   virtual const char *GetText() const;
   virtual void SetText(const char* txt);

   virtual void SetEnabled(bool enabled);
   virtual void SetVisible(bool visible);

   void SetReadOnly(bool bReadOnly);
   //TODO: this was declared but not implemented
   //virtual void SetFocus(bool bReadOnly);
   bool IsReadOnly() const;

   virtual void Event(TEventUI& event);

   virtual SIZE EstimateSize(SIZE szAvailable);
   virtual void SetPos(RECT rc);
   virtual void SetPos(int left, int top, int right, int bottom);
   virtual void DoPaint(HDC hDC, const RECT& rcPaint);

protected:
   MultiLineEditWnd* m_win;
};


#endif // !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)

