#ifndef _Snmp_Interface_h_27E39E90_744D_4560_95FD_F5C6B2BBC9A6_
#define _Snmp_Interface_h_27E39E90_744D_4560_95FD_F5C6B2BBC9A6_

#include <snmp.h>
#pragma comment(lib,"snmpapi.lib")

#include <mgmtapi.h>
#pragma comment(lib,"mgmtapi.lib")

#include <winsock2.h>
#include <IPExport.h>
#include <icmpapi.h>
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

#include "PrinterMIBv2_Def.h"
#include "PrinterAlert_Def.h"
#include "RFC1213MIB_Def.h"
#include "HostResourcesMIB_Def.h"

#define Max_Paper_Index	5
#define Max_Ink_Index	5

#define Max_Tray_Count	10
#define Max_Ink_Count	5

#define SNMP_CMMT_SIZE	100
#define SNMP_CMMT_COUNT 50
struct tag_Snmp_Desc
{
	char szCmmt[SNMP_CMMT_SIZE];
};

//FX_DC_SCP_STATUS��ʱ���ܳ���100����ΪҪ��PrinterAlertCode���ּ��ݡ�
enum FX_DC_SCP_STATUS
{
	FX_DC_OP_IDLE = 3,

	FX_DC_DEVICE_OTHER = 11,  
	FX_DC_DEVICE_UNKNOWN = 12,
	FX_DC_DEVICE_IDLE = 13,
	FX_DC_DEVICE_PRINTING = 14,
	FX_DC_DEVICE_WARMUP =15,
	FX_DC_DEVICE_SCANING =16,
	FX_DC_DEVICE_COPYING =17,
	FX_DC_DEVICE_FAXING =18,

	FX_DC_FIR_PAPER_OK = 21, 
	FX_DC_FIR_PAPER_LESS ,
	FX_DC_FIR_PAPER_NO,

	FX_DC_SEC_PAPER_OK = 31, 
	FX_DC_SEC_PAPER_LESS,
	FX_DC_SEC_PAPER_NO,

	FX_DC_FIR_INK_OK = 41,
	FX_DC_FIR_INK_LESS,
	FX_DC_FIR_INK_NO,

	FX_DC_ERR = 50,

	FX_DC_NET_ERR = 60,
	FX_DC_POWER_ON,		//��ӡ������
	FX_DC_POWER_OFF,	//��ӡ���ػ�
	FX_DC_POWER_SAVE,	//��ӡ���ڵ�

	FX_DC_ALERT = 70	//Ԥ����Ϣ
};

enum FX_DC_INK_CMYK
{
	FX_DC_INK_NULL = 0,		//��ī��
	FX_DC_INK_CYAN = 1,		//��
	FX_DC_INK_MAGENTA = 2,	//Ʒ��
	FX_DC_INK_YELLOW = 3,	//��
	FX_DC_INK_BLACK = 4		//��
};

class CSCPStatus
{
public:
	FX_DC_SCP_STATUS m_eOPStatus;
	FX_DC_SCP_STATUS m_eDeviceStatus;
	FX_DC_SCP_STATUS m_ePrintStatus;
	FX_DC_SCP_STATUS m_eCopyStatus;
	FX_DC_SCP_STATUS m_eScanStatus;
	FX_DC_SCP_STATUS m_eFaxStatus;
	FX_DC_SCP_STATUS m_eFirPaper;
	FX_DC_SCP_STATUS m_eSecPaper;
	FX_DC_SCP_STATUS m_eFirInk;
	FX_DC_SCP_STATUS m_ePowerStatus;	//��Դ״̬
	FX_DC_SCP_STATUS m_aryPaper[Max_Paper_Index];
	FX_DC_SCP_STATUS m_aryInk[Max_Ink_Index];
};


/*
SNMP��Ҫ��OID�������
һ,ֽ����Ϣ
1.ֽ�и���
2.ֽ��ֽ��
3.ֽ���������
4.ֽ��ʣ������

��,ī����Ϣ
1.ī������
2.ī����ɫ
3.ī���������
4.ī��ʣ������

��,�豸״̬
1.�豸�ĵ�ǰ����״̬,�����,Ԥ��,����,��ӡ��,ɨ���еȵ�.
2.�豸�ĵ�ǰӲ��״̬,�翨ֽ,ֽ�г��,ī�г��,ȱֽ,��ֽ,ȱī,��ī,�ǰ�򿪵ȵ�

��,��ҵ��Ϣ
1.��ǰ��ҵ��Ϣ
A.��ǰ��ҵ����
B.��ǰ��ҵ����
C.��ǰ��ҵ����״̬
D.��ǰ��ҵ����
E.��ǰ��ҵ�û���
F.��ǰ��ҵֽ����

2.��ʷ��ҵ��Ϣ
A.��ʷ��ҵ����
B.��ʷ��ҵ����
C.��ʷ��ҵ����״̬
D.��ʷ��ҵ����
E.��ʷ��ҵ�û���
F.��ʷ��ҵֽ����
G.��ʷ��ҵ���״̬
*/

//��ҵ����
enum SCP_JOB_TYPE
{
	SCP_JOB_TYPE_PRINT = 1,
	SCP_JOB_TYPE_COPY = 2,
	SCP_JOB_TYPE_SCAN = 3,
	SCP_JOB_TYPE_FAX = 4,
	SCP_JOB_TYPE_RECEIVE_JOB = 5,
	SCP_JOB_TYPE_SEND_JOB = 6,
	SCP_JOB_TYPE_MULTI = 7,
	SCP_JOB_TYPE_OTHER = 8
};

//��ҵ״̬
enum SCP_JOB_STATUS
{
	SCP_JOB_STATUS_START = 0,			//��ҵ��ʼ
	SCP_JOB_STATUS_CANCEL_REQUEST = 1,	//����ȡ��
	SCP_JOB_STATUS_PENDING = 2,			//�ȴ���
	SCP_JOB_STATUS_PROCESSING = 3,		//������
	SCP_JOB_STATUS_SENDING = 4,			//������
	SCP_JOB_STATUS_RECEIVING = 5,		//������
	SCP_JOB_STATUS_PRINTING = 6,		//��ӡ��
	SCP_JOB_STATUS_SCANNING = 7,		//ɨ����
	SCP_JOB_STATUS_PROCESING_STOPED = 8,	//������-����ֹ
	SCP_JOB_STATUS_COMPLETED = 9,		//�����
	SCP_JOB_STATUS_CAUTION = 10,		//����
	SCP_JOB_STATUS_ABORTED = 11,		//�ѷ���
	SCP_JOB_STATUS_CANCELED = 12,		//��ȡ��
	SCP_JOB_STATUS_SEND_COMPLETED = 13,	//�������
	SCP_JOB_STATUS_RECE_COMPLETED = 14	//�������
};

//��ҵ��Ϣ
typedef struct tag_SCP_JobInfo
{
	int				nJobId;			//��ҵID
	char			cJobName[MAX_PATH];	//��ҵ����
	double			fJobStartTime;	//��ҵ��ʼʱ��
	double			fJobEndTime;	//��ҵ����ʱ��	
	int				nTotalPage;		//��ҵ��ҳ��	
	int				nPage;			//��ҵ��������ҳ��	
	int				nCopies;		//��ҵ����
	int				nPrintedPage;	//��ҵ�Ѿ���ӡҳ��
	int				nScanedPage;	//��ҵ�Ѿ�ɨ��ҳ��
	int				nMediaSize;		//��ҵֽ��,��A4
	bool			bColor;			//��ҵ�Ƿ��ɫ
	SCP_JOB_TYPE	JobType;		//��ҵ����
	SCP_JOB_STATUS	JobStatus;		//��ҵ״̬
	char			cDesc[MAX_PATH];	//״̬˵��
	char			szPrintTime[20];	//��ҵʱ��
	char			szOid[MAX_PATH];	//��ҵ��OID
}SCP_JobInfo, *PSCP_JobInfo;
typedef list<SCP_JobInfo> SCP_JobInfo_List, *PSCP_JobInfo_List;
typedef SCP_JobInfo_List::iterator SCP_JobInfo_List_I;

typedef struct tag_PrintJobInfo
{
	int nIdentifier;
	int nDocPrintedPages;
	double dPrintTime;
	char szPrintTime[20];
	char szDocName[MAX_PATH];
	char szOid[MAX_PATH];

}PRINTJOBINFO, *PPRINTJOBINFO;

typedef vector<PRINTJOBINFO> VPRINTJOBINFO;
typedef VPRINTJOBINFO::iterator VPRINTJOBINFOI;

struct ST_SCANFILE
{
	TCHAR cFileName[MAX_PATH];
	TCHAR cUserName[128];
};

typedef struct tag_PrinterMeterInfo
{
	int nPrintId;			//����Ĵ�ӡ��id
	double fRecordPrintMeterTime; //�����ʱ��
	int nOpAdminId;			//�����������ԱID

	//�������Ӵ�ӡ��������������
	int nAllTotalPage;			//��ҳ��
	int nAllTotalDistance;		//�ܳ���
	int nA3HBTotalPage;			//A3�ڰ���ҳ��
	int nA3ColorTotalPage;		//A3��ɫ��ҳ��
	int nA4HBTotalPage;			//A4�ڰ���ҳ��
	int nA4ColorTotalPage;		//A4��ɫ��ҳ��

	//��ӡ����
	int nA4PrintTotalPage;	//A4��ӡ�ܼ���
	int nA4PrintColorPage;	//A4��ӡ��ɫ�ܼ���
	int nA4PrintHBPage;	    //A4��ӡ�ڰ��ܼ���
	int nA3PrintTotalPage;	//A3��ӡ�ܼ���
	int nA3PrintColorPage;	//A3��ӡ��ɫ�ܼ���
	int nA3PrintHBPage;	    //A3��ӡ�ڰ��ܼ���
	int nOtherPrintTotalPage;	//����ֽ�ʹ�ӡ�ܼ���
	int nOtherPrintColorPage;	//����ֽ�ʹ�ӡ��ɫ�ܼ���
	int nOtherPrintHBPage;		//����ֽ�ʹ�ӡ�ڰ��ܼ���

	int nPrintDistance; //��ӡ����
	int nPrintArea;		//��ӡ���


	//��ӡ����
	int nA4CopyTotalPage;	//A4��ӡ�ܼ���
	int nA4CopyColorPage;	//A4��ӡ��ɫ�ܼ���
	int nA4CopyHBPage;	    //A4��ӡ�ڰ��ܼ���
	int nA3CopyTotalPage;	//A3��ӡ�ܼ���
	int nA3CopyColorPage;	//A3��ӡ��ɫ�ܼ���
	int nA3CopyHBPage;	    //A3��ӡ�ڰ��ܼ���
	int nOtherCopyTotalPage;	//����ֽ�͸�ӡ�ܼ���
	int nOtherCopyColorPage;	//����ֽ�͸�ӡ��ɫ�ܼ���
	int nOtherCopyHBPage;	//����ֽ�͸�ӡ�ڰ��ܼ���

	int nCopyDistance;			//��ӡ����
	int nCopyArea;				//��ӡ���

	//ɨ�����
	int nScanTotalPage;			//ɨ���ܼ���
	int nScanBigColorPage;		//ɨ������ɫ�ܼ���
	int nScanBigHBPage;			//ɨ�����ڰ��ܼ���
	int nScanSmallColorPage;	//ɨ��С����ɫ�ܼ���
	int nScanSmallHBPage;		//ɨ��С���ڰ��ܼ���

	//�������
	int nA4FaxTotalPage;		//A4�����ܼ���
	int nA4FaxColorPage;		//A4�����ɫ�ܼ���
	int nA4FaxHBPage;	    //A4����ڰ��ܼ���
	int nA3FaxTotalPage;		//A3�����ܼ���
	int nA3FaxColorPage;		//A3�����ɫ�ܼ���
	int nA3FaxHBPage;	    //A3����ڰ��ܼ���
	int nOtherFaxTotalPage;	//����ֽ�ʹ����ܼ���
	int nOtherFaxColorPage;	//����ֽ�ʹ����ɫ�ܼ���
	int nOtherFaxHBPage;		//����ֽ�ʹ���ڰ��ܼ���

	//������������
	int nOtherOpTotalPage;	//���������ܼ���
	int nOtherOpColorPage;	//����������ɫ�ܼ���
	int nOtherOpHBPage;	    //���������ڰ��ܼ���

	//������Ϊ���е�Ч�����Ļ�������
	int nEquivalentHBPrintPage; //��Ч�ڰ״�ӡ����
	int nEquivalentHBCopyPage;	//��Ч�ڰ׸�ӡ����
	int nEquivalentHBFaxPage;	//��Ч�ڰ״������
	int nEquivalentColorPrintPage; //��Ч��ɫ��ӡ����
	int nEquivalentColorCopyPage;	//��Ч��ɫ��ӡ����
	int nEquivalentColorFaxPage;	//��Ч��ɫ�������

	//ֽ����Ϣ
	int nPaper1MaxCapacity;
	int nPaper1CurrentLevel;
	char cPaper1LevelPercent[50];
	int nPaper2MaxCapacity;
	int nPaper2CurrentLevel;
	char cPaper2LevelPercent[50];
	int nPaper3MaxCapacity;
	int nPaper3CurrentLevel;
	char cPaper3LevelPercent[50];
	int nPaper4MaxCapacity;
	int nPaper4CurrentLevel;
	char cPaper4LevelPercent[50];
	int nPaperBypassMaxCapacity;
	int nPaperBypassCurrentLevel;
	char cPaperBypassLevelPercent[50];

	//ī�з���Ϣ:Cyan\Magenta\Yellow\Black[��ɫ��Ʒ�졢�ơ���]
	int nTonerCyanMaxCapacity;
	int nTonerCyanLevel;
	char cTonerCyanLevelPercent[50];
	int nTonerMagentaMaxCapacity;
	int nTonerMagentaLevel;
	char cTonerMagentaLevelPercent[50];
	int nTonerYellowMaxCapacity;
	int nTonerYellowLevel;
	char cTonerYellowLevelPercent[50];
	int nTonerBlackMaxCapacity;
	int nTonerBlackLevel;
	char cTonerBlackLevelPercent[50];

	//�й�ĵ�Ԫ:OPC
	int nOPCCyanMaxCapacity;
	int nOPCCyanLevel;
	char cOPCCyanLevelPercent[50];
	int nOPCMagentaMaxCapacity;
	int nOPCMagentaLevel;
	char cOPCMagentaLevelPercent[50];
	int nOPCYellowMaxCapacity;
	int nOPCYellowLevel;
	char cOPCYellowLevelPercent[50];
	int nOPCBlackMaxCapacity;
	int nOPCBlackLevel;
	char cOPCBlackLevelPercent[50];

	//��Ӱ��Ԫ:Developer
	int nDeveloperCyanMaxCapacity;
	int nDeveloperCyanLevel;
	char cDeveloperCyanLevelPercent[50];
	int nDeveloperMagentaMaxCapacity;
	int nDeveloperMagentaLevel;
	char cDeveloperMagentaLevelPercent[50];
	int nDeveloperYellowMaxCapacity;
	int nDeveloperYellowLevel;
	char cDeveloperYellowLevelPercent[50];
	int nDeveloperBlackMaxCapacity;
	int nDeveloperBlackLevel;
	char cDeveloperBlackLevelPercent[50];
}PrinterMeterInfo, *PPrinterMeterInfo;

class ISCPStatusInterface
{
public:
	virtual void OnStatus(int, TCHAR*){ASSERT(0);}
	virtual void OnSCPIdle(int nSec){ASSERT(0);}
	virtual void OnMeterInfo(PPrinterMeterInfo){ASSERT(0);}
	virtual void OnJobInfo(PSCP_JobInfo){ASSERT(0);}
	virtual void OnJobInfoList(PSCP_JobInfo_List){ASSERT(0);}
};

class _AA_DLL_EXPORT_ CSnmpOP
{
public:
	CSnmpOP();
	~CSnmpOP();

protected:
	char* SNMP_AnyToStr(AsnObjectSyntax *sAny, UINT& uLen);	//uLen:�ַ������ȣ�����'\0'
	char* SNMP_AnyToHexStr(AsnObjectSyntax *sAny, UINT& uLen);	//uLen:�ַ������ȣ�����'\0'
	BOOL GetRequest(char* pszOid,int& nVal);
	BOOL GetRequest(char* pszOid,unsigned int& nVal);
	BOOL GetRequest(char* pszOid,unsigned char* pszStr,int nStrBufferLen);
	BOOL GetHexRequest(char* pszOid,unsigned char* pszStr,int nStrBufferLen);
	BOOL GetNextRequest(char* pszOid,int& nVal);
	BOOL GetRequestStr(char* pszOid,unsigned char* pszStr,int& nStrBufferLen);//nStrBufferLen:�ַ������ȣ�����'\0'
	BOOL GetNextRequestStr(char* pszOid,unsigned char* pszStr,int& nStrBufferLen);//nStrBufferLen:�ַ������ȣ�����'\0'
	BOOL GetNextRequestStrEx(char* pszOid,char* pszStr,int nStrBufferLen,char* pszOidNext,int nOidNextBufferLen);
	int GetOidEndNumber(char* pszOid);
	BOOL OidBeginWithStr(char* pszOid, char* pszStr);
	void TimeTicksToDateTimeStr(TimeTicks timeticks, char* pszDateTimeStr);
	CString GetStrByOid(CStringA szOid);
	BOOL CheckIPActive();
	BOOL CheckSupportSnmp();
	CStringA EncryptOID(IN CString szOrgString);
	CStringA DecryptOID(IN CString szEncryptString);
	char* CStringToChar(CStringA& szString);
	wchar_t* CStringToChar(CStringW& szString);

public:
	void SetConnect(CString szIP, CString szCommunity = _T("public"));
	BOOL InitSnmp();
	void ReleaseSnmp();
	void SetCallBack(ISCPStatusInterface* pCallBack);
	CString GetPrtIP();
	void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen );  

protected:
	BOOL m_bIsActiveIP;
	BOOL m_bIsSupportSnmp;
	ISCPStatusInterface* m_pCallBack;
	LPSNMP_MGR_SESSION m_lpMgrSession;
	CString	m_szIP;
	CString m_szCommunity;
};

class CSnmpJobMonitor : public CSnmpOP
{
public:
	CSnmpJobMonitor();
	virtual ~CSnmpJobMonitor();
	virtual void CheckJob() = NULL;
	virtual int CheckLastPrintCount() = NULL;
	virtual int CheckLastPrintCount_Color() = NULL;
	virtual int CheckLastPrintCount_WB() = NULL;
	virtual int CheckLastCopyCount() = NULL;
	virtual int CheckLastCopyCount_Color() = NULL;
	virtual int CheckLastCopyCount_WB() = NULL;
	virtual int CheckLastScanCount() = NULL;
	virtual int CheckLastScanCount_Color() = NULL;
	virtual int CheckLastScanCount_WB() = NULL;
	virtual int CheckLastFaxCount() = NULL;
	virtual int CheckLastFaxCount_Color() = NULL;
	virtual int CheckLastFaxCount_WB() = NULL;
	virtual int CheckLastPageCount_A3() = NULL;
	virtual int CheckLastPageCount_A4() = NULL;
	void Release();
protected:
	typedef CMap<int, int, PSCP_JobInfo, PSCP_JobInfo> CJobMap;
	CJobMap m_oJobMap;
	typedef CList<PSCP_JobInfo> CJobList;
	CJobList m_oJobList;
	int m_nLastPrintCount;
	int m_nLastPrintCount_Color;
	int m_nLastPrintCount_WB;
	int m_nLastCopyCount;
	int m_nLastCopyCount_Color;
	int m_nLastCopyCount_WB;
	int m_nLastScanCount;
	int m_nLastScanCount_Color;
	int m_nLastScanCount_WB;
	int m_nLastFaxCount;
	int m_nLastFaxCount_Color;
	int m_nLastFaxCount_WB;
	int m_nA3PageCount;
	int m_nA4PageCount;
};

//��ӡ��������
class CSnmpPrinterMeterMonitor : public CSnmpOP
{
public:
	CSnmpPrinterMeterMonitor();
	virtual ~CSnmpPrinterMeterMonitor();
	void CheckMeterInfo();
	BOOL GetLastMeterInfo(PrinterMeterInfo* pInfo);	//��ȡ���һ����ȷ�ĳ�����Ϣ
	void SetMeterInfo(PrtInputEntryMap& map);
	void SetMeterInfo(PrtMarkerSuppliesEntryMap& map);
	void SetBrandType(BRAND_TYPE eType);
	void Release();

protected:
	virtual BOOL GetMeterInfo();		//������ֻ����CheckMeterInfoֻ�ڲ�����
	void InitMeter();
	void SetPaperMeter(PrinterMeterInfo& oMeterInfo);
	BOOL CheckMeterValid();
	BOOL IsMeterValid(PrinterMeterInfo& oMeterInfo);
	BRAND_TYPE m_eType;	//Ʒ������
	PrinterMeterInfo m_oMeterInfo;
	PrinterMeterInfo m_oLastMeterInfo;	//���һ����ȷ�ĳ�����Ϣ
	CCriticalSection2 m_cs4MeterInfo;
	BOOL m_bFirstSucc;	//��һ�γ���ɹ���ʶ

protected:
	void InitOID();
	CStringA m_szPrtMarkerLifeCountOID;
};

//��ӡ��������
class CSnmpMonitorHelper : public CSnmpOP
{
public:
	CSnmpMonitorHelper();
	virtual ~CSnmpMonitorHelper();

public:
	void Init();
	void Release();
	void SetPrinterMeterMonitor(CSnmpPrinterMeterMonitor* pMeter);
	void SetPaperConfig(CString sConfig, int nMin);
	void SetInkConfig(CString sConfig, int nMin);
	void CheckMonitor();
	void SetBrandType(BRAND_TYPE eType);

protected:
	//ͨ��SNMP��ȡOID��Ϣ
	virtual BOOL GetAll();
	virtual BOOL GetGeneral();
	virtual BOOL GetStorageRef();
	virtual BOOL GetDeviceRef();
	virtual BOOL GetCover();
	virtual BOOL GetLocalization();
	virtual BOOL GetInput();
	virtual BOOL GetOutput();
	virtual BOOL GetMarker();
	virtual BOOL GetMarkerSupplies();
	virtual BOOL GetMarkerColorant();
	virtual BOOL GetMarkerPath();
	virtual BOOL GetChannel();
	virtual BOOL GetInterpreter();
	virtual BOOL GetConsoleDisplayBuffer();
	virtual BOOL GetConsoleLight();
	virtual BOOL GetAlert();

	//���SNMP��ȡ����OID��Ϣ
	virtual void CheckAll();
	virtual void CheckGeneral();
	virtual void CheckStorageRef();
	virtual void CheckDeviceRef();
	virtual void CheckCover();
	virtual void CheckLocalization();
	virtual void CheckInput();
	virtual void CheckOutput();
	virtual void CheckMarker();
	virtual void CheckMarkerSupplies();
	virtual void CheckMarkerColorant();
	virtual void CheckMarkerPath();
	virtual void CheckChannel();
	virtual void CheckInterpreter();
	virtual void CheckConsoleDisplayBuffer();
	virtual void CheckConsoleLight();
	virtual void CheckAlert();

	//����SNMP��ȡ����OID��Ϣ
	virtual void ClearAll();
	virtual void ClearGeneral();
	virtual void ClearStorageRef();
	virtual void ClearDeviceRef();
	virtual void ClearCover();
	virtual void ClearLocalization();
	virtual void ClearInput();
	virtual void ClearOutput();
	virtual void ClearMarker();
	virtual void ClearMarkerSupplies();
	virtual void ClearMarkerColorant();
	virtual void ClearMarkerPath();
	virtual void ClearChannel();
	virtual void ClearInterpreter();
	virtual void ClearConsoleDisplayBuffer();
	virtual void ClearConsoleLight();
	virtual void ClearAlert();

	//��չ���ܼ��,��PrintMIB�ʹ�ӡ��˽��MIB�Ļ�����,������չ���ܵķ���
	virtual void CheckMeter();		//��չ����(�Ǳ�׼PrintMib)������
	virtual void CheckAlertEx();	//��չ����(�Ǳ�׼PrintMib)�����˹��ϴ��������Ԥ����Ϣ
	virtual BOOL IsFalutInfo(CString szDetial);	//�ж���Ϣ�Ƿ�Ϊ������Ϣ
	virtual CString GetFaultCode(CString szDetial);	//��ȡ���ϴ���
	virtual void CheckFaultCode();	//�����Ĺ��ϴ����⣬�Ӵ�ӡ����˽�нڵ��ȡ

protected:
	//��ȡֽ��ֽ��
	CString GetPaperTypeByPrtInputEntry(PPrtInputEntry pEntry);
	void InitOID();
	
protected:
	//��Ԫ��ӡ��������Ϣ��
	CSnmpPrinterMeterMonitor* m_pPrinterMeterMonitor;
	//����ͨ��SNMP��ȡOID��Ϣ��MAP
	PrtGeneralEntryMap m_oGeneralMap;
	PrtStorageRefEntryMap m_oStorageRefMap;
	PrtDeviceRefEntryMap m_oDeviceRefMap;
	PrtCoverEntryMap m_oCoverMap;
	PrtLocalizationEntryMap m_oLocalizationMap;
	PrtInputEntryMap m_oInputMap;
	PrtOutputEntryMap m_oOutputMap;
	PrtMarkerEntryMap m_oMarkerMap;
	PrtMarkerSuppliesEntryMap m_oMarkerSuppliesMap;
	PrtMarkerColorantEntryMap m_oMarkerColorantMap;
	PrtMediaPathEntryMap m_oMarkerPathMap;
	PrtChannelEntryMap m_oChannelMap;
	PrtInterpreterEntryMap m_oInterpreterMap;
	PrtConsoleDisplayBufferEntryMap m_oConsoleDisplayBufferMap;
	PrtConsoleLightEntryMap m_oConsoleLightMap;
	PrtAlertEntryMap m_oAlertMap;

	//ͨ��SNMP��ȡOID��Ϣ��MAP���߳���
// 	CCriticalSection2 m_cs4GeneralMap;
// 	CCriticalSection2 m_cs4StorageRefMap;
// 	CCriticalSection2 m_cs4DeviceRefMap;
// 	CCriticalSection2 m_cs4CoverMap;
// 	CCriticalSection2 m_cs4LocalizationMap;
// 	CCriticalSection2 m_cs4InputMap;
// 	CCriticalSection2 m_cs4OutputMap;
// 	CCriticalSection2 m_cs4MarkerMap;
// 	CCriticalSection2 m_cs4MarkerSuppliesMap;
// 	CCriticalSection2 m_cs4MarkerColorantMap;
// 	CCriticalSection2 m_cs4MarkerPathMap;
// 	CCriticalSection2 m_cs4ChannelMap;
// 	CCriticalSection2 m_cs4InterpreterMap;
// 	CCriticalSection2 m_cs4ConsoleDisplayBufferMap;
// 	CCriticalSection2 m_cs4ConsoleLightMap;
// 	CCriticalSection2 m_cs4AlertMap;

	//ֽ������
	CString m_szPaperConfig;
	int m_nPaperMinRatio;
	//ī������
	CString m_szInkConfig;
	int m_nInkMinRatio;
	//Ʒ������
	BRAND_TYPE m_eType;

	//OIDs
	//printmib.prtGeneral.prtGeneralTable.prtGeneralEntry
	CStringA m_szPrtGeneralEntryOID;
	CStringA m_szPrtGeneralConfigChangesOID;
	CStringA m_szPrtGeneralCurrentLocalizationOID;
	CStringA m_szPrtGeneralResetOID;
	CStringA m_szPrtGeneralCurrentOperatorOID;
	CStringA m_szPrtGeneralServicePersonOID;
	CStringA m_szPrtInputDefaultIndexOID;
	CStringA m_szPrtOutputDefaultIndexOID;
	CStringA m_szPrtMarkerDefaultIndexOID;
	CStringA m_szPrtMediaPathDefaultIndexOID;
	CStringA m_szPrtConsoleLocalizationOID;
	CStringA m_szPrtConsoleNumberOfDisplayLinesOID;
	CStringA m_szPrtConsoleNumberOfDisplayCharsOID;
	CStringA m_szPrtConsoleDisableOID;
	CStringA m_szPrtAuxiliarySheetStartupPageOID;
	CStringA m_szPrtAuxiliarySheetBannerPageOID;
	CStringA m_szPrtGeneralPrinterNameOID;
	CStringA m_szPrtGeneralSerialNumberOID;
	CStringA m_szPrtAlertCriticalEventsOID;
	CStringA m_szPrtAlertAllEventsOID;

	//printmib.prtCover.prtCoverTable.prtCoverEntry
	CStringA m_szPrtCoverEntryOID;
	CStringA m_szPrtCoverIndexOID;
	CStringA m_szPrtCoverDescriptionOID;
	CStringA m_szPrtCoverStatusOID;

	//printmib.prtLocalization.prtLocalizationTable.prtLocalizationEntry
	CStringA m_szPrtLocalizationEntryOID;
	CStringA m_szPrtLocalizationIndexOID;
	CStringA m_szPrtLocalizationLanguageOID;
	CStringA m_szPrtLocalizationCountryOID;
	CStringA m_szPrtLocalizationCharacterSetOID;

	//printmib.prtInput.prtInputTable.prtInputEntry
	CStringA m_szPrtInputEntryOID;
	CStringA m_szPrtInputIndexOID;
	CStringA m_szPrtInputTypeOID;
	CStringA m_szPrtInputDimUnitOID;
	CStringA m_szPrtInputMediaDimFeedDirDeclaredOID;
	CStringA m_szPrtInputMediaDimXFeedDirDeclaredOID;
	CStringA m_szPrtInputMediaDimFeedDirChosenOID;
	CStringA m_szPrtInputMediaDimXFeedDirChosenOID;
	CStringA m_szPrtInputCapacityUnitOID;
	CStringA m_szPrtInputMaxCapacityOID;
	CStringA m_szPrtInputCurrentLevelOID;
	CStringA m_szPrtInputStatusOID;
	CStringA m_szPrtInputMediaNameOID;
	CStringA m_szPrtInputNameOID;
	CStringA m_szPrtInputVendorNameOID;
	CStringA m_szPrtInputModelOID;
	CStringA m_szPrtInputVersionOID;
	CStringA m_szPrtInputSerialNumberOID;
	CStringA m_szPrtInputDescriptionOID;
	CStringA m_szPrtInputSecurityOID;
	CStringA m_szPrtInputMediaWeightOID;
	CStringA m_szPrtInputMediaTypeOID;
	CStringA m_szPrtInputMediaColorOID;
	CStringA m_szPrtInputMediaFormPartsOID;
#ifdef RFC_3805
	CStringA m_szPrtInputMediaLoadTimeoutOID;
	CStringA m_szPrtInputNextIndexOID;
#endif

	//printmib.prtOutput.prtOutputTable.prtOutputEntry
	CStringA m_szPrtOutputEntryOID;
	CStringA m_szPrtOutputIndexOID;
	CStringA m_szPrtOutputTypeOID;
	CStringA m_szPrtOutputCapacityUnitOID;
	CStringA m_szPrtOutputMaxCapacityOID;
	CStringA m_szPrtOutputRemainingCapacityOID;
	CStringA m_szPrtOutputStatusOID;
	CStringA m_szPrtOutputNameOID;
	CStringA m_szPrtOutputVendorNameOID;
	CStringA m_szPrtOutputModelOID;
	CStringA m_szPrtOutputVersionOID;
	CStringA m_szPrtOutputSerialNumberOID;
	CStringA m_szPrtOutputDescriptionOID;
	CStringA m_szPrtOutputSecurityOID;
	CStringA m_szPrtOutputDimUnitOID;
	CStringA m_szPrtOutputMaxDimFeedDirOID;
	CStringA m_szPrtOutputMaxDimXFeedDirOID;
	CStringA m_szPrtOutputMinDimFeedDirOID;
	CStringA m_szPrtOutputMinDimXFeedDirOID;
	CStringA m_szPrtOutputStackingOrderOID;
	CStringA m_szPrtOutputPageDeliveryOrientationOID;
	CStringA m_szPrtOutputBurstingOID;
	CStringA m_szPrtOutputDecollatingOID;
	CStringA m_szPrtOutputPageCollatedOID;
	CStringA m_szPrtOutputOffsetStackingOID;

	//printmib.prtMarker.prtMarkerTable.prtMarkerEntry
	CStringA m_szPrtMarkerEntryOID;
	CStringA m_szPrtMarkerIndexOID;
	CStringA m_szPrtMarkerMarkTechOID;
	CStringA m_szPrtMarkerCounterUnitOID;
	CStringA m_szPrtMarkerLifeCountOID;
	CString m_szPrtMarkerPowerOnCount;
	CStringA m_szPrtMarkerProcessColorantsOID;
	CStringA m_szPrtMarkerSpotColorantsOID;
	CStringA m_szPrtMarkerAddressabilityUnitOID;
	CStringA m_szPrtMarkerAddressabilityFeedDirOID;
	CStringA m_szPrtMarkerAddressabilityXFeedDirOID;
	CStringA m_szPrtMarkerNorthMarginOID;
	CStringA m_szPrtMarkerSouthMarginOID;
	CStringA m_szPrtMarkerWestMarginOID;
	CStringA m_szPrtMarkerEastMarginOID;
	CStringA m_szPrtMarkerStatusOID;

	//printmib.prtMarkerSupplies.prtMarkerSuppliesTable.prtMarkerSuppliesEntry
	CStringA m_szPrtMarkerSuppliesEntryOID;
	CStringA m_szPrtMarkerSuppliesIndexOID;
	CStringA m_szPrtMarkerSuppliesMarkerIndexOID;
	CStringA m_szPrtMarkerSuppliesColorantIndexOID;
	CStringA m_szPrtMarkerSuppliesClassOID;
	CStringA m_szPrtMarkerSuppliesTypeOID;
	CStringA m_szPrtMarkerSuppliesDescriptionOID;
	CStringA m_szPrtMarkerSuppliesSupplyUnitOID;
	CStringA m_szPrtMarkerSuppliesMaxCapacityOID;
	CStringA m_szPrtMarkerSuppliesLevelOID;

	//printmib.prtMarkerColorant.prtMarkerColorantTable.prtMarkerColorantEntry
	CStringA m_szPrtMarkerColorantEntryOID;
	CStringA m_szPrtMarkerColorantIndexOID;
	CStringA m_szPrtMarkerColorantMarkerIndexOID;
	CStringA m_szPrtMarkerColorantRoleOID;
	CStringA m_szPrtMarkerColorantValueOID;
	CStringA m_szPrtMarkerColorantTonalityOID;

	//printmib.prtMediaPath.prtMediaPathTable.prtMediaPathEntry
	CStringA m_szPrtMediaPathEntryOID;
	CStringA m_szPrtMediaPathIndexOID;
	CStringA m_szPrtMediaPathMaxSpeedPrintUnitOID;
	CStringA m_szPrtMediaPathMediaSizeUnitOID;
	CStringA m_szPrtMediaPathMaxSpeedOID;
	CStringA m_szPrtMediaPathMaxMediaFeedDirOID;
	CStringA m_szPrtMediaPathMaxMediaXFeedDirOID;
	CStringA m_szPrtMediaPathMinMediaFeedDirOID;
	CStringA m_szPrtMediaPathMinMediaXFeedDirOID;
	CStringA m_szPrtMediaPathTypeOID;
	CStringA m_szPrtMediaPathDescriptionOID;
	CStringA m_szPrtMediaPathStatusOID;

	//printmib.prtChannel.prtChannelTable.prtChannelEntry
	CStringA m_szPrtChannelEntryOID;
	CStringA m_szPrtChannelIndexOID;
	CStringA m_szPrtChannelTypeOID;
	CStringA m_szPrtChannelProtocolVersionOID;
	CStringA m_szPrtChannelCurrentJobCntlLangIndexOID;
	CStringA m_szPrtChannelDefaultPageDescLangIndexOID;
	CStringA m_szPrtChannelStateOID;
	CStringA m_szPrtChannelIfIndexOID;
	CStringA m_szPrtChannelStatusOID;
	CStringA m_szPrtChannelInformationOID;

	//printmib.prtInterpreter.prtInterpreterTable.prtInterpreterEntry
	CStringA m_szPrtInterpreterEntryOID;
	CStringA m_szPrtInterpreterIndexOID;
	CStringA m_szPrtInterpreterLangFamilyOID;
	CStringA m_szPrtInterpreterLangLevelOID;
	CStringA m_szPrtInterpreterLangVersionOID;
	CStringA m_szPrtInterpreterDescriptionOID;
	CStringA m_szPrtInterpreterVersionOID;
	CStringA m_szPrtInterpreterDefaultOrientationOID;
	CStringA m_szPrtInterpreterFeedAddressabilityOID;
	CStringA m_szPrtInterpreterXFeedAddressabilityOID;
	CStringA m_szPrtInterpreterDefaultCharSetInOID;
	CStringA m_szPrtInterpreterDefaultCharSetOutOID;
	CStringA m_szPrtInterpreterTwoWayOID;

	//printmib.prtConsoleDisplayBuffer.prtConsoleDisplayBufferTable.prtConsoleDisplayBufferEntry
	CStringA m_szPrtConsoleDisplayBufferEntryOID;
	CStringA m_szPrtConsoleDisplayBufferIndexOID;
	CStringA m_szPrtConsoleDisplayBufferTextOID;

	//printmib.prtConsoleLight.prtConsoleLightTable.prtConsoleLightEntry
	CStringA m_szPrtConsoleLightEntryOID;
	CStringA m_szPrtConsoleLightIndexOID;
	CStringA m_szPrtConsoleOnTimeOID;
	CStringA m_szPrtConsoleOffTimeOID;
	CStringA m_szPrtConsoleColorOID;
	CStringA m_szPrtConsoleDescriptionOID;

	//printmib.prtAlert.prtAlertTable.prtAlertEntry
	CStringA m_szPrtAlertEntryOID;
	CStringA m_szPrtAlertIndexOID;
	CStringA m_szPrtAlertSeverityLevelOID;
	CStringA m_szPrtAlertTrainingLevelOID;
	CStringA m_szPrtAlertGroupOID;
	CStringA m_szPrtAlertGroupIndexOID;
	CStringA m_szPrtAlertLocationOID;
	CStringA m_szPrtAlertCodeOID;
	CStringA m_szPrtAlertDescriptionOID;
	CStringA m_szPrtAlertTimeOID;
};

#endif //_Snmp_Interface_h_27E39E90_744D_4560_95FD_F5C6B2BBC9A6_