#pragma once

// CMainThread

class CMainThread : public QObject, public CWinThread
{
	Q_OBJECT
	DECLARE_DYNCREATE(CMainThread)

protected:
	CMainThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CMainThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

	void OnTimer(WPARAM wp,LPARAM lp);
	void OnEndTh(WPARAM wp,LPARAM lp);
	void EnableConnDataBase(BOOL bEnable);
	void EnableCheckDbCloudDoc(BOOL bEnable);
	void EnableCheckConfigChange(BOOL bEnable);
	void CheckDbCloudDoc();
	void KillThis();

protected:
	UINT_PTR m_nConnDBTimer;
	UINT_PTR m_nCheckDbCloudDocTimer;
	UINT_PTR m_nCheckConfigChangeTimer;
	CCloudDocDealMgr* m_pCloudDocDealMgr;

signals:
	void CmdToUi(int nMainCmd, int nSubCmd, void* pData);

protected slots:
	void onCmdFromUi(int nMainCmd, int nSubCmd, void* pData);
};
