#include "StdAfx.h"
#include "PrintJobMgr.h"
#include "XabPrinterMonitor.h"

CPrintJobMgr::CPrintJobMgr(void)
{
	m_nDatalen = 0;
	m_pBuf = 0;
	m_pCallback = 0;
	m_pPrinterMon = 0;
}

CPrintJobMgr::~CPrintJobMgr(void)
{
	Release();
}

void CPrintJobMgr::Release()
{

	
	if(m_pBuf)
	{
		delete m_pBuf;
		m_pBuf = NULL;
	}
}
void CPrintJobMgr::StartPrinter()
{
	if (m_pPrinterMon)
	{
		return ;
	}
	//打印监视
	m_pPrinterMon = new CXabPrinterMonitor();
	ASSERT(m_pPrinterMon);
	if(m_pPrinterMon)
	{
		m_pPrinterMon->SetCallback(this);
		m_pPrinterMon->StartMonitor();
	}		
}

void CPrintJobMgr::SetCallback(IModuleCallback* pCall)
{
	ASSERT(pCall);
	m_pCallback = pCall;
}

CPrintJobMgr& CPrintJobMgr::GetInstance()
{
	static CPrintJobMgr theOne;
	return theOne;
}

//立即上传
BOOL CPrintJobMgr::IsUploadActOnce()
{
	return TRUE;
}

double CPrintJobMgr::GetNowDateTime()
{
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);
	COleDateTime dt(st);
	return dt.m_dt;
}

BOOL CPrintJobMgr::UploadPrintLog(SubmitPrintJob& log)
{
	BOOL bSuc = FALSE;
	CString sPath;
	static long lJobIndex = 0;
	long nId = InterlockedIncrement(&lJobIndex);
	if(IsUploadActOnce())
	{
		int nDataLen = log.ByteSize();
		theLog.Write("UploadPrintLog = %d",nDataLen);
		CheckBufSize(nDataLen);
		log.SerializeToArray(m_pBuf,nDataLen);

		if(OnUpload(WM_SUBMIT_PRINT_JOB,nId,m_pBuf,nDataLen))
		{
			BYTE* pBuf = NULL;
			CFile file;
			try
			{
				sPath = log.job_path().c_str();
				theLog.Write("UploadPrintLog upload = %s",sPath);
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
								int n = OnUpload(WM_PRINT_FILE_DATA,nId,pBuf,nRead,0);
								if(n < 1)
								{
									bUploadSuc = FALSE;
									ASSERT(0);
									theLog.Write("print file WM_PRINT_FILE_DATA err = %d",n);
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
						bSuc = OnUpload(WM_PRINT_FILE_END,nId,0,0) > 0 ? TRUE : FALSE;
					}
					else
					{
						ASSERT(0);
					}
				}
				else
				{
					theLog.Write("% print file not exist",sPath);
					bSuc = OnUpload(WM_PRINT_FILE_END,nId,0,0) > 0 ? TRUE : FALSE;
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
		else
		{
			ASSERT(0);
		}
	}
	return bSuc;
}

//打印日志
void CPrintJobMgr::OnPrinted(void* pVoid)
{
	theLog.Write("打印日志");

	ASSERT(pVoid);
	if(!pVoid)
		return;
	CCriticalSection2::Owner ow(m_cs);
	SubmitPrintJob* pJob = (SubmitPrintJob*)pVoid;
	int nDataLen = pJob->ByteSize();
	CheckBufSize(nDataLen);
	pJob->SerializePartialToArray(m_pBuf,nDataLen);

	//保存到本地数据库
	int nId = 0;
	if(!CSQliteDataSvc::GetInstance().InsertLog(PRINT_LOG,m_pBuf,nDataLen,nId))
	{
		ASSERT(0);
		theLog.Write("OnPrinted InsertLog errr");
		return ;
	}
	if(m_pCallback)
	{
		if(!m_pCallback->OnRequestSubmitPrintJob(nId))
		{
			//通知ui上传结果
			m_pCallback->OnUploadLogResult(PRINT_LOG,0,pJob);
		}
	}
	else
	{
		ASSERT(0);
		theLog.Write("OnPrinted m_pCallback = 0");
	}
}

//上传日志或资产信息
BOOL CPrintJobMgr::OnUpload(int nCmd,int nSub,BYTE* pData,int nDataLen,int nCompress /*= -1*/)
{
	int nRet = 0;
	if(m_pCallback)
	{
// 		nRet = m_pCallback->SendByNetHelper(nCmd,nSub,pData,nDataLen,nCompress);
	}
	return nRet > 0 ;
}

void CPrintJobMgr::CheckBufSize(int nBufSize)
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

//获取缓冲指针
BYTE* CPrintJobMgr::GetBufPtr()
{
	ASSERT(m_pBuf);
	return m_pBuf;
}

int CPrintJobMgr::OnLogUpload(int id,int type,const void* pData,int nSize,void* context)
{
	CCriticalSection2::Owner ow(m_cs);
	int nUploadSuc = 0;
	LOG_TYPE eType = (LOG_TYPE)type;
	if (eType == PRINT_LOG)
	{
		SubmitPrintJob log;
		if(log.ParsePartialFromArray(pData,nSize))
		{
			if (context)
			{
				PrintCltCheckUserResult* res = (PrintCltCheckUserResult*)context;
				if (id != res->job_id())
				{
					theLog.Write("not the request job(%d),current job (%d) ",res->job_id(),id);
					ASSERT(0);
					return FALSE;
				}

				log.set_direct_prtstation_id(res->print_station_id());
				log.set_order_prtstation_id(res->order_station_id());
				log.set_project_id(res->project_id());
				log.set_user_id(res->user_id());		
			}
			else if (m_pCallback)
			{
				m_pCallback->OnUploadLog(type,&log);
			}
			log.set_job_id(id);
			nUploadSuc = UploadPrintLog(log);

			//通知ui上传结果
			if (m_pCallback)
			{
				m_pCallback->OnUploadLogResult(type,nUploadSuc,&log);
			}
		}
	}
	else
		nUploadSuc = OnUpload(WM_UPLOAD_LOG,type,(BYTE*)pData,nSize);
	return nUploadSuc;
}
//本地提示
void CPrintJobMgr::OnTipMsg(int type,CString s ,DWORD w,DWORD w2)
{
	if (m_pCallback)
	{
		m_pCallback->OnTipMsg(type,s,w,w2);
	}
}