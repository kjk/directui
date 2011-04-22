
#include "StdAfx.h"

#ifndef TRACE
   #define TRACE
#endif


MarkupNode::MarkupNode() : m_owner(NULL)
{
}

MarkupNode::MarkupNode(MarkupParser* owner, int pos) : m_owner(owner), m_pos(pos), m_nAttributes(0)
{
}

MarkupNode MarkupNode::GetSibling()
{
   if (m_owner == NULL)  return MarkupNode();
   ULONG pos = m_owner->m_elements[m_pos].iNext;
   if (pos == 0)  return MarkupNode();
   return MarkupNode(m_owner, pos);
}

bool MarkupNode::HasSiblings() const
{
   if (m_owner == NULL)  return false;
   ULONG pos = m_owner->m_elements[m_pos].iNext;
   return pos > 0;
}

MarkupNode MarkupNode::GetChild()
{
   if (m_owner == NULL)  return MarkupNode();
   ULONG pos = m_owner->m_elements[m_pos].iChild;
   if (pos == 0)  return MarkupNode();
   return MarkupNode(m_owner, pos);
}

MarkupNode MarkupNode::GetChild(const char* name)
{
   if (m_owner == NULL)  return MarkupNode();
   ULONG pos = m_owner->m_elements[m_pos].iChild;
   while (pos != 0)  {
      if (str::Eq(m_owner->m_xml + m_owner->m_elements[pos].iStart, name) == 0)  {
         return MarkupNode(m_owner, pos);
      }
      pos = m_owner->m_elements[pos].iNext;
   }
   return MarkupNode();
}

bool MarkupNode::HasChildren() const
{
   if (m_owner == NULL)  return false;
   return m_owner->m_elements[m_pos].iChild != 0;
}

MarkupNode MarkupNode::GetParent()
{
   if (m_owner == NULL)  return MarkupNode();
   ULONG pos = m_owner->m_elements[m_pos].iParent;
   if (pos == 0)  return MarkupNode();
   return MarkupNode(m_owner, pos);
}

bool MarkupNode::IsValid() const
{
   return m_owner != NULL;
}

const char* MarkupNode::GetName() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_xml + m_owner->m_elements[m_pos].iStart;
}

const char* MarkupNode::GetValue() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_xml + m_owner->m_elements[m_pos].iData;
}

const char* MarkupNode::GetAttributeName(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return "";
   return m_owner->m_xml + m_aAttributes[idx].iName;
}

const char* MarkupNode::GetAttributeValue(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return "";
   return m_owner->m_xml + m_aAttributes[idx].iValue;
}

const char* MarkupNode::GetAttributeValue(const char* name)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (str::Eq(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)
         return m_owner->m_xml + m_aAttributes[i].iValue;
   }
   return "";
}

bool MarkupNode::GetAttributeValue(int idx, char* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return false;
   strncpy(value, m_owner->m_xml + m_aAttributes[idx].iValue, cchMax);
   return true;
}

bool MarkupNode::GetAttributeValue(const char* name, char* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (str::Eq(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)  {
         strncpy(value, m_owner->m_xml + m_aAttributes[i].iValue, cchMax);
         return true;
      }
   }
   return false;
}

int MarkupNode::GetAttributeCount()
{
   if (m_owner == NULL)  return 0;
   if (m_nAttributes == 0)  _MapAttributes();
   return m_nAttributes;
}

bool MarkupNode::HasAttributes()
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   return m_nAttributes > 0;
}

bool MarkupNode::HasAttribute(const char* name)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (str::Eq(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)
         return true;
   }
   return false;
}

void MarkupNode::_MapAttributes()
{
   m_nAttributes = 0;
   const char* pstr = m_owner->m_xml + m_owner->m_elements[m_pos].iStart;
   const char* pstrEnd = m_owner->m_xml + m_owner->m_elements[m_pos].iData;
   pstr += str::Len(pstr) + 1;
   while (pstr < pstrEnd)  {
      m_owner->_SkipWhitespace(pstr);
      m_aAttributes[m_nAttributes].iName = pstr - m_owner->m_xml;
      pstr += str::Len(pstr) + 1;
      if (*pstr++ != '\"' && *pstr++ != '\'')  return;
      m_aAttributes[m_nAttributes++].iValue = pstr - m_owner->m_xml;
      if (m_nAttributes >= MAX_XML_ATTRIBUTES)  return;
      pstr += str::Len(pstr) + 1;
   }
}

MarkupParser::MarkupParser(const char* xml)
{
   m_xml = NULL;
   m_elements = NULL;
   m_nElements = 0;
   m_bPreserveWhitespace = false;
   if (xml != NULL)  Load(xml);
}

MarkupParser::~MarkupParser()
{
   Release();
}

bool MarkupParser::IsValid() const
{
   return m_elements != NULL;
}

void MarkupParser::SetPreserveWhitespace(bool bPreserve)
{
   m_bPreserveWhitespace = bPreserve;
}

bool MarkupParser::Load(const char* xml)
{
   Release();
   SIZE_T cbLen = str::Len(xml) + 1;
   m_xml = static_cast<char*>(malloc(cbLen));
   ::CopyMemory(m_xml, xml, cbLen);
   bool bRes = _Parse();
   if (!bRes)  Release();
   return bRes;
}

bool MarkupParser::LoadFromFile(const TCHAR* fileName)
{
   Release();
   HANDLE hFile = ::CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)  
      return _Failed("Error opening file");
   DWORD dwSize = ::GetFileSize(hFile, NULL);
   if (dwSize == 0)
      return _Failed("File is empty");
   DWORD dwRead = 0;
   m_xml = static_cast<char*>(malloc(dwSize + 1));
   ::ReadFile(hFile, m_xml, dwSize, &dwRead, NULL);
   ::CloseHandle(hFile);
   m_xml[dwSize] = '\0';

   if (dwRead != dwSize)  {
      Release();
      return _Failed("Could not read file");
   }
   bool ok = _Parse();
   if (!ok)  Release();
   return ok;
}

void MarkupParser::Release()
{
   free(m_xml);
   free(m_elements);
   m_xml = NULL;
   m_elements = NULL;
   m_nElements;
}

void MarkupParser::GetLastErrorMessage(char* msg, SIZE_T cchMax) const
{
   strncpy(msg, m_errorMsg, cchMax);
}

void MarkupParser::GetLastErrorLocation(char* src, SIZE_T cchMax) const
{
   strncpy(src, m_errorXml, cchMax);
}

MarkupNode MarkupParser::GetRoot()
{
   if (m_nElements == 0)  return MarkupNode();
   return MarkupNode(this, 1);
}

bool MarkupParser::_Parse()
{
   _ReserveElement(); // Reserve index 0 for errors
   ::ZeroMemory(m_errorMsg, sizeof(m_errorMsg));
   ::ZeroMemory(m_errorXml, sizeof(m_errorXml));
   char* xml = m_xml;
   return _Parse(xml, 0);
}

bool MarkupParser::_Parse(char*& txt, ULONG iParent)
{
   ULONG iPrevious = 0;
   for (; ;)  
   {
      if (*txt == '\0' && iParent <= 1)  return true;
      if (*txt != '<')  return _Failed("Expected start tag", txt);
      if (txt[1] == '/')  return true;
      *txt++ = '\0';
      // Skip comment or processing directive
      if (*txt == '!' || *txt == '?')  {
         char chEnd = *txt == '!' ? '-' : '?';
         while (*txt != '\0' && !(*txt == chEnd && *(txt + 1) == '>')) {
            txt = ::CharNextA(txt);
         }
         if (*txt != '\0')  txt += 2;
         _SkipWhitespace(txt);
         continue;
      }
      // Fill out element structure
      XMLELEMENT* pEl = _ReserveElement();
      ULONG pos = pEl - m_elements;
      pEl->iStart = txt - m_xml;
      pEl->iParent = iParent;
      pEl->iNext = pEl->iChild = 0;
      if (iPrevious != 0)  m_elements[iPrevious].iNext = pos;
      else if (iParent > 0)  m_elements[iParent].iChild = pos;
      iPrevious = pos;
      // Parse name
      const char* name = txt;
      _SkipIdentifier(txt);
      char* nameEnd = txt;
      if (*txt == '\0')  return _Failed("Error parsing element name", txt);
      // Parse attributes
      if (!_ParseAttributes(txt))  return false;
      _SkipWhitespace(txt);
      if (txt[0] == '/' && txt[1] == '>') 
      {
         pEl->iData = txt - m_xml;
         *txt = '\0';
         txt += 2;
      }
      else
      {
         if (*txt != '>')  return _Failed("Expected start-tag closing", txt);
         // Parse node data
         pEl->iData = ++txt - m_xml;
         char* pstrDest = txt;
         if (!_ParseData(txt, pstrDest, '<'))  return false;
         // Determine type of next element
         if (*txt == '\0' && iParent <= 1)  return true;
         if (*txt != '<')  return _Failed("Expected end-tag start", txt);
         if (txt[0] == '<' && txt[1] != '/')  
         {
            if (!_Parse(txt, pos))  return false;
         }
         if (txt[0] == '<' && txt[1] == '/')  
         {
            *pstrDest = '\0';
            *txt = '\0';
            txt += 2;
            SIZE_T cchName = nameEnd - name;
            if (!str::EqN(txt, name, cchName))
                return _Failed("Unmatched closing tag", txt);
            if (txt[cchName] != '>')
                return _Failed("Unmatched closing tag", txt);
            txt += cchName + 1;
         }
      }
      *nameEnd = '\0';
      _SkipWhitespace(txt);
   }
}

MarkupParser::XMLELEMENT* MarkupParser::_ReserveElement()
{
   if (m_nElements == 0)  m_nReservedElements = 0;
   if (m_nElements >= m_nReservedElements)  {
      m_nReservedElements += (m_nReservedElements / 2) + 500;
      m_elements = static_cast<XMLELEMENT*>(realloc(m_elements, m_nReservedElements * sizeof(XMLELEMENT)));
   }
   return &m_elements[m_nElements++];
}

void MarkupParser::_SkipWhitespace(const char*& s) const
{
   while (*s != '\0' && *s <= ' ')
      s++;
}

void MarkupParser::_SkipWhitespace(char*& s) const
{
   while (*s != '\0' && *s <= ' ')
      s++;
}

void MarkupParser::_SkipIdentifier(const char*& s) const
{
   while (*s != '\0' && (*s == '_' || *s == ':' || isalnum(*s)))
      s++;
}

void MarkupParser::_SkipIdentifier(char*& s) const
{
   while (*s != '\0' && (*s == '_' || *s == ':' || isalnum(*s)))
      s++;
}

bool MarkupParser::_ParseAttributes(char*& txt)
{   
   if (*txt == '>')  return true;
   *txt++ = '\0';
   _SkipWhitespace(txt);
   while (*txt != '\0' && *txt != '>' && *txt != '/')  {
      _SkipIdentifier(txt);
      if (*txt != '=')
         return _Failed("Error while parsing attributes", txt);
      *txt++ = '\0';
      char chQuote = *txt++;
      if (chQuote != '\"' && chQuote != '\'')
         return _Failed("Expected attribute value", txt);
      char* pstrDest = txt;
      if (!_ParseData(txt, pstrDest, chQuote))  return false;
      if (*txt == '\0')
         return _Failed("Error while parsing attribute string", txt);
      *pstrDest = '\0';
      *txt++ = '\0';
      _SkipWhitespace(txt);
   }
   return true;
}

bool MarkupParser::_ParseData(char*& txt, char*& pstrDest, char cEnd)
{
   while (*txt != '\0' && *txt != cEnd)  {
      if (*txt == '&')  {
         _ParseMetaChar(++txt, pstrDest);
      }
      if (*txt == ' ')  {
         *pstrDest++ = *txt++;
         if (!m_bPreserveWhitespace)  _SkipWhitespace(txt);
      }
      else {
         *pstrDest++ = *txt++;
#ifdef _MBCS
         if (::IsDBCSLeadByte(*(txt - 1)))  *pstrDest++ = *txt++;
#endif // _MBCS
      }
   }
   // Make sure that MapAttributes() works correctly when it parses
   // over a value that has been transformed.
   char* pstrFill = pstrDest + 1;
   while (pstrFill < txt)  *pstrFill++ = ' ';
   return true;
}

void MarkupParser::_ParseMetaChar(char*& txt, char*& pstrDest)
{
   if (txt[0] == 'a' && txt[1] == 'm' && txt[2] == 'p' && txt[3] == ';')  {
      *pstrDest++ = '&';
      txt += 4;
   }
   else if (txt[0] == 'l' && txt[1] == 't' && txt[2] == ';')  {
      *pstrDest++ = '<';
      txt += 3;
   }
   else if (txt[0] == 'g' && txt[1] == 't' && txt[2] == ';')  {
      *pstrDest++ = '>';
      txt += 3;
   }
   else if (txt[0] == 'q' && txt[1] == 'u' && txt[2] == 'o' && txt[3] == 't' && txt[4] == ';')  {
      *pstrDest++ = '\"';
      txt += 5;
   }
   else if (txt[0] == 'a' && txt[1] == 'p' && txt[2] == 'o' && txt[3] == 's' && txt[4] == ';')  {
      *pstrDest++ = '\'';
      txt += 5;
   }
   else {
      *pstrDest++ = '&';
   }
}

// remember last error
bool MarkupParser::_Failed(const char* pstrError, const char* location)
{
    if (!location) location = "";
   TRACE("XML Error: %s", pstrError);
   TRACE(location);
   strncpy(m_errorMsg, pstrError, dimof(m_errorMsg) - 1);
   strncpy(m_errorXml, location, dimof(m_errorXml) - 1);
   return false; // Always return 'false'
}

