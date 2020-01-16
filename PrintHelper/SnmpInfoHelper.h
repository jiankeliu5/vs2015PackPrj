#pragma once
#include "snmpinterface.h"
#include "RFC1213MIB_Def.h"
#include "HostResourcesMIB_Def.h"
#include "PrinterInfoMIB_Def.h"

class _AA_DLL_EXPORT_ CSnmpInfoHelper : public CSnmpOP
{
public:
	CSnmpInfoHelper(void);
	~CSnmpInfoHelper(void);

	BOOL InitConnect(CString szIP, CString szCommunity = _T("public"));
	CString GetPrinterMac();

	//add by zfq,2016-09-07 ȡԶ��IP��MAC��ַ
	CString GetRmtMac(LPCTSTR szRmtIP);

	BOOL IsPrinter();
	CString GetModel();
	CString GetManufacturer();
	CString GetManufacturer2();
	BRAND_TYPE GetBrandType();
	int GetManufacturerSnmpPrivateKey();	//��ȡ�豸�ĳ���˽��SNMP����Key
	CString GetPrinterSerialNumber();		//��ȡ�豸�����к�
	BOOL IsColorCapable();			//�Ƿ�֧�ֲ�ɫ��ӡ
	BOOL IsDoublePrintCapable();	//�Ƿ�֧��˫���ӡ

protected:
	//ͨ��SNMP��ȡOID��Ϣ
	BOOL GetAll();
	BOOL GetSystem();
	BOOL GetInterface();
	BOOL GetHrSystem();
	BOOL GetHrDevice();
	BOOL GetHrPrinter();
	BOOL GetPrtGeneral();
	BOOL GetPrtMarkerColorant();
	BOOL GetPrtMediaPath();

	//����SNMP��ȡ����OID��Ϣ
	void ClearAll();
	void ClearSystem();
	void ClearInterface();
	void ClearHrSystem();
	void ClearHrDevice();
	void ClearHrPrinter();
	void ClearPrtGeneral();
	void ClearPrtMarkerColorant();
	void ClearPrtMediaPath();

	//����
	ENUM_STRUCT_TYPE(HrDeviceTypes) GetDeviceTypes(CStringA szOid);
	CString GetSysName();
	CString GetSysDescr();
	void InitOID();

protected:
	//����ͨ��SNMP��ȡOID��Ϣ��MAP��Group
	PSystemGroup m_pSystemGroup;
	PInterfacesGroup m_pInterfacesGroup;
	PHrSystemGroup m_pHrSystemGroup;
	HrDeviceEntryMap m_oHrDeviceEntryMap;
	HrPrinterEntryMap m_oHrPrinterEntryMap;
	PrtGeneralEntryMap m_oPrtGeneralEntryMap;
	PrtMarkerColorantEntryMap m_oPrtMarkerColorantEntryMap;
	PrtMediaPathEntryMap m_oPrtMediaPathEntryMap;

	//ͨ��SNMP��ȡOID��Ϣ��MAP��Group���߳���
// 	CCriticalSection2 m_cs4SystemGroup;
// 	CCriticalSection2 m_cs4InterfacesGroup;
// 	CCriticalSection2 m_cs4HrSystemGroup;
// 	CCriticalSection2 m_cs4HrDeviceEntryMap;
// 	CCriticalSection2 m_cs4HrPrinterEntryMap;
// 	CCriticalSection2 m_cs4PrtGeneralEntryMap;

protected:
	//prtMarker
	CStringA m_szPrtMarkerLifeCountOID;
	CStringA m_szPrtMarkerLifeCountExOID;

	//prtGeneral
	CStringA m_szPrtGeneralEntryOID;
	CStringA m_szPrtGeneralPrinterNameOID;
	CStringA m_szPrtGeneralSerialNumberOID;

	//prtMarkerColorant
	CStringA m_szPrtMarkerColorantValueOID;

	//prtMediaPath
	CStringA m_szPrtMediaPathTypeOID;

	//PrinterModel
	CStringA m_szStandardModelOID;
	CStringA m_szCanonModelOID;
	CStringA m_szLexmarkModelOID;
	CStringA m_szJingTuModelOID;
	CStringA m_szKonicaMinoltaModelOID;
	CStringA m_szAuroraModelOID;
	CStringA m_szEpsonModelOID;
	CStringA m_szSamsungModelOID;

	//PrinterSerialNumber
	CStringA m_szCanonSerialNumberOID;
	CStringA m_szRicohSerialNumberOID;
	CStringA m_szOkiSerialNumberOID;
	CStringA m_szKyoceraSerialNumberOID;
	CStringA m_szEpsonSerialNumberOID;
	CStringA m_szEpsonSerialNumber2OID;
	CStringA m_szPantumSerialNumberOID;

	//system
	CStringA m_szSystemGroupOID;

	//hrSystem
	CStringA m_szHrSystemGroupOID;

	//interfaces
	CStringA m_szInterfacesGroupOID;

	//interfaces.ifTable.ifEntry
	CStringA m_szIfIndexOID;
	CStringA m_szIfDescrOID;
	CStringA m_szIfTypeOID;
	CStringA m_szIfMtuOID;
	CStringA m_szIfSpeedOID;
	CStringA m_szIfPhysAddressOID;
	CStringA m_szIfAdminStatusOID;
	CStringA m_szIfOperStatusOID;
	CStringA m_szIfLastChangeOID;
	CStringA m_szIfInOctetsOID;
	CStringA m_szIfInUcastPktsOID;
	CStringA m_szIfInNUcastPktsOID;
	CStringA m_szIfInDiscardsOID;
	CStringA m_szIfInErrorsOID;
	CStringA m_szIfInUnknownProtosOID;
	CStringA m_szIfOutOctetsOID;
	CStringA m_szIfOutUcastPktsOID;
	CStringA m_szIfOutNUcastPktsOID;
	CStringA m_szIfOutDiscardsOID;
	CStringA m_szIfOutErrorsOID;
	CStringA m_szIfOutQLenOID;
	CStringA m_szIfSpecificOID;

	//host.hrDevice.hrDeviceTable.hrDeviceEntry
	CStringA m_szHrDeviceIndexOID;
	CStringA m_szHrDeviceTypeOID;
	CStringA m_szHrDeviceDescrOID;
	CStringA m_szHrDeviceIDOID;
	CStringA m_szHrDeviceStatusOID;
	CStringA m_szHrDeviceErrorsOID;
	CStringA m_szHrDeviceTypePrefixOID;

	//host.hrDevice.hrPrinterTable.hrPrinterEntry
	CStringA m_szHrPrinterStatusOID;
	CStringA m_szHrPrinterDetectedErrorStateOID;
};
