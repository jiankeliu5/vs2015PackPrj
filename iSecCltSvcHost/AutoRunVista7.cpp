#include "StdAfx.h"
#include "AutoRunVista7.h"
#include "Userenv.h"
#pragma comment(lib, "Userenv.lib")

extern BOOL IsRunEnu();	//add by zfq,2013.10.22

CAutoRunVista7::CAutoRunVista7(void)
{
}

CAutoRunVista7::~CAutoRunVista7(void)
{
}


void CAutoRunVista7::OnTimerProtect()
{
	if (!NeedProtect())
		return;

	//theLog.Write(_T("CAutoRunVista7::OnTimerProtect,2,before proccount = %d"), m_ProcList.GetCount());
	for (int i = 0; i < m_ProcList.GetCount(); i++)
	{
		POSITION pos = m_ProcList.FindIndex(i);
		if (pos == NULL)
			continue;
		PROCESSINFO pi = m_ProcList.GetAt(pos);
		//检查进程状态,这里会改变pi，所以要重新获取
		int nState = CheckProcessState(pos);
		pi = m_ProcList.GetAt(pos);
		//theLog.Write(_T("pos:%d, session:%d, state:%d."), pos, pi.dwSession, nState);
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
	if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount) || dwCount<=0)
	{
		theLog.Write(_T("CAutoRunVista7::OnTimerProtect,5,Enum Session fail, count = %d, err=%u"), dwCount, GetLastError());
	}

	for (int i = 0; i < dwCount; i++)
	{
		WTS_SESSION_INFO* pInfo = pSessionInfo;
		pInfo += i;
		if (pInfo)
		{
			//theLog.Write(_T("CAutoRunVista7::OnTimerProtect,6,SessionId=%d--WinStationName=%s--%s."), pInfo->SessionId, pInfo->pWinStationName, 
			//	(pInfo->State - WTSActive < 10) ? saWTSState[pInfo->State - WTSActive] : "");
			BOOL bInDesktop = FALSE;
			if (pInfo->State == WTSActive)
				bInDesktop = TRUE;
			if (CProcessExp::ProcessExist(CCommonFun::GetDefaultPath() + m_szProtectName, pInfo->SessionId, TRUE)) //add by zfq,2013.10.22
			{
				//本session有个进程存在
				POSITION pos = GetProcPosition(pInfo->SessionId);
				if (pos == NULL)
				{
					PROCESSINFO pi;
					pi.dwSession = pInfo->SessionId;
					pi.bSessionExist = TRUE;
					pi.nLostNet = 0;
					pi.nLostMain = 0;
					m_ProcList.AddTail(pi);
					theLog.Write(_T("添加一个session%d"), pi.dwSession);
				}
			}

			if (m_bOneInstance)
			{
				//一台机器只运行一个实例,只运行connected或active的
				if (pInfo->State != WTSActive/* && pInfo->State != WTSConnected*/)
				{
					//存在Active状态的时候把非active的杀掉杀掉
					if (HasActiveSession())
					{
						POSITION pos = GetProcPosition(pInfo->SessionId);
						if (pos)
						{
							TerminateProcess(pInfo->SessionId);
							m_ProcList.RemoveAt(pos);
						}
						//theLog.Write(_T("pass session %d"), pInfo->SessionId);
						continue;
					}
				}
			}
			if (pInfo->SessionId > 0 && pInfo->State == WTSActive)
			{
				POSITION pos = GetProcPosition(pInfo->SessionId);
				if (pos == NULL)
				{
					if (m_bOneInstance && m_ProcList.GetCount() > 0)
					{
						theLog.Write(_T("bOneInstance but list count > 0"));
						continue;
					}
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
					if (pInfo->State == WTSActive/* || pInfo->State == WTSConnected*/)
					{
						//判断桌面是否正确
						BOOL bStudentDesktop = IsProcessInDefault(pInfo->SessionId);
						if (bInDesktop == bStudentDesktop)
						{
							//正确
						}
						else
						{
							//不正确
							theLog.Write(_T("should in desktop :%d, actually:%d"), bInDesktop, bStudentDesktop);
							TerminateProcess(pInfo->SessionId);
							RunProcess(pInfo->SessionId, bInDesktop);
						}
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

BOOL CAutoRunVista7::RunProcess(DWORD dwActiveSessionId, BOOL bInDefaultDesktop)
{
	theLog.Write(_T("CAutoRunVista7::RunProcess,session: %d, bInDefaultDesktop = %d"), dwActiveSessionId, bInDefaultDesktop);
	bool bCreatSuccess = false;
	LPVOID lpEnvironment  = NULL;
	HANDLE hToken = NULL;
	HANDLE hToken2 = NULL;
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
		//ASSERT(0);
		_ENVSTRING_t stEnvData;
		//BOOL bUnicode =  TRUE;
		if (bInDefaultDesktop)
		{
			if (!ReadUserEnvirnment(dwActiveSessionId, stEnvData))
			{
				theLog.Write(_T("default: failed to read user envirnment."));
//				return FALSE;	//del by zfq,2015-04-29 下面创建CreateEnvironmentBlock
			}
			//bUnicode = 1;(*(LPBYTE(stEnvData.pData) + 1) == 0);
		}
		//theLog.Write(_T("env size:%d"), stEnvData.nSize);
		if (stEnvData.nSize == 0)
		{
			if(!CreateEnvironmentBlock(&lpEnvironment, hToken2, FALSE))
			{
				theLog.Write(_T("CreateEnvironmentBlock fail,env size:%d,err:%u"), stEnvData.nSize, GetLastError());
			}
		}
		else
		{
			lpEnvironment = stEnvData.pData;
		}

		//WCHAR wcFileName[260] = {0};
		//int nRet = MultiByteToWideChar(CP_GB2312,0,cFileName,_tcsclen(cFileName)+1,wcFileName,260);
		
			STARTUPINFOW StartInfo;
			memset(&StartInfo,0,sizeof(StartInfo));
			PROCESS_INFORMATION pinfo;
			memset(&pinfo,0,sizeof(pinfo));
			StartInfo.cb = sizeof(STARTUPINFOW);
			if (bInDefaultDesktop)
			{
				StartInfo.lpDesktop = L"winsta0\\default";	
			}
			else
			{
				StartInfo.lpDesktop = L"winsta0\\winlogon";	
			}
			if(!CreateProcessAsUserW(
				hToken2,
				NULL,//(WCHAR*)cFileName,
				cFileName,
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
				//| IsTextUnicode((LPVOID)stEnvData.pData, stEnvData.nSize, NULL) ? CREATE_UNICODE_ENVIRONMENT : NULL ,
				bInDefaultDesktop ? lpEnvironment : NULL,
				//lpEnvironment,
				NULL,
				&StartInfo,
				&pinfo) )
			{
				theLog.Write(_T("use wide char to create failed  error:%d 文件名：%s"),::GetLastError(),cFileName);
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
	if (lpEnvironment)
	{
		DestroyEnvironmentBlock(lpEnvironment);
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
	if (lpEnvironment)
	{
		DestroyEnvironmentBlock(lpEnvironment);
	}
	return FALSE;

}

BOOL CAutoRunVista7::HasActiveSession()
{
	WTS_SESSION_INFO* pSessionInfo = NULL;
	DWORD dwCount = 0;
	WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);
	for (int i = 0; i < dwCount; i++)
	{
		WTS_SESSION_INFO* pInfo = pSessionInfo;
		pInfo += i;
		if (pInfo)
		{
			if (pInfo->State == WTSActive)
			{
				//theLog.Write(_T("Has active session%d"), pInfo->SessionId);
				return TRUE;
			}
		}
	}
	WTSFreeMemory(pSessionInfo);
	theLog.Write(_T("Not has active session"));
	return FALSE;
}