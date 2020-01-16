#pragma once

// CCloudDocDealThread

class CCloudDocDealMgr;

class CCloudDocDealThread : public CWinThread
{
	DECLARE_DYNCREATE(CCloudDocDealThread)

protected:
	CCloudDocDealThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CCloudDocDealThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void SetOwner(CCloudDocDealMgr* pOwner);
	BOOL IsActive();
	void SetActive(BOOL bActive = TRUE);

protected:
	DECLARE_MESSAGE_MAP()

	void OnTimer(WPARAM wp,LPARAM lp);
	void OnEndTh(WPARAM wp,LPARAM lp);
	void EnableCheckCloudDocDeal(BOOL bEnable);
	void EnableCheckDbConn(BOOL bEnable);
	void CheckCloudDocDeal();
	void CheckAuditProcess(PCloudJobInfo pInfo);
	CString GetTimeStr(double fTime);

protected:
	UINT_PTR m_nCloudDocDealTimer;
	UINT_PTR m_nCheckDbConnTimer;
	CCloudDocDealMgr* m_pOwner;
	long m_lActiveDealDoc;
};
