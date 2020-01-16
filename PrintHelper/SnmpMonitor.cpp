// SnmpMonitor.cpp: implementation of the CSnmpMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SnmpMonitor.h"

//��ӡ��ҵ����ͷ�ļ�
#include "SnmpJobMonitorKM.h"
#include "SnmpJobMonitorSL.h"
#include "SnmpJobMonitorLG.h"
#include "SnmpJobMonitorDZ.h"
#include "SnmpJobMonitorJC.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSnmpMonitor::CSnmpMonitor() : CSnmpOP()
{
	m_lActive = 0;
	m_hThreadTerm = 0;
	m_hThreadStarted = 0;
	m_hThread = 0;
	m_pJobMonitor = NULL;
	m_pMeterMonitor = NULL;
	m_pPrinterMonitorHelper = NULL;
	m_nA4Pos = 1;
	m_nA3Pos = 2;
	m_nMinPaper = 25;
	m_nMinInk = 10;
	m_dwPowerMaxSecond = 60 * 10;	//Ĭ��10����
	m_bEnablePrinterJobMonitor = FALSE;	//���ô�ӡ����ҵ����
	m_nCheckJobTimeOutSecond = 1;		//����ӡ����ҵ��Ϣ��ʱ����
	m_bEnablePrinterMeterMonitor = FALSE;//���ô�ӡ���������
	m_nCheckMeterTimeOutSecond = 1;		//����ӡ��������Ϣ��ʱ����
	m_bEnablePrinterInfoMonitor = TRUE;	//���ô�ӡ����Ϣ����
	m_nCheckInfoTimeOutSecond = 1;		//����ӡ����Ϣ��ʱ����
	m_bEnableConnectPrinter = FALSE;	//���ô�ӡ������
	m_nCheckConnectTimeOutSecond = 10;	//����ӡ�����ӵ�ʱ����
	m_bEnablePrinterMonitor = FALSE;	//���ô�ӡ��״̬����
	m_nCheckMonitorTimeOutSecond = 1;	//����ӡ��״̬���ӵ�ʱ����

	m_nCheckTick = 0;

//	memset(m_aryInk,0,sizeof(m_aryInk));
	m_nLastPrintCount = 0;
	m_nLastPrintCount_Color = 0;
	m_nLastPrintCount_WB = 0;
	m_nLastCopyCount = 0;
	m_nLastA4CopyCount_Color = 0;
	m_nLastA4CopyCount_WB = 0;
	m_nLastA3CopyCount_Color = 0;
	m_nLastA3CopyCount_WB = 0;
	m_nLastScanCount = 0;
	m_nLastScanCount_Color = 0;
	m_nLastScanCount_WB = 0;
	m_nLastFaxCount = 0;
	m_nLastFaxCount_Color = 0;
	m_nLastFaxCount_WB = 0;
	InitCmmt();
	InitializeCriticalSection (&m_csLock);
// 	InitializeCriticalSection(&m_csLock4MeterMonitor);
// 	InitializeCriticalSection(&m_csLock4JobMonitor);
// 	InitializeCriticalSection(&m_csLock4PrinterMonitorHelper);

	m_nLastCheckJobTimeOutSecond = 0;	//add by zfq,�ϴμ���ӡ����ҵ��Ϣ��ʱ��,MS
	m_nLastCheckMeterTimeOutSecond = 0;	//add by zfq,Last ����ӡ��������Ϣ��ʱ����
	m_nLastCheckInfoTimeOutSecond = 0;	//add by zfq,Last ����ӡ����Ϣ��ʱ������У԰����ҵ��ӡ��
	m_nLastCheckMonitorTimeOutSecond = 0;	//add by zfq,Last ����ӡ��״̬���ӵ�ʱ�������ƴ�ӡ��
	m_nLastCheckConnectTimeOutSecond = 0;	//add by zfq,Last ����ӡ�����ӵ�ʱ����
}

CSnmpMonitor::~CSnmpMonitor()
{
	Shutdown();
	DeleteCriticalSection (&m_csLock );
// 	DeleteCriticalSection (&m_csLock4MeterMonitor);
// 	DeleteCriticalSection (&m_csLock4JobMonitor);
// 	DeleteCriticalSection (&m_csLock4PrinterMonitorHelper);	//add by zfq,2016-01-28
}
void CSnmpMonitor::Shutdown()
{
	theLog.Write(_T("CSnmpMonitor::Shutdown,0.1"));
	InterlockedExchange(&m_lActive,0);
	theLog.Write(_T("CSnmpMonitor::Shutdown,0.2"));
	if (m_hThread && m_hThreadTerm)
	{
		theLog.Write(_T("CSnmpMonitor::Shutdown,1,"));
		SetEvent(m_hThreadTerm);
		theLog.Write(_T("CSnmpMonitor::Shutdown,2,"));
		WaitForSingleObject(m_hThread,INFINITE);
		theLog.Write(_T("CSnmpMonitor::Shutdown,3,"));
		CloseHandle(m_hThread);
		CloseHandle(m_hThreadTerm);
		m_hThreadTerm = 0;
		m_hThread = 0;
	}
	m_nCheckTick = 0;
	m_bEnableConnectPrinter = FALSE;
}

void CSnmpMonitor::CreateTh()
{
	if (!m_hThreadTerm)
	{
		InterlockedExchange(&m_lActive,1);
		m_hThreadTerm = CreateEvent(0,0,0,0);
		m_hThreadStarted = CreateEvent(0,0,0,0);
		m_hThread			= 	(HANDLE)_beginthreadex(0,0,CSnmpMonitor::ThreadFn,(void*)this,0,0 );

		DWORD dwWait = WaitForSingleObject ( m_hThreadStarted , INFINITE );

		ASSERT ( dwWait ==		WAIT_OBJECT_0 );

		CloseHandle(m_hThreadStarted);
		m_hThreadStarted = 0;
	}
}

BOOL CSnmpMonitor::IsRunning()
{
	long lVal = 0;
	InterlockedExchange(&lVal,m_lActive);
	return lVal;
}

BOOL CSnmpMonitor::GetScanFileUser(IN CString szFileName, OUT CString& szUserName)
{
	CCriticalSection2::Owner ow(m_csCheckScanjobLock);
	theLog.Write(_T("CSnmpMonitor::GetScanFileUser,size=%d,szFileName=%s"), m_oScanFileMap.GetSize(), szFileName);
	POSITION pos = m_oScanFileMap.GetStartPosition();
	while (pos)
	{
		int key;
		ST_SCANFILE* value;
		m_oScanFileMap.GetNextAssoc(pos, key, value);
		if (value)
		{
			CString szTitle(value->cFileName);
			//theLog.Write("CSnmpMonitor::GetScanFileUser,szTitle=%s", szTitle);
			CString szFilePreStufix = szFileName.Mid(0, szFileName.ReverseFind('.'));
			if (szFilePreStufix.Find(szTitle) >= 0 && szTitle.GetLength() > 0)
			{
				szUserName.Format(_T("%s"), value->cUserName);
				theLog.Write(_T("CSnmpMonitor::GetScanFileUser,szTitle=%s,szUserName=%s"), szTitle, szUserName);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CSnmpMonitor::CheckStatus()
{
	if(m_nCheckTick > (24 * 3600))	//���һ��ѭ��
	{
		m_nCheckTick = 0;
	}
	
	DWORD dwCur = GetTickCount();
	DWORD dwTick1 = dwCur;
	DWORD dwTick2 = 0;
	DWORD dwTick3 = 0;
	DWORD dwTick4 = 0;
	DWORD dwTick5 = 0;
	int nSpanMS = 0;
	int nSpanSec = 0;

	//theLog.Write(_T("CSnmpMonitor::CheckStatus,0.1,m_lpMgrSession=%x,m_bEnableConnectPrinter=%d"), m_lpMgrSession, m_bEnableConnectPrinter);

	if (m_lpMgrSession && !m_bEnableConnectPrinter)
	{
//		theLog.Write(_T("CSnmpMonitor::CheckStatus,1,PrtIP=%s,begin"), m_szIP);

		//--add by zfq,begin
		nSpanMS = dwCur - m_nLastCheckInfoTimeOutSecond;
		nSpanSec = (0 > nSpanMS) ? 0 : (nSpanMS / 1000);
		//--add by zfq,end
//		if ((m_nCheckTick % m_nCheckInfoTimeOutSecond) == 0)	//del by zfq
		if(nSpanSec
			> m_nCheckInfoTimeOutSecond
			|| 0 == m_nLastCheckInfoTimeOutSecond)	//add by zfq
		{
			if (m_bEnablePrinterInfoMonitor)
			{
				CheckDeviceStatus();
				if (m_bEnableConnectPrinter)
				{
					//�˴���m_nCheckTick��m_nCheckConnectTimeOutSecond����Ϊ��ȣ�������������
					m_nCheckTick = m_nCheckConnectTimeOutSecond;
				}
				else
				{
// 					CheckPaper();
// 					CheckInk();
					CheckPower();
					CheckPrintCount();
					CheckCopyCount();
					CheckScanCount();
					CheckFaxCount();
				}
			}

			m_nLastCheckInfoTimeOutSecond = GetTickCount();	//add by zfq
		}

		dwTick2 = GetTickCount();

		//--add by zfq,begin
		nSpanMS = dwCur - m_nLastCheckJobTimeOutSecond;
		nSpanSec = (0 > nSpanMS) ? 0 : (nSpanMS / 1000);
		//--add by zfq,end
//		if ((m_nCheckTick % m_nCheckJobTimeOutSecond) == 0)	//del by zfq
		if(nSpanSec
			> m_nCheckJobTimeOutSecond
			|| 0 == m_nLastCheckJobTimeOutSecond)	//add by zfq
		{
			CheckJob();	//����ӡ��ҵ��Ϣ

			m_nLastCheckJobTimeOutSecond = GetTickCount();	//add by zfq
		}

		dwTick3 = GetTickCount();

		//--add by zfq,begin
		nSpanMS = dwCur - m_nLastCheckMonitorTimeOutSecond;
		nSpanSec = (0 > nSpanMS) ? 0 : (nSpanMS / 1000);
		//--add by zfq,end

//		if ((m_nCheckTick % m_nCheckMonitorTimeOutSecond) == 0) //del by zfq
		if((nSpanSec
			> m_nCheckMonitorTimeOutSecond)
			|| 0 == m_nLastCheckMonitorTimeOutSecond)	//add by zfq
		{
			//��ʼ���Ԥ����Ϣ
			CheckBegin();

			CheckPrinterMonitorInfo();	//����ӡ��������Ϣ

			//�������Ԥ����Ϣ
			CheckEnd();

			m_nLastCheckMonitorTimeOutSecond = GetTickCount();	//add by zfq
		}

		dwTick4 = GetTickCount();

		//--add by zfq,begin
		nSpanMS = dwCur - m_nLastCheckMeterTimeOutSecond;
		nSpanSec = (0 > nSpanMS) ? 0 : (nSpanMS / 1000);
		//--add by zfq,end

// 		theLog.Write(_T("!@CSnmpMonitor::CheckStatus,9.1,PrtIP=%s,nSpanMS=%d,nSpanSec=%d,m_nCheckMeterTimeOutSecond=%d,m_nLastCheckMeterTimeOutSecond=%d,dwCur=%d,Go=%d")
// 			, m_szIP, nSpanMS, nSpanSec, m_nCheckMeterTimeOutSecond
// 			, m_nLastCheckMeterTimeOutSecond, dwCur
// 			, 0 == m_nLastCheckMeterTimeOutSecond);


//		if ((m_nCheckTick % m_nCheckMeterTimeOutSecond) == 0)	//del by zfq
		
		if((nSpanSec
			> m_nCheckMeterTimeOutSecond)
			|| 0 == m_nLastCheckMeterTimeOutSecond)	//add by zfq
		{
			CheckPrinterMeterInfo();	//��Ⳮ����Ϣ,�˴�������Ϣ����ī�ۺ�ֽ����Ϣ��
										//���Ե�һ��Ҫ��CheckPrinterMonitorInfo֮����ò��ܻ�ȡ��
			m_nLastCheckMeterTimeOutSecond = GetTickCount();	//add by zfq
		}

		dwTick5 = GetTickCount();

		
		DWORD dwCost1 = dwTick2 - dwTick1;
		DWORD dwCost2 = dwTick3 - dwTick2;
		DWORD dwCost3 = dwTick4 - dwTick3;
		DWORD dwCost4 = dwTick5 - dwTick4;
		DWORD dwMax = 15000;	//15��
		if(dwMax < dwCost1
			|| dwMax < dwCost2
			|| dwMax < dwCost3
			|| dwMax < dwCost4)
		{
			theLog.Write(_T("!@CSnmpMonitor::CheckStatus,10,PrtIP=%s,end,Cost TOO Much Time,dwCost1=%d,dwCost2=%d,dwCost3=%d,dwCost4=%d")
				, m_szIP, dwCost1, dwCost2, dwCost3, dwCost4);
		}
		else
		{
//			theLog.Write(_T("CSnmpMonitor::CheckStatus,10,PrtIP=%s,end,dwCost1=%d,dwCost2=%d,dwCost3=%d,dwCost4=%d")
//				, m_szIP, dwCost1, dwCost2, dwCost3, dwCost4);
		}
	}//if (m_lpMgrSession && !m_bEnableConnectPrinter)
	else
	{
		//--add by zfq,begin
		nSpanMS = dwCur - m_nLastCheckConnectTimeOutSecond;
		nSpanSec = (0 > nSpanMS) ? 0 : (nSpanMS / 1000);
		//--add by zfq,end

//		theLog.Write(_T("CSnmpMonitor::CheckStatus,20.1,PrtIP=%s,nSpanMS=%d,nSpanSec=%d,m_nCheckConnectTimeOutSecond=%d,m_nLastCheckConnectTimeOutSecond=%d,GO=%d")
//			, m_szIP, nSpanMS, nSpanSec, m_nCheckConnectTimeOutSecond
//			, m_nLastCheckConnectTimeOutSecond, 0 == m_nLastCheckConnectTimeOutSecond);


//		if ((m_nCheckTick % m_nCheckConnectTimeOutSecond) == 0)	//del by zfq
		if((nSpanSec
			> m_nCheckConnectTimeOutSecond)
			|| 0 == m_nLastCheckConnectTimeOutSecond)	//add by zfq
		{
			theLog.Write(_T("CSnmpMonitor::CheckStatus,21,PrtIP=%s,begin,nSpanSec=%d,m_nCheckConnectTimeOutSecond=%d,m_bEnableConnectPrinter=%d")
				, m_szIP, nSpanSec , m_nCheckConnectTimeOutSecond, m_bEnableConnectPrinter);

			//��ʼ���Ԥ����Ϣ
			CheckBegin();

			dwTick2 = GetTickCount();

			m_bEnableConnectPrinter = CheckIPActive() ? FALSE : TRUE;	//����ӡ��������ͨ��

			theLog.Write(_T("CSnmpMonitor::CheckStatus,21.2,PrtIP=%s,nSpanSec=%d,m_nCheckConnectTimeOutSecond=%d,m_bEnableConnectPrinter=%d")
				, m_szIP, nSpanSec , m_nCheckConnectTimeOutSecond, m_bEnableConnectPrinter);


			if (m_bEnableConnectPrinter && m_pCallBack)
			{
				CString szDesc;
				szDesc.Format(_T("AlarmNetWorkDisconnectOfPrinter|�������ӡ�������粻ͨ"));
				m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::NetWorkDisconnectOfPrinter,CStringToChar(szDesc));
			}

			dwTick3 = GetTickCount();

			//�������Ԥ����Ϣ
			CheckEnd();

			dwTick4 = GetTickCount();

			DWORD dwCost1 = dwTick2 - dwTick1;
			DWORD dwCost2 = dwTick3 - dwTick2;
			DWORD dwCost3 = dwTick4 - dwTick3;
			DWORD dwMax = 5000;
			if(dwMax < dwCost1
				|| dwMax < dwCost2
				|| dwMax < dwCost3)
			{
				theLog.Write(_T("!@CSnmpMonitor::CheckStatus,20,PrtIP=%s,end,Cost TOO Much Time,dwCost1=%d,dwCost2=%d,dwCost3=%d")

					, m_szIP, dwCost1, dwCost2, dwCost3);
			}
			else
			{
//				theLog.Write(_T("CSnmpMonitor::CheckStatus,20,PrtIP=%s,end,dwCost1=%d,dwCost2=%d,dwCost3=%d")

//					, m_szIP, dwCost1, dwCost2, dwCost3);
			}

			m_nLastCheckConnectTimeOutSecond = GetTickCount();	//add by zfq
		}

	}

	m_nCheckTick++;
}

unsigned __stdcall CSnmpMonitor::ThreadFn(void*pvParam)
{
	CSnmpMonitor* pThis = (CSnmpMonitor*)pvParam;
	
	SetEvent(pThis->m_hThreadStarted);

	theLog.Write(_T("CSnmpMonitor::ThreadFn,begin,PrtIP=%s"), pThis->GetPrtIP());

	//����snmp
	if (!pThis->OpenSnmp())
	{
		theLog.Write(_T("!!CSnmpMonitor::ThreadFn,1"));
		return 0;
	}

	//��ʼ��ҵ����
	pThis->StartJobMonitor();

	theLog.Write(_T("CSnmpMonitor::ThreadFn,2"));
	//��ʼ��ӡ���������
	pThis->StartPrinterMeterMonitor(); 

	theLog.Write(_T("CSnmpMonitor::ThreadFn,2.2"));
	//��ʼ���Ӵ�ӡ��״̬
	pThis->StartPrinterMonitorHelper();

	theLog.Write(_T("CSnmpMonitor::ThreadFn,3"));
	while (pThis->IsRunning())
	{
		DWORD dwWait = WaitForSingleObject(pThis->m_hThreadTerm,1000);
		switch (dwWait)
		{
		case WAIT_OBJECT_0:
			{
				//_endthreadex(1);
				theLog.Write(_T("CSnmpMonitor::ThreadFn,4"));
				goto threadend;
			}
			break;
		case WAIT_TIMEOUT:
			{
				//theLog.Write(_T("CSnmpMonitor::ThreadFn,5"));
				pThis->CheckStatus();
				pThis->CheckScanjobInfo();
			}
			break;
		default:
			{
				theLog.Write(_T("CSnmpMonitor::ThreadFn,6"));
			}
			break;
		}//switch
	}
threadend:

	theLog.Write(_T("CSnmpMonitor::ThreadFn,7"));

	//������ӡ����
// 	pThis->ReleaseSnmp();

	theLog.Write(_T("CSnmpMonitor::ThreadFn,8"));
	//������ҵ����
	pThis->StopJobMonitor();


	theLog.Write(_T("CSnmpMonitor::ThreadFn,9"));
	//������ӡ���������
	pThis->StopPrinterMeterMonitor();

	theLog.Write(_T("CSnmpMonitor::ThreadFn,10"));
	//�������Ӵ�ӡ��״̬
	pThis->StopPrinterMonitorHelper();

	theLog.Write(_T("CSnmpMonitor::ThreadFn,end"));
	return 0;
}
//�豸״̬
void CSnmpMonitor::CheckDeviceStatus()
{
	Owner ow(m_csLock);
	char szOid[50] = {".1.3.6.1.2.1.25.3.5.1.1.1"};
	
	int nVal = -1;
	if(GetRequest(szOid,nVal))
	{
		if (nVal == 1)
		{
			//other
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_OTHER;
		}
		else if (nVal == 2)
		{
			//unknown
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_UNKNOWN;
		}
		else if (nVal == 3)
		{
			//����
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_IDLE;
			if (m_pCallBack)
			{
				DWORD dw = GetTickCount();
				m_pCallBack->OnSCPIdle((dw - m_lTickCount) /1000);
			}
		}
		else if (nVal == 4)
		{
			//��ӡ
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_PRINTING;
			m_lTickCount = GetTickCount();
		}
		else if (nVal == 5)
		{
			//Ԥ��
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_WARMUP;
		}
		else
		{
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_OTHER;
		}
		//theLog.Write(_T("CSnmpMonitor::CheckDeviceStatus,nVal=%d"),nVal);
	}
	else
	{
		if (CheckIPActive())
		{
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_OTHER;
			m_bEnableConnectPrinter = FALSE;
		}
		else
		{
			theLog.Write(_T("!!CheckDeviceStatus GetRequest fail,��ӡ�������ӡ�������粻ͨ"));
			m_oStatus.m_eDeviceStatus = FX_DC_NET_ERR;
			m_oStatus.m_ePowerStatus = FX_DC_POWER_OFF;
			m_bEnableConnectPrinter = TRUE;
		}
	}
}

//��ȡ��ӡ��״̬
void CSnmpMonitor::GetPrinterStatus(CSCPStatus& status)
{
	Owner ow(m_csLock);
	memcpy(&status, &m_oStatus, sizeof(CSCPStatus));
}

//����״̬
void CSnmpMonitor::CheckOPStatus()
{
	Owner ow(m_csLock);

	char szOid[50] = {".1.3.6.1.2.1.25.3.2.1.5.1"};
	
	int nVal = -1;
	if(GetRequest(szOid,nVal))
	{
		if (nVal == 3)
		{
			m_oStatus.m_eOPStatus = FX_DC_OP_IDLE;
		}
		//theLog.Write(_T("CSnmpMonitor::CheckOPStatus,nVal=%d"),nVal);
	}
	else
	{
		theLog.Write(_T("!!CSnmpMonitor::CheckOPStatus,fail,err=%d"), GetLastError());
	}
}

void CSnmpMonitor::CheckLmInk()
{
	return CheckInk();
}

//���ī��
void CSnmpMonitor::CheckInk()
{
	Owner ow(m_csLock);

	CStringA sTotalInkOid = ".1.3.6.1.2.1.43.11.1.1.8.1.";
	CStringA sLeftInkOid = ".1.3.6.1.2.1.43.11.1.1.9.1.";
	for (int n = 1; n < Max_Ink_Index; n++)
	{
		if (m_aryInk[n] > FX_DC_INK_NULL)
		{
			int nTotal = 100;
			int nVal = -1;
			CStringA sTotalTmp;
			sTotalTmp.Format("%s%d",sTotalInkOid,n);
			CStringA sLeftTmp;
			sLeftTmp.Format("%s%d",sLeftInkOid,n);
			if (GetRequest(CStringToChar(sTotalTmp), nTotal))
			{
				if(GetRequest(CStringToChar(sLeftTmp),nVal))
				{
					double fRate = nVal * 1.0 / nTotal;
					int nLeft = (int)(fRate * 100);
					m_oStatus.m_aryInk[n] = FX_DC_FIR_INK_OK;
					if (nTotal>=0 && nLeft>=0)	//���ܻ����-2��-3������ֵ��һ���ʾ������
					{
					}
					else
					{
						continue;
					}

					//theLog.Write(_T("CheckInk(%d) total=%d,left=%d rate=%d"),n,nTotal,nVal,nLeft);
					if (m_pCallBack)
					{
						CString sDesc;
						if (nLeft <= 0)
						{
							m_oStatus.m_aryInk[n] = FX_DC_FIR_INK_NO;
							sDesc.Format(_T("��%d��ī���޷ۣ��밲װ��ī�У�"),n);
							m_pCallBack->OnStatus(FX_DC_FIR_INK_NO,CStringToChar(sDesc));
							theLog.Write(_T("CheckInk(%d) total=%d,left=%d rate=%d,sDesc=%s"),n,nTotal,nVal,nLeft,sDesc);
						}
						else if (nLeft <= m_nMinInk)
						{
							m_oStatus.m_aryInk[n] = FX_DC_FIR_INK_LESS;
							sDesc.Format(_T("��%d��ī���ٷۣ���׼������ī�У�"),n);
							m_pCallBack->OnStatus(FX_DC_FIR_INK_LESS,CStringToChar(sDesc));
						}
					}
				}
				else
				{
					theLog.Write(_T("!!CSnmpMonitor::CheckInk,1,fail,err=%d"), GetLastError());
				}
			}
			else
			{
				theLog.Write(_T("!!CSnmpMonitor::CheckInk,2,fail,err=%d"), GetLastError());
			}
		}
	}
}

void CSnmpMonitor::CheckPower()
{
	char* pszSysUpTime = ".1.3.6.1.2.1.1.3.0";
	TimeTicks dwSysUpTime = 0;
	if (GetRequest(pszSysUpTime, dwSysUpTime))
	{
		//SysUpTime
		//"The time (in hundredths of a second) since the
		//network management portion of the system was last
		//re-initialized.",
		//��ӡ������ʱ����ǵ�ǰʱ���ȥsysUpTime���ٷ�һ�뵥λ��ʱ��
		DWORD dwSecond = dwSysUpTime / 100;
		if ((dwSecond <= m_dwPowerMaxSecond) && (m_oStatus.m_ePowerStatus != FX_DC_POWER_ON))
		{
			m_oStatus.m_ePowerStatus = FX_DC_POWER_ON;
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(m_oStatus.m_ePowerStatus, _T("��ӡ������"));
			}
		}
	}
}

void CSnmpMonitor::CheckPrinterMeterInfo()
{
// 	Owner ow(m_csLock4MeterMonitor);

	//theLog.Write(_T("CSnmpMonitor::CheckPrinterMeterInfo,begin"));
	if (m_bEnablePrinterMeterMonitor && m_pMeterMonitor)
	{
		m_pMeterMonitor->CheckMeterInfo();
	}
	//theLog.Write(_T("CSnmpMonitor::CheckPrinterMeterInfo,end"));
}

void CSnmpMonitor::CheckPrinterMonitorInfo()
{
// 	Owner ow(m_csLock4PrinterMonitorHelper);

	//theLog.Write(_T("CSnmpMonitor::CheckPrinterMonitorInfo,begin"));
	if (m_bEnablePrinterMonitor && m_pPrinterMonitorHelper)
	{
		m_pPrinterMonitorHelper->CheckMonitor();
	}
	//theLog.Write(_T("CSnmpMonitor::CheckPrinterMonitorInfo,end"));
}

void CSnmpMonitor::CheckScanjobInfo()
{
	CCriticalSection2::Owner ow(m_csCheckScanjobLock);
	if (m_eType != BT_KM)
	{
		return;
	}
	//��ȡɨ�����Ӧ��jobid
	//char *cOidBegin = ".1.3.6.1.4.1.18334.1.1.4.1.3.1.1.12.1";
	char *cOidBegin = ".iso.org.dod.internet.private.enterprises.18334.1.1.4.1.3.1.1.12.1";
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = { 0 };
	char pszOidNext[128] = { 0 };
	{
		POSITION pos = m_oScanFileMap.GetStartPosition();
		while (pos)
		{
			int key;
			ST_SCANFILE* value;
			m_oScanFileMap.GetNextAssoc(pos, key, value);
			if (value)
			{
				delete value;
			}
		}
		m_oScanFileMap.RemoveAll();
	}
	while (TRUE)
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			//theLog.Write("CSnmpMonitor::CheckScanjobInfo,pszOidNext=%s", pszOidNext);
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			char cOidStr[128] = { 0 };
			char cValue[128] = { 0 };
			sprintf(cOidStr, "%s.%d", ".1.3.6.1.4.1.18334.1.1.4.1.3.1.1.10.1", nIndex);
			int len = 128;
			GetRequestStr(cOidStr, (unsigned char*)cValue, len);
			ST_SCANFILE* scanfile = new ST_SCANFILE;
			memset(scanfile, 0x0, sizeof(ST_SCANFILE));

			memcpy(scanfile->cFileName, CCommonFun::UTF8ToUnicode(pszValue).GetString(), 128);
			memcpy(scanfile->cUserName, CCommonFun::UTF8ToUnicode(cValue).GetString(), 128);
			m_oScanFileMap.SetAt(nIndex, scanfile);
			//theLog.Write("CSnmpMonitor::CheckScanjobInfo,cFileName=%s,cUserName=%s,m_oScanFileMap.size=%d",scanfile->cFileName, scanfile->cUserName, m_oScanFileMap.GetSize());
		}
		else
		{
			//theLog.Write("!!CSnmpMonitor::CheckScanjobInfo, failed to find next oid");
			break;
		}
	}
}

void CSnmpMonitor::CheckLmPaper()
{
	Owner ow(m_csLock);

	CStringA sOid = ".1.3.6.1.2.1.43.8.2.1.11.1.";

	for (int n = 1; n < Max_Paper_Index; n++)
	{
		if (m_aryPaper[n] > 0)
		{
			CStringA sTmp;
			sTmp.Format("%s%d",sOid,n);
			int nVal = -1;
			CString sDesc;
			if(GetRequest(CStringToChar(sTmp),nVal))
			{
				if (nVal == 17)
				{
					m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_NO;
					sDesc.Format(_T("��%d��ֽ��ȱֽ"),n);
					theLog.Write(_T("CheckPaper(%s)(%d) %d ,%s "),sTmp,n,nVal,sDesc);
				}
				else if (nVal == 12)
				{
					m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_LESS;
					sDesc.Format(_T("��%d��ֽ����ֽ"),n);
				}
				else
				{
					m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_OK;
					sDesc.Format(_T("��%d��ֽ������"),n);
				}
				if (m_pCallBack)
				{
					
					m_pCallBack->OnStatus(m_oStatus.m_aryPaper[n],CStringToChar(sDesc));
				}
			}
			else
			{
				theLog.Write(_T("!!CheckLmPaper err,err=%d"), GetLastError());
			}
		}
	}
}
void CSnmpMonitor::CheckOkiPaper()
{
	Owner ow(m_csLock);
	CStringA sOid = ".1.3.6.1.2.1.43.16.5.1.2.1.2";//��һֽ��oid
	for (int n = 1; n < Max_Paper_Index; n++)
	{
		if (m_aryPaper[n] > 0)
		{
			CStringA sTmp;
			CString sDesc;
			sTmp = sOid;
			unsigned char Buf[1024];
			memset(Buf,0x0,1024);
			if (1 == n)
			{
				if(GetRequest(CStringToChar(sTmp),Buf,1024))
				{
					if (0 == strcpy((char*)Buf,"Tray1 Empty"))
					{
						m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_NO;
						sDesc.Format(_T("��%d��ֽ��ȱֽ"),n);
						theLog.Write(_T("CheckPaper(%s)(%d) %s ,%s "),sTmp,n,Buf,sDesc);
					}
					if (m_pCallBack)
					{

						m_pCallBack->OnStatus(m_oStatus.m_aryPaper[n],CStringToChar(sDesc));
					}
				}
				else
				{
					theLog.Write(_T("!!CheckLmPaper err,err=%d"), GetLastError());
				}
			}	
		}
	}
}
void CSnmpMonitor::CheckManualPaper()
{
	Owner ow(m_csLock);

	int nTrayNo = 0;
	if (m_eType == BT_KM)
	{
		nTrayNo = 8;	//����ֽ��8Ϊ����ֽ��
	}
	else
	{
		m_oStatus.m_aryPaper[0] = FX_DC_FIR_PAPER_NO;	//Ĭ��Ϊֽ�п�
		return;
	}
	
	CStringA sTotalTemp;
	CStringA sLeftTemp;
	sTotalTemp.Format(".1.3.6.1.2.1.43.8.2.1.9.1.%d", nTrayNo);
	sLeftTemp.Format(".1.3.6.1.2.1.43.8.2.1.10.1.%d", nTrayNo);

	int nTotalVal = -1;
	int nLeftVal = -1;
	if(!GetRequest(CStringToChar(sTotalTemp),nTotalVal))
	{
		theLog.Write(_T("!!CSnmpMonitor::CheckManualPaper,fail.sTotalTemp=[%s]"),sTotalTemp);
		return;
	}
	if(!GetRequest(CStringToChar(sLeftTemp),nLeftVal))
	{
		theLog.Write(_T("!!CSnmpMonitor::CheckManualPaper,fail.sLeftTemp=[%s]"),sLeftTemp);
		return;
	}

	double fRate = nLeftVal * 1.0 / nTotalVal;
	int nRate = (int)(fRate * 100);

	CString sDesc;
	if (nLeftVal == 0 || nLeftVal == -2)
	{
		m_oStatus.m_aryPaper[0] = FX_DC_FIR_PAPER_NO;
		//sDesc.Format("��%d��ֽ��%sȱֽ",n,(m_aryPaper[n] == 4 ? "A4":"A3"));
	}
	else if (nLeftVal > 0 && nRate <= m_nMinPaper)
	{
		m_oStatus.m_aryPaper[0] = FX_DC_FIR_PAPER_LESS;
		//sDesc.Format("��%d��ֽ��%s��ֽ",n,(m_aryPaper[n] == 4 ? "A4":"A3"));
	}
	else
	{
		m_oStatus.m_aryPaper[0] = FX_DC_FIR_PAPER_OK;
		//sDesc.Format("��%d��ֽ��%s����",n,(m_aryPaper[n] == 4 ? "A4":"A3"));
	}
	//theLog.Write(_T("CSnmpMonitor::CheckManualPaper,�ֶ���ֽ��(%d),����(%d),����(%d),%s"), nTrayNo, nTotalVal, nLeftVal, sDesc);
	if (m_pCallBack)
	{
		//m_pCallBack->OnStatus(m_oStatus.m_aryPaper[n],CStringToChar(sDesc));
	}
}

//ֽ��״̬
void CSnmpMonitor::CheckPaper()
{
	if(m_eType == BT_LM)
	{
		return CheckLmPaper();
	}
	if (m_eType == BT_OKI)
	{
		return CheckOkiPaper();
	}
	
	Owner ow(m_csLock);

	CStringA sTotalOid = ".1.3.6.1.2.1.43.8.2.1.9.1.";
	CStringA sLeftOid = ".1.3.6.1.2.1.43.8.2.1.10.1.";
	for (int n = 1; n < Max_Paper_Index; n++)
	{
		if (m_aryPaper[n] > 0)
		{
			CStringA sTotalTemp;
			CStringA sLeftTemp;
			sTotalTemp.Format("%s%d",sTotalOid,n);
			sLeftTemp.Format("%s%d",sLeftOid,n);
			int nTotalVal = -1;
			int nLeftVal = -1;
			if(!GetRequest(CStringToChar(sTotalTemp),nTotalVal))
			{
				theLog.Write(_T("!!CSnmpMonitor::CheckPaper,fail.sTotalTemp=[%s]"),sTotalTemp);
				return;
			}
			if(!GetRequest(CStringToChar(sLeftTemp),nLeftVal))
			{
				theLog.Write(_T("!!CSnmpMonitor::CheckPaper,fail.sLeftTemp=[%s]"),sLeftTemp);
				return;
			}

			m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_OK;
			if (nTotalVal>=0 && nLeftVal>=0)	//���ܻ����-2��-3������ֵ��һ���ʾ������
			{
			}
			else
			{
				continue;
			}

			double fRate = nLeftVal * 1.0 / nTotalVal;
			int nRate = (int)(fRate * 100);

			CString sDesc;
			if (nLeftVal == 0 || nLeftVal == -2)
			{
				m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_NO;
				sDesc.Format(_T("��%d��ֽ��%sȱֽ"),n,(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
				theLog.Write(_T("CheckPaper,ֽ��(%d),����(%d),����(%d),%s"), n, nTotalVal, nLeftVal, sDesc);
			}
			else if (nLeftVal > 0 && nRate <= m_nMinPaper)
			{
				m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_LESS;
				sDesc.Format(_T("��%d��ֽ��%s��ֽ"),n,(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
			}
			else
			{
				m_oStatus.m_aryPaper[n] = FX_DC_FIR_PAPER_OK;
				sDesc.Format(_T("��%d��ֽ��%s����"),n,(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
			}
			//theLog.Write(_T("CheckPaper,ֽ��(%d),����(%d),����(%d),%s"), n, nTotalVal, nLeftVal, sDesc);
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(m_oStatus.m_aryPaper[n],CStringToChar(sDesc));
			}
		}
	}

	//����ֶ���ֽ״̬
	//CheckManualPaper();
}
void CSnmpMonitor::CheckLmDeviceStatus()
{
	Owner ow(m_csLock);
	char szOid[50] = {".1.3.6.1.2.1.43.18.1.1.7"};

	int nVal = -1;
	if(GetNextRequest(szOid,nVal))
	{
		CString sDesc;
		if (nVal == 1)
		{
			
		}
		else if (nVal == 2)
		{
			
		}
		//��ֽ
		else if (nVal == 8)
		{
			sDesc.Format(_T("��ӡ����ֽ"));
		}
		//��ֽ
		else if (nVal == 801)
		{
			sDesc.Format(_T("��ӡ��ֽ�г������ȱ��"));
		}
		//��ֽ
		else if (nVal == 807)
		{
			sDesc.Format(_T("��ӡ����ֽ"));
		}
		//ȱֽ
		else if (nVal == 808)
		{
			sDesc.Format(_T("��ӡ��ȱֽ"));
		}
		// ֽ�г��
		else if (nVal == 901)
		{
			sDesc.Format(_T("��ӡ��ֽ�г��"));
		}
		if (m_pCallBack && !sDesc.IsEmpty())
		{
			m_pCallBack->OnStatus(FX_DC_ERR,CStringToChar(sDesc));
		}
		//theLog.Write(_T("CheckLmDeviceStatus %d"),nVal);
	}
	else
	{
		if (CheckIPActive())
		{
			m_oStatus.m_eDeviceStatus = FX_DC_DEVICE_OTHER;
			m_bEnableConnectPrinter = FALSE;
		}
		else
		{
			theLog.Write(_T("!!CheckLmDeviceStatus fail,��ӡ�������ӡ�������粻ͨ"));
			m_oStatus.m_eDeviceStatus = FX_DC_NET_ERR;
			m_bEnableConnectPrinter = TRUE;
		}
	}
}

//��ӡҳ��
void CSnmpMonitor::CheckPrintCount()
{
	BOOL bIsPrinting = FALSE;

#if 0
	if (m_pJobMonitor)
	{
		int nPrintColor = m_pJobMonitor->CheckLastPrintCount_Color();
		int nPrintWB = m_pJobMonitor->CheckLastPrintCount_WB();
#else
	PrinterMeterInfo oMeterInfo;
	if (GetPrinterMeterInfo(&oMeterInfo))
	{
		int nPrintColor = oMeterInfo.nA4PrintColorPage + oMeterInfo.nA3PrintColorPage + oMeterInfo.nOtherPrintColorPage;
		int nPrintWB = oMeterInfo.nA4PrintHBPage + oMeterInfo.nA3PrintHBPage + oMeterInfo.nOtherPrintHBPage;
#endif
		if ((m_nLastPrintCount_Color > 0) && (nPrintColor > m_nLastPrintCount_Color))
		{
			theLog.Write(_T("CSnmpMonitor::CheckPrintCount,1,nPrintColor=%d,nPrintWB=%d"),nPrintColor,nPrintWB);
			bIsPrinting = TRUE;
		}

		if ((m_nLastPrintCount_WB > 0) && (nPrintWB > m_nLastPrintCount_WB))
		{
			theLog.Write(_T("CSnmpMonitor::CheckPrintCount,2,nPrintColor=%d,nPrintWB=%d"),nPrintColor,nPrintWB);
			bIsPrinting = TRUE;
		}
		m_nLastPrintCount_Color = nPrintColor;
		m_nLastPrintCount_WB = nPrintWB;
		m_nLastPrintCount = m_nLastPrintCount_Color + m_nLastPrintCount_WB;
	}
	if (bIsPrinting)
	{
		if (m_pCallBack)
		{
			m_pCallBack->OnStatus(FX_DC_DEVICE_PRINTING,0);
		}
	}
	m_oStatus.m_ePrintStatus = bIsPrinting ? FX_DC_DEVICE_PRINTING : FX_DC_DEVICE_OTHER;
}

void CSnmpMonitor::CheckCopyCount()
{
	BOOL bIsCopying = FALSE;
	CString szCopyInfo = _T("");	//��ʽΪ��"PAPER_COLOR|PAPER_TYPE|dwPageCount"

#if 0
	if (m_pJobMonitor)
	{
		int nCopyColor = m_pJobMonitor->CheckLastCopyCount_Color();
		int nCopyWB = m_pJobMonitor->CheckLastCopyCount_WB();
#else
	PrinterMeterInfo oMeterInfo;
	if (GetPrinterMeterInfo(&oMeterInfo))
	{
		int nA4CopyColor = oMeterInfo.nA4CopyColorPage + oMeterInfo.nOtherCopyColorPage;
		int nA3CopyColor = oMeterInfo.nA3CopyColorPage;
		int nA4CopyWB = oMeterInfo.nA4CopyHBPage + oMeterInfo.nOtherCopyHBPage;
		int nA3CopyWB = oMeterInfo.nA3CopyHBPage;
#endif
		if ((m_nLastA4CopyCount_Color > 0) && (nA4CopyColor > m_nLastA4CopyCount_Color))
		{
			int nPageCount = nA4CopyColor - m_nLastA4CopyCount_Color;
			theLog.Write(_T("CSnmpMonitor::CheckCopyCount,1,nA4CopyColor=%d,m_nLastA4CopyCount_Color=%d,nPageCount=%d")

				,nA4CopyColor,m_nLastA4CopyCount_Color,nPageCount);
			bIsCopying = TRUE;
			szCopyInfo.Format(_T("%d|%d|%d"), COLOR_PAPER, A4_PAPER, nPageCount);
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(FX_DC_DEVICE_COPYING,CStringToChar(szCopyInfo));
			}
		}

		if ((m_nLastA4CopyCount_WB > 0) && (nA4CopyWB > m_nLastA4CopyCount_WB))
		{
			int nPageCount = nA4CopyWB - m_nLastA4CopyCount_WB;
			theLog.Write(_T("CSnmpMonitor::CheckCopyCount,2,nA4CopyWB=%d,m_nLastA4CopyCount_WB=%d,nPageCount=%d")

				,nA4CopyWB,m_nLastA4CopyCount_WB,nPageCount);
			bIsCopying = TRUE;
			szCopyInfo.Format(_T("%d|%d|%d"), WB_PAPER, A4_PAPER, nPageCount);
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(FX_DC_DEVICE_COPYING,CStringToChar(szCopyInfo));
			}
		}

		if ((m_nLastA3CopyCount_Color > 0) && (nA3CopyColor > m_nLastA3CopyCount_Color))
		{
			int nPageCount = nA3CopyColor - m_nLastA3CopyCount_Color;
			theLog.Write(_T("CSnmpMonitor::CheckCopyCount,3,nA3CopyColor=%d,m_nLastA3CopyCount_Color=%d,nPageCount=%d")

				,nA3CopyColor,m_nLastA3CopyCount_Color,nPageCount);
			bIsCopying = TRUE;
			szCopyInfo.Format(_T("%d|%d|%d"), COLOR_PAPER, A3_PAPER, nPageCount);
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(FX_DC_DEVICE_COPYING,CStringToChar(szCopyInfo));
			}
		}

		if ((m_nLastA3CopyCount_WB > 0) && (nA3CopyWB > m_nLastA3CopyCount_WB))
		{
			int nPageCount = nA3CopyWB - m_nLastA3CopyCount_WB;
			theLog.Write(_T("CSnmpMonitor::CheckCopyCount,4,nA3CopyWB=%d,m_nLastA3CopyCount_WB=%d,nPageCount=%d")

				,nA3CopyWB,m_nLastA3CopyCount_WB,nPageCount);
			bIsCopying = TRUE;
			szCopyInfo.Format(_T("%d|%d|%d"), WB_PAPER, A3_PAPER, nPageCount);
			if (m_pCallBack)
			{
				m_pCallBack->OnStatus(FX_DC_DEVICE_COPYING,CStringToChar(szCopyInfo));
			}
		}		

		m_nLastA4CopyCount_Color = nA4CopyColor;
		m_nLastA3CopyCount_Color = nA3CopyColor;
		m_nLastA4CopyCount_WB = nA4CopyWB;
		m_nLastA3CopyCount_WB = nA3CopyWB;
		m_nLastCopyCount = nA4CopyColor + nA3CopyColor	+ nA4CopyWB + nA3CopyWB;
	}

	m_oStatus.m_eCopyStatus = bIsCopying ? FX_DC_DEVICE_COPYING : FX_DC_DEVICE_OTHER;
}

void CSnmpMonitor::CheckScanCount()
{
	BOOL bIsScaning = FALSE;

#if 0
	if (m_pJobMonitor)
	{
		int nScanColor = m_pJobMonitor->CheckLastScanCount_Color();
		int nScanWB = m_pJobMonitor->CheckLastScanCount_WB();
#else
	PrinterMeterInfo oMeterInfo;
	if (GetPrinterMeterInfo(&oMeterInfo))
	{
		int nScanColor = oMeterInfo.nScanBigColorPage + oMeterInfo.nScanSmallColorPage;
		int nScanWB = oMeterInfo.nScanBigHBPage + oMeterInfo.nScanSmallHBPage;
#endif
		if ((m_nLastScanCount_Color > 0) && (nScanColor > m_nLastScanCount_Color))
		{
			theLog.Write(_T("CSnmpMonitor::CheckScanCount,1,nScanColor=%d,nScanWB=%d"),nScanColor,nScanWB);
			bIsScaning = TRUE;
		}

		if ((m_nLastScanCount_WB > 0) && (nScanWB > m_nLastScanCount_WB))
		{
			theLog.Write(_T("CSnmpMonitor::CheckScanCount,2,nScanColor=%d,nScanWB=%d"),nScanColor,nScanWB);
			bIsScaning = TRUE;
		}
		m_nLastScanCount_Color = nScanColor;
		m_nLastScanCount_WB = nScanWB;
		m_nLastScanCount = m_nLastScanCount_Color + m_nLastScanCount_WB;
	}

	if (bIsScaning)
	{
		if (m_pCallBack)
		{
			m_pCallBack->OnStatus(FX_DC_DEVICE_SCANING,0);
		}
	}
	m_oStatus.m_eScanStatus = bIsScaning ? FX_DC_DEVICE_SCANING : FX_DC_DEVICE_OTHER;
}

void CSnmpMonitor::CheckFaxCount()
{
	BOOL bIsFaxing = FALSE;

#if 0
	if (m_pJobMonitor)
	{
		int nFaxColor = m_pJobMonitor->CheckLastFaxCount_Color();
		int nFaxWB = m_pJobMonitor->CheckLastFaxCount_WB();
#else
	PrinterMeterInfo oMeterInfo;
	if (GetPrinterMeterInfo(&oMeterInfo))
	{
		int nFaxColor = oMeterInfo.nA4FaxColorPage + oMeterInfo.nA3FaxColorPage + oMeterInfo.nOtherFaxColorPage;
		int nFaxWB = oMeterInfo.nA4FaxHBPage + oMeterInfo.nA3FaxHBPage + oMeterInfo.nOtherFaxHBPage;
#endif
		if ((m_nLastFaxCount_Color > 0) && (nFaxColor > m_nLastFaxCount_Color))
		{
			theLog.Write(_T("CSnmpMonitor::CheckFaxCount,1,nFaxColor=%d,nFaxWB=%d"),nFaxColor,nFaxWB);
			bIsFaxing = TRUE;
		}

		if ((m_nLastFaxCount_WB > 0) && (nFaxWB > m_nLastFaxCount_WB))
		{
			theLog.Write(_T("CSnmpMonitor::CheckFaxCount,2,nFaxColor=%d,nFaxWB=%d"),nFaxColor,nFaxWB);
			bIsFaxing = TRUE;
		}
		m_nLastFaxCount_Color = nFaxColor;
		m_nLastFaxCount_WB = nFaxWB;
		m_nLastFaxCount = m_nLastFaxCount_Color + m_nLastFaxCount_WB;
	}

	if (bIsFaxing)
	{
		if (m_pCallBack)
		{
			m_pCallBack->OnStatus(FX_DC_DEVICE_FAXING,0);
		}
	}
	m_oStatus.m_eFaxStatus = bIsFaxing ? FX_DC_DEVICE_FAXING : FX_DC_DEVICE_OTHER;
}

void CSnmpMonitor::CheckJob()
{
// 	Owner ow(m_csLock4JobMonitor);

	if (m_pJobMonitor && m_bEnablePrinterJobMonitor)
	{
		m_pJobMonitor->CheckJob();
	}
}

void CSnmpMonitor::SetPaperConfig(CString sConfig, int nMin)
{
	Owner ow(m_csLock);

	theLog.Write(_T("CSnmpMonitor::SetPaperConfig,sConfig=[%s],nMin=%d"), sConfig, nMin);
	m_nMinPaper = nMin;
	memset(m_aryPaper,0,sizeof(m_aryPaper));
	if(sConfig.IsEmpty())
	{
		m_aryPaper[1] = 4;
	}
	else
	{
		CStringArray ary;
		CCommonFun::StringSplit(sConfig,&ary,_T(","));
		for (int n = 0; n < ary.GetCount(); n ++)
		{
			CString s = ary.GetAt(n);
			if (s.GetLength() > 0 )
			{
				int nPos = _tstoi(s);
				if (nPos >= Max_Paper_Index)
				{
					theLog.Write(_T("!!CSnmpMonitor::SetPaperConfig,�������ֽ����,max=%d,current=%d"), Max_Paper_Index, nPos);
					continue;
				}
				s.MakeLower();
				int a4 = 0;
				if(s.Find(_T("a4")) > 0)
				{
					a4 = 1;
				}
				if (a4)
				{
					m_aryPaper[nPos] = 4;
				}
				else
				{
					m_aryPaper[nPos] = 3;
				}
			}
		}
	}
}

void CSnmpMonitor::SetInkConfig(CString sConfig, int nMin)
{
	Owner ow(m_csLock);

	theLog.Write(_T("CSnmpMonitor::SetInkConfig,sConfig=[%s],nMin=%d"),sConfig,nMin);
	m_nMinInk = nMin;
	memset(m_aryInk,FX_DC_INK_NULL,sizeof(m_aryInk));

	if(sConfig.IsEmpty())
	{
		m_aryInk[1] = FX_DC_INK_BLACK;
	}
	else
	{
		CStringArray ary;
		CCommonFun::StringSplit(sConfig,&ary,_T(","));
		for (int n = 0; n < ary.GetCount(); n ++)
		{
			CString s = ary.GetAt(n);
			int nPos = _tstoi(s.Left(1));
			if (nPos >= Max_Ink_Index)
			{
				theLog.Write(_T("!!CSnmpMonitor::SetPaperConfig,�������ī����,max=%d,current=%d"),Max_Ink_Index, nPos);
				continue;
			}
			m_aryInk[nPos] = (FX_DC_INK_CMYK)_tstoi(s.Right(1));
		}
	}
}

BOOL CSnmpMonitor::OpenSnmp()
{
	if (!InitSnmp())
	{
		theLog.Write(_T("!!CSnmpMonitor::Start,InitSnmp fail"));
	}
	return TRUE;
}

BOOL CSnmpMonitor::Start(const TCHAR* szAddr,ISCPStatusInterface* pCall)
{
	Owner ow(m_csLock);
	theLog.Write(_T("CSnmpMonitor::Start,szAddr=%s,pCall=%p"), szAddr, pCall);

	if(0 != m_hThreadTerm)
	{
		theLog.Write(_T("!+CSnmpMonitor::Start,Already has been Started!"));
		return TRUE;
	}

	SetCallBack(pCall);
	SetConnect(szAddr);
	CreateTh();
	return TRUE;
}

void CSnmpMonitor::Stop()
{
//	Owner ow(m_csLock);	//del by zfq,2015-08-25

	theLog.Write(_T("CSnmpMonitor::Stop,1"));

	InterlockedExchange(&m_lActive,0);

	theLog.Write(_T("CSnmpMonitor::Stop,2"));

	Shutdown();

	theLog.Write(_T("CSnmpMonitor::Stop,3"));
}

void CSnmpMonitor::StartJobMonitor()
{
// 	Owner ow(m_csLock4JobMonitor);

	if (!m_bEnablePrinterJobMonitor || m_pJobMonitor)
	{
		theLog.Write(_T("##CSnmpMonitor::StartJobMonitor,PrtIP=%s,m_bEnablePrinterJobMonitor=%d,m_pJobMonitor=%p")

			, m_szIP, m_bEnablePrinterJobMonitor, m_pJobMonitor);
		return;
	}

	if (m_eType == BT_SL || m_eType == BT_SL2)
	{
		m_pJobMonitor = new CSnmpJobMonitorSL();
	}
	else if (m_eType == BT_DZ)
	{
		m_pJobMonitor = new CSnmpJobMonitorDZ();
	}
	//del by zxl,20160715,
	//��ҵ���ӵ�Ŀ���Ǵ�ӡ�쳣�˷�,������Ӻܲ�׼ȷ,
	//���ڲ������ַ���:
	//һ,ͨ��snmp��ȡ��ӡ��ʷ��ҵ�б�,�����е���У׼.
	//��,ͨ��snmp����,�Ƚ����ε�¼�ĳ����ֵ,�����ҵ����У׼.
	//Ŀǰֻ��ʩ��֧��snmp��ȡ��ӡ��ʷ��ҵ�б�Ĺ���.
#if 0
	else if (m_eType == BT_KM || m_eType == BT_ZD)
	{
		m_pJobMonitor = new CSnmpJobMonitorKM();
	}
	else if (m_eType == BT_LG)
	{
		m_pJobMonitor = new CSnmpJobMonitorLG();
	}
	else if (m_eType == BT_JC)
	{
		m_pJobMonitor = new CSnmpJobMonitorJC();
	}
#endif
	else
	{
		m_pJobMonitor = NULL;
		theLog.Write(_T("##CSnmpMonitor::StartJobMonitor, m_eType=%d, No Support SNMP Job Monitor"), m_eType);
	}

	if (m_pJobMonitor)
	{
		m_pJobMonitor->SetCallBack(m_pCallBack);
		m_pJobMonitor->SetConnect(m_szIP, m_szCommunity);
		m_pJobMonitor->InitSnmp();
	}
}

void CSnmpMonitor::StopJobMonitor()
{
// 	Owner ow(m_csLock4JobMonitor);

	if (m_pJobMonitor)
	{
		m_pJobMonitor->Release();
		delete m_pJobMonitor;
		m_pJobMonitor = NULL;
	}
}

void CSnmpMonitor::StartPrinterMeterMonitor()
{
// 	Owner ow(m_csLock4MeterMonitor);

	if (!m_bEnablePrinterMeterMonitor)
	{
		theLog.Write(_T("##CSnmpMonitor::StartPrinterMeterMonitor,PrtIP=%s,m_bEnablePrinterMeterMonitor=%d,m_pMeterMonitor=%p")

			, m_szIP, m_bEnablePrinterMeterMonitor, m_pMeterMonitor);
		return;
	}

	

	if (m_pMeterMonitor)
	{
		m_pMeterMonitor->SetCallBack(m_pCallBack);
		m_pMeterMonitor->SetBrandType(m_eType);
		m_pMeterMonitor->SetConnect(m_szIP, m_szCommunity);
		m_pMeterMonitor->InitSnmp();
	}
}

void CSnmpMonitor::StopPrinterMeterMonitor()
{
// 	Owner ow(m_csLock4MeterMonitor);

	if (m_pMeterMonitor)
	{
		m_pMeterMonitor->Release();
		delete m_pMeterMonitor;
		m_pMeterMonitor = NULL;
	}
}

void CSnmpMonitor::StartPrinterMonitorHelper()
{
// 	Owner ow(m_csLock4PrinterMonitorHelper);

	if (!m_bEnablePrinterMonitor || m_pPrinterMonitorHelper)
	{
		theLog.Write(_T("##CSnmpMonitor::StartPrinterMonitorHelper, m_bEnablePrinterMonitor=%d, m_pPrinterMonitorHelper=%p")

			, m_bEnablePrinterMonitor, m_pPrinterMonitorHelper);
		return;
	}
	if (m_pPrinterMonitorHelper)
	{
		m_pPrinterMonitorHelper->SetCallBack(m_pCallBack);
		m_pPrinterMonitorHelper->SetBrandType(m_eType);
		m_pPrinterMonitorHelper->SetPrinterMeterMonitor(m_pMeterMonitor);
		m_pPrinterMonitorHelper->SetPaperConfig(_T(""), m_nMinPaper);
		m_pPrinterMonitorHelper->SetInkConfig(_T(""), m_nMinInk);
		m_pPrinterMonitorHelper->SetConnect(m_szIP, m_szCommunity);
		m_pPrinterMonitorHelper->InitSnmp();
	}
}

void CSnmpMonitor::StopPrinterMonitorHelper()
{
// 	Owner ow(m_csLock4PrinterMonitorHelper);

	if (m_pPrinterMonitorHelper)
	{
		m_pPrinterMonitorHelper->Release();
		delete m_pPrinterMonitorHelper;
		m_pPrinterMonitorHelper = NULL;
	}
}

//���Ӳ����ı�ǲ���
void CSnmpMonitor::CheckBegin()
{
	if (m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::PrinterMonitorCheckBegin, _T("��ʼ���"));
	}
}

void CSnmpMonitor::CheckProcess()
{
	if (m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::PrinterMonitorCheckProcess, _T("���ڼ��"));
	}
}

void CSnmpMonitor::CheckEnd()
{
	if (m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::PrinterMonitorCheckEnd, _T("�������"));
	}
}

//��ֽ
BOOL CSnmpMonitor::IsLowerPager(CString& sDesc)
{
	Owner ow(m_csLock);
	sDesc.Empty();
	if(m_hThread)
	{
		for (int n = 1; n < Max_Paper_Index; n++)
		{
			if (m_aryPaper[n] > 0)
			{
				CString sTmp;
				//theLog.Write(_T("CSnmpMonitor::IsLowerPager,paper(%d),%d"),n,m_oStatus.m_aryPaper[n]);
				if(m_oStatus.m_aryPaper[n] == FX_DC_FIR_PAPER_LESS)
				{
					sTmp.Format(_T("�ڣ�%d��ֽ��%s���٣�"),n,(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
					sDesc += sTmp;
				}
				else if(m_oStatus.m_aryPaper[n] == FX_DC_FIR_PAPER_NO)
				{
					sTmp.Format(_T("�ڣ�%d��ֽ��%sȱֽ��"),n,(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
					sDesc += sTmp;
				}
			}
		}
	}
	return sDesc.IsEmpty() ? FALSE : TRUE;
}
//��ī
BOOL CSnmpMonitor::IsLowerInk(CString& sDesc)
{
	Owner ow(m_csLock);
	CString sTmp;
	sDesc.Empty();
	if(m_hThread)
	{
		for (int n = 1; n < Max_Ink_Index; n++)
		{
			//theLog.Write(_T("Ink(%d),%d"),n,m_oStatus.m_aryInk[n]);
			if(m_oStatus.m_aryInk[n] == FX_DC_FIR_INK_LESS)
			{
				CString sTmp;
				sTmp.Format(_T("��%d��ī���ٷۣ���׼������ī�У�"),n);
				sDesc += sTmp;
			}
		}
	}
	return sDesc.IsEmpty() ? FALSE : TRUE;
}

BOOL CSnmpMonitor::IsOutPager()
{
	Owner ow(m_csLock);
	if(m_hThread)
	{
		for (int n = 1; n < Max_Paper_Index; n++)
		{
			if (m_aryPaper[n] > 0)
			{
				//theLog.Write(_T("paper(%d),%d"),n,m_oStatus.m_aryPaper[n]);
				if(m_oStatus.m_aryPaper[n] != FX_DC_FIR_PAPER_NO)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSnmpMonitor::IsOutPager(CString szPapeType)
{
	theLog.Write(_T("CSnmpMonitor::IsOutPager,0.1"));

	Owner ow(m_csLock);

	theLog.Write(_T("CSnmpMonitor::IsOutPager,0.2"));

	if(m_hThread)
	{
		theLog.Write(_T("CSnmpMonitor::IsOutPager,0.3"));

		for (int n = 1; n < Max_Paper_Index; n++)
		{
			if (m_aryPaper[n] > 0)
			{
				if(m_oStatus.m_aryPaper[n] != FX_DC_FIR_PAPER_NO)
				{
					CString sTmp;
					sTmp.Format(_T("%s"),(m_aryPaper[n] == 4 ? _T("A4"):_T("A3")));
					if (szPapeType.CompareNoCase(sTmp)==0)
					{
//						theLog.Write(_T("!!CSnmpMonitor::IsOutPager,1,szPapeType=%s"), szPapeType);
						return FALSE;
					}
				}
			}
		}

		theLog.Write(_T("CSnmpMonitor::IsOutPager,1.4,szPapeType=%s"), szPapeType);
 
#pragma message("Ŀǰֻ֧��A3,A4����ֽ��,����ֽ����ʱ����ͨ��ȱֽ���.")
		if (szPapeType.CompareNoCase(_T("A4"))!=0 && szPapeType.CompareNoCase(_T("A3"))!=0)
		{
			return FALSE;
		}
		else
		{
			theLog.Write(_T("!!CSnmpMonitor::IsOutPager,2,szPapeType=%s"), szPapeType);
			return TRUE;
		}

		theLog.Write(_T("CSnmpMonitor::IsOutPager,2.1"));
	}
	else //if(m_hThread)
	{
		theLog.Write(_T("CSnmpMonitor::IsOutPager,3,m_hThread=0x%x"), m_hThread);
	}
	return FALSE;
}

BOOL CSnmpMonitor::IsOutManualPaper()
{
	Owner ow(m_csLock);
	if(m_hThread)
	{
		return (m_oStatus.m_aryPaper[0] == FX_DC_FIR_PAPER_NO);
	}
	return TRUE;
}

// #define DEF_GuiZhouCaiJin	//���ݲƾ���ѧ
BOOL CSnmpMonitor::IsJamPaper()
{
	Owner ow(m_csLock);
#ifdef DEF_GuiZhouCaiJin
	if (m_oStatus.m_eDeviceStatus == FX_DC_DEVICE_UNKNOWN)
	{
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CSnmpMonitor::IsOutInk()
{
	Owner ow(m_csLock);
	if(m_hThread)
	{
		for (int n = 1; n < Max_Ink_Index; n++)
		{
			if (m_aryInk[n] > FX_DC_INK_NULL)
			{
				//theLog.Write(_T("ink(%d),%d"),n,m_oStatus.m_aryInk[n]);
				if(m_oStatus.m_aryInk[n] != FX_DC_FIR_INK_NO)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSnmpMonitor::IsOutNet()
{
	Owner ow(m_csLock);
	if(m_hThread)
	{
		if(m_oStatus.m_eDeviceStatus == FX_DC_NET_ERR)
		{
			return TRUE;
		}
	}
	return FALSE;
}

char* CSnmpMonitor::GetCmmt(FX_DC_SCP_STATUS eState)
{
	return m_aryCmmt[eState].szCmmt;
}

void CSnmpMonitor::InitCmmt()
{
	strcpy(m_aryCmmt[FX_DC_OP_IDLE].szCmmt,"��������");

	strcpy(m_aryCmmt[FX_DC_DEVICE_OTHER].szCmmt,"��ӡ������");
	strcpy(m_aryCmmt[FX_DC_DEVICE_UNKNOWN].szCmmt,"��ӡ��δ֪");
	strcpy(m_aryCmmt[FX_DC_DEVICE_IDLE].szCmmt,"��ӡ������");
	strcpy(m_aryCmmt[FX_DC_DEVICE_PRINTING].szCmmt,"��ӡ�����ڴ�ӡ");
	strcpy(m_aryCmmt[FX_DC_DEVICE_WARMUP].szCmmt,"��ӡ��Ԥ����");

	if(m_nA4Pos == 1)
	{
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_OK].szCmmt,"A4ֽ������");
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_LESS].szCmmt,"A4ֽ����ֽ");
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_NO].szCmmt,"A4ֽ��ȱֽ");
	}
	else if (m_nA4Pos == 2)
	{
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_OK].szCmmt,"A4ֽ������");
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_LESS].szCmmt,"A4ֽ����ֽ");
		strcpy(m_aryCmmt[FX_DC_FIR_PAPER_NO].szCmmt,"A4ֽ��ȱֽ");
	}

	if(m_nA3Pos == 1)
	{
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_OK].szCmmt,"A3ֽ������");
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_LESS].szCmmt,"A3ֽ����ֽ");
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_NO].szCmmt,"A3ֽ��ȱֽ");
	}
	else if (m_nA3Pos == 2)
	{
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_OK].szCmmt,"A3ֽ������");
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_LESS].szCmmt,"A3ֽ����ֽ");
		strcpy(m_aryCmmt[FX_DC_SEC_PAPER_NO].szCmmt,"A3ֽ��ȱֽ");
	}
/*
	strcpy(m_ary[FX_DC_SEC_PAPER_OK].szCmmt,"�ڶ���ֽ������");
	strcpy(m_ary[FX_DC_SEC_PAPER_LESS].szCmmt,"�ڶ���ֽ����ֽ");
	strcpy(m_ary[FX_DC_SEC_PAPER_NO].szCmmt,"�ڶ���ֽ��ȱֽ");
*/
	strcpy(m_aryCmmt[FX_DC_FIR_INK_OK].szCmmt,"ī������");
	strcpy(m_aryCmmt[FX_DC_FIR_INK_LESS].szCmmt,"ī����ī");
	strcpy(m_aryCmmt[FX_DC_FIR_INK_NO].szCmmt,"ī��ȱī");

	
}

void CSnmpMonitor::InitBrand(BRAND_TYPE eType)
{
	Owner ow(m_csLock);

	m_eType = eType;
}

void CSnmpMonitor::EnablePrinterInfoMonitor(BOOL bEnable /*= TRUE*/, int nCheckInfoTimeOutSecond /*= 10*/)
{
	Owner ow(m_csLock);

	m_bEnablePrinterInfoMonitor = bEnable;
	m_nCheckInfoTimeOutSecond = nCheckInfoTimeOutSecond;
	theLog.Write(_T("CSnmpMonitor::EnablePrinterInfoMonitor,PrtIP=%s, m_bEnablePrinterInfoMonitor=%d, m_nCheckInfoTimeOutSecond=%d")
		, m_szIP, m_bEnablePrinterInfoMonitor, m_nCheckInfoTimeOutSecond);
}

void CSnmpMonitor::EnablePrinterJobMonitor(BOOL bEnable, int nCheckJobTimeOutSecond /*= 10*/)
{
	Owner ow(m_csLock);

	m_bEnablePrinterJobMonitor = bEnable;
	m_nCheckJobTimeOutSecond = nCheckJobTimeOutSecond;
	theLog.Write(_T("CSnmpMonitor::EnablePrinterJobMonitor,PrtIP=%s, m_bEnablePrinterJobMonitor=%d, m_nCheckJobTimeOutSecond=%d")
		, m_szIP, m_bEnablePrinterJobMonitor, m_nCheckJobTimeOutSecond);
}

void CSnmpMonitor::EnablePrinterMeterMonitor(BOOL bEnable, int nCheckMeterTimeOutSecond /*= 15*/)
{
	Owner ow(m_csLock);

	m_bEnablePrinterMeterMonitor = bEnable;
	m_nCheckMeterTimeOutSecond = nCheckMeterTimeOutSecond;
	theLog.Write(_T("CSnmpMonitor::EnablePrinterMeterMonitor,PrtIP=%s, m_bEnablePrinterMeterMonitor=%d, m_nCheckMeterTimeOutSecond=%d")
		, m_szIP, m_bEnablePrinterMeterMonitor, m_nCheckMeterTimeOutSecond);
}

void CSnmpMonitor::EnablePrinterMonitor(BOOL bEnable, int nCheckMonitorTimeOutSecond /*= 60*/)
{
	Owner ow(m_csLock);


	m_bEnablePrinterMonitor = bEnable;
	m_nCheckMonitorTimeOutSecond = nCheckMonitorTimeOutSecond;
	theLog.Write(_T("CSnmpMonitor::EnablePrinterMonitor,PrtIP=%s, m_bEnablePrinterMonitor=%d, m_nCheckMonitorTimeOutSecond=%d")
		, m_szIP, m_bEnablePrinterMonitor, m_nCheckMonitorTimeOutSecond);
}


//��ȡ��ӡ��ֽ��״̬��Ϣ
/*1.C++��ͨ��snmp��ȡ����ֽ�е���Ϣ
��ʽ:            ֽ����;��һֽ�б��;��һֽ��ֽ��;��һֽ��״̬;        ʾ��[1;1;A4;21;] - ��һֽ��ΪA4,״̬����(FX_DC_FIR_PAPER_OK )
��Ϣ����:        #define WM_PRINT_TRAY_INFO	(WM_USER+1061)      //ֽ����Ϣ
*/
CString CSnmpMonitor::GetPrinterPaperInfo()
{
	Owner ow(m_csLock);
	CString szPaperInfo;
	if(m_hThread)
	{
		int nTrayCount = 0;
		CString szTrayInfo;
		for (int n = 1; n < Max_Paper_Index; n++)
		{
			if (m_aryPaper[n] > 0)
			{
				//theLog.Write(_T("paper(%d),%d"),n,m_oStatus.m_aryPaper[n]);
				nTrayCount++;
				szTrayInfo.AppendFormat(_T("%d;%s;%d;"), n, (m_aryPaper[n] == 4 ? _T("A4"):_T("A3")), m_oStatus.m_aryPaper[n]);
			}
		}
		if (nTrayCount>0)
		{
			szPaperInfo.Format(_T("%d;%s"), nTrayCount, szTrayInfo);
		}
	}
	return szPaperInfo;
}

//��ȡ��ӡ��ī��״̬��Ϣ
/*
2.C++��ͨ��snmp��ȡ����ī�е���Ϣ
��ʽ:            ī����;��һī�б��;��һī������;��һī��״̬;        ʾ��[1;1;4;41;] - ��һī��Ϊ��ɫ(FX_DC_INK_BLACK),״̬����(FX_DC_FIR_INK_OK)
��Ϣ����:        #define WM_PRINT_INK_INFO	(WM_USER+1062)      //ī����Ϣ
*/
CString CSnmpMonitor::GetPrinterInkInfo()
{
	Owner ow(m_csLock);
	CString szInkInfo;
	if(m_hThread)
	{
		int nInkCount = 0;
		CString szInkInfoTemp;
		for (int n = 1; n < Max_Ink_Index; n++)
		{
			if (m_aryInk[n] > FX_DC_INK_NULL)
			{
				//theLog.Write(_T("ink(%d),%d"),n,m_oStatus.m_aryInk[n]);
				nInkCount++;
				szInkInfoTemp.AppendFormat(_T("%d;%d;%d;"), n, m_aryInk[n], m_oStatus.m_aryInk[n]);
			}
		}
		if (nInkCount>0)
		{
			szInkInfo.Format(_T("%d;%s"), nInkCount, szInkInfoTemp);
		}
	}
	return szInkInfo;
}

//��ȡ��ӡ��������Ϣ
BOOL CSnmpMonitor::GetPrinterMeterInfo(PrinterMeterInfo* pInfo)
{
// 	Owner ow(m_csLock4MeterMonitor);

	if (!pInfo || !m_pMeterMonitor)
	{
		theLog.Write(_T("!!CSnmpMonitor::GetPrinterMeterInfo,1,pInfo=%p,m_pMeterMonitor=%p"), pInfo, m_pMeterMonitor);
		return FALSE;
	}
	
	return m_pMeterMonitor->GetLastMeterInfo(pInfo);
}
