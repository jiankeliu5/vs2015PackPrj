// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // �Ƴ��Ի����е� MFC �ؼ�֧��

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include <afx.h>
#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
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




// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
