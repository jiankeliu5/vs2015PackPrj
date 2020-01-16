// CloudDocDealThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CloudDocDealThread.h"

IMPLEMENT_DYNCREATE(CCloudDocDealThread, CWinThread)

CCloudDocDealThread::CCloudDocDealThread()
{
	m_nCloudDocDealTimer = NULL;
	m_nCheckDbConnTimer = NULL;
	m_pOwner = NULL;
	SetActive(FALSE);
}

CCloudDocDealThread::~CCloudDocDealThread()
{
}

BOOL CCloudDocDealThread::InitInstance()
{
	theLog.Write(_T("CCloudDocDealThread::InitInstance, m_nThreadID=%u"), m_nThreadID);
	EnableCheckCloudDocDeal(TRUE);
	EnableCheckDbConn(TRUE);
	return TRUE;
}

int CCloudDocDealThread::ExitInstance()
{
	theLog.Write(_T("CCloudDocDealThread::ExitInstance, m_nThreadID=%u"), m_nThreadID);
	SetActive(FALSE);
	EnableCheckCloudDocDeal(FALSE);
	EnableCheckDbConn(FALSE);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCloudDocDealThread, CWinThread)
END_MESSAGE_MAP()


//�����߳�
void CCloudDocDealThread::OnEndTh(WPARAM wp, LPARAM lp)
{
	theLog.Write(_T("CCloudDocDealThread::OnEndTh,m_nThreadID=%u"), m_nThreadID);
	ExitInstance();
	AfxEndThread(0);
}

BOOL CCloudDocDealThread::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_TIMER:
		{
			OnTimer(pMsg->wParam,pMsg->lParam);		
		}
		break;
	case WM_THREAD_EXIT:
		{
			OnEndTh(pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
		break;
	}

	return CWinThread::PreTranslateMessage(pMsg);
}

void CCloudDocDealThread::OnTimer(WPARAM wp, LPARAM lp)
{
	if (wp == m_nCloudDocDealTimer)
	{
		//EnableCheckCloudDocDeal(FALSE);
		if (IsActive())
		{
			CheckCloudDocDeal();
		}
		else
		{
			theLog.Write(_T("##CCloudDocDealThread::OnTimer,IsActive false"));
		}
		//EnableCheckCloudDocDeal(TRUE);
	}
	else if (wp == m_nCheckDbConnTimer)
	{
		EnableCheckDbConn(FALSE);
		CDbOperateHelper helper;
		if (!helper.CheckDbConn())
		{
			theLog.Write(_T("!!CCloudDocDealThread::OnTimer,1,CheckDbConn fail"));
#if 0	//���Բ���Ҫ������ado���Զ������Ļ��ơ�
			if (!helper.ReConnDb())
			{
				theLog.Write(_T("!!CCloudDocDealThread::OnTimer,2,ReConnDb fail"));
			}
#endif
		}
		EnableCheckDbConn(TRUE);
	}
}

void CCloudDocDealThread::EnableCheckCloudDocDeal(BOOL bEnable)
{
	//theLog.Write(_T("CCloudDocDealThread::EnableCheckCloudDocDeal,bEnable=%d,m_nCloudDocDealTimer=%u"), bEnable, m_nCloudDocDealTimer);
	if (bEnable)
	{
		if (!m_nCloudDocDealTimer)
		{
			m_nCloudDocDealTimer = ::SetTimer(NULL,0,Check_Cloud_Doc_Deal_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nCloudDocDealTimer)
		{
			KillTimer(NULL, m_nCloudDocDealTimer);
			m_nCloudDocDealTimer = NULL;
		}
	}
}

void CCloudDocDealThread::EnableCheckDbConn(BOOL bEnable)
{
	//theLog.Write(_T("CCloudDocDealThread::EnableCheckDbConn,bEnable=%d,m_nCheckDbConnTimer=%u"), bEnable, m_nCheckDbConnTimer);
	if (bEnable)
	{
		if (!m_nCheckDbConnTimer)
		{
			m_nCheckDbConnTimer = ::SetTimer(NULL,0,Check_Cloud_Db_Conn_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nCheckDbConnTimer)
		{
			KillTimer(NULL, m_nCheckDbConnTimer);
			m_nCheckDbConnTimer = NULL;
		}
	}
}


void CCloudDocDealThread::CheckCloudDocDeal()
{
	if (m_pOwner)
	{
		PCloudJobInfo pJob = m_pOwner->GetOneCloudJob();
		if (pJob)
		{
			theLog.Write(_T("CCloudDocDealThread::CheckCloudDocDeal,2,m_nThreadID=%d,")
				_T("nJobId=%d, nUserId=%d, cAccount=%s, cDocName=%s, cJobDir=%s, cOrgJobName=%s, nJobState=%d, cPageType=%s, nPaperSize=%d, ")
				_T("nColor=%d, nCopies=%d, nDoublePrint=%d, nDocSize=%d, nOrgJobSize=%d, nOrientation=%d, nPaperSource=%d, nTotalPage=%d, fSubmitTime=%s")
				, m_nThreadID
				, pJob->nJobId, pJob->nUserId, pJob->cAccount, pJob->cDocName, pJob->cJobDir, pJob->cOrgJobName, pJob->nJobState
				, pJob->cPageType, pJob->nPaperSize, pJob->nColor, pJob->nCopies, pJob->nDoublePrint, pJob->nDocSize, pJob->nOrgJobSize
				, pJob->nOrientation, pJob->nPaperSource, pJob->nTotalPage, GetTimeStr(pJob->fSubmitTime));

			CCloudDocDeal deal(pJob);
			if (!deal.ConvertFile())
			{
				pJob->nJobState = JOB_CLOUD_DEAL_FAIL;
				theLog.Write(_T("!!CCloudDocDealThread::CheckCloudDocDeal,3,ConvertFile fail, m_nThreadID=%d"), m_nThreadID);
			}
			else
			{
				pJob->nJobState = JOB_CLOUD_DEAL_SUCC;

				//���ļ��ϴ����洢������
				{
					BOOL bRet = FALSE;
					CString szLocalFilePath;szLocalFilePath.Format(_T("%s\\%s"), pJob->cJobDir, pJob->cNewJobName);
					COleDateTime now = COleDateTime::GetCurrentTime();
					CString szFtpDir;szFtpDir.Format(_T("/PrintDir/%04d%02d%02d"), now.GetYear(), now.GetMonth(), now.GetDay());
					CString szFtpFileName = CCommonFun::GetFileNameByPathName(szLocalFilePath);
					CString szJobPath;szJobPath.Format(_T("%s/%s"), szFtpDir, szFtpFileName);
					_tcscpy_s(pJob->cNewJobName, szJobPath);

					//��ȡ�洢�б�
					CFileStorageList oList;
					CFileStorageHelper::GetInstance().GetBestFileStorageList(pJob->cIP, oList);
					int nListCount = oList.GetCount();
					if (nListCount <= 0)
					{
						theLog.Write(_T("!!CCloudDocDealThread::CheckCloudDocDeal,3.2,GetBestFileStorageList fail,nListCount=%d"), nListCount);
					}
					else
					{
						//��ͷ��ʼ��������ϴ���ֱ��ȫ��������ɻ��߳ɹ�Ϊֹ
						POSITION pos = oList.GetHeadPosition();
						for (int index=0; index<nListCount; index++)
						{
							FileStorage* pFileStorage = oList.GetNext(pos);
							CString szIP = CCommonFun::UTF8ToUnicode(pFileStorage->ip().c_str());
							CString szUserName = CCommonFun::UTF8ToUnicode(pFileStorage->username().c_str());
#ifdef ENABLE_PWD_CRY
							char cPwd[MAX_PATH] = { 0 };
							int nSize = MAX_PATH;
							CPWDCry cry;
							CStringA szSaPasswordA = pFileStorage->password().c_str();
							cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
							szSaPasswordA.ReleaseBuffer();
							CString szPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
							CString szPassword = CCommonFun::UTF8ToUnicode(pFileStorage->password().c_str());
#endif
							pJob->nFileStorageId = pFileStorage->id();
							bRet = CFileHelper::UploadFileToFtpServer(szIP, pFileStorage->port(), szFtpDir
								, szUserName, szPassword, szFtpFileName, szLocalFilePath);
							if (!bRet)
							{
								theLog.Write(_T("!!CCloudDocDealThread::CheckCloudDocDeal,3.3,UploadFileToFtpServer fail,index=%d,ip=%s")
									, index, szIP);
							}
							else
							{
								break;
							}
						}
					}

					//�ͷŴ洢�б�
					CFileStorageHelper::GetInstance().ReleaseFileStorageList(oList);

					if (!bRet)
					{
						pJob->nJobState = JOB_CLOUD_DEAL_FAIL;
						theLog.Write(_T("!!CCloudDocDealThread::CheckCloudDocDeal,3.4,UploadFileToFtpServer fail,szLocalFilePath=%s"), szLocalFilePath);
					}
					else
					{
						theLog.Write(_T("CCloudDocDealThread::CheckCloudDocDeal,3.5,UploadFileToFtpServer succ,szLocalFilePath=%s"), szLocalFilePath);
					}

					//�����ϴ��ɹ�����ʧ�ܣ���ɾ�������ĵ���
					DeleteFile(szLocalFilePath);
				}			
			}

			//�ĵ�����ɹ�����鿴�Ƿ���Ҫ���
			if (pJob->nJobState == JOB_CLOUD_DEAL_SUCC)
			{
				CheckAuditProcess(pJob);
			}
			CDbOperateHelper helper;
			if (!helper.UpdateCloudJob(pJob))
			{
				theLog.Write(_T("!!CCloudDocDealThread::CheckCloudDocDeal,4,UpdateCloudJob fail, m_nThreadID=%d"), m_nThreadID);
			}

			//�ͷ��ڴ�
			deal.Release();
		}
	}
// 	theLog.Write(_T("CCloudDocDealThread::CheckCloudDocDeal, end, m_nThreadID=%d"), m_nThreadID);
}

void CCloudDocDealThread::SetOwner(CCloudDocDealMgr* pOwner)
{
	m_pOwner = pOwner;
	if (!m_pOwner)
	{
		theLog.Write(_T("!!CCloudDocDealThread::SetOwner,m_pOwner=%p"), m_pOwner);
	}
}

void CCloudDocDealThread::SetActive(BOOL bActive)
{
	if (bActive)
	{
		InterlockedExchange(&m_lActiveDealDoc,TRUE);
	}
	else
	{
		InterlockedExchange(&m_lActiveDealDoc,FALSE);
	}
}

BOOL CCloudDocDealThread::IsActive()
{
	long lVal = 0;
	InterlockedExchange(&lVal,m_lActiveDealDoc);
	return lVal;
}

CString CCloudDocDealThread::GetTimeStr(double fTime)
{
	COleDateTime time;
	time.m_dt = fTime;
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

class CAuditJob
{
public:
	static BOOL Check(JobAuditProcess* process,PCloudJobInfo job)
	{
		BOOL bPass = TRUE;
		if (process->check_by() == JOB_CHECK_BY_SYS)
		{
			JobFilterPolciy* policy = process->mutable_flt_policy();
			if (policy)
			{
				CString sDocName = job->cDocName;
				CString sKeyword = CCommonFun::UTF8ToUnicode(policy->key_word().c_str());
				sKeyword.MakeLower();
				sDocName.MakeLower();
				theLog.Write(_T("doc = %s,filter = %s"),job->cDocName,policy->key_word().c_str());
				//�ļ����ƣ�
				if (policy->type() == JOB_FLT_BY_DOC_NAME)
				{
					CStringArray ary;
					CCommonFun::StringSplit(sKeyword,&ary,_T("|"));
					int nSize = ary.GetSize();
					for (int n = 0 ; n < nSize; n ++)
					{
						if(sDocName.Find(ary.GetAt(n)) >= 0)
						{
							bPass = FALSE;
							break;
						}
					}
				}
				//�ĵ�����
				else if (policy->type() == JOB_FLT_BY_DOC_TYPE)
				{
					int nOffset = sDocName.GetLength() - 1 - sDocName.Find('.');
					if (nOffset > 0)
					{
						CString sDocType = sDocName.Right(nOffset);
						CStringArray ary;
						CCommonFun::StringSplit(sKeyword,&ary,_T("|"));
						int nSize = ary.GetSize();
						for (int n = 0 ; n < nSize; n ++)
						{
							if(sDocType.CompareNoCase(ary.GetAt(n)) == 0)
							{
								bPass = FALSE;
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
		}
		return bPass;
	}
	static BOOL NeedDeptUserAudit(JobAuditProcess* process,PCloudJobInfo job)
	{
		BOOL bNeed = FALSE;
		if (process->check_by() == JOB_CHECK_BY_DEPTUSERS)
		{
			JobFilterPolciy* policy = process->mutable_flt_policy();
			CString sKeywordDoctype ;
			CString sKeywordDocname ;
			if (policy->has_key_word_doctype())
			{
				sKeywordDoctype = policy->key_word_doctype().c_str();
			}
			if (policy->has_key_word_docname())
			{
				sKeywordDocname = policy->key_word_docname().c_str();
			}
			theLog.Write(_T("sKeywordDoctype = %s,sKeywordDocname = %s, docname=%s")
				,sKeywordDoctype,sKeywordDocname, job->cDocName);
			if (sKeywordDoctype.GetLength() <= 0 && sKeywordDocname.GetLength()<= 0)
			{
				return TRUE;
			}	
			if (policy)
			{
				//�ļ����ƣ�
				CString sDocName;
				sDocName.Format(_T("%s"),job->cDocName);
				CStringArray ary;
				sKeywordDocname.MakeLower();
				sDocName.MakeLower();
				CCommonFun::StringSplit(sKeywordDocname,&ary,_T("|"));
				int nSize = ary.GetSize();
				for (int n = 0 ; n < nSize; n ++)
				{
					if(sDocName.Find(ary.GetAt(n)) >= 0)
					{
						bNeed = TRUE;
						break;
					}
				}

				//���ĵ�����
				int nOffset = sDocName.GetLength() - 1 - sDocName.ReverseFind('.');//Ӧ�ô��ұ���
				if (nOffset > 0)
				{
					CString sDocType = sDocName.Right(nOffset);
					//theLog.Write(_T("============sKeywordDoctype=%s"),sKeywordDoctype);
					CStringArray ary;
					sKeywordDoctype.MakeLower();
					sDocType.MakeLower();
					CCommonFun::StringSplit(sKeywordDoctype,&ary,_T("|"));
					int nSize = ary.GetSize();
					for (int n = 0 ; n < nSize; n ++)
					{
						if(sDocType.CompareNoCase(ary.GetAt(n)) == 0)
						{
							bNeed = TRUE;
							break;
						}
					}
				}
			}
		}
		return bNeed;
	}
};
void CCloudDocDealThread::CheckAuditProcess( PCloudJobInfo pInfo )
{
	if(!pInfo)
	{
		theLog.Write(_T("!!CCloudDocDealThread::CheckAuditProcess,1,pInfo=NULL"));
		return;
	}
	theLog.Write(_T("CCloudDocDealThread::CheckAuditProcess,1,nUserId=%d"),pInfo->nUserId);
#if 0
	CDbOperateHelper helper;
	JobAuditProcess audit;
	CPrintAlarmInfo alarm;
	SUBMIT_JOB_STATE eState = JOB_CLOUD_DEAL_SUCC;
	int nAdminId = 0;
	int nUserId = pInfo->nUserId;
	JOb_CHECK_BY eCheck = JOB_CHECK_BY_NULL;
	{
		/*
		������ҵ��ȫ���ԣ��û�id�жϳ��Ƿ���Ҫ��ˣ�����˷�ʽ
		0,����ˣ���ҵ�Զ�����ΪJOB_CLOUD_DEAL_SUCC״̬
		1.ϵͳ�Զ���ˣ�������Ƿ���йؼ��֣����û�����Զ��������Ѿ���ˣ����Դ�ӡ״̬������Ԥ��
		2,ָ����Ա��ˣ������ѵ�ָ����Ա
		*/
	}
	do 
	{
		int nDeptId = helper.GetDeptByUserId(nUserId);
		if(!helper.GetJobAuditProcess(nUserId,audit))
		{
			break;
		}
		eCheck = audit.check_by();			
		if (eCheck == JOB_CHECK_BY_NULL)
		{
			break;
		}
		nAdminId = audit.admin_id();
		alarm.m_nDeptId = 0;
		alarm.m_sUserIds = _T("");
		CString szUserIds = CCommonFun::UTF8ToUnicode(audit.userids().c_str());
		UiLanguage language = helper.GetBackendLanguage();
		theLog.Write(_T("check by %d,%d,userids=%s,language=%d"),eCheck,nAdminId,szUserIds,language);
		//��������е�Ա�����
		if (eCheck == JOB_CHECK_BY_DEPTUSERS)
		{
			if (CAuditJob::NeedDeptUserAudit(&audit,pInfo))//�������������Ҫ�����,��Ҫ���
			{
				eState = JOB_SUBMITED_UNCHECK;
				eCheck = audit.check_by();

				alarm.m_sSubject = MultiLanguageString(_T("PrintCheckSubject"), _T("��ӡ�������"), language);
				alarm.m_sContent.Format(MultiLanguageString(_T("PrintCheckContent"), _T("��ӡ�ĵ���%s����Ҫ��ˣ�"), language), pInfo->cDocName);
				alarm.m_bNeedAudit = TRUE;
				alarm.m_nUserId = nUserId;
				alarm.m_nDeptId = nDeptId;
				alarm.m_sUserIds = audit.has_userids() ? szUserIds : _T("");
			}
			else
			{
				eState = JOB_CLOUD_DEAL_SUCC;
				eCheck = audit.check_by();
			}

		}
		//�˹����
		else if (eCheck == JOB_CHECK_BY_MANUAL)
		{
			eState = JOB_SUBMITED_UNCHECK;

			alarm.m_sSubject = MultiLanguageString(_T("PrintChecklSubject"), _T("��ӡ�������"), language);
			alarm.m_sContent.Format(MultiLanguageString(_T("PrintCheckContent"), _T("��ӡ�ĵ���%s����Ҫ��ˣ�"), language), pInfo->cDocName);
			alarm.m_nAdminId = nAdminId;
			alarm.m_bNeedAudit = TRUE;
			alarm.m_nUserId = nUserId;
		}
		//ϵͳ�Զ����
		else if (eCheck == JOB_CHECK_BY_SYS)
		{
			if(!CAuditJob::Check(&audit, pInfo))
			{
				if (audit.next_action() == JOB_CONTINE_PRT_AND_ALARM_ADMIN)
				{
					eState = JOB_CHECKED_FAILED;

					alarm.m_sSubject = MultiLanguageString(_T("PrintCheckSystemSubject"), _T("��ӡΥ��Ԥ��"), language);
					alarm.m_sContent.Format(MultiLanguageString(_T("PrintCheckSystemContent"), _T("��ӡ�ĵ���%s����ϵͳ����У�����Υ�棡"), language), pInfo->cDocName);
					alarm.m_nAdminId = nAdminId;
				}
				else if (audit.next_action() == JOB_PAUSE_AND_ALARM_USER)
				{
					eState = JOB_CHECKED_FAILED;

					alarm.m_sSubject = MultiLanguageString(_T("PrintCheckSystemSubject"), _T("��ӡΥ��Ԥ��"), language);
					alarm.m_sContent.Format(MultiLanguageString(_T("PrintCheckSystemPauseContent"), _T("��ӡ�ĵ���%s����ϵͳ����У�����Υ�棬����ͣ��ӡ��"), language), pInfo->cDocName);
					alarm.m_nUserId = nUserId;
				}
				else
				{
					ASSERT(0);
					eState = JOB_CLOUD_DEAL_SUCC;
				}

			}
			else
			{
				JobFilterPolciy* policy = audit.mutable_flt_policy();
				if (policy)
				{
					eState = JOB_SUBMITED_UNCHECK;
				}
				else
				{
					eState = JOB_CLOUD_DEAL_SUCC;
				}
			}
		}
	} while (FALSE);
	pInfo->nJobState = eState;
	if (pInfo->nJobId > 0 && alarm.m_nUserId > 0)//ϵͳ���֪ͨ����Ա�����
	{
		alarm.m_nJobId = pInfo->nJobId;
		//alarm.m_nUserId = nUserId;
		helper.AddCheck(&alarm);
	}
#else
	//nAuditType��������ͣ�Ĭ��Ϊ0��0:��������1���ֶ���ˣ�2��OCR���)
	//bAuditLog�������־�Ƿ�����Ĭ��false��true�� ������false���رգ�
	//OCRʶ��״̬��0:��ʶ��1����ʶ��2��ʶ���У�3:ʶ��ɹ���4��ʶ��ʧ�ܣ�
	int nAuditTacticsId = 0;
	int nAuditType = 0;
	BOOL bAuditLog = FALSE;
	CString szAuditUsers;
	CString szAuditAdmins;
	CDbOperateHelper helper;
	helper.GetUserJobAuditPolicy(pInfo->nUserId, nAuditTacticsId, nAuditType, bAuditLog, szAuditUsers, szAuditAdmins);
	//theLog.Write(_T("1111,nAuditType=%d,bAuditLog=%d,szAuditUsers=%s"), nAuditType, bAuditLog, szAuditUsers);

	if (nAuditType == 2)
	{
		pInfo->nOcrState = 1;
		pInfo->nJobState = JOB_SUBMITED_UNCHECK;
		pInfo->nAuditTacticsId = nAuditTacticsId;
	}
	else if (nAuditType == 1)
	{
		UiLanguage language = helper.GetBackendLanguage();
		CPrintAlarmInfo alarm;
		alarm.m_nUserId = pInfo->nUserId;
		alarm.m_nDeptId = 0;
		alarm.m_nJobId = pInfo->nJobId;
		alarm.m_bNeedAudit = TRUE;
		alarm.m_sAdminIds = szAuditAdmins;
		alarm.m_sUserIds = szAuditUsers;
		alarm.m_sSubject = MultiLanguageString(_T("PrintCheckSubject"), _T("��ӡ�������"), language);
		alarm.m_sContent.Format(MultiLanguageString(_T("PrintCheckContent"), _T("��ӡ�ĵ���%s����Ҫ��ˣ�"), language), pInfo->cDocName);
		helper.AddCheck(&alarm);

		pInfo->nJobState = JOB_SUBMITED_UNCHECK;
		pInfo->nOcrState = 0;
		pInfo->nAuditTacticsId = nAuditTacticsId;
	}
	else if (!bAuditLog)
	{
		pInfo->nOcrState = 0;
	}

	//���������OCR��־����һ��ҪOCR����
	if (bAuditLog)
	{
		pInfo->nOcrState = 1;
		pInfo->nAuditTacticsId = nAuditTacticsId;
	}
#endif

}
