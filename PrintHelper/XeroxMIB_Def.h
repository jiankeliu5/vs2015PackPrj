#ifndef _XEROX_MIB_DEF_8BE86FA5_B805_43A0_AEDD_681C9DAEDF4A_H_
#define _XEROX_MIB_DEF_8BE86FA5_B805_43A0_AEDD_681C9DAEDF4A_H_

#include "Snmp_Def.h"

//ʩ�ֳ�������ڵ�
#define XeroxA4PrintHBPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9Ug==")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.7"
#define XeroxA4PrintColorPageOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9V1o=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.29"
#define XeroxA4CopyHBPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MDAkdhVU0B")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.103.20.3"
#define XeroxA4CopyColorPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MDAkdhVU0AZg==")			//".1.3.6.1.4.1.253.8.53.13.2.1.6.103.20.25"
#define XeroxScannedImagesStoredOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MDA0dhVU0AYg==")			//".1.3.6.1.4.1.253.8.53.13.2.1.6.102.20.21"
#define XeroxNetworkScanningImagesSentOID	TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MDA0dhVU0DYg==")			//".1.3.6.1.4.1.253.8.53.13.2.1.6.102.20.11"
#define XeroxA4FaxTotalPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9UlI=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.71"
#define XeroxA4ColorTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9VlA=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.33"
#define XeroxA4HBTotalPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9Vlc=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.34"
#define XeroxA3ColorTotalPageOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9UVA=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.43"
#define XeroxA3HBTotalPageOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRBHQBafVdNA31CT0MdA1l9UVc=")				//".1.3.6.1.4.1.253.8.53.13.2.1.6.1.20.44"

//ʩ����ҵ״̬
#define XeroxJobStatusProcessing			7		//��ҵ���ڴ�������
#define XeroxJobStatusFinished				17		//��ҵ�Ѿ�����

//ʩ����ҵ����OID
#define XeroxJobTypeScanToFileOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYg==")								//".1.3.6.1.4.1.253.8.58.2.1"
#define XeroxJobTypeScanToPrintOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYQ==")								//".1.3.6.1.4.1.253.8.58.2.2"
#define XeroxJobTypeScanToFaxOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYA==")								//".1.3.6.1.4.1.253.8.58.2.3"
#define XeroxJobTypeScanToMailListOID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHZw==")								//".1.3.6.1.4.1.253.8.58.2.4"
#define XeroxJobTypeFaxToFileOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHZg==")								//".1.3.6.1.4.1.253.8.58.2.5"
#define XeroxJobTypeFaxToPrintOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHZQ==")								//".1.3.6.1.4.1.253.8.58.2.6"
#define XeroxJobTypeFaxToMailListOID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHZA==")								//".1.3.6.1.4.1.253.8.58.2.7"
#define XeroxJobTypePrintOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHaw==")								//".1.3.6.1.4.1.253.8.58.2.8"
#define XeroxJobTypeFileToFaxOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHag==")								//".1.3.6.1.4.1.253.8.58.2.9"
#define XeroxJobTypeFileToMailListOID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYlU=")								//".1.3.6.1.4.1.253.8.58.2.10"
#define XeroxJobTypeCopyOID					TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYlQ=")								//".1.3.6.1.4.1.253.8.58.2.11"
#define XeroxJobTypeFileToFileOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRKHQNHYlc=")								//".1.3.6.1.4.1.253.8.58.2.12"

//ʩ�ֵ�ǰ��ҵ��Ϣ
#define XeroxCurentJobListBeginOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQdHYktSHGFaUEICH1F9UVscZ0xPRgsfXWs=")	//".1.3.6.1.4.1.253.8.59.6.1.1.2.101.8.48.48.48.48"		//ʩ�ִ�ӡ����ǰ��ҵ�б���ʼoid
#define XeroxCurentJobTypeOID				TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQdHYktSHGdaUEICH1F9UVscZ0xPRgsfXWs=")	//".1.3.6.1.4.1.253.8.59.6.1.1.4.101.8.48.48.48.48"		//��ҵ���͵�OID
#define XeroxCurentJobDocNameIndexOID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQdHYktSHGZaUEICH1F9UVscZ0xPRgsfXWs=")	//".1.3.6.1.4.1.253.8.59.6.1.1.5.101.8.48.48.48.48"		//�ĵ���Index
#define XeroxCurentJobOwnerIndexOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQdHYktSHGVaUEICH1F9UVscZ0xPRgsfXWs=")	//".1.3.6.1.4.1.253.8.59.6.1.1.6.101.8.48.48.48.48"		//��ҵ�û���Index
#define XeroxCurentJobSenderIndexOID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQBZfVRNA31FT0MDAEdrS1cKfUBZXAcJR2dd")	//".1.3.6.1.4.1.253.8.59.10.1.1.1.101.8.48.48.48.48"	//��ҵ������Index
#define XeroxCurentJobDocNameOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRDHQRHYktSHGdaUEIC")						//".1.3.6.1.4.1.253.8.51.5.1.1.4.101"					//XeroxCurentJobDocNameIndexOID,Index������ҵID,=".1.3.6.1.4.1.253.8.51.5.1.1.4.101.Index.3"

//ʩ����ʷ��ҵ��Ϣ
#define XeroxHistoryJobListBeginOID			TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRLHQdHYktSHGY=")							//".1.3.6.1.4.1.253.8.59.6.1.1.5"						//ʩ�ִ�ӡ����ʷ��ҵ�б���ʼoid
#define XeroxHistoryDocNameFormat1OID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRDHQRHYktSHGdaUFwWVUdiVVU=")				//".1.3.6.1.4.1.253.8.51.5.1.1.4.1.%d.106"				//XeroxHistoryJobListBeginOID,Index������ҵID,��ҵ��ɺ���ҵ�ĵ���=".1.3.6.1.4.1.253.8.51.5.1.1.4.1.Index.106"
#define XeroxHistoryDocNameFormat151OID		TEXT("H1h9Vk0EfUVPRh0AR2FQUBxrWlRDHQRHYktSHGdaUEcCH0w3S1A=")				//".1.3.6.1.4.1.253.8.51.5.1.1.4.151.%d.3"				//XeroxHistoryJobListBeginOID,Index������ҵID,��ҵ��ɺ���ҵ�ĵ���=".1.3.6.1.4.1.253.8.51.5.1.1.4.151.Index.3"

//ʩ���滻�ַ���
#define XeroxPrivateEnterprisesOID			TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdP")			//".iso.org.dod.internet.private.enterprises."
#define XeroxPrivateEnterprises2OID			TEXT("H1h9Vk0EfUVPRh0ARw==")												//".1.3.6.1.4.1."
#define XeroxPrintStatusOID					TEXT("A1xgS1scZk1PRB0AR2JLVg==")											//"253.8.59.6.1.1.5"
#define XeroxPrintStatus2OID				TEXT("A1xgS1scZk1PRB0AR2JLWg==")											//"253.8.59.6.1.1.9"
#define XeroxPrintedPagesOID				TEXT("A1xgS1scZk1PRB0AR2JLVg==")											//"253.8.59.6.1.1.5"
#define XeroxPrintedPages2OID				TEXT("A1xgS1scZk1PQwUfWH1UTQc=")											//"253.8.59.16.1.1.5"
#define XeroxPrintTimeOID					TEXT("A1xgS1scZk1PRB0AR2JLVg==")											//"253.8.59.6.1.1.5"
#define XeroxPrintTime2OID					TEXT("A1xgS1scZk1PQwMfWH1UTQNj")											//"253.8.59.10.1.1.10"

//ʩ��HTML�������������ͣ�begin
BEGIN_ENUM_STRUCT(XeroxMarkerSuppliesType)
{
	Other = 0,		//��Ӱ���������2ƫѹתӡ����תӡ��
	OPC = 1,		//�й��
	WasteToner = 2,	//�Ϸۺ�
	Toner = 3		//ī��Ͳ
}
END_ENUM_STRUCT(XeroxMarkerSuppliesType)

//var stsother = ['����','����ʹ��','Ԥ������Ʒ','��Ҫ����','Ʒ����������','��Ҫ�������','δ��װ��װ����(��Ҫ���°�װ)'];
BEGIN_ENUM_STRUCT(XeroxMarkerSuppliesStatusOther)
{
	Good = 0,				//����
	UnAvailable = 1,		//����ʹ��
	PrepareSpares = 2,		//Ԥ������Ʒ
	NeedReplaced = 3,		//��Ҫ����
	QualityLifeDue = 4,		//Ʒ����������
	NeedReplacedSoon = 5,	//��Ҫ�������
	BadInstalled = 6		//δ��װ��װ����(��Ҫ���°�װ)
}
END_ENUM_STRUCT(XeroxMarkerSuppliesStatusOther)

//var stsTnr=['����','�й��/ī��Ͳ�쳣','��Ҫ�������','��Ҫ����','�����趨','δ��װ��װ����(��Ҫ���°�װ)','����','����(Ԥ������Ʒ)'];
BEGIN_ENUM_STRUCT(XeroxMarkerSuppliesStatusToner)
{
	Good = 0,				//����
	Abnormal = 1,			//�й��/ī��Ͳ�쳣
	NeedReplacedSoon = 2,	//��Ҫ�������
	NeedReplaced = 3,		//��Ҫ����
	Setting = 4,			//�����趨
	BadInstalled = 5,		//δ��װ��װ����(��Ҫ���°�װ)
	Fault = 6,				//����
	GoodAndPrepareSpares = 7	//����(Ԥ������Ʒ)
}
END_ENUM_STRUCT(XeroxMarkerSuppliesStatusToner)

//var stsTnrBtl=['����','Ԥ������Ʒ','��Ҫ����','����������','δ��װ'];
BEGIN_ENUM_STRUCT(XeroxMarkerSuppliesStatusWasteToner)
{
	Good = 0,				//����
	PrepareSpares = 1,		//Ԥ������Ʒ
	NeedReplaced = 2,		//��Ҫ����
	SensorFault = 3,		//����������
	UnInstalled = 4			//δ��װ
}
END_ENUM_STRUCT(XeroxMarkerSuppliesStatusWasteToner)

//var stsDrm=['����','ʶ�����ƥ��','�й��/ī��Ͳ�쳣','Ԥ������Ʒ','��Ҫ����','δ��װ��װ����(��Ҫ���°�װ)','δ��װ','Ʒ����������','����','��Ҫ�������'];
BEGIN_ENUM_STRUCT(XeroxMarkerSuppliesStatusOPC)
{
	Good = 0,				//����
	IdentifierNotMatch = 1,	//ʶ�����ƥ��
	Abnormal = 2,			//�й��/ī��Ͳ�쳣
	PrepareSpares = 3,		//Ԥ������Ʒ
	NeedReplaced = 4,		//��Ҫ����
	BadInstalled = 5,		//δ��װ��װ����(��Ҫ���°�װ)
	UnInstalled = 6,		//δ��װ
	QualityLifeDue = 7,		//Ʒ����������
	Fault = 8,				//����
	NeedReplacedSoon = 9	//��Ҫ�������
}
END_ENUM_STRUCT(XeroxMarkerSuppliesStatusOPC)

typedef struct tag_XeroxMarkerSupplies
{
	ENUM_STRUCT_TYPE(XeroxMarkerSuppliesType) xeroxMarkerSuppliesType;
	char xeroxMarkerSuppliesName[256];
	int xeroxMarkerSuppliesStatus;	//�ο�XeroxMarkerSuppliesStatusXXX������ȡ����xeroxMarkerSuppliesType
	int xeroxMarkerSuppliesLevel;
}XeroxMarkerSupplies, *PXeroxMarkerSupplies;
typedef map<int, PXeroxMarkerSupplies> XeroxMarkerSuppliesMap;
typedef list<PXeroxMarkerSupplies> XeroxMarkerSuppliesList;

//ʩ��HTML�������������ͣ�end

#endif	//_XEROX_MIB_DEF_8BE86FA5_B805_43A0_AEDD_681C9DAEDF4A_H_
