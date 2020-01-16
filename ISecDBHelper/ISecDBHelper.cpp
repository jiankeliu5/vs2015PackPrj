// AsSpy.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "ISecDBHelper.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
CWinApp theApp;
using namespace std;
#define SERVICENAME _T("ISecDbHelper")
#define DBBACKPROCNAME _T("DbBackupConfig.exe")
#define DBBACKINTERVAL 1001
#define DBBACKTIMER	60*1000

//////////////////////////////////////////////////////////////////////////
// 服务控制
void InstallWntService(bool bstart);
void UninstallService();
void StopService();
void ChangeServiceOp(LPTSTR SysName);

//////////////////////////////////////////////////////////////////////////
// 时间控制
static void CALLBACK TimerDayDbProcs(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void MyServiceStart(DWORD argc, LPTSTR *argv);
void WINAPI MyServiceCtrlHandler(DWORD opcode);
void GetDBBackProcPath(CString& sProcPath);

//////////////////////////////////////////////////////////////////////////
// 使用变量
SERVICE_STATUS          MyServiceStatus;
SERVICE_STATUS_HANDLE   MyServiceStatusHandle;
DWORD					m_dwThreadID = 0;
UINT					m_nTimeInterval = 0;
BOOL					m_bAutoDBBacked = FALSE;
COleDateTime			m_dPrevDateTime;
SERVICE_TABLE_ENTRY DispatchTable[]=
{
	{
		SERVICENAME,
		(LPSERVICE_MAIN_FUNCTION)MyServiceStart
	},
	{	NULL,
		NULL
	}
};

//////////////////////////////////////////////////////////////////////////
// 函数实现
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{	
	//在NT中安装并启动这个服务
	if( argc > 1 && (_tcsicmp(argv[1], _T("-Install")) == 0))
	{
		InstallWntService(true);
		ChangeServiceOp(SERVICENAME);				
		return 0;
	}
	//在NT中安装这个服务
	else if( argc > 1 && (_tcsicmp(argv[1], _T("-InstallOnly")) == 0))
	{
		InstallWntService(false);
		return 0;
	}
	//卸载服务
	else if( argc > 1 && (_tcsicmp(argv[1], _T("-Unstall")) == 0))
	{		
		UninstallService();		
		return 0;
	}
	//停止服务
	else if( argc > 1 && (_tcsicmp(argv[1], _T("-stop")) == 0))
	{
		StopService();
		return 0;
	}
	StartServiceCtrlDispatcher(DispatchTable);
	return 0;
}

void MyServiceStart(DWORD argc,LPTSTR *argv) 
{
	m_dwThreadID = GetCurrentThreadId();
	MyServiceStatusHandle = RegisterServiceCtrlHandler(SERVICENAME,MyServiceCtrlHandler); 
	if (MyServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
		return; 

	// Initialization complete - report running status. 
	MyServiceStatus.dwServiceType        = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS; 
	MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP;
	MyServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
	MyServiceStatus.dwWin32ExitCode		 = NO_ERROR;
	MyServiceStatus.dwCheckPoint         = 0; 
	MyServiceStatus.dwWaitHint           = 0;  
	SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus);

	// 开启事件
	m_nTimeInterval = ::SetTimer(NULL,DBBACKINTERVAL,DBBACKTIMER,TimerDayDbProcs);
	m_dPrevDateTime = COleDateTime::GetCurrentTime();
	m_dPrevDateTime.ParseDateTime(m_dPrevDateTime.Format(VAR_DATEVALUEONLY),VAR_DATEVALUEONLY);

	// 事件循环
	BOOL bRet;
	MSG msg;
	while((bRet = GetMessage(&msg,NULL,0,0)) != 0)
	{
		if (bRet==0)
			break;

		if(msg.message == WM_QUIT || msg.message == WM_CLOSE) 
			break;

		if (bRet > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 关闭计时器
	if (m_nTimeInterval != 0)
	{
		::KillTimer(NULL,m_nTimeInterval);
		m_nTimeInterval = 0;
	}

	// 设置服务为停止状态
	MyServiceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(MyServiceStatusHandle,&MyServiceStatus); 
	return;
}

VOID WINAPI MyServiceCtrlHandler(DWORD Opcode) 
{
	switch(Opcode) 
	{ 
	case SERVICE_CONTROL_PAUSE:
		MyServiceStatus.dwCurrentState = SERVICE_PAUSED; 
		break;

	case SERVICE_CONTROL_CONTINUE:
		MyServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;

	case SERVICE_CONTROL_STOP:
		::PostThreadMessage(m_dwThreadID,WM_QUIT,0,0);
		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;
	}

	// 设置服务状态
	SetServiceStatus(MyServiceStatusHandle,&MyServiceStatus);
	return; 
} 

static void CALLBACK TimerDayDbProcs(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
#if 0
	//add by zhandb 2014.6.30
	//处理演示版本加密文件自动过期的问题
	CString file;
	file.Format(_T("%siSecBSvcHost.exe"),CCommonFun::GetDefaultPath());
	
	if((_access(file, 0) != -1) && (ERROR_SERVICE_ALREADY_RUNNING != CCommonFun::StartSvc("iSecBSvcHost")))
	{
		CCommonFun::WriteFileLog(_T("iSecBSvcHost ERROR_SERVICE_ALREADY_RUNNING"));
		UnZipCrp();
	}

	file.Format(_T("%siSecASvcHost.exe"),CCommonFun::GetDefaultPath());
	if((_access(file, 0) != -1)  && (ERROR_SERVICE_ALREADY_RUNNING != CCommonFun::StartSvc("iSecASvcHost")))
	{
		CCommonFun::WriteFileLog(_T("iSecASvcHost ERROR_SERVICE_ALREADY_RUNNING"));
		UnZipCrp();
	}
#endif

	CString sPath;
	BOOL bAutoBack;
	int nInterval;
	double dTime;
	BACKTYPE type;
	CDBBackFun::GetDBBackConfig(sPath,bAutoBack,type,nInterval,dTime);
	if (!bAutoBack)
		return;

	// 得到当前时间
	COleDateTime dcur = COleDateTime::GetCurrentTime();
	
	// 如果已经过去了一天
	COleDateTime dtToday;
	dtToday.ParseDateTime(dcur.Format(VAR_DATEVALUEONLY),VAR_DATEVALUEONLY);
	if (dtToday.m_dt > m_dPrevDateTime.m_dt)
		m_bAutoDBBacked = FALSE;

	// 根据类型进行判断
	switch (type)
	{
		// 每天备份
	case BACKTYPE_BYDAY:
		{
			dcur.ParseDateTime(dcur.Format(VAR_TIMEVALUEONLY),VAR_TIMEVALUEONLY);
			if (dcur.m_dt > dTime && !m_bAutoDBBacked)
				m_bAutoDBBacked = TRUE;
			else
				return;
		}
		break;

		// 每个月
	case BACKTYPE_BYMONTH:
		{
			if (dcur.GetDay() != nInterval)
				return;

			dcur.ParseDateTime(dcur.Format(VAR_TIMEVALUEONLY),VAR_TIMEVALUEONLY);
			if (dcur.m_dt > dTime && !m_bAutoDBBacked)
				m_bAutoDBBacked = TRUE;
			else
				return;
		}
		break;

		// 每周
	case BACKTYPE_BYWEEK:
		{
			if (dcur.GetDayOfWeek() != nInterval + 1)
				return;

			dcur.ParseDateTime(dcur.Format(VAR_TIMEVALUEONLY),VAR_TIMEVALUEONLY);
			if (dcur.m_dt > dTime && !m_bAutoDBBacked)
				m_bAutoDBBacked = TRUE;
			else
				return;
		}
		break;
	}

	m_dPrevDateTime = COleDateTime::GetCurrentTime();
	m_dPrevDateTime.ParseDateTime(m_dPrevDateTime.Format(VAR_DATEVALUEONLY),VAR_DATEVALUEONLY);

	// 运行传参备份程序
	CString sDBBackFilePath;
	GetDBBackProcPath(sDBBackFilePath);
	CSecurityTool::CreateProcByCurUsr(sDBBackFilePath.GetBuffer(),_T("/s"),SW_SHOW);
	sDBBackFilePath.ReleaseBuffer();
}

void ChangeServiceOp(LPTSTR SysName)
{
    BOOL status = FALSE;
 
	SC_HANDLE  schService;
	SC_HANDLE  schSCManager;
 
	//打开服务管理机
	schSCManager = OpenSCManager (NULL, NULL,SC_MANAGER_ALL_ACCESS);
	if (!schSCManager) 
	{
		return;
	}
	
	schService = OpenService (schSCManager,SysName, SERVICE_ALL_ACCESS);
	if (!schService)
	{
		CloseServiceHandle (schSCManager);	
	   return;
	}	


	SERVICE_FAILURE_ACTIONS sdBuf={0};

	sdBuf.lpRebootMsg=NULL;
	sdBuf.dwResetPeriod=3600*24;

	SC_ACTION action[3];

	action[0].Delay=5*1000;
	action[0].Type=SC_ACTION_RESTART;

	action[1].Delay=5*1000;
	action[1].Type=SC_ACTION_RESTART;
	action[2].Delay=5*1000;
	action[2].Type=SC_ACTION_RESTART;

	sdBuf.cActions=3;
	sdBuf.lpsaActions=action;
	sdBuf.lpCommand=NULL;

	if( !ChangeServiceConfig2(
		schService,                 
		SERVICE_CONFIG_FAILURE_ACTIONS, 
		&sdBuf) )                   
	{
		CCommonFun::WriteFileLog(_T("ChangeServiceConfig2 failed\n"));
	}
	else
		CCommonFun::WriteFileLog(_T(" ChangeServiceConfig2 succeeded\n"));

	CloseServiceHandle (schService);
	CloseServiceHandle (schSCManager);	
	//////////////////////////
	return  ;
}

void InstallWntService(bool bstart)
{
	TCHAR sysdir[MAX_PATH+1];
	::GetModuleFileName(NULL, sysdir, MAX_PATH+1);
	CString sExe = sysdir;
	LPCTSTR lpDependencies = NULL;
	BOOL bRegOK = FALSE;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
	if(scm != NULL)
	{
		CCommonFun::WriteFileLog(_T("OpenSCManager "));
		SC_HANDLE svc  = OpenService( 
			scm,       // SCManager database 
			SERVICENAME,       // name of service 
			SERVICE_START
			);            // only need DELETE access 
		if(svc)
		{
			StartService(svc,0,NULL);	
			CloseServiceHandle(scm);
			return;
		}
		CCommonFun::WriteFileLog(_T("CreateService "));
		svc = CreateService(scm,SERVICENAME,SERVICENAME,
							SERVICE_ALL_ACCESS,
							SERVICE_WIN32_OWN_PROCESS |	SERVICE_INTERACTIVE_PROCESS,
							SERVICE_AUTO_START,//SERVICE_AUTO_START,
							SERVICE_ERROR_IGNORE,
							sysdir,	
							NULL,NULL,
							lpDependencies,
							NULL,NULL);
		if(svc != NULL)
		{
			CCommonFun::WriteFileLog(_T("CreateService suc"));
			if(bstart)
			{
				StartService(svc,0,NULL);
			}
			bRegOK = TRUE;
			CloseServiceHandle(svc);
		}	
		else
		{
			CCommonFun::WriteFileLog(_T("CreateService %d "),::GetLastError());
		}
		
		CloseServiceHandle(scm);
    }
	else
	{
		CCommonFun::WriteFileLog(_T("OpenSCManager %d "),::GetLastError());
	}
}
 
void StopService()
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm != NULL)
	{
		SC_HANDLE svc = OpenService(scm,SERVICENAME,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc,&ssa);
			if (ssa.dwCurrentState == SERVICE_RUNNING) //停止此Service.
				ControlService(svc, SERVICE_CONTROL_STOP, &ssa);
			CloseServiceHandle(svc);
        }                           
		CloseServiceHandle(scm);
    }
}

void UninstallService()
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm != NULL)
	{
		SC_HANDLE svc = OpenService(scm,SERVICENAME,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc,&ssa);
			if (ssa.dwCurrentState == SERVICE_RUNNING) //删除前，先停止此Service.
				ControlService(svc, SERVICE_CONTROL_STOP, &ssa);
			DeleteService(svc);
			CloseServiceHandle(svc);
        }                           
		//以便立即从数据库中移走此条目。
		CloseServiceHandle(scm);
    }
}

void GetDBBackProcPath(CString& sProcPath)
{
	CString sCulFileName;
	GetModuleFileName(NULL,sCulFileName.GetBuffer(_MAX_PATH), _MAX_PATH);
	sCulFileName.ReleaseBuffer();
	int inum = sCulFileName.ReverseFind('\\');
	if (inum == -1)
		return;

	sProcPath.Format(_T("\"%s\\%s\""),sCulFileName.Left(inum),DBBACKPROCNAME);
}