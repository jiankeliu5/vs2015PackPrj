//��ͼ��ӡ�����Ӱ�����

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperPT : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperPT(void);
	~CSnmpMonitorHelperPT(void);

protected:
	//ͨ��SNMP��ȡOID��Ϣ
	virtual BOOL GetMarkerSupplies();	//��д��ȡ�ĲĵĽӿ�
 	virtual BOOL GetCover();//��д��ȡ�ǰ���Ϣ
// 
// 	virtual BOOL GetInput();
// 	virtual void CheckInput();

	virtual BOOL GetAlert();
	
protected:
	void InitOID();
	CStringA m_szPantumTonerLevelPercentOID;
	//CStringA m_szPantumCoverDescriptionOID;
	CStringA m_szPantumCoverStatusOID;
	CString m_szPantumPrinterStatus;
	CStringA m_szPantumTrayPaperErrorOID;
};
