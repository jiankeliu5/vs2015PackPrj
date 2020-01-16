#include "StdAfx.h"
#include "SnmpMonitorHelperPT.h"
#include "PantumMIB_Def.h"
#include <algorithm>


CSnmpMonitorHelperPT::CSnmpMonitorHelperPT(void)
{
	InitOID();
}

void CSnmpMonitorHelperPT::InitOID()
{
	m_szPantumTonerLevelPercentOID = DecryptOID(PantumTonerLevelPercentOID);
	m_szPantumCoverStatusOID = DecryptOID(PantumCoverStausOID);
	m_szPantumPrinterStatus = DecryptOID(PantumPrinterStatus);
	m_szPantumTrayPaperErrorOID = DecryptOID(PantumTrayPaperErrorOID);
}

CSnmpMonitorHelperPT::~CSnmpMonitorHelperPT(void)
{
}
BOOL CSnmpMonitorHelperPT::GetMarkerSupplies()	//��д��ȡ�ĲĵĽӿ�
{
	//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);

// 	char *cOidBegin = CStringToChar(m_szRicohTonerColorOID); //RicohTonerColorOID
// 	char *cOidCurrent = cOidBegin;
// 	char pszValue[128] = {0};
// 	char pszOidNext[128] = {0};

	int nIndex = 1;//GetOidEndNumber(cOidCurrent);
	if (m_oMarkerSuppliesMap.find(nIndex) == m_oMarkerSuppliesMap.end())
	{
		PPrtMarkerSuppliesEntry pEntry = new PrtMarkerSuppliesEntry;
		memset(pEntry, 0x0, sizeof(PrtMarkerSuppliesEntry));
		m_oMarkerSuppliesMap.insert(pair<int,PPrtMarkerSuppliesEntry>(nIndex, pEntry));
		pEntry->prtMarkerSuppliesIndex = nIndex;
	}

#if 0
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
#endif

	PrtMarkerSuppliesEntryMap::iterator it;
	for (it=m_oMarkerSuppliesMap.begin(); it!=m_oMarkerSuppliesMap.end(); it++)
	{
		PPrtMarkerSuppliesEntry pEntry = it->second;
		if (pEntry)
		{
			pEntry->prtMarkerSuppliesClass = ENUM_STRUCT_VALUE(PrtMarkerSuppliesClassTC)::SupplyThatIsConsumed;
			pEntry->prtMarkerSuppliesType = ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner;	//Ĭ��Ϊī����Ϣ
			pEntry->prtMarkerSuppliesSupplyUnit = ENUM_STRUCT_VALUE(PrtMarkerSuppliesSupplyUnitTC)::Percent;
			pEntry->prtMarkerSuppliesMaxCapacity = 100;	//��������ȡ���Ƿۺе�ʣ��ٷֱȣ�����Ĭ�����Ϊ����Ϊ100��
			
			int nIndex = pEntry->prtMarkerSuppliesIndex;
			char cOidStr[128] = {0};
			//sprintf(cOidStr, "%s.%d", m_szRicohTonerColorOID, nIndex);	//RicohTonerColorOID
			//GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription, sizeof(pEntry->prtMarkerSuppliesDescription));

			//sprintf(cOidStr, "%s.%d", m_szRicohTonerDescOID, nIndex);	//RicohTonerDescOID
			//GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription);

			//sprintf(cOidStr, "%s.%d", m_szRicohTonerColorTypeOID, nIndex);	//RicohTonerColorTypeOID
			//GetRequest(cOidStr, (int&)pEntry->prtMarkerSuppliesClass);
			//theLog.Write(_T("====CSnmpMonitorHelperPT::GetMarkerSupplies,0,oid=%s"),m_szPantumTonerLevelPercentOID);
			sprintf(cOidStr, "%s", m_szPantumTonerLevelPercentOID);	//RicohTonerLevelPercentOID
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesLevel);
			//theLog.Write(_T("====CSnmpMonitorHelperPT::GetMarkerSupplies,test=%d"),pEntry->prtMarkerSuppliesLevel);
		}
	}

	return TRUE;
}
BOOL CSnmpMonitorHelperPT::GetCover()
{
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverIndex
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverStatus

	//CCriticalSection2::Owner lock(m_cs4CoverMap);

	char *cOidBegin = CStringToChar(m_szPrtCoverDescriptionOID);	//"printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	int nIndex = 1;//��ͼ��ӡ���ǰ�ֻ��һ���ڵ�
	if (m_oCoverMap.find(nIndex) == m_oCoverMap.end())
	{
		PPrtCoverEntry pEntry = new PrtCoverEntry;
		memset(pEntry, 0x0, sizeof(PrtCoverEntry));
		m_oCoverMap.insert(pair<int,PPrtCoverEntry>(nIndex, pEntry));
		pEntry->prtCoverIndex = nIndex;
	}

	PrtCoverEntryMap::iterator it;
	for (it=m_oCoverMap.begin(); it!=m_oCoverMap.end(); it++)
	{
		PPrtCoverEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->prtCoverIndex;

			char cOidStr[128] = {0};
// 			sprintf(cOidStr, "%s", m_szPrtCoverDescriptionOID, nIndex);	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription
// 			GetRequest(cOidStr, pEntry->prtCoverDescription, sizeof(pEntry->prtCoverDescription));
			CString szCoverDescription;
			int nCoverStatus = 0;
			sprintf(cOidStr, "%s", m_szPantumCoverStatusOID);	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverStatus
			GetRequest(cOidStr, nCoverStatus);
			if (nCoverStatus > 0)
			{
				pEntry->prtCoverStatus = ENUM_STRUCT_VALUE(PrtCoverStatusTC)::CoverOpen;
				if (1 == nCoverStatus)
				{
					sprintf((char*)pEntry->prtCoverDescription,"%s",CCommonFun::MultiByteToUTF8("ǰ��"));
				}
				else if (2 == nCoverStatus)
				{
					sprintf((char*)pEntry->prtCoverDescription,"%s",CCommonFun::MultiByteToUTF8("���"));
				}
				else if (3 == nCoverStatus)
				{
					sprintf((char*)pEntry->prtCoverDescription,"%s",CCommonFun::MultiByteToUTF8("ADF"));
				}				
			}
			else
			{
				pEntry->prtCoverStatus = ENUM_STRUCT_VALUE(PrtCoverStatusTC)::CoverClosed;
			}
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelperPT::GetAlert()
{
	//CCriticalSection2::Owner lock(m_cs4AlertMap);
	char *cOidBegin = CStringToChar(m_szPrtAlertSeverityLevelOID);	//"printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertSeverityLevel"
	char *cOidCurrent = cOidBegin;
	char pszValue[128] = {0};
	char pszOidNext[128] = {0};
	int nIndex = 1;
	if (m_oAlertMap.find(nIndex) == m_oAlertMap.end())
	{
		PPrtAlertEntry pEntry = new PrtAlertEntry;
		memset(pEntry, 0x0, sizeof(PrtAlertEntry));
		m_oAlertMap.insert(pair<int,PPrtAlertEntry>(nIndex, pEntry));
		pEntry->prtAlertIndex = nIndex;
	}	

	PrtAlertEntryMap::iterator it;
	for (it=m_oAlertMap.begin(); it!=m_oAlertMap.end(); it++)
	{
		PPrtAlertEntry pEntry = it->second;
		if (pEntry)
		{
			//int nIndex = pEntry->prtAlertIndex;
			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s", m_szPantumPrinterStatus);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertCode
			int nValue = 0;
			GetRequest(cOidStr, nValue);
			theLog.Write(_T("CSnmpMonitorHelperPT::GetAlert,nValue=%d"),nValue);
			CString szPrinterStatus;
			if (nValue == 0)
			{
				szPrinterStatus = _T("��ʼ��");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��ʼ��"));
			}
			if (nValue == 1)
			{
				szPrinterStatus = _T("����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("����"));
			}
			if (nValue == 2)
			{
				szPrinterStatus = _T("����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("����"));
			}
			if (nValue == 3)
			{
				szPrinterStatus = _T("Ԥ����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("Ԥ����"));
			}
			if (nValue == 4)
			{
				szPrinterStatus = _T("��ӡ��");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��ӡ��"));
			}
			if (nValue == 5)
			{
				szPrinterStatus = _T("ȡ����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("ȡ����"));
			}
			if (nValue == 6)
			{
				szPrinterStatus = _T("������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("������"));
			}
			if (nValue == 7)
			{
				szPrinterStatus = _T("ɨ����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("ɨ����"));
			}
			if (nValue == 8)
			{
				szPrinterStatus = _T("��ӡ��");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��ӡ��"));
			}
			if (nValue == 9)
			{
				szPrinterStatus = _T("���ڷ���");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("���ڷ���"));
			}
			if (nValue == 10)
			{
				szPrinterStatus = _T("�ڴ��");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("�ڴ��"));
			}
			else if (nValue == 11)
			{
				szPrinterStatus = _T("����");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("�ڴ���"));
			}
			else if (nValue == 12)
			{
				szPrinterStatus = _T("���ڲ���");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("���ڲ���"));
			}
			else if (nValue == 13)
			{
				szPrinterStatus = _T("��������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��������"));
			}
			else if (nValue == 14)
			{
				szPrinterStatus = _T("���ڽ���");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("���ڽ���"));
			}
			else if (nValue == 15)
			{
				szPrinterStatus = _T("�������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("�������"));
			}
			else if (nValue == 16)
			{
				szPrinterStatus = _T("�绰");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("�绰"));
			}
			else if (nValue == 17)
			{
				szPrinterStatus = _T("����ʼ������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("����ʼ������"));
			}
			else if (nValue == 18)
			{
				szPrinterStatus = _T("��Ӧ��");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��Ӧ��"));
			}
			else if (nValue == 501)
			{
				szPrinterStatus = _T("�ۺл���������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("�ۺл���������"));
			}
			else if (nValue == 502)
			{
				szPrinterStatus = _T("���Ǵ���");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("���Ǵ���"));
			}
			else if (nValue == 503)
			{
				szPrinterStatus = _T("ֽ�л�ֽ�Ŵ���");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("ֽ�л�ֽ�Ŵ���"));
			}
			else if (nValue == 504)
			{
				szPrinterStatus = _T("��������");
				//sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8("��������"));
			}
			CString szTmp;
			memset(cOidStr,0,128);
			sprintf(cOidStr, "%s", m_szPantumTrayPaperErrorOID);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertCode
			GetRequest(cOidStr, nValue);
			BYTE byLow = (BYTE)nValue;
			BYTE byHigh = (BYTE)(nValue >> 8);
			CString szStatus;
			CString szTray;
			theLog.Write(_T("SnmpMonitorHelperPT::GetAlert,1,nValue=%x,byLow=%x,byHigh=%x"),nValue,byLow,byHigh);
			if (byLow == 0x00)
			{
				szStatus = _T("����");
			}
			else if (byLow == 0x01)
			{
				szStatus = _T("ȱֽ");
			}
			else if (byLow == 0x02)
			{
				szStatus = _T("��ֽʧ��");
			}
			else if (byLow == 0x03)
			{
				szStatus = _T("��ֽ����ֽ");
			}
			else if (byLow == 0x04)
			{
				szStatus = _T("ֽ����Դ��ƥ��");
			}
			else if (byLow == 0x05)
			{
				szStatus = _T("ֽ�����ò�ƥ��");
			}
			else if (byLow == 0x06)
			{
				szStatus = _T("δ��װ");
			}
			else if (byLow == 0x07)
			{
				szStatus = _T("ֽ���Ѵ�");
			}
			else if (byLow == 0x08)
			{
				szStatus = _T("����1");
			}
			else if (byLow == 0x09)
			{
				szStatus = _T("����2");
			}
			else if (byLow == 0x0A)
			{
				szStatus = _T("����3");
			}
			else if (byLow == 0x0B)
			{
				szStatus = _T("ֽ�Ͳ�ƥ��");
			}
			else if (byLow == 0x80)
			{
				szStatus = _T("��ֽ����ֽ");
			}
			else if (byLow == 0x81)
			{
				szStatus = _T("�м俨ֽ");
			}
			else if (byLow == 0x82)
			{
				szStatus = _T("˫�浥Ԫ��ֽ");
			}
			else if (byLow == 0x83)
			{
				szStatus = _T("�Զ�+�ֶ���ֽ��ȱֽ");
			}
			else if (byLow == 0x84)
			{
				szStatus = _T("ADF��ֽ");
			}
			else if (byLow == 0x85)
			{
				szStatus = _T("�Զ�+�ֶ���ֽ�в�����");
			}
//=====================================================
			if (byHigh == 0x00)
			{
				szTray = _T("������ֽ��");
			}
			else if (byHigh == 0x01)
			{
				szTray = _T("�Զ���ֽ��");
			}
			else if (byHigh == 0x02)
			{
				szTray = _T("�ֶ���ֽ��");
			}
			else if (byHigh == 0x03)
			{
				szTray = _T("ѡ��ֽ��1");
			}
			else if (byHigh == 0x04)
			{
				szTray = _T("ѡ��ֽ��2");
			}
			else if (byHigh == 0x05)
			{
				szTray = _T("ADF");
			}
			//=====================================================
			szTmp.Format(_T("��ӡ��״̬��%s��ֽ�У�%s%s"),szPrinterStatus,szTray,szStatus);
			theLog.Write(_T("CSnmpMonitorHelperPT::GetAlert,alert content=%s,szPrinterStatus=%s,szTray=%s,szStatus=%s")

				,szTmp,szPrinterStatus,szTray,szStatus);
#ifdef UNICODE
			sprintf((char*)pEntry->prtAlertDescription, "%s", CCommonFun::UnicodeToUTF8(szTmp.GetString()));
#else
			sprintf((char*)pEntry->prtAlertDescription,"%s",CCommonFun::MultiByteToUTF8(szTmp.GetString()));
#endif
			pEntry->prtAlertCode = ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Other;			
		}
	}

	return TRUE;
}
