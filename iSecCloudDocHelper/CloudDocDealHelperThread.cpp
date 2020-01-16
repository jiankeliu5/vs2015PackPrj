// CloudDocDealHelperThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CloudDocDealHelperThread.h"

IMPLEMENT_DYNCREATE(CCloudDocDealHelperThread, CWinThread)

CCloudDocDealHelperThread::CCloudDocDealHelperThread()
{
	m_nCheckBlockedCloudDocTimer = NULL;
	m_pOwner = NULL;
	SetActive(FALSE);
}

CCloudDocDealHelperThread::~CCloudDocDealHelperThread()
{
}

BOOL CCloudDocDealHelperThread::InitInstance()
{
	theLog.Write(_T("CCloudDocDealHelperThread::InitInstance, m_nThreadID=%u"), m_nThreadID);
	EnableCheckBlockedCloudDocTimer(TRUE);
	return TRUE;
}

int CCloudDocDealHelperThread::ExitInstance()
{
	theLog.Write(_T("CCloudDocDealHelperThread::ExitInstance, m_nThreadID=%u"), m_nThreadID);
	SetActive(FALSE);

	//ֹͣ����������ĵ�
	EnableCheckBlockedCloudDocTimer(FALSE);

	//�ͷ�Office������Ϣ
	ReleaseCloudJobDealProcessInfo();

	//�ͷŴ��ڽ�����Ϣ
	ReleaseWindowProcessInfo();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCloudDocDealHelperThread, CWinThread)
END_MESSAGE_MAP()


//�����߳�
void CCloudDocDealHelperThread::OnEndTh(WPARAM wp, LPARAM lp)
{
	theLog.Write(_T("CCloudDocDealHelperThread::OnEndTh,m_nThreadID=%u"), m_nThreadID);
	ExitInstance();
	AfxEndThread(0);
}

BOOL CCloudDocDealHelperThread::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_TIMER:
		{
			OnTimer(pMsg->wParam,pMsg->lParam);		
		}
		break;
	case WM_THREAD_EXIT:
		{
			OnEndTh(pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
		break;
	}

	return CWinThread::PreTranslateMessage(pMsg);
}

void CCloudDocDealHelperThread::OnTimer(WPARAM wp, LPARAM lp)
{
	if (wp == m_nCheckBlockedCloudDocTimer)
	{
		if (IsActive())
		{
			OnCheckBlockedCloudDoc();
		}
	}
}

void CCloudDocDealHelperThread::SetOwner(CCloudDocDealMgr* pOwner)
{
	m_pOwner = pOwner;
	if (!m_pOwner)
	{
		theLog.Write(_T("!!CCloudDocDealHelperThread::SetOwner,m_pOwner=%p"), m_pOwner);
	}
}

void CCloudDocDealHelperThread::SetActive(BOOL bActive)
{
	if (bActive)
	{
		InterlockedExchange(&m_lActiveDealDoc,TRUE);
	}
	else
	{
		InterlockedExchange(&m_lActiveDealDoc,FALSE);
	}
}

BOOL CCloudDocDealHelperThread::IsActive()
{
	long lVal = 0;
	InterlockedExchange(&lVal,m_lActiveDealDoc);
	return lVal;
}

CString CCloudDocDealHelperThread::GetTimeStr(double fTime)
{
	COleDateTime time;
	time.m_dt = fTime;
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

void CCloudDocDealHelperThread::EnableCheckBlockedCloudDocTimer(BOOL bEnable)
{
	//theLog.Write(_T("CCloudDocDealHelperThread::EnableCheckBlockedCloudDocTimer,bEnable=%d,m_nCheckBlockedCloudDocTimer=%u"), bEnable, m_nCheckBlockedCloudDocTimer);
	if (bEnable)
	{
		if (!m_nCheckBlockedCloudDocTimer)
		{
			m_nCheckBlockedCloudDocTimer = ::SetTimer(NULL,0,Check_Blocked_Cloud_Doc_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nCheckBlockedCloudDocTimer)
		{
			KillTimer(NULL, m_nCheckBlockedCloudDocTimer);
			m_nCheckBlockedCloudDocTimer = NULL;
		}
	}
}

void CCloudDocDealHelperThread::OnCheckBlockedCloudDoc()
{
	//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc"));

	//��ȡ���н��洰�ڽ���
	GetAllUiProcess();

	//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,m_oCloudJobDealProcessInfoMap count=%d")
	//	, m_oCloudJobDealProcessInfoMap.GetCount());

	//��m_oCloudJobDealProcessInfoMap�еĽ�����Ϣ����״̬ΪPROCESS_STATE_NOT_EXIST
	POSITION pos = m_oCloudJobDealProcessInfoMap.GetStartPosition();
	DWORD  dwProcessID = 0;	//����ID
	CloudJobDealProcessInfo* pInfo = NULL;	//������Ϣ
	while (pos != NULL)
	{
		m_oCloudJobDealProcessInfoMap.GetNextAssoc(pos, dwProcessID, pInfo);
		if (pInfo)
		{
			pInfo->oProcessState = PROCESS_STATE_NOT_EXIST;
		}
	}

	//��ǰ���е�office����
	CString szMSOfficeProcessExe(MS_Office_ProcessExe);
	CString szMSOfficeErrorExe(MS_Office_ErrorExe);

	//��ȡ����office����
	PROCESSENTRY32 pe32;//������ſ��ս�����Ϣ��һ���ṹ�塣
	pe32.dwSize = sizeof(pe32);   //�����������Ҫ��Ȼ��ȡ����pe32  

	//��ϵͳ�ڵ����н�����һ������  
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	if (hProcessSnap == INVALID_HANDLE_VALUE)  
	{  
		theLog.Write(_T("!!CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,1,hProcessSnap=%p,err=%u"), hProcessSnap, GetLastError());
		return;  
	}

	BOOL bMore = Process32First(hProcessSnap , &pe32);
	if (!bMore)
	{
		theLog.Write(_T("!!CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,2,err=%u"), GetLastError());
	}

	//�������̿��գ�������ʾÿ�����̵���Ϣ  
	while (bMore)  
	{
		//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,3,szExeFile=[%s],PID=[%u],Threads=[%u]")
		//	, pe32.szExeFile, pe32.th32ProcessID, pe32.cntThreads);
		CString szExeFile(pe32.szExeFile);
		szExeFile.MakeUpper();
		if (szMSOfficeProcessExe.Find(szExeFile) >= 0)
		{
			BOOL bNewProcess = TRUE;
			if (m_oCloudJobDealProcessInfoMap.Lookup(pe32.th32ProcessID, pInfo) && pInfo)
			{
				//�������ID���ڲ��ҽ�������ͬ����״̬����ΪPROCESS_STATE_RUNING
				if (szExeFile.CompareNoCase(pInfo->oProcessEntry32.szExeFile) == 0)
				{
					bNewProcess = FALSE;
					pInfo->oProcessState = PROCESS_STATE_RUNING;
				}
				else
				{//�������ID���ڲ��ҽ�������ͬ�����ʾ�ɵĽ����Ѿ�������Ҫɾ����Ϣ
					delete pInfo;
					m_oCloudJobDealProcessInfoMap.RemoveKey(pe32.th32ProcessID);
				}
			}
			if (bNewProcess)
			{
				pInfo = new CloudJobDealProcessInfo;
				memset(pInfo, 0x0, sizeof(CloudJobDealProcessInfo));
				memcpy(&pInfo->oProcessEntry32, &pe32, sizeof(PROCESSENTRY32));
				pInfo->oProcessState = PROCESS_STATE_RUNING;
				pInfo->fProcessBeginTime = COleDateTime::GetCurrentTime().m_dt;
				m_oCloudJobDealProcessInfoMap.SetAt(pe32.th32ProcessID, pInfo);
			}
		}
		else if (szMSOfficeErrorExe.Find(szExeFile) >= 0)
		{
			//����Office�������
			BOOL bRet = CCommonFun::TerminaPID(pe32.th32ProcessID);
			theLog.Write(_T("##CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,4,TerminaPID,bRet=%d,th32ProcessID=%d,szExeFile=%s")
				, bRet, pe32.th32ProcessID, pe32.szExeFile);
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}  

	//�ر��ں˶���  
	CloseHandle(hProcessSnap);

	//��m_oCloudJobDealProcessInfoMap�еĽ�����ϢΪPROCESS_STATE_NOT_EXIST��ɾ�������ҽ���ʱ����ǿ�ƽ�����
	pos = m_oCloudJobDealProcessInfoMap.GetStartPosition();
	dwProcessID = 0;	//����ID
	pInfo = NULL;	//������Ϣ
	while (pos != NULL)
	{
		m_oCloudJobDealProcessInfoMap.GetNextAssoc(pos, dwProcessID, pInfo);
		if (pInfo)
		{
			//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,4.1,oProcessState=%d,fProcessBeginTime=%s,")
			//	_T("dwProcessID=%d,th32ProcessID=%d,szExeFile=%s")
			//	, pInfo->oProcessState, GetTimeStr(pInfo->fProcessBeginTime), dwProcessID
			//	, pInfo->oProcessEntry32.th32ProcessID, pInfo->oProcessEntry32.szExeFile);

			//�жϽ����Ƿ���ڴ��ڣ�������ڣ����򴰿ڷ���ģ��Enter������
			//��ģ���������ڵ�office�ĵ����ڴ���ʱ���ᵯ����ʾ���Ӷ������̣߳�
			//һ������µ��������ʾ���ĵ����ܻ�ת���ɹ���
			//�������һЩ��ʾ���ڵ��ȷ��֮���ĵ�ת���߳���Ȼ������
			//��ֻ���ڴﵽ������ʱʱ���ǿ�ƽ�����Office���̣��ĵ�ת��ʧ�ܡ�
			PWindowProcessInfo pWindows = GetWindowProcessInfo(dwProcessID);
			if (pWindows)
			{
				theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,4.2,hWnd=%u,dwProcessID=%d,cWindowText=%s")
					, pWindows->hWnd, pWindows->dwProcessID, pWindows->cWindowText);
				if (_tcsicmp(pInfo->oProcessEntry32.szExeFile, MS_Office_Word) == 0)
				{
					//��ʱ�򵯿�Ҫ������Enter,����һ��Esc
					SimulationKeyboard(pWindows->hWnd, VK_ESCAPE);
				}
				else if (_tcsicmp(pInfo->oProcessEntry32.szExeFile, MS_Office_PowerPoint) == 0)
				{
					SimulationKeyboard(pWindows->hWnd, VK_RETURN);
					CString szWindowText(pWindows->cWindowText);
					if (szWindowText.Find(_T("Microsoft PowerPoint ��ȫ����")) >= 0)
					{
						//�����İ汾office2016�£��˴���ֻ�����Ӵ��ڷ�����Ϣ�����ܽ�������
						//����officeû�в��ԣ����δ֪
						HWND hWnd = GetWindow(pWindows->hWnd, GW_CHILD);
						if (hWnd)
						{
							SimulationKeyboard(hWnd, VK_ESCAPE);
						}
					}
				}
				else
				{
					SimulationKeyboard(pWindows->hWnd, VK_RETURN);
				}
			}
			else
			{
				//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,4.3,dwProcessID=%d"), dwProcessID);
			}
			
			if (pInfo->oProcessState == PROCESS_STATE_NOT_EXIST)
			{
				delete pInfo;
				m_oCloudJobDealProcessInfoMap.RemoveKey(dwProcessID);
			}
			else if (pInfo->oProcessState == PROCESS_STATE_RUNING)
			{
				COleDateTime oProcessBeginTime;
				oProcessBeginTime.m_dt = pInfo->fProcessBeginTime;
				COleDateTimeSpan span = COleDateTime::GetCurrentTime() - oProcessBeginTime;
				if (span.GetTotalSeconds() > Max_Wait_Blocked_Cloud_Doc_Time_S)
				{
					BOOL bRet = CCommonFun::TerminaPID(dwProcessID);
					theLog.Write(_T("##CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,5,�ĵ�����ʱ(%d��),ProcessBeginTime=%s,TerminaPID,bRet=%d,dwProcessID=%d,szExeFile=%s")
						, Max_Wait_Blocked_Cloud_Doc_Time_S, oProcessBeginTime.Format(_T("%Y-%m-%d %H:%M:%S")), bRet, dwProcessID, pInfo->oProcessEntry32.szExeFile);
					delete pInfo;
					m_oCloudJobDealProcessInfoMap.RemoveKey(dwProcessID);
				}
			}
		}
	}

	//theLog.Write(_T("CCloudDocDealHelperThread::OnCheckBlockedCloudDoc,end,count=%d"), m_oCloudJobDealProcessInfoMap.GetCount());
}

void CCloudDocDealHelperThread::ReleaseCloudJobDealProcessInfo()
{
	theLog.Write(_T("CCloudDocDealHelperThread::ReleaseCloudJobDealProcessInfo,count=%d"), m_oCloudJobDealProcessInfoMap.GetCount());

	POSITION pos = m_oCloudJobDealProcessInfoMap.GetStartPosition();
	DWORD  dwProcessID = 0;	//����ID
	CloudJobDealProcessInfo* pInfo = NULL;	//������Ϣ
	while (pos != NULL)
	{
		m_oCloudJobDealProcessInfoMap.GetNextAssoc(pos, dwProcessID, pInfo);
		if (pInfo)
		{
			delete pInfo;
		}
	}
	m_oCloudJobDealProcessInfoMap.RemoveAll();
}

#if 0
BOOL CALLBACK EnumWindowsFunc(HWND hWnd, LPARAM lParam)
{
	if (IsWindowVisible(hWnd))
	{
		CCloudDocDealHelperThread* pThis = (CCloudDocDealHelperThread*)lParam;
		if (!pThis)
		{
			theLog.Write(_T("!!EnumWindowsFunc fail,pThis=%p"), pThis);
			return FALSE;
		}

		pThis->AddWindowProcessInfo(hWnd);
	}

	return TRUE;
}
#endif

void CCloudDocDealHelperThread::GetAllUiProcess()
{
	//�����ڽ�����Ϣ
	ReleaseWindowProcessInfo();

	//���»�ȡ���ڽ�����Ϣ
#if 0
	if (!EnumWindows(EnumWindowsFunc, this))
	{
		theLog.Write(_T("!!CCloudDocDealHelperThread::GetAllUiProcess,EnumWindows fail,err=%u"), GetLastError());
	}
#else
	//1.�Ȼ�����洰��
	CWnd* pDesktopWnd = CWnd::GetDesktopWindow();
	if (!pDesktopWnd)
	{
		theLog.Write(_T("!!CCloudDocDealHelperThread::GetAllUiProcess,GetDesktopWindow fail,err=%u"), GetLastError());
		return;
	}
	//2.���һ���Ӵ���
	CWnd* pWnd = pDesktopWnd->GetWindow(GW_CHILD);
	//3.ѭ��ȡ�������µ������Ӵ���
	while(pWnd != NULL)
	{
		if (pWnd->IsWindowVisible())
		{
			AddWindowProcessInfo(pWnd->m_hWnd);
		}

		//������һ���Ӵ���
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
#endif
}

BOOL CCloudDocDealHelperThread::IsUiProcess(DWORD dwProcessID)
{
	WindowProcessInfo* pInfo = NULL;	//���ڽ�����Ϣ
	if (m_oWindowProcessInfo.Lookup(dwProcessID, pInfo))
	{
		return TRUE;
		//ͬһ�����̿����ж��UI���ڣ������߳�ID���ܲ���ͬ����ʱ������ֻ����һ����Ϣ
	}
	else
	{
		return FALSE;
	}
}

void CCloudDocDealHelperThread::ReleaseWindowProcessInfo()
{
	//theLog.Write(_T("CCloudDocDealHelperThread::ReleaseWindowProcessInfo,count=%d"), m_oWindowProcessInfo.GetCount());

	POSITION pos = m_oWindowProcessInfo.GetStartPosition();
	DWORD  dwProcessID = 0;	//����ID
	WindowProcessInfo* pInfo = NULL;	//���ڽ�����Ϣ
	while (pos != NULL)
	{
		m_oWindowProcessInfo.GetNextAssoc(pos, dwProcessID, pInfo);
		if (pInfo)
		{
			delete pInfo;
		}
	}
	m_oWindowProcessInfo.RemoveAll();
}

void CCloudDocDealHelperThread::AddWindowProcessInfo(HWND hWnd)
{
	//��ȡ���ڵ��߳�ID�ͽ���ID
	DWORD dwProcessID = 0;
	DWORD dwThreadID = 0;
	dwThreadID = GetWindowThreadProcessId(hWnd, &dwProcessID);

	WindowProcessInfo* pWindows = NULL;	//���ڽ�����Ϣ
	if (m_oWindowProcessInfo.Lookup(dwProcessID, pWindows))
	{
		//ͬһ�����̿����ж��UI���ڣ������߳�ID���ܲ���ͬ����ʱ������ֻ����һ����Ϣ
	}
	else
	{
		WindowProcessInfo* pInfo = new WindowProcessInfo;
		memset(pInfo, 0x0, sizeof(WindowProcessInfo));

		//���ô��ھ��
		pInfo->hWnd = hWnd;

		//���ô��ڵ��߳�ID�ͽ���ID
		pInfo->dwThreadID = dwThreadID;
		pInfo->dwProcessID = dwProcessID;

		//��ȡ��������
		GetClassName(hWnd, pInfo->cClassName, sizeof(pInfo->cClassName));

		//��ȡ���ڱ���
		GetWindowText(hWnd, pInfo->cWindowText, sizeof(pInfo->cWindowText));

		m_oWindowProcessInfo.SetAt(pInfo->dwProcessID, pInfo);
	}
}

PWindowProcessInfo CCloudDocDealHelperThread::GetWindowProcessInfo(DWORD dwProcessID)
{
	WindowProcessInfo* pInfo = NULL;	//���ڽ�����Ϣ
	m_oWindowProcessInfo.Lookup(dwProcessID, pInfo);
	return pInfo;
}

void CCloudDocDealHelperThread::SimulationKeyboard(HWND hWnd, int nVK)
{
	//ģ��Esc����
	//VK_RETURN,VK_ESCAPE
	::PostMessage(hWnd, WM_KEYDOWN, nVK, 0);
	::PostMessage(hWnd, WM_KEYUP, nVK, 0);	
}
