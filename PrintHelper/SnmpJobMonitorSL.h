//SNMP监视施乐打印机作业

#pragma once
#include "SnmpInterface.h"

class CSnmpJobMonitorSL : public CSnmpJobMonitor
{
public:
	CSnmpJobMonitorSL(void);
	~CSnmpJobMonitorSL(void);
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
	BOOL GetCurrentJobIndex(CJobList& oCurrentJobList);	//获取当前打印队列信息
	int GetJobIndexOld(CString& szJobOid);	//获取最旧的一个打印作业Index和作业标识OID
	bool GetJobInfo(PSCP_JobInfo pJobInfo); //获取作业信息
	void GetSLPrintJobList();	//只针对施乐打印机,获取已经打印列表

protected:
	void InitOID();
	//施乐作业类型OID
	CStringA m_szXeroxJobTypeScanToFileOID;
	CStringA m_szXeroxJobTypeScanToPrintOID;
	CStringA m_szXeroxJobTypeScanToFaxOID;
	CStringA m_szXeroxJobTypeScanToMailListOID;
	CStringA m_szXeroxJobTypeFaxToFileOID;
	CStringA m_szXeroxJobTypeFaxToPrintOID;
	CStringA m_szXeroxJobTypeFaxToMailListOID;
	CStringA m_szXeroxJobTypePrintOID;
	CStringA m_szXeroxJobTypeFileToFaxOID;
	CStringA m_szXeroxJobTypeFileToMailListOID;
	CStringA m_szXeroxJobTypeCopyOID;
	CStringA m_szXeroxJobTypeFileToFileOID;

	//施乐当前作业信息
	CStringA m_szXeroxCurentJobListBeginOID;
	CStringA m_szXeroxCurentJobTypeOID;
	CStringA m_szXeroxCurentJobDocNameIndexOID;
	CStringA m_szXeroxCurentJobOwnerIndexOID;
	CStringA m_szXeroxCurentJobSenderIndexOID;
	CStringA m_szXeroxCurentJobDocNameOID;

	//施乐历史作业信息
	CStringA m_szXeroxHistoryJobListBeginOID;
	CStringA m_szXeroxHistoryDocNameFormat1OID;
	CStringA m_szXeroxHistoryDocNameFormat151OID;
	CStringA m_szXeroxHistoryDocNameFormatOID;

	//施乐替换字符串
	CStringA m_szXeroxPrivateEnterprisesOID;
	CStringA m_szXeroxPrivateEnterprises2OID;
	CStringA m_szXeroxPrintStatusOID;
	CStringA m_szXeroxPrintStatus2OID;
	CStringA m_szXeroxPrintedPagesOID;
	CStringA m_szXeroxPrintedPages2OID;
	CStringA m_szXeroxPrintTimeOID;
	CStringA m_szXeroxPrintTime2OID;
};
