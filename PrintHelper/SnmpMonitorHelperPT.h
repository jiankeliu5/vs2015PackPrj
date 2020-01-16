//奔图打印机监视帮助类

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperPT : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperPT(void);
	~CSnmpMonitorHelperPT(void);

protected:
	//通过SNMP获取OID信息
	virtual BOOL GetMarkerSupplies();	//重写获取耗材的接口
 	virtual BOOL GetCover();//重写获取盖板信息
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
