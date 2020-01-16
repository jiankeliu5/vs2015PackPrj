#pragma once
#include "autorun.h"
#pragma warning(disable:4996)
#if 0
**************************************************************************************************
// Multiple user
----------------------------------------------
| Session 0  [System]
| Session 1  [user 1]
| Session 2  [user 2]
| Session 3  [user 3]
----------------------------------------------

//single user
------------------------------------------------------------------------------------------------------------------------------------------
{session 1}[user 1](Console_Connect) -$$- {session 1}[user 1](Session_Logon) - 
{session 1}[user 1](Session_Logoff)-{session 1}[user 1](Console_DisConnect) -
{session 2}[user 2](session_Connect)
------------------------------------------------------------------------------------------------------------------------------------------

//user switch
//{session 0}[system](Console_Connect) - {session 0}[user 1](Session_Logon) - 
//{session 0}[user 1](SESSION_LOCK)-{session 1}[user 2](Session_Logon)-{session 1}[user 2](SESSION_LOCK)-
//{session 1}[user 2](Console_DisConnect)-{session 0}[user 1](Console_Connect)-{session 0}[user 1](Session_UnLock)

1个用户登录：0--Services--WTSDisconnected. 1--Console--WTSActive.
win+L：
win+L后选择用户：0--Services--WTSDisconnected.	 2----WTSDisconnected. 3--Console--WTSConnected.
****************************************************************************************************
#endif
class CAutoRunVista7 :
	public CAutoRun
{
public:
	CAutoRunVista7(void);
	~CAutoRunVista7(void);

public:
	virtual void OnTimerProtect();
	BOOL	RunProcess(DWORD dwActiveSessionId, BOOL bInDefaultDesktop);
	BOOL	HasActiveSession();//是否有active session
};
