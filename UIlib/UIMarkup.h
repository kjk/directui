#if !defined(AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_)
#define AFX_MARKUP_H__20050505_6983_3DDF_F867_0080AD509054__INCLUDED_

class MarkupParser;
class MarkupNode;

class UILIB_API MarkupParser
{
friend MarkupNode;
public:
   MarkupParser(const char* xml = NULL);
   ~MarkupParser();

   bool Load(const char* xml);
   bool LoadFromFile(const char* fileName);
   void Release();
   bool IsValid() const;

   void SetPreserveWhitespace(bool bPreserve = true);
   void GetLastErrorMessage(char* msg, size_t cchMax) const;
   void GetLastErrorLocation(char* pstrSource, size_t cchMax) const;

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
   
   char*        m_xml;
   XMLELEMENT*  m_elements;
   ULONG        m_nElements;
   ULONG        m_nReservedElements;
   char         m_errorMsg[100];
   char         m_errorXml[50];
   bool         m_bPreserveWhitespace;

private:
   bool _Parse();
   bool _Parse(char*& txt, ULONG iParent);
   XMLELEMENT* _ReserveElement();
   inline void _SkipWhitespace(char*& s) const;
   inline void _SkipIdentifier(char*& s) const;
   bool _ParseData(char*& txt, char*& pstrData, char cEnd);
   void _ParseMetaChar(char*& txt, char*& pstrDest);
   bool _ParseAttributes(char*& txt);
   bool _Failed(const char* pstrError, const char* pstrLocation = NULL);
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
   MarkupNode GetChild(const char* name);

   bool HasSiblings() const;
   bool HasChildren() const;
   const char* GetName() const;
   const char* GetValue() const;

   bool HasAttributes();
   bool HasAttribute(const char* name);
   int GetAttributeCount();
   const char* GetAttributeName(int idx);
   const char* GetAttributeValue(int idx);
   const char* GetAttributeValue(const char* name);

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

