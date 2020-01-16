#pragma once


extern UINT GetOSVersion();

enum	ProcessDesktop
{
	IN_DEFAULT = 0,
	IN_WINLOGON = 1,
	IN_ANY = 2
};

#define ACTION_DONOTHING			0x00
#define ACTION_TERMINATE			0x01
#define ACTION_RUN					0x10

#define PROCESS_OK					ACTION_DONOTHING
#define PROCESS_NOTEXSIT			ACTION_RUN
#define PROCESS_ILL					ACTION_TERMINATE | ACTION_RUN

typedef	struct tagProcessInfo
{
	DWORD	dwSession;
	int		nLostMain;
	int		nLostNet;
	BOOL	bMustInWinlogon;
	BOOL    bSessionExist;//进程session是否存在
}PROCESSINFO;
static CString saWTSState[10] = {_T("WTSActive"),
_T("WTSConnected"),
_T("WTSConnectQuery"),
_T("WTSShadow"),
_T("WTSDisconnected"),
_T("WTSIdle"),
_T("WTSListen"),
_T("WTSReset"),
_T("WTSDown"),
_T("WTSInit") };
class CAutoRun
{
public:
	CAutoRun(void);
	~CAutoRun(void);
public:
	virtual void OnServiceStart();//服务启动
	virtual void OnServiceStop();//服务终止
	virtual void OnConsoleConnect(DWORD dwSession);
	virtual void OnConsoleDisconnect(DWORD dwSession);
	virtual void OnSessionLogon(DWORD dwSession);
	virtual void OnSessionLogoff(DWORD dwSession);

	virtual void OnTimerProtect();
	virtual void OnTimerSwitchDesk();

	virtual void	ResetProcess(DWORD dwSession, BOOL bMain);
public:
	static DWORD	GetCurrentSessionID();
	
	BOOL	IsProcessInDefault(DWORD dwSession);//判断进程有没有进入default桌面
	void	EnableProtect(BOOL bEnable);
	void    TerminateAll();

	BOOL	RunProcess(DWORD dwActiveSessionId, ProcessDesktop pDesk = IN_ANY);
	void SetProtectName(CString szProtectName, CString szParam)
	{
		m_szProtectName = szProtectName;
		m_szParam = szParam;
	}


protected:
	virtual BOOL	IsDefaultDestop();

protected:
	//将dwCurrentSession外的session中的进程结束掉
	void	TerminateOtherInstance(DWORD dwCurrentSession);

	void	OnLogon(DWORD dwSession);

	BOOL    RunProcess(CString strPath, DWORD dwActiveSessionId, HANDLE hToken, BOOL bInDefaultDesktop);
	int		CheckProcessState(POSITION pos);//得到进程状态
	BOOL	FindToken(HANDLE& hProcessToken, DWORD dwActiveSessionId, BOOL bInDefaultDesktop);
	void	AddProcessInfo(DWORD dwSession, BOOL bMustInWinlogon = FALSE);
	void	RemoveProcessInfo(DWORD dwSession,BOOL bLogoff,  BOOL bOneInstance);

	//无关函数
	BOOL    ReadUserEnvirnment(DWORD dwSession, _ENVSTRING_t& stEnvData);//读取某一用户的环境，explorer进程不存在时失败
	BOOL	NeedProtect();
	BOOL	RunInSystem();
	BOOL	TerminateProcess(DWORD dwSession);

	POSITION GetProcPosition(DWORD dwSession);//得到进程列表中的位置
protected:
	CList<PROCESSINFO, PROCESSINFO> m_ProcList;
	int			m_nProtectCountDown;//注销3秒开始保护
	BOOL		m_bOneInstance;
	CString m_szProtectName = _T("");
	CString m_szParam = _T("");
private:
	BOOL    m_bSetDeskSwitchTimer;//检查进程是否在正确的桌面上
	BOOL	m_bProtectEnabled;
	CCriticalSection2	m_cs;
};

class CAutoRunMgr
{
public:
	CAutoRunMgr();
	~CAutoRunMgr();
	CAutoRun* GetAutoRun();
private:
	CAutoRun* m_pAutoRun;
};