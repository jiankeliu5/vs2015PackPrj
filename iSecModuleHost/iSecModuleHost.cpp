// iSecClientHost.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "iSecModuleHost.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;
CFileLog theLog;

BOOL g_bExitWindows = FALSE;
BOOL g_bEndAll = FALSE;
UINT_PTR		g_nIDEvent = 0;//���exe�Ƿ���������
CSvcHost theService(LOCAL_SVC_NAME);

#pragma data_seg("iSecModuleHost_data") 
long g_lInstance = 0;
#pragma data_seg() 
#pragma comment(linker,"/Section:iSecModuleHost_data,rws")
using namespace std;

CSvcHost::CSvcHost(TCHAR* szName) : CService(szName)
{
}

CSvcHost::~CSvcHost(void)
{
}

void CSvcHost::OnStart()
{
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnStart"));
	//::Begin(); //todol
	Initial();
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnStart,2"));
}
void CSvcHost::OnStop()
{
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnStop"));
	//::Stop();//todol
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnStop,2"));
	CService::OnStop();
	theLog.Write(_T("iSecModuleHost,CSvcHost::,3"));
	Release();
	theLog.Write(_T("iSecModuleHost,CSvcHost::,4"));
}
void CSvcHost::OnShutdown()
{
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnShutdown"));
	//::Stop();//todol
	CService::OnShutdown();
}

BOOL CSvcHost::OnServiceInstalled(SC_HANDLE svc)
{
	theLog.Write(_T("iSecModuleHost,CSvcHost::OnServiceInstalled."));
	SetFailedAction(svc);
	SetDelayedAutoStart(svc);
	return TRUE;
}

BOOL CSvcHost::OnServiceUninstalled(SC_HANDLE svc)
{
	theLog.Write(_T("OnServiceUninstalled."));
	SetFailedAction(svc, SC_ACTION_NONE);
	return TRUE;
}

void CSvcHost::Initial()
{
	g_nIDEvent = ::SetTimer(NULL, g_nIDEvent, 5000, TimerProc);
	//theLog.Write("id = %d", g_nIDEvent);
}

void CSvcHost::Release()
{
}

void CALLBACK CSvcHost::TimerProc(HWND hwnd,
	UINT uMsg,
	UINT_PTR idEvent,
	DWORD dwTime
)
{
	///if (g_EndAll)
	//	return;

	//theLog.Write(_T("timer id :0x%x"), idEvent);
	if (g_nIDEvent == idEvent)//���student.exe�Ƿ���������
	{
		//WriteLog("timer protect");
		theService.m_oAutoMgr.GetAutoRun()->SetProtectName(_T("iSecTouchStationUi.exe"),_T("/auto"));
		theService.m_oAutoMgr.GetAutoRun()->OnTimerProtect();
		//CLocalThreadMgr::GetInstance().m_bShowUiProcess;
	}
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;
	theLog.SetLogName(_T("iSecModuleHost"));
	theLog.EnableLog(TRUE);
	CMiniDumper::Enable(_T("iSecModuleHost"), true);

	if (InterlockedIncrement(&g_lInstance) > 1)
	{
		theLog.Write(_T("!!��������Ѿ����� g_lInstance = %d"), g_lInstance);
		InterlockedDecrement(&g_lInstance);
		return -1;
	}

    HMODULE hModule = ::GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        // ��ʼ�� MFC ����ʧ��ʱ��ʾ����
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: ���Ĵ�������Է���������Ҫ
            wprintf(L"����: MFC ��ʼ��ʧ��\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
        }
    }
    else
    {
        // TODO: ���Ĵ�������Է���������Ҫ
        wprintf(L"����: GetModuleHandle ʧ��\n");
        nRetCode = 1;
    }

	if (argc == 2 && (_tcsicmp(argv[1], _T("/runasapp")) == 0))
	{
		DWORD dwStatus = CCommonFun::GetSvcStatus(LOCAL_SVC_NAME);
		if (dwStatus == SERVICE_RUNNING)
		{
			printf("iSecModuleHost�Ѿ�����������ʹ��������������\n");
			InterlockedDecrement(&g_lInstance);
			return nRetCode;
		}
		theService.OnStart();
		printf("iSecModuleHost��������\n");
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_TIMER)
			{
				DispatchMessage(&msg);
			}
		}
		printf("�����˳�iSecModuleHost,�Ե� ...\n");
		theService.OnStop();
		printf("iSecModuleHost���˳���\n");
	}
	else
	{
		nRetCode = theService.main(argc, argv, envp);
	}
	InterlockedDecrement(&g_lInstance);
    return nRetCode;
}
