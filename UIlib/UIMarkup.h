#if !defined(AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_)
#define AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_

#pragma once

class CMarkup;
class CMarkupNode;


class UILIB_API CMarkup
{
friend CMarkupNode;
public:
   CMarkup(const TCHAR* pstrXML = NULL);
   ~CMarkup();

   bool Load(const TCHAR* pstrXML);
   bool LoadFromFile(const TCHAR* pstrFilename);
   void Release();
   bool IsValid() const;

   void SetPreserveWhitespace(bool bPreserve = true);
   void GetLastErrorMessage(TCHAR* pstrMessage, SIZE_T cchMax) const;
   void GetLastErrorLocation(TCHAR* pstrSource, SIZE_T cchMax) const;

   CMarkupNode GetRoot();

private:
   typedef struct tagXMLELEMENT
   {
      ULONG iStart;
      ULONG iChild;
      ULONG iNext;
      ULONG iParent;
      ULONG iData;
   } XMLELEMENT;
   
   TCHAR* m_pstrXML;
   XMLELEMENT* m_pElements;
   ULONG m_nElements;
   ULONG m_nReservedElements;
   TCHAR m_szErrorMsg[100];
   TCHAR m_szErrorXML[50];
   bool m_bPreserveWhitespace;

private:
   bool _Parse();
   bool _Parse(TCHAR*& txt, ULONG iParent);
   XMLELEMENT* _ReserveElement();
   inline void _SkipWhitespace(TCHAR*& pstr) const;
   inline void _SkipWhitespace(const TCHAR*& pstr) const;
   inline void _SkipIdentifier(TCHAR*& pstr) const;
   inline void _SkipIdentifier(const TCHAR*& pstr) const;
   bool _ParseData(TCHAR*& txt, TCHAR*& pstrData, char cEnd);
   void _ParseMetaChar(TCHAR*& txt, TCHAR*& pstrDest);
   bool _ParseAttributes(TCHAR*& txt);
   bool _Failed(const TCHAR* pstrError, const TCHAR* pstrLocation = NULL);
};


class UILIB_API CMarkupNode
{
friend CMarkup;
private:
   CMarkupNode();
   CMarkupNode(CMarkup* owner, int pos);

public:
   bool IsValid() const;

   CMarkupNode GetParent();
   CMarkupNode GetSibling();
   CMarkupNode GetChild();
   CMarkupNode GetChild(const TCHAR* name);

   bool HasSiblings() const;
   bool HasChildren() const;
   const TCHAR* GetName() const;
   const TCHAR* GetValue() const;

   bool HasAttributes();
   bool HasAttribute(const TCHAR* name);
   int GetAttributeCount();
   const TCHAR* GetAttributeName(int idx);
   const TCHAR* GetAttributeValue(int idx);
   const TCHAR* GetAttributeValue(const TCHAR* name);
   bool GetAttributeValue(int idx, TCHAR* value, SIZE_T cchMax);
   bool GetAttributeValue(const TCHAR* name, TCHAR* value, SIZE_T cchMax);

private:
   void _MapAttributes();

   enum { MAX_XML_ATTRIBUTES = 20 };

   typedef struct
   {
      ULONG iName;
      ULONG iValue;
   } XMLATTRIBUTE;

   int m_pos;
   int m_nAttributes;
   XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
   CMarkup* m_owner;
};


#endif // !defined(AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_)

