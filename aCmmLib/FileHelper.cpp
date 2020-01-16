#include "StdAfx.h"
#include "FileHelper.h"
#include "Base64.h"
#include "../Public/ZLibWrapPort.h"

#include <afxinet.h>

#define DownLoad_File_Unit_Size		(1024*64)	//文件下载的最大单元大小，默认为64KB大小。
#define UpLoad_File_Unit_Size		(1024*64)	//文件上传的最大单元大小，默认为64KB大小。
#define Compress_File_Unit_Size		(1024*1024*10)	//文件压缩的最大单元大小，默认为10MB大小。
#define UnCompress_File_Unit_Size	(1024*1024*10)	//文件解压的最大单元大小，默认为10MB大小。

CFileHelper::CFileHelper(void)
{
}

CFileHelper::~CFileHelper(void)
{
}

//设置网络超时时间，单位是毫秒
void SetTimeOut(CInternetSession* pSession, DWORD dwConnectRetries = 3, DWORD dwConnectTimeout = 5000
				, DWORD dwReceiveTimeout = 8000, DWORD dwSendTimeout = 8000)
{
	if (pSession)
	{
		BOOL bRet = pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, dwConnectRetries);
		bRet = pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, dwConnectTimeout);
		bRet = pSession->SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, dwReceiveTimeout);
		bRet = pSession->SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, dwSendTimeout);
		bRet = pSession->SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, dwReceiveTimeout);
		bRet = pSession->SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, dwSendTimeout);
	}
}

BOOL CreateAndSetFtpDir(CFtpConnection* pConn, CString szFtpDir)
{
	BOOL bSucc = FALSE;
	do 
	{
		if (!pConn)
		{
			theLog.Write(_T("CreateAndSetFtpDir,1,pConn=%p"), pConn);
			break;
		}

		CString sSplitter = _T("\\");
		int nSplitLen = sSplitter.GetLength();

		szFtpDir.Replace(_T("/"), _T("\\"));
		CString sTemp(szFtpDir);
		sTemp.Replace(_T("/"), _T("\\"));
		sTemp.TrimLeft(_T("\\"));
		sTemp.TrimRight(_T("\\"));
		if(sTemp.IsEmpty())
		{
			theLog.Write(_T("!!CreateAndSetFtpDir,2,sTemp=%s"), sTemp);
			break;
		}

		if (!pConn->SetCurrentDirectory(_T("\\")))
		{
			theLog.Write(_T("!!CreateAndSetFtpDir,2,SetCurrentDirectory (\\) fail,error=%u"), GetLastError());
			break;
		}

		//逐级创建目录
		while(sTemp)
		{
			int nSite = sTemp.Find(sSplitter);
			if(nSite == -1)
			{
				if (!pConn->CreateDirectory(sTemp))
				{
					theLog.Write(_T("!!CreateAndSetFtpDir,3,CreateDirectory (%s) fail,error=%u"), sTemp, GetLastError());
				}
				else
				{
					//进入最后一级目录
					if (!pConn->SetCurrentDirectory(sTemp))
					{
						theLog.Write(_T("!!CreateAndSetFtpDir,3.2,SetCurrentDirectory (%s) fail,error=%u"), sTemp, GetLastError());
					}
				}
				break;
			}

			CString sNew = sTemp.Left(nSite);

			//第一次设置当前目录
			if (!pConn->SetCurrentDirectory(sNew))
			{
				theLog.Write(_T("!!CreateAndSetFtpDir,4,SetCurrentDirectory (%s) fail,error=%u"), sNew, GetLastError());
				//创建目录
				if (!pConn->CreateDirectory(sNew))
				{
					theLog.Write(_T("!!CreateAndSetFtpDir,5,CreateDirectory (%s) fail,error=%u"), sNew, GetLastError());
					break;
				}

				//第二次设置当前目录
				if (!pConn->SetCurrentDirectory(sNew))
				{
					theLog.Write(_T("!!CreateAndSetFtpDir,6,SetCurrentDirectory (%s) fail,error=%u"), sNew, GetLastError());
					break;
				}
			}

			sTemp = (LPCTSTR)sTemp + nSplitLen + nSite;
		}

		bSucc = TRUE;
	} while (FALSE);

	return bSucc;
}

BOOL CFileHelper::UploadFileToFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir
											 , CString szUserName, CString szPwd
											 , CString szFtpFileName, CString szLocalFilePath
											 , IFileOperateInterface *pCallBack /*= NULL*/)
{
// 	theLog.Write(_T("CFileHelper::UploadFileToFtpServer,1,szFtpServer=%s,nFtpPort=%d,szFtpDir=%s,szUserName=%s,szPwd=%s")
// 		_T(",szFtpFileName=%s,szLocalFilePath=%s"),szFtpServer, nFtpPort, szFtpDir, szUserName, /*szPwd*/_T("***"), szFtpFileName, szLocalFilePath);
	theLog.Write(_T("CFileHelper::UploadFileToFtpServer,1,szFtpDir=%s,pCallBack=%p"),szFtpDir,pCallBack);

	DWORD dwTick1 = GetTickCount();

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	CInternetSession* pSession = NULL;
	CFtpConnection* pConn = NULL;
	CInternetFile* pInternetFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;
	BYTE* pData = NULL;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szLocalFilePath.GetString(), FOT_Upload);
	}

	try
	{
		do 
		{
			if (!PathFileExists(szLocalFilePath))
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,2,Path not Exists, szLocalFilePath=[%s}"), szLocalFilePath);
				break;
			}

			dwFileSize = CCommonFun::GetFileSizeByte(szLocalFilePath);

			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,2.1,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			pConn = pSession->GetFtpConnection(szFtpServer,szUserName,szPwd,nFtpPort,TRUE);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,2.2,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}

			szFtpDir.Replace(_T("/"), _T("\\"));
			szFtpDir.Trim(_T("\\"));
			szFtpDir = _T("\\") + szFtpDir;
			if (!pConn->SetCurrentDirectory(szFtpDir))
			{
				theLog.Write(_T("CFileHelper::UploadFileToFtpServer,3,pConn->SetCurrentDirectory fail,err=%d, szFtpDir=%s")
					, GetLastError(), szFtpDir);
				if (!CreateAndSetFtpDir(pConn, szFtpDir))
				{
					theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,3.2,CreateAndSetFtpDir fail"));
					break;
				}
			}

			CFile file;
			if (!file.Open(szLocalFilePath, CFile::modeRead))
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,4,file.Open fail,err=%d,szLocalFilePath=%s")
					, GetLastError(), szLocalFilePath);
				break;
			}

			pInternetFile = pConn->OpenFile(szFtpFileName, GENERIC_WRITE);
			if (!pInternetFile)
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,5,file.OpenFile fail,err=%d,szFtpFileName=%s")
					, GetLastError(), szFtpFileName);
				break;
			}

			pData = new BYTE[UpLoad_File_Unit_Size];
			while (TRUE)
			{
				memset(pData, 0x0, UpLoad_File_Unit_Size);
				int nRead = file.Read(pData, UpLoad_File_Unit_Size);
				if (nRead > 0)
				{
					pInternetFile->Write(pData, nRead);
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szLocalFilePath.GetString(), FOT_Upload, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,6,OnFileOperateData,status=%d"), status);
							break;
						}
					}
				}
				else
				{
					break;
				}
			}

			file.Close();
			if (dwFileSize == dwOperateFileSize)
			{
				status = FOS_Ok;
				bRet = TRUE;
			}
			else
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,7,dwFileSize=%u,dwOperateFileSize=%u"), dwFileSize, dwOperateFileSize);
			}
		} while (FALSE);
	}
	catch (CException* e)
	{
		status = FOS_Error;
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,8,catch CException,err=%d,tcErrMsg=%s"), GetLastError(), tcErrMsg);
	}

	if (pData)
	{
		delete[] pData;
	}
	if (pInternetFile)
	{
		pInternetFile->Close();
		delete pInternetFile;
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szLocalFilePath.GetString(), FOT_Upload, status);
	}

	DWORD dwTick2 = GetTickCount();
	theLog.Write(_T("CFileHelper::UploadFileToFtpServer,9,bRet=%d,dwFileSize=%u,耗时=%d(ms)"), bRet, dwFileSize, dwTick2-dwTick1);

	return bRet;
}

BOOL CFileHelper::DownloadFileFromFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir
												 , CString szUserName, CString szPwd
												 , CString szFtpFileName, CString szLocalFilePath
												 , IFileOperateInterface *pCallBack /*= NULL*/)
{
// 	theLog.Write(_T("CFileHelper::DownloadFileFromFtpServer,1,szFtpServer=%s,nFtpPort=%d,szFtpDir=%s,")
// 		_T("szUserName=%s,szPwd=%s,szFtpFileName=%s,szLocalFilePath=%s")
// 		,szFtpServer, nFtpPort, szFtpDir, szUserName, /*szPwd*/_T("***"), szFtpFileName, szLocalFilePath);
	theLog.Write(_T("CFileHelper::DownloadFileFromFtpServer,1,szFtpDir=%s,pCallBack=%p"),szFtpDir,pCallBack);

	DWORD dwTick1 = GetTickCount();

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	CInternetSession* pSession = NULL;
	CFtpConnection* pConn = NULL;
	CInternetFile* pInternetFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;
	BYTE* pData = NULL;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szLocalFilePath.GetString(), FOT_Download);
	}

	try
	{
		do
		{
			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,2.1,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			pConn = pSession->GetFtpConnection(szFtpServer,szUserName,szPwd,nFtpPort,TRUE);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,2.2,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}

			szFtpDir.Replace(_T("/"), _T("\\"));
			szFtpDir.Trim(_T("\\"));
			szFtpDir = _T("\\") + szFtpDir;
			if (!pConn->SetCurrentDirectory(szFtpDir))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,3,pConn->SetCurrentDirectory fail,err=%d, szFtpDir=%s"), GetLastError(), szFtpDir);
				break;
			}

			CFile file;
			if (!file.Open(szLocalFilePath, CFile::modeCreate|CFile::modeWrite))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,4,file.Open fail,err=%d,szLocalFilePath=%s")
					, GetLastError(), szLocalFilePath);
				break;
			}

			//在打开文件之前，获取文件大小，否则find.FindFile会失败
			CFtpFileFind find(pConn);
			if (!find.FindFile(szFtpFileName))
			{
				find.Close();
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,4.2,find.FindFile fail,err=%d,szFtpFileName=%s")
					, GetLastError(), szFtpFileName);
				break;
			}
			find.FindNextFile();
			dwFileSize = find.GetLength();	//只支持4G大小
			find.Close();

			pInternetFile = pConn->OpenFile(szFtpFileName, GENERIC_READ);
			if (!pInternetFile)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,5,file.OpenFile fail,err=%d,szFtpFileName=%s")
					, GetLastError(), szFtpFileName);
				break;
			}
			pData = new BYTE[DownLoad_File_Unit_Size];
			while (TRUE)
			{
				memset(pData, 0x0, DownLoad_File_Unit_Size);
				int nRead = pInternetFile->Read(pData, DownLoad_File_Unit_Size);
				if (nRead > 0)
				{
					file.Write(pData, nRead);
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szLocalFilePath.GetString(), FOT_Download, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,6,OnFileOperateData,status=%d"), status);
							break;
						}
					}
				}
				else
				{
					break;
				}
			}

			file.Close();

			if (dwFileSize == dwOperateFileSize)
			{
				status = FOS_Ok;
				bRet = TRUE;
			}
			else
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,7,dwFileSize=%u,dwOperateFileSize=%u"), dwFileSize, dwOperateFileSize);
			}
		} while (FALSE);
	}
	catch (CException* e)
	{
		status = FOS_Error;
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::DownloadFileFromFtpServer,8,catch CException,err=%d,tcErrMsg=%s"), GetLastError(), tcErrMsg);
	}

	if (pData)
	{
		delete[] pData;
	}
	if (pInternetFile)
	{
		pInternetFile->Close();
		delete pInternetFile;
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szLocalFilePath.GetString(), FOT_Download, status);
	}

	DWORD dwTick2 = GetTickCount();
	theLog.Write(_T("CFileHelper::DownloadFileFromFtpServer,9,bRet=%d,dwFileSize=%u,耗时=%d(ms)"), bRet, dwFileSize, dwTick2-dwTick1);

	return bRet;
}

BOOL CFileHelper::DeleteFileFromFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd, CString szFtpFileName)
{
	// 	theLog.Write(_T("CFileHelper::DeleteFileFromFtpServer,1,szFtpServer=%s,nFtpPort=%d,szFtpDir=%s,")
	// 		_T("szUserName=%s,szPwd=%s,szFtpFileName=%s,szLocalFilePath=%s")
	// 		,szFtpServer, nFtpPort, szFtpDir, szUserName, /*szPwd*/_T("***"), szFtpFileName, szLocalFilePath);
	theLog.Write(_T("CFileHelper::DeleteFileFromFtpServer,1,szFtpDir=%s"),szFtpDir);

	BOOL bRet = FALSE;
	CInternetSession* pSession = NULL;
	CFtpConnection* pConn = NULL;
	CInternetFile* pInternetFile = NULL;

	try
	{
		do 
		{
			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::DeleteFileFromFtpServer,2.1,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			pConn = pSession->GetFtpConnection(szFtpServer,szUserName,szPwd,nFtpPort,TRUE);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::DeleteFileFromFtpServer,2.2,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}

			szFtpDir.Replace(_T("/"), _T("\\"));
			szFtpDir.Trim(_T("\\"));
			szFtpDir = _T("\\") + szFtpDir;
			if (!pConn->SetCurrentDirectory(szFtpDir))
			{
				theLog.Write(_T("!!CFileHelper::DeleteFileFromFtpServer,3,pConn->SetCurrentDirectory fail,err=%d, szFtpDir=%s"), GetLastError(), szFtpDir);
				break;
			}

			bRet = pConn->Remove(szFtpFileName);
			if (!bRet)
			{
				theLog.Write(_T("!!CFileHelper::DeleteFileFromFtpServer,5,pConn->Remove fail,err=%d,szFtpFileName=%s")
					, GetLastError(), szFtpFileName);
				break;
			}

		} while (FALSE);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::DeleteFileFromFtpServer,6,catch CException,err=%d,tcErrMsg=%s"), GetLastError(), tcErrMsg);
	}

	if (pInternetFile)
	{
		pInternetFile->Close();
		delete pInternetFile;
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}

	theLog.Write(_T("CFileHelper::DeleteFileFromFtpServer,7,bRet=%d"), bRet);

	return bRet;
}

//HTTP文件操作
BOOL CFileHelper::DownloadFileFromHttpServer(CString szHttpServer, int nHttpPort, CString szVirtualDirectory, CString szHttpFilePath
												  , CString szUserName, CString szPwd, CString szLocalFilePath
												  , IFileOperateInterface *pCallBack /*= NULL*/)
{
// 	theLog.Write(_T("CFileHelper::DownloadFileFromHttpServer,1,szHttpServer=%s,nHttpPort=%d,szHttpFilePath=%s,")
// 		_T("szUserName=%s,szPwd=%s,szLocalFilePath=%s"),szHttpServer, nHttpPort, szHttpFilePath, szUserName, szPwd, szLocalFilePath);
	theLog.Write(_T("CFileHelper::DownloadFileFromHttpServer,1,szHttpFilePath=%s,pCallBack=%p"),szHttpFilePath,pCallBack);

	DWORD dwTick1 = GetTickCount();

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	CInternetSession* pSession = NULL;
	CHttpConnection* pConn = NULL;
	CHttpFile* pFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;
	BYTE* pData = NULL;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szLocalFilePath.GetString(), FOT_Download);
	}

	try
	{
		do 
		{
			if (PathFileExists(szLocalFilePath))
			{
				theLog.Write(_T("####CFileHelper::DownloadFileFromHttpServer,2,Path Exists, szLocalFilePath=[%s]"), szLocalFilePath);
				bRet = TRUE;
				break;
			}

			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,3,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			pConn = pSession->GetHttpConnection(szHttpServer, nHttpPort, szUserName, szPwd);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,4,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}
			DWORD dwType = 0;
			INTERNET_PORT wPort = 0;
			CString szServer = _T("");
			CString szObject = _T("");
			CString szURL;
			szURL.Format(_T("http://%s:%d/%s/%s"), szHttpServer, nHttpPort, szVirtualDirectory, szHttpFilePath.TrimLeft(_T("/")));
			if (!AfxParseURL(szURL, dwType, szServer, szObject, wPort))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,5,AfxParseURL fail,err=%d,szURL=%s"), GetLastError(), szURL);
				break;
			}
			pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_GET,szObject);
			if (!pFile)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,6,pConn->OpenRequest fail,err=%d"), GetLastError());
				break;
			}
			if(!pFile->SendRequest())
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,7,pConn->SendRequest fail,err=%d"), GetLastError());
				break;
			}

			DWORD dwStateCode;
			pFile->QueryInfoStatusCode(dwStateCode);
			if(dwStateCode != HTTP_STATUS_OK)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,8,pConn->QueryInfoStatusCode fail,err=%d,dwStateCode=%d")
					, GetLastError(), dwStateCode);
				break;
			}

			CString szFileSize = _T("");
			if (pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szFileSize))
			{
				dwFileSize = (DWORD)_ttoi64(szFileSize);//不超过4G文件大小
			}
			CFile file;
			if (!file.Open(szLocalFilePath, CFile::modeCreate|CFile::modeWrite))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,9,file.Open fail,err=%d,szLocalFilePath=%s")
					, GetLastError(), szLocalFilePath);
				break;
			}
			
			int nRead = 0;
			//不超过4G文件大小
			//如果不调用pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szFileSize)
			//pFile->GetLength()无法获取下载文件大小，调用之后可以获取大小，但是此处已经没有必要调用了。
			//dwFileSize = (DWORD)pFile->GetLength();	
			pData = new BYTE[DownLoad_File_Unit_Size];
			while (TRUE) 
			{
				memset(pData, 0x0, DownLoad_File_Unit_Size);
				nRead = pFile->Read(pData, DownLoad_File_Unit_Size);
				if (nRead > 0)
				{
					file.Write(pData, nRead);
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szLocalFilePath.GetString(), FOT_Upload, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,10,OnFileOperateData,status=%d"), status);
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			file.Close();
			if (dwFileSize == dwOperateFileSize)
			{
				status = FOS_Ok;
				bRet = TRUE;
			}
			else
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,11,dwFileSize=%u,dwOperateFileSize=%u"), dwFileSize, dwOperateFileSize);
			}
		} while (FALSE);
	}
	catch (CException* e)
	{
		status = FOS_Error;
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,12,catch CException,err=%d,tcErrMsg=%s"), GetLastError(), tcErrMsg);
	}

	if (pData)
	{
		delete[] pData;
	}
	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szLocalFilePath.GetString(), FOT_Download, status);
	}

	DWORD dwTick2 = GetTickCount();
	theLog.Write(_T("CFileHelper::DownloadFileFromHttpServer,13,bRet=%d,dwFileSize=%u,耗时=%d(ms)"), bRet, dwFileSize, dwTick2-dwTick1);

	return bRet;
}

BOOL CFileHelper::DownloadFileFromHttpServer(CString szHttpUrl, CString szLocalFilePath, IFileOperateInterface *pCallBack /*= NULL*/)
{
	theLog.Write(_T("CFileHelper::DownloadFileFromHttpServer,1,szHttpUrl=%s,szLocalFilePath=%s,pCallBack=%p"), szHttpUrl, szLocalFilePath, pCallBack);

	DWORD dwTick1 = GetTickCount();

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	CInternetSession* pSession = NULL;
	CHttpConnection* pConn = NULL;
	CHttpFile* pFile = NULL;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;
	BYTE* pData = NULL;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szLocalFilePath.GetString(), FOT_Download);
	}

	try
	{
		do 
		{
			if (PathFileExists(szLocalFilePath))
			{
				theLog.Write(_T("####CFileHelper::DownloadFileFromHttpServer,2,Path Exists, szLocalFilePath=[%s]"), szLocalFilePath);
				bRet = TRUE;
				break;
			}

			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,3,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			DWORD dwType = 0;
			INTERNET_PORT wPort = 0;
			CString szServer = _T("");
			CString szObject = _T("");
			if (!AfxParseURL(szHttpUrl, dwType, szServer, szObject, wPort))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,4,AfxParseURL fail,err=%d,szHttpUrl=%s"), GetLastError(), szHttpUrl);
				break;
			}

			pConn = pSession->GetHttpConnection(szServer, wPort);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,5,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}

			pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_GET,szObject);
			if (!pFile)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,6,pConn->OpenRequest fail,err=%d"), GetLastError());
				break;
			}

			if(!pFile->SendRequest())
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,7,pConn->SendRequest fail,err=%d"), GetLastError());
				break;
			}

			DWORD dwStateCode;
			pFile->QueryInfoStatusCode(dwStateCode);
			if(dwStateCode != HTTP_STATUS_OK)
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,8,pConn->QueryInfoStatusCode fail,err=%d,dwStateCode=%d")
					, GetLastError(), dwStateCode);
				break;
			}

			CString szFileSize = _T("");
			if (pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szFileSize))
			{
				dwFileSize = (DWORD)_ttoi64(szFileSize);//不超过4G文件大小
			}

			CFile file;
			if (!file.Open(szLocalFilePath, CFile::modeCreate|CFile::modeWrite))
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,9,file.Open fail,err=%d,szLocalFilePath=%s")
					, GetLastError(), szLocalFilePath);
				break;
			}

			int nRead = 0;
			//不超过4G文件大小
			//如果不调用pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szFileSize)
			//pFile->GetLength()无法获取下载文件大小，调用之后可以获取大小，但是此处已经没有必要调用了。
			//dwFileSize = (DWORD)pFile->GetLength();	
			pData = new BYTE[DownLoad_File_Unit_Size];
			while (TRUE) 
			{
				memset(pData, 0x0, DownLoad_File_Unit_Size);
				nRead = pFile->Read(pData, DownLoad_File_Unit_Size);
				if (nRead > 0)
				{
					file.Write(pData, nRead);
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szLocalFilePath.GetString(), FOT_Download, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,10,OnFileOperateData,status=%d"), status);
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			file.Close();
			if (dwFileSize == dwOperateFileSize)
			{
				status = FOS_Ok;
				bRet = TRUE;
			}
			else
			{
				theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,11,dwFileSize=%u,dwOperateFileSize=%u"), dwFileSize, dwOperateFileSize);
			}
		} while (FALSE);
	}
	catch (CException* e)
	{
		status = FOS_Error;
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::DownloadFileFromHttpServer,12,catch CException,err=%d,tcErrMsg=%s"), GetLastError(), tcErrMsg);
	}

	if (pData)
	{
		delete[] pData;
	}
	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szLocalFilePath.GetString(), FOT_Download, status);
	}

	DWORD dwTick2 = GetTickCount();
	theLog.Write(_T("CFileHelper::DownloadFileFromHttpServer,13,bRet=%d,dwFileSize=%u,耗时=%d(ms)"), bRet, dwFileSize, dwTick2-dwTick1);

	return bRet;
}

BOOL CFileHelper::UnCompressGzFile(IN CString szFileGzPath, OUT CString& szFileOrgPath, BOOL bDelGzFile, IFileOperateInterface *pCallBack /*= NULL*/)
{
	theLog.Write(_T("CFileHelper::UnCompressGzFile, szFileGzPath=%s, szFileOrgPath=%s, bDelGzFile=%d, pCallBack=%p")
		, szFileGzPath, szFileOrgPath, bDelGzFile, pCallBack);

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szFileGzPath.GetString(), FOT_UnCompress);
	}

	do 
	{
		dwFileSize = CCommonFun::GetFileSizeByte(szFileGzPath);

		CString szSuffix = szFileGzPath.Right(3);
		if (szSuffix.CompareNoCase(_T(".gz")) != 0)
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile,1,szSuffix UnKnow=[%s],szFileGzPath=[%s]"), szSuffix, szFileGzPath);
			break;
		}

		if (!PathFileExists(szFileGzPath))
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile,2,PathFileExists fail,szFileGzPath=[%s]"), szFileGzPath);
			break;
		}

		szFileOrgPath = szFileGzPath;
		szFileOrgPath.MakeLower();
		if (szFileOrgPath.Find(_T(".gz"))>0)
		{
			szFileOrgPath = szFileOrgPath.Left(szFileOrgPath.GetLength()-3);
		}

		CFile fileOrg;
		if (!fileOrg.Open(szFileOrgPath, CFile::modeReadWrite|CFile::modeCreate))
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile,3,fileOrg.Open fail,err=%u,szFileOrgPath=[%s]"), GetLastError(), szFileOrgPath);
			break;
		}

#ifdef UNICODE
		gzFile GzFile = gzopen(CCommonFun::UnicodeToUTF8(szFileGzPath), "rb");
#else
		gzFile GzFile = gzopen(CCommonFun::MultiByteToUTF8(szFileGzPath), "rb");
#endif // UNICODE
		if(GzFile == NULL)
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile,4,gzopen fail,GzFile=%p szFileGzPath=[%s]"), GzFile, szFileGzPath);
			break;
		}

		int nRead = 0;
		BYTE *pBuf = new BYTE[UnCompress_File_Unit_Size];
		BOOL bSucc = TRUE;
		while(!gzeof(GzFile))
		{
			memset(pBuf, 0x0, UnCompress_File_Unit_Size);
			nRead = gzread(GzFile, pBuf, UnCompress_File_Unit_Size);
			if (nRead>0)
			{
				fileOrg.Write(pBuf, nRead);
				dwOperateFileSize = gztell(GzFile);	//前档案位置(解压缩后的位置)  
				if (pCallBack)
				{
					status = pCallBack->OnFileOperateData(szFileGzPath.GetString(), FOT_UnCompress, dwFileSize, dwOperateFileSize);
					if (status != FOS_Ok)
					{
						theLog.Write(_T("!!CFileHelper::CompressGzFile,5,OnFileOperateData,status=%d"), status);
						bSucc = FALSE;
						break;
					}
				}
			}
			else if (nRead<0)
			{
				theLog.Write(_T("!!CFileHelper::CompressGzFile,5,gzread fail,nRead=%d"), nRead);
				bSucc = FALSE;
				break;
			}
			else
			{
				break;
			}
		}
		fileOrg.Close();
		gzclose(GzFile);
		delete[] pBuf;

		if (!bSucc)
		{
			DeleteFile(szFileOrgPath);
		}
		else
		{
			status = FOS_Ok;
		}

		if (bDelGzFile)
		{
			if (!DeleteFile(szFileGzPath))
			{
				theLog.Write(_T("!!CFileHelper::UnCompressGzFile,6,DeleteFile fail,szFileGzPath=[%s]"), szFileGzPath);
			}
		}
		bRet = bSucc;
	} while (FALSE);

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szFileGzPath.GetString(), FOT_UnCompress, status);
	}

	return bRet;
}

BOOL CFileHelper::CompressGzFile(IN CString szFileOrgPath, OUT CString& szFileGzPath, BOOL bDelOrgzFile, IFileOperateInterface *pCallBack /*= NULL*/)
{
	theLog.Write(_T("CFileHelper::CompressGzFile, szFileOrgPath=%s, szFileGzPath=%s, bDelOrgzFile=%d, pCallBack=%p")
		, szFileOrgPath, szFileGzPath, bDelOrgzFile, pCallBack);

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szFileOrgPath.GetString(), FOT_Compress);
	}

	do 
	{
		dwFileSize = CCommonFun::GetFileSizeByte(szFileOrgPath);

		if (!PathFileExists(szFileOrgPath))
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile,1,PathFileExists fail,szFileOrg=[%s]"), szFileOrgPath);
			break;
		}

		szFileGzPath = szFileOrgPath + _T(".gz");

		CFile fileOrg;
		if (!fileOrg.Open(szFileOrgPath, CFile::modeRead))
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile,2,szFileGzPath.Open fail,err=%u,szFileOrgPath=[%s]"), GetLastError(), szFileOrgPath);
			break;
		}

#ifdef UNICODE
		gzFile GzFile = gzopen(CCommonFun::UnicodeToUTF8(szFileGzPath), "wb");
#else
		gzFile GzFile = gzopen(CCommonFun::MultiByteToUTF8(szFileGzPath), "wb");
#endif // UNICODE
		if(GzFile == NULL)
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile,3,gzopen fail,GzFile=%p szFileGzPath=[%s]"), GzFile, szFileGzPath);
			break;
		}

		int nRead = 0;
		int nWrite = 0;
		BYTE *pBuf = new BYTE[Compress_File_Unit_Size];
		BOOL bSucc = TRUE;
		do 
		{
			memset(pBuf, 0x0, Compress_File_Unit_Size);
			nRead = fileOrg.Read(pBuf, Compress_File_Unit_Size);
			if (nRead>0)
			{
				nWrite = gzwrite(GzFile, pBuf, nRead);
				if(nWrite != nRead)
				{
					theLog.Write(_T("!!CFileHelper::CompressGzFile,4,gzwrite fail,nWrite=%d,nRead=%d"), nWrite, nRead);
					bSucc = FALSE;
					break;
				}
				else
				{
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szFileOrgPath.GetString(), FOT_Compress, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::CompressGzFile,5,OnFileOperateData,status=%d"), status);
							bSucc = FALSE;
							break;
						}
					}
				}
			}
			else
			{
				break;
			}		
		} while (nRead>0);
		fileOrg.Close();
		gzclose(GzFile);
		delete[] pBuf;

		if (!bSucc)
		{
			DeleteFile(szFileGzPath);
		}
		else
		{
			status = FOS_Ok;
		}

		if (bDelOrgzFile)
		{
			if (!DeleteFile(szFileOrgPath))
			{
				theLog.Write(_T("!!CFileHelper::CompressGzFile,6,DeleteFile fail,szFileGzPath=[%s]"), szFileGzPath);
			}
		}
		bRet = bSucc;
	} while (FALSE);

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szFileOrgPath.GetString(), FOT_Compress, status);
	}

	return bRet;
}

BOOL CFileHelper::UnCompressZipFile(IN CString szZipFilePath, IN CString szOrgFilePath, BOOL bDelZipFile)
{
	theLog.Write(_T("CFileHelper::UnCompressZipFile,1,szZipFilePath=%s,szOrgFilePath=%s,bDelZipFile=%d")
		, szZipFilePath, szOrgFilePath, bDelZipFile);
	BOOL bRet = FALSE;
	bRet = ZipExtract(szZipFilePath, szOrgFilePath);
	if (bDelZipFile)
	{
		if (!DeleteFile(szZipFilePath))
		{
			theLog.Write(_T("!!CFileHelper::UnCompressZipFile,2,DeleteFile fail,UnCompressZipFile=[%s]"), UnCompressZipFile);
		}
	}
	return bRet;
}

BOOL CFileHelper::CompressZipFile(IN CString szOrgFilePath, IN CString szZipFilePath, BOOL bDelOrgFile)
{
	theLog.Write(_T("CFileHelper::CompressZipFile,1,szOrgFilePath=%s,szZipFilePath=%s,bDelOrgFile=%d")
		, szOrgFilePath, szZipFilePath, bDelOrgFile);
	BOOL bRet = FALSE;
	bRet = ZipCompress(szOrgFilePath, szZipFilePath);
	if (bDelOrgFile)
	{
		if (!DeleteFile(szOrgFilePath))
		{
			theLog.Write(_T("!!CFileHelper::CompressZipFile,2,DeleteFile fail,UnCompressZipFile=[%s]"), UnCompressZipFile);
		}
	}
	return bRet;

}

BOOL CFileHelper::FileToBase64(IN CString szFilePath, OUT CString& szBase64)
{
	BOOL bRet = FALSE;
	char* pBase64 = NULL;
	unsigned char* pFile = NULL;
	do 
	{
		CFile file;
		if (file.Open(szFilePath, CFile::modeRead))
		{
			int nFileLen = (int)file.GetLength();	//最大支持2G文件
			if (nFileLen <= 0)
			{
				theLog.Write(_T("!!CFileHelper::FileToBase64, nFileLen=%d, szFilePath=%s"), nFileLen, szFilePath);
				break;
			}
			pFile = new unsigned char[nFileLen+1];
			memset(pFile, 0x0, nFileLen+1);
			int nRead = file.Read(pFile, nFileLen);
			if (nRead != nFileLen)
			{
				theLog.Write(_T("!!CFileHelper::FileToBase64, nFileLen=%d, nRead=%d"), nFileLen, nRead);
				break;
			}

			int nEncodeSize = CBase64::Base64EncodeSize(nFileLen);
			pBase64 = new char[nEncodeSize+1];
			memset(pBase64, 0x0, nEncodeSize+1);
			CBase64::EncodeBase64(pFile, pBase64, nFileLen);
			szBase64 = pBase64;

			file.Close();
			bRet = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CFileHelper::FileToBase64,Open file err=%u, szFilePath=%s")
				, GetLastError(), szFilePath);
		}	
	} while (FALSE);

	if (pBase64)
	{
		delete[] pBase64;
	}
	if (pFile)
	{
		delete[] pFile;
	}
	return bRet;
}

BOOL CFileHelper::Base64ToFile(IN CString& szBase64, IN CString szFilePath)
{
	BOOL bRet = FALSE;
	char* pBase64 = NULL;
	unsigned char* pFile = NULL;
	do 
	{
		int nLen = szBase64.GetLength();
		if (nLen < 4)
		{
			theLog.Write(_T("!!CFileHelper::Base64ToFile, nLen=%d, szBase64=%s"), nLen, szBase64);
			break;
		}

		pBase64 = new char[nLen+1];
		memset(pBase64, 0x0, nLen+1);
#ifdef UNICODE
		strcpy_s(pBase64, nLen + 1, CCommonFun::UnicodeToMultiByte(szBase64).GetString());
#else
		strcpy_s(pBase64, nLen + 1, szBase64.GetString());
#endif // UNICODE
		int nDecodeSize = CBase64::Base64DecodeSize(pBase64);
		pFile = new unsigned char[nDecodeSize+1];
		memset(pFile, 0x0, nDecodeSize+1);
		CBase64::DecodeBase64(pBase64, pFile, nLen);
		CFile file;
		if (file.Open(szFilePath, CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(pFile, nDecodeSize);
			file.Close();
			bRet = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CFileHelper::FileToBase64,Open file err=%u, szFilePath=%s")
				, GetLastError(), szFilePath);
			break;
		}	
	} while (FALSE);

	if (pBase64)
	{
		delete[] pBase64;
	}
	if (pFile)
	{
		delete[] pFile;
	}
	return bRet;
}

BOOL CFileHelper::CreateFtpDir( CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd )
{
	BOOL bRet = FALSE;
	CInternetSession* pSession = NULL;
	CFtpConnection* pConn = NULL;

	try
	{
		do 
		{
			CString strAppName = AfxGetAppName();
			pSession = new CInternetSession(strAppName);
			if (!pSession)
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,2.1,pSession=%p,err=%d"), pSession, GetLastError());
				break;
			}

			//设置网络超时参数
			SetTimeOut(pSession);

			pConn = pSession->GetFtpConnection(szFtpServer,szUserName,szPwd,nFtpPort,TRUE);
			if (!pConn)
			{
				theLog.Write(_T("!!CFileHelper::UploadFileToFtpServer,2.2,pSession->GetFtpConnection fail,err=%d"), GetLastError());
				break;
			}
			bRet = CreateAndSetFtpDir(pConn, szFtpDir);
		} while (FALSE);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		e->Delete();
		theLog.Write(_T("!!CFileHelper::CreateFtpDir,3,catch CException,err=%d,tcErrMsg=%s,szFtpDir=%s"), GetLastError(), tcErrMsg, szFtpDir);
	}
	if (pConn)
	{
		pConn->Close();
		delete pConn;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}
	return bRet;
}

BOOL CFileHelper::CompressGzFile2(IN CString szFileOrgPath, OUT CString& szFileGzPath, BOOL bDelOrgzFile, IFileOperateInterface *pCallBack /*= NULL*/)
{
	theLog.Write(_T("CFileHelper::CompressGzFile2, szFileOrgPath=%s, szFileGzPath=%s, bDelOrgzFile=%d, pCallBack=%p")
		, szFileOrgPath, szFileGzPath, bDelOrgzFile, pCallBack);

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szFileOrgPath.GetString(), FOT_Compress);
	}

	do 
	{
		dwFileSize = CCommonFun::GetFileSizeByte(szFileOrgPath);

		if (!PathFileExists(szFileOrgPath))
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile2,1,PathFileExists fail,szFileOrg=[%s]"), szFileOrgPath);
			break;
		}

		CFile fileOrg;
		if (!fileOrg.Open(szFileOrgPath, CFile::modeRead))
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile2,2,szFileGzPath.Open fail,err=%u,szFileOrgPath=[%s]"), GetLastError(), szFileOrgPath);
			break;
		}

#ifdef UNICODE
		gzFile GzFile = gzopen(CCommonFun::UnicodeToUTF8(szFileGzPath), "wb");
#else
		gzFile GzFile = gzopen(CCommonFun::MultiByteToUTF8(szFileGzPath), "wb");
#endif // UNICODE
		if(GzFile == NULL)
		{
			theLog.Write(_T("!!CFileHelper::CompressGzFile2,3,gzopen fail,GzFile=%p szFileGzPath=[%s]"), GzFile, szFileGzPath);
			break;
		}

		int nRead = 0;
		int nWrite = 0;
		BYTE *pBuf = new BYTE[Compress_File_Unit_Size];
		BOOL bSucc = TRUE;
		do 
		{
			memset(pBuf, 0x0, Compress_File_Unit_Size);
			nRead = fileOrg.Read(pBuf, Compress_File_Unit_Size);
			if (nRead>0)
			{
				nWrite = gzwrite(GzFile, pBuf, nRead);
				if(nWrite != nRead)
				{
					theLog.Write(_T("!!CFileHelper::CompressGzFile2,4,gzwrite fail,nWrite=%d,nRead=%d"), nWrite, nRead);
					bSucc = FALSE;
					break;
				}
				else
				{
					dwOperateFileSize += nRead;
					if (pCallBack)
					{
						status = pCallBack->OnFileOperateData(szFileOrgPath.GetString(), FOT_Compress, dwFileSize, dwOperateFileSize);
						if (status != FOS_Ok)
						{
							theLog.Write(_T("!!CFileHelper::CompressGzFile2,5,OnFileOperateData,status=%d"), status);
							bSucc = FALSE;
							break;
						}
					}
				}
			}
			else
			{
				break;
			}		
		} while (nRead>0);
		fileOrg.Close();
		gzclose(GzFile);
		delete[] pBuf;

		if (!bSucc)
		{
			DeleteFile(szFileGzPath);
		}
		else
		{
			status = FOS_Ok;
		}

		if (bDelOrgzFile)
		{
			if (!DeleteFile(szFileOrgPath))
			{
				theLog.Write(_T("!!CFileHelper::CompressGzFile2,6,DeleteFile fail,szFileGzPath=[%s]"), szFileGzPath);
			}
		}
		bRet = bSucc;
	} while (FALSE);

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szFileOrgPath.GetString(), FOT_Compress, status);
	}

	return bRet;
}

BOOL CFileHelper::UnCompressGzFile2( IN CString szFileGzPath, IN CString& szFileOrgPath, BOOL bDelGzFile, IFileOperateInterface *pCallBack /*= NULL*/ )
{
	theLog.Write(_T("CFileHelper::UnCompressGzFile2, szFileGzPath=%s, szFileOrgPath=%s, bDelGzFile=%d, pCallBack=%p")
		, szFileGzPath, szFileOrgPath, bDelGzFile, pCallBack);

	BOOL bRet = FALSE;
	FileOperateStatus status = FOS_Error;
	DWORD dwFileSize = 0;
	DWORD dwOperateFileSize = 0;

	if (pCallBack)
	{
		pCallBack->OnFileOperateBegin(szFileGzPath.GetString(), FOT_UnCompress);
	}

	do 
	{
		dwFileSize = CCommonFun::GetFileSizeByte(szFileGzPath);

		CString szSuffix = szFileGzPath.Right(3);
		if (szSuffix.CompareNoCase(_T(".gz")) != 0)
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,1,szSuffix UnKnow=[%s],szFileGzPath=[%s]"), szSuffix, szFileGzPath);
			break;
		}

		if (!PathFileExists(szFileGzPath))
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,2,PathFileExists fail,szFileGzPath=[%s]"), szFileGzPath);
			break;
		}

		/*szFileOrgPath = szFileGzPath;
		szFileOrgPath.MakeLower();
		if (szFileOrgPath.Find(_T(".gz"))>0)
		{
			szFileOrgPath = szFileOrgPath.Left(szFileOrgPath.GetLength()-3);
		}*/

		CFile fileOrg;
		if (!fileOrg.Open(szFileOrgPath, CFile::modeReadWrite|CFile::modeCreate))
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,3,fileOrg.Open fail,err=%u,szFileOrgPath=[%s]"), GetLastError(), szFileOrgPath);
			break;
		}

#ifdef UNICODE
		gzFile GzFile = gzopen(CCommonFun::UnicodeToUTF8(szFileGzPath), "rb");
#else
		gzFile GzFile = gzopen(CCommonFun::MultiByteToUTF8(szFileGzPath), "rb");
#endif // UNICODE
		if(GzFile == NULL)
		{
			theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,4,gzopen fail,GzFile=%p szFileGzPath=[%s]"), GzFile, szFileGzPath);
			break;
		}

		int nRead = 0;
		BYTE *pBuf = new BYTE[UnCompress_File_Unit_Size];
		BOOL bSucc = TRUE;
		while(!gzeof(GzFile))
		{
			memset(pBuf, 0x0, UnCompress_File_Unit_Size);
			nRead = gzread(GzFile, pBuf, UnCompress_File_Unit_Size);
			if (nRead>0)
			{
				fileOrg.Write(pBuf, nRead);
				dwOperateFileSize = gztell(GzFile);	//前档案位置(解压缩后的位置)  
				if (pCallBack)
				{
					status = pCallBack->OnFileOperateData(szFileGzPath.GetString(), FOT_UnCompress, dwFileSize, dwOperateFileSize);
					if (status != FOS_Ok)
					{
						theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,5,OnFileOperateData,status=%d"), status);
						bSucc = FALSE;
						break;
					}
				}
			}
			else if (nRead<0)
			{
				theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,5,gzread fail,nRead=%d"), nRead);
				bSucc = FALSE;
				break;
			}
			else
			{
				break;
			}
		}
		fileOrg.Close();
		gzclose(GzFile);
		delete[] pBuf;

		if (!bSucc)
		{
			DeleteFile(szFileOrgPath);
		}
		else
		{
			status = FOS_Ok;
		}

		if (bDelGzFile)
		{
			if (!DeleteFile(szFileGzPath))
			{
				theLog.Write(_T("!!CFileHelper::UnCompressGzFile2,6,DeleteFile fail,szFileGzPath=[%s]"), szFileGzPath);
			}
		}
		bRet = bSucc;
	} while (FALSE);

	if (pCallBack)
	{
		pCallBack->OnFileOperateEnd(szFileGzPath.GetString(), FOT_UnCompress, status);
	}

	return bRet;
}