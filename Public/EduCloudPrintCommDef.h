/************************************************************************/
/* 本文件最后修改日期：2016-09-20                                       */
/************************************************************************/

#ifndef _EduCloudPrintRouterTmepDef_h_B9ED834F_A02F_4996_89E6_559841D7280D_
#define _EduCloudPrintRouterTmepDef_h_B9ED834F_A02F_4996_89E6_559841D7280D_

//保证宏在多平台(linux,windows等等)下通用
#ifndef WM_USER
#define WM_USER	0x0400
#endif

#ifndef	TEXT
#define TEXT(quote) quote
#endif

#ifndef MAX_PATH
#define MAX_PATH	260
#endif

#ifndef MAXIMUM_WAIT_OBJECTS
#define MAXIMUM_WAIT_OBJECTS 64     // Maximum number of wait objects
#endif
//
#if 0
#define WM_EDU_CLOUD_BASE							(WM_USER + 10000)

#//define WM_MSG_QUE_OK								WM_EDU_CLOUD_BASE + 1
//#define WM_MSG_MAIN_PROCESS						WM_EDU_CLOUD_BASE + 2


#define WM_MSG_PRINT_REQ						WM_EDU_CLOUD_BASE + 101
#define WM_MSG_PRINT_BEGIN						WM_EDU_CLOUD_BASE + 102
#define WM_MSG_PRINT_PROCESS					WM_EDU_CLOUD_BASE + 103
#define WM_MSG_PRINT_INFO						WM_EDU_CLOUD_BASE + 104
#define WM_MSG_PRINT_END						WM_EDU_CLOUD_BASE + 105
#define WM_MSG_PRINT_ONE_BEGIN					WM_EDU_CLOUD_BASE + 106
#define WM_MSG_PRINT_ONE_PROCESS				WM_EDU_CLOUD_BASE + 107
#define WM_MSG_PRINT_ONE_INFO					WM_EDU_CLOUD_BASE + 108
#define WM_MSG_PRINT_ONE_END					WM_EDU_CLOUD_BASE + 109
#define WM_MSG_PRINT_JOB_HISTORY_LIST			WM_EDU_CLOUD_BASE + 110

#define WM_MSG_COPY_REQ							WM_EDU_CLOUD_BASE + 121
#define WM_MSG_COPY_BEGIN						WM_EDU_CLOUD_BASE + 122
#define WM_MSG_COPY_PROCESS						WM_EDU_CLOUD_BASE + 123
#define WM_MSG_COPY_INFO						WM_EDU_CLOUD_BASE + 124
#define WM_MSG_COPY_END							WM_EDU_CLOUD_BASE + 125
#define WM_MSG_COPY_ONE_BEGIN					WM_EDU_CLOUD_BASE + 126
#define WM_MSG_COPY_ONE_PROCESS					WM_EDU_CLOUD_BASE + 127
#define WM_MSG_COPY_ONE_INFO					WM_EDU_CLOUD_BASE + 128
#define WM_MSG_COPY_ONE_END						WM_EDU_CLOUD_BASE + 129
#define WM_MSG_COPY_JOB_HISTORY_LIST			WM_EDU_CLOUD_BASE + 130

#define WM_MSG_SCAN_REQ							WM_EDU_CLOUD_BASE + 141
#define WM_MSG_SCAN_BEGIN						WM_EDU_CLOUD_BASE + 142
#define WM_MSG_SCAN_PROCESS						WM_EDU_CLOUD_BASE + 143
#define WM_MSG_SCAN_INFO						WM_EDU_CLOUD_BASE + 144
#define WM_MSG_SCAN_END							WM_EDU_CLOUD_BASE + 145
#define WM_MSG_SCAN_ONE_BEGIN					WM_EDU_CLOUD_BASE + 146
#define WM_MSG_SCAN_ONE_PROCESS					WM_EDU_CLOUD_BASE + 147
#define WM_MSG_SCAN_ONE_INFO					WM_EDU_CLOUD_BASE + 148
#define WM_MSG_SCAN_ONE_END						WM_EDU_CLOUD_BASE + 149
#define WM_MSG_SCAN_JOB_HISTORY_LIST			WM_EDU_CLOUD_BASE + 150

#define WM_MSG_FAX_REQ							WM_EDU_CLOUD_BASE + 161
#define WM_MSG_FAX_BEGIN						WM_EDU_CLOUD_BASE + 162
#define WM_MSG_FAX_PROCESS						WM_EDU_CLOUD_BASE + 163
#define WM_MSG_FAX_INFO							WM_EDU_CLOUD_BASE + 164
#define WM_MSG_FAX_END							WM_EDU_CLOUD_BASE + 165
#define WM_MSG_FAX_ONE_BEGIN					WM_EDU_CLOUD_BASE + 166
#define WM_MSG_FAX_ONE_PROCESS					WM_EDU_CLOUD_BASE + 167
#define WM_MSG_FAX_ONE_INFO						WM_EDU_CLOUD_BASE + 168
#define WM_MSG_FAX_ONE_END						WM_EDU_CLOUD_BASE + 169
#define WM_MSG_FAX_JOB_HISTORY_LIST				WM_EDU_CLOUD_BASE + 170

#define WM_MSG_USER_LOGON						WM_EDU_CLOUD_BASE + 201
#define WM_MSG_USER_LOGOFF						WM_EDU_CLOUD_BASE + 202
#define WM_MSG_ACCOUNT_INFO						WM_EDU_CLOUD_BASE + 203
#define WM_MSG_PRINT_STATION_INFO				WM_EDU_CLOUD_BASE + 204
#define WM_MSG_PRINT_JOB_LIST					WM_EDU_CLOUD_BASE + 205		//打印作业列表
#define WM_MSG_PRINTER_METER_CONFIG				WM_EDU_CLOUD_BASE + 206		//打印机抄表配置信息
#define WM_MSG_REQUEST_PRINTER_METER_INFO		WM_EDU_CLOUD_BASE + 207		//请求打印机抄表信息
#define WM_MSG_PRINTER_FUNCTIONAL_CHANGES		WM_EDU_CLOUD_BASE + 208		//打印机功能变更


#define WM_MSG_CLOUD_DATA_SYNC					WM_EDU_CLOUD_BASE + 301




#define WM_SearchNetPrinter_Start				WM_EDU_CLOUD_BASE + 401	//开启网络打印机搜索
#define WM_SearchNetPrinter_Stop				WM_EDU_CLOUD_BASE + 402	//停止网络打印机搜索
#define WM_SearchLocalPrinter_Start				WM_EDU_CLOUD_BASE + 403	//开启本地打印机搜索
#define WM_SearchLocalPrinter_Stop				WM_EDU_CLOUD_BASE + 404	//停止本地打印机搜索

#define WM_PrinterMgr_Add						WM_EDU_CLOUD_BASE + 411
#define WM_PrinterMgr_Remove					WM_EDU_CLOUD_BASE + 412
#define WM_PrinterMgr_Update					WM_EDU_CLOUD_BASE + 413

#define WM_CloudPrintRouter_Update				WM_EDU_CLOUD_BASE + 421	//云打印网关更新

//中心服务器与网关的命令
#define WM_MfpUserLogin							WM_EDU_CLOUD_BASE + 1001 //用户登录打印机
#define WM_MfpUserLogOut						WM_EDU_CLOUD_BASE + 1002 //用户登出打印机
#define WM_MfpPrintJobList						WM_EDU_CLOUD_BASE + 1003 //用户提交打印作业到打印点
#define WM_PrintStationInfoChange				WM_EDU_CLOUD_BASE + 1004 //打印点信息
#define WM_RequestPrinterMeterInfo				WM_EDU_CLOUD_BASE + 1005 //请求打印点抄表信息
#define WM_RouterEntChange						WM_EDU_CLOUD_BASE + 1006 //打印网关企业变更
#define WM_DeletePrintStation					WM_EDU_CLOUD_BASE + 1007 //删除打印点
#define WM_RouterCommonCmd						WM_EDU_CLOUD_BASE + 1008 //网关与服务器的通用交互命令

//中心服务器与客户端连接时的验证命令
#define WM_CloudRouterLogin						WM_EDU_CLOUD_BASE + 1101 //云网关登录
#define WM_WebInfNotify							WM_EDU_CLOUD_BASE + 1102 //WebInf通知

#endif

#define QR_Code_Dir								TEXT("QR_Code")		//二维码保存路径
#define Print_Cache								TEXT("Print_Cache")	//打印保存总路径
#define Copy_Cache								TEXT("Copy_Cache")	//复印保存总路径
#define Scan_Cache								TEXT("Scan_Cache")	//扫描保存总路径
#define Fax_Cache								TEXT("Fax_Cache")	//传真保存总路径
#define Update_Dir								TEXT("Update_Cache")	//云打印升级保存目录

#define WebServiceKey							TEXT("|%^@@^%|")			//云打印WebService调用时的密钥
#define XabEntKey								TEXT("704325")				//云打印系统中信安宝的企业ID
#define Cloud_LOCAL_DB_NAME						TEXT("CloudPrintDb.dll")
#define CloudPrintRouterServiceName				TEXT("CloudPrintRouterHost")//云打印服务名
#define Max_Detction_Thread_Count				MAXIMUM_WAIT_OBJECTS		//检测网络设备的最大线程数,WaitForMultipleObjects最大等待64
#define Wait_Milliseconds_Per_Device			(1000*15)	//检测网络设备的最大等待时间，毫秒，（默认是15秒）

#define ISEC_PRINT_ROUTER_SVC_PIPE				TEXT("\\\\.\\pipe\\{AD09D604-A872-42d2-A63C-786B1272AAD3}")
#define ISEC_PRINT_ROUTER_UI_PIPE				TEXT("\\\\.\\pipe\\{5654F353-A926-4173-9D66-7FA68DD92219}")
#define ISEC_PRINT_ROUTER_SQLITE_MUTEX_NAME		TEXT("Global\\{CB502331-84D8-44af-BBDA-2053E0614291}")

#define ALL_PRINTER_MANUFACTURER				TEXT("所有品牌")
#define PRINTER_MANUFACTURER_UNKNOW				TEXT("UnKnow")
#define PRINTER_MANUFACTURER2_UNKNOW			TEXT("未知")
#define PRINTER_IP								TEXT("打印机IP")
#define PRINTER_NAME							TEXT("打印机名")
#define PRINTER_MODEL							TEXT("打印机型号")
#define ERROR_IP_ADDRESS						TEXT("0.0.0.0")
#define ERROR_MAC_ADDRESS						TEXT("00-00-00-00-00-00")

#define FILTR_PRINTER_IP						1	//过虑打印机IP
#define FILTR_PRINTER_NAME						2	//过虑打印机名
#define FILTR_PRINTER_MODEL						4	//过虑打印机型号

#define LinkActivatedPrefix_View				TEXT("View_")			//云打印网关管理界面的点击[查看二维码]链接前缀
#define LinkActivatedPrefix_Print				TEXT("Print_")			//云打印网关管理界面的点击[打印二维码]链接前缀
#define LinkActivatedPrefix_ScanDir				TEXT("ScanDir_")		//云打印网关管理界面的点击[打开扫描目录]链接前缀

#define Update_Folder_Name						TEXT("UpdateFile")					//打包云打印升级文件的文件夹名
#define Pack_Cloud_Print_Router					TEXT("-PackCloudPrintRouter")		//打包云打印网关升级文件
#define Pack_Cloud_Print_Client					TEXT("-PackCloudPrintClient")		//打包云打印客户端升级文件
#define Update_Cloud_Print_Router				TEXT("-UpdateCloudPrintRouter")		//云打印网关升级参数
#define Update_Cloud_Print_Client				TEXT("-UpdateCloudPrintClient")		//云打印客户端升级参数
#define Copy_Cloud_Print_Router					TEXT("-CopyCloudPrintRouter")		//云打印网关执行覆盖参数
#define Copy_Cloud_Print_Client					TEXT("-CopyCloudPrintClient")		//云打印客户端执行覆盖参数

#define Cloud_Print_Update_Exe					TEXT("CloudPrintUpdate.exe")		//云打印升级进程
#define Cloud_Print_Router_Exe					TEXT("CloudPrintRouterHost.exe")	
#define Cloud_Print_Router_Service				TEXT("CloudPrintRouterHost")	
#define Cloud_Print_Router_Mgr_Exe				TEXT("CloudPrintMgr.exe")	
#define Cloud_Print_Client_Exe					TEXT("iCltPrintHost.exe")	
#define Cloud_Print_Client_Service				TEXT("iCltPrintHost")	
#define Cloud_Print_Safe_Client_Exe				TEXT("iSafeClient.exe")

#define Cloud_Print_Config_Update_Ini			TEXT("config\\config_update.ini")	//配置文件更新文件


#define Windows_Print_Spooler_Service			TEXT("Spooler")						//windows打印服务名
#define _PrintEnvironment		NULL

//#define Print_Job_Info_Dir						TEXT("PrintJobInfo_Cache")	//打印作业信息保存总路径
#define Print_Job_Info_File_Suffix				TEXT("pji")					//打印作业信息保存文件后缀
#define Print_Job_Info_Clear_Hours				(24)						//打印作业信息清理周期(小时)
#define Print_Job_Info_Check_Seconds			(60)						//打印作业信息检测周期(秒)

#define Printer_Meter_Info_Dir					TEXT("PrinterMeterInfo_Cache")	//打印机抄表信息保存总路径
#define Printer_Meter_Info_Begin				TEXT("PrinterMeterInfoBegin.Snapshoot")	//打印机抄表起始信息文件名
#define Printer_Meter_Info_End					TEXT("PrinterMeterInfoEnd.Snapshoot")	//打印机抄表结束信息文件名


#pragma pack(push, 1)

//包头,参考NET_PACK_HEAD
typedef struct tag_MSG_Pack_Head
{
	UINT		nPackBodySize;		//包的大小，不包括包头
	UINT		nMainCmd;			//主命令
	UINT		nSubCmd;			//子命令
	WORD		nIndentify;			//包的标识
	WORD		nPackMask;			//分大包，小包，是否为	压缩包，用掩码区分
	UINT		nReserve;			//保留字段
	//组播包接收时，标识组播数据源的ip或学生id
}MSG_PACK_HEAD,*PMSG_PACK_HEAD;

typedef struct tag_ST_CloudServFirstPack
{
	BYTE		bysEntID[20];
}ST_CloudServFirstPack, *PST_CloudServFirstPack;

typedef struct tag_NetSearchRange
{
	DWORD dwIPBegin;
	DWORD dwIPEnd;
	DWORD dwIPSubnetMask;
}NetSearchRange, *PNetSearchRange;

//打印机信息
typedef struct tag_PrinterInfo 
{
	char cFlag[50];					//打印机唯一标记号，由cPrinterName+cIPAddress字符串的MD5加密得到
	char cPrinterName[MAX_PATH];	//打印机名
	int nBrand;						//品牌类型
	char cManufacturer[100];		//制造商-英文名
	char cManufacturer2[100];		//制造商-中文名
	char cModel[MAX_PATH];			//型号
	char cIPAddress[20];			//IP地址
	char cMAC[20];					//MAC
	char cPort[MAX_PATH];			//端口名
	DWORD dwStatus;					//打印机状态
	char cStatus[MAX_PATH];			//打印机状态描述
	int nStationId;					//云打印系统中的打印点ID
	BOOL bEnableCloudPrint;			//启用云打印
	BOOL bEnableCloudOPS;			//启用云运维
	char cQrCodePath[MAX_PATH];		//二维码路径
	char cDNS[50];					//DNS
	char cSerialNumber[50];			//序列号
	char cServiceTag[50];			//
	char cLocation[50];				//
	char cType[50];					//网络、USB等
	DWORD nType;					//网络、USB等,参考Cloud_Printer_Port_Type
	BOOL bColorCapable;				//
	BOOL bDoublePrintCapable;		//
	float fDPI;						//
	int nPPM;						//
	char cProtocolVersion[50];		//SNMP1,2c,3
	BOOL bPINCapable;				//
	BOOL bPINEnabled;
	int nEquipId;					//设备ID，一般为网关ID
	int nEntId;						//企业ID
	char cEntKey[20];				//企业标识符
	char cEntName[100];				//企业名称
}PrinterInfo, *PPrinterInfo;


#pragma pack(pop)

#endif //_EduCloudPrintRouterTmepDef_h_B9ED834F_A02F_4996_89E6_559841D7280D_