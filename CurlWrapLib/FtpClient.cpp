#include "StdAfx.h"
#include "FtpClient.h"
#include <io.h>

#include <curl/curl.h>

CFtpClient::CFtpClient(void)
{
	m_bDebug = false;
	m_oFtpType = FT_Ftp;
	m_strServer.clear();
	m_nPort = 21;
	m_strUser.clear();
	m_strPassword.clear();

	/*
	{
		//��ȡcurl֧�ֵ�����Э�飬һ������£�curl���ܲ�֧��sftp��
		//���±���curl���libssh2��֧�֣��Դﵽ֧��sftpЭ���Ŀ�ġ�
		curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
		printf("age=%d,ssl_version=%s\n", data->age, data->ssl_version);
		int i=0;
		const char* pp = data->protocols[i];
		while (pp)
		{
			i++;
			printf("%s\n", pp);
			pp = data->protocols[i];
		}
	}
	*/
}

CFtpClient::~CFtpClient(void)
{
}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)  
{  
	if(itype == CURLINFO_TEXT)  
	{  
		//printf("[TEXT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_IN)  
	{  
		printf("[HEADER_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_HEADER_OUT)  
	{  
		printf("[HEADER_OUT]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_IN)  
	{  
		printf("[DATA_IN]%s\n", pData);  
	}  
	else if(itype == CURLINFO_DATA_OUT)  
	{  
		printf("[DATA_OUT]%s\n", pData);  
	}  
	return 0;  
}

/* read data to upload */ 
static size_t readfunc(void *buffer, size_t size, size_t nitems, void *instream)
{
	FtpFileOperate *pOperate = (FtpFileOperate *)instream;
	size_t nSize;

	if (pOperate)
	{
		IFtpTransferInterface* pCallBack = (IFtpTransferInterface*)pOperate->pData;
		if (pCallBack)
		{
			FtpTransferStatus status = pCallBack->OnFtpTransferData(pOperate->szLocalPath
				, pOperate->oFtpTransferType, pOperate->nFileSize, pOperate->nFileTransferSize);
			if (status != FTS_Ok)
			{
				printf("!!CFtpClient::readfunc,fail,OnFileOperateData,status=%d\n", status);
				return CURL_READFUNC_ABORT;
			}
		}

		if(ferror(pOperate->pLocalFile))
			return CURL_READFUNC_ABORT;

		nSize = fread(buffer, size, nitems, pOperate->pLocalFile) * size;

		pOperate->nFileTransferSize += nSize;

		return nSize;
	}	
	else
	{
		return CURL_READFUNC_ABORT;
	}
}

static size_t writefunc(void *buffer, size_t size, size_t nmemb, void *outstream)
{
	FtpFileOperate *pOperate = (FtpFileOperate *)outstream;
	size_t nSize;

	if (pOperate)
	{
		if(ferror(pOperate->pLocalFile))
			return CURL_WRITEFUNC_PAUSE;

		nSize = fwrite(buffer, size, nmemb, pOperate->pLocalFile);

		pOperate->nFileTransferSize += nSize;

		IFtpTransferInterface* pCallBack = (IFtpTransferInterface*)pOperate->pData;
		if (pCallBack)
		{
			FtpTransferStatus status = pCallBack->OnFtpTransferData(pOperate->szLocalPath
				, pOperate->oFtpTransferType, pOperate->nFileSize, pOperate->nFileTransferSize);
			if (status != FTS_Ok)
			{
				printf("!!CFtpClient::writefunc,fail,OnFileOperateData,status=%d\n", status);
				return CURL_READFUNC_ABORT;
			}
		}

		return nSize;
	}
	else
	{
		return CURL_WRITEFUNC_PAUSE;
	}
}

//�Ƿ�������־
void CFtpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

//����FTP��Ϣ
void CFtpClient::SetFtpInfo(FtpType oFtpType, const std::string & strServer, int nPort, 
				const std::string & strUser, const std::string & strPassword)
{
	m_oFtpType = oFtpType;
	m_strServer = strServer;
	m_nPort = nPort;
	m_strUser = strUser;
	m_strPassword = strPassword;
}

//�ϴ�ftp�ļ�
bool CFtpClient::UploadFile(const std::string & strLocalPath, const std::string & strRemotePath, IFtpTransferInterface *pCallBack /*= NULL*/)
{
	printf("CFtpClient::UploadFile,1,strLocalPath=%s,strRemotePath=%s,pCallBack=%p\n", strLocalPath.c_str(), strRemotePath.c_str(), pCallBack);

	DWORD dwTick1 = GetTickCount();

	bool bRet = false;
	FtpFileOperate oOperate;
	memset(&oOperate, 0x0, sizeof(FtpFileOperate));
	strcpy_s(oOperate.szRemotePath, sizeof(oOperate.szRemotePath), strRemotePath.c_str());
	strcpy_s(oOperate.szLocalPath, sizeof(oOperate.szLocalPath), strLocalPath.c_str());
	oOperate.oFtpTransferType = FTT_Upload;
	oOperate.oFtpTransferStatus = FTS_Error;
	oOperate.pData = pCallBack;

	if (pCallBack)
	{
		pCallBack->OnFtpTransferBegin(oOperate.szLocalPath, oOperate.oFtpTransferType);
	}

	do 
	{
		//�ϴ��ļ�֮ǰ���ȴ���Ŀ¼�ṹ
		CreateDirectory(strRemotePath);

		CURLcode res;  
		CURL* curl = curl_easy_init();  
		if(NULL == curl)  
		{  
			printf("!!CFtpClient::UploadFile,2,curl_easy_init fail\n");
			break;  
		}  

		FILE *pFile = NULL;
		errno_t nError = _wfopen_s(&pFile, UTF8ToUnicode(strLocalPath.c_str()).c_str(), L"rb");
		if (!pFile)
		{
			printf("!!CFtpClient::UploadFile,3,_wfopen_s fail,nError=%d\n", nError);
			break;
		}

		oOperate.pLocalFile = pFile;
		oOperate.nFileSize = GetFileSize(pFile);

		if(m_bDebug)  
		{  
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}

		//���ò���
		ConfigSSL(curl);
		ConfigNetwork(curl);
	
		std::string strUrl = GetRemoteUrl(TransCodeURL(strRemotePath));
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
		curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);		
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunc);
		curl_easy_setopt(curl, CURLOPT_READDATA, &oOperate);
		//curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1);//�ϵ��������ܣ�׷���ļ�
		res = curl_easy_perform(curl);  
		curl_easy_cleanup(curl);  

		fclose(pFile);

		if(res == CURLE_OK)
		{
			printf("CFtpClient::UploadFile,4,succ\n");
			oOperate.oFtpTransferStatus = FTS_Ok;
			bRet = true;
		}
		else 
		{
			printf("!!CFtpClient::UploadFile,4,fail,errmg=%s\n", curl_easy_strerror(res));
		}	
	} while (false);

	if (pCallBack)
	{
		pCallBack->OnFtpTransferEnd(oOperate.szLocalPath, oOperate.oFtpTransferType, oOperate.oFtpTransferStatus);
	}

	DWORD dwTick2 = GetTickCount();
	printf("CFileHelper::UploadFile,9,bRet=%d,dwFileSize=%u,��ʱ=%d(ms)\n", bRet, (DWORD)oOperate.nFileSize, dwTick2-dwTick1);

	return bRet;
}

//�����ļ�
bool CFtpClient::DownloadFile(const std::string & strRemotePath, const std::string & strLocalPath, IFtpTransferInterface *pCallBack /*= NULL*/)
{
	printf("CFtpClient::DownloadFile,1,strRemotePath=%s,strLocalPath=%s,pCallBack=%p\n", strRemotePath.c_str(), strLocalPath.c_str(), pCallBack);

	DWORD dwTick1 = GetTickCount();

	bool bRet = false;
	FtpFileOperate oOperate;
	memset(&oOperate, 0x0, sizeof(FtpFileOperate));
	strcpy_s(oOperate.szRemotePath, sizeof(oOperate.szRemotePath), strRemotePath.c_str());
	strcpy_s(oOperate.szLocalPath, sizeof(oOperate.szLocalPath), strLocalPath.c_str());
	oOperate.oFtpTransferType = FTT_Download;
	oOperate.oFtpTransferStatus = FTS_Error;
	oOperate.pData = pCallBack;

	if (pCallBack)
	{
		pCallBack->OnFtpTransferBegin(oOperate.szLocalPath, oOperate.oFtpTransferType);
	}

	do 
	{
		CURLcode res;  
		CURL* curl = curl_easy_init();  
		if(NULL == curl)  
		{  
			printf("!!CFtpClient::DownloadFile,2,curl_easy_init fail\n");
			break;  
		} 

		FILE *pFile = NULL;
		errno_t nError = _wfopen_s(&pFile, UTF8ToUnicode(strLocalPath.c_str()).c_str(), L"wb");
		if (!pFile)
		{
			printf("!!CFtpClient::DownloadFile,3,_wfopen_s fail,nError=%d\n", nError);
			break;
		}

		oOperate.pLocalFile = pFile;

		if(m_bDebug)  
		{  
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}

		//���ò���
		ConfigSSL(curl);
		ConfigNetwork(curl);

		std::string strUrl = GetRemoteUrl(TransCodeURL(strRemotePath));
		oOperate.nFileSize = GetRemoteFileSize(strUrl);
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
		curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &oOperate);
		res = curl_easy_perform(curl);  
		curl_easy_cleanup(curl);  

		fclose(pFile);

		if(res == CURLE_OK)
		{
			printf("CFtpClient::DownloadFile,4,succ\n");
			oOperate.oFtpTransferStatus = FTS_Ok;
			bRet = true;
		}
		else 
		{
			printf("!!CFtpClient::DownloadFile,5,fail,errmg=%s\n", curl_easy_strerror(res));
		}	
	} while (false);


	if (pCallBack)
	{
		pCallBack->OnFtpTransferEnd(oOperate.szLocalPath, oOperate.oFtpTransferType, oOperate.oFtpTransferStatus);
	}

	DWORD dwTick2 = GetTickCount();
	printf("CFileHelper::DownloadFile,9,bRet=%d,dwFileSize=%u,��ʱ=%d(ms)\n", bRet, (DWORD)oOperate.nFileSize, dwTick2-dwTick1);

	return bRet;
}

//ɾ���ļ�
bool CFtpClient::DeleteFile(const std::string & strRemotePath)
{
	printf("CFtpClient::DeleteFile,1,strRemotePath=%s\n", strRemotePath.c_str());

	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		printf("!!CFtpClient::DeleteFile,2,curl_easy_init fail\n");
		return false;  
	} 

	if(m_bDebug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_slist *headerlist = NULL;
	if (m_oFtpType == FT_SFtp)
	{
		headerlist = curl_slist_append(headerlist, GetCommand("RM", strRemotePath).c_str());
	}
	else
	{
		headerlist = curl_slist_append(headerlist, GetCommand("DELE", strRemotePath, false).c_str());
	}

	//���ò���
	ConfigSSL(curl);
	ConfigNetwork(curl);

	std::string strUrl = GetServerUrl();
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
	curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
	curl_easy_setopt(curl, CURLOPT_QUOTE , headerlist); 
	res = curl_easy_perform(curl);  
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);

	if(res == CURLE_OK)
	{
		printf("CFtpClient::DeleteFile,4,succ\n");
		return true;
	}
	else 
	{
		printf("!!CFtpClient::DeleteFile,5,fail,errmg=%s\n", curl_easy_strerror(res));
		return false;
	}
}

//����Ŀ¼���𼶴���Ŀ¼�ṹ
bool CFtpClient::CreateDirectory(const std::string & strRemotePath)
{
	printf("CFtpClient::CreateDirectory,1,strRemotePath=%s\n", strRemotePath.c_str());

	std::vector<std::string> oDirectoryVector;
	GetDirectory(strRemotePath, oDirectoryVector);
	size_t nSize = oDirectoryVector.size();

	if (nSize <= 1)
	{
		return true;
	}

	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		printf("!!CFtpClient::CreateDirectory,2,curl_easy_init fail\n");
		return false;  
	} 

	if(m_bDebug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	//���ò���
	ConfigSSL(curl);
	ConfigNetwork(curl);

	std::string strUrl = GetServerUrl();
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
	curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());

	//����𼶴���Ŀ¼�����Ĭ�ϴӵڶ�����ʼ����Ϊ��һ�����Ǹ�Ŀ¼��/����
	if (m_oFtpType == FT_SFtp)
	{
		for (size_t nIndex=1; nIndex<oDirectoryVector.size(); nIndex++)
		{
			struct curl_slist *headerlist = NULL;
			headerlist = curl_slist_append(headerlist, GetCommand("MKDIR", oDirectoryVector[nIndex]).c_str());
			curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist); 
			res = curl_easy_perform(curl);
			curl_slist_free_all(headerlist);
			headerlist = NULL;
		}
	}
	else
	{
		//FTP·������ʱ��֧��ʹ��˫���ţ����򴴽�Ŀ¼ʧ��
		for (size_t nIndex=1; nIndex<oDirectoryVector.size(); nIndex++)
		{
			struct curl_slist *headerlist = NULL;
			//1.���ȳ��Խ���Ŀ¼
			headerlist = curl_slist_append(headerlist, GetCommand("CWD", oDirectoryVector[nIndex], false).c_str());
			curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist); 
			res = curl_easy_perform(curl);
			curl_slist_free_all(headerlist);
			headerlist = NULL;
			if (res != CURLE_OK)
			{
				//2.����Ŀ¼ʧ�ܣ������Ŀ��Ŀ¼����һ��Ŀ¼
				headerlist = curl_slist_append(headerlist, GetCommand("CWD", oDirectoryVector[nIndex-1], false).c_str());
				curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist); 
				res = curl_easy_perform(curl);
				curl_slist_free_all(headerlist);
				headerlist = NULL;

				//3.����Ŀ��Ŀ¼����һ��Ŀ¼�󣬴�����Ŀ¼
				headerlist = curl_slist_append(headerlist, GetCommand("MKD", GetLastDirectory(oDirectoryVector[nIndex]), false).c_str());
				curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist); 
				res = curl_easy_perform(curl);
				curl_slist_free_all(headerlist);
				headerlist = NULL;
			}
		}
	}
	
	curl_easy_cleanup(curl);

	if(res == CURLE_OK)
	{
		printf("CFtpClient::CreateDirectory,4,succ\n");
		return true;
	}
	else 
	{
		printf("!!CFtpClient::CreateDirectory,5,fail,errmg=%s\n", curl_easy_strerror(res));
		return false;
	}
}

//ɾ��Ŀ¼����Ŀ¼������Ŀ¼���ļ�ʱ��ɾ��Ŀ¼ʧ��
bool CFtpClient::DeleteDirectory(const std::string & strRemotePath)
{
	printf("CFtpClient::DeleteDirectory,1,strRemotePath=%s\n", strRemotePath.c_str());

	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		printf("!!CFtpClient::DeleteDirectory,2,curl_easy_init fail\n");
		return false;  
	} 

	if(m_bDebug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_slist *headerlist = NULL;
	if (m_oFtpType == FT_SFtp)
	{
		headerlist = curl_slist_append(headerlist, GetCommand("RMDIR", strRemotePath).c_str());
	}
	else
	{
		headerlist = curl_slist_append(headerlist, GetCommand("RMD", strRemotePath, false).c_str());
	}

	//���ò���
	ConfigSSL(curl);
	ConfigNetwork(curl);

	std::string strUrl = GetServerUrl();
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
	curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
	curl_easy_setopt(curl, CURLOPT_QUOTE , headerlist); 
	res = curl_easy_perform(curl);  
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);

	if(res == CURLE_OK)
	{
		printf("CFtpClient::DeleteDirectory,4,succ\n");
		return true;
	}
	else 
	{
		printf("!!CFtpClient::DeleteDirectory,5,fail,errmg=%s\n", curl_easy_strerror(res));
		return false;
	}
}

//���������󣬿������ļ�����Ŀ¼
bool CFtpClient::Rename(const std::string & strOldRemotePath, const std::string & strNewRemotePath)
{
	printf("CFtpClient::Rename,1,strOldRemotePath=%s,strNewRemotePath=%s\n"
		, strOldRemotePath.c_str(), strNewRemotePath.c_str());

	CURLcode res;  
	CURL* curl = curl_easy_init();  
	if(NULL == curl)  
	{  
		printf("!!CFtpClient::Rename,2,curl_easy_init fail\n");
		return false;  
	} 

	if(m_bDebug)  
	{  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_slist *headerlist = NULL;
	if (m_oFtpType == FT_SFtp)
	{
		headerlist = curl_slist_append(headerlist, GetCommand2("RENAME", strOldRemotePath, strNewRemotePath).c_str());
	}
	else
	{
		headerlist = curl_slist_append(headerlist, GetCommand("RNFR", strOldRemotePath, false).c_str());
		headerlist = curl_slist_append(headerlist, GetCommand("RNTO", strNewRemotePath, false).c_str());
	}

	//���ò���
	ConfigSSL(curl);
	ConfigNetwork(curl);

	std::string strUrl = GetServerUrl();
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str()); 
	curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
	curl_easy_setopt(curl, CURLOPT_QUOTE , headerlist); 
	res = curl_easy_perform(curl);  
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);

	if(res == CURLE_OK)
	{
		printf("CFtpClient::Rename,4,succ\n");
		return true;
	}
	else 
	{
		printf("!!CFtpClient::Rename,5,fail,errmg=%s\n", curl_easy_strerror(res));
		return false;
	}
}

std::string CFtpClient::GetServerUrl()
{
	//Url��ʽ
	//ftp://server
	//sftp://server:port
	std::ostringstream ostr;
	if (m_oFtpType == FT_SFtp)
	{
		ostr << "sftp://";  
	}
	else if (m_oFtpType == FT_Ftps)
	{
		ostr << "ftps://";  
	}
	else //FT_Ftp
	{
		ostr << "ftp://";  
	}

	ostr << m_strServer << ":" << m_nPort;
	return ostr.str();
}

std::string CFtpClient::GetRemoteUrl(const std::string & strRemotePath)
{
	//Url��ʽ���������û��������룬��Ϊ����������@���ţ��ᵼ��URL�쳣
	//ftp://user@server/home/user/file.txt
	//sftp://user@server:port/home/user/file.txt
	std::ostringstream ostr;
	if (m_oFtpType == FT_SFtp)
	{
		ostr << "sftp://";  
	}
	else if (m_oFtpType == FT_Ftps)
	{
		ostr << "ftps://";  
	}
	else //FT_Ftp
	{
		ostr << "ftp://";  
	}

#if 0
	ostr << m_strUser << ":" << m_strPassword << "@" << m_strServer << ":" << m_nPort << strRemotePath;
#else
	ostr << m_strServer << ":" << m_nPort << strRemotePath;
#endif
	return ostr.str();
}

std::string CFtpClient::GetCommand(const std::string & strCommand, const std::string & strRemotePath, bool bAddDoubleQuotationMarks /*= true*/)
{
	std::ostringstream ostr;

	if (bAddDoubleQuotationMarks)
	{
		//�����ʽ���硾DELE "/test.pdf"��
		ostr << strCommand << " \"" << strRemotePath << "\"";
	}
	else
	{
		//�����ʽ���硾DELE /test.pdf��
		ostr << strCommand << " " << strRemotePath;
	}
	return ostr.str();
}

std::string CFtpClient::GetCommand2(const std::string & strCommand, const std::string & strRemotePath, const std::string & strRemotePath2, bool bAddDoubleQuotationMarks /*= true*/)
{
	std::ostringstream ostr;

	if (bAddDoubleQuotationMarks)
	{
		//�����ʽ���硾RENAME "/test.pdf" "/test2.pdf"��
		ostr << strCommand << " \"" << strRemotePath << "\" \"" << strRemotePath2 << "\"";
	}
	else
	{
		//�����ʽ���硾RENAME /test.pdf /test2.pdf��
		ostr << strCommand << " " << strRemotePath << " " << strRemotePath2;
	}
	return ostr.str();
}

//��ȡһ��URL�е����м�������Ŀ¼��ͨ�����飨oDirectoryVector������
//��/aa/bb/test.pdf��=����/��+��/aa/��+��/aa/bb/��
void CFtpClient::GetDirectory(const std::string & strRemotePath, std::vector<std::string>& oDirectoryVector)
{
	std::string strTemp = strRemotePath;
	size_t nPos = strTemp.find('/');
	while (nPos != std::string::npos)
	{
		oDirectoryVector.push_back(strTemp.substr(0, nPos+1));
		nPos = strTemp.find('/', nPos+1);
	}
}

//��ȡһ��·���е����һ��Ŀ¼��
//��/aa/bb/��=����bb��
//��/aa/bb��=����bb��
std::string CFtpClient::GetLastDirectory(const std::string & strFullDirectory)
{
	std::string strTemp = strFullDirectory;
	strTemp = strTemp.substr(0, strTemp.find_last_not_of('/')+1);
	strTemp = strTemp.substr(strTemp.find_last_of('/')+1);
	return strTemp;
}

std::string CFtpClient::TransCodeURL(const std::string & strURL)
{
	std::string strPath;
	size_t nLength = strURL.length();
	for (size_t nIndex=0; nIndex<nLength; nIndex++)
	{
		char nChar = strURL[nIndex];
		switch (nChar)
		{
		case ' ':
			strPath += "%20";
				break;
		case '!':
			strPath += "%21";
				break;
		case '#':
			strPath += "%23";
				break;
		case '%':
			strPath += "%25";
			break;
		case '&':
			strPath += "%26";
				break;
		case '@':
			strPath += "%40";
				break;
		/*
		case '/':
			strPath += "%2F";
				break;
		*/
		case ':':
			strPath += "%3A";
				break;
		case '=':
			strPath += "%3D";
				break;
		case '?':
			strPath += "%3F";
				break;
		case '~':
			strPath += "%7E";;
				break;
		default:
			strPath += nChar;
			break;			
		}
	}

	return strPath;
}

__int64 CFtpClient::GetRemoteFileSize(const std::string & strRemoteUrl)
{
	CURLcode result = CURLE_GOT_NOTHING;
	curl_off_t remoteFileSizeByte = -1;
	CURL *curl = NULL;

	curl = curl_easy_init();

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strRemoteUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_FILETIME, 0);

	result = curl_easy_perform(curl);
	if(CURLE_OK == result) 
	{
		result = curl_easy_getinfo(curl,
			CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,
			&remoteFileSizeByte);
		if(result)
			return -1;
		printf("filesize: %" CURL_FORMAT_CURL_OFF_T "\n", remoteFileSizeByte);
	}
	else
	{
		printf("!!CFtpClient::GetRemoteFileSize,fail,errmg=%s\n", curl_easy_strerror(result));
	}

	curl_easy_cleanup(curl);

	return remoteFileSizeByte;
}

__int64 CFtpClient::GetFileSize(const std::string & strFilePath)
{
	FILE *pFile = NULL;
	errno_t nError = _wfopen_s(&pFile, UTF8ToUnicode(strFilePath.c_str()).c_str(), L"rb");
	if (!pFile)
	{
		printf("!!CFtpClient::GetFileSize,fail,_wfopen_s fail, nError=%d\n", nError);
		return -1;
	}

	__int64 nFileSize = GetFileSize(pFile);
	fclose(pFile);
	return nFileSize;
}

__int64 CFtpClient::GetFileSize(const FILE* pFile)
{
	return _filelengthi64(_fileno((FILE*)pFile));
}

std::wstring CFtpClient::UTF8ToUnicode(const char* pszSource)
{
	std::wstring szRetW = L"";
	if (pszSource)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, pszSource, -1, NULL, 0);
		wchar_t * wszText = new wchar_t[len + 1];
		memset(wszText, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, pszSource, -1, wszText, len);

		szRetW = wszText;
		delete[] wszText;
	}
	return szRetW;
}

std::string CFtpClient::UnicodeToUTF8(const wchar_t* pwszSource)
{
	std::string szRetA = "";
	if (pwszSource)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, pwszSource, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_UTF8,0, pwszSource, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] szText;
	}
	return szRetA;
}

//����SSL����
void CFtpClient::ConfigSSL(void* pContent)
{
	CURL* curl = (CURL*)pContent;
	if (!curl)
	{
		return;
	}

	//curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	if (m_oFtpType == FT_Ftps)
	{
		//FTPS���ִ���ʱ��SSL peer certificate or SSH remote key was not OK��,
		//�������������������ɳɹ���������ʱ��ô��������Ҫ����ȷ�İ�ȫ����
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	}
}

//�����������
void CFtpClient::ConfigNetwork(void* pContent)
{
	//��ʱ�����κ����ã�ʹ��curl��Ĭ�ϲ�����
	//��������Ҫ�����Խ��г�ʱ�ʹ���֮�������
#if 0
	CURL* curl = (CURL*)pContent;
	if (!curl)
	{
		return;
	}

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
#endif
}

