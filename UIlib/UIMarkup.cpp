
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

MarkupNode MarkupNode::GetChild(const TCHAR* name)
{
   if (m_owner == NULL)  return MarkupNode();
   ULONG pos = m_owner->m_elements[m_pos].iChild;
   while (pos != 0)  {
      if (_tcscmp(m_owner->m_xml + m_owner->m_elements[pos].iStart, name) == 0)  {
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

const TCHAR* MarkupNode::GetName() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_xml + m_owner->m_elements[m_pos].iStart;
}

const TCHAR* MarkupNode::GetValue() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_xml + m_owner->m_elements[m_pos].iData;
}

const TCHAR* MarkupNode::GetAttributeName(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return _T("");
   return m_owner->m_xml + m_aAttributes[idx].iName;
}

const TCHAR* MarkupNode::GetAttributeValue(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return _T("");
   return m_owner->m_xml + m_aAttributes[idx].iValue;
}

const TCHAR* MarkupNode::GetAttributeValue(const TCHAR* name)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)  return m_owner->m_xml + m_aAttributes[i].iValue;
   }
   return _T("");
}

bool MarkupNode::GetAttributeValue(int idx, TCHAR* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return false;
   _tcsncpy(value, m_owner->m_xml + m_aAttributes[idx].iValue, cchMax);
   return true;
}

bool MarkupNode::GetAttributeValue(const TCHAR* name, TCHAR* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)  {
         _tcsncpy(value, m_owner->m_xml + m_aAttributes[i].iValue, cchMax);
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

bool MarkupNode::HasAttribute(const TCHAR* name)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for (int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_xml + m_aAttributes[i].iName, name) == 0)  return true;
   }
   return false;
}

void MarkupNode::_MapAttributes()
{
   m_nAttributes = 0;
   const TCHAR* pstr = m_owner->m_xml + m_owner->m_elements[m_pos].iStart;
   const TCHAR* pstrEnd = m_owner->m_xml + m_owner->m_elements[m_pos].iData;
   pstr += _tcslen(pstr) + 1;
   while (pstr < pstrEnd)  {
      m_owner->_SkipWhitespace(pstr);
      m_aAttributes[m_nAttributes].iName = pstr - m_owner->m_xml;
      pstr += _tcslen(pstr) + 1;
      if (*pstr++ != '\"' && *pstr++ != '\'')  return;
      m_aAttributes[m_nAttributes++].iValue = pstr - m_owner->m_xml;
      if (m_nAttributes >= MAX_XML_ATTRIBUTES)  return;
      pstr += _tcslen(pstr) + 1;
   }
}


MarkupParser::MarkupParser(const TCHAR* xml)
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

bool MarkupParser::Load(const TCHAR* xml)
{
   Release();
   SIZE_T cbLen = (_tcslen(xml) + 1) * sizeof(TCHAR);
   m_xml = static_cast<TCHAR*>(malloc(cbLen));
   ::CopyMemory(m_xml, xml, cbLen);
   bool bRes = _Parse();
   if (!bRes)  Release();
   return bRes;
}

bool MarkupParser::LoadFromFile(const TCHAR* fileName)
{
   Release();
   HANDLE hFile = ::CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)  return _Failed(_T("Error opening file"));
   DWORD dwSize = ::GetFileSize(hFile, NULL);
   if (dwSize == 0)  return _Failed(_T("File is empty"));
   DWORD dwRead = 0;
#ifdef _UNICODE
   // BUG: We don't support UNICODE file loading yet.
   ::CloseHandle(hFile);
   return false;
#else
   m_xml = static_cast<TCHAR*>(malloc(dwSize + 1));
   ::ReadFile(hFile, m_xml, dwSize, &dwRead, NULL);
   ::CloseHandle(hFile);
   m_xml[dwSize] = '\0';
#endif // _UNICODE
   if (dwRead != dwSize)  {
      Release();
      return _Failed(_T("Could not read file"));
   }
   bool bRes = _Parse();
   if (!bRes)  Release();
   return bRes;
}

void MarkupParser::Release()
{
   free(m_xml);
   free(m_elements);
   m_xml = NULL;
   m_elements = NULL;
   m_nElements;
}

void MarkupParser::GetLastErrorMessage(TCHAR* msg, SIZE_T cchMax) const
{
   _tcsncpy(msg, m_errorMsg, cchMax);
}

void MarkupParser::GetLastErrorLocation(TCHAR* pstrSource, SIZE_T cchMax) const
{
   _tcsncpy(pstrSource, m_errorXml, cchMax);
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
   TCHAR* xml = m_xml;
   return _Parse(xml, 0);
}

bool MarkupParser::_Parse(TCHAR*& txt, ULONG iParent)
{
   ULONG iPrevious = 0;
   for (; ;)  
   {
      if (*txt == '\0' && iParent <= 1)  return true;
      if (*txt != '<')  return _Failed(_T("Expected start tag"), txt);
      if (txt[1] == '/')  return true;
      *txt++ = '\0';
      // Skip comment or processing directive
      if (*txt == '!' || *txt == '?')  {
         TCHAR chEnd = *txt == '!' ? '-' : '?';
         while (*txt != '\0' && !(*txt == chEnd && *(txt + 1) == '>'))  txt = ::CharNext(txt);
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
      const TCHAR* name = txt;
      _SkipIdentifier(txt);
      TCHAR* nameEnd = txt;
      if (*txt == '\0')  return _Failed(_T("Error parsing element name"), txt);
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
         if (*txt != '>')  return _Failed(_T("Expected start-tag closing"), txt);
         // Parse node data
         pEl->iData = ++txt - m_xml;
         TCHAR* pstrDest = txt;
         if (!_ParseData(txt, pstrDest, '<'))  return false;
         // Determine type of next element
         if (*txt == '\0' && iParent <= 1)  return true;
         if (*txt != '<')  return _Failed(_T("Expected end-tag start"), txt);
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
            if (_tcsncmp(txt, name, cchName) != 0)  return _Failed(_T("Unmatched closing tag"), txt);
            if (txt[cchName] != '>')  return _Failed(_T("Unmatched closing tag"), txt);
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

void MarkupParser::_SkipWhitespace(const TCHAR*& pstr) const
{
   while (*pstr != '\0' && *pstr <= ' ')  pstr++;
}

void MarkupParser::_SkipWhitespace(TCHAR*& pstr) const
{
   while (*pstr != '\0' && *pstr <= ' ')  pstr++;
}

void MarkupParser::_SkipIdentifier(const TCHAR*& pstr) const
{
   while (*pstr != '\0' && (*pstr == '_' || *pstr == ':' || _istalnum(*pstr)))  pstr++;
}

void MarkupParser::_SkipIdentifier(TCHAR*& pstr) const
{
   while (*pstr != '\0' && (*pstr == '_' || *pstr == ':' || _istalnum(*pstr)))  pstr++;
}

bool MarkupParser::_ParseAttributes(TCHAR*& txt)
{   
   if (*txt == '>')  return true;
   *txt++ = '\0';
   _SkipWhitespace(txt);
   while (*txt != '\0' && *txt != '>' && *txt != '/')  {
      _SkipIdentifier(txt);
      if (*txt != '=')  return _Failed(_T("Error while parsing attributes"), txt);
      *txt++ = '\0';
      TCHAR chQuote = *txt++;
      if (chQuote != '\"' && chQuote != '\'')  return _Failed(_T("Expected attribute value"), txt);
      TCHAR* pstrDest = txt;
      if (!_ParseData(txt, pstrDest, chQuote))  return false;
      if (*txt == '\0')  return _Failed(_T("Error while parsing attribute string"), txt);
      *pstrDest = '\0';
      *txt++ = '\0';
      _SkipWhitespace(txt);
   }
   return true;
}

bool MarkupParser::_ParseData(TCHAR*& txt, TCHAR*& pstrDest, TCHAR cEnd)
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
   TCHAR* pstrFill = pstrDest + 1;
   while (pstrFill < txt)  *pstrFill++ = ' ';
   return true;
}

void MarkupParser::_ParseMetaChar(TCHAR*& txt, TCHAR*& pstrDest)
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

bool MarkupParser::_Failed(const TCHAR* pstrError, const TCHAR* pstrLocation)
{
   // Register last error
   //TRACE("XML Error: %s", pstrError);
   //TRACE(pstrLocation);
   _tcsncpy(m_errorMsg, pstrError, (sizeof(m_errorMsg) / sizeof(m_errorMsg[0])) - 1);
   _tcsncpy(m_errorXml, pstrLocation != NULL ? pstrLocation : _T(""), dimof(m_errorXml) - 1);
   return false; // Always return 'false'
}

