////////////////////////////////// HttpClient.cpp
#include "StdAfx.h"
#include "HttpClient.h"

#define  READ_BUFFER_SIZE			1024
#ifndef CP_GB2312
#define  CP_GB2312					936	//Code Page Identifiers ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
#endif

#define  NORMAL_CONNECT             INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT             NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST             INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST             NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

CHttpClient::CHttpClient(LPCTSTR strAgent)
{
    m_pSession = new CInternetSession(strAgent);
    m_pConnection = NULL;
    m_pFile = NULL;
}


CHttpClient::~CHttpClient(void)
{
    Clear();
    if(NULL != m_pSession)
    {
        m_pSession->Close();
        delete m_pSession;
        m_pSession = NULL;
    }
}

void CHttpClient::Clear()
{
    if(NULL != m_pFile)
    {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }

    if(NULL != m_pConnection)
    {
        m_pConnection->Close();
        delete m_pConnection;
        m_pConnection = NULL;
    }
}

int CHttpClient::ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, LPCSTR strPostData, CString &strResponse, bool bTransCode /*= true*/, bool bPostDataIsJson /*= true*/)
{
    CString strServer;
    CString strObject;
    DWORD dwServiceType;
    INTERNET_PORT nPort;
    strResponse = "";

    AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

    if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
    {
        return FAILURE;
    }

    try
    {
        m_pConnection = m_pSession->GetHttpConnection(strServer,
            dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
            nPort);
        m_pFile = m_pConnection->OpenRequest(strMethod, strObject, 
            NULL, 1, NULL, NULL, 
            (dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

		//设置忽略一些安全选项，如一些HTTPS网站使用了自签的安全证书不受信任等，
		//否则将无法正常获取HTTPS数据。
		if (dwServiceType == AFX_INET_SERVICE_HTTPS)
		{
			DWORD dwFlags;
			m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
			dwFlags |= SECURITY_IGNORE_ERROR_MASK;
			//set web server option
			m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
		}

        m_pFile->AddRequestHeaders(_T("Accept: *,*/*"));
        m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
		if (bPostDataIsJson)
		{
			m_pFile->AddRequestHeaders(_T("Content-Type: application/json"));
		}
		else
		{
			m_pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));
		}
		m_pFile->AddRequestHeaders(_T("Accept-Encoding: utf-8"));

        m_pFile->SendRequest(NULL, 0, (LPVOID)strPostData, (strPostData ? strlen(strPostData) : 0));

        char szChars[READ_BUFFER_SIZE + 1] = {0};
        CStringA strRawResponse = "";
        UINT nReaded = 0;
        while ((nReaded = m_pFile->Read((void*)szChars, READ_BUFFER_SIZE)) > 0)
        {
            szChars[nReaded] = '\0';
            strRawResponse += szChars;
            memset(szChars, 0, READ_BUFFER_SIZE + 1);
        }

		if (bTransCode)
		{
#ifdef UNICODE
			strResponse = CCommonFun::UTF8ToUnicode(strRawResponse);
#else
			strResponse = CCommonFun::UTF8ToMultiByte(strRawResponse);
#endif // UNICODE
		}
		else
		{
			strResponse = strRawResponse;
		}

        Clear();
    }
    catch (CInternetException* e)
    {
        Clear();
		DWORD dwErrorCode = e->m_dwError;
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
        e->Delete();

        theLog.Write(_T("!!CHttpClient::ExecuteRequest,fail,dwError=%u,tcErrMsg=[%s]"), GetLastError(), tcErrMsg);

        if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
        {
            return OUTTIME;
        }
        else
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int CHttpClient::HttpGet(LPCTSTR strUrl, LPCSTR strPostData, CString &strResponse, bool bTransCode /*= true*/, bool bPostDataIsJson /*= true*/)
{
    return ExecuteRequest(_T("GET"), strUrl, strPostData, strResponse, bTransCode, bPostDataIsJson);
}

int CHttpClient::HttpPost(LPCTSTR strUrl, LPCSTR strPostData, CString &strResponse, bool bTransCode /*= true*/, bool bPostDataIsJson /*= true*/)
{
    return ExecuteRequest(_T("POST"), strUrl, strPostData, strResponse, bTransCode, bPostDataIsJson);
}
