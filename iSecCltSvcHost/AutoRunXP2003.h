#pragma once
#include "autorun.h"
#pragma warning(disable:4996)

#if 0
**************************************************************************************************
// Multiple user
----------------------------------------------
| Session 0  [System] [user 1]
| Session 1  [user 2]
| Session 2  [user 3]
----------------------------------------------

//single user
------------------------------------------------------------------------------------------------------------------------------------------
{session 0}[system](Console_Connect) -$$- {session 0}[user 1](Session_Logon) - 
{session 0}[user 1](Session_Logoff)-{session 0}[system](Console_Connect)-{session 0}[user 2](session_Logon)
------------------------------------------------------------------------------------------------------------------------------------------

//user switch
{session 0}[system](Console_Connect) - {session 0}[user 1](Session_Logon) - 
{session 0}[user 1](SESSION_LOCK)-{session 1}[user 2](Session_Logon)-{session 1}[user 2](SESSION_LOCK)-
{session 1}[user 2](Console_DisConnect)-{session 0}[user 1](Console_Connect)-{session 0}[user 1](Session_UnLock)

1个用户进入windows桌面：0--Console--WTSActive
切换用户界面：0--Console--WTSActive.
2个用户登录：0----WTSDisconnected. 1--Console--WTSActive.
2个用户都注销：0--Console--WTSConnected. 2----WTSIdle.
****************************************************************************************************
#endif
class CAutoRunXP2003 :
	public CAutoRun
{
public:
	CAutoRunXP2003(void);
	~CAutoRunXP2003(void);
	
public:
	virtual void OnTimerProtect();
	BOOL	RunProcess(DWORD dwActiveSessionId, BOOL bInDefaultDesktop);
};
