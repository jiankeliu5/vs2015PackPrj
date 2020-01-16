#pragma once

class CCloudDocDealMgr
{
public:
	CCloudDocDealMgr(void);
	~CCloudDocDealMgr(void);

	void InitWork();
	void Release();
	void CheckNeedDealCloudJob();
	BOOL AddOneCloudJob(PCloudJobInfo job);
	PCloudJobInfo GetOneCloudJob();
	DWORD GetCloudJobCount();

protected:
	void InitTh();
	void InitHelperTh();
	void ReleaseJob();
	void ReleaseTh();
	void ReleaseHelperTh();

protected:
	CList<CCloudDocDealThread*> m_oListTh;
	CCloudDocDealHelperThread* m_pHelperTh;
	CCriticalSection2 m_cs4ListTh;
	CListCloudJobInfo m_oListJob;
	CCriticalSection2 m_cs4ListJob;
};
