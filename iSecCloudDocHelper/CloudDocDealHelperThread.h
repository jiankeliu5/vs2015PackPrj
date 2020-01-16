#pragma once

// CCloudDocDealHelperThread

class CCloudDocDealMgr;

class CCloudDocDealHelperThread : public CWinThread
{
	DECLARE_DYNCREATE(CCloudDocDealHelperThread)

protected:
	CCloudDocDealHelperThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CCloudDocDealHelperThread();

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
	CString GetTimeStr(double fTime);

	void EnableCheckBlockedCloudDocTimer(BOOL bEnable);
	void OnCheckBlockedCloudDoc();	//����ڴ����������Ĵ�ӡ�ĵ�

protected:
	void ReleaseCloudJobDealProcessInfo();	//�ͷ�Office������Ϣ

	void GetAllUiProcess();		//��ȡ����������е�UI����
	BOOL IsUiProcess(DWORD dwProcessID);	//�ж�ָ������ID�Ƿ��д���		
	void ReleaseWindowProcessInfo();		//�ͷŴ��ڽ�����Ϣ
	void AddWindowProcessInfo(HWND hWnd);	//���һ�����ڽ�����Ϣ
	PWindowProcessInfo GetWindowProcessInfo(DWORD dwProcessID);	//��ȡָ������ID�Ĵ��ڽ�����Ϣ

	void SimulationKeyboard(HWND hWnd, int nVK);	//��ָ�����ڣ�ģ���������

protected:
	UINT_PTR m_nCheckBlockedCloudDocTimer;
	CCloudDocDealMgr* m_pOwner;
	long m_lActiveDealDoc;
	CMap<DWORD, DWORD, PCloudJobDealProcessInfo, PCloudJobDealProcessInfo> m_oCloudJobDealProcessInfoMap;	//��ӡ�ĵ����������Ϣ
	CMap<DWORD, DWORD, PWindowProcessInfo, PWindowProcessInfo> m_oWindowProcessInfo;	//���ڽ�����Ϣ
};
