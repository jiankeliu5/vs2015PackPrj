#include "StdAfx.h"
#include "AutoRunxp2003.h"

extern BOOL IsRunEnu();	//add by zfq,2013.10.22

CAutoRunXP2003::CAutoRunXP2003(void)
{
}

CAutoRunXP2003::~CAutoRunXP2003(void)
{
}

void CAutoRunXP2003::OnTimerProtect()
{
	if (!NeedProtect())
		return;

	//theLog.Write(_T("CAutoRunXP2003::OnTimerProtect before proccount = %d"), m_ProcList.GetCount());
	for (int i = 0; i < m_ProcList.GetCount(); i++)
	{
		POSITION pos = m_ProcList.FindIndex(i);
		if (pos == NULL)
			continue;
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		//检查进程状态
		int nState = CheckProcessState(pos);
		pi = m_ProcList.GetAt(pos);
		//theLog.Write(_T("pos:%d, session:%d, state%d."), pos, pi.dwSession, nState);
		if (nState & ACTION_TERMINATE)
		{
			theLog.Write(_T("%d进程存在但不正常"), pi.dwSession);
			if (!TerminateProcess(pi.dwSession))
			{
				theLog.Write(_T("终止失败"));
			}
		}
		if (nState & ACTION_RUN)
		{
			theLog.Write(_T("进程%d不存在或不正常"), pi.dwSession);
			m_ProcList.RemoveAt(pos);
			i--;
		}
		else
		{
			pi.bSessionExist = FALSE;
			m_ProcList.SetAt(pos, pi);
		}
	}

	for (POSITION pos = m_ProcList.GetHeadPosition(); pos != NULL; m_ProcList.GetNext(pos))
	{
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		pi.bSessionExist = FALSE;
		m_ProcList.SetAt(pos, pi);
	}
	//theLog.Write(_T("end proccount = %d"), m_ProcList.GetCount());

	WTS_SESSION_INFO* pSessionInfo = NULL;
	DWORD dwCount = 0;
	//char szServerName[MAX_PATH] = "";
	//HANDLE hServer = WTSOpenServer(szServerName);
	//if (hServer == NULL)
	//{
	//	theLog.Write(_T("failed to open server, err = %d"), GetLastError());
	//}
	BOOL bRet = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);
	//theLog.Write(_T("Enum Session, count = %d, ret = %d, GetLastError = %d"), dwCount, bRet, GetLastError());
	if (bRet == 0)
	{
		theLog.Write(_T("Enum Session,fail, count = %d, ret = %d, GetLastError = %d"), dwCount, bRet, GetLastError());
		//获取失败，默认是0，在default桌面
		POSITION pos = GetProcPosition(0);
		BOOL bInDesktop = IsDefaultDestop();
		if (pos == NULL)
		{
			//创建
			if (RunProcess(0, bInDesktop))
			{
				PROCESSINFO pi;
				pi.dwSession = 0;
				pi.bSessionExist = TRUE;
				pi.nLostNet = 0;
				pi.nLostMain = 0;
				m_ProcList.AddTail(pi);
			}
			else
			{
				theLog.Write(_T("Failed to RunSessionSvc"));
			}
		}
		else
		{
			//判断所在桌面是否正确
			//判断桌面是否正确
			if (bInDesktop == IsProcessInDefault(0))
			{
				//正确
			}
			else
			{//不正确
				theLog.Write(_T("!!CAutoRunXP2003::OnTimerProtect,13,Wrong Desk"));
				TerminateProcess(0);
				RunProcess(0, bInDesktop);
			}
		}
		return;
	}
	for (int i = 0; i < dwCount; i++)
	{
		WTS_SESSION_INFO* pInfo = pSessionInfo;
		pInfo += i;
		if (pInfo)
		{
			//theLog.Write(_T("%u--%s--%s."), pInfo->SessionId, pInfo->pWinStationName, 
			//	(pInfo->State - WTSActive < 10) ? saWTSState[pInfo->State - WTSActive] : "");
			BOOL bInDesktop = FALSE;
			if (pInfo->State == WTSActive)
				bInDesktop = TRUE;

			if (m_bOneInstance)
			{
				//一台机器只运行一个实例,只运行connected或active的
				if (pInfo->State != WTSActive && pInfo->State != WTSConnected)
				{
					//杀掉
					POSITION pos = GetProcPosition(pInfo->SessionId);
					if (pos)
					{
						TerminateProcess(pInfo->SessionId);
					}
					//theLog.Write(_T("pass session %d"), pInfo->SessionId);
					continue;
				}
			}
#pragma message(__LOC__"这里改成>0")
			if (pInfo->SessionId >= 0)
			{
				POSITION pos = GetProcPosition(pInfo->SessionId);
				if (pos == NULL)
				{
					//创建
					if (RunProcess(pInfo->SessionId, bInDesktop))
					{
						PROCESSINFO pi;
						pi.dwSession = pInfo->SessionId;
						pi.bSessionExist = TRUE;
						pi.nLostNet = 0;
						pi.nLostMain = 0;
						m_ProcList.AddTail(pi);
					}
					else
					{
						theLog.Write(_T("Failed to RunSessionSvc"));
					}
				}
				else
				{
					//判断桌面是否正确
					if (bInDesktop == IsProcessInDefault(pInfo->SessionId))
					{
						//正确
					}
					else
					{//不正确
						theLog.Write(_T("!!CAutoRunXP2003::OnTimerProtect,17,Wrong Desk,i=%d,dwCount=%d,SessionId=%d")
							, i, dwCount, pInfo->SessionId);

						TerminateProcess(pInfo->SessionId);
						RunProcess(pInfo->SessionId, bInDesktop);
					}
				}
			}
		}
	}
	WTSFreeMemory(pSessionInfo);
	//把session不存在的服务进程杀掉
	for (int i = 0; i < m_ProcList.GetCount(); i++)
	{
		POSITION pos = m_ProcList.FindIndex(i);
		if (pos == NULL)
			continue;
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		if (pi.bSessionExist)
			continue;

		//杀死进程

		//进程是否存在
		if (CProcessExp::ProcessExist(CCommonFun::GetDefaultPath() + m_szProtectName, pi.dwSession, TRUE)) //add by zfq,2013.10.22
			continue;

		m_ProcList.RemoveAt(pos);
		i--;
	}
}


BOOL CAutoRunXP2003::RunProcess(DWORD dwActiveSessionId, BOOL bInDefaultDesktop)
{
	theLog.Write(_T("CAutoRunXP2003::RunProcess: session: %d, bInDefaultDesktop = %d"), dwActiveSessionId, bInDefaultDesktop);
	HANDLE hToken = NULL;
	HANDLE hToken2 = NULL;
	bool bCreatSuccess = false;
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
		_tcscat(cFileName, _T("\\"));
		_tcscat(cFileName, m_szProtectName); //add by zfq,2013.10.22
		if (!m_szParam.IsEmpty())
		{
			_tcscat(cFileName, _T(" "));
			_tcscat(cFileName, m_szParam.GetString());
		}


		if(!FindToken(hToken, dwActiveSessionId, bInDefaultDesktop) || !hToken)
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
				theLog.Write(_T("DuplicateTokenEx error:%d"),GetLastError());
				goto ERR1;
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

		//WCHAR wcFileName[260] = {0};
		//int nRet = MultiByteToWideChar(CP_GB2312,0, cFileName,_tcsclen(cFileName)+1,wcFileName,260);
		if (bInDefaultDesktop)
		{
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
				cFileName,
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
		else
		{
			//theLog.Write(_T("before ShellExecuteW,1,bCreatSuccess=%d"), bCreatSuccess);
			if (!bCreatSuccess)
			{
				//theLog.Write(_T("before ShellExecuteW,2,bCreatSuccess=%d"), bCreatSuccess);
				HINSTANCE hinst = ::ShellExecuteW(GetDesktopWindow(),NULL,cFileName,NULL,NULL,SW_SHOW);
				int nRet = (int)hinst;
				if (nRet<=32)
				{
					theLog.Write(_T("ShellExecuteW fail,nRet=%d,err=%d"), nRet, GetLastError());
				}
				else
				{
					//theLog.Write(_T("ShellExecuteW succ,nRet=%d,err=%d"), nRet, GetLastError());
					bCreatSuccess = true;
				}
			}

			//theLog.Write(_T("before ShellExecute,1,bCreatSuccess=%d"), bCreatSuccess);
			if (!bCreatSuccess)
			{
				//theLog.Write(_T("before ShellExecute,2,bCreatSuccess=%d"), bCreatSuccess);
				HINSTANCE hinst = ::ShellExecute(GetDesktopWindow(),NULL,cFileName,NULL,NULL,SW_SHOW);
				int nRet = (int)hinst;
				if (nRet<=32)
				{
					theLog.Write(_T("ShellExecute fail,nRet=%d,err=%d"), nRet, GetLastError());
				}
				else
				{
					//theLog.Write(_T("ShellExecute succ,nRet=%d,err=%d"), nRet, GetLastError());
					bCreatSuccess = true;
				}
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
