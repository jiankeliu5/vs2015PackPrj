#include "StdAfx.h"
#include "WinFireWallOp.h"
#include "FileLog.h"

CWinFireWallOp::CWinFireWallOp(void)
{
	
}

CWinFireWallOp::~CWinFireWallOp(void)
{
}

void CWinFireWallOp::Install(CString sPath,CString sParam,DWORD dwWait /* = TRUE*/,DWORD dwShowCmd)
{
	CFileLog theLog;
//	theLog.EnableLog(TRUE);
	theLog.SetLogName(_T("Install"));

	int nLen = sPath.GetLength() + sParam.GetLength() + 10;
	TCHAR* pszPath = new TCHAR[nLen];
	ASSERT(pszPath);
	memset(pszPath,0,nLen * sizeof(TCHAR));

	_stprintf(pszPath,_T("\"%s\" %s"),sPath,sParam);
	STARTUPINFO si = { sizeof si, 0, _T("") };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = dwShowCmd;

	theLog.Write(_T("Install pathandParam = %s"),pszPath);
	PROCESS_INFORMATION pi;
	if(CreateProcess(0, 
		pszPath,// command line
		0, 0,  // security attributes
		FALSE, // handle inheritance
		0,     // creation flags
		0,     // environment block
		0,     // current directory
		&si, &pi ))
	{
		theLog.Write(_T("createprocess pid = %d"),pi.dwProcessId);
		//	if(bWait)
		{
			::WaitForSingleObject(pi.hProcess,dwWait);
		}
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}
	else
	{
		CString s;
		s.Format(_T("Install Ê§°Ü err = %d \n"),::GetLastError());
		theLog.Write(s);
	}
	if (pszPath)
	{
		delete pszPath;
		pszPath = NULL;
	}
}

void CWinFireWallOp::CloseFireWall()
{
	TCHAR szSysDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szSysDir,MAX_PATH + 1);

	CString sPath;
	sPath.Format(_T("%s\\%s"),szSysDir,"netsh.exe");

	CString sParam = _T("firewall set opmode disable");

	Install(sPath,sParam);
}
void CWinFireWallOp::EnableExceptMode()
{
	TCHAR szSysDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szSysDir,MAX_PATH + 1);

	CString sPath;
	sPath.Format(_T("%s\\%s"),szSysDir,"netsh.exe");

	CString sParam = _T("firewall set opmode mode=ENABLE exception=enable ");

	Install(sPath,sParam);
}
void CWinFireWallOp::ExceptProgram(CString sProgram)
{
	TCHAR szSysDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szSysDir,MAX_PATH + 1);

	CString sPath;
	sPath.Format(_T("%s\\%s"),szSysDir,"netsh.exe");

	CString sParam;
	if (CCommonFun::IsWinVistaOrLater())
	{
		sParam.Format(_T("advfirewall firewall add rule name=\"iSecStar\" dir=in protocol=any program=\"%s\" action=allow"),sProgram);
	}
	else
	{
		sParam.Format(_T("firewall set allowedprogram \"%s\" iSecStar enable"),sProgram);;
	}
	Install(sPath,sParam);
}
void CWinFireWallOp::ExceptUDPPort(UINT nPort)
{
	TCHAR szSysDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szSysDir,MAX_PATH + 1);

	CString sPath;
	sPath.Format(_T("%s\\%s"),szSysDir,"netsh.exe");

	CString sParam;
	sParam.Format(_T("firewall set portopening udp %d enable "),nPort);

	Install(sPath,sParam);
}
void CWinFireWallOp::ExceptTCPPort(UINT nPort)
{
	TCHAR szSysDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szSysDir,MAX_PATH + 1);

	CString sPath;
	sPath.Format(_T("%s\\%s"),szSysDir,"netsh.exe");

	CString sParam;
	sParam.Format(_T("firewall set portopening tcp %d enable "),nPort);
	Install(sPath,sParam);
}

BOOL CWinFireWallOp::IsWinFireWallEnabled()
{
	CString sPath("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile"); 
	CRegistry2 reg(2,TRUE);
	if (reg.Open(sPath))
	{
		DWORD dwVal = 0;
		if(reg.Read(_T("EnableFirewall"),dwVal) && (dwVal == 1))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CWinFireWallOp::IsExceptModeEnabled()
{
	CString sPath(_T("SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile")); 
	CRegistry2 reg(2,TRUE);
	if (reg.Open(sPath))
	{
		DWORD dwVal = 0;
		if(reg.Read(_T("DoNotAllowExceptions"),dwVal) && (dwVal == 1))
		{
			return FALSE;
		}
	}
	return TRUE;
}