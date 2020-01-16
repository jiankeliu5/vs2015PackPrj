//SNMP����ʩ�ִ�ӡ����ҵ

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
	BOOL GetCurrentJobIndex(CJobList& oCurrentJobList);	//��ȡ��ǰ��ӡ������Ϣ
	int GetJobIndexOld(CString& szJobOid);	//��ȡ��ɵ�һ����ӡ��ҵIndex����ҵ��ʶOID
	bool GetJobInfo(PSCP_JobInfo pJobInfo); //��ȡ��ҵ��Ϣ
	void GetSLPrintJobList();	//ֻ���ʩ�ִ�ӡ��,��ȡ�Ѿ���ӡ�б�

protected:
	void InitOID();
	//ʩ����ҵ����OID
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

	//ʩ�ֵ�ǰ��ҵ��Ϣ
	CStringA m_szXeroxCurentJobListBeginOID;
	CStringA m_szXeroxCurentJobTypeOID;
	CStringA m_szXeroxCurentJobDocNameIndexOID;
	CStringA m_szXeroxCurentJobOwnerIndexOID;
	CStringA m_szXeroxCurentJobSenderIndexOID;
	CStringA m_szXeroxCurentJobDocNameOID;

	//ʩ����ʷ��ҵ��Ϣ
	CStringA m_szXeroxHistoryJobListBeginOID;
	CStringA m_szXeroxHistoryDocNameFormat1OID;
	CStringA m_szXeroxHistoryDocNameFormat151OID;
	CStringA m_szXeroxHistoryDocNameFormatOID;

	//ʩ���滻�ַ���
	CStringA m_szXeroxPrivateEnterprisesOID;
	CStringA m_szXeroxPrivateEnterprises2OID;
	CStringA m_szXeroxPrintStatusOID;
	CStringA m_szXeroxPrintStatus2OID;
	CStringA m_szXeroxPrintedPagesOID;
	CStringA m_szXeroxPrintedPages2OID;
	CStringA m_szXeroxPrintTimeOID;
	CStringA m_szXeroxPrintTime2OID;
};
