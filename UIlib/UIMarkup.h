#if !defined(AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_)
#define AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_

#pragma once

class MarkupParser;
class MarkupNode;


class UILIB_API MarkupParser
{
friend MarkupNode;
public:
   MarkupParser(const TCHAR* xml = NULL);
   ~MarkupParser();

   bool Load(const TCHAR* xml);
   bool LoadFromFile(const TCHAR* fileName);
   void Release();
   bool IsValid() const;

   void SetPreserveWhitespace(bool bPreserve = true);
   void GetLastErrorMessage(TCHAR* msg, SIZE_T cchMax) const;
   void GetLastErrorLocation(TCHAR* pstrSource, SIZE_T cchMax) const;

   MarkupNode GetRoot();

private:
   typedef struct tagXMLELEMENT
   {
      ULONG iStart;
      ULONG iChild;
      ULONG iNext;
      ULONG iParent;
      ULONG iData;
   } XMLELEMENT;
   
   TCHAR* m_xml;
   XMLELEMENT* m_elements;
   ULONG m_nElements;
   ULONG m_nReservedElements;
   TCHAR m_errorMsg[100];
   TCHAR m_errorXml[50];
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


class UILIB_API MarkupNode
{
friend MarkupParser;
private:
   MarkupNode();
   MarkupNode(MarkupParser* owner, int pos);

public:
   bool IsValid() const;

   MarkupNode GetParent();
   MarkupNode GetSibling();
   MarkupNode GetChild();
   MarkupNode GetChild(const TCHAR* name);

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
   MarkupParser* m_owner;
};


#endif // !defined(AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_)

