#include "StdAfx.h"
#include "CloudDocDealMgr.h"

CCloudDocDealMgr::CCloudDocDealMgr(void)
{
	m_pHelperTh = NULL;
}

CCloudDocDealMgr::~CCloudDocDealMgr(void)
{
}

void CCloudDocDealMgr::InitWork()
{
	Release();
	InitTh();
	InitHelperTh();
}

void CCloudDocDealMgr::Release()
{
	ReleaseJob();
	ReleaseTh();
	ReleaseHelperTh();
}

void CCloudDocDealMgr::CheckNeedDealCloudJob()
{
	//theLog.Write("CCloudDocDealMgr::CheckNeedDealCloudJob,begin");
	
	DWORD dwCloudJobCount = GetCloudJobCount();
	if (dwCloudJobCount > Max_Wait_Cloud_Doc_Deal_Count)
	{
		theLog.Write(_T("##CCloudDocDealMgr::CheckNeedDealCloudJob,dwCloudJobCount=%d,Max_Wait_Cloud_Doc_Deal_Count=%d")
			, dwCloudJobCount, Max_Wait_Cloud_Doc_Deal_Count);
		return;
	}

	CDbOperateHelper helper;
	CListCloudJobInfo list;
	if (helper.GetNeedDealCloudJob(list))
	{
		theLog.Write(_T("CCloudDocDealMgr::CheckNeedDealCloudJob,2,GetNeedDealCloudJob count=%d,dwCloudJobCount=%d")
			, list.GetCount(), dwCloudJobCount);
		POSITION pos = list.GetHeadPosition();
		for (int i = 0; i < list.GetCount(); i++)
		{
			PCloudJobInfo pJob = (PCloudJobInfo)list.GetNext(pos);
			if (!AddOneCloudJob(pJob))
			{
				//当从数据库获取到的作业信息添加到待处理列表时发生错误，
				//则删除此信息，在下次获取中将重新获取到它。
				delete pJob;
			}
		}

	}
	//theLog.Write(_T("CCloudDocDealMgr::CheckNeedDealCloudJob,end"));
}

BOOL CCloudDocDealMgr::AddOneCloudJob(PCloudJobInfo job)
{
	CCriticalSection2::Owner ow(m_cs4ListJob);

	BOOL bSucc = FALSE;
	if (job)
	{
		//设置云打印作业状态,
		CDbOperateHelper helper;
		if (helper.UpdateCloudJobState(job, JOB_CLOUD_DEAL_PENDING))
		{
			bSucc = TRUE;
			m_oListJob.AddTail(job);
		}
		else
		{
			theLog.Write(_T("!!CCloudDocDealMgr::AddOneCloudJob,UpdateCloudJobState fail,nJobId=%d"), job->nJobId);
		}
	}
	else
	{
		theLog.Write(_T("!!CCloudDocDealMgr::AddOneCloudJob,job=%p"), job);
	}

	return bSucc;
}

PCloudJobInfo CCloudDocDealMgr::GetOneCloudJob()
{
	CCriticalSection2::Owner ow(m_cs4ListJob);

	PCloudJobInfo job = NULL;
	if (m_oListJob.GetCount()>0)
	{
		job = m_oListJob.RemoveHead();	//返回头结点，并且删除头结点
	}
	if (job)
	{
		CDbOperateHelper helper;
		helper.UpdateCloudJobState(job, JOB_CLOUD_DEALING);
	}
	return job;
}

DWORD CCloudDocDealMgr::GetCloudJobCount()
{
	CCriticalSection2::Owner ow(m_cs4ListJob);
	DWORD dwCloudJobCount = m_oListJob.GetCount();
	return dwCloudJobCount;
}

void CCloudDocDealMgr::InitTh()
{
	CCriticalSection2::Owner ow(m_cs4ListTh);

	SYSTEM_INFO si;
	memset(&si, 0x0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);
#ifdef _DEBUG
	DWORD dwNumberOfProcessors = 2;
#else
	DWORD dwNumberOfProcessors = si.dwNumberOfProcessors;
	if (dwNumberOfProcessors < Min_Cloud_Doc_Deal_Thread_Count)
	{
		theLog.Write(_T("##CCloudDocDealMgr::InitTh,1.B,dwNumberOfProcessors=%d,Min_Cloud_Doc_Deal_Thread_Count")
			, dwNumberOfProcessors, Min_Cloud_Doc_Deal_Thread_Count);
		dwNumberOfProcessors = Min_Cloud_Doc_Deal_Thread_Count;
	}

	//云文档处理最大线程数
	int nMaxCloudDocDealThreadCount = theIniFile.GetVal(_T("CloudDocHelper"), _T("MaxCloudDocDealThreadCount"), 128);
	theLog.Write(_T("CCloudDocDealMgr::InitTh,1.2,dwNumberOfProcessors=%d,nMaxCloudDocDealThreadCount=%d")
		, dwNumberOfProcessors, nMaxCloudDocDealThreadCount);
	if (dwNumberOfProcessors > nMaxCloudDocDealThreadCount)
	{
		dwNumberOfProcessors = nMaxCloudDocDealThreadCount;
	}
#endif
	theLog.Write(_T("CCloudDocDealMgr::InitTh,1,dwNumberOfProcessors=%d"), dwNumberOfProcessors);
	for (DWORD i=0; i<dwNumberOfProcessors; i++)
	{
		CCloudDocDealThread* pTh = (CCloudDocDealThread*)::AfxBeginThread(RUNTIME_CLASS(CCloudDocDealThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if (pTh)
		{
			pTh->SetOwner(this);
			pTh->SetActive(TRUE);
			m_oListTh.AddTail(pTh);
			DWORD dwRet = pTh->ResumeThread();
			if (dwRet == 0xFFFFFFFF)
			{
				theLog.Write(_T("!!CCloudDocDealMgr::InitTh,2,i=%d,pTh=%p,dwRet=0x%X,err=%u"), i, pTh, dwRet, GetLastError());
			}
		}
		else
		{
			theLog.Write(_T("!!CCloudDocDealMgr::InitTh,3,i=%d,pTh=%p,err=%u"), i, pTh, GetLastError());
		}
	}
}

void CCloudDocDealMgr::InitHelperTh()
{
	theLog.Write(_T("CCloudDocDealMgr::InitHelperTh,1,m_pHelperTh=%p"), m_pHelperTh);
	if (m_pHelperTh)
	{
		return;
	}

	m_pHelperTh = (CCloudDocDealHelperThread*)::AfxBeginThread(RUNTIME_CLASS(CCloudDocDealHelperThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if (m_pHelperTh)
	{
		m_pHelperTh->SetOwner(this);
		m_pHelperTh->SetActive(TRUE);
		DWORD dwRet = m_pHelperTh->ResumeThread();
		if (dwRet == 0xFFFFFFFF)
		{
			theLog.Write(_T("!!CCloudDocDealMgr::InitHelperTh,2,m_pHelperTh=%p,dwRet=0x%X,err=%u"), m_pHelperTh, dwRet, GetLastError());
		}
	}
	else
	{
		theLog.Write(_T("!!CCloudDocDealMgr::InitHelperTh,3,m_pHelperTh=%p,err=%u"), m_pHelperTh, GetLastError());
	}
}

void CCloudDocDealMgr::ReleaseJob()
{
	CCriticalSection2::Owner ow(m_cs4ListJob);

	theLog.Write(_T("CCloudDocDealMgr::ReleaseJob,1,count=%d"), m_oListJob.GetCount());

	POSITION pos = m_oListJob.GetHeadPosition();
	for (int i = 0; i < m_oListJob.GetCount(); i++)
	{
		PCloudJobInfo pJob = (PCloudJobInfo)m_oListJob.GetNext(pos);
		if (pJob)
		{
			//设置云打印作业状态,
			CDbOperateHelper helper;
			helper.UpdateCloudJobState(pJob, JOB_CLOUD_DEAL_WAIT);

			delete pJob;
			pJob = NULL;
		}
	}  
	m_oListJob.RemoveAll();
}

void CCloudDocDealMgr::ReleaseTh()
{
	CCriticalSection2::Owner ow(m_cs4ListTh);

	theLog.Write(_T("CCloudDocDealMgr::ReleaseTh,1,count=%d"), m_oListTh.GetCount());

	POSITION pos = m_oListTh.GetHeadPosition();
	for (int i = 0; i < m_oListTh.GetCount(); i++)
	{
		CCloudDocDealThread* pTh = (CCloudDocDealThread*)m_oListTh.GetNext(pos);
		if (pTh)
		{
			CThreadMgr::DestroyThread(pTh, INFINITE);
			pTh = NULL;
		}
	}  
	m_oListTh.RemoveAll();
}

void CCloudDocDealMgr::ReleaseHelperTh()
{
	theLog.Write(_T("CCloudDocDealMgr::ReleaseHelperTh,1,m_pHelperTh=%p"), m_pHelperTh);
	if (m_pHelperTh)
	{
		CThreadMgr::DestroyThread(m_pHelperTh, INFINITE);
		m_pHelperTh = NULL;
	}
	theLog.Write(_T("CCloudDocDealMgr::ReleaseHelperTh,2,m_pHelperTh=%p"), m_pHelperTh);
}
