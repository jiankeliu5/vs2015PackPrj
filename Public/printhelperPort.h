#pragma once
#include "../printhelper/ioprinter.h"
#include "../printhelper/scanmonitor.h"
#include "../PrintHelper/CopyMgr.h"
#include "../PrintHelper/CopyInterface.h"
#include "../PrintHelper/SnmpMonitor.h"
#include "../PrintHelper/SnmpInfoHelper.h"
#include "../PrintHelper/PrinterAlert_Def.h"
#include "../PrintHelper/COMPortHelper.h"
#include "../PrintHelper/InfraredInduction.h"
#include "../PrintHelper/SnmpInterface.h"
#ifdef _DEBUG
#pragma comment(lib,"..\\Debug\\printhelper.lib")
#else
#pragma comment(lib,"..\\Release\\printhelper.lib")
#endif

extern "C"  __declspec(dllimport)   void __stdcall  SetPrintHelperBindPrinter(const TCHAR* pszBindPrinter);



