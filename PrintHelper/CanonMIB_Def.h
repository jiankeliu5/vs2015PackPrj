#ifndef _CANON_MIB_DEF_24431697_D6A9_4A8B_8213_4ACC178F9E27_H_
#define _CANON_MIB_DEF_24431697_D6A9_4A8B_8213_4ACC178F9E27_H_

#include "Snmp_Def.h"

//���ܳ�������ڵ�
#define CanonAllTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0MDAA==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.101"
#define CanonA3HBTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0MCAw==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.112"
#define CanonA4HBTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0MCAg==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.113"
#define CanonA3ColorTotalPageOID		TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0MBAw==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.122"
#define CanonA4ColorTotalPageOID		TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0MBAg==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.123"
#define CanonScanTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JTUwB9RU9DAh9YfVFNA31AT0cDAA==")	//".1.3.6.1.4.1.1602.1.11.1.4.1.4.501"

/*
OID=.1.3.6.1.4.1.1602.1.5.5.3.1.1.2.1.2, Type=Integer, Value=3
OID=.1.3.6.1.4.1.1602.1.5.5.3.1.1.3.1.2, Type=Integer, Value=5
OID=.1.3.6.1.4.1.1602.1.5.5.3.1.1.4.1.2, Type=Integer, Value=67109888
OID=.1.3.6.1.4.1.1602.1.5.5.3.1.1.5.1.2, Type=OctetString, Value=E400
OID=.1.3.6.1.4.1.1602.1.5.5.3.1.1.6.1.2, Type=TimeTicks, Value=0:00:50.68
*/
//���ܹ��ϴ���ڵ㣬����snmp�����ڵ㣬�������ṹ
#define CanonFaultInfoEntryOID			TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQwUBW31UTQd9QU9BHQBHYg==")			//".iso.org.dod.internet.private.enterprises.1602.1.5.5.3.1.1"
typedef struct tag_canonFaultInfoEntry
{
	Integer32 canonFaultInfoIndex;							//(1)
	Integer32 canonFaultInfoData2;							//(2)	//δ֪��Ϣ
	Integer32 canonFaultInfoData3;							//(3)	//δ֪��Ϣ
	Integer32 canonFaultInfoData4;							//(4)	//δ֪��Ϣ
	OCTET_STRING canonFaultInfoDesc[OCTET_STRING_SIZE_128];	//(5)
	TimeTicks canonFaultInfoTime;							//(6)
}CanonFaultInfoEntry , *PCanonFaultInfoEntry;
typedef list<PCanonFaultInfoEntry> RicohEngCounterEntryList;
typedef map<int, PCanonFaultInfoEntry> RicohEngCounterEntryMap;
#endif	//_CANON_MIB_DEF_24431697_D6A9_4A8B_8213_4ACC178F9E27_H_
