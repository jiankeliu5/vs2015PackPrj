#include "StdAfx.h"
#include "SnmpInfoHelper.h"
#include "PrinterBrandInfo.h"

CSnmpInfoHelper::CSnmpInfoHelper(void)
{
	InitOID();
	m_pSystemGroup = NULL;
	m_pInterfacesGroup = NULL;
	m_pHrSystemGroup = NULL;
}

CSnmpInfoHelper::~CSnmpInfoHelper(void)
{
	ClearAll();
}

BOOL CSnmpInfoHelper::InitConnect(CString szIP, CString szCommunity /*= _T("public")*/)
{
	BOOL bSucc = FALSE;
	SetConnect(szIP, szCommunity);
	if (InitSnmp())
	{
		GetAll();
		bSucc = TRUE;
	}
	else
	{
		theLog.Write(_T("!!CSnmpInfoHelper::InitConnect,InitSnmp fail,szIP=%s"), szIP);
	}

	return bSucc;	
}

//add by zfq,2016-09-07 ȡԶ��IP��MAC��ַ
CString CSnmpInfoHelper::GetRmtMac(LPCTSTR szRmtIP)
{
	DWORD	dwRet;
	IPAddr  ipAddr;
	ULONG  pulMac[ 2 ] = {0};
	ULONG  ulLen;
	CString sIP = szRmtIP;
	CString	sMac = ERROR_MAC_ADDRESS;

	ipAddr = inet_addr( ( char * )( LPCTSTR )sIP );
	memset( pulMac,0xff,sizeof( pulMac ) );
	ulLen = 6;

	dwRet = ::SendARP( ipAddr,0,pulMac,&ulLen );
	if( NO_ERROR != dwRet || ulLen == 0 ) 
		return sMac;

	//	ULONG i;
	PBYTE pbHexMac = ( PBYTE )pulMac;

	if(ulLen < 6)
		return sMac;
	// Convert the binary MAC address into human-readable
	sMac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5]);

	return sMac;
}

CString CSnmpInfoHelper::GetPrinterMac()
{
	//CCriticalSection2::Owner lock(m_cs4InterfacesGroup);

	CString szPrinterMac = ERROR_MAC_ADDRESS;//��ӡ��MAC

	if (!m_pInterfacesGroup || !m_pInterfacesGroup->ifTable)
	{
		theLog.Write(_T("!!CSnmpInfoHelper::GetPrinterMac,m_pInterfacesGroup=%p,ifTable=%p")

			, m_pInterfacesGroup, m_pInterfacesGroup->ifTable);
		return szPrinterMac;
	}

	IfEntryMap* ifTable = m_pInterfacesGroup->ifTable;
	IfEntryMap::iterator it;
	for (it=ifTable->begin(); it!=ifTable->end(); it++)
	{
		PIfEntry pEntry = it->second;
		if (pEntry)
		{
			if (pEntry->ifType == ENUM_STRUCT_VALUE(IfType)::ethernet_csmacd
				|| pEntry->ifType == ENUM_STRUCT_VALUE(IfType)::iso88023_csmacd)
			{
				szPrinterMac.Empty();
				for (int i=0; i<6; i++)
				{
					szPrinterMac.AppendFormat(_T("%02X-"), pEntry->ifPhysAddress[i]);
				}
				szPrinterMac.TrimRight(_T("-"));
				break;
			}
		}
	}

	//��һЩ��ӡ����SNMP�У�����û��ifType����һ�ֵͶ˵ľ��ɣ�
	//����������£�����Ĵ���ͻ����ifPhysAddress
	//���Խ��õ��ĵ�һ���Ϸ���ifPhysAddress��Ϊ��ӡ����MAC
	if ((szPrinterMac.CompareNoCase(ERROR_MAC_ADDRESS) == 0) && (ifTable->size() > 0))
	{
		IfEntryMap::iterator it;
		for (it=ifTable->begin(); it!=ifTable->end(); it++)
		{
			PIfEntry pEntry = it->second;
			if (pEntry)
			{
				szPrinterMac.Empty();
				for (int i=0; i<6; i++)
				{
					szPrinterMac.AppendFormat(_T("%02X-"), pEntry->ifPhysAddress[i]);
				}
				szPrinterMac.TrimRight(_T("-"));
				if (szPrinterMac.CompareNoCase(ERROR_MAC_ADDRESS) != 0)
				{
					break;
				}
			}
		}
	}

	//Mac��ַ��Ч��ͨ��ARP���õ�MAC
	if(szPrinterMac.CompareNoCase(ERROR_MAC_ADDRESS) == 0)
	{
		szPrinterMac = GetRmtMac(m_szIP);
		theLog.Write(_T("##CSnmpInfoHelper::GetPrinterMac,m_szIP=%s,szPrinterMac=%s"), m_szIP, szPrinterMac);
	}

	return szPrinterMac;
}

BOOL CSnmpInfoHelper::IsPrinter()
{
	//CCriticalSection2::Owner lock(m_cs4HrDeviceEntryMap);

	BOOL bIsPrinter = FALSE;

	if (GetManufacturerSnmpPrivateKey() == 311)	//windowsϵͳ
	{
		return bIsPrinter;
	}

	HrDeviceEntryMap::iterator it;
	for (it=m_oHrDeviceEntryMap.begin(); it!=m_oHrDeviceEntryMap.end(); it++)
	{
		PHrDeviceEntry pEntry = it->second;
		if (pEntry)
		{
			if (pEntry->hrDeviceType == ENUM_STRUCT_VALUE(HrDeviceTypes)::hrDevicePrinter)
			{
				bIsPrinter = TRUE;
				break;
			}
		}
	}

	if (!bIsPrinter)
	{
		theLog.Write(_T("!!CSnmpInfoHelper::IsPrinter,Not Find hrDevicePrinter,Try Again"));
		int nValue = 0;
		//��ֽ��ҳ��
		if (!GetRequest(CStringToChar(m_szPrtMarkerLifeCountOID), nValue))	//��ֽ��ҳ��="printmib.prtMarker.prtMarkerTable.prtMarkerEntry.prtMarkerLifeCount.1.1"
		{
			theLog.Write(_T("!!CSnmpInfoHelper::IsPrinter,Not Support PrintMIB,A"));
			if (!GetRequest(CStringToChar(m_szPrtMarkerLifeCountExOID), nValue))	//��ֽ��ҳ��="printmib.prtMarker.prtMarkerTable.prtMarkerEntry.prtMarkerLifeCount.1"
			{
				theLog.Write(_T("!!CSnmpInfoHelper::IsPrinter,Not Support PrintMIB,B"));
			}
			else
			{
				bIsPrinter = TRUE;
			}
		}
		else
		{
			bIsPrinter = TRUE;
		}
	}
	
	return bIsPrinter;
}

CString CSnmpInfoHelper::GetModel()
{
	//CCriticalSection2::Owner lock(m_cs4HrDeviceEntryMap);

	CString szModel = _T("");

	HrDeviceEntryMap::iterator it;
	for (it=m_oHrDeviceEntryMap.begin(); it!=m_oHrDeviceEntryMap.end(); it++)
	{
		PHrDeviceEntry pEntry = it->second;
		if (pEntry)
		{
			if (pEntry->hrDeviceType == ENUM_STRUCT_VALUE(HrDeviceTypes)::hrDevicePrinter)
			{
#ifdef UNICODE 
				CString szDeviceDescr = CCommonFun::UTF8ToUnicode((char*)pEntry->hrDeviceDescr);
#else
				CString szDeviceDescr = (char*)pEntry->hrDeviceDescr; 
#endif
				BRAND_TYPE type = GetBrandType();
				if (type == BT_CANON)
				{
					szModel = GetStrByOid(m_szCanonModelOID); //=".1.3.6.1.4.1.1602.1.1.1.1.0"
					if (szModel.GetLength() <= 0)
					{
						szModel = GetSysName();
						if (szModel.GetLength() <= 0)
						{
							szModel = szDeviceDescr;
						}
					}
				}
				else if (type == BT_SL || type == BT_SL2 || type == BT_LG)
				{
					szModel = GetStrByOid(m_szStandardModelOID);	//��ӡ���ͺţ���׼OID=".1.3.6.1.2.1.43.5.1.1.16.1"
					if (szModel.GetLength() <= 0)
					{
						szModel = GetSysName();
						if (szModel.GetLength() <= 0)
						{
							szModel = szDeviceDescr;
						}
					}
				}
				else if (type == BT_LM)
				{
					szModel = GetStrByOid(m_szKonicaMinoltaModelOID); //=".1.3.6.1.4.1.641.2.1.2.1.2.1"
					if (szModel.GetLength() <= 0)
					{
						CStringArray ary;
						CCommonFun::StringSplit(szDeviceDescr, &ary, _T(" "), TRUE);
						if (ary.GetCount() >= 2)
						{
							szModel.Format(_T("%s %s"), ary.GetAt(0), ary.GetAt(1));
						}
						else
						{
							szModel = szDeviceDescr;
						}
					}
				}
				else if (type == BT_JT)
				{
					szModel = GetStrByOid(m_szJingTuModelOID); //=".1.3.6.1.4.1.44966.2.2.2.1.1.1.0"
					if (szModel.GetLength() <= 0)
					{
						szModel = szDeviceDescr;
					}
				}
				else if (type == BT_KM || type == BT_ZD)
				{
					szModel = GetStrByOid(m_szKonicaMinoltaModelOID); //=".1.3.6.1.4.1.18334.1.1.1.1.6.2.1.0"
					if (szModel.GetLength() <= 0)
					{
						szModel = szDeviceDescr;
					}
				}
				else if (type == BT_EPSON)
				{
					szModel = GetStrByOid(m_szEpsonModelOID); //=".1.3.6.1.4.1.1248.1.1.3.1.3.8.0"
					if (szModel.GetLength() <= 0)
					{
						szModel = szDeviceDescr;
					}
				}
				else if (type == BT_SX)
				{
					szModel = GetStrByOid(m_szSamsungModelOID); //=".1.3.6.1.4.1.236.11.5.11.53.31.1.4.0"
					if (szModel.GetLength() <= 0)
					{
						CString szSysDesc = GetSysDescr();
						CStringArray ary;
						CCommonFun::StringSplit(szSysDesc, &ary, _T(";"), TRUE);
						if (ary.GetCount() >= 1)
						{
							szModel.Format(_T("%s"), ary.GetAt(0));
						}
						else
						{
							szModel = szDeviceDescr;
						}
					}
				}
				else
				{
					szModel = szDeviceDescr;
				}

				break;
			}
		}
	}

	//��һЩ��ӡ�У�û��ENUM_STRUCT_VALUE(HrDeviceTypes)::hrDevicePrinter�豸���͵Ľ�㣬
	//��ʱ������豸�ĵ�һ����Ϊ�յ�hrDeviceDescrΪ�豸�ͺ�
	if (szModel.GetLength() <= 0)
	{
		theLog.Write(_T("##CSnmpInfoHelper::GetModel,szModel is Empty,Try Again."));
		for (it=m_oHrDeviceEntryMap.begin(); it!=m_oHrDeviceEntryMap.end(); it++)
		{
			PHrDeviceEntry pEntry = it->second;
			if (pEntry)
			{
#ifdef UNICODE 
				CString szDeviceDescr = CCommonFun::UTF8ToUnicode((char*)pEntry->hrDeviceDescr);
#else
				CString szDeviceDescr = (char*)pEntry->hrDeviceDescr;
#endif
				szModel = szDeviceDescr;
				if (szModel.GetLength() > 0)
				{
					break;
				}
			}
		}
	}

	return szModel;
}

CString CSnmpInfoHelper::GetManufacturer()
{
	CString szManufacturer = ::GetManufacturer(GetBrandType());
	if (szManufacturer.GetLength() <= 0)
	{
		szManufacturer = PRINTER_MANUFACTURER_UNKNOW;
	}
	return  szManufacturer;
}

CString CSnmpInfoHelper::GetManufacturer2()
{
	CString szManufacturer2 = ::GetManufacturer2(GetBrandType());
	if (szManufacturer2.GetLength() <= 0)
	{
		szManufacturer2 = PRINTER_MANUFACTURER2_UNKNOW;
	}
	return  szManufacturer2;
}

BRAND_TYPE CSnmpInfoHelper::GetBrandType()
{
	BRAND_TYPE type = ::GetBrandType(GetManufacturerSnmpPrivateKey());

	//���OEM��ӡ�������ƻ���
	if (type == BT_KM)
	{
		//�ж��Ƿ�Ϊ����OEM���𵩴�ӡ��
		CString szSysName = GetSysName();
		szSysName.MakeUpper();
		CString szModel = GetStrByOid(m_szKonicaMinoltaModelOID); //=".1.3.6.1.4.1.18334.1.1.1.1.6.2.1.0"
		szModel.MakeUpper();
		if ((szSysName.Find(_T("AURORA")) >= 0) || (szModel.Find(_T("AURORA")) >= 0))
		{
			type = BT_ZD;
			theLog.Write(_T("CSnmpInfoHelper::GetBrandType,BT_ZD"));
		}
	}

	return type;	
}

int CSnmpInfoHelper::GetManufacturerSnmpPrivateKey()	//��ȡ�豸�ĳ���˽��SNMP����Key
{
	//CCriticalSection2::Owner lock(m_cs4SystemGroup);

	int nPrivateKey = 0;

	if (m_pSystemGroup)
	{
#ifdef UNICODE
		CString szSysObjectID = CCommonFun::UTF8ToUnicode((char*)m_pSystemGroup->sysObjectID);
#else
		CString szSysObjectID = (char*)m_pSystemGroup->sysObjectID;
#endif
		//.1.3.6.1.4.1.641.2.71107142	//���˴�ӡ����sysObjectID
		CStringArray ary;
		CCommonFun::StringSplit(szSysObjectID, &ary, _T("."), TRUE);
		if (ary.GetCount() > 7)
		{
			nPrivateKey = _tstoi(ary.GetAt(7));
		}
	}

	return nPrivateKey;
}

CString CSnmpInfoHelper::GetPrinterSerialNumber()
{
	//CCriticalSection2::Owner lock(m_cs4PrtGeneralEntryMap);

	CString szPrinterSerialNumber = _T("");
	PrtGeneralEntryMap::iterator it;
	for (it=m_oPrtGeneralEntryMap.begin(); it!=m_oPrtGeneralEntryMap.end(); it++)
	{
		PPrtGeneralEntry pEntry = it->second;
		if (pEntry)
		{
#ifdef UNICODE
			szPrinterSerialNumber = CCommonFun::UTF8ToUnicode((char*)pEntry->prtGeneralSerialNumber);
#else
			szPrinterSerialNumber = (char*)pEntry->prtGeneralSerialNumber;
#endif
		}
		if (szPrinterSerialNumber.GetLength() > 0)
		{
			break;
		}
	}

	//û���ҵ�ͨ�õ��豸���кŽڵ㣬��ʹ��˽�нڵ��ȡ��
	if (szPrinterSerialNumber.GetLength() <= 0)
	{
		BRAND_TYPE type = GetBrandType();
		if (type == BT_CANON)
		{
			szPrinterSerialNumber = GetStrByOid(m_szCanonSerialNumberOID);
		}
		else if (type == BT_LG)
		{
			szPrinterSerialNumber = GetStrByOid(m_szRicohSerialNumberOID);
		}
		else if (type == BT_OKI)
		{
			szPrinterSerialNumber = GetStrByOid(m_szOkiSerialNumberOID);
		}
		else if (type == BT_JC)
		{
			szPrinterSerialNumber = GetStrByOid(m_szKyoceraSerialNumberOID);
		}
		else if (type == BT_EPSON)
		{
			szPrinterSerialNumber = GetStrByOid(m_szEpsonSerialNumberOID);
			if (szPrinterSerialNumber.GetLength() <= 0)
			{
				szPrinterSerialNumber = GetStrByOid(m_szEpsonSerialNumber2OID);
			}
		}
		else if (type == BT_PANTUM)
		{
			szPrinterSerialNumber = GetStrByOid(m_szPantumSerialNumberOID);
		}
	}

	return szPrinterSerialNumber;
}

//�Ƿ�֧�ֲ�ɫ��ӡ
BOOL CSnmpInfoHelper::IsColorCapable()
{
	//�жϴ�ӡ���ж��ٸ���ɫ��������1��������Ϊ�ǲ�ɫ��ӡ���������Ǻڰ״�ӡ����
	int nMarkerColorantCount = m_oPrtMarkerColorantEntryMap.size();
	if (nMarkerColorantCount > 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;		
	}
}

//�Ƿ�֧��˫���ӡ
BOOL CSnmpInfoHelper::IsDoublePrintCapable()
{
	BOOL bRet = FALSE;	//Ĭ��Ϊ��֧��˫���ӡ
	PrtMediaPathEntryMap::iterator it;
	for (it=m_oPrtMediaPathEntryMap.begin(); it!=m_oPrtMediaPathEntryMap.end(); it++)
	{
		PPrtMediaPathEntry pEntry = it->second;
		if (pEntry)
		{
			//�����ӡ���г��߻��߶̱�˫���ֽ��������Ϊ��֧��˫��ġ�
			if (pEntry->prtMediaPathType == ENUM_STRUCT_VALUE(PrtMediaPathTypeTC)::LongEdgeBindingDuplex
				|| pEntry->prtMediaPathType == ENUM_STRUCT_VALUE(PrtMediaPathTypeTC)::ShortEdgeBindingDuplex)
			{
				bRet = TRUE;
				break;
			}
		}
	}
	return bRet;
}

//ͨ��SNMP��ȡOID��Ϣ
BOOL CSnmpInfoHelper::GetAll()
{
	GetSystem();
	GetInterface();
	GetHrSystem();
	GetHrDevice();
	GetHrPrinter();
	GetPrtGeneral();
	GetPrtMarkerColorant();
	GetPrtMediaPath();
	return TRUE;
}

BOOL CSnmpInfoHelper::GetSystem()
{
	//CCriticalSection2::Owner lock(m_cs4SystemGroup);

	if (!m_pSystemGroup)
	{
		m_pSystemGroup = new SystemGroup;
		memset(m_pSystemGroup, 0x0, sizeof(SystemGroup));
	}

	char cOidStr[128] = {0};
	sprintf(cOidStr, "%s.1.%d", m_szSystemGroupOID, 0);	//=SystemGroupOID
	GetRequest(cOidStr, m_pSystemGroup->sysDescr, sizeof(m_pSystemGroup->sysDescr));

	sprintf(cOidStr, "%s.2.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysObjectID, sizeof(m_pSystemGroup->sysObjectID));

	sprintf(cOidStr, "%s.3.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysUpTime);

	sprintf(cOidStr, "%s.4.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysContact, sizeof(m_pSystemGroup->sysContact));

	sprintf(cOidStr, "%s.5.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysName, sizeof(m_pSystemGroup->sysName));

	sprintf(cOidStr, "%s.6.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysLocation, sizeof(m_pSystemGroup->sysLocation));

	sprintf(cOidStr, "%s.7.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysServices);

	sprintf(cOidStr, "%s.8.%d", m_szSystemGroupOID, 0);
	GetRequest(cOidStr, m_pSystemGroup->sysORLastChange);

	m_pSystemGroup->sysORTable = NULL;	//��ʱ����ȡsysORTable

	return TRUE;
}

BOOL CSnmpInfoHelper::GetInterface()
{
	//CCriticalSection2::Owner lock(m_cs4InterfacesGroup);

	if (!m_pInterfacesGroup)
	{
		m_pInterfacesGroup = new InterfacesGroup;
		memset(m_pInterfacesGroup, 0x0, sizeof(InterfacesGroup));
	}

	char cOidStr[128] = {0};
	sprintf(cOidStr, "%s.1.%d", m_szInterfacesGroupOID, 0);	//=InterfacesGroupOID
	GetRequest(cOidStr, m_pInterfacesGroup->ifNumber);

	if (!m_pInterfacesGroup->ifTable)
	{
		m_pInterfacesGroup->ifTable = new IfEntryMap;
	}

	IfEntryMap* ifTable = m_pInterfacesGroup->ifTable;

	//interfaces.ifTable.ifEntry.ifIndex
	//interfaces.ifTable.ifEntry.ifDescr
	//interfaces.ifTable.ifEntry.ifType
	//interfaces.ifTable.ifEntry.ifMtu
	//interfaces.ifTable.ifEntry.ifSpeed
	//interfaces.ifTable.ifEntry.ifPhysAddress
	//interfaces.ifTable.ifEntry.ifAdminStatus
	//interfaces.ifTable.ifEntry.ifOperStatus
	//interfaces.ifTable.ifEntry.ifLastChange
	//interfaces.ifTable.ifEntry.ifInOctets
	//interfaces.ifTable.ifEntry.ifInUcastPkts
	//interfaces.ifTable.ifEntry.ifInNUcastPkts
	//interfaces.ifTable.ifEntry.ifInDiscards
	//interfaces.ifTable.ifEntry.ifInErrors
	//interfaces.ifTable.ifEntry.ifInUnknownProtos
	//interfaces.ifTable.ifEntry.ifOutOctets
	//interfaces.ifTable.ifEntry.ifOutUcastPkts
	//interfaces.ifTable.ifEntry.ifOutNUcastPkts
	//interfaces.ifTable.ifEntry.ifOutDiscards
	//interfaces.ifTable.ifEntry.ifOutErrors
	//interfaces.ifTable.ifEntry.ifOutQLen
	//interfaces.ifTable.ifEntry.ifSpecific

	char *cOidBegin = CStringToChar(m_szIfIndexOID);	//="interfaces.ifTable.ifEntry.ifIndex"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (ifTable->find(nIndex) == ifTable->end())
			{
				PIfEntry pEntry = new IfEntry;
				memset(pEntry, 0x0, sizeof(IfEntry));
				ifTable->insert(pair<int,PIfEntry>(nIndex, pEntry));
				pEntry->ifIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	IfEntryMap::iterator it;
	for (it=ifTable->begin(); it!=ifTable->end(); it++)
	{
		PIfEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->ifIndex;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.%d", m_szIfDescrOID, nIndex);	//interfaces.ifTable.ifEntry.ifDescr
			GetRequest(cOidStr, pEntry->ifDescr, sizeof(pEntry->ifDescr));

			sprintf(cOidStr, "%s.%d", m_szIfTypeOID, nIndex);	//interfaces.ifTable.ifEntry.ifType
			GetRequest(cOidStr, (int&)pEntry->ifType);

			sprintf(cOidStr, "%s.%d", m_szIfMtuOID, nIndex);	//interfaces.ifTable.ifEntry.ifMtu
			GetRequest(cOidStr, pEntry->ifMtu);

			sprintf(cOidStr, "%s.%d", m_szIfSpeedOID, nIndex);	//interfaces.ifTable.ifEntry.ifSpeed
			GetRequest(cOidStr, pEntry->ifSpeed);

			sprintf(cOidStr, "%s.%d", m_szIfPhysAddressOID, nIndex);	//interfaces.ifTable.ifEntry.ifPhysAddress
			GetRequest(cOidStr, pEntry->ifPhysAddress, sizeof(pEntry->ifPhysAddress));

			sprintf(cOidStr, "%s.%d", m_szIfAdminStatusOID, nIndex);	//interfaces.ifTable.ifEntry.ifAdminStatus
			GetRequest(cOidStr, (int&)pEntry->ifAdminStatus);

			sprintf(cOidStr, "%s.%d", m_szIfOperStatusOID, nIndex);		//interfaces.ifTable.ifEntry.ifOperStatus
			GetRequest(cOidStr, (int&)pEntry->ifOperStatus);

			sprintf(cOidStr, "%s.%d", m_szIfLastChangeOID, nIndex);	//interfaces.ifTable.ifEntry.ifLastChange
			GetRequest(cOidStr, pEntry->ifLastChange);

			sprintf(cOidStr, "%s.%d", m_szIfInOctetsOID, nIndex);	//interfaces.ifTable.ifEntry.ifInOctets
			GetRequest(cOidStr, pEntry->ifInOctets);

			sprintf(cOidStr, "%s.%d", m_szIfInUcastPktsOID, nIndex);	//interfaces.ifTable.ifEntry.ifInUcastPkts
			GetRequest(cOidStr, pEntry->ifInUcastPkts);

			sprintf(cOidStr, "%s.%d", m_szIfInNUcastPktsOID, nIndex);	//interfaces.ifTable.ifEntry.ifInNUcastPkts
			GetRequest(cOidStr, pEntry->ifInNUcastPkts);

			sprintf(cOidStr, "%s.%d", m_szIfInDiscardsOID, nIndex);		//interfaces.ifTable.ifEntry.ifInDiscards
			GetRequest(cOidStr, pEntry->ifInDiscards);

			sprintf(cOidStr, "%s.%d", m_szIfInErrorsOID, nIndex);	//interfaces.ifTable.ifEntry.ifInErrors
			GetRequest(cOidStr, pEntry->ifInErrors);

			sprintf(cOidStr, "%s.%d", m_szIfInUnknownProtosOID, nIndex);	//interfaces.ifTable.ifEntry.ifInUnknownProtos
			GetRequest(cOidStr, pEntry->ifInUnknownProtos);

			sprintf(cOidStr, "%s.%d", m_szIfOutOctetsOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutOctets
			GetRequest(cOidStr, pEntry->ifOutOctets);

			sprintf(cOidStr, "%s.%d", m_szIfOutUcastPktsOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutUcastPkts
			GetRequest(cOidStr, pEntry->ifOutUcastPkts);

			sprintf(cOidStr, "%s.%d", m_szIfOutNUcastPktsOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutNUcastPkts
			GetRequest(cOidStr, pEntry->ifOutNUcastPkts);

			sprintf(cOidStr, "%s.%d", m_szIfOutDiscardsOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutDiscards
			GetRequest(cOidStr, pEntry->ifOutDiscards);

			sprintf(cOidStr, "%s.%d", m_szIfOutErrorsOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutErrors
			GetRequest(cOidStr, pEntry->ifOutErrors);

			sprintf(cOidStr, "%s.%d", m_szIfOutQLenOID, nIndex);	//interfaces.ifTable.ifEntry.ifOutQLen
			GetRequest(cOidStr, pEntry->ifOutQLen);

			sprintf(cOidStr, "%s.%d", m_szIfSpecificOID, nIndex);	//interfaces.ifTable.ifEntry.ifSpecific
			GetRequest(cOidStr, pEntry->ifSpecific, sizeof(pEntry->ifSpecific));
		}
	}
	
	return TRUE;
}

BOOL CSnmpInfoHelper::GetHrSystem()
{
	//CCriticalSection2::Owner lock(m_cs4HrSystemGroup);

	if (!m_pHrSystemGroup)
	{
		m_pHrSystemGroup = new HrSystemGroup;
		memset(m_pHrSystemGroup, 0x0, sizeof(HrSystemGroup));
	}

	char cOidStr[128] = {0};
	sprintf(cOidStr, "%s.1.%d", m_szHrSystemGroupOID, 0);	//=HrSystemGroupOID
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemUptime);

	sprintf(cOidStr, "%s.2.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemDate, sizeof(m_pHrSystemGroup->hrSystemDate));

	sprintf(cOidStr, "%s.3.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemInitialLoadDevice);

	sprintf(cOidStr, "%s.4.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemInitialLoadParameters, sizeof(m_pHrSystemGroup->hrSystemInitialLoadParameters));

	sprintf(cOidStr, "%s.5.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemNumUsers);

	sprintf(cOidStr, "%s.6.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemProcesses);

	sprintf(cOidStr, "%s.7.%d", m_szHrSystemGroupOID, 0);
	GetRequest(cOidStr, m_pHrSystemGroup->hrSystemMaxProcesses);

	return TRUE;
}

BOOL CSnmpInfoHelper::GetHrDevice()
{
	//CCriticalSection2::Owner lock(m_cs4HrDeviceEntryMap);

	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceIndex
	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceType
	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceDescr
	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceID
	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceStatus
	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceErrors

	char *cOidBegin = CStringToChar(m_szHrDeviceIndexOID);	//"host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceIndex"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (m_oHrDeviceEntryMap.find(nIndex) == m_oHrDeviceEntryMap.end())
			{
				PHrDeviceEntry pEntry = new HrDeviceEntry;
				memset(pEntry, 0x0, sizeof(HrDeviceEntry));
				m_oHrDeviceEntryMap.insert(pair<int,PHrDeviceEntry>(nIndex, pEntry));
				pEntry->hrDeviceIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	//��Щ��ӡ��û��hrDeviceIndex��OID,����hrDeviceType,��OKI��ӡ��
	if (m_oHrDeviceEntryMap.size() <= 0)
	{
		char *cOidBegin = CStringToChar(m_szHrDeviceTypeOID);	//"host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceType"
		char *cOidCurrent = cOidBegin;
		char pszValue[128] = {0};
		char pszOidNext[128] = {0};
		while (TRUE) 
		{
			if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
				&& OidBeginWithStr(pszOidNext, cOidBegin))
			{
				cOidCurrent = pszOidNext;
				int nIndex = GetOidEndNumber(cOidCurrent);
				if (m_oHrDeviceEntryMap.find(nIndex) == m_oHrDeviceEntryMap.end())
				{
					PHrDeviceEntry pEntry = new HrDeviceEntry;
					memset(pEntry, 0x0, sizeof(HrDeviceEntry));
					m_oHrDeviceEntryMap.insert(pair<int,PHrDeviceEntry>(nIndex, pEntry));
					pEntry->hrDeviceIndex = nIndex;
				}
			}
			else
			{
				break;
			}
		}
	}

	HrDeviceEntryMap::iterator it;
	for (it=m_oHrDeviceEntryMap.begin(); it!=m_oHrDeviceEntryMap.end(); it++)
	{
		PHrDeviceEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->hrDeviceIndex;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.%d", m_szHrDeviceTypeOID, nIndex);		//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceType
			OBJECT_IDENTIFIER* pDeviceType = new OBJECT_IDENTIFIER[OBJECT_IDENTIFIER_SIZE];
			memset(pDeviceType, 0x0, OBJECT_IDENTIFIER_SIZE);
			GetRequest(cOidStr, pDeviceType, OBJECT_IDENTIFIER_SIZE);
			pEntry->hrDeviceType = GetDeviceTypes((char*)pDeviceType);
			delete[] pDeviceType;

			sprintf(cOidStr, "%s.%d", m_szHrDeviceDescrOID, nIndex);	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceDescr
			GetRequest(cOidStr, pEntry->hrDeviceDescr, sizeof(pEntry->hrDeviceDescr));

			sprintf(cOidStr, "%s.%d", m_szHrDeviceIDOID, nIndex);		//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceID
			GetRequest(cOidStr, pEntry->hrDeviceID, sizeof(pEntry->hrDeviceID));

			sprintf(cOidStr, "%s.%d", m_szHrDeviceStatusOID, nIndex);	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceStatus
			GetRequest(cOidStr, (int&)pEntry->hrDeviceStatus);

			sprintf(cOidStr, "%s.%d", m_szHrDeviceErrorsOID, nIndex);	//host.hrDevice.hrDeviceTable.hrDeviceEntry.hrDeviceErrors
			GetRequest(cOidStr, pEntry->hrDeviceErrors);
		}
	}

	return TRUE;
}

BOOL CSnmpInfoHelper::GetHrPrinter()
{
	//CCriticalSection2::Owner lock(m_cs4HrPrinterEntryMap);

	//host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterStatus
	//host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterDetectedErrorState

	char *cOidBegin = CStringToChar(m_szHrPrinterStatusOID);	//"host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterStatus"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (m_oHrPrinterEntryMap.find(nIndex) == m_oHrPrinterEntryMap.end())
			{
				PHrPrinterEntry pEntry = new HrPrinterEntry;
				memset(pEntry, 0x0, sizeof(HrPrinterEntry));
				m_oHrPrinterEntryMap.insert(pair<int,PHrPrinterEntry>(nIndex, pEntry));
				pEntry->hrPrinterStatus = (ENUM_STRUCT_TYPE(HrPrinterStatus))nIndex;
			}
		}
		else
		{
			break;
		}
	}

	HrPrinterEntryMap::iterator it;
	for (it=m_oHrPrinterEntryMap.begin(); it!=m_oHrPrinterEntryMap.end(); it++)
	{
		PHrPrinterEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->hrPrinterStatus;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.%d", m_szHrPrinterStatusOID, nIndex);	//host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterStatus
			GetRequest(cOidStr, (int&)pEntry->hrPrinterStatus);

			sprintf(cOidStr, "%s.%d", m_szHrPrinterDetectedErrorStateOID, nIndex);	//host.hrDevice.hrPrinterTable.hrPrinterEntry.hrPrinterDetectedErrorState
			GetRequest(cOidStr, (int&)pEntry->hrPrinterDetectedErrorState);
		}
	}

	return TRUE;
}

BOOL CSnmpInfoHelper::GetPrtGeneral()
{
	//CCriticalSection2::Owner lock(m_cs4PrtGeneralEntryMap);

	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry.prtGeneralPrinterName		//16
	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry.prtGeneralSerialNumber	//17

	//��Щʱ��һ��Entry��һ���ֶλ�ȱʧ���ر���index�����ֶΡ�
	//��һЩ��ӡ����û��prtGeneralConfigChanges����ѡ��prtGeneralSerialNumber������[HP M775]��
	//PrtGeneralEntryMapĿǰ�����������ǣ�������һ��group��һ��ֻ��һ��������1����
	char *cOidBegin = CStringToChar(m_szPrtGeneralSerialNumberOID);	//"printmib.prtGeneral.prtGeneralTable.prtGeneralEntry.17"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (m_oPrtGeneralEntryMap.find(nIndex) == m_oPrtGeneralEntryMap.end())
			{
				PPrtGeneralEntry pEntry = new PrtGeneralEntry;
				memset(pEntry, 0x0, sizeof(PrtGeneralEntry));
				m_oPrtGeneralEntryMap.insert(pair<int,PPrtGeneralEntry>(nIndex, pEntry));
			}
		}
		else
		{
			break;
		}
	}

	PrtGeneralEntryMap::iterator it;
	for (it=m_oPrtGeneralEntryMap.begin(); it!=m_oPrtGeneralEntryMap.end(); it++)
	{
		int nIndex = it->first;
		PPrtGeneralEntry pEntry = it->second;
		if (pEntry)
		{
			//Ϊ������ٶȣ���ʱ������Ϣû�б�Ҫ��ȡ
			char cOidStr[128] = {0};
			//prtAuxiliarySheetStartupPage�Ժ���ֶ��ǰ汾1���ǲ����ڵģ��ñ�Ŵ��桾16=prtGeneralSerialNumber��
			sprintf(cOidStr, "%s.%d", m_szPrtGeneralPrinterNameOID, nIndex);	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry.16
			GetRequest(cOidStr, pEntry->prtGeneralPrinterName, sizeof(pEntry->prtGeneralPrinterName));

			sprintf(cOidStr, "%s.%d", m_szPrtGeneralSerialNumberOID, nIndex);	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry.17
			GetRequest(cOidStr, pEntry->prtGeneralSerialNumber, sizeof(pEntry->prtGeneralSerialNumber));
		}
	}

	return TRUE;
}

BOOL CSnmpInfoHelper::GetPrtMarkerColorant()
{
	//"printmib.prtMarkerColorant.prtMarkerColorantTable.prtMarkerColorantEntry.prtMarkerColorantValue"
	char *cOidBegin = CStringToChar(m_szPrtMarkerColorantValueOID);
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (m_oPrtMarkerColorantEntryMap.find(nIndex) == m_oPrtMarkerColorantEntryMap.end())
			{
				PPrtMarkerColorantEntry pEntry = new PrtMarkerColorantEntry;
				memset(pEntry, 0x0, sizeof(PrtMarkerColorantEntry));
				strcpy_s((char*)pEntry->prtMarkerColorantValue, sizeof(pEntry->prtMarkerColorantValue), pszValue);
				m_oPrtMarkerColorantEntryMap.insert(pair<int,PPrtMarkerColorantEntry>(nIndex, pEntry));
			}
		}
		else
		{
			break;
		}
	}

	return TRUE;
}

BOOL CSnmpInfoHelper::GetPrtMediaPath()
{
	//"printmib.prtMediaPath.prtMediaPathTable.prtMediaPathEntry.prtMediaPathType"
	char *cOidBegin = CStringToChar(m_szPrtMediaPathTypeOID);
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	while (TRUE) 
	{
		if (GetNextRequestStrEx(cOidCurrent, pszValue, sizeof(pszValue), pszOidNext, sizeof(pszOidNext))
			&& OidBeginWithStr(pszOidNext, cOidBegin))
		{
			cOidCurrent = pszOidNext;
			int nIndex = GetOidEndNumber(cOidCurrent);
			if (m_oPrtMediaPathEntryMap.find(nIndex) == m_oPrtMediaPathEntryMap.end())
			{
				PPrtMediaPathEntry pEntry = new PrtMediaPathEntry;
				memset(pEntry, 0x0, sizeof(PrtMediaPathEntry));
				pEntry->prtMediaPathType = (ENUM_STRUCT_TYPE(PrtMediaPathTypeTC))atoi(pszValue);
				m_oPrtMediaPathEntryMap.insert(pair<int,PPrtMediaPathEntry>(nIndex, pEntry));
			}
		}
		else
		{
			break;
		}
	}

	return TRUE;
}

//����SNMP��ȡ����OID��Ϣ
void CSnmpInfoHelper::ClearAll()
{
	ClearSystem();
	ClearInterface();
	ClearHrSystem();
	ClearHrDevice();
	ClearHrPrinter();
	ClearPrtGeneral();
	ClearPrtMarkerColorant();
	ClearPrtMediaPath();
}

void CSnmpInfoHelper::ClearSystem()
{
	//CCriticalSection2::Owner lock(m_cs4SystemGroup);

	if (m_pSystemGroup)
	{
		SysOREntryMap* sysORTable = m_pSystemGroup->sysORTable;
		if (sysORTable)
		{
			SysOREntryMap::iterator it;
			for (it=sysORTable->begin(); it!=sysORTable->end(); it++)
			{
				PSysOREntry pEntry = it->second;
				if (pEntry)
				{
					delete pEntry;
					pEntry = NULL;
				}
			}
			sysORTable->clear();
			delete sysORTable;
		}
		delete m_pSystemGroup;
		m_pSystemGroup = NULL;
	}
}

void CSnmpInfoHelper::ClearInterface()
{
	//CCriticalSection2::Owner lock(m_cs4InterfacesGroup);

	if (m_pInterfacesGroup)
	{
		IfEntryMap* ifTable = m_pInterfacesGroup->ifTable;
		if (ifTable)
		{
			IfEntryMap::iterator it;
			for (it=ifTable->begin(); it!=ifTable->end(); it++)
			{
				PIfEntry pEntry = it->second;
				if (pEntry)
				{
					delete pEntry;
					pEntry = NULL;
				}
			}
			ifTable->clear();
			delete ifTable;
		}
		delete m_pInterfacesGroup;
		m_pInterfacesGroup = NULL;
	}
}

void CSnmpInfoHelper::ClearHrSystem()
{
	//CCriticalSection2::Owner lock(m_cs4HrSystemGroup);

	if (m_pHrSystemGroup)
	{
		delete m_pHrSystemGroup;
		m_pHrSystemGroup = NULL;
	}
}

void CSnmpInfoHelper::ClearHrDevice()
{
	//CCriticalSection2::Owner lock(m_cs4HrDeviceEntryMap);

	HrDeviceEntryMap::iterator it;
	for (it=m_oHrDeviceEntryMap.begin(); it!=m_oHrDeviceEntryMap.end(); it++)
	{
		PHrDeviceEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oHrDeviceEntryMap.clear();
}

void CSnmpInfoHelper::ClearHrPrinter()
{
	//CCriticalSection2::Owner lock(m_cs4HrPrinterEntryMap);

	HrPrinterEntryMap::iterator it;
	for (it=m_oHrPrinterEntryMap.begin(); it!=m_oHrPrinterEntryMap.end(); it++)
	{
		PHrPrinterEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oHrPrinterEntryMap.clear();
}

void CSnmpInfoHelper::ClearPrtGeneral()
{
	//CCriticalSection2::Owner lock(m_cs4PrtGeneralEntryMap);

	PrtGeneralEntryMap::iterator it;
	for (it=m_oPrtGeneralEntryMap.begin(); it!=m_oPrtGeneralEntryMap.end(); it++)
	{
		PPrtGeneralEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oPrtGeneralEntryMap.clear();
}

void CSnmpInfoHelper::ClearPrtMarkerColorant()
{
	PrtMarkerColorantEntryMap::iterator it;
	for (it=m_oPrtMarkerColorantEntryMap.begin(); it!=m_oPrtMarkerColorantEntryMap.end(); it++)
	{
		PPrtMarkerColorantEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oPrtMarkerColorantEntryMap.clear();
}

void CSnmpInfoHelper::ClearPrtMediaPath()
{
	PrtMediaPathEntryMap::iterator it;
	for (it=m_oPrtMediaPathEntryMap.begin(); it!=m_oPrtMediaPathEntryMap.end(); it++)
	{
		PPrtMediaPathEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oPrtMediaPathEntryMap.clear();
}

ENUM_STRUCT_TYPE(HrDeviceTypes) CSnmpInfoHelper::GetDeviceTypes(CStringA szOidA)
{
	ENUM_STRUCT_TYPE(HrDeviceTypes) type = ENUM_STRUCT_VALUE(HrDeviceTypes)::hrDeviceOther;

	//HrDeviceTypes�ǡ�.1.3.6.1.2.1.25.3.1.x���е����һλ��x��
	if (szOidA.Find(m_szHrDeviceTypePrefixOID) >= 0)	//".1.3.6.1.2.1.25.3.1"
	{
		CStringArray ary;
#ifdef UNICODE
		CString szOid = CCommonFun::MultiByteToUnicode(szOidA);
#else
		CString szOid = szOidA;
#endif
		CCommonFun::StringSplit(szOid, &ary, _T("."), TRUE);
		if (ary.GetCount() == 11)
		{
			type = (ENUM_STRUCT_TYPE(HrDeviceTypes))_tstoi(ary.GetAt(10));
		}
	}

	return type;
}

CString CSnmpInfoHelper::GetSysName()
{
	//CCriticalSection2::Owner lock(m_cs4SystemGroup);

	CString szSysName = _T("");
	if (m_pSystemGroup)
	{
#ifdef UNICODE
		szSysName = CCommonFun::UTF8ToUnicode((char*)m_pSystemGroup->sysName);
#else
		szSysName = m_pSystemGroup->sysName;
#endif
	}
	return szSysName;
}

CString CSnmpInfoHelper::GetSysDescr()
{
	//CCriticalSection2::Owner lock(m_cs4SystemGroup);

	CString szSysDescr = _T("");
	if (m_pSystemGroup)
	{
#ifdef UNICODE
		szSysDescr = CCommonFun::UTF8ToUnicode((char*)m_pSystemGroup->sysDescr);
#else
		szSysDescr = m_pSystemGroup->sysDescr;
#endif
	}
	return szSysDescr;
}

void CSnmpInfoHelper::InitOID()
{
	//prtMarker
	m_szPrtMarkerLifeCountOID.Format("%s.1.1", DecryptOID(PrtMarkerLifeCountOID));
	m_szPrtMarkerLifeCountExOID.Format("%s.1", DecryptOID(PrtMarkerLifeCountOID));

	//prtGeneral
	m_szPrtGeneralEntryOID = DecryptOID(PrtGeneralEntryOID);
#if 0
	m_szPrtGeneralPrinterNameOID = DecryptOID(PrtGeneralPrinterNameOID);
	m_szPrtGeneralSerialNumberOID = DecryptOID(PrtGeneralSerialNumberOID);
#else
	m_szPrtGeneralPrinterNameOID.Format("%s.16", m_szPrtGeneralEntryOID);
	m_szPrtGeneralSerialNumberOID.Format("%s.17", m_szPrtGeneralEntryOID);
#endif

	//prtMarkerColorant
	m_szPrtMarkerColorantValueOID = DecryptOID(PrtMarkerColorantValueOID);

	//prtMediaPath
	m_szPrtMediaPathTypeOID = DecryptOID(PrtMediaPathTypeOID);

	//system
	m_szSystemGroupOID = DecryptOID(SystemGroupOID);

	//hrSystem
	m_szHrSystemGroupOID = DecryptOID(HrSystemGroupOID);

	//PrinterModel
	m_szStandardModelOID = DecryptOID(StandardModelOID);
	m_szCanonModelOID = DecryptOID(CanonModelOID);
	m_szLexmarkModelOID = DecryptOID(LexmarkModelOID);
	m_szJingTuModelOID = DecryptOID(JingTuModelOID);
	m_szKonicaMinoltaModelOID = DecryptOID(KonicaMinoltaModelOID);
	m_szAuroraModelOID = DecryptOID(AuroraModelOID);
	m_szEpsonModelOID = DecryptOID(EpsonModelOID);
	m_szSamsungModelOID = DecryptOID(SamsungModelOID);

	//PrinterSerialNumber
	m_szCanonSerialNumberOID = DecryptOID(CanonSerialNumberOID);
	m_szRicohSerialNumberOID = DecryptOID(RicohSerialNumberOID);
	m_szOkiSerialNumberOID = DecryptOID(OkiSerialNumberOID);
	m_szKyoceraSerialNumberOID = DecryptOID(KyoceraSerialNumberOID);
	m_szEpsonSerialNumberOID = DecryptOID(EpsonSerialNumberOID);
	m_szEpsonSerialNumber2OID = DecryptOID(EpsonSerialNumber2OID);
	m_szPantumSerialNumberOID = DecryptOID(PantumSerialNumberOID);

	//interfaces
	m_szInterfacesGroupOID = DecryptOID(InterfacesGroupOID);

	//interfaces.ifTable.ifEntry
	m_szIfIndexOID = DecryptOID(IfIndexOID);
	m_szIfDescrOID = DecryptOID(IfDescrOID);
	m_szIfTypeOID = DecryptOID(IfTypeOID);
	m_szIfMtuOID = DecryptOID(IfMtuOID);
	m_szIfSpeedOID = DecryptOID(IfSpeedOID);
	m_szIfPhysAddressOID = DecryptOID(IfPhysAddressOID);
	m_szIfAdminStatusOID = DecryptOID(IfAdminStatusOID);
	m_szIfOperStatusOID = DecryptOID(IfOperStatusOID);
	m_szIfLastChangeOID = DecryptOID(IfLastChangeOID);
	m_szIfInOctetsOID = DecryptOID(IfInOctetsOID);
	m_szIfInUcastPktsOID = DecryptOID(IfInUcastPktsOID);
	m_szIfInNUcastPktsOID = DecryptOID(IfInNUcastPktsOID);
	m_szIfInDiscardsOID = DecryptOID(IfInDiscardsOID);
	m_szIfInErrorsOID = DecryptOID(IfInErrorsOID);
	m_szIfInUnknownProtosOID = DecryptOID(IfInUnknownProtosOID);
	m_szIfOutOctetsOID = DecryptOID(IfOutOctetsOID);
	m_szIfOutUcastPktsOID = DecryptOID(IfOutUcastPktsOID);
	m_szIfOutNUcastPktsOID = DecryptOID(IfOutNUcastPktsOID);
	m_szIfOutDiscardsOID = DecryptOID(IfOutDiscardsOID);
	m_szIfOutErrorsOID = DecryptOID(IfOutErrorsOID);
	m_szIfOutQLenOID = DecryptOID(IfOutQLenOID);
	m_szIfSpecificOID = DecryptOID(IfSpecificOID);

	//host.hrDevice.hrDeviceTable.hrDeviceEntry
	m_szHrDeviceIndexOID = DecryptOID(HrDeviceIndexOID);
	m_szHrDeviceTypeOID = DecryptOID(HrDeviceTypeOID);
	m_szHrDeviceDescrOID = DecryptOID(HrDeviceDescrOID);
	m_szHrDeviceIDOID = DecryptOID(HrDeviceIDOID);
	m_szHrDeviceStatusOID = DecryptOID(HrDeviceStatusOID);
	m_szHrDeviceErrorsOID = DecryptOID(HrDeviceErrorsOID);
	m_szHrDeviceTypePrefixOID = DecryptOID(HrDeviceTypePrefixOID);

	//host.hrDevice.hrPrinterTable.hrPrinterEntry
	m_szHrPrinterStatusOID = DecryptOID(HrPrinterStatusOID);
	m_szHrPrinterDetectedErrorStateOID = DecryptOID(HrPrinterDetectedErrorStateOID);
}
