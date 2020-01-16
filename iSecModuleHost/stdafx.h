// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // 移除对话框中的 MFC 控件支持

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#define OS_VERSION_9X		0
#define OS_VERSION_2000		1
#define OS_VERSION_XP		2
#define OS_VERSION_2003		3
#define OS_VERSION_VISTA	4
#define OS_VERSION_2008		5
#define OS_VERSION_WIN7		6
#define OS_VERSION_UNKOWN	7

typedef BOOL(WINAPI* PFGetVersionEx)(LPOSVERSIONINFO);

#ifndef VER_NT_WORKSTATION
typedef struct _OSVERSIONINFOEXAS {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	CHAR   szCSDVersion[128];     // Maintenance string for PSS usage
	WORD   wServicePackMajor;
	WORD   wServicePackMinor;
	WORD   wSuiteMask;
	BYTE  wProductType;
	BYTE  wReserved;
} OSVERSIONINFOEXAS, *POSVERSIONINFOEXAS, *LPOSVERSIONINFOEXAS;

typedef OSVERSIONINFOEXAS OSVERSIONINFOEXS;

#define VER_NT_WORKSTATION              0x0000001
#define VER_NT_DOMAIN_CONTROLLER        0x0000002
#define VER_NT_SERVER                   0x0000003
#else
typedef OSVERSIONINFOEXA OSVERSIONINFOEXS;
#endif


#include <iostream>
#include "WtsApi32.h"
#pragma comment(lib, "WtsApi32.lib")
#include <winsvc.h>
#include "../Public/Global.h"
#include "../Public/CmmLibport.h"
#include "../Public/include/glCmdAndStructDef.h"

#include "../Public/ProtectDef.h"
#include "../iSecCltSvcHost/AutoRun.h"



extern HANDLE g_hMutexInstance;
extern BOOL HasRun(CString szGlobalMutexName);
extern CFileLog theLog;


#define LOCAL_SVC_NAME _T("iSecModuleHost")
typedef DWORD(WINAPI* WTSGETACTIVECONSOLESESSIONID)(VOID);




// TODO:  在此处引用程序需要的其他头文件
