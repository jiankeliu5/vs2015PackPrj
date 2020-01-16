#pragma once

//异常处理的宏
#define Cloud_Webservice_Try_Catch_Exception_Log	\
	theLog.Write(_T("Webservice执行出现异常：file：[%s]，function：[%s]，line：[%d]")\
	, __FILE__, __FUNCTION__, __LINE__);\
	return FALSE;

#define Cloud_Webservice_Try_Catch_Exception_Begin	try {
#define Cloud_Webservice_Try_Catch_Exception_End	}catch (...){ Cloud_Webservice_Try_Catch_Exception_Log }


/************************************************************************/
/* 		特别说明，在调用本类的webservice接口前不能调用【CoUninitialize】 
		如：CDiskSerial disk;disk.GetDiskSerialID();
		【GetDiskSerialID】内部调用了【CoUninitialize】，
		导致调用本类的webservice接口失败。
                                                                     */
/************************************************************************/
class CWebServiceHelper
{
public:
	CWebServiceHelper(void);
	~CWebServiceHelper(void);

public:
	static CString CryptMD5(unsigned char* pSource, int nLen);
	static CString CryptMD5(CString szSource);

public:
	BOOL IsInited();	//webservice是否初始化成功
	void ConfigWebService(CString szWebserviceKey, CString szServiceUrl, int nServiceTimeout = 15000
		, BOOL bIEProxy = FALSE, CString szIEProxyAddr = _T(""), int nIEProxyPort = 808);
	BOOL ConnWebService();
	BOOL ReleaseWebService();
	BOOL TestWebService();
	BOOL KMNotifyUrl(CString szServerIP);//http://localhost/xabprint/services/kmnotifyurl

protected:
	void SetInited(BOOL bSucc);	//设置webservice是否初始化成功
	CString UrlEncode(LPCSTR strDataUtf8);	//对数据进行URL编码

protected:
	long	m_lIsInited;		//webservice是否已经初始化成功
	CString m_szServiceUrl;		//webservice的URL全路径
	int		m_nServiceTimeout;	//webservice接口的超时时间,毫秒
	BOOL	m_bIEProxy;			//webservice是否启用代理
	CString m_szIEProxyAddr;	//webservice代理的IP或者域名
	int		m_nIEProxyPort;		//webservice代理端口
	CString m_szWebserviceKey;		//云服务器接口调用的key
};
