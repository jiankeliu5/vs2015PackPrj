//理光打印机监视帮助类

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperLG : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperLG(void);
	~CSnmpMonitorHelperLG(void);

protected:
	//通过SNMP获取OID信息
	virtual BOOL GetMarkerSupplies();	//重写获取耗材的接口
	virtual BOOL GetAlert();			//重写获取预警信息
	virtual BOOL IsFalutInfo(CString szDetial);	//重写判断信息是否为故障信息
	virtual CString GetFaultCode(CString szDetial);	//重写获取故障代码

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
