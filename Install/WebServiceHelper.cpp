#include "StdAfx.h"
#include "WebServiceHelper.h"
#include <wincrypt.h>
#include "HttpClient.h"
#include "UrlEncode.h"

CWebServiceHelper::CWebServiceHelper(void)
{
	CoInitialize(NULL); //初始化com

	SetInited(FALSE);
	m_szServiceUrl = _T("http://localhost/xabprint/services");
	m_nServiceTimeout = 15 * 1000;	//15秒
	m_bIEProxy = FALSE;
	m_szIEProxyAddr.Empty();
	m_nIEProxyPort = 808;
	m_szWebserviceKey = _T("xab!@#$%^&&*()?");
}

CWebServiceHelper::~CWebServiceHelper(void)
{
	CoUninitialize();   //退出com
}

BOOL CWebServiceHelper::IsInited()
{
	long lVal = 0;
	InterlockedExchange(&lVal, m_lIsInited);
	return lVal;
}

void CWebServiceHelper::SetInited(BOOL bSucc)
{
	InterlockedExchange(&m_lIsInited, bSucc);
}

CString CWebServiceHelper::UrlEncode(LPCSTR strData)
{
#ifdef UNICODE
	CString wszRequest = CCommonFun::UTF8ToUnicode(strData);
	wstring wszRequestTemp(wszRequest.GetString());
	wszRequestTemp = ::UrlEncode(wszRequestTemp);
	CString szRequest(wszRequestTemp.c_str());
	return szRequest;
#else
	CStringW wszRequest = MultiByteToWideChar(strData);
	wstring wszRequestTemp(wszRequest.GetString());
	wszRequestTemp = ::UrlEncode(wszRequestTemp);
	CString szRequest = UnicodeToUTF8(wszRequestTemp.c_str());
	return szRequest;
#endif
}

BOOL CWebServiceHelper::ConnWebService()
{
	Cloud_Webservice_Try_Catch_Exception_Begin

	if (IsInited())
	{
		theLog.Write(_T("CWebServiceHelper::ConnWebService,1,IsInited True"));
		return TRUE;
	}

	do 
	{
		if (!TestWebService())
		{
			theLog.Write(_T("!!CWebServiceHelper::ConnWebService,4,TestWebService fail"));
			break;
		}

		SetInited(TRUE);

	} while (FALSE);

	return IsInited();
	Cloud_Webservice_Try_Catch_Exception_End
}

BOOL CWebServiceHelper::ReleaseWebService()
{
	if (IsInited())
	{
		SetInited(FALSE);
	}
	return TRUE;
}

void CWebServiceHelper::ConfigWebService(CString szWebserviceKey, CString szServiceUrl, int nServiceTimeout/* = 15000*/
											  , BOOL bIEProxy /*= FALSE*/, CString szIEProxyAddr /*= _T("")*/, int nIEProxyPort /*= 808*/)
{
	m_szWebserviceKey = szWebserviceKey;
	m_szServiceUrl = szServiceUrl;
	m_nServiceTimeout = nServiceTimeout;
	m_bIEProxy = bIEProxy;
	m_szIEProxyAddr = szIEProxyAddr;
	m_nIEProxyPort = nIEProxyPort;

	theLog.Write(_T("CWebServiceHelper::ConfigWebService,m_szServiceUrl=%s,m_nServiceTimeout=%d,")
		_T("m_bIEProxy=%d,m_szIEProxyAddr=%s,m_nIEProxyPort=%d"), m_szServiceUrl, m_nServiceTimeout, m_bIEProxy, m_szIEProxyAddr, m_nIEProxyPort);
}


CString CWebServiceHelper::CryptMD5(unsigned char* pSource, int nLen)
{
	CString outData = _T("");
	if (!pSource)
	{
		return outData;
	}

	//CSP指针
	HCRYPTPROV hProv = 0;
	//MD5 HASH
	HCRYPTHASH hHash = 0;

	BYTE bHash[32];
	DWORD dwHashLen = 16; // The MD5 algorithm always returns 16 bytes.
	DWORD cbContent = nLen;
	BYTE* pbContent = (BYTE*)pSource;

	if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return _T("");
	if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
		return _T("");
	if(!CryptHashData(hHash, pbContent, cbContent, 0))
		return _T("");
	if(!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
		return _T("");

	CString tmp;
	for (unsigned int i = 0; i < dwHashLen; i += 4)
	{
		tmp.Format(_T("%02x%02x%02x%02x"), bHash[i], bHash[i+1],bHash[i+2],bHash[i+3]);
		outData += tmp;
	}

	if(hHash)
		CryptDestroyHash(hHash);
	if(hProv)
		CryptReleaseContext(hProv, 0);

	return outData.MakeUpper();
}

CString CWebServiceHelper::CryptMD5(CString szSource)
{
	CStringA szSourceA = CCommonFun::UnicodeToMultiByte(szSource);
	return CryptMD5((unsigned char*)szSourceA.GetString(), szSourceA.GetLength());
}

BOOL CWebServiceHelper::TestWebService()
{
	Cloud_Webservice_Try_Catch_Exception_Begin

	CString szResponse;

	CString szServiceUrl;
	szServiceUrl.Format(_T("%s/test"), m_szServiceUrl);
	CHttpClient	oHttpClient;
	int nRet = oHttpClient.HttpPost(szServiceUrl, NULL, szResponse);

	if (nRet != SUCCESS)
	{
		theLog.Write(_T("!!CCloudWebServiceHelper::TestWebService,1,fail,nRet=%d,err=%u,szResponse=%s")
			, nRet, GetLastError(), szResponse);
		return FALSE;
	}

	Cloud_Webservice_Try_Catch_Exception_End
		return TRUE;
}

BOOL CWebServiceHelper::KMNotifyUrl(CString szServerIP)
{
	Cloud_Webservice_Try_Catch_Exception_Begin
	CString szJsonStr;
	CString szMD5Src;
	szMD5Src.Format(_T("type=14&serverip=%s&%s"), szServerIP, m_szWebserviceKey);
	theLog.Write(_T("szMD5Src=%s"), szMD5Src);
	szJsonStr.Format(_T("{")
		"\"serverip\":\"%s\","
		"\"signature\":\"%s\""
		"}", szServerIP, CryptMD5(szMD5Src));

	CString szResponse;
	CString szServiceUrl;
	szServiceUrl.Format(_T("%s/kmnotifyurl"), m_szServiceUrl);
	CHttpClient	oHttpClient;
	int nRet = oHttpClient.HttpPost(szServiceUrl, CCommonFun::UnicodeToUTF8(szJsonStr), szResponse);
	theLog.Write(_T("CWebServiceHelper::KMNotifyUrl,szServiceUrl=%s,szJsonStr=%s,szResponse=%s,nRet=%d"), szServiceUrl, szJsonStr,szResponse, nRet);
	if (nRet != SUCCESS)
	{
		theLog.Write(_T("!!CCloudWebServiceHelper::KMNotifyUrl,1,fail,nRet=%d,err=%u,szResponse=%s")
			, nRet, GetLastError(), szResponse);
		return FALSE;
	}

	Cloud_Webservice_Try_Catch_Exception_End
	return TRUE;
}

