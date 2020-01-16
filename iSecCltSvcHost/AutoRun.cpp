#include "StdAfx.h"
#include "AutoRun.h"
#include "AutoRunvista7.h"
#include "AutoRunXP2003.h"

BOOL IsRunEnu();	//add by zfq,2013.10.22

extern  BOOL			g_bExitWindows;
extern  BOOL			g_bEndAll;
BOOL g_bIsWinXpOrLater;
UINT	GetOSVersion()
{
	//Rather than using GetVersionEx to determine the OS or version number,
	//test for the presence of the feature itself.

	HMODULE hModule = LoadLibrary(_T("Kernel32.dll"));
	if (hModule == NULL)
		return OS_VERSION_UNKOWN;
	PFGetVersionEx pfGetVersionEx = (PFGetVersionEx)GetProcAddress(hModule, "GetVersionExA");
	FreeLibrary(hModule);
	hModule = NULL;
	if (pfGetVersionEx == NULL)
		return OS_VERSION_UNKOWN;
	OSVERSIONINFOEXS osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXS));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXS);
	if (!pfGetVersionEx((OSVERSIONINFO*)&osvi))
		return OS_VERSION_UNKOWN;

	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 ) //Vista or Windows Server 2008
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
				return OS_VERSION_VISTA;
			return OS_VERSION_2008;
		}
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 ) //Vista or Windows Server 2008
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
				return OS_VERSION_WIN7;
			return OS_VERSION_2008;
		}
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			return OS_VERSION_2003;
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			return OS_VERSION_XP;
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			return OS_VERSION_2000;
	}
	else if (VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId)
	{
		return OS_VERSION_9X;
	}
	return OS_VERSION_UNKOWN;
}
CAutoRunMgr::CAutoRunMgr()
{
	m_pAutoRun = NULL;
}
CAutoRunMgr::~CAutoRunMgr()
{
	delete m_pAutoRun;
	m_pAutoRun = NULL;
}
CAutoRun* CAutoRunMgr::GetAutoRun()
{
	if (m_pAutoRun == NULL)
	{
		UINT nOSVersion = GetOSVersion();
		theLog.Write(_T("osVersion:%d"), nOSVersion);
		if (nOSVersion <= OS_VERSION_2003)
		{
			CAutoRunXP2003* pAutoRun = new CAutoRunXP2003();
			m_pAutoRun = pAutoRun;
		}
		else
		{
			CAutoRunVista7* pAutoRun = new CAutoRunVista7();
			m_pAutoRun = pAutoRun;
		}
	}
	ASSERT(m_pAutoRun);
	return m_pAutoRun;
}

CAutoRun::CAutoRun(void)
{
	m_bProtectEnabled = TRUE;
	m_bOneInstance = TRUE;
	m_nProtectCountDown = 0;
}

CAutoRun::~CAutoRun(void)
{
}

void CAutoRun::OnServiceStart()
{
	RunProcess(GetCurrentSessionID());
	m_bSetDeskSwitchTimer = TRUE;
}

void CAutoRun::OnServiceStop()
{
	m_ProcList.RemoveAll();

	if (0/*bTerminatExe*/)
	{
		for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; )
		{
			PROCESSINFO pi = m_ProcList.GetNext(pos);
			TerminateProcess(pi.dwSession);
		}
	}
}
void CAutoRun::OnConsoleConnect(DWORD dwActiveSID)
{
	//RunProcess(dwActiveSID);
}
void CAutoRun::OnConsoleDisconnect(DWORD dwSession)
{
	//
}
void CAutoRun::OnSessionLogon(DWORD dwSession)
{
	m_nProtectCountDown = 0;
	OnLogon(dwSession);
}
void CAutoRun::OnSessionLogoff(DWORD dwSession)
{
	m_nProtectCountDown = 3;
	RemoveProcessInfo(dwSession, TRUE, FALSE);
}
void CAutoRun::OnTimerProtect()
{
	theLog.Write(_T("####CAutoRun::OnTimerProtect"));
	if (m_szProtectName.IsEmpty())
	{
		return;
	}
	if (!NeedProtect())
	{
		theLog.Write(_T("不保护，返回。"));
		return;
	}
	//KillTimer(NULL, g_nIDEvent);
	theLog.Write(_T("count: %d."), m_ProcList.GetCount());
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		int nState = CheckProcessState(pos);
		theLog.Write(_T("pos:%d, session:%d, state%d."), pos, pi.dwSession, nState);
		if (nState & ACTION_TERMINATE)
		{
			theLog.Write(_T("%d进程存在但不正常"), pi.dwSession);
			//MessageBox(NULL, "123", "", MB_OK);
			//kill student
			if (!TerminateProcess(pi.dwSession))
			{
				theLog.Write(_T("终止失败"));
				goto RET;
			}
		}
		if (nState & ACTION_RUN)
		{
			theLog.Write(_T("重新运行进程%d， 必须在winlogon桌面：%d"), pi.dwSession, pi.bMustInWinlogon);
			pi.nLostMain = 0;
			pi.nLostNet = 0;
			m_ProcList.SetAt(pos, pi);
			RunProcess(pi.dwSession, pi.bMustInWinlogon ? IN_WINLOGON : IN_ANY);
		}
	}
RET:
	;
	//if (!g_EndAll)
	//	g_nIDEvent = ::SetTimer(NULL, g_nIDEvent, PROTECT_ELAPSE, TimerProc);
}
void CAutoRun::OnTimerSwitchDesk()
{
	//theLog.Write(_T("DESKSWTICHTIMER == idEvent"));
	if (!m_bSetDeskSwitchTimer)
	{
		//theLog.Write(_T("bSetDeskSwitchTimer == false."));
		return;
	}

	//只检查session 0
	if (GetCurrentSessionID() != 0)
	{
		//KillTimer(NULL, g_nDeskSwitchEvent);
		m_bSetDeskSwitchTimer = FALSE;
		theLog.Write(_T("session is not 0, bSetdesktopswtich = false."));
		return;
	}
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		if (pi.dwSession == 0)
		{
			//判断session 0的进程是否在winlogon桌面，若已经在default桌面，则返回
			BOOL bDiff = !IsProcessInDefault(pi.dwSession);

			if (bDiff)
			{
				if (IsDefaultDestop())
				{
					//进程是在winlogon，但windows已经进入default桌面
					theLog.Write(_T("Switch desktop, NOT kill desktop swtich timer."));
					OnLogon(0);
				}
			}
			else if(CProcessExp::ProcessExist(CCommonFun::GetDefaultPath() + m_szProtectName, pi.dwSession, TRUE))	//add by zfq,2013.10.22
			{
				//当前已经在default了
				if (g_bIsWinXpOrLater)
					m_bSetDeskSwitchTimer = FALSE;
				else
				{
					//2000一直检查
				}
				theLog.Write(_T("student.exe is in default desktop, bSetdesktopswtich = false."));

				//AllowDesktopAccessToUser("WinStat0", "WinLogon", "everyone");
#if 0
				CString strUser;
				if (CProcessExp::GetProcessUser(CCommonFun::GetDefaultPath() + m_szProtectName, 0, strUser, TRUE))
				{
					theLog.Write(_T("获取学生程序用户名成功"));
					DWORD dw = AllowDesktopAccessToUser("WinStat0", "WinLogon", strUser);
					theLog.Write(_T("AllowDesktopAccessToUser Ret %X"), dw);
				}
#endif
			}
			else
			{
				theLog.Write(_T("student.exe not exist."));
			}
		}
	}
}

//收到一个进程状态响应
void CAutoRun::ResetProcess(DWORD dwSession, BOOL bMain)
{
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		if (pi.dwSession == dwSession)
		{
			if (bMain)
				pi.nLostMain = 0;
			else
				pi.nLostNet = 0;
			theLog.Write(_T("reset process, session:%d, bmain:%d"), dwSession, bMain);
			m_ProcList.SetAt(pos, pi);
		}
	}
}

//一个用户登录
void CAutoRun::OnLogon(DWORD dwSession)
{
	theLog.Write(_T("On Logon: session:%d"), dwSession);

	//先从保护列表中删除
	RemoveProcessInfo(dwSession, FALSE, FALSE);

	//终止在winlogon创建的进程
	TerminateProcess(dwSession);

	//确保进程在default桌面创建
	RunProcess(dwSession, IN_DEFAULT);
	if (dwSession == 0 && !m_bSetDeskSwitchTimer)
	{
		m_bSetDeskSwitchTimer = TRUE;
		//theLog.Write(_T("m_ndesktopswtichevent:%d"), m_nDeskSwitchEvent);
	}
}

BOOL CAutoRun::IsDefaultDestop()
{
	DWORD dwSession = GetCurrentSessionID();
	theLog.Write(_T("current session is %d in isdefaultdesktop"), dwSession);
	if (CProcessExp::ProcessExist(NORMALUSER_EXE_NAME, dwSession))
		return TRUE;
	return FALSE;

	//--------------
	//服务一直在session 0桌面，
	//WTSEnumerateSessions
	//不能跨session
	HDESK hDesk = OpenInputDesktop(DF_ALLOWOTHERACCOUNTHOOK,
		TRUE, 
		READ_CONTROL|WRITE_DAC|
		DESKTOP_WRITEOBJECTS|DESKTOP_READOBJECTS);
	if(!hDesk)
		return FALSE;
	DWORD dwSize = 1024;
	BYTE  pBuf[1024];
	CString sDskName;
	BOOL  bRet = TRUE;
	bRet = GetUserObjectInformation(hDesk,2,pBuf,1024,&dwSize);
	if(bRet)
	{
		sDskName.Format(_T("%s"),(char*)pBuf);
#pragma message(__LOC__"登录session 0， 登录session 1， 注销session 0， 杀死session1进程，获取桌面出错")
		theLog.Write(_T("---IsDefaultDsk::CurDskName = %s-----"),sDskName);
		bRet = (sDskName.CompareNoCase(_T("Default")) == 0) ? TRUE : FALSE;
	} 
	return bRet;				
}
//添加一个进程到保护列表，一个session只有添加一个进程
void CAutoRun::AddProcessInfo(DWORD dwSession, BOOL bMustInWinlogon)
{
	//同一个session只添加一次
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		if (pi.dwSession == dwSession)
		{
			pi.bMustInWinlogon = bMustInWinlogon;
			m_ProcList.SetAt(pos, pi);
			return;
		}
	}
	PROCESSINFO pi;
	pi.nLostNet = 0;
	pi.nLostMain = 0;
	pi.dwSession = dwSession;
	pi.bMustInWinlogon = bMustInWinlogon;
	m_ProcList.AddTail(pi);
	TCHAR szText[64];
	wsprintf(szText, _T("Add session:%d"),dwSession);
	theLog.Write(szText);
}


//bLogoff:是否是因为注销而删除
//bOneInstance：是否因为确保只有一个实例而删除
void CAutoRun::RemoveProcessInfo(DWORD dwSession, BOOL bLogoff, BOOL bOneInstance)
{
	theLog.Write(_T("remove session:%d"), dwSession);
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL;)
	{
		if (m_ProcList.GetAt(pos).dwSession == dwSession)
		{
			m_ProcList.RemoveAt(pos);
			break;
		}
		m_ProcList.GetNext(pos);
	}
	//theLog.Write(_T("count after remove %d."),3 )
	//不为0的session注销后，若不存在session0用户，windows会WTS_CONSOLE_CONNECT；
	//session0用户注销后不会WTS_CONSOLE_CONNECT
	//最后要留一个

	//当是确保只有一个实例运行时一个不留
	if (!bOneInstance && dwSession == 0 && m_ProcList.GetCount() == 0)
		AddProcessInfo(0, bLogoff);//所有用户都注销后在winlogon桌面创建进程
	else
	{
		theLog.Write(_T("bOneInstance = %d, dwSession = %d, proclist count = %d"),
			bOneInstance, dwSession, m_ProcList.GetCount());
	}
}

//判断进程有没有进入桌面
BOOL CAutoRun::IsProcessInDefault(DWORD dwSession)
{
	BOOL bDiff = FALSE;
	if (RunInSystem())
	{
		_ENVSTRING_t stEnvDataStu;
		_ENVSTRING_t stEnvDataUser;

		if (ReadUserEnvirnment(dwSession, stEnvDataUser) 
			&& CProcessExp::ReadEnvironmentBlock(CCommonFun::GetDefaultPath()
			+ m_szProtectName, dwSession, stEnvDataStu))	//add by zfq,2013.10.22
		{
			bDiff = stEnvDataStu.nSize != stEnvDataUser.nSize;
			for (int  i = 0 ; !bDiff && i < stEnvDataStu.nSize; i++)
			{
				if (*((LPBYTE)(stEnvDataStu.pData) + i) != *((LPBYTE)(stEnvDataUser.pData) + i))
				{
					bDiff = TRUE;
					//theLog.Write(_T("envirment is different, %d:%d--%d"), i, *((LPBYTE)(stEnvDataStu.pData) + i), *((LPBYTE)(stEnvDataUser.pData) + i));
					break;
				}
			}
		}
		else
		{
			theLog.Write(_T("failed to read env."));
		}
	}
	else
	{
		CString strUser;
		if (CProcessExp::GetProcessUser(CCommonFun::GetDefaultPath() + m_szProtectName, dwSession, strUser, TRUE)) //add by zfq,2013.10.22
		{
			if (strUser.CompareNoCase(_T("System")) == 0)
			{
				theLog.Write(_T("GetProcessUser 进程用户名：%s"), strUser);
				bDiff = TRUE;
			}
		}
		else
		{
			theLog.Write(_T("failed to get student user."));
		}
	}
	return !bDiff;
}
int	CAutoRun::CheckProcessState(POSITION pos)
{
#pragma message(__LOC__"此函数会改变内容，调用后需要重新获取")
	//	return PROCESS_OK;
	//#pragma message(__LOC__"取消进程检查jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj")
	if (pos == NULL)
		return PROCESS_OK;
	int nRet = PROCESS_OK;
	PROCESSINFO pi = m_ProcList.GetAt(pos);
	//theLog.Write(_T("CheckProcessState: pos%d, session:%d"), pos, pi.dwSession);
	pi.nLostMain++;
	pi.nLostNet++;
	m_ProcList.SetAt(pos, pi);
	//if (pi.nLostNet > 1 || pi.nLostMain > 1)
	//theLog.Write(_T("CAutoRun::CheckProcessState: session:%d, main:%d, net:%d,m_szProtectName=%s"), pi.dwSession, pi.nLostMain, pi.nLostNet, m_szProtectName);
	if (CProcessExp::ProcessExist(CCommonFun::GetDefaultPath() + m_szProtectName, pi.dwSession, TRUE)) //add by zfq,2013.10.22
	{
		// [12/27/2011 Songqx]
		// 不通讯了。
#if 0
		if (pi.nLostNet > MAX_LOST || pi.nLostMain > MAX_LOST)
			nRet = PROCESS_ILL;
		else
			nRet = PROCESS_OK;
#else
		nRet = PROCESS_OK;
#endif
		//	theLog.Write(_T("CheckProcessState: checkprocess, nret%d"), nRet);
	}
	else
		nRet = PROCESS_NOTEXSIT;
	//theLog.Write(_T("CheckProcessState: pos:%d, session:%d, state:%d"), pos, pi.dwSession, nRet);
	return nRet;
}

BOOL CAutoRun::TerminateProcess(DWORD dwSession)
{
	theLog.Write(_T("terminate process:%d"), dwSession);
	return CCommonFun::TerminaPID(CProcessExp::GetProcessIdByName(CCommonFun::GetDefaultPath() + m_szProtectName, dwSession)); //add by zfq,2013.10.22
}
void CAutoRun::EnableProtect(BOOL bEnable)
{
	m_bProtectEnabled = bEnable;
	theLog.Write(_T("m_bProtectEnabled = %d"), m_bProtectEnabled);
}
BOOL CAutoRun::NeedProtect()
{
#if 0
	//#ifdef _DEBUG
	return FALSE;
#endif
	if (g_bExitWindows || g_bEndAll)
		return FALSE;

	if (!m_bProtectEnabled)
		return FALSE;

	BOOL bNoprect = FALSE;
	if (m_nProtectCountDown > 0)
	{
		m_nProtectCountDown -= 3;
		bNoprect = TRUE;
		theLog.Write(_T("m_nProtectCountDown = %d"), m_nProtectCountDown);
	}
	if (bNoprect)
		return FALSE;
	do 
	{
		HANDLE hMapFile;
		char* pBuf;
		hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			MAP_NAME_NO_PROTECT);               // name of mapping object 
		if (hMapFile == NULL) 
		{ 
			break;
		} 
		pBuf = (char*)MapViewOfFile(hMapFile,    // handle to mapping object
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,                    
			0,                    
			1);                   

		if (pBuf == NULL) 
		{ 
			CloseHandle(hMapFile);
			break;
		}
		if (*pBuf == 1)
			bNoprect = TRUE;
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}while (0);
	if (bNoprect)
		return FALSE;

	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, PROJECT_REGISTY_KEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwVal, dwSize = sizeof(DWORD);
		if(RegQueryValueEx(hKey, _T("NoProtect"), NULL, NULL, (PBYTE)&dwVal, &dwSize) == ERROR_SUCCESS)
		{
			bNoprect = dwVal;
		}
		RegCloseKey(hKey);
	}
	return !bNoprect;
}

//将dwCurrentSession外的session中的进程结束掉
void CAutoRun::TerminateOtherInstance(DWORD dwCurrentSession)
{
	if (!m_bOneInstance)
	{
		return;
	}

	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		if (pi.dwSession != dwCurrentSession)
		{
			theLog.Write(_T("Terminate other instance:%d"), pi.dwSession);
			RemoveProcessInfo(pi.dwSession, FALSE, TRUE);
			TerminateProcess(pi.dwSession);
		}
	}
}
void CAutoRun::TerminateAll()
{
	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		RemoveProcessInfo(pi.dwSession, FALSE, TRUE);
		TerminateProcess(pi.dwSession);
	}
}
BOOL CAutoRun::RunProcess(DWORD dwActiveSessionId, ProcessDesktop pDesk)
{
	//theLog.Write(_T("cs0"));
	CCriticalSection2::Owner ow(m_cs);
	//theLog.Write(_T("cs1"));

	if (m_bOneInstance)
	{
		TerminateOtherInstance(dwActiveSessionId);
	}
	//创建不成功也添加, 服务会继续创建进程
	AddProcessInfo(dwActiveSessionId, 0);
	theLog.Write(_T("CAutoRun::RunProcess,3,runprocess: session: %d, pdesk = %d,m_szProtectName=%s"), dwActiveSessionId, (int)pDesk, m_szProtectName);

	HANDLE hToken = NULL;
	HANDLE hToken2= NULL;
	bool bCreatSuccess = false;
	//只分配了100，下面打路径会挂。
	//char cMsg[100];
	try
	{
		TCHAR cFileName[260] = {'\0'};
		TCHAR sPath[MAX_PATH];
		if(::GetModuleFileName(0, sPath, MAX_PATH)==0)
			goto ERR0;

		TCHAR* pEnd=NULL;
		pEnd= _tcsrchr(sPath,_T('\\'));
		if(pEnd)
			*pEnd=0;

		_tcscpy_s(cFileName,sPath);
		_tcscat(cFileName,_T("\\"));
		_tcscat(cFileName, m_szProtectName); //add by zfq,2013.10.22
		if (!m_szParam.IsEmpty())
		{
			_tcscat(cFileName, _T(" "));
			_tcscat(cFileName, m_szParam.GetString());
		}

		BOOL   bDefaultDesktop = (pDesk == IN_DEFAULT) || (pDesk != IN_WINLOGON && IsDefaultDestop());
		if(!FindToken(hToken, dwActiveSessionId, bDefaultDesktop) || !hToken)
		{
			theLog.Write(_T("Find Token error"));
			goto ERR0;
		}
		else
		{
			//theLog.Write(_T("find token successfully"));
			BOOL bRet = DuplicateTokenEx(hToken,0,NULL,SecurityAnonymous,TokenPrimary,&hToken2);//复制一下Token才能创建用户进程成功
			if(bRet)
			{
				//theLog.Write(_T("DuplicateTokenEx success"));
			}
			else
			{
				//sprintf(cMsg,);
				theLog.Write(_T("DuplicateTokenEx error:%d"),GetLastError());
				goto ERR1;
			}
		}
		//LPVOID lpEnvironment  = NULL;
		//ASSERT(0);
		//CreateEnvironmentBlock(&lpEnvironment, hToken2, FALSE);
		_ENVSTRING_t stEnvData;
		//BOOL bUnicode =  TRUE;
		if (bDefaultDesktop)
		{
			if (!ReadUserEnvirnment(dwActiveSessionId, stEnvData))
			{
				theLog.Write(_T("default: failed to read user envirnment."));
				goto ERR2;
			}
			//bUnicode = 1;(*(LPBYTE(stEnvData.pData) + 1) == 0);
		}
		//theLog.Write(_T("env size:%d"), stEnvData.nSize);

		WCHAR wcFileName[260] = {0};
		//int nRet = MultiByteToWideChar(CP_GB2312,0,cFileName,_tcsclen(cFileName)+1,wcFileName,260);
	
		STARTUPINFOW StartInfo;
		memset(&StartInfo, 0, sizeof(StartInfo));
		PROCESS_INFORMATION pinfo;
		memset(&pinfo, 0, sizeof(pinfo));
		StartInfo.cb = sizeof(STARTUPINFOW);
		if (bDefaultDesktop)
		{
			StartInfo.lpDesktop = L"winsta0\\default";
		}
		else
		{
			StartInfo.lpDesktop = L"winsta0\\winlogon";
		}
		if (!CreateProcessAsUserW(
			hToken2,
			NULL,//(WCHAR*)wcFileName,
			cFileName,
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
			//| IsTextUnicode((LPVOID)stEnvData.pData, stEnvData.nSize, NULL) ? CREATE_UNICODE_ENVIRONMENT : NULL ,
			bDefaultDesktop ? (LPVOID)stEnvData.pData : NULL,
			//lpEnvironment,
			NULL,
			&StartInfo,
			&pinfo))
		{
			theLog.Write(_T("use wide char to create failed  error:%d 文件名：%s"), ::GetLastError(), cFileName);
		}
		else
		{
			//theLog.Write(_T("use wide char to create  successfully, sessionid: %d"), dwActiveSessionId);
			bCreatSuccess = true;
		}
		

		//如果创建失败，则使用多字节字符创建
		if(!bCreatSuccess)
		{
			STARTUPINFO StartInfo;
			memset(&StartInfo,0,sizeof(StartInfo));
			PROCESS_INFORMATION pinfo;
			memset(&pinfo,0,sizeof(pinfo));
			if (bDefaultDesktop)
			{
				StartInfo.lpDesktop = _T("winsta0\\default");
			}
			else
			{
				StartInfo.lpDesktop = _T("winsta0\\winlogon");
			}	
			StartInfo.cb = sizeof(STARTUPINFO);

			if(!CreateProcessAsUser(
				hToken2,
				(LPCTSTR)cFileName,
				0,
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&StartInfo,
				&pinfo) )
			{
				//sprintf(cMsg,);
				theLog.Write(_T("------user multi char to create error:%d 文件名：%s"),::GetLastError(),cFileName);
			}
			else
			{
				bCreatSuccess = true;
				//theLog.Write(_T("------user multi char to create eguard.exe successfully"));
				//AddProcessInfo(dwActiveSessionId, 0);
			}
		}
	}
	catch(...)
	{
		//sprintf(cMsg,);
		theLog.Write(_T("catch2 error:%d"),::GetLastError());
		goto ERR2;
	}

	if (hToken2)
	{
		CloseHandle(hToken2);
	}
	if (hToken)
	{
		CloseHandle(hToken);
	}
	return bCreatSuccess ? TRUE : FALSE;

ERR2:
	if (hToken2)
	{
		CloseHandle(hToken2);
	}
ERR1:
	if (hToken)
	{
		CloseHandle(hToken);
	}
ERR0:
	return FALSE;
}

DWORD  CAutoRun::GetCurrentSessionID()
{
	DWORD nRet = 0;
	HMODULE hWtsLib = NULL;

	hWtsLib = LoadLibrary(_T("Kernel32.dll"));

	WTSGETACTIVECONSOLESESSIONID WTSGetActiveConsoleSessionId = NULL;
	if (hWtsLib)
	{
		WTSGetActiveConsoleSessionId = 
			(WTSGETACTIVECONSOLESESSIONID)GetProcAddress( (HMODULE)hWtsLib, 
			"WTSGetActiveConsoleSessionId" );

		if (WTSGetActiveConsoleSessionId)
		{
			DWORD dwSessionID =  WTSGetActiveConsoleSessionId();
			nRet = dwSessionID;		
		}
		else
			theLog.Write(_T("Get WTSGetActiveConsoleSessionId错误"));

		::FreeLibrary(hWtsLib);
	}
	else
	{
		theLog.Write(_T("Load libray error"));
	}
	return nRet;
}

BOOL CAutoRun::FindToken(HANDLE& hProcessToken,DWORD dwActiveSessionId, BOOL bInDefaultDesktop)
{
	TCHAR szExeName[MAX_PATH];
	//#pragma message(__LOC__"******************************始终system用户")
	if (RunInSystem())
		bInDefaultDesktop = FALSE;
	if (bInDefaultDesktop)
		_tcscpy_s(szExeName, NORMALUSER_EXE_NAME);
	else
		_tcscpy_s(szExeName, SYSTEMUSER_EXE_NAME);
	//theLog.Write(_T("Find Token %s, bDefaultDesktop:%d, session:%d."), szExeName, bInDefaultDesktop, dwActiveSessionId);
	return CProcessExp::GetProcessToken(szExeName, dwActiveSessionId, hProcessToken);	
}
//判断是否将学生进程创建为system权限
BOOL CAutoRun::RunInSystem()
{
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, PROJECT_REGISTY_KEY, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwVal, dwSize = sizeof(DWORD);
		if(RegQueryValueEx(hKey, _T("RunSystem"), NULL, NULL, (PBYTE)&dwVal, &dwSize) == ERROR_SUCCESS)
		{
			return dwVal;
		}
		RegCloseKey(hKey);
	}
	return FALSE;
}
//读取某一用户的环境，explorer进程不存在时失败
BOOL CAutoRun::ReadUserEnvirnment(DWORD dwSession, _ENVSTRING_t& stEnvData)
{
	TCHAR szExplorerPath[MAX_PATH + 1];
	GetWindowsDirectory(szExplorerPath, MAX_PATH);
	_tcscat(szExplorerPath, _T("\\"));
	_tcscat(szExplorerPath, NORMALUSER_EXE_NAME);
	//theLog.Write(_T("Read session:%d, %s"), dwSession, szExplorerPath);
	BOOL bRet =  CProcessExp::ReadEnvironmentBlock(szExplorerPath, dwSession, stEnvData);
	//theLog.Write(_T("Read env over"));
	return bRet;
}

POSITION CAutoRun::GetProcPosition(DWORD dwSession)
{
	POSITION pos = m_ProcList.GetHeadPosition();
	for (; pos != NULL; m_ProcList.GetNext(pos))
	{
		if (m_ProcList.GetAt(pos).dwSession == dwSession)
			return pos;
	}
	return pos;
}

BOOL CAutoRun::RunProcess(CString strPath, DWORD dwActiveSessionId, HANDLE hToken, BOOL bInDefaultDesktop)
{
	bool bCreatSuccess = false;
	HANDLE hToken2 = NULL;
	try
	{
		TCHAR cFileName[260] = {'\0'};
		TCHAR sPath[MAX_PATH];
		if(::GetModuleFileName(0, sPath, MAX_PATH)==0)
			return FALSE;

		TCHAR* pEnd=NULL;
		pEnd= _tcsrchr(sPath,_T('\\'));
		if(pEnd)
			*pEnd=0;

		_tcscpy_s(cFileName, sPath);
		_tcscat(cFileName, _T("\\"));
		_tcscat(cFileName, strPath);
		if (!m_szParam.IsEmpty())
		{
			_tcscat(cFileName, _T(" "));
			_tcscat(cFileName, m_szParam.GetString());
		}
		if(hToken == NULL)
		{
			theLog.Write(_T("token == Null."));
			return FALSE;
		}
		else
		{
			//theLog.Write(_T("find token successfully"));
			BOOL bRet = DuplicateTokenEx(hToken,0,NULL,SecurityAnonymous,TokenPrimary,&hToken2);//复制一下Token才能创建用户进程成功
			if(bRet)
			{
				//theLog.Write(_T("DuplicateTokenEx success"));
			}
			else
			{
				theLog.Write(_T("DuplicateTokenEx error:%d"),GetLastError());
				goto ERR0;
			}
		}
		//LPVOID lpEnvironment  = NULL;
		//ASSERT(0);
		//CreateEnvironmentBlock(&lpEnvironment, hToken2, FALSE);
		_ENVSTRING_t stEnvData;
		//BOOL bUnicode =  TRUE;
		if (bInDefaultDesktop)
		{
			if (!ReadUserEnvirnment(dwActiveSessionId, stEnvData))
			{
				theLog.Write(_T("default: failed to read user envirnment."));
				goto ERR2;
			}
			//bUnicode = 1;(*(LPBYTE(stEnvData.pData) + 1) == 0);
		}
		//theLog.Write(_T("env size:%d"), stEnvData.nSize);

		WCHAR wcFileName[260] = {0};
		//int nRet = MultiByteToWideChar(CP_GB2312,0,cFileName,_tcsclen(cFileName)+1,wcFileName,260);
		STARTUPINFOW StartInfo;
		memset(&StartInfo, 0, sizeof(StartInfo));
		PROCESS_INFORMATION pinfo;
		memset(&pinfo, 0, sizeof(pinfo));
		StartInfo.cb = sizeof(STARTUPINFOW);
		if (bInDefaultDesktop)
		{
			StartInfo.lpDesktop = L"winsta0\\default";
		}
		else
		{
			StartInfo.lpDesktop = L"winsta0\\winlogon";
		}
		if (!CreateProcessAsUserW(
			hToken2,
			NULL,
			wcFileName,
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
			//| IsTextUnicode((LPVOID)stEnvData.pData, stEnvData.nSize, NULL) ? CREATE_UNICODE_ENVIRONMENT : NULL ,
			bInDefaultDesktop ? (LPVOID)stEnvData.pData : NULL,
			//lpEnvironment,
			NULL,
			&StartInfo,
			&pinfo))
		{
			theLog.Write(_T("use wide char to create failed  error:%d 文件名：%s"), ::GetLastError(), cFileName);
		}
		else
		{
			//theLog.Write(_T("use wide char to create  successfully, sessionid: %d"),	dwActiveSessionId);
			bCreatSuccess = true;
		}
		

		//如果创建失败，则使用多字节字符创建
		if(!bCreatSuccess)
		{
			STARTUPINFO StartInfo;
			memset(&StartInfo,0,sizeof(StartInfo));
			PROCESS_INFORMATION pinfo;
			memset(&pinfo,0,sizeof(pinfo));
			if (bInDefaultDesktop)
			{
				StartInfo.lpDesktop = _T("winsta0\\default");
			}
			else
			{
				StartInfo.lpDesktop = _T("winsta0\\winlogon");
			}	
			StartInfo.cb = sizeof(STARTUPINFO);

			if(!CreateProcessAsUser(
				hToken2,
				NULL,//(LPCSTR)cFileName,
				cFileName,
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&StartInfo,
				&pinfo) )
			{
				theLog.Write(_T("------user multi char to create error:%d 文件名：%s"),::GetLastError(),cFileName);
			}
			else
			{
				bCreatSuccess = true;
				//theLog.Write(_T("------user multi char to create eguard.exe successfully"));
				//AddProcessInfo(dwActiveSessionId, 0);
			}
		}
	}
	catch(...)
	{
		theLog.Write(_T("catch2 error:%d"),::GetLastError());
		goto ERR2;
	}

	if (hToken2)
	{
		CloseHandle(hToken2);
	}
	return bCreatSuccess ? TRUE : FALSE;

ERR2:
	if (hToken2)
	{
		CloseHandle(hToken2);
	}
ERR0:
	return FALSE;
}

//add by zfq,2013.10.22
#ifndef SYS_CONFIG_FILE
#	define SYS_CONFIG_FILE			_T("config\\config.ini")
#endif	//#ifndef SYS_CONFIG_FILE

BOOL IsRunEnu()
{
	BOOL BUseEnu = FALSE;	//是否使用英文版
	{
		CIniFile theIniFile;
		theIniFile.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
		int nUseMultiLanguage = theIniFile.GetVal(_T("Ext"), _T("DynLanguage"), 1);
		if(1 == nUseMultiLanguage)
		{
			int nValue = theIniFile.GetVal(_T("Ext"), _T("LanguageType"), UILANG_CHINESE);
			if(UILANG_ENGLISH == nValue)
			{
				BUseEnu = TRUE;
			}
		}
		//theLog.Write(_T("nUseMultiLanguage=%d,BUseEnu=%d"), nUseMultiLanguage, BUseEnu);
	}

	return BUseEnu;
}