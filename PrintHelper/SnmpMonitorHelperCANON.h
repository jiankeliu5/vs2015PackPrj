//���ܴ�ӡ�����Ӱ�����

#pragma once
#include "snmpinterface.h"

class CSnmpMonitorHelperCANON : public CSnmpMonitorHelper
{
public:
	CSnmpMonitorHelperCANON(void);
	~CSnmpMonitorHelperCANON(void);

protected:
	virtual void CheckFaultCode();	//�����Ĺ��ϴ����⣬�Ӵ�ӡ����˽�нڵ��ȡ

protected:
	void InitOID();
	CStringA m_szCanonFaultInfoEntryOID;
	CStringA m_szCanonFaultInfoDescOID;
};
