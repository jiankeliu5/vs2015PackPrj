// SnmpMonitor.h: interface for the CSnmpMonitor class.
//

/////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNMPMONITOR_H__72D628C2_3438_40B1_8628_271517537FAC__INCLUDED_)
#define AFX_SNMPMONITOR_H__72D628C2_3438_40B1_8628_271517537FAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SnmpInterface.h"

class _AA_DLL_EXPORT_ CSnmpMonitor : public CSnmpOP
{
public:
	CSnmpMonitor();
	virtual ~CSnmpMonitor();

	tag_Snmp_Desc m_aryCmmt[SNMP_CMMT_COUNT];


	BOOL OpenSnmp();
	BOOL Start(const TCHAR* szAddr,ISCPStatusInterface* pCall);
	void Stop();
	char* GetCmmt(FX_DC_SCP_STATUS eState);
	BOOL IsOutPager();
	BOOL IsOutPager(CString szPapeType);
	BOOL IsOutManualPaper();
	BOOL IsJamPaper();
	BOOL IsOutInk();
	BOOL IsOutNet();
	//��ֽ
	BOOL IsLowerPager(CString& sDesc);
	BOOL IsLowerInk(CString& sDesc);
	void SetPaperConfig(CString sConfig, int nMin);
	void SetInkConfig(CString sConfig, int nMin);
	
	//����Ʒ��
	void InitBrand(BRAND_TYPE eType);
	void EnablePrinterInfoMonitor(BOOL bEnable = TRUE, int nCheckInfoTimeOutSecond = 10);
	void EnablePrinterJobMonitor(BOOL bEnable, int nCheckJobTimeOutSecond = 10);
	void EnablePrinterMeterMonitor(BOOL bEnable, int nCheckMeterTimeOutSecond = 15);
	void EnablePrinterMonitor(BOOL bEnable, int nCheckMonitorTimeOutSecond = 60);

	//��ȡ��ӡ��״̬
	void GetPrinterStatus(CSCPStatus& status);
	//��ȡ��ӡ��ֽ��״̬��Ϣ
	CString GetPrinterPaperInfo();
	//��ȡ��ӡ��ī��״̬��Ϣ
	CString GetPrinterInkInfo();
	//add by zxl,20150721,��ȡ��ӡ��������Ϣ
	BOOL GetPrinterMeterInfo(PrinterMeterInfo* pInfo);
	BOOL IsRunning();

	BOOL GetScanFileUser(IN CString szFileName, OUT CString& szUserName);
private:
	void InitCmmt();

	void CheckManualPaper();
	void CheckStatus();
	void CheckPaper();
	void CheckOPStatus();
	void CheckDeviceStatus();
	void CheckInk();
	void CheckPower();	//����ӡ����Դ
	void CheckPrinterMeterInfo();
	void CheckPrinterMonitorInfo();
	void CheckScanjobInfo();

	void CheckLmPaper();
	void CheckLmInk();
	void CheckLmDeviceStatus();

	void CheckOkiPaper();

	void CheckPrintCount();
	void CheckCopyCount();
	void CheckScanCount();
	void CheckFaxCount();
	void CheckJob();

	void CreateTh();
	void Shutdown();
	void StartJobMonitor();
	void StopJobMonitor();
	void StartPrinterMeterMonitor();
	void StopPrinterMeterMonitor();
	void StartPrinterMonitorHelper();
	void StopPrinterMonitorHelper();

	//���Ӳ����ı�ǲ���
	void CheckBegin();
	void CheckProcess();
	void CheckEnd();

protected:
	CSCPStatus m_oStatus;

	long m_lActive;
	HANDLE	m_hThreadTerm ;
	HANDLE	m_hThread;
	HANDLE	m_hThreadStarted;

	int m_nA4Pos;
	int m_nA3Pos;
	int m_aryPaper[Max_Paper_Index];
	int m_nMinPaper;

	FX_DC_INK_CMYK m_aryInk[Max_Ink_Index];
	int m_nMinInk;

	DWORD m_dwPowerMaxSecond;	//�жϵ�Դ�������������

	CRITICAL_SECTION m_csLock;
	CCriticalSection2 m_csCheckScanjobLock;
	CSnmpJobMonitor* m_pJobMonitor;
	//CRITICAL_SECTION m_csLock4JobMonitor;
	CSnmpPrinterMeterMonitor* m_pMeterMonitor;
	//CRITICAL_SECTION m_csLock4MeterMonitor;	//add by lzj
	CSnmpMonitorHelper* m_pPrinterMonitorHelper;
	//CRITICAL_SECTION m_csLock4PrinterMonitorHelper;
	CMap<int, int, ST_SCANFILE*, ST_SCANFILE*> m_oScanFileMap;

	DWORD m_lTickCount;
	static unsigned __stdcall ThreadFn(void*pvParam);

	BRAND_TYPE m_eType;

	BOOL m_bEnablePrinterJobMonitor;	//���ô�ӡ����ҵ����
	DWORD m_nCheckJobTimeOutSecond;		//����ӡ����ҵ��Ϣ��ʱ����
	DWORD m_nLastCheckJobTimeOutSecond;	//add by zfq,�ϴμ���ӡ����ҵ��Ϣ��ʱ��,MS

	BOOL m_bEnablePrinterMeterMonitor;	//���ô�ӡ���������
	DWORD m_nCheckMeterTimeOutSecond;	//����ӡ��������Ϣ��ʱ����
	DWORD m_nLastCheckMeterTimeOutSecond;	//add by zfq,Last ����ӡ��������Ϣ��ʱ����

	BOOL m_bEnablePrinterInfoMonitor;	//���ô�ӡ����Ϣ���ӣ�У԰����ҵ��ӡ��
	DWORD m_nCheckInfoTimeOutSecond;	//����ӡ����Ϣ��ʱ������У԰����ҵ��ӡ��
	DWORD m_nLastCheckInfoTimeOutSecond;	//add by zfq,Last ����ӡ����Ϣ��ʱ������У԰����ҵ��ӡ��

	BOOL m_bEnablePrinterMonitor;		//���ô�ӡ��״̬���ӣ��ƴ�ӡ��
	DWORD m_nCheckMonitorTimeOutSecond;	//����ӡ��״̬���ӵ�ʱ�������ƴ�ӡ��
	DWORD m_nLastCheckMonitorTimeOutSecond;	//add by zfq,Last ����ӡ��״̬���ӵ�ʱ�������ƴ�ӡ��

	BOOL m_bEnableConnectPrinter;		//���ô�ӡ������
	DWORD m_nCheckConnectTimeOutSecond;	//����ӡ�����ӵ�ʱ����
	DWORD m_nLastCheckConnectTimeOutSecond;	//add by zfq,Last ����ӡ�����ӵ�ʱ����

	int m_nLastPrintCount;
	int m_nLastPrintCount_Color;
	int m_nLastPrintCount_WB;
	int m_nLastCopyCount;
	int m_nLastA4CopyCount_Color;
	int m_nLastA4CopyCount_WB;
	int m_nLastA3CopyCount_Color;
	int m_nLastA3CopyCount_WB;
	int m_nLastScanCount;
	int m_nLastScanCount_Color;
	int m_nLastScanCount_WB;
	int m_nLastFaxCount;
	int m_nLastFaxCount_Color;
	int m_nLastFaxCount_WB;

	int m_nCheckTick;
};

#endif // !defined(AFX_SNMPMONITOR_H__72D628C2_3438_40B1_8628_271517537FAC__INCLUDED_)
