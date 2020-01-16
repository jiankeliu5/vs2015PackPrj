#ifndef _LmPrintStationCommDef_H_A9C189B3_0FFE_4026_8247_14EFE044029B_
#define _LmPrintStationCommDef_H_A9C189B3_0FFE_4026_8247_14EFE044029B_

#include "../../PrintHelper/CopyInterface.h"

#define LM_PRINT_ROUTER_MUTEX_NAME			TEXT("Global\\{A2D4DA69-68E7-4a22-8D52-E14EF4D45032}")
#define LM_PRINT_ROUTER_PIPE				TEXT("\\\\.\\pipe\\{BECEB51C-36B8-4206-A854-B27792B006D0}")
#define LM_PRINT_STATION_MUTEX_NAME			TEXT("Global\\{7C37FBAE-879D-439a-8598-DB468A80E223}")
#define LM_PRINT_STATION_PIPE				TEXT("\\\\.\\pipe\\{EB4C7483-916B-480b-8C4F-485BF7A64F68}")
#define LM_PRINT_JOB_DISPATCH_MUTEX_NAME	TEXT("Global\\{B34A655B-3EEC-4fc5-AB6B-F7DB98B09909}")
#define LM_PRINT_JOB_DISPATCH_PIPE			TEXT("\\\\.\\pipe\\{B03CB28E-C7F3-4da5-8FA3-A9491A797347}")

typedef struct tag_LmProcessParam
{
	DWORD dwPrinterIndex;
	DWORD dwProcessId;
	DWORD dwInstanceState;
	DWORD dwRunState;
	char pszPrinterName[MAX_PATH];
	char pszModuleExePath[MAX_PATH];
}LmProcessParam, *PLmProcessParam;

//企业打印点进程共享实例变量运行状态
enum LM_Instance_State
{
	LM_Instance_State_Orig	= 0,	//初始状态
	LM_Instance_State_Run	= 1,	//运行
	LM_Instance_State_Stop	= 2,	//停止
	LM_Instance_State_Disable	= 3		//实例禁用
};

//企业打印点进程运行状态
enum LM_Run_State
{
	LM_Run_State_UnKnown	= 0,	//未知
	LM_Run_State_Running	= 1,	//已运行
	LM_Run_State_Stoped		= 2,	//已停止
	LM_Run_State_RunningAndAbnormal		= 3,	//运行但程序异常
	LM_Run_State_ExceptionExit			= 4,	//异常退出
	LM_Run_State_ParameterErrorExit		= 5,	//参数错误退出
	LM_Run_State_MultiInstanceExit		= 6,	//多实例异常退出
	LM_Run_State_NormalExit				= 7,	//正常退出
	LM_Run_State_Exiting				= 8,	//正在退出
	LM_Run_State_Starting				= 9,	//正在启动
	LM_Run_State_Disable				= 10,	//实例被禁用，要求重启实例
	LM_Run_State_MAX					= 11	//最大值
};

static TCHAR *gc_LM_Run_State_Table[] = {
	TEXT("未知"),
	TEXT("已运行"),
	TEXT("已停止"),
	TEXT("运行有异常"),
	TEXT("异常退出"),
	TEXT("参数错误退出"),
	TEXT("多实例异常退出"),
	TEXT("已退出"),
	TEXT("正在退出"),
	TEXT("正在启动"),
	TEXT("已禁用")
};


#define MAX_PRINTER_COUNT		128			//最大控制打印机数量

#define LmPrintStationEXE		TEXT("LmPrintStation.exe")
#define LmPrintStationSetFile	TEXT("config\\settings.ini")
#define LmPrintStationRunPid	TEXT("LmPrintStationRunPid")	//保存所有企业打印点进程运行状态的节。[A=B],A表示打印点ID，B表示打印点进程ID
#define LmPrintStationMgr		TEXT("LmPrintStationMgr")		//

//命令行参数【LmPrintStation.exe /PrtID [打印点ID]】
#define LmPrintStationPrtIDFlag		TEXT("/PrtID")		//命令格式一：LmPrintStation.exe /PrtID	1
#define LmPrintStationPrtIDFlag2	TEXT("-PrtID")		//命令格式二：LmPrintStation.exe -PrtID 1

#define LmPrintStationQuitCmd		TEXT("quit")		//退出命令行指令


enum PrintOutMode
{
	PrintOutMode_MultiDrive,	//多驱动模式
	PrintOutMode_SignalDrive,	//单驱动模式，一种品牌安装一个驱动
	PrintOutMode_NetPrint		//网络打印，即通过9100直接将作业投递到打印机
};

struct BrandPrinterName
{
	BRAND_TYPE nBrand;			//打印机品牌
	TCHAR cPrinterName[128];	//打印机名
};

static struct BrandPrinterName g_oBrandPrinterName[] =
{
	{BT_SL, _T("XeroxPrinter")},	//0,默认不使用，使用BT_SL2
	{BT_DZ, _T("ToshibaPrinter")},
	{BT_KM, _T("KmPrinter")},
	{BT_LM, _T("LexmarkPrinter")},
	{BT_THIN_CLT, _T("")},
	{BT_LG, _T("RicohPrinter")},
	{BT_SL2, _T("XeroxPrinter")},
	{BT_JC, _T("KyoceraPrinter")},
	{BT_ZD, _T("AuroraPrinter")},
	{BT_XP, _T("SharpPrinter")},
	{BT_SX, _T("SamsungPrinter")},
	{BT_HP, _T("HpPrinter")},
	{BT_EPSON, _T("EpsonPrinter")},
	{BT_CANON, _T("CanonPrinter")},
	{BT_BROTHER, _T("BrotherPrinter")}
};

#endif //_LmPrintStationCommDef_H_A9C189B3_0FFE_4026_8247_14EFE044029B_