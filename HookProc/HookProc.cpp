    // ***********************************************************************
// HookProc.cpp�����װ�˷������Ĺ���
// Edited by:JARRY
// Edited Time:2011��01��19��
// ***********************************************************************


#include "stdafx.h"


#include "HookProc.h"
#include <Psapi.h>
#include <Tlhelp32.h>
#include "FileMap.h"

#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
#	ifdef AFX_WINX64
#		include "../Public/Include/MinHook.h"
#		pragma comment(lib, "../Public/Lib/Release_x64/libMinHook.x64.lib")
#	else
#		include "../Public/Include/MinHook.h"
#		pragma comment(lib, "../Public/Lib/Release/libMinHook.x86.lib")
#endif

#endif

HHOOK glGetMessageHook = NULL;
CFileMap m_oMap;
CString m_sProcessPath;

CString LoadS(UINT nID)
{
	CString s;
	s.LoadString(nID);
	return s;
}

#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
typedef HANDLE (WINAPI *PFN_CreateFileW)(
				  LPCWSTR lpFileName,                         // file name
				  DWORD dwDesiredAccess,                      // access mode
				  DWORD dwShareMode,                          // share mode
				  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
				  DWORD dwCreationDisposition,                // how to create
				  DWORD dwFlagsAndAttributes,                 // file attributes
				  HANDLE hTemplateFile                        // handle to template file
				  );

typedef HANDLE (WINAPI *PFN_CreateFileA)(
				  LPCSTR lpFileName,                         // file name
				  DWORD dwDesiredAccess,                      // access mode
				  DWORD dwShareMode,                          // share mode
				  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
				  DWORD dwCreationDisposition,                // how to create
				  DWORD dwFlagsAndAttributes,                 // file attributes
				  HANDLE hTemplateFile                        // handle to template file
				  );

typedef BOOL (WINAPI *PFN_CloseHandle)(
				  __in HANDLE hObject
				  );

typedef BOOL (WINAPI *PFN_SetJob)(
				  HANDLE hPrinter,  // handle to printer object
				  DWORD JobId,      // print job identifier
				  DWORD Level,      // information level
				  LPBYTE  pJob,     // job information buffer
				  DWORD Command     // job command value
				  );

typedef BOOL (WINAPI *PFN_StartDocW)(
				  HDC hdc, CONST DOCINFOW* lpdi
				  );


PFN_CreateFileW g_pfnCreateFileW = NULL;
PFN_CreateFileA g_pfnCreateFileA = NULL;
PFN_CloseHandle g_pfnCloseHandle = NULL;
PFN_SetJob		g_pfnSetJob = NULL;
PFN_StartDocW   g_pfnStartDocW = NULL;

#else
DETOUR_TRAMPOLINE(HANDLE WINAPI Real_CreateFileW(
				  LPCWSTR lpFileName,                         // file name
				  DWORD dwDesiredAccess,                      // access mode
				  DWORD dwShareMode,                          // share mode
				  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
				  DWORD dwCreationDisposition,                // how to create
				  DWORD dwFlagsAndAttributes,                 // file attributes
				  HANDLE hTemplateFile                        // handle to template file
				  ),CreateFileW);

DETOUR_TRAMPOLINE(HANDLE WINAPI Real_CreateFileA(
				  LPCSTR lpFileName,                         // file name
				  DWORD dwDesiredAccess,                      // access mode
				  DWORD dwShareMode,                          // share mode
				  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
				  DWORD dwCreationDisposition,                // how to create
				  DWORD dwFlagsAndAttributes,                 // file attributes
				  HANDLE hTemplateFile                        // handle to template file
				  ),CreateFileA);

DETOUR_TRAMPOLINE(BOOL WINAPI Real_CloseHandle(
				   __in HANDLE hObject
				  ),CloseHandle);

DETOUR_TRAMPOLINE(BOOL WINAPI Real_SetJob(
			HANDLE hPrinter,  // handle to printer object
			DWORD JobId,      // print job identifier
			DWORD Level,      // information level
			LPBYTE  pJob,     // job information buffer
			DWORD Command     // job command value
			),SetJob);

DETOUR_TRAMPOLINE(BOOL WINAPI Real_StartDocW(
				  HDC hdc, CONST DOCINFOW* lpdi
				  ),StartDocW);

DETOUR_TRAMPOLINE(BOOL WINAPI Real_ExtTextOutA(
				HDC hdc,          // handle to DC
				int X,            // x-coordinate of reference point
				int Y,            // y-coordinate of reference point
				UINT fuOptions,   // text-output options
				CONST RECT* lprc, // optional dimensions
				LPCSTR lpString, // string
				UINT cbCount,     // number of characters in string
				CONST INT* lpDx   // array of spacing values
				),ExtTextOutA);
DETOUR_TRAMPOLINE(BOOL WINAPI Real_ExtTextOutW(
				  HDC hdc,          // handle to DC
				  int X,            // x-coordinate of reference point
				  int Y,            // y-coordinate of reference point
				  UINT fuOptions,   // text-output options
				  CONST RECT* lprc, // optional dimensions
				  LPCWSTR lpString, // string
				  UINT cbCount,     // number of characters in string
				  CONST INT* lpDx   // array of spacing values
				  ),ExtTextOutW);
#endif //#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������

BOOL WINAPI MyExtTextOutA(
							 HDC hdc,          // handle to DC
							 int X,            // x-coordinate of reference point
							 int Y,            // y-coordinate of reference point
							 UINT fuOptions,   // text-output options
							 CONST RECT* lprc, // optional dimensions
							 LPCSTR lpString, // string
							 UINT cbCount,     // number of characters in string
							 CONST INT* lpDx   // array of spacing values
							 )
{
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	return FALSE;
#else
	WriteFileLog("MyExtTextOutA %s",lpString);
	return Real_ExtTextOutA(hdc,X,Y,fuOptions,lprc,lpString,cbCount,lpDx);
#endif
}

BOOL WINAPI MyExtTextOutW(
						  HDC hdc,          // handle to DC
						  int X,            // x-coordinate of reference point
						  int Y,            // y-coordinate of reference point
						  UINT fuOptions,   // text-output options
						  CONST RECT* lprc, // optional dimensions
						  LPCWSTR lpString, // string
						  UINT cbCount,     // number of characters in string
						  CONST INT* lpDx   // array of spacing values
							 )
{
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	return TRUE;
#else
	USES_CONVERSION;
	WriteFileLog("MyExtTextOutW %s",W2A(lpString));
	return Real_ExtTextOutW(hdc,X,Y,fuOptions,lprc,lpString,cbCount,lpDx);
#endif
}

BOOL ParseName(CString strDisplayName);
BOOL CheckDocType(CString s);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHookProcApp
BEGIN_MESSAGE_MAP(CHookProcApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// ���ǵ�Win32 DLL��Win16 DLL���ڵĲ����Win32������Ҫ�ڶ�����̼乲�����ݣ��ͱ����ȡһЩ��ʩ���������������ȡ��һ�����������ݶ�


char g_szDocType[1024] = {0};

//��ȡ��־Ŀ¼
CString& GetLogDir()
{
	static CString g_szLogDir = "";
	if (g_szLogDir.IsEmpty())
	{
		OutputDebugString("##CCommonFun::GetLogDir");

		TCHAR path[MAX_PATH];
		CString sDefaultPath;
		if(GetModuleFileName(0,path,MAX_PATH))
		{
			TCHAR *pdest;
			pdest=_tcsrchr(path,_T('\\'));
			if(pdest) 
				*pdest=0;
			sDefaultPath=path;
			sDefaultPath += "\\xlog";
		}

		if (sDefaultPath.IsEmpty())
		{
			sDefaultPath = ".\\xlog";
		}

		g_szLogDir = sDefaultPath;
	}

	return g_szLogDir;
}

void WriteFileLog(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sLog;
	sLog.FormatV(lpszFormat, argList);
	va_end(argList);
	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));
	CString sFileName;
	sFileName.Format(_T("%s\\HookPrint.Log"), GetLogDir());

	//OutputDebugStringA(sLog);
	//return ;
	try
	{
		CFile fileLog;
		if(!fileLog.Open(sFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
			return;

		CString sTime = time.Format( _T("%Y-%m-%d %H:%M:%S") );
		CString sTmp;

		sTmp.Format(_T("%s,	 %s"),sTime,sLog);
		sTmp += _T("\r\n");
		fileLog.SeekToEnd();
		fileLog.Write(sTmp,sTmp.GetLength());
		fileLog.Close();
	}
	catch ( CFileException* ex ) 
	{
		ex->Delete();
	}
	catch (...) 
	{
		;
	}
}

CString GetProcessName()
{
	CString strExe;
	char szExe[MAX_PATH]= {0};
	GetModuleFileName(NULL, szExe, MAX_PATH);
	//strExe = GetFileName(szExe);
	strExe.Format(_T("%s"),szExe);
	strExe.MakeLower();
	return strExe;
}
// StartDocPrinterA��StartDocPrinterW���ڴ����ӡ���ļ�����
typedef DWORD (WINAPI* PFStartDocPrinterA)(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo);
DWORD WINAPI MyStartDocPrinterA(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo);

typedef DWORD (WINAPI* PFStartDocPrinterW)(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo);
DWORD WINAPI MyStartDocPrinterW(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo);


typedef DWORD (WINAPI* PFStartDocW)(HDC hdc, CONST DOCINFOW* lpdi);
DWORD WINAPI MyStartDocW(HDC hdc, CONST DOCINFOW* lpdi);

typedef DWORD (WINAPI* PFStartDocA)(HDC hdc, CONST DOCINFOA* lpdi);
DWORD WINAPI MyStartDocA(HDC hdc, CONST DOCINFOA* lpdi);



//////////////////////////////////////////////////////////////////////////
// �����Ķ���

void HookFunctions();													// ����API��������
void UnHookFunctions();													// ��ԭAPI��������

LRESULT CALLBACK HookGetMessageProc(int code,WPARAM wParam,LPARAM lParam);// �¼��ص�


//////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���
HINSTANCE glInstanceHook = NULL;

// CHookProcApp ����
CHookProcApp::CHookProcApp()
{
}

BOOL CHookProcApp::InitInstance()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	glInstanceHook = m_hInstance;
	HookFunctions();
	return TRUE;
}

int CHookProcApp::ExitInstance()
{
	UnHookFunctions();
	glInstanceHook = NULL;

	return TRUE;
}

CHookProcApp theApp;


HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName,                         // file name
							DWORD dwDesiredAccess,                      // access mode
							DWORD dwShareMode,                          // share mode
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
							DWORD dwCreationDisposition,                // how to create
							DWORD dwFlagsAndAttributes,                 // file attributes
							HANDLE hTemplateFile  )                      // handle to template file
{
	HANDLE hRet=INVALID_HANDLE_VALUE;
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	hRet = g_pfnCreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,
		dwFlagsAndAttributes,hTemplateFile);
#else
	hRet= Real_CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,
				dwFlagsAndAttributes,hTemplateFile);
#endif

	USES_CONVERSION;
	if (INVALID_HANDLE_VALUE != hRet)
	{
		m_oMap.OnOpenFile(hRet,W2A(lpFileName),m_sProcessPath);
	}
	return hRet;
}						   

HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName,                         // file name
							DWORD dwDesiredAccess,                      // access mode
							DWORD dwShareMode,                          // share mode
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
							DWORD dwCreationDisposition,                // how to create
							DWORD dwFlagsAndAttributes,                 // file attributes
							HANDLE hTemplateFile  )                      // handle to template file
{
	HANDLE hRet=INVALID_HANDLE_VALUE;
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	hRet= g_pfnCreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,
		dwFlagsAndAttributes,hTemplateFile);
#else
	hRet= Real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,
		dwFlagsAndAttributes,hTemplateFile);
#endif
	if (INVALID_HANDLE_VALUE != hRet)
	{
		m_oMap.OnOpenFile(hRet,lpFileName,m_sProcessPath);
	}
	return hRet;
}
BOOL WINAPI MySetJob(
						HANDLE hPrinter,  // handle to printer object
						DWORD JobId,      // print job identifier
						DWORD Level,      // information level
						LPBYTE  pJob,     // job information buffer
						DWORD Command     // job command value
						)
{
	BOOL bRet = FALSE;
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	bRet = g_pfnSetJob(
		hPrinter,  // handle to printer object
		JobId,      // print job identifier
		Level,      // information level
		pJob,     // job information buffer
		Command     // job command value
		);
#else
	bRet =  Real_SetJob(
		 hPrinter,  // handle to printer object
		 JobId,      // print job identifier
		 Level,      // information level
		  pJob,     // job information buffer
		 Command     // job command value
		);
#endif
	if (Level == 3)
	{
		JOB_INFO_3* pInfo = (JOB_INFO_3*)pJob;
		WriteFileLog("jobid3 = %d",pInfo->JobId);
	}
	else if (Level == 2)
	{
		JOB_INFO_2* pInfo = (JOB_INFO_2*)pJob;
		WriteFileLog("jobid2 = %d,%s",pInfo->JobId,pInfo->pDocument);
	}
	else if (Level == 1)
	{
		JOB_INFO_1* pInfo = (JOB_INFO_1*)pJob;
		WriteFileLog("jobid1 = %d,%s",pInfo->JobId,pInfo->pDocument);
	}
	return bRet;
}


BOOL IsNotepad()
{
	// ��ȡ������
	if(m_sProcessPath.Find("notepad.exe") > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CheckProcess(CString sName)
{
	sName.MakeLower();
	if(m_sProcessPath.Find(sName) > 0)
	{
		return TRUE;
	}
	return FALSE;
}

//-------------
//�򿪶���ĵ���ȷ����Щ�ĵ�
BOOL WINAPI MyCloseHandle(
							 __in HANDLE hObject
							 )
{
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	BOOL bRet = g_pfnCloseHandle(hObject);
#else
	BOOL bRet = Real_CloseHandle(hObject);
#endif
	m_oMap.OnCloseFile(hObject,m_sProcessPath);
	return bRet;
	if (bRet)
	{
		if (!IsNotepad())
		{
			//HandleMap.RemoveKey(hObject);
		}
		else
		{
			WriteFileLog("MyCloseHandle");
		}
		
	}
	return bRet;
}


DWORD WINAPI MyStartDocPrinterA(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo)
{
	DWORD dwRet = 0;
	
	do 
	{
		// �õ���ӡ���ļ���Ϣ
		if (Level == 1)
		{
			DOC_INFO_1A* docinfo = (DOC_INFO_1A*)pDocInfo;
			CString sFileName(docinfo->pDocName);
			sFileName.MakeLower();
			WriteFileLog("StartDocPrinterA1----", sFileName.GetBuffer());
			sFileName.ReleaseBuffer(-1);

		}
		else
		{
			DOC_INFO_2A* docinfo = (DOC_INFO_2A*)pDocInfo;
			CString sFileName(docinfo->pDocName);
			sFileName.MakeLower();
			WriteFileLog("StartDocPrinterA2----", sFileName.GetBuffer());
			sFileName.ReleaseBuffer(-1);

		}

		dwRet = StartDocPrinterA(hPrinter,Level,pDocInfo);
	} while (0);

	return dwRet;
}

DWORD WINAPI MyStartDocPrinterW(HANDLE hPrinter,DWORD Level,LPBYTE pDocInfo)
{
	DWORD dwRet = 0;
	do 
	{
		// �õ���ӡ���ļ���Ϣ
		if (Level == 1)
		{
			DOC_INFO_1W* docinfo = (DOC_INFO_1W*)pDocInfo;
			CString sFileName(docinfo->pDocName);
			sFileName.MakeLower();
			WriteFileLog("StartDocPrinterW1 %s", sFileName.GetBuffer());

		}
		else
		{
			DOC_INFO_2W* docinfo = (DOC_INFO_2W*)pDocInfo;
			CString sFileName(docinfo->pDocName);
			sFileName.MakeLower();
			WriteFileLog("StartDocPrinterW2 %s", sFileName);
		}

		dwRet = StartDocPrinterW(hPrinter,Level,pDocInfo);
	} while (0);

	return dwRet;
}

class CFindWndParam
{
public:
	CString m_sTitle;
	CString m_sClass;
	BOOL m_bFindChild;
	BOOL m_bSuc;
};

BOOL CALLBACK EnumChildProc(          HWND hwnd,
							LPARAM lParam
							)
{
	char sz[MAX_PATH] = {0};
	GetWindowText(hwnd,sz,MAX_PATH);
	char szCls[MAX_PATH] = {0};
	GetClassName(hwnd,szCls,MAX_PATH);
	WriteFileLog("Txt = %s,Cls = %s",sz,szCls);
	//::PostMessage(hwnd,WM_CANCELMODE,0,0);
	if (stricmp(szCls,"edit") == 0 )
	{
		//::PostMessage(hwnd,WM_CLOSE,0,0);
		
		char szTxt[MAX_PATH] = {0};
		::GetWindowText(hwnd,szTxt,MAX_PATH);
		WriteFileLog("txt = %s",szTxt);
	}
	return TRUE;
}

BOOL CALLBACK EnumThreadWndProc( HWND hwnd,LPARAM lParam)
{
	CFindWndParam* pFind = (CFindWndParam*)lParam;

	char szTxt[MAX_PATH] = {0};
	GetWindowText(hwnd,szTxt,MAX_PATH);
	char szCls[MAX_PATH] = {0};
	GetClassName(hwnd,szCls,MAX_PATH);
	WriteFileLog("EnumThreadWndProc Txt = %s,Cls = %s",szTxt,szCls);
	CString sTitle = szTxt;
	sTitle.MakeLower();
	if (pFind )
	{
		if(!pFind->m_sClass.IsEmpty()&&pFind->m_sClass.CompareNoCase(szCls)==0)
		{
			pFind->m_sTitle = szTxt;
			pFind->m_bSuc = TRUE;
			return FALSE;
		}
		else if (!pFind->m_sTitle.IsEmpty()&& sTitle.Find(pFind->m_sTitle) >=0)
		{
			pFind->m_sTitle = sTitle;
			pFind->m_bSuc = TRUE;
			return FALSE;
		}

		
		if(pFind->m_bFindChild)
		{
			EnumChildWindows(hwnd,EnumChildProc,lParam);
		}
	}
	return TRUE;
}
DWORD WINAPI MyStartDocW(HDC hdc, CONST DOCINFOW* lpdi)
{
	DWORD dwRet = 0;
	do 
	{
		// �õ���ӡ���ļ���Ϣ
		CString sFileName(lpdi->lpszDocName);
		sFileName.MakeLower();
		WriteFileLog("StartDocW---- %s", sFileName);
		DWORD dwThId = ::GetCurrentThreadId();
		//if(IsNotepad())
		{
			//WriteFileLog("MyStartDocW node");
			//EnumThreadWindows( dwThId,EnumThreadWndProc,0);
		}
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
		dwRet = g_pfnStartDocW(hdc, lpdi);
#else
		dwRet = Real_StartDocW(hdc, lpdi);
#endif
		// ��������
		ParseName(sFileName);
	} while (0);

	return dwRet;
}


DWORD WINAPI MyStartDocA(HDC hdc, CONST DOCINFO* lpdi)
{
	DWORD dwRet = 0;
// 	EnterCriticalSection(&mhSection);
// 	UnHookOneAPI(3);
	do 
	{
		// �õ���ӡ���ļ���Ϣ
		CString sFileName(lpdi->lpszDocName);
		sFileName.MakeLower();
		WriteFileLog("StartDocA:%s", sFileName);
		

		dwRet = StartDocA(hdc, lpdi);
	} while (0);
// 	HookOneAPI(3);
// 	LeaveCriticalSection(&mhSection);
	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
// ��ʼ����
BOOL  __declspec(dllexport)StartHook(char* szTxt)
{
	BOOL bRet = TRUE;
	do 
	{		
		WriteFileLog("szType = %s",szTxt);
		if(m_oMap.Create(szTxt))
		{
			
		}
	
		// �����¼���ȡ����
		if (!glGetMessageHook)
		{
			glGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)HookGetMessageProc,glInstanceHook,0);
			if (glGetMessageHook == NULL)
			{
				bRet = FALSE;
				break;
			}
		}
		break;
		
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// ֹͣ����
BOOL  __declspec(dllexport)StopHook()
{
	BOOL bRet = TRUE;
	do 
	{
		// �ͷ��¼�����
		if (glGetMessageHook)
		{
			if (!UnhookWindowsHookEx(glGetMessageHook))
			{
				bRet =FALSE;
				break;
			}

			glGetMessageHook = NULL;
		}

		
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// ��ʼע��API��������
void HookFunctions()
{	
	WriteFileLog("HookFunctions,1");

#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	WriteFileLog("HookFunctions,2");

	if (MH_Initialize() != MH_OK)
	{
		WriteFileLog(_T("MH_Initialize failed!\n"));
		return;
	}

	if (MH_CreateHook(&CreateFileW, &MyCreateFileW, reinterpret_cast<void**>(&g_pfnCreateFileW)) != MH_OK)
	{
		WriteFileLog(_T("CreateFileW failed!\n"));
		return;
	}

	if (MH_EnableHook(&CreateFileW) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CreateFileW failed!\n"));
		return;
	}

	//--CreateFileA
	if (MH_CreateHook(&CreateFileA, &MyCreateFileA, reinterpret_cast<void**>(&g_pfnCreateFileA)) != MH_OK)
	{
		WriteFileLog(_T("CreateFileA failed!\n"));
		return;
	}

	if (MH_EnableHook(&CreateFileA) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CreateFileA failed!\n"));
		return;
	}

	//--CloseHandle
	if (MH_CreateHook(&CloseHandle, &MyCloseHandle, reinterpret_cast<void**>(&g_pfnCloseHandle)) != MH_OK)
	{
		WriteFileLog(_T("CloseHandle failed!\n"));
		return;
	}

	if (MH_EnableHook(&CloseHandle) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CloseHandle failed!\n"));
		return;
	}

	//--SetJob
	if (MH_CreateHook(&SetJob, &MySetJob, reinterpret_cast<void**>(&g_pfnSetJob)) != MH_OK)
	{
		WriteFileLog(_T("SetJob failed!\n"));
		return;
	}

	if (MH_EnableHook(&SetJob) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook SetJob failed!\n"));
		return;
	}

	//--StartDocW
	if (MH_CreateHook(&StartDocW, &MyStartDocW, reinterpret_cast<void**>(&g_pfnStartDocW)) != MH_OK)
	{
		WriteFileLog(_T("StartDocW failed!\n"));
		return;
	}

	if (MH_EnableHook(&StartDocW) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook StartDocW failed!\n"));
		return;
	}
#else
	WriteFileLog("HookFunctions,10");

	DetourFunctionWithTrampoline((PBYTE)Real_CreateFileW, (PBYTE)MyCreateFileW);
	DetourFunctionWithTrampoline((PBYTE)Real_CreateFileA, (PBYTE)MyCreateFileA);
	DetourFunctionWithTrampoline((PBYTE)Real_CloseHandle, (PBYTE)MyCloseHandle);
	DetourFunctionWithTrampoline((PBYTE)Real_SetJob, (PBYTE)MySetJob);
	DetourFunctionWithTrampoline((PBYTE)Real_StartDocW, (PBYTE)MyStartDocW);
#endif

//	DetourFunctionWithTrampoline((PBYTE)Real_ExtTextOutA, (PBYTE)MyExtTextOutA);
//	DetourFunctionWithTrampoline((PBYTE)Real_ExtTextOutW, (PBYTE)MyExtTextOutW);
	
	char szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	m_sProcessPath.Format("%s", szPath);
	m_sProcessPath.MakeLower();
	WriteFileLog("hook %s",m_sProcessPath);	
}

//////////////////////////////////////////////////////////////////////////
// �ͷ�API��������
void UnHookFunctions()
{
#ifdef DEF_UseMinHook //add by zfq,2016-04-06 ʹ��MINHOOK���������
	if (MH_DisableHook(&CreateFileW) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CreateFileW failed!\n"));
//		return;
	}

	if (MH_DisableHook(&CreateFileA) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CreateFileA failed!\n"));
//		return;
	}

	if (MH_DisableHook(&CloseHandle) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook CloseHandle failed!\n"));
//		return;
	}

	if (MH_DisableHook(&SetJob) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook SetJob failed!\n"));
//		return;
	}

	if (MH_DisableHook(&StartDocW) != MH_OK)
	{
		WriteFileLog(_T("MH_EnableHook StartDocW failed!\n"));
//		return;
	}

	if (MH_Uninitialize() != MH_OK)
	{
		WriteFileLog(_T("MH_Uninitialize failed!\n"));
		return;
	}
#else
	DetourRemove((PBYTE)Real_CreateFileW, (PBYTE)MyCreateFileW);
	DetourRemove((PBYTE)Real_CreateFileA, (PBYTE)MyCreateFileA);
	DetourRemove((PBYTE)Real_CloseHandle, (PBYTE)MyCloseHandle);
	DetourRemove((PBYTE)Real_StartDocW, (PBYTE)MyStartDocW);
	DetourRemove((PBYTE)Real_SetJob, (PBYTE)MySetJob);
#endif

//	DetourRemove((PBYTE)Real_ExtTextOutA, (PBYTE)MyExtTextOutA);
//	DetourRemove((PBYTE)Real_ExtTextOutW, (PBYTE)MyExtTextOutW);

	m_oMap.OnProcessClosed(::GetCurrentProcessId());
}


// ������Ϣ�¼�
LRESULT CALLBACK HookGetMessageProc(int code,WPARAM wParam,LPARAM lParam)
{
	return CallNextHookEx(glGetMessageHook, code, wParam, lParam);
}

BOOL Split(IN CString str,  OUT CString& str1, OUT CString& str2)
{
	int nIndex = str.Find(" - ");
	if (-1 == nIndex)
	{
		str1 = str;
		return FALSE;
	}
	else
	{
		str1 = str.Left(nIndex);
		str2 = str.Right(str.GetLength() - nIndex - 3);
		return TRUE;
	}	
}

BOOL ParseName(CString strDisplayName)
{
	CString sPrintedName = strDisplayName;
	BOOL bRet = FALSE;
	CString str1, str2;
	CString strFind;
// 	if (CheckProcess("explorer.exe") && (strDisplayName.CompareNoCase(_T("ȫҳ����ͼƬ")) == 0))
	if (CheckProcess("explorer.exe") && (strDisplayName.CompareNoCase(LoadS(IDS_STR_FULLPAGE_FAX_PIC)) == 0))
	{
		//cls:ShImgVw:CPreviewWnd

		HWND hWnd = FindWindow(_T("ShImgVw:CPreviewWnd"),0);
		if (hWnd)
		{
			char szTxt[MAX_PATH] = {0};
			GetWindowText(hWnd,szTxt,MAX_PATH);
			strDisplayName = szTxt;
			strDisplayName.MakeLower();
		}
		BOOL b = FALSE;
		/*
		CFindWndParam* parm = new CFindWndParam();
		parm->m_sClass.Format(_T("ShImgVw:CPreviewWnd"));
		BOOL b = EnumThreadWindows( GetCurrentThreadId(),EnumThreadWndProc,(LPARAM)parm);
		if(!parm->m_sTitle.IsEmpty())
		{
			strDisplayName = parm->m_sTitle;
		}
		delete parm;*/
		WriteFileLog("Find ȫҳ����ͼƬ %s,%d,%d",strDisplayName,b,::GetLastError());
	}
	if (Split(strDisplayName, str1, str2))
	{
		WriteFileLog("ParseName(%s,%s)",str1, str2);
		if (str1.Compare("microsoft word") == 0 ||
			str1.CompareNoCase("microsoft powerpoint") == 0)
		{
			strFind = str2;
			CString sTmp = LoadS(IDS_STR_COMP_MODE);//_T(" [����ģʽ]");
			if(strFind.GetLength() > sTmp.GetLength() && strFind.Right(sTmp.GetLength()).Compare(sTmp) == 0)
			{
				strFind = strFind.Left(strFind.GetLength() - sTmp.GetLength());
			}
		}
// 		else if (str2.IsEmpty() || (str2.Compare("���±�") == 0) || (str2.CompareNoCase(_T("Windows ͼƬ�ʹ���鿴��")) == 0))
		else if (str2.IsEmpty() || (str2.Compare(LoadS(IDS_STR_NOTEPAD)) == 0) || (str2.CompareNoCase(LoadS(IDS_STR_WIN_PIC_FAX_VIEWER)) == 0))
		{
			strFind = str1;
		}
		
	}
	else
	{
		// ֱ�Ӳ���
		strFind = strDisplayName;
		if (CheckProcess("acad.exe"))
		{
			strFind.MakeLower();
			CString sTmp = _T(" model (1)");
			if(strFind.GetLength() > sTmp.GetLength() && strFind.Right(sTmp.GetLength()).Compare(sTmp) == 0)
			{
				strFind = strFind.Left(strFind.GetLength() - sTmp.GetLength());
				CString sSufix;
				CFindWndParam parm;
				parm.m_sTitle = strFind;
				parm.m_bSuc = FALSE;
				BOOL b = EnumThreadWindows( GetCurrentThreadId(),EnumThreadWndProc,(LPARAM)&parm);
				if(parm.m_bSuc)
				{
					int nFind = parm.m_sTitle.Find(strFind);
					if (nFind >= 0 && parm.m_sTitle.GetLength() >= (nFind + strFind.GetLength() + 4))
					{
						sSufix = parm.m_sTitle.Mid(nFind + strFind.GetLength(),4);
					}
				}

				if (sSufix.IsEmpty())
				{
					strFind += _T(".dwg");
				}
				else
				{
					strFind += sSufix;
				}	
			}					
		}
		else if (CheckProcess("ugraf.exe"))
		{
			CString sDocName;
			CFindWndParam parm;
			CString sTmp = LoadS(IDS_STR_BUILD);//_T("- ��ģ - [");
			parm.m_sTitle = sTmp;
			parm.m_bSuc = FALSE;
			BOOL b = EnumThreadWindows( GetCurrentThreadId(),EnumThreadWndProc,(LPARAM)&parm);
			if(parm.m_bSuc)
			{
				parm.m_sTitle.TrimRight("]");
				int nFind = parm.m_sTitle.Find("[");
				
				if (nFind >= 0)
				{
					int nCount = parm.m_sTitle.GetLength() - nFind - 1;
					sDocName = parm.m_sTitle.Mid(nFind + 1,nCount);
					sDocName.MakeLower();
				}
			}

			strFind = sDocName;
		}
		else if (CheckProcess("visio.exe"))
		{
			CString sTmp = _T("visio-");
			strFind.TrimLeft(sTmp);
		}
	}
	if (strFind.GetLength() > 0)
	{
		WriteFileLog("Find:%s", strFind);
		m_oMap.OnPrinted(sPrintedName,strFind,m_sProcessPath);
	}
	return bRet;
}