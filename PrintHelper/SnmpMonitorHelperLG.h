//����ӡ�����Ӱ�����

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperLG : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperLG(void);
	~CSnmpMonitorHelperLG(void);

protected:
	//ͨ��SNMP��ȡOID��Ϣ
	virtual BOOL GetMarkerSupplies();	//��д��ȡ�ĲĵĽӿ�
	virtual BOOL GetAlert();			//��д��ȡԤ����Ϣ
	virtual BOOL IsFalutInfo(CString szDetial);	//��д�ж���Ϣ�Ƿ�Ϊ������Ϣ
	virtual CString GetFaultCode(CString szDetial);	//��д��ȡ���ϴ���

protected:
	void InitOID();
	CStringA m_szRicohEngPrtAlertEntryOID;
	CStringA m_szRicohEngFAXAlertEntryOID;
	CStringA m_szRicohEngCpyAlertEntryOID;
	CStringA m_szRicohEngScnAlertEntryOID;
	CStringA m_szRicohEngCounterEntryOID;
	CStringA m_szRicohTonerColorOID;
	CStringA m_szRicohTonerDescOID;
	CStringA m_szRicohTonerColorTypeOID;
	CStringA m_szRicohTonerLevelPercentOID;
};
