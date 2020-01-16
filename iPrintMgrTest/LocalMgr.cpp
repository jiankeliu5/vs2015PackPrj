#include "stdafx.h"
//#include "iCltprint.h"
#include "LocalMgr.h"
//#include "NetThHelper.h"
#include "NetThread.h"
#include "PrintDocMgr.h"

CLocalThreadMgr::CLocalThreadMgr()
{
	m_pMainTh = 0;
	m_pHelperTh = 0;
	m_pSvcCallback = 0;
	gdiplusToken = 0;
}

CLocalThreadMgr& CLocalThreadMgr::GetInstance()
{
	static CLocalThreadMgr one;
	return one;
}

BOOL CLocalThreadMgr::InitTh()
{
	CCriticalSection2::Owner ow(m_cs);
	//��ʼ��gdi++
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//��ʼ�����
	CPrintJobMgr::GetInstance().SetCallback(&CLocalThreadMgr::GetInstance().GetInstance());
	CPrintDocMgr::GetInstance().Init(&CLocalThreadMgr::GetInstance().GetInstance());
	
	//��ʼ�����ݿ�
	CString sPath ;
	sPath.Format("%s%s",CCommonFun::GetDefaultPath(),ISEC_LOCAL_DB_NAME);
	CSQliteDataSvc::GetInstance().InitConnt(sPath.GetBuffer());
	sPath.ReleaseBuffer();
	
	InterlockedExchange(&m_nCnntState,2);

	if(!m_pHelperTh)
	{
		m_pHelperTh = (CNetThHelper*)::AfxBeginThread(RUNTIME_CLASS(CNetThHelper),THREAD_PRIORITY_NORMAL,0);
	}
	if(!m_pMainTh)
	{
		m_pMainTh = (CNetThread*)::AfxBeginThread(RUNTIME_CLASS(CNetThread),THREAD_PRIORITY_NORMAL,0);
		if (m_pMainTh)
		{
			m_pMainTh->SetMainCallback(this);
		}
	}

	//
	if (m_pSvcCallback)
	{
		m_pSvcCallback->SetCallback(this);
	}

	return m_pMainTh && m_pHelperTh;
}

void CLocalThreadMgr::EndTh()
{
	CCriticalSection2::Owner ow(m_cs);
	CWinThread* pTh = m_pHelperTh;
	ASSERT(pTh);
	//��Ҫ������ʱ����
	if (pTh && pTh->m_hThread)
	{
		CThreadMgr::DestroyThread(pTh,INFINITE,WM_THREAD_EXIT);
		m_pHelperTh = 0;
	}

	pTh = m_pMainTh;
	ASSERT(pTh);
	//��Ҫ������ʱ����
	if (pTh && pTh->m_hThread)
	{
		CThreadMgr::DestroyThread(pTh,INFINITE,WM_THREAD_EXIT);
		m_pMainTh =0;
	}

	if (m_pSvcCallback)
	{
		m_pSvcCallback->SetCallback(0);
		m_pSvcCallback = 0;
	}
	CPrintDocMgr::GetInstance().Shutdown();
	CSQliteDataSvc::GetInstance().Release();
	if(gdiplusToken)
	{
		GdiplusShutdown(gdiplusToken);
		gdiplusToken = 0;
	}
}
//���糬ʱ
void CLocalThreadMgr::OnSocketTimeOut()
{
	NotifyNetHelperTh(WM_SOCKET_ON_CLOSE,0,0);

}
//ͨ�����������̷߳�������
int CLocalThreadMgr::SendByNetHelper(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress /*= -1*/ )
{
	if(m_pHelperTh && m_pHelperTh->IsConnected())
	{
		int n = m_pHelperTh->Send(nCmd,nSub,pData,nSize,nCompress);
		if (n < 1)
		{
			//CThreadMgr::PostThreadMsg(m_nThreadID,WM_SOCKET_ON_CLOSE,0,(LPARAM)m_pSock);
		}
		return n;
	}
	return 0;
}

//ͨ���������̷߳�������
int CLocalThreadMgr::SendByNetTh(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress/* = -1*/ )
{
	if(m_pMainTh)
	{
		int n = m_pMainTh->Send(nCmd,nSub,pData,nSize,nCompress);
		if (n < 1)
		{
			//CThreadMgr::PostThreadMsg(m_nThreadID,WM_SOCKET_ON_CLOSE,0,(LPARAM)m_pSock);
		}
		return n;
	}
	return 0;
}
//֪ͨ�����߳�
int CLocalThreadMgr::NotifyNetTh(DWORD dwCmd,WPARAM w,LPARAM l)
{
	if(m_pMainTh)
		return CThreadMgr::PostThreadMsg(m_pMainTh->m_nThreadID,dwCmd,w,l);
	return 0;
}
//֪ͨ���縨���߳�
int CLocalThreadMgr::NotifyNetHelperTh(DWORD dwCmd,WPARAM w,LPARAM l)
{
	if(m_pHelperTh)
		return CThreadMgr::PostThreadMsg(m_pHelperTh->m_nThreadID,dwCmd,w,l);
	return 0;
}
//��ѯ��ǰ�û���Ϣ
void CLocalThreadMgr::QueryUserInfo(int& nUserId,int& nAccountId,CString& sName)
{
	nUserId = m_oPrtAccount.user_id();
	nAccountId = m_oPrtAccount.account_id();
}
//���е�ǰsession�Ľ������
BOOL CLocalThreadMgr::RunUiProcess()
{
	theLog.Write("RunUiProcess %X",m_pSvcCallback);
	CString sPath = CCommonFun::GetDefaultPath();
	sPath += "iSafeClient.exe";
#ifndef _DEBUG
 	if (m_pSvcCallback)
 	{
 		return m_pSvcCallback->OnRunCurrentSessionUiProcess(0);
 	}
	//CProcessExp::CreateProcessByCurUser(sPath,"",0);
#else
	CCommonFun::RunProcess(sPath,"",0,SW_HIDE);
#endif
	return FALSE;
}
void CLocalThreadMgr::OnTipMsg(int nType,CString sMsg,DWORD dwParam,DWORD dwParam2)
{
	TIP_MSG_PARAM param ;
	param.eType = (TIP_MSG_TYPE)nType;
	param.dwParam1 = dwParam;
	param.dwParam2 = dwParam2;
	strncpy_s(param.szMsg,sizeof(param.szMsg),sMsg,sizeof(param.szMsg) - 1);
	if(!m_pMainTh->PostToPipe(WM_MSG_TIP_LOG,0,(BYTE*)&param,sizeof(param)))
	{
		ASSERT(0);
	}
}

CString CLocalThreadMgr::GetLmPrintSvcHostPath()
{
	BOOL status = FALSE;
	SC_HANDLE  schService;
	SC_HANDLE  schSCManager;
	CString sPath;
	//�򿪷�������
	schSCManager = OpenSCManager (NULL, NULL,SC_MANAGER_ALL_ACCESS);
	if (!schSCManager) 
	{
		return sPath;
	}

	schService = OpenService (schSCManager,"LmPrinterSvcHost", SERVICE_ALL_ACCESS);
	if (!schService)
	{
		CloseServiceHandle (schSCManager);
		return sPath;
	}	

	BYTE * lpqscBuf=new BYTE[4096];
	DWORD dwBytesNeeded;
	if(lpqscBuf)
	{
		if (QueryServiceConfig( 
			schService, 
			(LPQUERY_SERVICE_CONFIG)lpqscBuf, 
			4096, 
			&dwBytesNeeded) ) 
		{
			char * pServeDepend=((LPQUERY_SERVICE_CONFIG) lpqscBuf)->lpBinaryPathName;
			
			sPath.Format("%s",strupr(pServeDepend));
			
		}
		delete lpqscBuf;
		lpqscBuf=NULL;
	}

	CloseServiceHandle (schService);
	CloseServiceHandle (schSCManager);	
	
	return sPath;

}
//�ϴ���־��Ϣ����
void CLocalThreadMgr::OnUploadLogResult(int type,BOOL bSuc,void* other /*= 0*/)
{
	theLog.Write("OnUploadLogResult %d,%d",type,bSuc);
	if (m_pMainTh)
	{
		CString sTip; 
		LOG_TYPE eType = (LOG_TYPE)type;
		switch (eType)
		{
		case PRINT_LOG:
			{
				SubmitPrintJob* log = (SubmitPrintJob*)other;
				CString s(log->doc_name().c_str());
				s.Replace('%','#');
				if (bSuc)
				{	
					sTip.Format(_T("��ӡ�ĵ���%s\n�ύ�ɹ���"),s);
					CString sPath = log->job_path().c_str();
#ifndef _DEBUG
					//���������˴�ӡ��̨����
					//ֱ���ύ��ҵ���������ύ������
					if (m_oPrtPolicy.submit_mode() == FIXED_PRINTER_NOCHECK)
					{
						CString sTmp = GetLmPrintSvcHostPath();
						if(!sTmp.IsEmpty())
						{
							sTmp = sTmp.Left(sTmp.ReverseFind('\\'));
							CString sJobPath;
							sJobPath.Format(_T("%s\\%s"),sTmp,PRT_STATION_JOB_DIR);
							if (PathFileExists(sJobPath))
							{
								theLog.Write("exist %s",sJobPath);
								sJobPath += "\\";
								sJobPath += PathFindFileName(sPath);
								BOOL b = CopyFile(sPath,sJobPath,FALSE);
								theLog.Write("CopyFile %s,%s,%d",sPath,sJobPath,b);
							}
						}
					}
					

					BOOL b = DeleteFile(sPath);
					theLog.Write("OnUploadLogResult del the job %s (%d)",sPath,b);
#endif
				}
				else
				{
					sTip.Format(_T("��ӡ�ĵ���%s\n�ύʧ�ܣ��������磡"),s);
				}
				OnTipMsg(eType,sTip,bSuc,log->job_id());
			}
			break;
		}
	}
	else
	{
		ASSERT(0);
	}
}
//��ӡ���˽��
BOOL CLocalThreadMgr::OnJobFilterResult(SubmitPrintJob* job,JobFilterPolciy* policy)
{
	BOOL bPass = TRUE;
	if (job && policy)
	{
		//�ļ����ƣ�
		if (policy->type() == JOB_FLT_BY_DOC_NAME)
		{
			CString sDocName;
			sDocName.Format(_T("%s"),job->doc_name().c_str());
			CString sKeyword = policy->key_word().c_str();
			CStringArray ary;
			sKeyword.MakeLower();
			sDocName.MakeLower();
			CCommonFun::StringSplit(sKeyword,&ary,"|");
			int nSize = ary.GetSize();
			for (int n = 0 ; n < nSize; n ++)
			{
				if(sDocName.Find(ary.GetAt(n)) >= 0)
				{
					bPass = FALSE;
					
					CString s(job->doc_name().c_str());
					s.Replace('%','#');
					CString sTip;
					sTip.Format(_T("��ӡ�ĵ���%s\n��ֹ�ύ������ϵ����Ա��"),s);
					OnTipMsg(PRINT_LOG,sTip,0,job->job_id());


					break;
				}
			}
		}
		//�ĵ�����
		else if (policy->type() == JOB_FLT_BY_DOC_TYPE)
		{
			CString sDocName = job->doc_name().c_str();
			//int nOffset = sDocName.GetLength() - 1 - sDocName.Find('.');
			int nOffset = sDocName.ReverseFind('.');
			if (nOffset > 0)
			{
				//CString sDocType = sDocName.Right(nOffset);
				CString sDocType = sDocName.Mid(nOffset+1);
				CString sKeyword = policy->key_word().c_str();
				CStringArray ary;
				sKeyword.MakeLower();
				sDocType.MakeLower();
				CCommonFun::StringSplit(sKeyword,&ary,"|");
				int nSize = ary.GetSize();
				for (int n = 0 ; n < nSize; n ++)
				{
					if(sDocType.Find(ary.GetAt(n)) >= 0)
					{
						bPass = FALSE;
						CString s(job->doc_name().c_str());
						s.Replace('%','#');
						CString sTip;
						sTip.Format(_T("��ӡ�ĵ���%s\n��ֹ�ύ������ϵ����Ա��"),s);
						OnTipMsg(PRINT_LOG,sTip,0,job->job_id());
						break;
					}
				}
			}
		}
		//�ĵ�����
		else if (policy->type() == JOB_FLT_BY_DOC_CONTENT)
		{
		}
	}
	return bPass;
}
//��ʼ�ύ��ҵ
BOOL CLocalThreadMgr::doSubmitPrintJob(int nId,int nPrtId)
{
	PrintCltCheckUserResult* res = new PrintCltCheckUserResult();
	BOOL b = FALSE;
	
	res->set_job_id(nId);
	res->set_print_station_id(nPrtId);
		
	b = CLocalThreadMgr::GetInstance().NotifyNetHelperTh(WM_SUBMIT_PRINT_JOB,nId,(LPARAM)res);


	if (!b)
	{
		delete res;
	}
	return b;
}
//��ʼ�ύ��ҵ
BOOL CLocalThreadMgr::doSubmitPrintJob(BYTE* pData,int nSize)
{
	PrintCltCheckUserResult* res = new PrintCltCheckUserResult();
	BOOL b = FALSE;
	do 
	{
		if (pData && nSize > 0)
		{
			if(!res->ParseFromArray(pData,nSize))
			{
				ASSERT(0);
				theLog.Write("doSubmitPrintJob Err %d,%X",nSize,pData);
				break;
			}
		}
		
		b = CLocalThreadMgr::GetInstance().NotifyNetHelperTh(WM_SUBMIT_PRINT_JOB,0,(LPARAM)res);
	} while (0);
	
	if (!b)
	{
		delete res;
	}
	return b;
}
//�����ӡ����
BOOL CLocalThreadMgr::OnRequestSubmitPrintJob(int nJobId)
{
	theLog.Write("submitmode = %d,printmode = %d,prtid = %d,jobid = %d",
		m_oPrtPolicy.submit_mode(),
		m_oPrtPolicy.print_mode(),
		m_oPrtPolicy.prt_id(),
		nJobId);
	
	if (IsNetInitSuccessed())
	{
		//��Ҫ�û���¼
		if (m_oPrtPolicy.submit_mode() == USER_SEL_PRINTER_CHECK)
		{
			//���н������
			RunUiProcess();
			//�ӷ������ϻ�ȡ����ʹ�õĴ�ӡ��
			int nRet = SendByNetTh(WM_PRINT_CLT_REQ_CHECK,nJobId,0,0,-1);
			//
			return nRet > 0;
		}
		else if (m_oPrtPolicy.submit_mode() == FIXED_PRINTER_NOCHECK)
		{
			//ʹ��Ĭ�ϴ�ӡ����ӡ
			return CLocalThreadMgr::GetInstance().doSubmitPrintJob(nJobId,0);
		}
		else
		{
			ASSERT(0);
			theLog.Write("û�д���Ĭ��ʹ��ֱ�Ӵ�ӡģʽ");
			//ʹ��Ĭ�ϴ�ӡ����ӡ
			return CLocalThreadMgr::GetInstance().doSubmitPrintJob(nJobId,0);
		}
	}
	else
	{
		theLog.Write("net not connected");
	}
	

	return 0;

}
//��־�ϴ�
void CLocalThreadMgr::OnUploadLog(int type,void* log)
{
	//zhandb todo
	if(type == PRINT_LOG)
	{
		int nPrtId = 0;
		int nOrderPrtId = 0;
		int nPrjId = 0;
		if (m_oPrtPolicy.submit_mode() == FIXED_PRINTER_NOCHECK)
		{
			nPrtId = m_oPrtPolicy.prt_id();
		}
		SubmitPrintJob* job = (SubmitPrintJob*)log;
		job->set_user_id(0);
		job->set_order_prtstation_id(nOrderPrtId);
		job->set_project_id(nPrjId);
		job->set_direct_prtstation_id(nPrtId);
		job->set_sec_type(JOb_SEC_NORMAL);
	}
}
//��Ʒ��Ϣ
BOOL CLocalThreadMgr::OnProduct(BYTE* pData,int nSize)
{
	if(m_oProduct.ParseFromArray(pData,nSize))
	{
		CSQliteDataSvc::GetInstance().SaveConfig("product","",pData,nSize);
		return TRUE;
	}
	return FALSE;
}
//ƽ̨�˺���Ϣ
BOOL CLocalThreadMgr::OnUser(BYTE* pData,int nSize)
{
	if(m_oUser.ParseFromArray(pData,nSize))
	{
		CSQliteDataSvc::GetInstance().SaveConfig("user","",pData,nSize);
		return TRUE;
	}
	return FALSE;
}
//��ӡ�˺���Ϣ
BOOL CLocalThreadMgr::OnPrintAccount(BYTE* pData,int nSize)
{
	if(m_oPrtAccount.ParseFromArray(pData,nSize))
	{
		CSQliteDataSvc::GetInstance().SaveConfig("print_account","",pData,nSize);
		return TRUE;
	}
	return FALSE;
}
//��ӡվ����Ϣ
BOOL CLocalThreadMgr::OnPrinter(BYTE* pData,int nSize)
{
	if(m_oPrint.ParseFromArray(pData,nSize))
	{
		CSQliteDataSvc::GetInstance().SaveConfig("printer","",pData,nSize);
		return TRUE;
	}
	return FALSE;
}
//��ӡϵͳ����
BOOL CLocalThreadMgr::OnPrinterPolicy(BYTE* pData,int nSize)
{
	if(m_oPrtPolicy.ParseFromArray(pData,nSize))
	{
		theLog.Write("OnPrinterPolicy submitmode = %d,printmode = %d,prtid = %d,%d,%s",
			m_oPrtPolicy.submit_mode(),
			m_oPrtPolicy.print_mode(),
			m_oPrtPolicy.prt_id(),
			m_oPrtPolicy.enable_doc_log(),
			m_oPrtPolicy.doc_type().c_str());
#if 0
		//���ش���δ�ύ�Ĵ�ӡ����
		if (CSQliteDataSvc::GetInstance().GetLogCount(PRINT_LOG) > 0)
		{
			theLog.Write("exist PRINT_LOG req to submit");
			if(!RequestPrintJob())
			{
				ASSERT(0);
				theLog.Write("RequestPrintJob err");
			}
		}
#endif	
		CSQliteDataSvc::GetInstance().SaveConfig("printer_policy","",pData,nSize);
		CIniFile file;
		file.m_sPath.Format("%s%s",CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
		file.SetVal("ClientConfig","url",m_oPrtPolicy.url().c_str());

		//CCommonFun::CreateURLLink(m_oPrtPolicy.url().c_str(), 
		//	CCommonFun::GetCommonDesktopDir()+"\\��ӡϵͳ��ѯ.url", 
		//	CCommonFun::GetDefaultPath()+"res\\web.ico");

		CCommonFun::CreateShellLink(CCommonFun::GetDefaultPath()+"iSecWeb.exe", 
			CCommonFun::GetCommonDesktopDir()+"\\��ӡϵͳ��ѯ.lnk", 
			CCommonFun::GetDefaultPath()+"res\\web.ico",
			0,
			m_oPrtPolicy.url().c_str());

		if(!IsEduPrint())
		{
			if (m_oPrtPolicy.enable_doc_log())
			{
				CPrintDocMgr::GetInstance().SetDocType(m_oPrtPolicy.doc_type().c_str());
				CPrintDocMgr::GetInstance().Start();
			}
			else
			{
				CPrintDocMgr::GetInstance().Stop();
			}
		}
		return TRUE;
	}
	else
	{
		theLog.Write(_T("OnPrinterPolicy err"));
	}
	return FALSE;
}
BOOL CLocalThreadMgr::IsPrintActive()
{
	ProductInfo product;
	CSQliteDataSvc::GetInstance().GetProductInfo(ISEC_PRINT_CODE,&product);
	return IsPrintProduct(product.product_code());
}

BOOL CLocalThreadMgr::IsEduPrint()
{
	ProductInfo product;
	CSQliteDataSvc::GetInstance().GetProductInfo(ISEC_PRINT_CODE,&product);
	if ((product.product_code() & ISEC_PRINT_EDU) == ISEC_PRINT_EDU)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CLocalThreadMgr::IsNeedNotifyUi()
{
	if (m_oPrtPolicy.submit_mode() == FIXED_PRINTER_NOCHECK)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CLocalThreadMgr::IsNetInitSuccessed()
{
	if (m_pMainTh&& m_pMainTh->IsConnected() && m_pHelperTh && m_pHelperTh->IsConnected())
	{
		return TRUE;
	}
	return FALSE;
}

void CLocalThreadMgr::OnSessionChange(DWORD dwEventType,LPVOID lpEventData)
{
	theLog.Write("dwEventType = %d",dwEventType);
	if(dwEventType == WTS_SESSION_LOGON)
	{		
		theLog.Write("IsNeedNotifyUi = %d",IsNeedNotifyUi());
		if (IsNeedNotifyUi())
		{
			RunUiProcess();
		}
	}
}

CCmdMgr::CCmdMgr()
{
	//m_lReqCount = 0;
	InterlockedExchange(&m_lReqCount,0);
}

CCmdMgr& CCmdMgr::GetInstance()
{
	static CCmdMgr one;
	return one;
}

void CCmdMgr::Clear()
{
	InterlockedExchange(&m_lReqCount,0);
}

void CCmdMgr::doReqCmd()
{
	//if(InterlockedIncrement(&m_lReqCount) == 1)
		CLocalThreadMgr::GetInstance().SendByNetHelper(WM_CLT_REQ_ONLINE_CMD,0,0,0);
	//else
	//	theLog.Write("last req not response");
}

void CCmdMgr::OnResponseCmd(NET_PACK_HEAD* pHead)
{
	//InterlockedDecrement(&m_lReqCount);
	if (pHead->nPackBodySize > 0)
	{
		CLocalThreadMgr::GetInstance().m_pMainTh->PostToPipe((IST_PIPE_STRUCT*)pHead);
	}
}

void CCmdMgr::doCmd(const OnlineCmd& cmd)
{
	switch (cmd.type())
	{
	case CMD_TIP:
		{
			theLog.Write("cmd_tip = %s",cmd.msg().c_str());
		}
		break;
	}
}