#include "stdafx.h"

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include <wchar.h>
#include <string>
#include "AddInNative.h"
#include "AsteriskARI.h"

static const wchar_t *g_PropNames[] = {
	L"Enabled",
	L"Version"
};
static const wchar_t *g_MethodNames[] = {
	L"Connect",
	L"Dial"
};

static const wchar_t g_kClassNames[] = L"Obeliks"; 

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

long GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative();
        return (long)*pInterface;
    }
    return 0;
}
long DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
const WCHAR_T* GetClassNames()
{
    static WCHAR_T* names = 0;
    if (!names)
        ::convToShortWchar(&names, g_kClassNames);
    return names;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	return eAppCapabilitiesLast;
}

//---------------------------------------------------------------------------//
//CAddInNative
CAddInNative::CAddInNative()
{
	m_iMemory = 0;
	m_iConnect = 0;
	m_Asterisk = NULL;
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
	if (m_Asterisk)
		delete m_Asterisk;
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != NULL;
}

//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsLanguageExt)
{ 
	const wchar_t *wsExtension = L"Obeliks";
	int iActualSize = ::wcslen(wsExtension) + 1;

	if (m_iMemory) {
		if (m_iMemory->AllocMemory((void**)wsLanguageExt, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(wsLanguageExt, wsExtension, iActualSize);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
	long plPropNum = -1;
	wchar_t* propName = 0;

	::convFromShortWchar(&propName, wsPropName);
	plPropNum = findName(g_PropNames, propName, eLastProp);
	delete[] propName;
	return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
	if (lPropNum >= eLastProp || lPropNum < 0)
		return NULL;
	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsPropName = NULL;
	int iActualSize = 0;

	wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName) {
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}
	return wsPropName;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
	switch (lPropNum)
	{
	case ePropIsEnabled:
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_BOOL(pvarPropVal) = m_Enabled;
		break;
	case ePropVersion:
		TV_VT(pvarPropVal) = VTYPE_PWSTR;
		if (!m_Asterisk) {
			break;
		}
		if (m_iMemory) {
			int iActualSize = m_Asterisk->getAsteriskVersion().length();
			if (m_iMemory->AllocMemory((void**)&pvarPropVal->pwstrVal, iActualSize * sizeof(WCHAR_T)))
				::convToShortWchar(&pvarPropVal->pwstrVal, m_Asterisk->getAsteriskVersion().c_str(), iActualSize);
			pvarPropVal->wstrLen = iActualSize;
		}
		break;
	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
	switch (lPropNum) 
	{
	case ePropIsEnabled:
		if (TV_VT(varPropVal) != VTYPE_BOOL)
			return false;
		m_Enabled = TV_BOOL(varPropVal);
		break;
	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
	switch (lPropNum) {
	case ePropIsEnabled:
	case ePropVersion:
		return true;
	default:
		return false;
	}
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
	return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
	long plMethodNum = -1;
	wchar_t* name = 0;
	::convFromShortWchar(&name, wsMethodName);
	plMethodNum = findName(g_MethodNames, name, eLastMethod);

	delete[] name;
	return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
	if (lMethodNum >= eLastMethod)
		return NULL;
	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsMethodName = NULL;
	int iActualSize = 0;

	wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
	iActualSize = wcslen(wsCurrentName) + 1;
	if (m_iMemory && wsCurrentName)
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
	switch (lMethodNum) {
	case eMethConnect:
		return 3;
	case eMethDial:
		return 3;
	default:
		return 0;
	}
	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
	switch (lMethodNum) {
	case eMethConnect:
	case eMethDial:
		break;
	default:
		return false;
	}
	return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
	switch (lMethodNum)
	{
	case eMethConnect:
	case eMethDial:
		return true;
	default:
		return false;
	}
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
	switch (lMethodNum)
	{
	case eMethConnect:
		if (lSizeArray != 3 || !paParams) //no params, abort
			return false;
		if (TV_VT(&paParams[0]) != VTYPE_PWSTR || TV_VT(&paParams[1]) != VTYPE_PWSTR || TV_VT(&paParams[2]) != VTYPE_PWSTR) //check param types
			return false;
		m_Asterisk = new AsteriskARI(paParams[0], paParams[1], paParams[2]);
		TV_VT(pvarRetValue) = VTYPE_BOOL;
		if (m_Asterisk)
			pvarRetValue->bVal = true;
		else
			pvarRetValue->bVal = false;
		return true;
		break;
	case eMethDial:
		if (lSizeArray != 3 || !paParams) //no params, abort
			return false;
		if (TV_VT(&paParams[0]) != VTYPE_PWSTR || TV_VT(&paParams[1]) != VTYPE_PWSTR || TV_VT(&paParams[2]) != VTYPE_PWSTR) //check param types
			return false;
		TV_VT(pvarRetValue) = VTYPE_BOOL;
		if (m_Asterisk) {
			pvarRetValue->bVal = m_Asterisk->Dial(paParams[0], paParams[1], paParams[2]);
		}
		else
			pvarRetValue->bVal = false;
		return true;
		break;

	default:
		break;
	}
	return false;
}
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
    _wsetlocale(LC_ALL, loc);
#else
    int size = 0;
    char *mbstr = 0;
    wchar_t *tmpLoc = 0;
    convFromShortWchar(&tmpLoc, loc);
    size = wcstombs(0, tmpLoc, 0)+1;
    mbstr = new char[size];

    if (!mbstr)
    {
        delete[] tmpLoc;
        return;
    }

    memset(mbstr, 0, size);
    size = wcstombs(mbstr, tmpLoc, wcslen(tmpLoc));
    setlocale(LC_ALL, mbstr);
    delete[] tmpLoc;
    delete[] mbstr;
#endif
}
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}

void CAddInNative::addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code) {
	if (m_iConnect) {
		WCHAR_T *err = 0;
		WCHAR_T *descr = 0;

		::convToShortWchar(&err, source);
		::convToShortWchar(&descr, descriptor);

		m_iConnect->AddError(wcode, err, descr, code);
		delete[] err;
		delete[] descr;
	}
}

long CAddInNative::findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const {
	long ret = -1;
	for (uint32_t i = 0; i < size; i++) {
		if (!wcscmp(names[i], name)) {
			ret = i;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
