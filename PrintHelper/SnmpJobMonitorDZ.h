//SNMP���Ӷ�֥��ӡ����ҵ

#pragma once
#include "SnmpInterface.h"

class CSnmpJobMonitorDZ : public CSnmpJobMonitor
{
public:
	CSnmpJobMonitorDZ();
	~CSnmpJobMonitorDZ(void);
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
	int CheckLastPrintCount2();
	void GetPrintJobList();	//��ȡ�Ѿ���ӡ�б�

							//����У��ɹ��Ĵ�ӡ���ϵĴ�ӡ��¼����ֹ��ӡ���ڲ��Ĵ�ӡ��¼����ʹ���������ӴӶ����²�ѯ��־��Ч�ʽ���
	void ClearPrintJobList(SCP_JobInfo_List& PrintedJobList);

	//����ӡ��¼��־
	void ClearCopyLog(CXmlParse& xml);

	//��ҵ����
	enum ToshibaLogType
	{
		ToshibaLogType_Device,
		ToshibaLogType_Operation,
		ToshibaLogType_Access,
		ToshibaLogType_Message,
		ToshibaLogType_Job,
		ToshibaLogType_Scan,
		ToshibaLogType_Copy,
		ToshibaLogType_Fax,
		ToshibaLogType_FaxReception,
		ToshibaLogType_FaxSent,
		ToshibaLogType_Print,
		ToshibaLogType_Connector,
		ToshibaLogType_SaveAsFileConnector,
		ToshibaLogType_EFilingConnector,
		ToshibaLogType_MailConnector,
		ToshibaLogType_OtherConnector,
		ToshibaLogType_Application,
		ToshibaLogType_Maintenance,
		ToshibaLogType_Erase
	};

	//����ָ�����ͺ�ID����־��¼
	void ClearJobLog(ToshibaLogType nLogType, int nLogId);

protected:
	void InitOID();
	CStringA m_szToshibaTotalPageCountOID;
	SCP_JobInfo_List m_PrintedJobList;
};
