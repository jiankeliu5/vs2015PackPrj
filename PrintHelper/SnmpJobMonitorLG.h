//SNMP监视理光打印机作业

#pragma once
#include "SnmpInterface.h"

class CSnmpJobMonitorLG : public CSnmpJobMonitor
{
public:
	CSnmpJobMonitorLG();
	~CSnmpJobMonitorLG(void);
	virtual void CheckJob();
	virtual int CheckLastPrintCount();
	virtual int CheckLastPrintCount_Color();
	virtual int CheckLastPrintCount_WB();
	virtual int CheckLastCopyCount();
	virtual int CheckLastCopyCount_Color();
	virtual int CheckLastCopyCount_WB();
	virtual int CheckLastScanCount();
	virtual int CheckLastScanCount_Color();
	virtual int CheckLastScanCount_WB();
	virtual int CheckLastFaxCount();
	virtual int CheckLastFaxCount_Color();
	virtual int CheckLastFaxCount_WB();
	virtual int CheckLastPageCount_A3();
	virtual int CheckLastPageCount_A4();

protected:
	int GetJobPID(CString szValue);
	int GetJobPage(CString szValue);

protected:
	void InitOID();
	CStringA m_szRicohJobCurrentCountOID;
	CStringA m_szRicohJobMaxCountOID;

	CStringA m_szRicohJobInfo2OID;
	CStringA m_szRicohJobInfo3OID;
	CStringA m_szRicohJobInfoDocNameOID;
	CStringA m_szRicohJobInfo5OID;
	CStringA m_szRicohJobInfoPageOID;
	CStringA m_szRicohJobInfoIDOID;
	CStringA m_szRicohJobInfoCreateTimeOID;
	CStringA m_szRicohJobInfoUserNameOID;
	CStringA m_szRicohJobInfoStateStrOID;
	CStringA m_szRicohJobInfoStateOID;
	CStringA m_szRicohJobInfo12OID;
	CStringA m_szRicohJobInfo13OID;
};
