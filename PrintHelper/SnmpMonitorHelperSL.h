//施乐打印机监视帮助类

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperSL : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperSL(void);
	~CSnmpMonitorHelperSL(void);

protected:
	//通过SNMP获取OID信息
	virtual BOOL GetMarkerSupplies();	//重写获取耗材的接口

protected:
	void GetMarkerSuppliesByHtml();
	void Parse(stdtstring& strData);
	void ParseEx(stdtstring& strData, int nMarkerSuppliesType);
	void AddOneMarkerSupplies(stdtstring& strMarkerSuppliesName, int nMarkerSuppliesStatus, int nMarkerSuppliesLevel, int nMarkerSuppliesType);
	void ConvertColorDesc(stdtstring& strColor);
	int ConvertMarkerSuppliesType(int nType);
	int ConvertMarkerSuppliesLevel(int nMarkerSuppliesStatus, int nMarkerSuppliesLevel, int nMarkerSuppliesType);
};
