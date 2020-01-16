//ʩ�ִ�ӡ�����Ӱ�����

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperSL : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperSL(void);
	~CSnmpMonitorHelperSL(void);

protected:
	//ͨ��SNMP��ȡOID��Ϣ
	virtual BOOL GetMarkerSupplies();	//��д��ȡ�ĲĵĽӿ�

protected:
	void GetMarkerSuppliesByHtml();
	void Parse(stdtstring& strData);
	void ParseEx(stdtstring& strData, int nMarkerSuppliesType);
	void AddOneMarkerSupplies(stdtstring& strMarkerSuppliesName, int nMarkerSuppliesStatus, int nMarkerSuppliesLevel, int nMarkerSuppliesType);
	void ConvertColorDesc(stdtstring& strColor);
	int ConvertMarkerSuppliesType(int nType);
	int ConvertMarkerSuppliesLevel(int nMarkerSuppliesStatus, int nMarkerSuppliesLevel, int nMarkerSuppliesType);
};
