#include "StdAfx.h"
#include "SnmpMonitorHelperLG.h"
#include "RicohMIB_Def.h"

CSnmpMonitorHelperLG::CSnmpMonitorHelperLG(void)
{
	InitOID();
}

CSnmpMonitorHelperLG::~CSnmpMonitorHelperLG(void)
{
}

void CSnmpMonitorHelperLG::InitOID()
{
	m_szRicohEngPrtAlertEntryOID = DecryptOID(RicohEngPrtAlertEntryOID);
	m_szRicohEngFAXAlertEntryOID = DecryptOID(RicohEngFAXAlertEntryOID);
	m_szRicohEngCpyAlertEntryOID = DecryptOID(RicohEngCpyAlertEntryOID);
	m_szRicohEngScnAlertEntryOID = DecryptOID(RicohEngScnAlertEntryOID);
	m_szRicohEngCounterEntryOID = DecryptOID(RicohEngCounterEntryOID);
	m_szRicohTonerColorOID = DecryptOID(RicohTonerColorOID);
	m_szRicohTonerDescOID = DecryptOID(RicohTonerDescOID);
	m_szRicohTonerColorTypeOID = DecryptOID(RicohTonerColorTypeOID);
	m_szRicohTonerLevelPercentOID = DecryptOID(RicohTonerLevelPercentOID);

	//ricohEngFAXAlertEntry比ricohEngPrtAlertEntry、ricohEngCpyAlertEntry和ricohEngScnAlertEntry信息更全
	//所以以ricohEngFAXAlertEntry为标准，获取预警信息。
	//printmib.prtAlert.prtAlertTable.prtAlertEntry
	m_szPrtAlertEntryOID = m_szRicohEngFAXAlertEntryOID;
	m_szPrtAlertIndexOID.Format("%s.1.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertSeverityLevelOID.Format("%s.2.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertTrainingLevelOID.Format("%s.3.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertGroupOID.Format("%s.4.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertGroupIndexOID.Format("%s.5.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertLocationOID.Format("%s.6.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertCodeOID.Format("%s.7.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertDescriptionOID.Format("%s.8.4", m_szRicohEngFAXAlertEntryOID);
	m_szPrtAlertTimeOID.Format("%s.9.4", m_szRicohEngFAXAlertEntryOID);
}

//本函数是重写CSnmpMonitorHelper::GetMarkerSupplies
//只构造和添加墨盒信息
BOOL CSnmpMonitorHelperLG::GetMarkerSupplies()
{
	//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);

	char *cOidBegin = CStringToChar(m_szRicohTonerColorOID); //RicohTonerColorOID
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
			if (m_oMarkerSuppliesMap.find(nIndex) == m_oMarkerSuppliesMap.end())
			{
				PPrtMarkerSuppliesEntry pEntry = new PrtMarkerSuppliesEntry;
				memset(pEntry, 0x0, sizeof(PrtMarkerSuppliesEntry));
				m_oMarkerSuppliesMap.insert(pair<int,PPrtMarkerSuppliesEntry>(nIndex, pEntry));
				pEntry->prtMarkerSuppliesIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	PrtMarkerSuppliesEntryMap::iterator it;
	for (it=m_oMarkerSuppliesMap.begin(); it!=m_oMarkerSuppliesMap.end(); it++)
	{
		PPrtMarkerSuppliesEntry pEntry = it->second;
		if (pEntry)
		{
			pEntry->prtMarkerSuppliesClass = ENUM_STRUCT_VALUE(PrtMarkerSuppliesClassTC)::SupplyThatIsConsumed;
			pEntry->prtMarkerSuppliesType = ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner;	//默认为墨盒信息
			pEntry->prtMarkerSuppliesSupplyUnit = ENUM_STRUCT_VALUE(PrtMarkerSuppliesSupplyUnitTC)::Percent;
			pEntry->prtMarkerSuppliesMaxCapacity = 100;	//本函数获取的是粉盒的剩余百分比，所以默认最大为容量为100。
			
			int nIndex = pEntry->prtMarkerSuppliesIndex;
			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.%d", m_szRicohTonerColorOID, nIndex);	//RicohTonerColorOID
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription, sizeof(pEntry->prtMarkerSuppliesDescription));

			//sprintf(cOidStr, "%s.%d", m_szRicohTonerDescOID, nIndex);	//RicohTonerDescOID
			//GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription);

			//sprintf(cOidStr, "%s.%d", m_szRicohTonerColorTypeOID, nIndex);	//RicohTonerColorTypeOID
			//GetRequest(cOidStr, (int&)pEntry->prtMarkerSuppliesClass);

			sprintf(cOidStr, "%s.%d", m_szRicohTonerLevelPercentOID, nIndex);	//RicohTonerLevelPercentOID
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesLevel);
		}
	}

	return TRUE;
}

//本函数是重写CSnmpMonitorHelper::GetAlert
BOOL CSnmpMonitorHelperLG::GetAlert()
{
	//CCriticalSection2::Owner lock(m_cs4AlertMap);

	char *cOidBegin = CStringToChar(m_szPrtAlertSeverityLevelOID);	//ricohEngFAXAlertSeverityLevel
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
			if (m_oAlertMap.find(nIndex) == m_oAlertMap.end())
			{
				PPrtAlertEntry pEntry = new PrtAlertEntry;
				memset(pEntry, 0x0, sizeof(PrtAlertEntry));
				m_oAlertMap.insert(pair<int,PPrtAlertEntry>(nIndex, pEntry));
				pEntry->prtAlertIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	PrtAlertEntryMap::iterator it;
	for (it=m_oAlertMap.begin(); it!=m_oAlertMap.end(); it++)
	{
		PPrtAlertEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->prtAlertIndex;

			char cOidStr[128] = {0};
			//sprintf(cOidStr, "%s.%d", m_szPrtAlertSeverityLevelOID, nIndex);	//ricohEngFAXAlertIndex	//Not-accessible
			//GetRequest(cOidStr, (int&)pEntry->prtAlertSeverityLevel);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertTrainingLevelOID, nIndex);	//ricohEngFAXAlertTrainingLevel
			GetRequest(cOidStr, (int&)pEntry->prtAlertTrainingLevel);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertGroupOID, nIndex);			//ricohEngFAXAlertGroup
			GetRequest(cOidStr, (int&)pEntry->prtAlertGroup);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertGroupIndexOID, nIndex);		//ricohEngFAXAlertGroupIndex
			GetRequest(cOidStr, pEntry->prtAlertGroupIndex);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertLocationOID, nIndex);			//ricohEngFAXAlertLocation
			GetRequest(cOidStr, pEntry->prtAlertLocation);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertCodeOID, nIndex);				//ricohEngFAXAlertCode
			GetRequest(cOidStr, (int&)pEntry->prtAlertCode);

			sprintf(cOidStr, "%s.%d", m_szPrtAlertDescriptionOID, nIndex);		//ricohEngFAXAlertDescription
			GetRequest(cOidStr, pEntry->prtAlertDescription, sizeof(pEntry->prtAlertDescription));

			sprintf(cOidStr, "%s.%d", m_szPrtAlertTimeOID, nIndex);				//ricohEngFAXAlertTime
			GetRequest(cOidStr, pEntry->prtAlertTime);
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelperLG::IsFalutInfo(CString szDetial)
{
	BOOL bYes = FALSE;
	szDetial.Trim();
	if (szDetial.Find(_T("SC")) >= 0 || szDetial.Find(_T("错误:")) >= 0)
	{
		bYes = TRUE;
	}
	return bYes;
}

CString CSnmpMonitorHelperLG::GetFaultCode(CString szDetial)
{
	CString szFaultCode = _T("");
	szDetial.Trim();

	//联系服务中心:SC142{40800}
	szDetial.MakeUpper();
	int nPos = szDetial.Find(_T("SC"));
	if (nPos >= 0)
	{
		CString szTemp = _T("");
		for (int i=nPos+2; i<szDetial.GetLength(); i++)
		{
			if (!isdigit(szDetial.GetAt(i)))
			{
				break;
			}
			szTemp.AppendFormat(_T("%c"), szDetial.GetAt(i));
		}
		if (szTemp.GetLength() > 0)
		{
			szFaultCode.Format(_T("SC%s"), szTemp);
		}
	}

	return szFaultCode;
}
