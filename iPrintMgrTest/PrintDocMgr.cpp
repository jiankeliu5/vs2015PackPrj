#include "stdafx.h"
#include "PrintDocMgr.h"

CPrintDocMgr::CPrintDocMgr()
{
	m_nDatalen = 0;
	m_pBuf = 0;
	m_pCallback = 0;
	m_pPipeSvr = 0;
	m_hModule = 0;
	m_pfInstall = 0;
	m_pfStop = 0;
	memset(m_szDocType,sizeof(m_szDocType),0);
}

CPrintDocMgr::~CPrintDocMgr()
{
	Shutdown();
}

void CPrintDocMgr::SetDocType(const char* szType)
{
	strcpy_s(m_szDocType,sizeof(m_szDocType),szType);
}

void CPrintDocMgr::Start()
{
	if (!m_hModule)
	{
		m_hModule =LoadLibrary((LPCTSTR)"HookProc.dll");
		theLog.Write("LoadLibrary(hookproc) %d,%X",::GetLastError(),m_hModule);
		
	}
	if (m_hModule)
	{
		m_pfInstall =(PFINSHOOK)GetProcAddress(m_hModule, "StartHook");
		char szTxt[] = {"txt|doc|pdf|docx|xls|ppt|pptx"};
		if (strlen(m_szDocType) == 0)
		{
			SetDocType(szTxt);
		}
		if(m_pfInstall)
			m_pfInstall(m_szDocType);
	}
	
}

void CPrintDocMgr::Stop()
{
	if (m_hModule)
	{
		m_pfStop = (PFSTOPHOOK)GetProcAddress(m_hModule, "StopHook");
		if (m_pfStop)
		{
			m_pfStop();
		}
		::FreeLibrary(m_hModule);
		m_hModule = 0;
	}
}
void CPrintDocMgr::CheckBufSize(int nBufSize)
{
	if(nBufSize > m_nDatalen)
	{
		if(m_pBuf)
		{
			delete m_pBuf;
			m_pBuf = NULL;
		}
		m_pBuf = new BYTE[nBufSize];
	}
	m_nDatalen = nBufSize;
	memset(m_pBuf,0,m_nDatalen);
}
BOOL CPrintDocMgr::GetDocument(CString sPath,CString& sZipPath,int& nSize)
{	
	CString sDestPath;
	sDestPath.Format(_T("%s%s\\%s%s"),CCommonFun::GetDefaultPath(),PRT_CLIENT_JOB_DIR,CCommonFun::NewGuid(),PathFindExtension(sPath));
	CCommonFun::ValidatePath(sDestPath);
	theLog.Write("tiff = %s,dest = %s",sPath,sDestPath);
	if(!PathFileExists(sPath))
	{
		theLog.Write("GetDocument %s not exist",sPath);
	}
	if(CopyFile(sPath,sDestPath,FALSE) && PathFileExists(sDestPath))
	{
		//----------------------------
		//获取文档的大小
		CFileStatus st;
		CFile::GetStatus(sDestPath,st);
		nSize = st.m_size;
		//压缩
		char* pPath = sDestPath.GetBuffer();
		file_compress(pPath,-1);
		sDestPath.ReleaseBuffer();
		sZipPath.Format("%s.gz",sDestPath);
		return TRUE;
	}
	return FALSE;
}
void CPrintDocMgr::OnDocumentPrinted(CDocumentLogNode* pNode)
{
	CString sZipPath;
	int nSize = 0;
	if(GetDocument(pNode->m_szPath,sZipPath,nSize))
	{
		CFileStatus st;
		CFile::GetStatus(sZipPath,st);

		PrintDocumentInfo log;
		//OnSetEquipLogonInfo(&log);
		//log.set_time(GetNowDateTime());
		log.set_doc_name(pNode->m_szDocName);
		log.set_path(sZipPath);
		log.set_name(PathFindFileName(sZipPath));
		log.set_orig_path(pNode->m_szPath);
	
		log.set_doc_size(nSize);
		log.set_zip_size(st.m_size);

		log.set_id(::GetTickCount());
		int nDataLen = log.ByteSize();
		CheckBufSize(nDataLen);
		log.SerializePartialToArray(m_pBuf,nDataLen);
		BOOL bSuc = UploadPrintDocLog(log,m_pBuf,nDataLen);
		if(!bSuc)
		{
			theLog.Write("日志上传失败！");
			CSQliteDataSvc::GetInstance().InsertLog(PRINT_DOC_LOG,m_pBuf,nDataLen);
		}
	}
}
//上传日志或资产信息
BOOL CPrintDocMgr::OnUpload(int nCmd,int nSub,BYTE* pData,int nDataLen,int nCompress /*= -1*/)
{
	int nRet = 0;
	if(m_pCallback)
	{
// 		nRet = m_pCallback->SendByNetHelper(nCmd,nSub,pData,nDataLen,nCompress);
	}
	return nRet > 0 ;
}

BOOL CPrintDocMgr::UploadPrintDocLog(PrintDocumentInfo& log,BYTE* pData,int nDataLen)
{
	BOOL bSuc = FALSE;
	CString sPath;
	static long lScreenIndex = 0;
	long nId = InterlockedIncrement(&lScreenIndex);
	if(/*IsUploadActOnce(bChecked)*/1)
	{
		if(OnUpload(WM_UPLOAD_DOC_PRINT,nId,pData,nDataLen))
		{
			BYTE* pBuf = NULL;
			CFile file;
			try
			{
				sPath.Format("%s",log.path().c_str());
				if (file.Open(sPath,CFile::modeRead))
				{
					int nBufLen = 2048; 
					pBuf = new BYTE[nBufLen];
					ASSERT(pBuf);
					BOOL bUploadSuc = TRUE;
					if (pBuf)
					{
						while (1)
						{
							int nRead = file.Read(pBuf,nBufLen);
							if(nRead > 0)
							{
								int n = OnUpload(WM_UPLOAD_DOC_PRINT_DATA,nId,pBuf,nRead,0);
								if(n < 1)
								{
									bUploadSuc = FALSE;
									ASSERT(0);
									break;
								}
							}
							else
							{
								//ASSERT(0);
								break;
							}
						}
					}
					file.Close();
					if(bUploadSuc)
					{
						bSuc = OnUpload(WM_UPLOAD_DOC_PRINT_CMPLT,nId,0,0) > 0 ? TRUE : FALSE;
						DeleteFileA(sPath);
					}
				}
			}
			catch (CFileException* e)
			{
				ASSERT(0);
			}
			if (pBuf)
			{
				delete pBuf;
				pBuf = NULL;
			}
		}
	}
	return bSuc;
}

void CPrintDocMgr::OnPipeRcv(void*,BYTE* pData,int nSize)
{
	IST_PIPE_STRUCT* pInfo = (IST_PIPE_STRUCT*)pData;
	switch (pInfo->message)
	{
		//连接成功,请求信息
	case WM_PRINT_DOC_EVENT:
		{
			CDocumentLogNode* pNode = (CDocumentLogNode*)(pInfo + 1);
			if (pNode)
			{
				OnDocumentPrinted(pNode);	
			}

		}
		break;
	}
}
int CPrintDocMgr::OnNewPipe(void* )
{
	ASSERT(0);
	return 0;
}

void CPrintDocMgr::OnPipeClose(void*)
{
	ASSERT(0);
}

void CPrintDocMgr::Shutdown()
{
	if (m_hModule)
	{
		m_pfStop = (PFSTOPHOOK)GetProcAddress(m_hModule, "StopHook");
		if (m_pfStop)
		{
			m_pfStop();
		}
		::FreeLibrary(m_hModule);
		m_hModule = 0;
	}
	if (m_pPipeSvr)
	{
		CPipeSvrThread::End(m_pPipeSvr);
		m_pPipeSvr = 0;
	}
	if (m_pBuf)
	{
		delete m_pBuf;
		m_pBuf = 0;
	}
}

CPrintDocMgr& CPrintDocMgr::GetInstance()
{
	static CPrintDocMgr theOne;
	return theOne;
}

void CPrintDocMgr::Init(IModuleCallback* pCall)
{
	ASSERT(pCall);
	m_pCallback = pCall;
	if(!m_pPipeSvr)
	{
		m_pPipeSvr = CPipeSvrThread::Create(ISEC_HOOK_PRINT_PIPE,this);
		ASSERT(m_pPipeSvr);
		if (!m_pPipeSvr)
		{
			theLog.Write("err 客户端创建管道失败。");
			return ;
		}
	}
}
