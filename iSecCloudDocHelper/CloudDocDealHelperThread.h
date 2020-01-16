#pragma once

// CCloudDocDealHelperThread

class CCloudDocDealMgr;

class CCloudDocDealHelperThread : public CWinThread
{
	DECLARE_DYNCREATE(CCloudDocDealHelperThread)

protected:
	CCloudDocDealHelperThread();           // 动态创建所使用的受保护的构造函数
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
	void OnCheckBlockedCloudDoc();	//检测在处理中阻塞的打印文档

protected:
	void ReleaseCloudJobDealProcessInfo();	//释放Office进程信息

	void GetAllUiProcess();		//获取本计算机所有的UI进程
	BOOL IsUiProcess(DWORD dwProcessID);	//判断指定进程ID是否有窗口		
	void ReleaseWindowProcessInfo();		//释放窗口进程信息
	void AddWindowProcessInfo(HWND hWnd);	//添加一个窗口进程信息
	PWindowProcessInfo GetWindowProcessInfo(DWORD dwProcessID);	//获取指定进程ID的窗口进程信息

	void SimulationKeyboard(HWND hWnd, int nVK);	//对指定窗口，模拟键盘输入

protected:
	UINT_PTR m_nCheckBlockedCloudDocTimer;
	CCloudDocDealMgr* m_pOwner;
	long m_lActiveDealDoc;
	CMap<DWORD, DWORD, PCloudJobDealProcessInfo, PCloudJobDealProcessInfo> m_oCloudJobDealProcessInfoMap;	//打印文档处理进程信息
	CMap<DWORD, DWORD, PWindowProcessInfo, PWindowProcessInfo> m_oWindowProcessInfo;	//窗口进程信息
};
