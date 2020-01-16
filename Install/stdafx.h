
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include "Resource.h"
#include <afxsock.h>		// MFC socket extensions

#include <atlsecurity.h>
#include "../Public/GlobalPrintConfig.h"
#include "../Public/Global.h"
#include "../Public/Include/glCmdAndStructDef.h"
#include "../Public/Include/glCmdAndStructPrint2.h"
#include "../public/Include/NetCmdAndStructDef.h"
#include "../Public/Include/MultiLanguageDef.h"	//add by zfq,2013.10.22,��̬�л�����
#include <afxdlgs.h>
#include "commonfun.h"
#include "FileLog.h"
#include "ado.h"
#include "DbInstaller.h"
#include "IISWeb.h"
#include "IISFTP.h"
#include "RegistryEx.h"
#include "winfirewallop.h"
#include "XmlParse.h"
#include "BaseSock.h"
#include <string.h>
using namespace std;


#define PRINT_DB_NAME	_T("iSec_Print56")
#define PRINT_STATION_DB_NAME    _T("iSec_PrinterApp")
#define PRINT_WEB_SITE	_T("iSec_Print_Web_Site")
#define VIRTUALNAME		_T("XabPrint")
#define VIRTUALNAME_Canon		_T("CanonPrint")	//����Ƕ��ʽweb
#define VIRTUALNAME_KeMei		_T("KeMeiPrint")	//����Ƕ��ʽweb
#define VIRTUALNAME_Toshiba		_T("ToshibaPrint")	//��֥Ƕ��ʽweb
#define VIRTUALNAME_HP			_T("HpPrint")		//����Ƕ��ʽweb
#define ShareName				_T("XabShareDir")
#define ShareName_KeMei			_T("KeMeiShareDir")
#define ShareName_Toshiba		_T("ToshibaShareDir")
#define DOWN_DIR		_T("down")
#define SCAN_DIR		_T("ScanDir")
#define PRINT_DIR		_T("PrintDir")
#define TRANSCRIPT_DIR	_T("TranscriptDir")
#define COPYBAK_DIR		_T("CopyBakDir")
#define REPORT_DIR		_T("ReportDir")		//�����ͻ���Ŀ¼
#define CLIENT_DIR		_T("ClientDir")		//�ͻ�������Ŀ¼
#define UPFILR_DIR		_T("UpFile")		

#define PRINT_SVC_NAME _T("iSecASvcHost")
#define PRINT_SVC_EXE _T("iSecASvcHost.exe")

#define PRINT_DB_SVC_NAME _T("PrintDbSvcHost")
#define PRINT_DB_SVC_EXE _T("DbSvcHost.exe")

#define PRINT_WEB_CTL_SVC_NAME _T("iSvcCtlForWebHost")
#define PRINT_WEB_CTL_SVC_EXE _T("iSvcCtlForWebHost.exe")

#define PRINT_SHARE_SVC_NAME _T("iPrintCenterSvrHost")
#define PRINT_SHARE_SVC_EXE _T("iPrintCenterSvrHost.exe")

#define PRINT_STATION_SVC_NAME _T("LmPrinterSvcHost")
#define PRINT_STATION_SVC_EXE _T("LmPrinterSvcHost.exe")

#define PRINT_JOB_DISPATCH_SVC_NAME _T("LmPrintJobDispatchSvrHost")
#define PRINT_JOB_DISPATCH_SVC_EXE _T("LmPrintJobDispatchSvrHost.exe")

#define PRINT_UPDATE_MGR_EXE _T("PatchUpdateSvc.exe")

#define HP_PRINT_PRO_SVC_NAME _T("iHpPrintProSvrHost")		//����Ƕ��ʽ��PRO���Ƿ���
#define HP_PRINT_PRO_SVC_EXE _T("iHpPrintProSvrHost.exe")

#define FILE_OCR_SVC_NAME _T("iSecOcrSvrHost")	//�ļ�OCRʶ�����
#define FILE_OCR_SVC_EXE _T("iSecOcrSvrHost.exe")	
#define FILE_OCR_WORKER_EXE _T("iSecOcrWorker.exe")	

#define FILE_STORAGE_FTP_SVC_NAME	TEXT("iSecFtpSvr")
#define FILE_STORAGE_FTP_SVC_EXE _T("iSecFtpSvr.exe")

#define PRODUCT_CUSTOM_REGISTY_KEY_AMD64	_T("SOFTWARE\\Wow6432Node\\iSecStar") 
#define PRODUCT_CUSTOM_REGISTY_KEY			_T("SOFTWARE\\iSecStar") 


extern CFileLog theLog;
extern BOOL MyExecuteCmdComd();
extern BOOL Enable32BitAppOnWin64(BOOL bEnable);
extern BOOL MyStartAspnet_StateService();

extern CString LoadS(int nText);
extern void Install(CString sPath,CString sParam,DWORD dwWaitMilliseconds = INFINITE,WORD dwShowCmd = SW_SHOW, BOOL bDebuglog = TRUE);

#define MBI(x) ::MessageBox(GetSafeHwnd(), LoadS(x),LoadS(IDS_TIP), MB_OK|MB_ICONINFORMATION)
#define MBW(x) ::MessageBox(GetSafeHwnd(), LoadS(x),LoadS(IDS_TIP), MB_OK|MB_ICONWARNING)
#define MBHI(x, y) ::MessageBox(x, LoadS(y),LoadS(IDS_TIP), MB_OK|MB_ICONINFORMATION)
#define MBHW(x, y) ::MessageBox(x, LoadS(y),LoadS(IDS_TIP), MB_OK|MB_ICONWARNING)
#define MBYN(x) ::MessageBox(GetSafeHwnd(), x,LoadS(IDS_TIP), MB_YESNO|MB_ICONQUESTION)
#define MBHYN(x, y) MessageBox(x, LoadS(y),LoadS(IDS_TIP), MB_YESNO|MB_ICONQUESTION)

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#define ENABLE_PWD_CRY

#include "../encryptlib/PWDCry.h"
#include <afxdhtml.h>
// #ifdef _DEBUG
// #pragma comment(lib,"../debug/encryptlib.lib")
// #else
// #pragma comment(lib,"../release/encryptlib.lib")
// #endif

//FTP�û���������
#define FTP_USERNAME					_T("isecftpuser")
#define FTP_PASSWORD					_T("123456")

extern BOOL AddObjectAccess(CString strFileName, const CSid &rSid, ACCESS_MASK accessmask,SE_OBJECT_TYPE eType /*= SE_OBJECT_TYPE*/);
extern BOOL AddRegAccess(CString szRegPath);
extern CString LoadS(UINT nID);

extern CIniFile g_oIniFileInstall;
extern CIniFile g_oIniFileConfig;
extern CIniFile g_oIniFileSet;
extern CIniFile g_oIniFileCurretnConfig;	//config_X.ini
extern CIniFile g_oIniFileClientSetup;
extern CString g_sDefaultPath;
extern CString g_sInstallPath;
extern CString g_sInstallSourcePath;
extern BOOL g_bDbBuildIn;
extern ADO_INFO g_oAdoInfo;
