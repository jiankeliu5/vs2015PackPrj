#include "stdafx.h"
#include "SnmpInterface.h"

CSnmpOP::CSnmpOP()
{
	m_bIsActiveIP = FALSE;
	m_bIsSupportSnmp = FALSE;
	m_pCallBack = NULL;
	m_lpMgrSession = NULL;
	m_szIP.Empty();
	m_szCommunity = _T("public");
}

CSnmpOP::~CSnmpOP()
{
	ReleaseSnmp();
}

BOOL CSnmpOP::InitSnmp()
{
	theLog.Write(_T("CSnmpOP::InitSnmp,1,m_szIP=%s, m_szCommunity=%s, m_lpMgrSession=%p"), m_szIP, m_szCommunity, m_lpMgrSession);
	if (m_lpMgrSession)
	{
		return TRUE;
	}

	if (!CheckIPActive())
	{
		theLog.Write(_T("!!CSnmpOP::InitSnmp,2,CheckIPActive fail,m_szIP=%s"), m_szIP);
		//return FALSE;
	}

	CStringA szIPA = CCommonFun::UnicodeToMultiByte(m_szIP);
	CStringA szCommunityA = CCommonFun::UnicodeToMultiByte(m_szCommunity);
	m_lpMgrSession = SnmpMgrOpen(CStringToChar(szIPA), CStringToChar(szCommunityA), 1000, 3);
	if(m_lpMgrSession == NULL)
	{
		theLog.Write(_T("!!CSnmpOP::InitSnmp,3,SnmpMgrOpen err=%d"), ::GetLastError());
		return FALSE;
	}
	else
	{
		if (CheckSupportSnmp())
		{
			theLog.Write(_T("CSnmpOP::InitSnmp,4,succ,m_lpMgrSession=%p"), m_lpMgrSession);
			return TRUE;
		}
		else
		{
			theLog.Write(_T("!!CSnmpOP::InitSnmp,5,fail,m_szIP=%s,m_bIsActiveIP=%d,m_bIsSupportSnmp=%d")
				, m_szIP, m_bIsActiveIP, m_bIsSupportSnmp);
			return FALSE;
		}
	}
}

void CSnmpOP::SetConnect(CString szIP, CString szCommunity /*= _T("public")*/)
{
	m_szIP = szIP;
	m_szCommunity = szCommunity;
	theLog.Write(_T("CSnmpOP::SetConnect,m_szIP=%s, m_szCommunity=%s"), m_szIP, m_szCommunity);
}

void CSnmpOP::SetCallBack(ISCPStatusInterface* pCallBack)
{
	m_pCallBack = pCallBack;
	theLog.Write(_T("CSnmpOP::SetCallBack,m_pCallBack=%p"), m_pCallBack);
}

CString CSnmpOP::GetPrtIP()
{
	return m_szIP;
}

void CSnmpOP::ReleaseSnmp()
{
	m_bIsActiveIP = FALSE;
	m_bIsSupportSnmp = FALSE;
	m_pCallBack = NULL;
	m_szIP.Empty();
	m_szCommunity = _T("public");
	if(m_lpMgrSession)
	{
		SnmpMgrClose(m_lpMgrSession);
		m_lpMgrSession = NULL;
	}
}
void CSnmpOP::Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )  
{  
	int  i;  
	char szTmp[3];  

	for( i = 0; i < nSrcLen; i++ )  
	{  
		sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );  
		memcpy( &sDest[i * 2], szTmp, 2 );  
	}  
	return ;  
} 
char * CSnmpOP::SNMP_AnyToHexStr(AsnObjectSyntax *sAny, UINT& uLen)
{
	if (!sAny)
	{
		return NULL;
	}

	DWORD dwValue = 0;
	//UINT uLen = 0;
	BYTE *puData = 0;
	char *pString = NULL;

	//theLog.Write(_T("CSnmpOP::SNMP_AnyToHexStr,type=%d"),sAny->asnType);
	switch ( sAny->asnType )
	{
	case ASN_INTEGER:   
		pString = (char *) SnmpUtilMemAlloc(33);
		if(pString)
			ltoa(sAny->asnValue.number, pString, 10 );
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_COUNTER:
		dwValue = sAny->asnValue.counter;
		pString = (char *) SnmpUtilMemAlloc(33);
		if(pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_GAUGE:
		dwValue = sAny->asnValue.gauge;
		pString = (char *) SnmpUtilMemAlloc(33);
		if (pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_TIMETICKS:
		dwValue = sAny->asnValue.ticks;
		pString = (char *) SnmpUtilMemAlloc(33);
		if (pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_OCTETSTRING:   /* Same as ASN_RFC1213_DISPSTRING */
		uLen = sAny->asnValue.string.length*8+1;
		puData = sAny->asnValue.string.stream;
		pString = (char *) SnmpUtilMemAlloc(uLen + 1);
		if(pString)
		{
			if (sAny->asnValue.arbitrary.length)
			{
				char test[200] = {0};
				Hex2Str((const char *)puData,test,sizeof(puData));
				//theLog.Write(_T("CSnmpOP::SNMP_AnyToHexStr,1,test=%s"),test);
				memcpy(pString, test, uLen-1);
				//theLog.Write(_T("CSnmpOP::SNMP_AnyToHexStr,2,pString=%s"),pString);
				//strncpy(pString, (const char*)puData, uLen -1);
				pString[uLen] = '\0';
			}
		}
		break;
	case ASN_SEQUENCE:      /* Same as ASN_SEQUENCEOF */
		uLen = sAny->asnValue.sequence.length;
		puData = sAny->asnValue.sequence.stream;
		//theLog.Write(_T("CSnmpOP::SNMP_AnyToStr,6=="));
		if(pString)
		{
			if (sAny->asnValue.arbitrary.length)
			{
				memcpy(pString, puData, uLen-1);
				//strncpy(pString, (const char*)puData, uLen -1);
				pString[uLen] = '\0';
			}
		}
		break;
	case ASN_RFC1155_IPADDRESS:
		//theLog.Write(_T("CSnmpOP::SNMP_AnyToStr,7=="));
		if (sAny->asnValue.address.length )
		{
			UINT i;
			char szBuf[17];

			uLen = sAny->asnValue.address.length;
			puData = sAny->asnValue.address.stream;

			pString = (char *) SnmpUtilMemAlloc( uLen * 4 );
			if(pString)
			{
				pString[0] = '\0';

				for (i = 0; i < uLen; i++)
				{
					lstrcatA( pString, itoa( puData[i], szBuf, 10 ) );
					if( i < uLen-1 )
						lstrcatA( pString, "." );
				}
				uLen = strlen(pString) + 1;
			}
		}
		else
			pString = NULL;
		break;
	case ASN_RFC1155_OPAQUE:
		//theLog.Write(_T("CSnmpOP::SNMP_AnyToStr,8=="));
		if( sAny->asnValue.arbitrary.length )
		{
			uLen = sAny->asnValue.arbitrary.length;
			puData = sAny->asnValue.arbitrary.stream;
			pString = (char *) SnmpUtilMemAlloc(uLen + 1);
			if (pString)
			{
				if (sAny->asnValue.arbitrary.length)
				{
					memcpy(pString, puData, uLen-1);
					//strncpy(pString, (const char*)puData, uLen -1);
					pString[uLen] = '\0';
				}
			}
		}
		else
			pString = NULL;
		break;
	case ASN_OBJECTIDENTIFIER:
		if( sAny->asnValue.object.idLength )
		{
			pString = (char *) SnmpUtilMemAlloc( sAny->asnValue.object.idLength * 5 );
			if(pString)
			{
				UINT i;
				char szBuf[17];
				for( i = 0; i < sAny->asnValue.object.idLength; i++ )
				{
					lstrcatA( pString, "." );
					lstrcatA( pString, itoa( sAny->asnValue.object.ids[i], szBuf, 10 ) );
				}
				uLen = strlen(pString) + 1;
			}
		}
		else
			pString = NULL;
		break;
	default:             /* Unrecognised data type */
		return( FALSE );
	}
	return(pString);
}

char * CSnmpOP::SNMP_AnyToStr(AsnObjectSyntax *sAny, UINT& uLen)
{
	if (!sAny)
	{
		return NULL;
	}

	DWORD dwValue = 0;
	//UINT uLen = 0;
	BYTE *puData = 0;
	char *pString = NULL;

	switch ( sAny->asnType )
	{
	case ASN_INTEGER:    
		pString = (char *) SnmpUtilMemAlloc(33);
		if(pString)
			ltoa(sAny->asnValue.number, pString, 10 );
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_COUNTER:
		dwValue = sAny->asnValue.counter;
		pString = (char *) SnmpUtilMemAlloc(33);
		if(pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_GAUGE:
		dwValue = sAny->asnValue.gauge;
		pString = (char *) SnmpUtilMemAlloc(33);
		if (pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_RFC1155_TIMETICKS:
		dwValue = sAny->asnValue.ticks;
		pString = (char *) SnmpUtilMemAlloc(33);
		if (pString)
			ultoa(dwValue, pString, 10);
		uLen = strlen(pString);
		break;
	case ASN_OCTETSTRING:   /* Same as ASN_RFC1213_DISPSTRING */
		uLen = sAny->asnValue.string.length+1;
		puData = sAny->asnValue.string.stream;
		pString = (char *) SnmpUtilMemAlloc(uLen + 1);
		if(pString)
		{
			if (sAny->asnValue.arbitrary.length)
			{
				memcpy(pString, puData, uLen-1);
				//strncpy(pString, (const char*)puData, uLen -1);
				pString[uLen] = '\0';
			}
		}
		break;
	case ASN_SEQUENCE:      /* Same as ASN_SEQUENCEOF */
		uLen = sAny->asnValue.sequence.length;
		puData = sAny->asnValue.sequence.stream;
		if(pString)
		{
			if (sAny->asnValue.arbitrary.length)
			{
				memcpy(pString, puData, uLen-1);
				//strncpy(pString, (const char*)puData, uLen -1);
				pString[uLen] = '\0';
			}
		}
		break;
	case ASN_RFC1155_IPADDRESS:
		if (sAny->asnValue.address.length )
		{
			UINT i;
			char szBuf[17];

			uLen = sAny->asnValue.address.length;
			puData = sAny->asnValue.address.stream;

			pString = (char *) SnmpUtilMemAlloc( uLen * 4 );
			if(pString)
			{
				pString[0] = '\0';

				for (i = 0; i < uLen; i++)
				{
					lstrcatA( pString, itoa( puData[i], szBuf, 10 ) );
					if( i < uLen-1 )
						lstrcatA( pString, "." );
				}
				uLen = strlen(pString) + 1;
			}
		}
		else
			pString = NULL;
		break;
	case ASN_RFC1155_OPAQUE:
		if( sAny->asnValue.arbitrary.length )
		{
			uLen = sAny->asnValue.arbitrary.length;
			puData = sAny->asnValue.arbitrary.stream;
			pString = (char *) SnmpUtilMemAlloc(uLen + 1);
			if (pString)
			{
				if (sAny->asnValue.arbitrary.length)
				{
					memcpy(pString, puData, uLen-1);
					//strncpy(pString, (const char*)puData, uLen -1);
					pString[uLen] = '\0';
				}
			}
		}
		else
			pString = NULL;
		break;
	case ASN_OBJECTIDENTIFIER:
		if( sAny->asnValue.object.idLength )
		{
			pString = (char *) SnmpUtilMemAlloc( sAny->asnValue.object.idLength * 5 );
			if(pString)
			{
				UINT i;
				char szBuf[17];
				for( i = 0; i < sAny->asnValue.object.idLength; i++ )
				{
					lstrcatA( pString, "." );
					lstrcatA( pString, itoa( sAny->asnValue.object.ids[i], szBuf, 10 ) );
				}
				uLen = strlen(pString) + 1;
			}
		}
		else
			pString = NULL;
		break;
	default:             /* Unrecognised data type */
		return( FALSE );
	}
	return(pString);
}

BOOL CSnmpOP::GetRequest(char* pszOid, int& nVal)
{
	if (!m_bIsActiveIP || !pszOid)
	{
		//theLog.Write(_T("!!CSnmpOP::GetRequest,1,u i,m_bIsActiveIP=%d,pszOid=%x"), m_bIsActiveIP, pszOid);
		return FALSE;
	}

	BOOL bRet = FALSE;
	char *asciiStr = NULL;
	nVal = 0;	//Ĭ�Ϸ���0

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetRequest,2,u i,SnmpMgrStrToOid fail,err=%u,pszOid=%s")
			, GetLastError(), pszOid);
		return FALSE;
	}

	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	

	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		// initiates the GET request
		if(!SnmpMgrRequest(m_lpMgrSession,SNMP_PDU_GET,&snmpVarList,&errorStatus,&errorIndex))
		{		
			theLog.Write(_T("!!CSnmpOP::GetRequest,7,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			nVal = atoi(asciiStr);
			bRet = TRUE;
		}
		else {
			theLog.Write(_T("!!CSnmpOP::GetRequest,9,end,SNMP_AnyToStr error=%u"), GetLastError());
		}
	} while (0);

	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	

	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}

	return bRet;
}

BOOL CSnmpOP::GetRequest(char* pszOid, unsigned int& nVal)
{
	if (!m_bIsActiveIP || !pszOid)
	{
		//theLog.Write(_T("!!CSnmpOP::GetRequest,1,c ui,m_bIsActiveIP=%d,pszOid=%x"), m_bIsActiveIP, pszOid);
		return FALSE;
	}

	BOOL bRet = FALSE;
	char *asciiStr = NULL;
	nVal = 0;	//Ĭ�Ϸ���0

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetRequest,2,c ui,SnmpMgrStrToOid fail,err=%u,pszOid=%s")
			, GetLastError(), pszOid);
		return FALSE;
	}


	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	

	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		// initiates the GET request
		if(!SnmpMgrRequest(m_lpMgrSession,SNMP_PDU_GET,&snmpVarList,&errorStatus,&errorIndex))
		{	
			theLog.Write(_T("!!CSnmpOP::GetRequest,8,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			nVal = (unsigned int)_atoi64(asciiStr);
			bRet = TRUE;
		}


	} while (0);

	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	

	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}

	return bRet;
}

BOOL CSnmpOP::GetRequest(char* pszOid, unsigned char* pszStr,int nStrBufferLen)
{
	if (!m_bIsActiveIP || !pszStr)
	{
		//theLog.Write(_T("!!CSnmpOP::GetRequest,1,c,uc,m_bIsActiveIP=%d,pszStr=%x"), m_bIsActiveIP, pszStr);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,1,begin,szIP=%s,pszOid=%s"), m_szDestIP, pszOid);
	BOOL bRet = FALSE;
	char *asciiStr = NULL;

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetRequestStr,2,c uc,end��err=%u,%s"),GetLastError(),pszOid);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,3,end"));
	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	
	//theLog.Write(_T("CSnmpOP::GetRequestStr,4,end"));
	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		if(!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpOP::GetRequestStr,5,c uc,m_lpMgrSession=%p"), m_lpMgrSession);
			SnmpUtilOidFree(&asnOid);	//add by zfq,2016-01-28
			SnmpUtilVarBindListFree(&snmpVarList);	//add by zfq,2016-01-28
			return FALSE;
		}
		if(!SnmpMgrRequest(m_lpMgrSession, SNMP_PDU_GET, &snmpVarList, &errorStatus, &errorIndex))
		{			
			theLog.Write(_T("!!CSnmpOP::GetRequest,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			if (uLen > nStrBufferLen)
			{
				uLen = nStrBufferLen - 1;	//����buffer���ȣ��ַ����ضϣ����Ҵ���ʵ�ʵõ��ĳ���
			}
			memcpy(pszStr, asciiStr, uLen);
			//strncpy((char*)pszStr, asciiStr, uLen);
			bRet = TRUE;
		}
	} while (0);
	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	
	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}
	return bRet;
}
BOOL CSnmpOP::GetHexRequest(char* pszOid, unsigned char* pszStr,int nStrBufferLen)
{
	if (!m_bIsActiveIP || !pszStr)
	{
		//theLog.Write(_T("!!CSnmpOP::GetHexRequest,1,c,uc,m_bIsActiveIP=%d,pszStr=%x"), m_bIsActiveIP, pszStr);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetHexRequest,1,begin,szIP=%s,pszOid=%s"), m_szDestIP, pszOid);
	BOOL bRet = FALSE;
	char *asciiStr = NULL;

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetHexRequest,2,c uc,end��err=%u,%s"),GetLastError(),pszOid);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetHexRequest,3,end"));
	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	
	//theLog.Write(_T("CSnmpOP::GetHexRequest,4,end"));
	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		if(!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpOP::GetHexRequest,5,c uc,m_lpMgrSession=%p"), m_lpMgrSession);
			SnmpUtilOidFree(&asnOid);	
			SnmpUtilVarBindListFree(&snmpVarList);
			return FALSE;
		}
		if(!SnmpMgrRequest(m_lpMgrSession, SNMP_PDU_GET, &snmpVarList, &errorStatus, &errorIndex))
		{			
			theLog.Write(_T("!!CSnmpOP::GetHexRequest,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToHexStr(&snmpVarList.list[0].value, uLen);
		//theLog.Write(_T("CSnmpOP::GetHexRequest,1,asciiStr=%s,uLen=%d"),asciiStr,uLen);
		if (asciiStr)
		{
			if (uLen > nStrBufferLen)
			{
				uLen = nStrBufferLen - 1;	//����buffer���ȣ��ַ����ضϣ����Ҵ���ʵ�ʵõ��ĳ���
			}
			memcpy(pszStr, asciiStr, uLen);
			//strncpy((char*)pszStr, asciiStr, uLen);
			bRet = TRUE;
		}
	} while (0);
	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	
	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}
	return bRet;
}

BOOL CSnmpOP::GetNextRequest(char* pszOid, int& nVal)
{
	if (!m_bIsActiveIP || !pszOid)
	{
		//theLog.Write(_T("!!CSnmpOP::GetNextRequest,1 c i,m_bIsActiveIP=%d,pszOid=%x"), m_bIsActiveIP, pszOid);
		return FALSE;
	}

	BOOL bRet = FALSE;
	char *asciiStr = NULL;
	nVal = 0;	//Ĭ�Ϸ���0

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetNextRequest,2 c i,SnmpMgrStrToOid fail,err=%u,pszOid=%s")
			, GetLastError(), pszOid);
		return FALSE;
	}


	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	

	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		// initiates the GET request
		if(!SnmpMgrRequest(m_lpMgrSession,SNMP_PDU_GETNEXT,&snmpVarList,&errorStatus,&errorIndex))
		{			
			theLog.Write(_T("!!CSnmpOP::GetNextRequest,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			nVal = atoi(asciiStr);
			bRet = TRUE;
		}
	} while (0);

	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	

	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}

	return bRet;
}

BOOL CSnmpOP::GetRequestStr(char* pszOid, unsigned char* pszStr, int& nStrBufferLen)
{
	if (!m_bIsActiveIP || !pszStr)
	{
		//theLog.Write(_T("!!CSnmpOP::GetRequestStr,1,m_bIsActiveIP=%d,pszStr=%x"), m_bIsActiveIP, pszStr);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,1,begin,szIP=%s,pszOid=%s"), m_szDestIP, pszOid);
	BOOL bRet = FALSE;
	char *asciiStr = NULL;

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetRequestStr,2,c c,end��err=%u,%s"),GetLastError(),pszOid);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,3,end"));
	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	
	//theLog.Write(_T("CSnmpOP::GetRequestStr,4,end"));
	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		if(!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpOP::GetRequestStr 5 m_lpMgrSession=%p"), m_lpMgrSession);
			return FALSE;
		}
		if(!SnmpMgrRequest(m_lpMgrSession, SNMP_PDU_GET, &snmpVarList, &errorStatus, &errorIndex))
		{	
			theLog.Write(_T("!!CSnmpOP::GetRequestStr,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			if (uLen > nStrBufferLen)
			{
				uLen = nStrBufferLen - 1;	//����buffer���ȣ��ַ����ضϣ����Ҵ���ʵ�ʵõ��ĳ���
			}
			nStrBufferLen = uLen;
			memcpy(pszStr, asciiStr, uLen);
			//strncpy((char*)pszStr, asciiStr, uLen);
			bRet = TRUE;
		}
	} while (0);
	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	
	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}
	return bRet;
}

BOOL CSnmpOP::GetNextRequestStr(char* pszOid, unsigned char* pszStr, int& nStrBufferLen)
{
	if (!m_bIsActiveIP || !pszStr)
	{
		//theLog.Write(_T("!!CSnmpOP::GetNextRequestStr,1,m_bIsActiveIP=%d,pszStr=%x"), m_bIsActiveIP, pszStr);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetNextRequestStr,1,begin,szIP=%s,pszOid=%s"), m_szDestIP, pszOid);
	BOOL bRet = FALSE;
	char *asciiStr = NULL;

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetNextRequestStr,2,end��err=%u,%s"),GetLastError(),pszOid);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,3,end"));
	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	
	//theLog.Write(_T("CSnmpOP::GetRequestStr,4,end"));
	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		if(!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpOP::GetNextRequestStr 5 m_lpMgrSession=%p"), m_lpMgrSession);
			return FALSE;
		}
		if(!SnmpMgrRequest(m_lpMgrSession, SNMP_PDU_GETNEXT, &snmpVarList, &errorStatus, &errorIndex))
		{			
			theLog.Write(_T("!!CSnmpOP::GetNextRequestStr,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			if (uLen > nStrBufferLen)
			{
				uLen = nStrBufferLen - 1;	//����buffer���ȣ��ַ����ضϣ����Ҵ���ʵ�ʵõ��ĳ���
			}
			nStrBufferLen = uLen;
			memcpy(pszStr, asciiStr, uLen);
			//strncpy((char*)pszStr, asciiStr, uLen);
			bRet = TRUE;
		}
	} while (0);
	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	
	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}
	return bRet;
}

BOOL CSnmpOP::GetNextRequestStrEx(char* pszOid,char* pszStr,int nStrBufferLen,char* pszOidNext,int nOidNextBufferLen)
{
	if (!m_bIsActiveIP || !pszStr || !pszOidNext)
	{
		//theLog.Write(_T("!!CSnmpOP::GetNextRequestStrEx,1,m_bIsActiveIP=%d,pszStr=%x,pszOidNext=%x"), m_bIsActiveIP, pszStr, pszOidNext);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetNextRequestStr,1,begin,szIP=%s,pszOid=%s"), m_szDestIP, pszOid);
	BOOL bRet = FALSE;
	char *asciiStr = NULL;
	char *szOidNext = NULL;

	AsnObjectIdentifier asnOid;
	if(!SnmpMgrStrToOid(pszOid, &asnOid))
	{
		theLog.Write(_T("!!CSnmpOP::GetNextRequestStrEx,2,end��err=%u,%s"),GetLastError(),pszOid);
		return FALSE;
	}

	//theLog.Write(_T("CSnmpOP::GetRequestStr,3,end"));
	AsnInteger	errorStatus=0;	// Error type that is returned if encountered
	AsnInteger	errorIndex=0;		// Works with variable above

	SnmpVarBindList snmpVarList;
	snmpVarList.list = (SnmpVarBind *)SnmpUtilMemAlloc(0x18u); 
	snmpVarList.len = 1;	
	//theLog.Write(_T("CSnmpOP::GetRequestStr,4,end"));
	// Assigning OID to variable bindings list
	SnmpUtilOidCpy(&snmpVarList.list[0].name,&asnOid);
	snmpVarList.list[0].value.asnType = ASN_NULL;

	do 
	{
		if(!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpOP::GetNextRequestStrEx 5 m_lpMgrSession=%p"), m_lpMgrSession);
			return FALSE;
		}
		if(!SnmpMgrRequest(m_lpMgrSession, SNMP_PDU_GETNEXT, &snmpVarList, &errorStatus, &errorIndex))
		{			
			theLog.Write(_T("!!CSnmpOP::GetNextRequestStrEx,9,end,err=%u"), GetLastError());
			break;
		}
		if(errorStatus > 0)
		{
			break;
		}
		UINT uLen = 0;
		asciiStr = SNMP_AnyToStr(&snmpVarList.list[0].value, uLen);
		if (asciiStr)
		{
			if (uLen > nStrBufferLen)
			{
				uLen = nStrBufferLen - 1;	//����buffer���ȣ��ַ����ضϣ����Ҵ���ʵ�ʵõ��ĳ���
			}
			nStrBufferLen = uLen;
			memcpy(pszStr, asciiStr, uLen);
			//strncpy((char*)pszStr, asciiStr, uLen);
		}

		SnmpMgrOidToStr(&snmpVarList.list[0].name,&szOidNext);
		if (szOidNext)
		{
			int nOidNextLen = strlen(szOidNext);
			if (nOidNextLen > (nOidNextBufferLen-1))
			{
				nOidNextLen = nOidNextBufferLen-1;
			}
			nOidNextBufferLen = nOidNextLen;
			memcpy(pszOidNext, szOidNext, nOidNextLen);
			//strncpy(pszOidNext, szOidNext, nOidNextLen);
			bRet = TRUE;
		}
	} while (0);
	SnmpUtilOidFree(&asnOid);
	SnmpUtilVarBindListFree(&snmpVarList);	
	if(asciiStr)
	{
		SnmpUtilMemFree(asciiStr);
	}
	if (szOidNext)
	{
		SnmpUtilMemFree(szOidNext);
	}
	return bRet;
}

//��ȡOID���һλ����
//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription.1.1
//result = 1 
int CSnmpOP::GetOidEndNumber(char* pszOid)
{
	if (!pszOid)
	{
		return 0;
	}

	int nOidLen = strlen(pszOid);
	int nDotFirstPostionReverse = nOidLen - 1;
	while(nDotFirstPostionReverse>0)
	{
		if (pszOid[nDotFirstPostionReverse] == '.')
		{
			break;
		}
		nDotFirstPostionReverse--;
	}

	int nRet = atoi(pszOid+nDotFirstPostionReverse+1);
	return nRet;
}

BOOL CSnmpOP::OidBeginWithStr(char* pszOid, char* pszStr)
{
	if (!pszOid || !pszStr)
	{
		return FALSE;
	}

	int nOidLen = strlen(pszOid);
	int nStrLen = strlen(pszStr);
	if (nStrLen > nOidLen)
	{
		return FALSE;
	}

	BOOL bFail = FALSE;
	for (int i=0; i<nStrLen; i++)
	{
		if (pszOid[i] != pszStr[i])
		{
			bFail = TRUE;
			break;
		}
	}
	return !bFail;
}

void CSnmpOP::TimeTicksToDateTimeStr(TimeTicks timeticks, char* pszDateTimeStr)
{
	//ʱ���ʽ��2016-01-13 10:28:45
	if (!pszDateTimeStr && strlen(pszDateTimeStr) < 20)
	{
		return;
	}

	sprintf(pszDateTimeStr
		, "%04d-%02d-%02d %02d:%02d:%02d"
		, timeticks&0xFF
		, timeticks&0xF00
		, timeticks&0xF000
		, timeticks&0xF0000
		, timeticks&0xF00000
		, timeticks&0xF000000
		);
}

CString CSnmpOP::GetStrByOid(CStringA szOid)
{
	unsigned char ucStr[256] = {0};
	memset(ucStr, 0x0, 256);
	GetRequest(CStringToChar(szOid), ucStr, 256);
#ifdef UNICODE
	CString szStr(CCommonFun::UTF8ToUnicode((const char *)ucStr));
#else
	CString szStr(ucStr);
#endif
	return szStr;
}

BOOL CSnmpOP::CheckIPActive()
{
	BOOL bRet = FALSE;

	HANDLE hIcmpFile = NULL;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;

	do 
	{
#ifdef  UNICODE
		ipaddr = inet_addr(CCommonFun::UnicodeToMultiByte(m_szIP).GetString());
#else
		ipaddr = inet_addr(m_szIP.GetString());
#endif //  UNICODE

		if (ipaddr == INADDR_NONE) 
		{
			theLog.Write(_T("!!CSnmpOP::IsActiveIP,1,usage: %s IP address"), m_szIP);
			break;
		}

		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE) 
		{
			theLog.Write(_T("!!CSnmpOP::IsActiveIP,2,IcmpCreateFile fail. error: %ld"), GetLastError() );
			break;
		}    

		ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
		ReplyBuffer = (VOID*) malloc(ReplySize);
		if (ReplyBuffer == NULL) 
		{
			theLog.Write(_T("!!CSnmpOP::IsActiveIP,3,Unable to allocate memory."));
			break;
		}    

		dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
		if (dwRetVal != 0) 
		{
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
			struct in_addr ReplyAddr;
			ReplyAddr.S_un.S_addr = pEchoReply->Address;
			if (pEchoReply->Status == IP_SUCCESS)
			{
				bRet = TRUE;
			}
			else
			{
				CString szIP(inet_ntoa( ReplyAddr ));
				theLog.Write(_T("CSnmpOP::IsActiveIP,4,Received from %s, dwRetVal=%ld, Status=%ld, time=%ldms")
					, szIP
					, dwRetVal
					, pEchoReply->Status
					, pEchoReply->RoundTripTime);
			}
		}
		else 
		{
			theLog.Write(_T("!!CSnmpOP::IsActiveIP,5,IcmpSendEcho failed. error: %ld, m_szIP=%s,dwRetVal=%d"), GetLastError(), m_szIP, dwRetVal);
			break;
		}
	} while (FALSE);

	if (hIcmpFile)
	{
		IcmpCloseHandle(hIcmpFile);
		hIcmpFile = NULL;
	}

	if (ReplyBuffer)
	{
		free(ReplyBuffer);
		ReplyBuffer = NULL;
	}

	m_bIsActiveIP = bRet;

	return m_bIsActiveIP;
}


/************************************************************************/
/* 
//system       OBJECT IDENTIFIER ::= { mib-2 1 }
#define SystemGroupOID TEXT(".1.3.6.1.2.1.1")	//.iso.org.dod.internet.mgmt.mib-2.system
OCTET_STRING sysDescr[OCTET_STRING_SIZE_256];			//::= { system 1 }	//0,ֻ��һ����ֵ
*/
/************************************************************************/
BOOL CSnmpOP::CheckSupportSnmp()
{
	char cOidStr[128] = {0};
	sprintf(cOidStr, "%s.3.0", DecryptOID(SystemGroupOID));		//".1.3.6.1.2.1.1.3.0"
	TimeTicks sysUpTime = 0;

	if (GetRequest(cOidStr, sysUpTime))
	{
		m_bIsSupportSnmp = TRUE;
	}
	else
	{
		theLog.Write(_T("!!CSnmpOP::CheckSupportSnmp,failed."));
		m_bIsSupportSnmp = FALSE;
	}

	return m_bIsSupportSnmp;
}

CStringA CSnmpOP::EncryptOID(IN CString szOrgString)
{
// #ifdef UNICODE
// 	return EncryptString(CCommonFun::UnicodeToMultiByte(szOrgString));
// #else
// 	return EncryptString(szOrgString);
// #endif
	return "";
}

CStringA CSnmpOP::DecryptOID(IN CString szEncryptString)
{
// #ifdef UNICODE
// 	return DecryptString(CCommonFun::UnicodeToMultiByte(szEncryptString));
// #else
// 	return DecryptString(szEncryptString);
// #endif
	return "";
}

char* CSnmpOP::CStringToChar(CStringA& szString)
{
	return (char*)szString.GetString();
}
wchar_t* CSnmpOP::CStringToChar(CStringW& szString)
{
	return (wchar_t*)szString.GetString();
}

/////////////////////////////////////////////////////////////////////////////////////
/////////	CSnmpJobMonitor  ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CSnmpJobMonitor::CSnmpJobMonitor()
{
	m_oJobMap.RemoveAll();
	m_nLastPrintCount = 0;
	m_nLastPrintCount_Color = 0;
	m_nLastPrintCount_WB = 0;
	m_nLastCopyCount = 0;
	m_nLastCopyCount_Color = 0;
	m_nLastCopyCount_WB = 0;
	m_nLastScanCount = 0;
	m_nLastScanCount_Color = 0;
	m_nLastScanCount_WB = 0;
	m_nLastFaxCount = 0;
	m_nLastFaxCount_Color = 0;
	m_nLastFaxCount_WB = 0;
	m_nA3PageCount = 0;
	m_nA4PageCount = 0;
}

CSnmpJobMonitor::~CSnmpJobMonitor()
{
	Release();
}

void CSnmpJobMonitor::Release()
{
	//���m_oJobMap
	POSITION pos = m_oJobMap.GetStartPosition();
	while(pos)
	{
		int nJobId = 0;
		PSCP_JobInfo pVal = NULL;
		m_oJobMap.GetNextAssoc(pos,nJobId,pVal);
		if (pVal)
		{
			m_oJobMap.RemoveKey(nJobId);
			delete pVal;
		}
	}

	//���m_oJobList
	while (m_oJobList.GetCount() > 0)
	{
		PSCP_JobInfo pJob = m_oJobList.RemoveTail();
		if (pJob)
		{
			delete pJob;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////	CSnmpPrinterMeterMonitor  ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CSnmpPrinterMeterMonitor::CSnmpPrinterMeterMonitor()
{
	InitOID();
	InitMeter();
}

CSnmpPrinterMeterMonitor::~CSnmpPrinterMeterMonitor()
{
	Release();
}

void CSnmpPrinterMeterMonitor::CheckMeterInfo()
{
	//theLog.Write(_T("CSnmpPrinterMeterMonitor::CheckMeterInfo,1,begin"));
	do 
	{
		if(!CheckIPActive())
		{
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,2,CheckIPActive fail,m_szIP=%s"), m_szIP);
			break;
		}

		if(!CheckSupportSnmp())
		{
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,2.2,CheckSupportSnmp fail,m_szIP=%s"), (LPCTSTR)m_szIP);
			break;
		}

		if (!m_lpMgrSession)
		{
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,3,m_lpMgrSession=%p"), m_lpMgrSession);
			break;
		}

		//modify by zxl,20160703
		//���ڳ������лص���������Ϊ������Ϊ�˴����ӡ�쳣�˷ѵ����⣬
		//��Ҫ�ϲ������ʱ�ܻ�ȡ�����µĳ�����Ϣ�����Դ˴����û��Ƶ����
		//ʵ�ʵĳ����������ŵ��ϲ�����е�ȡ�˴�������Ϣ������
#if 0	
		BOOL bGetMeterInfo = FALSE;
		PrinterMeterInfo oMeterInfo;

		{
			CCriticalSection2::Owner lock(m_cs4MeterInfo);
			bGetMeterInfo = GetMeterInfo();
			if (bGetMeterInfo)
			{
				memcpy(&oMeterInfo, &m_oMeterInfo, sizeof(PrinterMeterInfo));
			}
		}

		if (!bGetMeterInfo)
		{
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,4,GetMeterInfo fail"));
			break;
		}
		else
		{
			if (m_pCallBack)
			{
				m_pCallBack->OnMeterInfo(&oMeterInfo);
			}
			else
			{
				theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,5,m_pCallBack=%p"), m_pCallBack);
			}
		}
#else
		if (GetMeterInfo() && CheckMeterValid())
		{
			if (!m_bFirstSucc)
			{
				//��һ�γɹ��ĳ�����Ϣ�ϴ������ϣ�һ���Ǵ�ӡ����������ʱ��
				m_bFirstSucc = TRUE;
				if (m_pCallBack)
				{
					PrinterMeterInfo oMeterInfo;
					GetLastMeterInfo(&oMeterInfo);
					m_pCallBack->OnMeterInfo(&oMeterInfo);
				}
				else
				{
					theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,4,m_pCallBack=%p"), m_pCallBack);
				}
			}
		}
		else
		{
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::CheckMeterInfo,5,���γ�����Ϣʧ�ܡ�"));
		}
#endif

	} while (FALSE);
	//theLog.Write(_T("CSnmpPrinterMeterMonitor::CheckMeterInfo,6,end"));
}

BOOL CSnmpPrinterMeterMonitor::GetLastMeterInfo(PrinterMeterInfo* pInfo)
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);
	BOOL bRet = FALSE;
	if (pInfo)
	{
		if (!IsMeterValid(m_oLastMeterInfo))
		{
			//����Ϊȫ��,����ʧ��
			theLog.Write(_T("!!CSnmpPrinterMeterMonitor::GetLastMeterInfo,����Ϊȫ��,���³���ʧ��"));
		}
		else
		{
		//	theLog.Write(_T("!!CSnmpPrinterMeterMonitor::GetLastMeterInfo,���³���ok"));
			bRet = TRUE;
			memcpy(pInfo, &m_oLastMeterInfo, sizeof(PrinterMeterInfo));
		}
	}
	return bRet;
}

void CSnmpPrinterMeterMonitor::SetMeterInfo(PrtInputEntryMap& map)
{

	theLog.Write(_T("CSnmpPrinterMeterMonitor::SetMeterInfo,1.1��ʼ����"));
	CCriticalSection2::Owner lock(m_cs4MeterInfo);

	PrtInputEntryMap::iterator it;
	for (it=map.begin(); it!=map.end(); it++)
	{
		PPrtInputEntry pEntry = it->second;
		if (pEntry)
		{
			CStringA szInputName(pEntry->prtInputName);
			szInputName.Trim();
			if (szInputName.GetLength()<=0)
			{
				szInputName = pEntry->prtInputDescription;
				szInputName.Trim();
			}
			szInputName.MakeLower();

			char cDesc[50] = {0};
			if ((pEntry->prtInputMaxCapacity > 0) && (pEntry->prtInputCurrentLevel >= 0))
			{
				double fRatio = 1.0 * pEntry->prtInputCurrentLevel / pEntry->prtInputMaxCapacity;
				int nRatio = (int)(fRatio * 100);
				sprintf(cDesc, "%d%%", nRatio);
			}
			else if (pEntry->prtInputCurrentLevel == 0)
			{
				sprintf(cDesc, "%d%%", pEntry->prtInputCurrentLevel);
			}
			else if ((pEntry->prtInputCurrentLevel == -1) || (pEntry->prtInputCurrentLevel == -2))
			{
				//sprintf(cDesc, "%s", u8"δ֪");
				sprintf(cDesc, "%d", pEntry->prtInputCurrentLevel);
			}
			else if (pEntry->prtInputCurrentLevel == -3)
			{
				//sprintf(cDesc, "%s", u8"��Ϊ��");
				sprintf(cDesc, "%d", pEntry->prtInputCurrentLevel);
			}

			if (strlen(cDesc) <= 0)
			{
				theLog.Write(_T("##CSnmpPrinterMeterMonitor::SetMeterInfo,szInputName=[%s],prtInputType=%d,")
					_T("prtInputCapacityUnit=%d,prtInputMaxCapacity=%d,prtInputCurrentLevel=%d")
				, szInputName, pEntry->prtInputType, pEntry->prtInputCapacityUnit
				, pEntry->prtInputMaxCapacity, pEntry->prtInputCurrentLevel);
				continue;
			}

			if (m_eType == BT_LG || m_eType == BT_LM)
			{
				if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray)
				{
					if (szInputName.Find("1") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper1LevelPercent, cDesc);
					}
					else if (szInputName.Find("2") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper2LevelPercent, cDesc);
					}
					else if (szInputName.Find("3") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper3LevelPercent, cDesc);

					}
					else if (szInputName.Find("4") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper4LevelPercent, cDesc);
					}
				}
				else if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray)
				{
					strcpy(m_oMeterInfo.cPaperBypassLevelPercent, cDesc);
				}
			}
			else if (m_eType == BT_XP)
			{
				if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray
					|| pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray)
				{
					if (szInputName.Find("1") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper1LevelPercent, cDesc);
					}
					else if (szInputName.Find("2") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper2LevelPercent, cDesc);
					}
					else if (szInputName.Find("3") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper3LevelPercent, cDesc);

					}
					else if (szInputName.Find("4") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper4LevelPercent, cDesc);
					}
				}
				else if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::Other)
				{
					strcpy(m_oMeterInfo.cPaperBypassLevelPercent, cDesc);
				}			
			}
			else
			{
				if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray
					|| pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray)
				{
					if (szInputName.Find("1") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper1LevelPercent, cDesc);
					}
					else if (szInputName.Find("2") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper2LevelPercent, cDesc);
					}
					else if (szInputName.Find("3") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper3LevelPercent, cDesc);

					}
					else if (szInputName.Find("4") >= 0)
					{
						strcpy(m_oMeterInfo.cPaper4LevelPercent, cDesc);
					}
				}
				else if (pEntry->prtInputType == ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedManual)
				{
					strcpy(m_oMeterInfo.cPaperBypassLevelPercent, cDesc);
				}			
			}
		}
	}
}

void CSnmpPrinterMeterMonitor::SetMeterInfo(PrtMarkerSuppliesEntryMap& map)
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);

	PrtMarkerSuppliesEntryMap::iterator it;
	for (it=map.begin(); it!=map.end(); it++)
	{
		PPrtMarkerSuppliesEntry pEntry = it->second;
		if (pEntry)
		{
			CString szDesc(CCommonFun::UTF8ToUnicode((char*)pEntry->prtMarkerSuppliesDescription));
			szDesc.MakeLower();
			char cDesc[50] = {0};
			if ((pEntry->prtMarkerSuppliesMaxCapacity > 0) && (pEntry->prtMarkerSuppliesLevel >= 0))
			{
				double fRatio = 1.0 * pEntry->prtMarkerSuppliesLevel / pEntry->prtMarkerSuppliesMaxCapacity;
				int nRatio = (int)(fRatio * 100);
				sprintf(cDesc, "%d%%", nRatio);
			}
			else if (pEntry->prtMarkerSuppliesLevel == 0)
			{
				sprintf(cDesc, "%d%%", pEntry->prtMarkerSuppliesLevel);
			}
			else if ((pEntry->prtMarkerSuppliesLevel == -1) || (pEntry->prtMarkerSuppliesLevel == -2))
			{
				//sprintf(cDesc, "%s", u8"δ֪");
				sprintf(cDesc, "%d", pEntry->prtMarkerSuppliesLevel);
			}
			else if (pEntry->prtMarkerSuppliesLevel == -3)
			{
				//sprintf(cDesc, "%s", u8"��Ϊ��");
				sprintf(cDesc, "%d", pEntry->prtMarkerSuppliesLevel);
			}
			else if (pEntry->prtMarkerSuppliesSupplyUnit == ENUM_STRUCT_VALUE(PrtMarkerSuppliesSupplyUnitTC)::Percent)
			{
				if (pEntry->prtMarkerSuppliesLevel >= 0)
				{
					sprintf(cDesc, "%d%%", pEntry->prtMarkerSuppliesLevel);
				}
			}

			if (strlen(cDesc) <= 0)
			{
				theLog.Write(_T("##CSnmpPrinterMeterMonitor::SetMeterInfo,szDesc=[%s],prtMarkerSuppliesType=%d,")
					_T("prtMarkerSuppliesSupplyUnit=%d,prtMarkerSuppliesMaxCapacity=%d,prtMarkerSuppliesLevel=%d")
					, szDesc, pEntry->prtMarkerSuppliesType, pEntry->prtMarkerSuppliesSupplyUnit
					, pEntry->prtMarkerSuppliesMaxCapacity, pEntry->prtMarkerSuppliesLevel);
				continue;
			}

			if (szDesc.Find(_T("cyan")) >= 0 || szDesc.Find(_T("��ɫ")) >= 0)
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					strcpy(m_oMeterInfo.cTonerCyanLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					strcpy(m_oMeterInfo.cOPCCyanLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					strcpy(m_oMeterInfo.cDeveloperCyanLevelPercent, cDesc);
				}
			}
			else if (szDesc.Find(_T("magenta")) >= 0 || szDesc.Find(_T("Ʒ��ɫ")) >= 0)
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					strcpy(m_oMeterInfo.cTonerMagentaLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					strcpy(m_oMeterInfo.cOPCMagentaLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					strcpy(m_oMeterInfo.cDeveloperMagentaLevelPercent, cDesc);
				}
			}
			else if (szDesc.Find(_T("yellow")) >= 0 || szDesc.Find(_T("��ɫ")) >= 0)
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					strcpy(m_oMeterInfo.cTonerYellowLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					strcpy(m_oMeterInfo.cOPCYellowLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					strcpy(m_oMeterInfo.cDeveloperYellowLevelPercent, cDesc);
				}
			}
			else //if (szDesc.Find(_T("black")) >= 0)	//�����֧�У����ܳ��ֺ��֣������˻�����[��ɫ̼��]��[���񲿼�]��[ά�����߰�]�ȵ�
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					strcpy(m_oMeterInfo.cTonerBlackLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					strcpy(m_oMeterInfo.cOPCBlackLevelPercent, cDesc);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					strcpy(m_oMeterInfo.cDeveloperBlackLevelPercent, cDesc);
				}
			}
		}
	}
}

void CSnmpPrinterMeterMonitor::InitMeter()
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);

	m_eType = BT_THIN_CLT;
	m_bFirstSucc = FALSE;

	memset(&m_oMeterInfo, 0x0, sizeof(PrinterMeterInfo));

	//�Ĳ�����������Ϊ������һЩ���ַ����ı��������Բ���Ϊ'\0'
	//ֽ����Ϣ
	//ī�з���Ϣ:Cyan\Magenta\Yellow\Black[��ɫ��Ʒ�졢�ơ���]
	//�й�ĵ�Ԫ:OPC
	//��Ӱ��Ԫ:Developer
	strcpy(m_oMeterInfo.cPaper1LevelPercent, "-");
	strcpy(m_oMeterInfo.cPaper2LevelPercent, "-");
	strcpy(m_oMeterInfo.cPaper3LevelPercent, "-");
	strcpy(m_oMeterInfo.cPaper4LevelPercent, "-");
	strcpy(m_oMeterInfo.cPaperBypassLevelPercent, "-");

	strcpy(m_oMeterInfo.cTonerCyanLevelPercent, "-");
	strcpy(m_oMeterInfo.cTonerMagentaLevelPercent, "-");
	strcpy(m_oMeterInfo.cTonerYellowLevelPercent, "-");
	strcpy(m_oMeterInfo.cTonerBlackLevelPercent, "-");

	strcpy(m_oMeterInfo.cOPCCyanLevelPercent, "-");
	strcpy(m_oMeterInfo.cOPCMagentaLevelPercent, "-");
	strcpy(m_oMeterInfo.cOPCYellowLevelPercent, "-");
	strcpy(m_oMeterInfo.cOPCBlackLevelPercent, "-");

	strcpy(m_oMeterInfo.cDeveloperCyanLevelPercent, "-");
	strcpy(m_oMeterInfo.cDeveloperMagentaLevelPercent, "-");
	strcpy(m_oMeterInfo.cDeveloperYellowLevelPercent, "-");
	strcpy(m_oMeterInfo.cDeveloperBlackLevelPercent, "-");

	memcpy(&m_oLastMeterInfo, &m_oMeterInfo, sizeof(PrinterMeterInfo));
}

void CSnmpPrinterMeterMonitor::SetPaperMeter(PrinterMeterInfo& oMeterInfo)
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);

	//�������Ӵ�ӡ��������������
	m_oMeterInfo.nAllTotalPage = oMeterInfo.nAllTotalPage;			//��ҳ��
	m_oMeterInfo.nAllTotalDistance = oMeterInfo.nAllTotalDistance;		//�ܳ���
	m_oMeterInfo.nA3HBTotalPage = oMeterInfo.nA3HBTotalPage;		//A3�ڰ���ҳ��
	m_oMeterInfo.nA3ColorTotalPage = oMeterInfo.nA3ColorTotalPage;		//A3��ɫ��ҳ��
	m_oMeterInfo.nA4HBTotalPage = oMeterInfo.nA4HBTotalPage;		//A4�ڰ���ҳ��
	m_oMeterInfo.nA4ColorTotalPage = oMeterInfo.nA4ColorTotalPage;		//A4��ɫ��ҳ��

	//��ӡ����
	m_oMeterInfo.nA4PrintTotalPage = oMeterInfo.nA4PrintTotalPage;		//A4��ӡ�ܼ���
	m_oMeterInfo.nA4PrintColorPage = oMeterInfo.nA4PrintColorPage;		//A4��ӡ��ɫ�ܼ���
	m_oMeterInfo.nA4PrintHBPage = oMeterInfo.nA4PrintHBPage;	    //A4��ӡ�ڰ��ܼ���
	m_oMeterInfo.nA3PrintTotalPage = oMeterInfo.nA3PrintTotalPage;		//A3��ӡ�ܼ���
	m_oMeterInfo.nA3PrintColorPage = oMeterInfo.nA3PrintColorPage;		//A3��ӡ��ɫ�ܼ���
	m_oMeterInfo.nA3PrintHBPage = oMeterInfo.nA3PrintHBPage;	    //A3��ӡ�ڰ��ܼ���
	m_oMeterInfo.nOtherPrintTotalPage = oMeterInfo.nOtherPrintTotalPage;	//����ֽ�ʹ�ӡ�ܼ���
	m_oMeterInfo.nOtherPrintColorPage = oMeterInfo.nOtherPrintColorPage;	//����ֽ�ʹ�ӡ��ɫ�ܼ���
	m_oMeterInfo.nOtherPrintHBPage = oMeterInfo.nOtherPrintHBPage;		//����ֽ�ʹ�ӡ�ڰ��ܼ���

	m_oMeterInfo.nPrintDistance = oMeterInfo.nPrintDistance;		//��ӡ����
	m_oMeterInfo.nPrintArea = oMeterInfo.nPrintArea;			//��ӡ���

	//��ӡ����
	m_oMeterInfo.nA4CopyTotalPage = oMeterInfo.nA4CopyTotalPage;		//A4��ӡ�ܼ���
	m_oMeterInfo.nA4CopyColorPage = oMeterInfo.nA4CopyColorPage;		//A4��ӡ��ɫ�ܼ���
	m_oMeterInfo.nA4CopyHBPage = oMeterInfo.nA4CopyHBPage;			//A4��ӡ�ڰ��ܼ���
	m_oMeterInfo.nA3CopyTotalPage = oMeterInfo.nA3CopyTotalPage;		//A3��ӡ�ܼ���
	m_oMeterInfo.nA3CopyColorPage = oMeterInfo.nA3CopyColorPage;		//A3��ӡ��ɫ�ܼ���
	m_oMeterInfo.nA3CopyHBPage = oMeterInfo.nA3CopyHBPage;			//A3��ӡ�ڰ��ܼ���
	m_oMeterInfo.nOtherCopyTotalPage = oMeterInfo.nOtherCopyTotalPage;	//����ֽ�͸�ӡ�ܼ���
	m_oMeterInfo.nOtherCopyColorPage = oMeterInfo.nOtherCopyColorPage;	//����ֽ�͸�ӡ��ɫ�ܼ���
	m_oMeterInfo.nOtherCopyHBPage = oMeterInfo.nOtherCopyHBPage;		//����ֽ�͸�ӡ�ڰ��ܼ���

	m_oMeterInfo.nCopyDistance = oMeterInfo.nCopyDistance;			//��ӡ����
	m_oMeterInfo.nCopyArea = oMeterInfo.nCopyArea;				//��ӡ���

	//ɨ�����
	m_oMeterInfo.nScanTotalPage = oMeterInfo.nScanTotalPage;		//ɨ���ܼ���
	m_oMeterInfo.nScanBigColorPage = oMeterInfo.nScanBigColorPage;		//ɨ������ɫ�ܼ���
	m_oMeterInfo.nScanBigHBPage = oMeterInfo.nScanBigHBPage;		//ɨ�����ڰ��ܼ���
	m_oMeterInfo.nScanSmallColorPage = oMeterInfo.nScanSmallColorPage;	//ɨ��С����ɫ�ܼ���
	m_oMeterInfo.nScanSmallHBPage = oMeterInfo.nScanSmallHBPage;		//ɨ��С���ڰ��ܼ���

	//�������
	m_oMeterInfo.nA4FaxTotalPage = oMeterInfo.nA4FaxTotalPage;		//A4�����ܼ���
	m_oMeterInfo.nA4FaxColorPage = oMeterInfo.nA4FaxColorPage;		//A4�����ɫ�ܼ���
	m_oMeterInfo.nA4FaxHBPage = oMeterInfo.nA4FaxHBPage;			//A4����ڰ��ܼ���
	m_oMeterInfo.nA3FaxTotalPage = oMeterInfo.nA3FaxTotalPage;		//A3�����ܼ���
	m_oMeterInfo.nA3FaxColorPage = oMeterInfo.nA3FaxColorPage;		//A3�����ɫ�ܼ���
	m_oMeterInfo.nA3FaxHBPage = oMeterInfo.nA3FaxHBPage;			//A3����ڰ��ܼ���
	m_oMeterInfo.nOtherFaxTotalPage = oMeterInfo.nOtherFaxTotalPage;	//����ֽ�ʹ����ܼ���
	m_oMeterInfo.nOtherFaxColorPage = oMeterInfo.nOtherFaxColorPage;	//����ֽ�ʹ����ɫ�ܼ���
	m_oMeterInfo.nOtherFaxHBPage = oMeterInfo.nOtherFaxHBPage;		//����ֽ�ʹ���ڰ��ܼ���

	//������������
	m_oMeterInfo.nOtherOpTotalPage = oMeterInfo.nOtherOpTotalPage;		//���������ܼ���
	m_oMeterInfo.nOtherOpColorPage = oMeterInfo.nOtherOpColorPage;		//����������ɫ�ܼ���
	m_oMeterInfo.nOtherOpHBPage = oMeterInfo.nOtherOpHBPage;	    //���������ڰ��ܼ���
	m_oMeterInfo.nEquivalentHBPrintPage = oMeterInfo.nEquivalentHBPrintPage; //��Ч�ڰ״�ӡ����
	m_oMeterInfo.nEquivalentHBCopyPage = oMeterInfo.nEquivalentHBCopyPage;	//��Ч�ڰ׸�ӡ����
	m_oMeterInfo.nEquivalentHBFaxPage = oMeterInfo.nEquivalentHBFaxPage;	//��Ч�ڰ״������
	m_oMeterInfo.nEquivalentColorPrintPage = oMeterInfo.nEquivalentColorPrintPage; //��Ч��ɫ��ӡ����
	m_oMeterInfo.nEquivalentColorCopyPage = oMeterInfo.nEquivalentColorCopyPage;	//��Ч��ɫ��ӡ����
	m_oMeterInfo.nEquivalentColorFaxPage = oMeterInfo.nEquivalentColorFaxPage;	//��Ч��ɫ�������

	//�Ĳ����������ܱ����ǣ���Ϊ������һЩ���ַ����ı���������Ϊ'\0'
	//ֽ����Ϣ
	//ī�з���Ϣ:Cyan\Magenta\Yellow\Black[��ɫ��Ʒ�졢�ơ���]
	//�й�ĵ�Ԫ:OPC
	//��Ӱ��Ԫ:Developer
}

BOOL CSnmpPrinterMeterMonitor::CheckMeterValid()
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);
	if (IsMeterValid(m_oMeterInfo))
	{
		memcpy(&m_oLastMeterInfo, &m_oMeterInfo, sizeof(PrinterMeterInfo));
		return TRUE;
	}

	return FALSE;
}

BOOL CSnmpPrinterMeterMonitor::IsMeterValid(PrinterMeterInfo& oMeterInfo)
{
	if (
		oMeterInfo.nAllTotalPage <= 0				//��ҳ��
		&& oMeterInfo.nAllTotalDistance <= 0		//�ܳ���
		&& oMeterInfo.nA3HBTotalPage <= 0			//A3�ڰ���ҳ��
		&& oMeterInfo.nA3ColorTotalPage <= 0		//A3��ɫ��ҳ��
		&& oMeterInfo.nA4HBTotalPage <= 0			//A4�ڰ���ҳ��
		&& oMeterInfo.nA4ColorTotalPage <= 0		//A4��ɫ��ҳ��

		//��ӡ����
		&& oMeterInfo.nA4PrintTotalPage <= 0		//A4��ӡ�ܼ���
		&& oMeterInfo.nA4PrintColorPage <= 0		//A4��ӡ��ɫ�ܼ���
		&& oMeterInfo.nA4PrintHBPage <= 0			//A4��ӡ�ڰ��ܼ���
		&& oMeterInfo.nA3PrintTotalPage <= 0		//A3��ӡ�ܼ���
		&& oMeterInfo.nA3PrintColorPage <= 0		//A3��ӡ��ɫ�ܼ���
		&& oMeterInfo.nA3PrintHBPage <= 0			//A3��ӡ�ڰ��ܼ���
		&& oMeterInfo.nOtherPrintTotalPage <= 0		//����ֽ�ʹ�ӡ�ܼ���
		&& oMeterInfo.nOtherPrintColorPage <= 0		//����ֽ�ʹ�ӡ��ɫ�ܼ���
		&& oMeterInfo.nOtherPrintHBPage <= 0		//����ֽ�ʹ�ӡ�ڰ��ܼ���

		&& oMeterInfo.nPrintDistance <= 0			//��ӡ����
		&& oMeterInfo.nPrintArea <= 0				//��ӡ���

		//��ӡ����
		&& oMeterInfo.nA4CopyTotalPage <= 0			//A4��ӡ�ܼ���
		&& oMeterInfo.nA4CopyColorPage <= 0			//A4��ӡ��ɫ�ܼ���
		&& oMeterInfo.nA4CopyHBPage <= 0			//A4��ӡ�ڰ��ܼ���
		&& oMeterInfo.nA3CopyTotalPage <= 0			//A3��ӡ�ܼ���
		&& oMeterInfo.nA3CopyColorPage <= 0			//A3��ӡ��ɫ�ܼ���
		&& oMeterInfo.nA3CopyHBPage <= 0			//A3��ӡ�ڰ��ܼ���
		&& oMeterInfo.nOtherCopyTotalPage <= 0		//����ֽ�͸�ӡ�ܼ���
		&& oMeterInfo.nOtherCopyColorPage <= 0		//����ֽ�͸�ӡ��ɫ�ܼ���
		&& oMeterInfo.nOtherCopyHBPage <= 0			//����ֽ�͸�ӡ�ڰ��ܼ���

		&& oMeterInfo.nCopyDistance <= 0			//��ӡ����
		&& oMeterInfo.nCopyArea <= 0				//��ӡ���


		//ɨ�����
		&& oMeterInfo.nScanTotalPage <= 0			//ɨ���ܼ���
		&& oMeterInfo.nScanBigColorPage <= 0		//ɨ������ɫ�ܼ���
		&& oMeterInfo.nScanBigHBPage <= 0			//ɨ�����ڰ��ܼ���
		&& oMeterInfo.nScanSmallColorPage <= 0		//ɨ��С����ɫ�ܼ���
		&& oMeterInfo.nScanSmallHBPage <= 0			//ɨ��С���ڰ��ܼ���

		//�������
		&& oMeterInfo.nA4FaxTotalPage <= 0			//A4�����ܼ���
		&& oMeterInfo.nA4FaxColorPage <= 0			//A4�����ɫ�ܼ���
		&& oMeterInfo.nA4FaxHBPage <= 0				//A4����ڰ��ܼ���
		&& oMeterInfo.nA3FaxTotalPage <= 0			//A3�����ܼ���
		&& oMeterInfo.nA3FaxColorPage <= 0			//A3�����ɫ�ܼ���
		&& oMeterInfo.nA3FaxHBPage <= 0				//A3����ڰ��ܼ���
		&& oMeterInfo.nOtherFaxTotalPage <= 0		//����ֽ�ʹ����ܼ���
		&& oMeterInfo.nOtherFaxColorPage <= 0		//����ֽ�ʹ����ɫ�ܼ���
		&& oMeterInfo.nOtherFaxHBPage <= 0			//����ֽ�ʹ���ڰ��ܼ���

		//������������
		&& oMeterInfo.nOtherOpTotalPage <= 0		//���������ܼ���
		&& oMeterInfo.nOtherOpColorPage <= 0		//����������ɫ�ܼ���
		&& oMeterInfo.nOtherOpHBPage <= 0			//���������ڰ��ܼ���
		)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CSnmpPrinterMeterMonitor::Release()
{
	CCriticalSection2::Owner lock(m_cs4MeterInfo);
	m_eType = BT_THIN_CLT;
	m_bFirstSucc = FALSE;
	memset(&m_oMeterInfo, 0x0, sizeof(PrinterMeterInfo));
}

void CSnmpPrinterMeterMonitor::SetBrandType(BRAND_TYPE eType)
{
	m_eType = eType;
	theLog.Write(_T("CSnmpPrinterMeterMonitor::SetBrandType,m_eType=%d"), m_eType);
}

void CSnmpPrinterMeterMonitor::InitOID()
{
	m_szPrtMarkerLifeCountOID.Format("%s.1.1", DecryptOID(PrtMarkerLifeCountOID));
}

//������ֻ����CheckMeterInfoֻ�ڲ�����
BOOL CSnmpPrinterMeterMonitor::GetMeterInfo()
{
	//theLog.Write(_T("CSnmpPrinterMeterMonitor::GetMeterInfo,1,begin"));

	PrinterMeterInfo oMeterInfoTmp;
	memset(&oMeterInfoTmp, 0, sizeof(oMeterInfoTmp));

	int nValue = 0;
	//��ֽ��ҳ��
	if (!GetRequest(CStringToChar(m_szPrtMarkerLifeCountOID), nValue))	//��ֽ��ҳ��=printmib.prtMarker.prtMarkerTable.prtMarkerEntry.prtMarkerLifeCount.1.1
	{
		theLog.Write(_T("!!CSnmpPrinterMeterMonitor::GetMeterInfo,2,��ȡ��ֽ��ҳ��ʧ��"));
		return FALSE;	//����ʧ���ˣ�������ڵ�ľͲ�Ҫ���������ˡ�
	}
	else
	{
		//oMeterInfoTmp.nOtherPrintTotalPage = nValue;
		oMeterInfoTmp.nAllTotalPage = (nValue > 0) ? nValue : 0;
		//theLog.Write(_T("CSnmpPrinterMeterMonitor::GetMeterInfo,3,��ȡ��ֽ��ҳ���ɹ���nValue=%d"), nValue);
	}

	SetPaperMeter(oMeterInfoTmp);	//����ʱ������Ϣ���浽�ڲ�

	//theLog.Write(_T("CSnmpPrinterMeterMonitor::GetMeterInfo,100,end"));
	return TRUE;
}


/************************************************************************/
/*     ��ӡ��״̬������                                                 */
/************************************************************************/
//
CSnmpMonitorHelper::CSnmpMonitorHelper()
{
	Init();
}

CSnmpMonitorHelper::~CSnmpMonitorHelper()
{
	Release();
}

void CSnmpMonitorHelper::SetBrandType(BRAND_TYPE eType)
{
	m_eType = eType;
	theLog.Write(_T("CSnmpMonitorHelper::SetBrandType,m_eType=%d"), m_eType);
}

CString CSnmpMonitorHelper::GetPaperTypeByPrtInputEntry(PPrtInputEntry pEntry)
{
	int nWidthTenthsOfMillimeter = 0;	//ʮ��֮һ����
	int nHeightTenthsOfMillimeter = 0;	//ʮ��֮һ����
	if (pEntry)
	{
		if (pEntry->prtInputDimUnit == 	ENUM_STRUCT_VALUE(PrtMediaUnitTC)::Micrometers)
		{
			//297000*210000
			nWidthTenthsOfMillimeter = MulDiv(pEntry->prtInputMediaDimFeedDirDeclared, 1, 100);
			nHeightTenthsOfMillimeter = MulDiv(pEntry->prtInputMediaDimXFeedDirDeclared, 1, 100);
		}
		else if (pEntry->prtInputDimUnit == ENUM_STRUCT_VALUE(PrtMediaUnitTC)::TenThousandthsOfInches)
		{
			//1Ӣ��=2.54����
			//116929*82677
			nWidthTenthsOfMillimeter = MulDiv(pEntry->prtInputMediaDimFeedDirDeclared*2.54, 1, 100);
			nHeightTenthsOfMillimeter = MulDiv(pEntry->prtInputMediaDimXFeedDirDeclared*2.54, 1, 100);
		}	
	}

	return CCommonFun::GetPagerSize3(nWidthTenthsOfMillimeter, nHeightTenthsOfMillimeter);
}

BOOL CSnmpMonitorHelper::IsFalutInfo(CString szDetial)
{
	BOOL bYes = FALSE;
	szDetial.Trim();
	if (m_eType == BT_CANON)
	{//����
		//A service call error has occurred.(0407)
		if (szDetial.Find(_T("A service call error has occurred.")) >= 0)
		{
			bYes = TRUE;
		}
	}
	else if (m_eType == BT_KM || m_eType == BT_ZD)
	{//����,�𵩺Ϳ�����һ����
		//Service Call C5351
		//Service Call C2021 (Fatal Error)
		if (szDetial.Find(_T("Service Call")) >= 0)
		{
			bYes = TRUE;
		}
	}
	else if (m_eType == BT_XP)
	{//����
		//Service Error F2-73
		//Service Error TA
		if (szDetial.Find(_T("Service Error")) >= 0)
		{
			bYes = TRUE;
		}
	}
#if 0	//�������������Ԥ������S2-3319�����ʽ
	else if (m_eType == BT_SX)
	{//����
		//S2-3319 The Machine is currently running on auto color registration mode.
		if (TRUE)
		{
			bYes = TRUE;
		}
	}
#endif
	return bYes;
}

CString CSnmpMonitorHelper::GetFaultCode(CString szDetial)
{
	CString szFaultCode = _T("");
	szDetial.Trim();
	if (m_eType == BT_CANON)
	{//����
		//A service call error has occurred.(0407)
		if (szDetial.Find(_T("A service call error has occurred.")) >= 0)
		{
			int nLeft = szDetial.Find(_T("("));
			int nRight = szDetial.Find(_T(")"));
			szFaultCode = szDetial.Mid(nLeft+1, nRight-nLeft-1);
		}
	}
	else if (m_eType == BT_KM || m_eType == BT_ZD)
	{//����,�𵩺Ϳ�����һ����
		//Service Call C5351
		//Service Call C2021 (Fatal Error)
		if (szDetial.Find(_T("Service Call")) >= 0)
		{
			CStringArray ary;
			CCommonFun::StringSplit(szDetial, &ary, _T(" "), TRUE);
			if (ary.GetCount() >= 3)
			{
				szFaultCode = ary.GetAt(2);
			}
		}
	}
	else if (m_eType == BT_XP)
	{//����
		//Service Error F2-73
		//Service Error TA
		if (szDetial.Find(_T("Service Error")) >= 0)
		{
			CStringArray ary;
			CCommonFun::StringSplit(szDetial, &ary, _T(" "), TRUE);
			if (ary.GetCount() >= 3)
			{
				szFaultCode = ary.GetAt(2);
			}
		}
	}
#if 0	//�������������Ԥ������S2-3319�����ʽ
	else if (m_eType == BT_SX)
	{//����
		//S2-3319 The Machine is currently running on auto color registration mode.
		if (TRUE)
		{
			CStringArray ary;
			CCommonFun::StringSplit(szDetial, &ary, _T(" "), TRUE);
			if (ary.GetCount() >= 1)
			{
				szFaultCode = ary.GetAt(0);
			}
		}
	}
#endif
	return szFaultCode;
}

void CSnmpMonitorHelper::CheckFaultCode()
{
	//�˽ӿ�Ҫ������ʵ��
}

void CSnmpMonitorHelper::Init()
{
	Release();
	InitOID();
}

void CSnmpMonitorHelper::Release()
{
	m_eType = BT_THIN_CLT;
	m_pPrinterMeterMonitor = NULL;
	ClearAll();
}

void CSnmpMonitorHelper::SetPrinterMeterMonitor(CSnmpPrinterMeterMonitor* pMeter)
{
	m_pPrinterMeterMonitor = pMeter;
	theLog.Write(_T("CSnmpMonitorHelper::SetPrinterMeterMonitor, m_pPrinterMeterMonitor=%p"), m_pPrinterMeterMonitor);
}

void CSnmpMonitorHelper::SetPaperConfig(CString sConfig, int nMin)
{
	m_szPaperConfig = sConfig;
	m_nPaperMinRatio = nMin;
	theLog.Write(_T("CSnmpMonitorHelper::SetPaperConfig, m_szPaperConfig=%s,m_nPaperMinRatio=%d")
		, m_szPaperConfig, m_nPaperMinRatio);
}

void CSnmpMonitorHelper::SetInkConfig(CString sConfig, int nMin)
{
	m_szInkConfig = sConfig;
	m_nInkMinRatio = nMin;
	theLog.Write(_T("CSnmpMonitorHelper::SetInkConfig, m_szInkConfig=%s,m_nInkMinRatio=%d")
		, m_szInkConfig, m_nInkMinRatio);
}

void CSnmpMonitorHelper::CheckMonitor()
{
	if (CheckIPActive() && CheckSupportSnmp())
	{
		ClearAll();
		GetAll();
		CheckAll();
	}
}

//ͨ��SNMP��ȡOID��Ϣ
BOOL CSnmpMonitorHelper::GetAll()
{
	GetGeneral();
	GetStorageRef();
	GetDeviceRef();
	GetCover();
	GetLocalization();
	GetInput();
	GetOutput();
	GetMarker();
	GetMarkerSupplies();
	GetMarkerColorant();
	GetMarkerPath();
	GetChannel();
	GetInterpreter();
	GetConsoleDisplayBuffer();
	GetConsoleLight();
	GetAlert();
	return TRUE;
}

BOOL CSnmpMonitorHelper::GetGeneral()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetStorageRef()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetDeviceRef()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetCover()
{
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverIndex
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription
	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverStatus

	//CCriticalSection2::Owner lock(m_cs4CoverMap);

	char *cOidBegin = CStringToChar(m_szPrtCoverDescriptionOID);	//"printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription"
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
			if (m_oCoverMap.find(nIndex) == m_oCoverMap.end())
			{
				PPrtCoverEntry pEntry = new PrtCoverEntry;
				memset(pEntry, 0x0, sizeof(PrtCoverEntry));
				m_oCoverMap.insert(pair<int,PPrtCoverEntry>(nIndex, pEntry));
				pEntry->prtCoverIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	PrtCoverEntryMap::iterator it;
	for (it=m_oCoverMap.begin(); it!=m_oCoverMap.end(); it++)
	{
		PPrtCoverEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->prtCoverIndex;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.1.%d", m_szPrtCoverDescriptionOID, nIndex);	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverDescription
			GetRequest(cOidStr, pEntry->prtCoverDescription, sizeof(pEntry->prtCoverDescription));

			sprintf(cOidStr, "%s.1.%d", m_szPrtCoverStatusOID, nIndex);	//printmib.prtCover.prtCoverTable.prtCoverEntry.prtCoverStatus
			GetRequest(cOidStr, (int&)pEntry->prtCoverStatus);
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelper::GetLocalization()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetInput()
{
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputIndex 
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputType
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputDimUnit
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimFeedDirDeclared
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimXFeedDirDeclared
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimFeedDirChosen
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimXFeedDirChosen
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputCapacityUnit
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMaxCapacity
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputCurrentLevel
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputStatus
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaName
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputName
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputVendorName
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputModel
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputVersion
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputSerialNumber
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputDescription
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputSecurity
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaWeight
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaType
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaColor
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaFormParts
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaLoadTimeout
	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputNextIndex

	//CCriticalSection2::Owner lock(m_cs4InputMap);

	char *cOidBegin = CStringToChar(m_szPrtInputTypeOID);	//"printmib.prtInput.prtInputTable.prtInputEntry.prtInputType"
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
			if (m_oInputMap.find(nIndex) == m_oInputMap.end())
			{
				PPrtInputEntry pEntry = new PrtInputEntry;
				memset(pEntry, 0x0, sizeof(PrtInputEntry));
				m_oInputMap.insert(pair<int,PPrtInputEntry>(nIndex, pEntry));
				pEntry->prtInputIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	PrtInputEntryMap::iterator it;
	for (it=m_oInputMap.begin(); it!=m_oInputMap.end(); it++)
	{
		PPrtInputEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->prtInputIndex;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.1.%d", m_szPrtInputTypeOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputType
			GetRequest(cOidStr, (int&)pEntry->prtInputType);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputDimUnitOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputDimUnit
			GetRequest(cOidStr, (int&)pEntry->prtInputDimUnit);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaDimFeedDirDeclaredOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimFeedDirDeclared
			GetRequest(cOidStr, (int&)pEntry->prtInputMediaDimFeedDirDeclared);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaDimXFeedDirDeclaredOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimXFeedDirDeclared
			GetRequest(cOidStr, pEntry->prtInputMediaDimXFeedDirDeclared);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaDimFeedDirChosenOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimFeedDirChosen
			GetRequest(cOidStr, pEntry->prtInputMediaDimFeedDirChosen);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaDimXFeedDirChosenOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaDimXFeedDirChosen
			GetRequest(cOidStr, pEntry->prtInputMediaDimXFeedDirChosen);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputCapacityUnitOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputCapacityUnit
			GetRequest(cOidStr, (int&)pEntry->prtInputCapacityUnit);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMaxCapacityOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMaxCapacity
			GetRequest(cOidStr, pEntry->prtInputMaxCapacity);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputCurrentLevelOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputCurrentLevel
			GetRequest(cOidStr, pEntry->prtInputCurrentLevel);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputStatusOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputStatus
			GetRequest(cOidStr, pEntry->prtInputStatus);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaNameOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaName
			GetRequest(cOidStr, pEntry->prtInputMediaName, sizeof(pEntry->prtInputMediaName));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputNameOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputName
			GetRequest(cOidStr, pEntry->prtInputName, sizeof(pEntry->prtInputName));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputVendorNameOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputVendorName
			GetRequest(cOidStr, pEntry->prtInputVendorName, sizeof(pEntry->prtInputVendorName));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputModelOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputModel
			GetRequest(cOidStr, pEntry->prtInputModel, sizeof(pEntry->prtInputModel));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputVersionOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputVersion
			GetRequest(cOidStr, pEntry->prtInputVersion, sizeof(pEntry->prtInputVersion));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputSerialNumberOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputSerialNumber
			GetRequest(cOidStr, pEntry->prtInputSerialNumber, sizeof(pEntry->prtInputSerialNumber));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputDescriptionOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputDescription
			GetRequest(cOidStr, pEntry->prtInputDescription, sizeof(pEntry->prtInputDescription));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputSecurityOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputSecurity
			GetRequest(cOidStr, (int&)pEntry->prtInputSecurity);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaWeightOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaWeight
			GetRequest(cOidStr, pEntry->prtInputMediaWeight);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaTypeOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaType
			GetRequest(cOidStr, pEntry->prtInputMediaType, sizeof(pEntry->prtInputMediaType));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaColorOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaColor
			GetRequest(cOidStr, pEntry->prtInputMediaColor, sizeof(pEntry->prtInputMediaColor));

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaFormPartsOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaFormParts
			GetRequest(cOidStr, pEntry->prtInputMediaFormParts);

#ifdef RFC_3805
			sprintf(cOidStr, "%s.1.%d", m_szPrtInputMediaLoadTimeoutOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputMediaLoadTimeout
			GetRequest(cOidStr, pEntry->prtInputMediaLoadTimeout);

			sprintf(cOidStr, "%s.1.%d", m_szPrtInputNextIndexOID, nIndex);	//printmib.prtInput.prtInputTable.prtInputEntry.prtInputNextIndex
			GetRequest(cOidStr, pEntry->prtInputNextIndex);
#endif
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelper::GetOutput()
{
	return FALSE;

}

BOOL CSnmpMonitorHelper::GetMarker()
{
	return FALSE;

}

BOOL CSnmpMonitorHelper::GetMarkerSupplies()
{
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesIndex
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesMarkerIndex
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesColorantIndex
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesClass
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesType
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesDescription
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesSupplyUnit
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesMaxCapacity
	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesLevel

	//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);

	char *cOidBegin = CStringToChar(m_szPrtMarkerSuppliesMarkerIndexOID);	//"printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesMarkerIndex"
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
			int nIndex = pEntry->prtMarkerSuppliesIndex;

			char cOidStr[128] = {0};
			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesMarkerIndexOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesMarkerIndex
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesMarkerIndex);

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesColorantIndexOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesColorantIndex
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesColorantIndex);

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesClassOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesClass
			GetRequest(cOidStr, (int&)pEntry->prtMarkerSuppliesClass);

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesTypeOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesType
			GetRequest(cOidStr, (int&)pEntry->prtMarkerSuppliesType);

			//�����ī�ۣ����prtMarkerColorant�л�ȡ��ɫ��Ϣ
			if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
				|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
				|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
				|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
				|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
			{
				sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerColorantValueOID, pEntry->prtMarkerSuppliesColorantIndex);	//printmib.prtMarkerColorant.prtMarkerColorantTable.prtMarkerColorantEntry.prtMarkerColorantValue
				GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription, sizeof(pEntry->prtMarkerSuppliesDescription));
			}
			else
			{
				sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesDescriptionOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesDescription
				GetRequest(cOidStr, pEntry->prtMarkerSuppliesDescription, sizeof(pEntry->prtMarkerSuppliesDescription));
			}

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesSupplyUnitOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesSupplyUnit
			GetRequest(cOidStr, (int&)pEntry->prtMarkerSuppliesSupplyUnit);

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesMaxCapacityOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesMaxCapacity
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesMaxCapacity);

			sprintf(cOidStr, "%s.1.%d", m_szPrtMarkerSuppliesLevelOID, nIndex);	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry.prtMarkerSuppliesLevel
			GetRequest(cOidStr, pEntry->prtMarkerSuppliesLevel);
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelper::GetMarkerColorant()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetMarkerPath()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetChannel()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetInterpreter()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetConsoleDisplayBuffer()
{
	//"printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry.prtConsoleDisplayBufferIndex"
	//"printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry.prtConsoleDisplayBufferText"

	//CCriticalSection2::Owner lock(m_cs4ConsoleDisplayBufferMap);

	char *cOidBegin = CStringToChar(m_szPrtConsoleDisplayBufferTextOID);	//"printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry.prtConsoleDisplayBufferText"
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
			if (m_oConsoleDisplayBufferMap.find(nIndex) == m_oConsoleDisplayBufferMap.end())
			{
				PPrtConsoleDisplayBufferEntry pEntry = new PrtConsoleDisplayBufferEntry;
				memset(pEntry, 0x0, sizeof(PrtConsoleDisplayBufferEntry));
				m_oConsoleDisplayBufferMap.insert(pair<int,PPrtConsoleDisplayBufferEntry>(nIndex, pEntry));
				pEntry->prtConsoleDisplayBufferIndex = nIndex;
			}
		}
		else
		{
			break;
		}
	}

	PrtConsoleDisplayBufferEntryMap::iterator it;
	for (it=m_oConsoleDisplayBufferMap.begin(); it!=m_oConsoleDisplayBufferMap.end(); it++)
	{
		PPrtConsoleDisplayBufferEntry pEntry = it->second;
		if (pEntry)
		{
			int nIndex = pEntry->prtConsoleDisplayBufferIndex;

			char cOidStr[128] = {0};
			//sprintf(cOidStr, "%s.1.%d", m_szPrtConsoleDisplayBufferIndexOID, nIndex);	//"printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry.prtConsoleDisplayBufferIndex"
			//GetRequest(cOidStr, pEntry->prtConsoleDisplayBufferIndex);

			sprintf(cOidStr, "%s.1.%d", m_szPrtConsoleDisplayBufferTextOID, nIndex);	//"printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry.prtConsoleDisplayBufferText"
			GetRequest(cOidStr, pEntry->prtConsoleDisplayBufferText, sizeof(pEntry->prtConsoleDisplayBufferText));
		}
	}

	return TRUE;
}

BOOL CSnmpMonitorHelper::GetConsoleLight()
{
	return FALSE;
}

BOOL CSnmpMonitorHelper::GetAlert()
{
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertIndex
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertSeverityLevel
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertTrainingLevel
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertGroup
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertGroupIndex
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertLocation
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertCode
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertDescription
	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertTime

	//CCriticalSection2::Owner lock(m_cs4AlertMap);

	char *cOidBegin = CStringToChar(m_szPrtAlertSeverityLevelOID);	//"printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertSeverityLevel"
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
			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertSeverityLevelOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertSeverityLevel
			GetRequest(cOidStr, (int&)pEntry->prtAlertSeverityLevel);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertTrainingLevelOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertTrainingLevel
			GetRequest(cOidStr, (int&)pEntry->prtAlertTrainingLevel);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertGroupOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertGroup
			GetRequest(cOidStr, (int&)pEntry->prtAlertGroup);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertGroupIndexOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertGroupIndex
			GetRequest(cOidStr, pEntry->prtAlertGroupIndex);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertLocationOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertLocation
			GetRequest(cOidStr, pEntry->prtAlertLocation);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertCodeOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertCode
			GetRequest(cOidStr, (int&)pEntry->prtAlertCode);

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertDescriptionOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertDescription
			GetRequest(cOidStr, pEntry->prtAlertDescription, sizeof(pEntry->prtAlertDescription));

			sprintf(cOidStr, "%s.1.%d", m_szPrtAlertTimeOID, nIndex);	//printmib.prtAlert.prtAlertTable.prtAlertEntry.prtAlertTime
			GetRequest(cOidStr, pEntry->prtAlertTime);
		}
	}

	return TRUE;
}

//���SNMP��ȡ����OID��Ϣ
void CSnmpMonitorHelper::CheckAll()
{
	theLog.Write(_T("CSnmpMonitor::CheckAll"));
	CheckGeneral();
	CheckStorageRef();
	CheckDeviceRef();
	CheckCover();
	CheckLocalization();
	CheckInput();
	CheckOutput();
	CheckMarker();
	CheckMarkerSupplies();
	CheckMarkerColorant();
	CheckMarkerPath();
	CheckChannel();
	CheckInterpreter();
//	CheckConsoleDisplayBuffer();
	CheckConsoleLight();
	CheckAlert();

	//��չ���ܼ��
	CheckMeter();	//����
	CheckAlertEx();	//Ԥ����չ
	CheckFaultCode();	//�����Ĺ��ϴ����⣬�Ӵ�ӡ����˽�нڵ��ȡ
}

void CSnmpMonitorHelper::CheckGeneral()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckStorageRef()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckDeviceRef()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckCover()
{
	//CCriticalSection2::Owner lock(m_cs4CoverMap);

	PrtCoverEntryMap::iterator it;
	for (it=m_oCoverMap.begin(); it!=m_oCoverMap.end(); it++)
	{
		PPrtCoverEntry pEntry = it->second;
		if (pEntry)
		{
			int nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::UnKnown;
#ifdef  UNICODE
			CString szCoverDescription = CCommonFun::UTF8ToUnicode((char*)pEntry->prtCoverDescription);
#else
			CString szCoverDescription = CCommonFun::UTF8ToMultiByte((char*)pEntry->prtCoverDescription);
#endif //  UNICODE
			CString szDesc = _T("");
			if (pEntry->prtCoverStatus == ENUM_STRUCT_VALUE(PrtCoverStatusTC)::CoverOpen)
			{
				nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::CoverOpen;
				//szDesc.Format(_T("�ǰ�(%s)���򿪣�"), szCoverDescription);
				szDesc.Format(_T("AlarmCoverOpen|%s|�ǰ�(%s)���򿪣�"), szCoverDescription, szCoverDescription);
			}
			else if (pEntry->prtCoverStatus == ENUM_STRUCT_VALUE(PrtCoverStatusTC)::InterlockOpen)
			{
				nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::InterlockOpen;
				//szDesc.Format(_T("����װ��(%s)���򿪣�"), szCoverDescription);
				szDesc.Format(_T("AlarmInterlockOpen|%s|����װ��(%s)���򿪣�"), szCoverDescription, szCoverDescription);
			}
			else if (pEntry->prtCoverStatus == ENUM_STRUCT_VALUE(PrtCoverStatusTC)::Other)
			{
				nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::Other;
				//szDesc.Format(_T("�ǰ�(%s)״̬�쳣��"), szCoverDescription);
				szDesc.Format(_T("AlarmCoverAbnormal|%s|�ǰ�(%s)״̬�쳣��"), szCoverDescription, szCoverDescription);
			}

			if (szDesc.GetLength() > 0 && m_pCallBack)
			{
				if (nAlertCode < 100)
				{
					nAlertCode += 100;	//�ο�:��PrinterAlert_Def.h���еġ�PrinterAlertCode���Ű�����չ����Ԥ��
				}
				m_pCallBack->OnStatus(nAlertCode, szDesc.GetBuffer());
			}
		}
	}
}

void CSnmpMonitorHelper::CheckLocalization()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckInput()
{
	//CCriticalSection2::Owner lock(m_cs4InputMap);

	PrtInputEntryMap::iterator it;
	for (it=m_oInputMap.begin(); it!=m_oInputMap.end(); it++)
	{
		PPrtInputEntry pEntry = it->second;
		if (pEntry)
		{
			//theLog.Write(_T("====CSnmpMonitorHelper::CheckInput,m_eType=%d,prtInputType=%d,prtInputMaxCapacity=%d,prtInputCurrentLevel=%d, prtInputName=%s,m_pCallBack=%p")
			//	, m_eType, pEntry->prtInputType, pEntry->prtInputMaxCapacity, pEntry->prtInputCurrentLevel, pEntry->prtInputName, m_pCallBack);
			/*
			if (m_eType == BT_LM || m_eType == BT_LG)
			{
				if (pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray)
				{
					continue;
				}
			}
			else if (m_eType == BT_XP || m_eType == BT_SX)
			{
				if (pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray
					&& pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray)
				{
					continue;
				}
			}
			else
			{
				if (pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray)
				{
					continue;
				}
			}
			*/
			if (pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoNonRemovableTray
				&& pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::SheetFeedAutoRemovableTray
				&& pEntry->prtInputType != ENUM_STRUCT_VALUE(PrtInputTypeTC)::ContinuousRoll)
			{
				continue;
			}

			if (pEntry->prtInputMaxCapacity <= 0 || pEntry->prtInputCurrentLevel < 0)
			{
				continue;
			}

			int nDiff = 0;
#ifdef UNICODE
			CString szInputName = CCommonFun::UTF8ToUnicode((char*)pEntry->prtInputName);
#else

			CString szInputName(pEntry->prtInputName);
#endif
			if (szInputName.Find(_T("1")) >= 0)
			{
				nDiff = 10;
			}
			else if (szInputName.Find(_T("2")) >= 0)
			{
				nDiff = 8;
			}
			else if (szInputName.Find(_T("3")) >= 0)
			{
				nDiff = 6;
			}
			else if (szInputName.Find(_T("4")) >= 0)
			{
				nDiff = 4;
			}
			else
			{
#if 0
				CString szTemp = szInputName;
				szTemp.MakeUpper();
				if (szTemp.Find("MP TRAY") >= 0)
				{
					continue;
				}
				else
				{
					nDiff = 0;
				}
#else
				continue;
#endif
			}

			int nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::UnKnown;
			CString szDesc = _T("");
			double fRatio = 1.0 * pEntry->prtInputCurrentLevel / pEntry->prtInputMaxCapacity;
			int nRatio = (int)(fRatio * 100);
			if (nRatio == 0)
			{
				nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::TaryPaperEmpty - nDiff;
				//szDesc.Format(_T("ֽ��(%s)��ֽ��(%s)����ֽ��"), szInputName, GetPaperTypeByPrtInputEntry(pEntry));
				szDesc.Format(_T("AlarmTaryPaperEmpty|%s|%s|ֽ��(%s)��ֽ��(%s)����ֽ��")
					, szInputName, GetPaperTypeByPrtInputEntry(pEntry)
					, szInputName, GetPaperTypeByPrtInputEntry(pEntry));
			}
			else if (nRatio <= m_nPaperMinRatio)
			{
				nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::TaryPaperLow - nDiff;
				//szDesc.Format(_T("ֽ��(%s)��ֽ��(%s)����ֽ(ʣ��%d%%)����׼������ֽ��")
				szDesc.Format(_T("AlarmTaryPaperLow2|%s|%s|%d%%|ֽ��(%s)��ֽ��(%s)����ֽ(ʣ��%d%%)����׼������ֽ��")
					, szInputName, GetPaperTypeByPrtInputEntry(pEntry), nRatio
					, szInputName, GetPaperTypeByPrtInputEntry(pEntry), nRatio);
			}

			//theLog.Write(_T("========nRatio=%d, szDesc=%s"), nRatio, szDesc);
			if (szDesc.GetLength()>0 && m_pCallBack)
			{
				if (nAlertCode < 100)
				{
					nAlertCode += 100;	//�ο�:��PrinterAlert_Def.h���еġ�PrinterAlertCode���Ű�����չ����Ԥ��
				}
				m_pCallBack->OnStatus(nAlertCode, szDesc.GetBuffer());
			}
		}
	}
}

void CSnmpMonitorHelper::CheckOutput()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckMarker()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckMarkerSupplies()
{
	//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);

	PrtMarkerSuppliesEntryMap::iterator it;
	for (it=m_oMarkerSuppliesMap.begin(); it!=m_oMarkerSuppliesMap.end(); it++)
	{
		PPrtMarkerSuppliesEntry pEntry = it->second;
		if (pEntry)
		{
			//theLog.Write(_T("CSnmpMonitorHelper::CheckMarkerSupplies,1,prtMarkerSuppliesType=%d,prtMarkerSuppliesMaxCapacity=%d,prtMarkerSuppliesLevel=%d")
			//	, pEntry->prtMarkerSuppliesType, pEntry->prtMarkerSuppliesMaxCapacity, pEntry->prtMarkerSuppliesLevel);
			if (pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner
				&& pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
				&& pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
				&& pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
				&& pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC
				&& pEntry->prtMarkerSuppliesType != ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
			{
				#pragma message("!!CSnmpMonitorHelper::CheckMarkerSupplies,��ʱֻ����Toner��Ink��OPC��Developer�������ĲĲ�����")
				continue;
			}

			if (pEntry->prtMarkerSuppliesMaxCapacity <= 0 || pEntry->prtMarkerSuppliesLevel < 0)
			{
				continue;
			}

			int nDiff = 0;
#ifdef UNICODE
			CString szMarkerSuppliesDescription = CCommonFun::UTF8ToUnicode((char*)pEntry->prtMarkerSuppliesDescription);
#else
			CString szMarkerSuppliesDescription = CCommonFun::UTF8ToMultiByte(pEntry->prtMarkerSuppliesDescription);
#endif
			szMarkerSuppliesDescription.MakeLower();
			if (szMarkerSuppliesDescription.Find(_T("cyan")) >= 0)
			{
				nDiff = 8;
			}
			else if (szMarkerSuppliesDescription.Find(_T("magenta")) >= 0)
			{
				nDiff = 6;
			}
			else if (szMarkerSuppliesDescription.Find(_T("yellow")) >= 0)
			{
				nDiff = 4;
			}
			else //if (szMarkerSuppliesDescription.Find(_T("black")) >= 0)	//�����֧�У����ܳ��ֺ��֣������˻�����[��ɫ̼��]��[���񲿼�]��[ά�����߰�]�ȵ�
			{
				nDiff = 2;
			}

			int nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::UnKnown;
			CString szDesc = _T("");
			double fRatio = 1.0 * pEntry->prtMarkerSuppliesLevel / pEntry->prtMarkerSuppliesMaxCapacity;
			int nRatio = (int)(fRatio * 100);
			if (nRatio == 0)
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::TonerEmpty - nDiff;
					//szDesc.Format(_T("�ۺ�(%s)���޷ۣ�"), szMarkerSuppliesDescription);
					szDesc.Format(_T("AlarmTonerEmpty|%s|�ۺ�(%s)���޷ۣ�")
						, szMarkerSuppliesDescription
						, szMarkerSuppliesDescription);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::InkEmpty - nDiff;
					//szDesc.Format(_T("ī��(%s)����ī��"), szMarkerSuppliesDescription);
					szDesc.Format(_T("AlarmInkEmpty|%s|ī��(%s)����ī��")
						, szMarkerSuppliesDescription
						, szMarkerSuppliesDescription);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::OPCEmpty - nDiff;
					//szDesc.Format(_T("�й�ĵ�Ԫ(%s)��ʹ�������ľ���"), szMarkerSuppliesDescription);
					szDesc.Format(_T("AlarmOPCEmpty|%s|�й�ĵ�Ԫ(%s)��ʹ�������ľ���")
						, szMarkerSuppliesDescription
						, szMarkerSuppliesDescription);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::DeveloperEmpty - nDiff;
					//szDesc.Format(_T("��Ӱ��Ԫ(%s)��ʹ�������ľ���"), szMarkerSuppliesDescription);
					szDesc.Format(_T("AlarmDeveloperEmpty|%s|��Ӱ��Ԫ(%s)��ʹ�������ľ���")
						, szMarkerSuppliesDescription
						, szMarkerSuppliesDescription);
				}
			} 
			else if (nRatio <= m_nInkMinRatio)
			{
				if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Toner)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::TonerLow - nDiff;
					//szDesc.Format(_T("�ۺ�(%s)���ٷ�(����%d%%)����׼�����÷ۺУ�"), szMarkerSuppliesDescription, nRatio);
					szDesc.Format(_T("AlarmTonerLow|%s|%d%%|�ۺ�(%s)���ٷ�(����%d%%)����׼�����÷ۺУ�")
						, szMarkerSuppliesDescription, nRatio
						, szMarkerSuppliesDescription, nRatio);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Ink
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkCartridge
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::InkRibbon
					|| pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::TonerCartridge)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::InkLow - nDiff;
					//szDesc.Format(_T("ī��(%s)����ī(����%d%%)����׼������ī�У�"), szMarkerSuppliesDescription, nRatio);
					szDesc.Format(_T("AlarmInkLow|%s|%d%%|ī��(%s)����ī(����%d%%)����׼������ī�У�")
						, szMarkerSuppliesDescription, nRatio
						, szMarkerSuppliesDescription, nRatio);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::OPC)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::OPCLow - nDiff;
					//szDesc.Format(_T("�й�ĵ�Ԫ(%s)��ʹ��������(����%d%%)����׼�����øй�ģ�"), szMarkerSuppliesDescription, nRatio);
					szDesc.Format(_T("AlarmOPCLow|%s|%d%%|�й�ĵ�Ԫ(%s)��ʹ��������(����%d%%)����׼�����øй�ģ�")
						, szMarkerSuppliesDescription, nRatio
						, szMarkerSuppliesDescription, nRatio);
				}
				else if (pEntry->prtMarkerSuppliesType == ENUM_STRUCT_VALUE(PrtMarkerSuppliesTypeTC)::Developer)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::DeveloperLow - nDiff;
					//szDesc.Format(_T("��Ӱ��Ԫ(%s)��ʹ��������(����%d%%)����׼��������Ӱ��"), szMarkerSuppliesDescription, nRatio);
					szDesc.Format(_T("AlarmDeveloperLow|%s|%d%%|��Ӱ��Ԫ(%s)��ʹ��������(����%d%%)����׼��������Ӱ��")
						, szMarkerSuppliesDescription, nRatio
						, szMarkerSuppliesDescription, nRatio);
				}
			}

			if (szDesc.GetLength()>0 && m_pCallBack)
			{
				if (nAlertCode < 100)
				{
					nAlertCode += 100;	//�ο�:��PrinterAlert_Def.h���еġ�PrinterAlertCode���Ű�����չ����Ԥ��
				}
				m_pCallBack->OnStatus(nAlertCode, szDesc.GetBuffer());
			}
		}
	}
}

void CSnmpMonitorHelper::CheckMarkerColorant()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckMarkerPath()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckChannel()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckInterpreter()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckConsoleDisplayBuffer()
{
	//CCriticalSection2::Owner lock(m_cs4ConsoleDisplayBufferMap);

	CString szPrtConsoleDisplayBufferText;
	PrtConsoleDisplayBufferEntryMap::iterator it;
	for (it=m_oConsoleDisplayBufferMap.begin(); it!=m_oConsoleDisplayBufferMap.end(); it++)
	{
		PPrtConsoleDisplayBufferEntry pEntry = it->second;
		if (pEntry)
		{
			//theLog.Write(_T("CSnmpMonitorHelper::CheckConsoleDisplayBuffer,1,prtConsoleDisplayBufferIndex=%d,prtConsoleDisplayBufferText=%s")
			//	, pEntry->prtConsoleDisplayBufferIndex, pEntry->prtConsoleDisplayBufferText);

			//�������������Ϣ�ϲ���һ�������Ҷ���֮�任�д���
			szPrtConsoleDisplayBufferText.AppendFormat(_T("%s\r\n"), CCommonFun::UTF8ToUnicode((char*)pEntry->prtConsoleDisplayBufferText));
		}
	}

	szPrtConsoleDisplayBufferText.TrimRight(_T("\r\n"));
	szPrtConsoleDisplayBufferText.Trim();
	if (szPrtConsoleDisplayBufferText.GetLength()>0 && m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::ConsoleDisplayBufferText, CStringToChar(szPrtConsoleDisplayBufferText));
	}
}

void CSnmpMonitorHelper::CheckConsoleLight()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::CheckAlert()
{
	//CCriticalSection2::Owner lock(m_cs4AlertMap);

	PrtAlertEntryMap::iterator it;
	for (it=m_oAlertMap.begin(); it!=m_oAlertMap.end(); it++)
	{
		PPrtAlertEntry pEntry = it->second;
		if (pEntry)
		{
#pragma message("!!CSnmpMonitorHelper::CheckAlert,��ӡ��SNMP��Ԥ������˴����߼��жϿ��ܻᵼ��Ԥ��ȱʧ��Ŀǰ��ʱ��ô���������ٲ�����֤���Ż�")
			//ֽ�С�ī�С��ۺС��й�ġ���Ӱ��Ԫ��CheckInput��CheckMarkerSupplies�Ѿ�������һЩԤ����Ϣ��
			//�˴���������û�д������Ԥ����Ϣ��
			CString szAlertDescription = CCommonFun::UTF8ToUnicode((char*)pEntry->prtAlertDescription);
			//theLog.Write(_T("CSnmpMonitorHelper::CheckAlert,prtAlertSeverityLevel=%d,prtAlertTrainingLevel=%d,prtAlertGroup=%d,")
			//	_T("prtAlertGroupIndex=%d,prtAlertLocation=%d,prtAlertCode=%d,prtAlertDescription=%s,prtAlertTime=%u")
			//	, pEntry->prtAlertSeverityLevel, pEntry->prtAlertTrainingLevel, pEntry->prtAlertGroup
			//	, pEntry->prtAlertGroupIndex, pEntry->prtAlertLocation, pEntry->prtAlertCode, szAlertDescription, pEntry->prtAlertTime);

			//�˴�һ����CheckCover()�ӿ�֮�󱻵��ã�����ǰ�Ԥ���Ѿ����ڣ�����ĸǰ�Ԥ�������ظ����
			if (m_eType == BT_HP || m_eType == BT_CANON)
			{
				//���ա����ܴ�ӡ����CheckCover()���޷���ȡ,�˴���Ҫ����ǰ�򿪵�Ԥ��.
			}
			else
			{
				if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::Cover)
				{
					continue;
				}
			}

			//�˴�����Other��UnKnown����Ԥ�����ŵ�CheckAlertEx()�й��˴���
			if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Other
				|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::UnKnown)
			{
				continue;
			}
			
			CString szDesc = _T("");
			if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::Cover)
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::CoverOpen
					|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::InterlockOpen
					|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::DoorOpen)
				{
					//szDesc.Format(_T("�ǰ���Ϣ���ǰ�(%s)���򿪣�"), szAlertDescription);
					szDesc.Format(_T("AlarmCoverInfo1|%s|�ǰ���Ϣ���ǰ屻�򿪣�"), szAlertDescription);
				}
				else
				{
					//szDesc.Format(_T("�ǰ���Ϣ���������(%d),������(%d),��������(%s)��")
					szDesc.Format(_T("AlarmCoverInfo2|%d|%d|%s|�ǰ���Ϣ���������(%d),������(%d),��������(%s)��")
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
				}
			}
			else if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::Input)
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::InputMediaSupplyLow
					|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::InputMediaSupplyEmpty
					|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::SubunitAlmostEmpty
					|| pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::SubunitEmpty)
				{
					//��CheckInput()���Ѿ�������
					continue;
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::InputMediaTrayMissing)
				{
					//szDesc.Format(_T("ֽ����Ϣ��ֽ��(%s)�����"), szAlertDescription);
					szDesc.Format(_T("AlarmInputMediaTrayMissing|%s|ֽ����Ϣ��ֽ��(%s)�����")
						, szAlertDescription, szAlertDescription);
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Jam)
				{
					//szDesc.Format(_T("���棺��ӡ����ֽ,��������(%s)��"), szAlertDescription);
					szDesc.Format(_T("AlarmPrinterJam|%s|���棺��ӡ����ֽ,��������(%s)��")
						, szAlertDescription, szAlertDescription);
				}
				else
				{
					//szDesc.Format(_T("������Ϣ���������(%d),������(%d),��������(%s)��")
					szDesc.Format(_T("AlarmAlertInfo|%d|%d|%s|������Ϣ���������(%d),������(%d),��������(%s)��")
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
				}
			}
			else if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::Output)
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Jam)
				{
					//szDesc.Format(_T("���棺��ӡ����ֽ,��������(%s)��"), szAlertDescription);
					szDesc.Format(_T("AlarmPrinterJam|%s|���棺��ӡ����ֽ,��������(%s)��")
						, szAlertDescription, szAlertDescription);
				}
				else
				{
					//szDesc.Format(_T("������Ϣ���������(%d),������(%d),��������(%s)��")
					szDesc.Format(_T("AlarmAlertInfo|%d|%d|%s|������Ϣ���������(%d),������(%d),��������(%s)��")
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
				}
			}
			else if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::MediaPath)
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Jam)
				{
					//szDesc.Format(_T("���棺��ӡ����ֽ,��������(%s)��"), szAlertDescription);
					szDesc.Format(_T("AlarmPrinterJam|%s|���棺��ӡ����ֽ,��������(%s)��")
						, szAlertDescription, szAlertDescription);
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::SubunitMissing)
				{
					//szDesc.Format(_T("ֽ����Ϣ��ֽ��(%s)�����"), szAlertDescription);
					szDesc.Format(_T("AlarmInputMediaTrayMissing|%s|ֽ����Ϣ��ֽ��(%s)�����")
						, szAlertDescription, szAlertDescription);
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::MediaPathMediaTrayMissing)
				{
					//szDesc.Format(_T("ֽ����Ϣ��ֽ��(%s)�����"), szAlertDescription);
					szDesc.Format(_T("AlarmInputMediaTrayMissing|%s|ֽ����Ϣ��ֽ��(%s)�����")
						, szAlertDescription, szAlertDescription);
				}
				else
				{
					//szDesc.Format(_T("������Ϣ���������(%d),������(%d),��������(%s)��")
					szDesc.Format(_T("AlarmAlertInfo|%d|%d|%s|������Ϣ���������(%d),������(%d),��������(%s)��")
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
				}
			}
			else if (pEntry->prtAlertGroup == ENUM_STRUCT_VALUE(PrtAlertGroupTC)::GeneralPrinter)
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Jam)
				{
					//szDesc.Format(_T("���棺��ӡ����ֽ,��������(%s)��"), szAlertDescription);
					szDesc.Format(_T("AlarmPrinterJam|%s|���棺��ӡ����ֽ,��������(%s)��")
						, szAlertDescription, szAlertDescription);
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::SubunitPowerSaver)
				{
					//�����ӵ�Ԫ�澯
					return;
				}
				else
				{
					//szDesc.Format(_T("������Ϣ���������(%d),������(%d),��������(%s)��")
					szDesc.Format(_T("AlarmAlertInfo|%d|%d|%s|������Ϣ���������(%d),������(%d),��������(%s)��")
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
						, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
				}
			}
			else
			{
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::CoverOpen)
				{
					//�����ߵ��˴���һ��ΪԤ����Ϣ���Ǹǰ���(ENUM_STRUCT_VALUE(PrtAlertGroupTC)::Cover)��
					//������ĳ��װ�ñ����ˣ��ֶ���Ԥ��������ΪInterlockOpen��������©��ҪԤ����
					//���ֲ���ǰ��Ԥ��(ENUM_STRUCT_VALUE(PrtAlertCodeTC)::CoverOpen)��ͻ��
					pEntry->prtAlertCode = ENUM_STRUCT_VALUE(PrtAlertCodeTC)::InterlockOpen;
				}

				//szDesc.Format(_T("������Ϣ���������(%d),������(%d),��������(%s)��")
				szDesc.Format(_T("AlarmAlertInfo|%d|%d|%s|������Ϣ���������(%d),������(%d),��������(%s)��")
					, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription
					, pEntry->prtAlertGroup, pEntry->prtAlertCode, szAlertDescription);
			}
			if (szDesc.GetLength()>0 && m_pCallBack)
			{
				int nAlertCode = pEntry->prtAlertCode;
				if (nAlertCode <= 0)
				{
					nAlertCode = ENUM_STRUCT_VALUE(PrinterAlertCode)::MultipleGroupsUnKnown;
				}
				if (nAlertCode < 100)
				{
					nAlertCode += 100;	//�ο�:��PrinterAlert_Def.h���еġ�PrinterAlertCode���Ű�����չ����Ԥ��
				}
				m_pCallBack->OnStatus(nAlertCode, CStringToChar(szDesc));
			}
		}
	}
}

void CSnmpMonitorHelper::CheckMeter()
{
	if (!m_pPrinterMeterMonitor)
	{
		return;
	}

	{
		//CCriticalSection2::Owner lock(m_cs4InputMap);
		m_pPrinterMeterMonitor->SetMeterInfo(m_oInputMap);
	}
	{
		//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);
		m_pPrinterMeterMonitor->SetMeterInfo(m_oMarkerSuppliesMap);
	}
}

void CSnmpMonitorHelper::CheckAlertEx()
{
	//CCriticalSection2::Owner lock(m_cs4AlertMap);

	CString szFault = _T("");
	CString szOther = _T("");
	CString szUnKnow = _T("");
	PrtAlertEntryMap::iterator it;
	for (it=m_oAlertMap.begin(); it!=m_oAlertMap.end(); it++)
	{
		PPrtAlertEntry pEntry = it->second;
		if (pEntry)
		{
			CString szAlertDescription = CCommonFun::UTF8ToUnicode((char*)pEntry->prtAlertDescription);
			//theLog.Write(_T("CSnmpMonitorHelper::CheckFaultCode,prtAlertSeverityLevel=%d,prtAlertTrainingLevel=%d,prtAlertGroup=%d,")
			//	_T("prtAlertGroupIndex=%d,prtAlertLocation=%d,prtAlertCode=%d,prtAlertDescription=%s,prtAlertTime=%u")
			//	, pEntry->prtAlertSeverityLevel, pEntry->prtAlertTrainingLevel, pEntry->prtAlertGroup
			//	, pEntry->prtAlertGroupIndex, pEntry->prtAlertLocation, pEntry->prtAlertCode, szAlertDescription, pEntry->prtAlertTime);

			//�˴�ֻ����Other��UnKnown����Ԥ������������CheckAlert()�й��˴���
			if (pEntry->prtAlertCode != ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Other
				&& pEntry->prtAlertCode != ENUM_STRUCT_VALUE(PrtAlertCodeTC)::UnKnown)
			{
				continue;
			}

			if (IsFalutInfo(szAlertDescription))
			{
				szFault.AppendFormat(_T("%s\r\n"), szAlertDescription);
			}
			else
			{
				//����Ԥ����Ϣ�ϲ���һ�������Ҷ���֮�任�д���
				if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::Other)
				{
					szOther.AppendFormat(_T("%s\r\n"), szAlertDescription);
				}
				else if (pEntry->prtAlertCode == ENUM_STRUCT_VALUE(PrtAlertCodeTC)::UnKnown)
				{
					szUnKnow.AppendFormat(_T("%s\r\n"), szAlertDescription);
				}
			}
		}
	}

	szFault.TrimRight(_T("\r\n"));
	szFault.Trim();
	if (szFault.GetLength()>0 && m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::PrinterFaultInfo, CStringToChar(szFault));
	}

	szOther.TrimRight(_T("\r\n"));
	szOther.Trim();
	if (szOther.GetLength()>0 && m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::MultipleGroupsOther, CStringToChar(szOther));
	}

	szUnKnow.TrimRight(_T("\r\n"));
	szUnKnow.Trim();
	if (szUnKnow.GetLength()>0 && m_pCallBack)
	{
		m_pCallBack->OnStatus(ENUM_STRUCT_VALUE(PrinterAlertCode)::MultipleGroupsUnKnown, CStringToChar(szUnKnow));
	}
}

//����SNMP��ȡ����OID��Ϣ
void CSnmpMonitorHelper::ClearAll()
{
	ClearGeneral();
	ClearStorageRef();
	ClearDeviceRef();
	ClearCover();
	ClearLocalization();
	ClearInput();
	ClearOutput();
	ClearMarker();
	ClearMarkerSupplies();
	ClearMarkerColorant();
	ClearMarkerPath();
	ClearChannel();
	ClearInterpreter();
	ClearConsoleDisplayBuffer();
	ClearConsoleLight();
	ClearAlert();
}

void CSnmpMonitorHelper::ClearGeneral()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearStorageRef()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearDeviceRef()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearCover()
{
	//CCriticalSection2::Owner lock(m_cs4CoverMap);

	PrtCoverEntryMap::iterator it;
	for (it=m_oCoverMap.begin(); it!=m_oCoverMap.end(); it++)
	{
		PPrtCoverEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oCoverMap.clear();
}

void CSnmpMonitorHelper::ClearLocalization()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearInput()
{
	//CCriticalSection2::Owner lock(m_cs4InputMap);

	PrtInputEntryMap::iterator it;
	for (it=m_oInputMap.begin(); it!=m_oInputMap.end(); it++)
	{
		PPrtInputEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oInputMap.clear();
}

void CSnmpMonitorHelper::ClearOutput()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearMarker()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearMarkerSupplies()
{
	//CCriticalSection2::Owner lock(m_cs4MarkerSuppliesMap);

	PrtMarkerSuppliesEntryMap::iterator it;
	for (it=m_oMarkerSuppliesMap.begin(); it!=m_oMarkerSuppliesMap.end(); it++)
	{
		PPrtMarkerSuppliesEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oMarkerSuppliesMap.clear();
}

void CSnmpMonitorHelper::ClearMarkerColorant()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearMarkerPath()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearChannel()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearInterpreter()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearConsoleDisplayBuffer()
{
	//CCriticalSection2::Owner lock(m_cs4ConsoleDisplayBufferMap);

	PrtConsoleDisplayBufferEntryMap::iterator it;
	for (it=m_oConsoleDisplayBufferMap.begin(); it!=m_oConsoleDisplayBufferMap.end(); it++)
	{
		PPrtConsoleDisplayBufferEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oConsoleDisplayBufferMap.clear();
}

void CSnmpMonitorHelper::ClearConsoleLight()
{
#pragma message("CSnmpMonitorHelper::XX ��δʵ��")
}

void CSnmpMonitorHelper::ClearAlert()
{
	//CCriticalSection2::Owner lock(m_cs4AlertMap);

	PrtAlertEntryMap::iterator it;
	for (it=m_oAlertMap.begin(); it!=m_oAlertMap.end(); it++)
	{
		PPrtAlertEntry pEntry = it->second;
		if (pEntry)
		{
			delete pEntry;
			pEntry = NULL;
		}
	}
	m_oAlertMap.clear();
}

void CSnmpMonitorHelper::InitOID()
{
	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry
	m_szPrtGeneralEntryOID = DecryptOID(PrtGeneralEntryOID);
	m_szPrtGeneralConfigChangesOID = DecryptOID(PrtGeneralConfigChangesOID);
	m_szPrtGeneralCurrentLocalizationOID = DecryptOID(PrtGeneralCurrentLocalizationOID);
	m_szPrtGeneralResetOID = DecryptOID(PrtGeneralResetOID);
	m_szPrtGeneralCurrentOperatorOID = DecryptOID(PrtGeneralCurrentOperatorOID);
	m_szPrtGeneralServicePersonOID = DecryptOID(PrtGeneralServicePersonOID);
	m_szPrtInputDefaultIndexOID = DecryptOID(PrtInputDefaultIndexOID);
	m_szPrtOutputDefaultIndexOID = DecryptOID(PrtOutputDefaultIndexOID);
	m_szPrtMarkerDefaultIndexOID = DecryptOID(PrtMarkerDefaultIndexOID);
	m_szPrtMediaPathDefaultIndexOID = DecryptOID(PrtMediaPathDefaultIndexOID);
	m_szPrtConsoleLocalizationOID = DecryptOID(PrtConsoleLocalizationOID);
	m_szPrtConsoleNumberOfDisplayLinesOID = DecryptOID(PrtConsoleNumberOfDisplayLinesOID);
	m_szPrtConsoleNumberOfDisplayCharsOID = DecryptOID(PrtConsoleNumberOfDisplayCharsOID);
	m_szPrtConsoleDisableOID = DecryptOID(PrtConsoleDisableOID);
#if 0
	m_szPrtAuxiliarySheetStartupPageOID = DecryptOID(PrtAuxiliarySheetStartupPageOID);
	m_szPrtAuxiliarySheetBannerPageOID = DecryptOID(PrtAuxiliarySheetBannerPageOID);
	m_szPrtGeneralPrinterNameOID = DecryptOID(PrtGeneralPrinterNameOID);
	m_szPrtGeneralSerialNumberOID = DecryptOID(PrtGeneralSerialNumberOID);
	m_szPrtAlertCriticalEventsOID = DecryptOID(PrtAlertCriticalEventsOID);
	m_szPrtAlertAllEventsOID = DecryptOID(PrtAlertAllEventsOID);
#else
	m_szPrtAuxiliarySheetStartupPageOID.Format("%s.14", m_szPrtGeneralEntryOID);
	m_szPrtAuxiliarySheetBannerPageOID.Format("%s.15", m_szPrtGeneralEntryOID);
	m_szPrtGeneralPrinterNameOID.Format("%s.16", m_szPrtGeneralEntryOID);
	m_szPrtGeneralSerialNumberOID.Format("%s.17", m_szPrtGeneralEntryOID);
	m_szPrtAlertCriticalEventsOID.Format("%s.18", m_szPrtGeneralEntryOID);
	m_szPrtAlertAllEventsOID.Format("%s.19", m_szPrtGeneralEntryOID);
#endif

	//printmib.prtCover.prtCoverTable.prtCoverEntry
	m_szPrtCoverEntryOID = DecryptOID(PrtCoverEntryOID);
	m_szPrtCoverIndexOID = DecryptOID(PrtCoverIndexOID);
	m_szPrtCoverDescriptionOID = DecryptOID(PrtCoverDescriptionOID);
	m_szPrtCoverStatusOID = DecryptOID(PrtCoverStatusOID);

	//printmib.prtLocalization.prtLocalizationTable.prtLocalizationEntry
	m_szPrtLocalizationEntryOID = DecryptOID(PrtLocalizationEntryOID);
	m_szPrtLocalizationIndexOID = DecryptOID(PrtLocalizationIndexOID);
	m_szPrtLocalizationLanguageOID = DecryptOID(PrtLocalizationLanguageOID);
	m_szPrtLocalizationCountryOID = DecryptOID(PrtLocalizationCountryOID);
	m_szPrtLocalizationCharacterSetOID = DecryptOID(PrtLocalizationCharacterSetOID);

	//printmib.prtInput.prtInputTable.prtInputEntry
	m_szPrtInputEntryOID = DecryptOID(PrtInputEntryOID);
	m_szPrtInputIndexOID = DecryptOID(PrtInputIndexOID);
	m_szPrtInputTypeOID = DecryptOID(PrtInputTypeOID);
	m_szPrtInputDimUnitOID = DecryptOID(PrtInputDimUnitOID);
	m_szPrtInputMediaDimFeedDirDeclaredOID = DecryptOID(PrtInputMediaDimFeedDirDeclaredOID);
	m_szPrtInputMediaDimXFeedDirDeclaredOID = DecryptOID(PrtInputMediaDimXFeedDirDeclaredOID);
	m_szPrtInputMediaDimFeedDirChosenOID = DecryptOID(PrtInputMediaDimFeedDirChosenOID);
	m_szPrtInputMediaDimXFeedDirChosenOID = DecryptOID(PrtInputMediaDimXFeedDirChosenOID);
	m_szPrtInputCapacityUnitOID = DecryptOID(PrtInputCapacityUnitOID);
	m_szPrtInputMaxCapacityOID = DecryptOID(PrtInputMaxCapacityOID);
	m_szPrtInputCurrentLevelOID = DecryptOID(PrtInputCurrentLevelOID);
	m_szPrtInputStatusOID = DecryptOID(PrtInputStatusOID);
	m_szPrtInputMediaNameOID = DecryptOID(PrtInputMediaNameOID);
	m_szPrtInputNameOID = DecryptOID(PrtInputNameOID);
	m_szPrtInputVendorNameOID = DecryptOID(PrtInputVendorNameOID);
	m_szPrtInputModelOID = DecryptOID(PrtInputModelOID);
	m_szPrtInputVersionOID = DecryptOID(PrtInputVersionOID);
	m_szPrtInputSerialNumberOID = DecryptOID(PrtInputSerialNumberOID);
	m_szPrtInputDescriptionOID = DecryptOID(PrtInputDescriptionOID);
	m_szPrtInputSecurityOID = DecryptOID(PrtInputSecurityOID);
	m_szPrtInputMediaWeightOID = DecryptOID(PrtInputMediaWeightOID);
	m_szPrtInputMediaTypeOID = DecryptOID(PrtInputMediaTypeOID);
	m_szPrtInputMediaColorOID = DecryptOID(PrtInputMediaColorOID);
	m_szPrtInputMediaFormPartsOID = DecryptOID(PrtInputMediaFormPartsOID);
#ifdef RFC_3805
#if 0
	m_szPrtInputMediaLoadTimeoutOID = DecryptOID(PrtInputMediaLoadTimeoutOID);
	m_szPrtInputNextIndexOID = DecryptOID(PrtInputNextIndexOID);
#else
	m_szPrtInputMediaLoadTimeoutOID.Format("%s.24", m_szPrtInputEntryOID);
	m_szPrtInputNextIndexOID.Format("%s.25", m_szPrtInputEntryOID);
#endif
#endif

	//printmib.prtOutput.prtOutputTable.prtOutputEntry
	m_szPrtOutputEntryOID = DecryptOID(PrtOutputEntryOID);
	m_szPrtOutputIndexOID = DecryptOID(PrtOutputIndexOID);
	m_szPrtOutputTypeOID = DecryptOID(PrtOutputTypeOID);
	m_szPrtOutputCapacityUnitOID = DecryptOID(PrtOutputCapacityUnitOID);
	m_szPrtOutputMaxCapacityOID = DecryptOID(PrtOutputMaxCapacityOID);
	m_szPrtOutputRemainingCapacityOID = DecryptOID(PrtOutputRemainingCapacityOID);
	m_szPrtOutputStatusOID = DecryptOID(PrtOutputStatusOID);
	m_szPrtOutputNameOID = DecryptOID(PrtOutputNameOID);
	m_szPrtOutputVendorNameOID = DecryptOID(PrtOutputVendorNameOID);
	m_szPrtOutputModelOID = DecryptOID(PrtOutputModelOID);
	m_szPrtOutputVersionOID = DecryptOID(PrtOutputVersionOID);
	m_szPrtOutputSerialNumberOID = DecryptOID(PrtOutputSerialNumberOID);
	m_szPrtOutputDescriptionOID = DecryptOID(PrtOutputDescriptionOID);
	m_szPrtOutputSecurityOID = DecryptOID(PrtOutputSecurityOID);
	m_szPrtOutputDimUnitOID = DecryptOID(PrtOutputDimUnitOID);
	m_szPrtOutputMaxDimFeedDirOID = DecryptOID(PrtOutputMaxDimFeedDirOID);
	m_szPrtOutputMaxDimXFeedDirOID = DecryptOID(PrtOutputMaxDimXFeedDirOID);
	m_szPrtOutputMinDimFeedDirOID = DecryptOID(PrtOutputMinDimFeedDirOID);
	m_szPrtOutputMinDimXFeedDirOID = DecryptOID(PrtOutputMinDimXFeedDirOID);
	m_szPrtOutputStackingOrderOID = DecryptOID(PrtOutputStackingOrderOID);
	m_szPrtOutputPageDeliveryOrientationOID = DecryptOID(PrtOutputPageDeliveryOrientationOID);
	m_szPrtOutputBurstingOID = DecryptOID(PrtOutputBurstingOID);
	m_szPrtOutputDecollatingOID = DecryptOID(PrtOutputDecollatingOID);
	m_szPrtOutputPageCollatedOID = DecryptOID(PrtOutputPageCollatedOID);
	m_szPrtOutputOffsetStackingOID = DecryptOID(PrtOutputOffsetStackingOID);

	//printmib.prtMarker.prtMarkerTable.prtMarkerEntry
	m_szPrtMarkerEntryOID = DecryptOID(PrtMarkerEntryOID);
	m_szPrtMarkerIndexOID = DecryptOID(PrtMarkerIndexOID);
	m_szPrtMarkerMarkTechOID = DecryptOID(PrtMarkerMarkTechOID);
	m_szPrtMarkerCounterUnitOID = DecryptOID(PrtMarkerCounterUnitOID);
	m_szPrtMarkerLifeCountOID = DecryptOID(PrtMarkerLifeCountOID);
	m_szPrtMarkerPowerOnCount = DecryptOID(PrtMarkerPowerOnCount);
	m_szPrtMarkerProcessColorantsOID = DecryptOID(PrtMarkerProcessColorantsOID);
	m_szPrtMarkerSpotColorantsOID = DecryptOID(PrtMarkerSpotColorantsOID);
	m_szPrtMarkerAddressabilityUnitOID = DecryptOID(PrtMarkerAddressabilityUnitOID);
	m_szPrtMarkerAddressabilityFeedDirOID = DecryptOID(PrtMarkerAddressabilityFeedDirOID);
	m_szPrtMarkerAddressabilityXFeedDirOID = DecryptOID(PrtMarkerAddressabilityXFeedDirOID);
	m_szPrtMarkerNorthMarginOID = DecryptOID(PrtMarkerNorthMarginOID);
	m_szPrtMarkerSouthMarginOID = DecryptOID(PrtMarkerSouthMarginOID);
	m_szPrtMarkerWestMarginOID = DecryptOID(PrtMarkerWestMarginOID);
	m_szPrtMarkerEastMarginOID = DecryptOID(PrtMarkerEastMarginOID);
	m_szPrtMarkerStatusOID = DecryptOID(PrtMarkerStatusOID);

	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry
	m_szPrtMarkerSuppliesEntryOID = DecryptOID(PrtMarkerSuppliesEntryOID);
	m_szPrtMarkerSuppliesIndexOID = DecryptOID(PrtMarkerSuppliesIndexOID);
	m_szPrtMarkerSuppliesMarkerIndexOID = DecryptOID(PrtMarkerSuppliesMarkerIndexOID);
	m_szPrtMarkerSuppliesColorantIndexOID = DecryptOID(PrtMarkerSuppliesColorantIndexOID);
	m_szPrtMarkerSuppliesClassOID = DecryptOID(PrtMarkerSuppliesClassOID);
	m_szPrtMarkerSuppliesTypeOID = DecryptOID(PrtMarkerSuppliesTypeOID);
	m_szPrtMarkerSuppliesDescriptionOID = DecryptOID(PrtMarkerSuppliesDescriptionOID);
	m_szPrtMarkerSuppliesSupplyUnitOID = DecryptOID(PrtMarkerSuppliesSupplyUnitOID);
	m_szPrtMarkerSuppliesMaxCapacityOID = DecryptOID(PrtMarkerSuppliesMaxCapacityOID);
	m_szPrtMarkerSuppliesLevelOID = DecryptOID(PrtMarkerSuppliesLevelOID);

	//printmib.prtMarkerColorant.prtMarkerColorantTable.prtMarkerColorantEntry
	m_szPrtMarkerColorantEntryOID = DecryptOID(PrtMarkerColorantEntryOID);
	m_szPrtMarkerColorantIndexOID = DecryptOID(PrtMarkerColorantIndexOID);
	m_szPrtMarkerColorantMarkerIndexOID = DecryptOID(PrtMarkerColorantMarkerIndexOID);
	m_szPrtMarkerColorantRoleOID = DecryptOID(PrtMarkerColorantRoleOID);
	m_szPrtMarkerColorantValueOID = DecryptOID(PrtMarkerColorantValueOID);
	m_szPrtMarkerColorantTonalityOID = DecryptOID(PrtMarkerColorantTonalityOID);

	//printmib.prtMediaPath.prtMediaPathTable.prtMediaPathEntry
	m_szPrtMediaPathEntryOID = DecryptOID(PrtMediaPathEntryOID);
	m_szPrtMediaPathIndexOID = DecryptOID(PrtMediaPathIndexOID);
	m_szPrtMediaPathMaxSpeedPrintUnitOID = DecryptOID(PrtMediaPathMaxSpeedPrintUnitOID);
	m_szPrtMediaPathMediaSizeUnitOID = DecryptOID(PrtMediaPathMediaSizeUnitOID);
	m_szPrtMediaPathMaxSpeedOID = DecryptOID(PrtMediaPathMaxSpeedOID);
	m_szPrtMediaPathMaxMediaFeedDirOID = DecryptOID(PrtMediaPathMaxMediaFeedDirOID);
	m_szPrtMediaPathMaxMediaXFeedDirOID = DecryptOID(PrtMediaPathMaxMediaXFeedDirOID);
	m_szPrtMediaPathMinMediaFeedDirOID = DecryptOID(PrtMediaPathMinMediaFeedDirOID);
	m_szPrtMediaPathMinMediaXFeedDirOID = DecryptOID(PrtMediaPathMinMediaXFeedDirOID);
	m_szPrtMediaPathTypeOID = DecryptOID(PrtMediaPathTypeOID);
	m_szPrtMediaPathDescriptionOID = DecryptOID(PrtMediaPathDescriptionOID);
	m_szPrtMediaPathStatusOID = DecryptOID(PrtMediaPathStatusOID);

	//printmib.prtChannel.prtChannelTable.prtChannelEntry
	m_szPrtChannelEntryOID = DecryptOID(PrtChannelEntryOID);
	m_szPrtChannelIndexOID = DecryptOID(PrtChannelIndexOID);
	m_szPrtChannelTypeOID = DecryptOID(PrtChannelTypeOID);
	m_szPrtChannelProtocolVersionOID = DecryptOID(PrtChannelProtocolVersionOID);
	m_szPrtChannelCurrentJobCntlLangIndexOID = DecryptOID(PrtChannelCurrentJobCntlLangIndexOID);
	m_szPrtChannelDefaultPageDescLangIndexOID = DecryptOID(PrtChannelDefaultPageDescLangIndexOID);
	m_szPrtChannelStateOID = DecryptOID(PrtChannelStateOID);
	m_szPrtChannelIfIndexOID = DecryptOID(PrtChannelIfIndexOID);
	m_szPrtChannelStatusOID = DecryptOID(PrtChannelStatusOID);
#if 0
	m_szPrtChannelInformationOID = DecryptOID(PrtChannelInformationOID);
#else
	m_szPrtChannelInformationOID.Format("%s.9", m_szPrtChannelEntryOID);
#endif

	//printmib.prtInterpreter.prtInterpreterTable.prtInterpreterEntry
	m_szPrtInterpreterEntryOID = DecryptOID(PrtInterpreterEntryOID);
	m_szPrtInterpreterIndexOID = DecryptOID(PrtInterpreterIndexOID);
	m_szPrtInterpreterLangFamilyOID = DecryptOID(PrtInterpreterLangFamilyOID);
	m_szPrtInterpreterLangLevelOID = DecryptOID(PrtInterpreterLangLevelOID);
	m_szPrtInterpreterLangVersionOID = DecryptOID(PrtInterpreterLangVersionOID);
	m_szPrtInterpreterDescriptionOID = DecryptOID(PrtInterpreterDescriptionOID);
	m_szPrtInterpreterVersionOID = DecryptOID(PrtInterpreterVersionOID);
	m_szPrtInterpreterDefaultOrientationOID = DecryptOID(PrtInterpreterDefaultOrientationOID);
	m_szPrtInterpreterFeedAddressabilityOID = DecryptOID(PrtInterpreterFeedAddressabilityOID);
	m_szPrtInterpreterXFeedAddressabilityOID = DecryptOID(PrtInterpreterXFeedAddressabilityOID);
	m_szPrtInterpreterDefaultCharSetInOID = DecryptOID(PrtInterpreterDefaultCharSetInOID);
	m_szPrtInterpreterDefaultCharSetOutOID = DecryptOID(PrtInterpreterDefaultCharSetOutOID);
	m_szPrtInterpreterTwoWayOID = DecryptOID(PrtInterpreterTwoWayOID);

	//printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry
	m_szPrtConsoleDisplayBufferEntryOID = DecryptOID(PrtConsoleDisplayBufferEntryOID);
	m_szPrtConsoleDisplayBufferIndexOID = DecryptOID(PrtConsoleDisplayBufferIndexOID);
	m_szPrtConsoleDisplayBufferTextOID = DecryptOID(PrtConsoleDisplayBufferTextOID);

	//printmib.prtConsoleLight.prtConsoleLightTable.prtConsoleLightEntry
	m_szPrtConsoleLightEntryOID = DecryptOID(PrtConsoleLightEntryOID);
	m_szPrtConsoleLightIndexOID = DecryptOID(PrtConsoleLightIndexOID);
	m_szPrtConsoleOnTimeOID = DecryptOID(PrtConsoleOnTimeOID);
	m_szPrtConsoleOffTimeOID = DecryptOID(PrtConsoleOffTimeOID);
	m_szPrtConsoleColorOID = DecryptOID(PrtConsoleColorOID);
	m_szPrtConsoleDescriptionOID = DecryptOID(PrtConsoleDescriptionOID);

	//printmib.prtAlert.prtAlertTable.prtAlertEntry
	m_szPrtAlertEntryOID = DecryptOID(PrtAlertEntryOID);
	m_szPrtAlertIndexOID = DecryptOID(PrtAlertIndexOID);
	m_szPrtAlertSeverityLevelOID = DecryptOID(PrtAlertSeverityLevelOID);
	m_szPrtAlertTrainingLevelOID = DecryptOID(PrtAlertTrainingLevelOID);
	m_szPrtAlertGroupOID = DecryptOID(PrtAlertGroupOID);
	m_szPrtAlertGroupIndexOID = DecryptOID(PrtAlertGroupIndexOID);
	m_szPrtAlertLocationOID = DecryptOID(PrtAlertLocationOID);
	m_szPrtAlertCodeOID = DecryptOID(PrtAlertCodeOID);
	m_szPrtAlertDescriptionOID = DecryptOID(PrtAlertDescriptionOID);
	m_szPrtAlertTimeOID = DecryptOID(PrtAlertTimeOID);
}