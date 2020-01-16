#pragma once

//�쳣����ĺ�
#define Cloud_Webservice_Try_Catch_Exception_Log	\
	theLog.Write(_T("Webserviceִ�г����쳣��file��[%s]��function��[%s]��line��[%d]")\
	, __FILE__, __FUNCTION__, __LINE__);\
	return FALSE;

#define Cloud_Webservice_Try_Catch_Exception_Begin	try {
#define Cloud_Webservice_Try_Catch_Exception_End	}catch (...){ Cloud_Webservice_Try_Catch_Exception_Log }


/************************************************************************/
/* 		�ر�˵�����ڵ��ñ����webservice�ӿ�ǰ���ܵ��á�CoUninitialize�� 
		�磺CDiskSerial disk;disk.GetDiskSerialID();
		��GetDiskSerialID���ڲ������ˡ�CoUninitialize����
		���µ��ñ����webservice�ӿ�ʧ�ܡ�
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
	BOOL IsInited();	//webservice�Ƿ��ʼ���ɹ�
	void ConfigWebService(CString szWebserviceKey, CString szServiceUrl, int nServiceTimeout = 15000
		, BOOL bIEProxy = FALSE, CString szIEProxyAddr = _T(""), int nIEProxyPort = 808);
	BOOL ConnWebService();
	BOOL ReleaseWebService();
	BOOL TestWebService();
	BOOL KMNotifyUrl(CString szServerIP);//http://localhost/xabprint/services/kmnotifyurl

protected:
	void SetInited(BOOL bSucc);	//����webservice�Ƿ��ʼ���ɹ�
	CString UrlEncode(LPCSTR strDataUtf8);	//�����ݽ���URL����

protected:
	long	m_lIsInited;		//webservice�Ƿ��Ѿ���ʼ���ɹ�
	CString m_szServiceUrl;		//webservice��URLȫ·��
	int		m_nServiceTimeout;	//webservice�ӿڵĳ�ʱʱ��,����
	BOOL	m_bIEProxy;			//webservice�Ƿ����ô���
	CString m_szIEProxyAddr;	//webservice�����IP��������
	int		m_nIEProxyPort;		//webservice����˿�
	CString m_szWebserviceKey;		//�Ʒ������ӿڵ��õ�key
};
