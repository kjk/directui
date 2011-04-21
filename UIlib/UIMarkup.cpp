
#include "StdAfx.h"

#ifndef TRACE
   #define TRACE
#endif


CMarkupNode::CMarkupNode() : m_owner(NULL)
{
}

CMarkupNode::CMarkupNode(CMarkup* owner, int pos) : m_owner(owner), m_pos(pos), m_nAttributes(0)
{
}

CMarkupNode CMarkupNode::GetSibling()
{
   if (m_owner == NULL)  return CMarkupNode();
   ULONG pos = m_owner->m_pElements[m_pos].iNext;
   if (pos == 0)  return CMarkupNode();
   return CMarkupNode(m_owner, pos);
}

bool CMarkupNode::HasSiblings() const
{
   if (m_owner == NULL)  return false;
   ULONG pos = m_owner->m_pElements[m_pos].iNext;
   return pos > 0;
}

CMarkupNode CMarkupNode::GetChild()
{
   if (m_owner == NULL)  return CMarkupNode();
   ULONG pos = m_owner->m_pElements[m_pos].iChild;
   if (pos == 0)  return CMarkupNode();
   return CMarkupNode(m_owner, pos);
}

CMarkupNode CMarkupNode::GetChild(const TCHAR* name)
{
   if (m_owner == NULL)  return CMarkupNode();
   ULONG pos = m_owner->m_pElements[m_pos].iChild;
   while (pos != 0)  {
      if (_tcscmp(m_owner->m_pstrXML + m_owner->m_pElements[pos].iStart, name) == 0)  {
         return CMarkupNode(m_owner, pos);
      }
      pos = m_owner->m_pElements[pos].iNext;
   }
   return CMarkupNode();
}

bool CMarkupNode::HasChildren() const
{
   if (m_owner == NULL)  return false;
   return m_owner->m_pElements[m_pos].iChild != 0;
}

CMarkupNode CMarkupNode::GetParent()
{
   if (m_owner == NULL)  return CMarkupNode();
   ULONG pos = m_owner->m_pElements[m_pos].iParent;
   if (pos == 0)  return CMarkupNode();
   return CMarkupNode(m_owner, pos);
}

bool CMarkupNode::IsValid() const
{
   return m_owner != NULL;
}

const TCHAR* CMarkupNode::GetName() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_pstrXML + m_owner->m_pElements[m_pos].iStart;
}

const TCHAR* CMarkupNode::GetValue() const
{
   if (m_owner == NULL)  return NULL;
   return m_owner->m_pstrXML + m_owner->m_pElements[m_pos].iData;
}

const TCHAR* CMarkupNode::GetAttributeName(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return _T("");
   return m_owner->m_pstrXML + m_aAttributes[idx].iName;
}

const TCHAR* CMarkupNode::GetAttributeValue(int idx)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return _T("");
   return m_owner->m_pstrXML + m_aAttributes[idx].iValue;
}

const TCHAR* CMarkupNode::GetAttributeValue(const TCHAR* name)
{
   if (m_owner == NULL)  return NULL;
   if (m_nAttributes == 0)  _MapAttributes();
   for( int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_pstrXML + m_aAttributes[i].iName, name) == 0)  return m_owner->m_pstrXML + m_aAttributes[i].iValue;
   }
   return _T("");
}

bool CMarkupNode::GetAttributeValue(int idx, TCHAR* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   if (idx < 0 || idx >= m_nAttributes)  return false;
   _tcsncpy(value, m_owner->m_pstrXML + m_aAttributes[idx].iValue, cchMax);
   return true;
}

bool CMarkupNode::GetAttributeValue(const TCHAR* name, TCHAR* value, SIZE_T cchMax)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for( int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_pstrXML + m_aAttributes[i].iName, name) == 0)  {
         _tcsncpy(value, m_owner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
         return true;
      }
   }
   return false;
}

int CMarkupNode::GetAttributeCount()
{
   if (m_owner == NULL)  return 0;
   if (m_nAttributes == 0)  _MapAttributes();
   return m_nAttributes;
}

bool CMarkupNode::HasAttributes()
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   return m_nAttributes > 0;
}

bool CMarkupNode::HasAttribute(const TCHAR* name)
{
   if (m_owner == NULL)  return false;
   if (m_nAttributes == 0)  _MapAttributes();
   for( int i = 0; i < m_nAttributes; i++)  {
      if (_tcscmp(m_owner->m_pstrXML + m_aAttributes[i].iName, name) == 0)  return true;
   }
   return false;
}

void CMarkupNode::_MapAttributes()
{
   m_nAttributes = 0;
   const TCHAR* pstr = m_owner->m_pstrXML + m_owner->m_pElements[m_pos].iStart;
   const TCHAR* pstrEnd = m_owner->m_pstrXML + m_owner->m_pElements[m_pos].iData;
   pstr += _tcslen(pstr) + 1;
   while (pstr < pstrEnd)  {
      m_owner->_SkipWhitespace(pstr);
      m_aAttributes[m_nAttributes].iName = pstr - m_owner->m_pstrXML;
      pstr += _tcslen(pstr) + 1;
      if (*pstr++ != '\"' && *pstr++ != '\'')  return;
      m_aAttributes[m_nAttributes++].iValue = pstr - m_owner->m_pstrXML;
      if (m_nAttributes >= MAX_XML_ATTRIBUTES)  return;
      pstr += _tcslen(pstr) + 1;
   }
}


CMarkup::CMarkup(const TCHAR* pstrXML)
{
   m_pstrXML = NULL;
   m_pElements = NULL;
   m_nElements = 0;
   m_bPreserveWhitespace = false;
   if (pstrXML != NULL)  Load(pstrXML);
}

CMarkup::~CMarkup()
{
   Release();
}

bool CMarkup::IsValid() const
{
   return m_pElements != NULL;
}

void CMarkup::SetPreserveWhitespace(bool bPreserve)
{
   m_bPreserveWhitespace = bPreserve;
}

bool CMarkup::Load(const TCHAR* pstrXML)
{
   Release();
   SIZE_T cbLen = (_tcslen(pstrXML) + 1) * sizeof(TCHAR);
   m_pstrXML = static_cast<TCHAR*>(malloc(cbLen));
   ::CopyMemory(m_pstrXML, pstrXML, cbLen);
   bool bRes = _Parse();
   if (!bRes)  Release();
   return bRes;
}

bool CMarkup::LoadFromFile(const TCHAR* pstrFilename)
{
   Release();
   HANDLE hFile = ::CreateFile(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)  return _Failed(_T("Error opening file"));
   DWORD dwSize = ::GetFileSize(hFile, NULL);
   if (dwSize == 0)  return _Failed(_T("File is empty"));
   DWORD dwRead = 0;
#ifdef _UNICODE
   // BUG: We don't support UNICODE file loading yet.
   ::CloseHandle(hFile);
   return false;
#else
   m_pstrXML = static_cast<TCHAR*>(malloc(dwSize + 1));
   ::ReadFile(hFile, m_pstrXML, dwSize, &dwRead, NULL);
   ::CloseHandle(hFile);
   m_pstrXML[dwSize] = '\0';
#endif // _UNICODE
   if (dwRead != dwSize)  {
      Release();
      return _Failed(_T("Could not read file"));
   }
   bool bRes = _Parse();
   if (!bRes)  Release();
   return bRes;
}

void CMarkup::Release()
{
   if (m_pstrXML != NULL)  free(m_pstrXML);
   if (m_pElements != NULL)  free(m_pElements);
   m_pstrXML = NULL;
   m_pElements = NULL;
   m_nElements;
}

void CMarkup::GetLastErrorMessage(TCHAR* pstrMessage, SIZE_T cchMax) const
{
   _tcsncpy(pstrMessage, m_szErrorMsg, cchMax);
}

void CMarkup::GetLastErrorLocation(TCHAR* pstrSource, SIZE_T cchMax) const
{
   _tcsncpy(pstrSource, m_szErrorXML, cchMax);
}

CMarkupNode CMarkup::GetRoot()
{
   if (m_nElements == 0)  return CMarkupNode();
   return CMarkupNode(this, 1);
}

bool CMarkup::_Parse()
{
   _ReserveElement(); // Reserve index 0 for errors
   ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
   ::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
   TCHAR* pstrXML = m_pstrXML;
   return _Parse(pstrXML, 0);
}

bool CMarkup::_Parse(TCHAR*& txt, ULONG iParent)
{
   ULONG iPrevious = 0;
   for( ; ;)  
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
      ULONG pos = pEl - m_pElements;
      pEl->iStart = txt - m_pstrXML;
      pEl->iParent = iParent;
      pEl->iNext = pEl->iChild = 0;
      if (iPrevious != 0)  m_pElements[iPrevious].iNext = pos;
      else if (iParent > 0)  m_pElements[iParent].iChild = pos;
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
         pEl->iData = txt - m_pstrXML;
         *txt = '\0';
         txt += 2;
      }
      else
      {
         if (*txt != '>')  return _Failed(_T("Expected start-tag closing"), txt);
         // Parse node data
         pEl->iData = ++txt - m_pstrXML;
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

CMarkup::XMLELEMENT* CMarkup::_ReserveElement()
{
   if (m_nElements == 0)  m_nReservedElements = 0;
   if (m_nElements >= m_nReservedElements)  {
      m_nReservedElements += (m_nReservedElements / 2) + 500;
      m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
   }
   return &m_pElements[m_nElements++];
}

void CMarkup::_SkipWhitespace(const TCHAR*& pstr) const
{
   while (*pstr != '\0' && *pstr <= ' ')  pstr++;
}

void CMarkup::_SkipWhitespace(TCHAR*& pstr) const
{
   while (*pstr != '\0' && *pstr <= ' ')  pstr++;
}

void CMarkup::_SkipIdentifier(const TCHAR*& pstr) const
{
   while (*pstr != '\0' && (*pstr == '_' || *pstr == ':' || _istalnum(*pstr)))  pstr++;
}

void CMarkup::_SkipIdentifier(TCHAR*& pstr) const
{
   while (*pstr != '\0' && (*pstr == '_' || *pstr == ':' || _istalnum(*pstr)))  pstr++;
}

bool CMarkup::_ParseAttributes(TCHAR*& txt)
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

bool CMarkup::_ParseData(TCHAR*& txt, TCHAR*& pstrDest, char cEnd)
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

void CMarkup::_ParseMetaChar(TCHAR*& txt, TCHAR*& pstrDest)
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

bool CMarkup::_Failed(const TCHAR* pstrError, const TCHAR* pstrLocation)
{
   // Register last error
   TRACE("XML Error: %s", pstrError);
   TRACE(pstrLocation);
   _tcsncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
   _tcsncpy(m_szErrorXML, pstrLocation != NULL ? pstrLocation : _T(""), lengthof(m_szErrorXML) - 1);
   return false; // Always return 'false'
}

