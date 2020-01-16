//SNMP监视京瓷打印机作业

#pragma once
#include "SnmpInterface.h"

class CSnmpJobMonitorJC : public CSnmpJobMonitor
{
public:
	CSnmpJobMonitorJC();
	~CSnmpJobMonitorJC(void);
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
	int GetPrintedCountByJobId(int nJobId);

protected:
	void InitOID();

	//京瓷当前作业信息节点
	CStringA m_szKyoceraActiveJobInfoJobCountOID;

	CStringA m_szKyoceraActiveJobInfoJobIDOID;
	CStringA m_szKyoceraActiveJobInfo3OID;
	CStringA m_szKyoceraActiveJobInfoJobTypeOID;
	CStringA m_szKyoceraActiveJobInfoJobNameOID;
	CStringA m_szKyoceraActiveJobInfoJobStateOID;
	CStringA m_szKyoceraActiveJobInfoJobBeginTimeOID;
	CStringA m_szKyoceraActiveJobInfo8OID;
	CStringA m_szKyoceraActiveJobInfoPrintedCountOID;
	CStringA m_szKyoceraActiveJobInfoPrintedCopyOID;
	CStringA m_szKyoceraActiveJobInfoTotalCopyOID;
	CStringA m_szKyoceraActiveJobInfoUserNameOID;
	CStringA m_szKyoceraActiveJobInfo13OID;
	CStringA m_szKyoceraActiveJobInfoFromOID;
	CStringA m_szKyoceraActiveJobInfoOrgDocPageCountOID;
	CStringA m_szKyoceraActiveJobInfo16OID;
	CStringA m_szKyoceraActiveJobInfoColorTypeOID;

	//京瓷历史作业信息节点
	CStringA m_szKyoceraHistoryJobInfoJobCountOID;

	CStringA m_szKyoceraHistoryJobInfoJobIDOID;
	CStringA m_szKyoceraHistoryJobInfoJobTypeOID;
	CStringA m_szKyoceraHistoryJobInfoJobNameOID;
	CStringA m_szKyoceraHistoryJobInfoJobStateOID;
	CStringA m_szKyoceraHistoryJobInfoBeginTimeOID;
	CStringA m_szKyoceraHistoryJobInfoEndTimeOID;
	CStringA m_szKyoceraHistoryJobInfo8OID;
	CStringA m_szKyoceraHistoryJobInfoPrintedCountOID;
	CStringA m_szKyoceraHistoryJobInfoPrintedCopyOID;
	CStringA m_szKyoceraHistoryJobInfoTotalCopyOID;
	CStringA m_szKyoceraHistoryJobInfoUserNameOID;
	CStringA m_szKyoceraHistoryJobInfo13OID;
	CStringA m_szKyoceraHistoryJobInfo14OID;
	CStringA m_szKyoceraHistoryJobInfoOrgDocPageCountOID;
	CStringA m_szKyoceraHistoryJobInfo16OID;
	CStringA m_szKyoceraHistoryJobInfoColorTypeOID;
};
