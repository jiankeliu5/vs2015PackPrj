
// Install.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Install.h"
#include "InstallDlg.h"
#include "WebSheet.h"
#include "CreateDbDlg.h"
#include "setipdlg.h"
#include "SetDbDlg.h" 
#include "EncryptDlg.h"
#include <Tlhelp32.h>
#include <atlsecurity.h>
#include "NtDriverController.h"
#include "ConfigPrtDlg.h"
#include "CeleritySettingDlg.h"
#include "ExportDataDlg.h"
#include "setuptipdlg.h"
#include "Registry2.h"	//add by zfq,2013.01.06
#include "DlgCfgPrtCenterSvr.h"	//add by zfq,2013.02.16
#include "ConfigEmbedWeb.h"//add by szy 20150421
#include "EmbedWebSheet.h"
#include "BaseSock.h"
#include "DlgConfigEmbeded.h"
#include "WebServiceHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL AddObjectAccess(CString strFileName, const CSid &rSid, ACCESS_MASK accessmask,SE_OBJECT_TYPE eType /*= SE_OBJECT_TYPE*/);
BOOL AddRegAccess(CString szRegPath);


// CInstallApp

BEGIN_MESSAGE_MAP(CInstallApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CInstallApp 构造

CInstallApp::CInstallApp()
{
	m_nExitCode = 0;
	m_bQueit = TRUE;
	m_bDataBaseInstalled;
	m_bCoreServerInstalled;
	m_bWebServerInstalled;
	m_bSharePrintServerInstalled;
	m_bPrintStationInstalled;
	m_bKMEmbedPrintInstalled;
	m_bHPEmbedPrintInstalled;
}

CFileLog theLog;
CIniFile g_oIniFileInstall;		//install.ini
CIniFile g_oIniFileConfig;
CIniFile g_oIniFileSet;
CIniFile g_oIniFileCurretnConfig;	//config_X.ini
CIniFile g_oIniFileClientSetup;
CString g_sDefaultPath;
CString g_sInstallPath;
CString g_sInstallSourcePath;
BOOL g_bDbBuildIn = TRUE;	//使用内置数据库SqlExpress
ADO_INFO g_oAdoInfo;	//保存第三方数据库的配置信息
CConfigSettingHelper theConfig;


// 唯一的一个 CInstallApp 对象

CInstallApp theApp;
// CInstallApp 初始化


BOOL CInstallApp::InitInstance()
{
	CFileLog::EnableLog(TRUE);
	theLog.SetLogName(_T("install"));

// 	CString sPath;
// 	sPath.Format(_T(_T("%s")),_T("MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\XabVPrinter"));
// 
// 	AddObjectAccess(sPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_REGISTRY_KEY);


	g_sDefaultPath = CCommonFun::GetDefaultPath();

	theConfig.SetCurrentConfig(_T(""));

	//------------------
	//安装程序路径
	CString sKey;sKey.Format(_T("%s%s"), PROJECT_REGISTY_KEY, PROJECT_PATH);
	CRegistryEx reg2;
	reg2.SetRootKey(HKEY_LOCAL_MACHINE);
	if(reg2.Open(sKey))
	{
		reg2.Read(_T("SourcePath"),g_sInstallSourcePath);
		reg2.Read(_T("InstallPath"), g_sInstallPath);
		reg2.Read(_T("InstallDataBase"), m_bDataBaseInstalled);
		reg2.Read(_T("InstallCoreServer"), m_bCoreServerInstalled);
		reg2.Read(_T("InstallWebServer"), m_bWebServerInstalled);
		reg2.Read(_T("InstallSharePrintServer"), m_bSharePrintServerInstalled);
		reg2.Read(_T("InstallPrintStation"), m_bPrintStationInstalled);
		reg2.Read(_T("InstallEmbedPrintWebHP"), m_bHPEmbedPrintInstalled);
		reg2.Read(_T("InstallEmbedPrintWebKM"), m_bKMEmbedPrintInstalled);
	}
	reg2.Close();

	if (g_sInstallSourcePath.IsEmpty())
	{
		g_sInstallSourcePath = g_sDefaultPath;
	}

	if (g_sInstallPath.IsEmpty())
	{
		g_sInstallPath = g_sDefaultPath;
	}

	g_sInstallSourcePath.TrimRight(_T("\\"));
	g_sInstallPath.TrimRight(_T("\\"));

	theLog.Write(_T("g_sInstallSourcePath = %s"),g_sInstallSourcePath);
	theLog.Write(_T("g_sInstallPath = %s"),g_sInstallPath);

	GetAdoInfo();
	theLog.Write(_T("g_bDbBuildIn = %d"),g_bDbBuildIn);
	
	//初始化配置文件路径
	g_oIniFileInstall.m_sPath.Format(_T("%sinstall.ini"),g_sDefaultPath);
	m_bQueit = g_oIniFileInstall.GetVal(_T("Install"),_T("Quiet"),TRUE);
	theLog.Write(_T("m_bQueit=%d"), m_bQueit);

	g_oIniFileConfig.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
	g_oIniFileSet.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_SETTINGS_FILE);
	g_oIniFileCurretnConfig.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
	g_oIniFileClientSetup.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_ClientSetup_FILE);

	COleDateTime dt = COleDateTime::GetCurrentTime();
	m_szThisBackupDir.Format(_T("%s\\BackupFiles\\%d%02d%02d%02d%02d%02d")
			, g_sInstallPath, dt.GetYear(), dt.GetMonth(), dt.GetDay()
			, dt.GetHour(), dt.GetMinute(), dt.GetSecond());

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	::CoInitialize(NULL);
	if (!AfxSocketInit())
	{
		return FALSE;
	}

	//解析参数
	int nRet = ParseParam();
	SetErrCode(nRet, _T(""));
	return TRUE;

#if 0
	{
		CExportDataDlg a;
		a.DoModal();
		CConfigPrtDlg c;
		c.DoModal();
		CInstallPrtDlg dlg;
		dlg.DoModal();
	}
#endif

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
#ifdef _DEBUG
	CInstallDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
#endif
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

CString LoadS(UINT nID)
{
	CString s;
	s.LoadString(nID);
	return s;
}

void RemoveUsers( ATL::CDacl &pDacl)
{
	for(UINT i = 0; i < pDacl.GetAceCount(); i++)
	{
		ATL::CSid pSid;
		ACCESS_MASK pMask = 0;
		BYTE pType = 0, pFlags = 0;
		/* Get the ith ACL */

		const_cast<ATL::CDacl &>(pDacl).GetAclEntry(i, &pSid, &pMask, &pType, &pFlags);

		CString str(pSid.AccountName());

		if (str.CompareNoCase(_T("Users")) == 0 )
		{
			pDacl.RemoveAce(i);
		}
	}
}
BOOL AddRegAccess(CString szRegPath)
{
	DWORD dwRet;

	// 下面这个字符串的值修改为想要进行权限操作的注册表项，注册表每一级的权限是不一样的，所以需要很具体地指定到某一级
	TCHAR pReg[100] = {0};
	_stprintf_s(pReg, 100, _T("MACHINE\\%s"), szRegPath);
	theLog.Write(_T("AddRegAccess,1,pReg=[%s]"), pReg);

	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pOldDacl = NULL;
	PACL pNewDacl = NULL;
	EXPLICIT_ACCESS ea;
	HKEY hKey = NULL;

	// 获取SAM主键的DACL
	dwRet = GetNamedSecurityInfo(pReg, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDacl, NULL, &pSD);
	if (dwRet != ERROR_SUCCESS)
	{
		theLog.Write(_T("!!AddRegAccess,2,GetNamedSecurityInfo Error: %d"), dwRet);
		goto FreeAndExit;
	}

	// 创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS,
		SUB_CONTAINERS_AND_OBJECTS_INHERIT);

	// 将新的ACE加入DACL
	dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl);
	if (dwRet != ERROR_SUCCESS)
	{
		theLog.Write(_T("!!AddRegAccess,3,SetEntriesInAcl Error: %d"), dwRet);
		goto FreeAndExit;
	}

	// 更新SAM主键的DACL
	dwRet = SetNamedSecurityInfo(pReg, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDacl, NULL);
	if (dwRet != ERROR_SUCCESS)
	{
		theLog.Write(_T("!!AddRegAccess,4,SetNamedSecurityInfo Error: %d"), dwRet);
		goto FreeAndExit;
	}

	theLog.Write(_T("AddRegAccess,5,SetNamedSecurityInfo Successfully."));

	return TRUE;

FreeAndExit:
	if (hKey) RegCloseKey(hKey);
	if (pNewDacl) LocalFree(pNewDacl);
	// 还原SAM主键的DACL
	if (pOldDacl) SetNamedSecurityInfo(pReg, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,
		NULL, NULL, pOldDacl, NULL);
	if (pSD) LocalFree(pSD);
	return FALSE;
}

BOOL AddObjectAccess(CString strFileName, const CSid &rSid, ACCESS_MASK accessmask,SE_OBJECT_TYPE eType /*= SE_OBJECT_TYPE*/)
{
	ATL::CSecurityDesc OutSecDesc;
	ATL::AtlGetSecurityDescriptor(strFileName, eType, &OutSecDesc);
	OutSecDesc.MakeAbsolute();
	bool pbPresent = 0;

	ATL::CDacl pDacl;
	if (!OutSecDesc.GetDacl(&pDacl, &pbPresent))
		return FALSE;

	RemoveUsers(pDacl);

	if (!pDacl.AddAllowedAce(rSid, accessmask,  CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE))
	{
		//已经把已有的删掉了！
		return FALSE;
	}

	OutSecDesc.SetControl(SE_DACL_AUTO_INHERITED | SE_DACL_PROTECTED, SE_DACL_AUTO_INHERITED);
	ATL::AtlSetDacl(strFileName, eType, pDacl);
	ATL::CSacl pSacl;
	/* We've already set the Dacl. Now set the SACL. */
	OutSecDesc.GetSacl(&pSacl, &pbPresent);
	if(pbPresent)
	{
		ATL::AtlSetSacl(strFileName, eType, pSacl);
	}
	return TRUE;
}
void Install(CString sPath,CString sParam,DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/, BOOL bDebuglog /*= TRUE*/)
{
	int nLen = sPath.GetLength() + sParam.GetLength() + 10;
	TCHAR* pszPath = new TCHAR[nLen];
	ASSERT(pszPath);
	memset(pszPath,0,nLen * sizeof(TCHAR));

	_stprintf(pszPath,_T("\"%s\" %s"),sPath,sParam);
	STARTUPINFO si = { sizeof si, 0, _T("") };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = dwShowCmd;

	if (bDebuglog)
	{
		theLog.Write(_T("Install pathandParam = %s"),pszPath);
	}
	else
	{
		theLog.Write(_T("Install sPath = %s"),sPath);
	}
	PROCESS_INFORMATION pi;
	if(CreateProcess(0, 
		pszPath,// command line
		0, 0,  // security attributes
		FALSE, // handle inheritance
		0,     // creation flags
		0,     // environment block
		0,     // current directory
		&si, &pi ))
	{
		theLog.Write(_T("createprocess pid = %d"),pi.dwProcessId);
		//	if(bWait)
		{
			::WaitForSingleObject(pi.hProcess,dwWaitMilliseconds);
		}
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}
	else
	{
		CString s;
		s.Format(_T("Install 失败 err = %d \n"),::GetLastError());
		theLog.Write(s);
	}
	if (pszPath)
	{
		delete pszPath;
		pszPath = NULL;
	}
}

BOOL TerminaPID(DWORD dw)
{
	DWORD fResult  = FALSE;
	HANDLE hProc  = NULL;

	__try
	{
		CCommonFun::ModifyPrivilege(SE_DEBUG_NAME,TRUE);
		// Open the process with PROCESS_QUERY_INFORMATION access
		hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dw);
		if (hProc == NULL)
		{
			__leave;
		}
		fResult = ::TerminateProcess(hProc,0);
		::CloseHandle(hProc);
	}
	__finally
	{
	}
	return fResult;
}

void FindTerminateProcess(CString strProcessName)
{
	//查找
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		strProcessName.MakeUpper();
		PROCESSENTRY32 ProcessEntry;  
		ZeroMemory(&ProcessEntry,sizeof(PROCESSENTRY32));
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		BOOL bFlag = Process32First(hSnapshot,&ProcessEntry);
		while(bFlag)
		{
			CString strExePath	= ProcessEntry.szExeFile;
			strExePath.MakeUpper();
			if(strExePath.Find(strProcessName) != -1)
			{
				TerminaPID(ProcessEntry.th32ProcessID);
			}
			ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
			bFlag = Process32Next(hSnapshot,&ProcessEntry);
		}
		CloseHandle(hSnapshot);
	}
}

void CInstallApp::SetErrCode(int nCode,CString sErr)
{
	m_nExitCode = nCode;
	theLog.Write(_T("ErrCode = %d,%s"),nCode,sErr);
}

//add by zfq,2013.01.06,当前用户是否用域账户登录计算机
BOOL IsLogonByDomainUser()
{
	CRegistry2 oReg(0, TRUE);
	CString szRegPath = _T("Volatile Environment");
	if(!oReg.Open(szRegPath))
	{
		theLog.Write(_T("!!IsLogonByDomainUser,1,Open FAIL,err=%d,szRegPath=%s"), GetLastError(), szRegPath);
		return FALSE;
	}

	CString szValue;
	if(!oReg.Read(_T("USERDNSDOMAIN"), szValue))
	{
		theLog.Write(_T("!!IsLogonByDomainUser,2,read FAIL,err=%d,szRegPath=%s\\USERDNSDOMAIN"), GetLastError(), szRegPath);
		return FALSE;
	}

	if(szValue.IsEmpty())
	{
		return FALSE;
	}

	return TRUE;
}

int CInstallApp::ParseParam(void)
{
//	theLog.Write(_T("##CInstallApp::ParseParam,1,__argc=%d"), __argc);
	for(int n = 0; n < __argc; n ++)
	{
		CString s;
		s.Format(_T("CInstallApp启动参数:%s"), __Argv[n]);
		theLog.Write(s);
	}

	if(__argc < 2)
		return FALSE;

	theLog.Write(_T("##CInstallApp::ParseParam,2,__Argv[1]=%s"), __Argv[1]);

	CDrvInstallHelper helper;
	//安装虚拟打印机
	if(_tcsicmp(__Argv[1],_T("-InstallPrinter")) == 0 || _tcsicmp(__Argv[1],_T("/InstallPrinter")) == 0)
	{
		helper.CallFun("InstallPrinter");
		return TRUE;
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallPrinter")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallPrinter")) == 0)
	{
		helper.CallFun("UnInstallPrinter");
		return TRUE;
	}
	//安装共享打印监视端口
	if(_tcsicmp(__Argv[1],_T("-InstallPrinterSharePort")) == 0 || _tcsicmp(__Argv[1],_T("/InstallPrinterSharePort")) == 0)
	{
		helper.CallFun("InstallPrinterSharePort");
		return TRUE;
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallPrinterSharePort")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallPrinterSharePort")) == 0)
	{
		helper.CallFun("UnInstallPrinterSharePort");
		return TRUE;
	}
	else if(_tcsicmp(__Argv[1],_T("-ConfigPrinter")) == 0 || _tcsicmp(__Argv[1],_T("/ConfigPrinter")) == 0)
	{
		helper.CallFun("ConfigPrinter");
		return TRUE;
	}
	//安装打印客户端
	else if(_tcsicmp(__Argv[1],_T("-InstallClient")) == 0 || _tcsicmp(__Argv[1],_T("/InstallClient")) == 0)
	{
		//设置防火墙
		theLog.Write(_T("设置防火墙"));
		//
		//x64
		//if(CCommonFun::IsWow64() && CCommonFun::IsWinVistaOrLater())
		if (CCommonFun::IsWindowsX64())
		{
			CString sPath,sParam;
			sPath.Format(_T("%sx64\\Install.exe"),g_sDefaultPath);

			sParam.Format(_T("-InstallPrinter"));
			Install(sPath,sParam,INFINITE,SW_HIDE);
		}
		else
		{
			helper.CallFun("InstallPrinter");
		}


		CWinFireWallOp firewall;
		if (firewall.IsWinFireWallEnabled())
		{
			if (firewall.IsExceptModeEnabled())
			{
				firewall.EnableExceptMode();
			}
		}
		CString sExPath;
		sExPath.Format(_T("%siCltPrintHost.exe"),g_sDefaultPath);
		firewall.ExceptProgram(sExPath);

		if(g_oIniFileConfig.GetVal(_T("ClientConfig"), _T("SvrIP"), _T("")).IsEmpty())
		{
			CString szServerIP;
			int nPos = g_sInstallSourcePath.ReverseFind('\\');
			CString szFileName = g_sInstallSourcePath.Mid(nPos+1);
			szFileName.Trim();
			//192.168.2.153-new_client.msi
			//192.168.2.1_ClientSetupHP-201801009-带签名.msi
			//192.168.2.226.msi
			//分别获取取“-”和“-”的位置，然后选择第一个出现的位置作为服务器IP的分隔点，
			//如果没有找到“-”和“-”，则使用文件名作为服务器IP。
			nPos = -1;
			int nPos2 = szFileName.Find('-');
			int nPos3 = szFileName.Find('_');
			if (nPos2 <= 0)
			{
				nPos = nPos3;
			}
			if (nPos3 <= 0)
			{
				nPos = nPos2;
			}
			if ((nPos2 > 0) && (nPos3 > 0))
			{
				nPos = min(nPos2, nPos3);
			}
			if (nPos <= 0)
			{
				//暂时不使用文件名作为服务器IP，要求用户手动输入服务器IP。
				//nPos = szFileName.ReverseFind('.');
			}
			if (nPos > 0)
			{
				szServerIP = szFileName.Left(nPos);
			}

			theLog.Write(_T("##CInstallApp::ParseParam,InstallClient,szServerIP=%s,szFileName=%s"), szServerIP, szFileName);
			if (szServerIP.GetLength() <= 0)
			{
				CSetIPDlg dlg(TYPE_PRINTER);
				if(dlg.DoModal() == IDOK)
					theConfig.SaveClientConfig(dlg.m_sIP);
			}
			else
			{
				theConfig.SaveClientConfig(szServerIP);
			}
		}
		
		// 安装服务
		Install(g_sDefaultPath+_T("iCltPrintHost.exe"),_T("-install"),1,SW_HIDE);

		//设置注册表权限
		AddRegAccess(PRODUCT_CUSTOM_REGISTY_KEY);
		AddRegAccess(PRODUCT_CUSTOM_REGISTY_KEY_AMD64);

		//设置权限
		CString sTmpPath;
		sTmpPath.Format(_T("%s%s"),g_sDefaultPath,SYS_CONFIG_DIR);
		AddObjectAccess(sTmpPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		//add by zfq,2013.06.24,begin,将我们的程序目录设置成everyone权限
		sTmpPath.Format(_T("%s"),g_sDefaultPath);
		AddObjectAccess(sTmpPath, ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		//==取windows目录 和 system32目录 
		TCHAR bysSysPath[255]; 
		memset(bysSysPath, 0x0, sizeof(bysSysPath));
		GetSystemDirectory(bysSysPath, MAX_PATH); 
		sTmpPath.Format(_T("%s\\spool"), bysSysPath);
		AddObjectAccess(sTmpPath, ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
		//add by zfq,2013.06.24,end
		return TRUE;
	}
	else if (_tcsicmp(__Argv[1], _T("-ServiceHostTest")) == 0 || _tcsicmp(__Argv[1], _T("/ServiceHostTest")) == 0)
	{
			theLog.Write(_T("安装组件开始"));
			CString szPath;
			szPath.Format(_T("%s%s"), g_sDefaultPath, _T("ServiceHostTest.exe"));
			Install(szPath, _T("-install"), 1, SW_HIDE);
			theLog.Write(_T("安装组件结束"));
	}
	//安装包调用
	else if(_tcsicmp(__Argv[1],_T("-UnInstallClient")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallClient")) == 0)
	{
		theLog.Write(_T("##CInstallApp::ParseParam,20.1"));

#if 0
		if(IsLogonByDomainUser())	//add by zfq,2013.01.06,加此判断,若是域用户登录，则不让其删除
		{
			theLog.Write(_T("##=====CInstallApp::ParseParam,20.11,IsLogonByDomainUser=TRUE"));
			return TRUE;
		}
#endif
		//在完整更新时,用iSecUpdate.exe将运行完整的安装包,这个时候要先结束iSecUpdate.exe,以便完整更新.
		FindTerminateProcess(_T("iSecUpdate.exe"));

		//卸载服务
		Install(g_sDefaultPath+_T("iCltPrintHost.exe"),_T("-uninstall"),INFINITE,SW_HIDE);

		theLog.Write(_T("##CInstallApp::ParseParam,20.2"));

		FindTerminateProcess(_T("iCltPrintHost.exe"));

		theLog.Write(_T("##CInstallApp::ParseParam,20.3"));

		FindTerminateProcess(_T("iSafeClientEx.exe"));
		FindTerminateProcess(_T("iSafeClient_ENU.exe"));	//add by zfq,2013.10.22

		theLog.Write(_T("##CInstallApp::ParseParam,20.4"));
		//卸载打印驱动
		theLog.Write(_T("begin uninstall printer"));
		//
		//x64
		if(CCommonFun::IsWow64() && CCommonFun::IsWinVistaOrLater())
		{
			theLog.Write(_T("##CInstallApp::ParseParam,21.1"));
			CString sPath,sParam;
			sPath.Format(_T("%sx64\\Install.exe"),g_sDefaultPath);
			
			sParam.Format(_T("-UnInstallPrinter"));
			Install(sPath,sParam,INFINITE,SW_HIDE);

		}
		else
		{
			theLog.Write(_T("##CInstallApp::ParseParam,22.1"));

			helper.CallFun("UnInstallPrinter");
		}
		
		theLog.Write(_T("##CInstallApp::ParseParam,23"));

		// 删除PrintJob文件夹
		CString sPath;
		CCommonFun::DeleteDir(CCommonFun::GetDefaultPath()+_T("PrintJob"));

		theLog.Write(_T("##CInstallApp::ParseParam,24"));

			sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_DIR);
			theLog.Write(_T("del %s"),sPath);
			CCommonFun::DeleteDir(sPath);
			sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),PRT_CLIENT_JOB_DIR);
			theLog.Write(_T("del %s"),sPath);
			CCommonFun::DeleteDir(sPath);
		
		theLog.Write(_T("##CInstallApp::ParseParam,28"));

#if 0	//delete by zfq,2013.01.06,先装客户端，再加入域，域用户卸载时会在下面这句报错
		if(PathFileExists(CCommonFun::GetCommonDesktopDir()+_T("\\打印系统查询.lnk")))
			CFile::Remove(CCommonFun::GetCommonDesktopDir()+_T("\\打印系统查询.lnk"));
#else	//add by zfq,2013.01.06
		CString szPrintLink = CCommonFun::GetCommonDesktopDir();
		if(szPrintLink.IsEmpty())
		{
			theLog.Write(_T("!!CInstallApp::ParseParam,28.1,szPrintLink is empty"));
		}
		else
		{
#if 0
			szPrintLink += _T("\\打印系统查询.lnk");
#else
			szPrintLink += _T("\\打印系统查询.url");
#endif
			try
			{
				if(PathFileExists(szPrintLink))
				{
					theLog.Write(_T("##CInstallApp::ParseParam,28.2,szPrintLink=%s"), szPrintLink);
					CFile::Remove(szPrintLink);
					theLog.Write(_T("##CInstallApp::ParseParam,28.3"));
				}
			}
			catch(...)
			{
				theLog.Write(_T("##CInstallApp::ParseParam,28.4,catch sth.szPrintLink=%s"), szPrintLink);
			}
		}
#endif

		theLog.Write(_T("##CInstallApp::ParseParam,29"));

		return TRUE;
	}
	//安装服务器
	else if(_tcsicmp(__Argv[1],_T("-ConfigSvrDb")) == 0 || _tcsicmp(__Argv[1],_T("/ConfigSvrDb")) == 0)
	{
		theLog.Write(_T("配置数据库,begin"));

		g_sDefaultPath.Format(_T("%sCoreServer"), CCommonFun::GetDefaultPath());
		theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
		theConfig.SetCurrentConfig(_T(""));

		CSetDbDlg dlg;
		dlg.DoModal();

		theLog.Write(_T("配置数据库,end"));

		return TRUE;
	}
	//安装服务器
	else if(_tcsicmp(__Argv[1],_T("-checkcry")) == 0 || _tcsicmp(__Argv[1],_T("/checkcry")) == 0)
	{
		EncryptCheck();
		return TRUE;
	}
	//配置web数据库连接
	else if(_tcsicmp(__Argv[1],_T("-ConfigWebDb")) == 0 || _tcsicmp(__Argv[1],_T("/ConfigWebDb")) == 0)
	{
		theLog.Write(_T("配置WEB数据库,begin"));

		g_sDefaultPath.Format(_T("%sCoreServer"), CCommonFun::GetDefaultPath());
		theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
		theConfig.SetCurrentConfig(_T(""));

		CWebSheet sheet(0,_T("打印Web服务器配置向导"));
		sheet.DoModal();

		theLog.Write(_T("配置WEB数据库,end"));

		return TRUE;
	}
	//安装打印站点
	else if(_tcsicmp(__Argv[1],_T("-ConfigPrtCnnt")) == 0 || _tcsicmp(__Argv[1],_T("/ConfigPrtCnnt")) == 0)
	{
		if (__argc == 3)
		{
			theConfig.SetCurrentConfig(__Argv[2]);
		}

		CConfigPrtDlg config;
		config.DoModal();
	}
	else if(_tcsicmp(__Argv[1],_T("-ConfigEmbedWeb")) == 0 || _tcsicmp(__Argv[1],_T("/ConfigEmbedWeb")) == 0)
	{
		if (__argc == 3)
		{
			theConfig.SetCurrentConfig(__Argv[2]);
		}

        ConfigEmbedWeb config;
        config.DoModal();
	}
	//安装数据库
	else if(_tcsicmp(__Argv[1],_T("-InstallDataBase")) == 0 || _tcsicmp(__Argv[1],_T("/InstallDataBase")) == 0 )
	{
		return InstallDataBase();
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallDataBase")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallDataBase")) == 0 )
	{
		return UnInstallDataBase();
	}
	//安装核心服务
	else if(_tcsicmp(__Argv[1],_T("-InstallCoreServer")) == 0 || _tcsicmp(__Argv[1],_T("/InstallCoreServer")) == 0)
	{
		return InstallCoreServer();
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallCoreServer")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallCoreServer")) == 0)
	{
		return UnInstallCoreServer();
	}
	//安装WEB服务
	else if(_tcsicmp(__Argv[1],_T("-InstallWebServer")) == 0 || _tcsicmp(__Argv[1],_T("/InstallWebServer")) == 0)
	{
		return InstallWebServer();
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallWebServer")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallWebServer")) == 0)
	{
		return UnInstallWebServer();
	}
	//安装共享打印服务
	else if(_tcsicmp(__Argv[1],_T("-InstallSharePrintServer")) == 0 || _tcsicmp(__Argv[1],_T("/InstallSharePrintServer")) == 0 )
	{
		return InstallSharePrintServer();
	}
	else if(_tcsicmp(__Argv[1], _T("-UnInstallSharePrintServer")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallSharePrintServer")) == 0 )
	{
		return UnInstallSharePrintServer();
	}	
	//安装打印站点
	else if(_tcsicmp(__Argv[1],_T("-InstallPrintStation")) == 0 || _tcsicmp(__Argv[1],_T("/InstallPrintStation")) == 0 )
	{
		return InstallPrintStation();
	}
	else if(_tcsicmp(__Argv[1],_T("-UnInstallPrintStation")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallPrintStation")) == 0 )
	{
		return UnInstallPrintStation();
	}
	//安装嵌入式web站点
    else if(_tcsicmp(__Argv[1],_T("-InstallEmbedPrintWeb")) == 0 || _tcsicmp(__Argv[1],_T("/InstallEmbedPrintWeb")) == 0)
    {
		return InstallEmbedPrintWeb();
    }
	else if(_tcsicmp(__Argv[1],_T("-UnInstallEmbedPrintWeb")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallEmbedPrintWeb")) == 0)
	{
		return UnInstallEmbedPrintWeb();
	}
	//安装IIS
	else if(_tcsicmp(__Argv[1],_T("-InstallIIS")) == 0 || _tcsicmp(__Argv[1],_T("/InstallIIS")) == 0)
	{
		return InstallIIS();
	}
	//安装嵌入式web(柯美)站点
    else if(_tcsicmp(__Argv[1],_T("-InstallEmbedPrintWebKM")) == 0 || _tcsicmp(__Argv[1],_T("/InstallEmbedPrintWebKM")) == 0)
    {
		return InstallEmbedPrintWebKM();
    }
	else if(_tcsicmp(__Argv[1],_T("-UnInstallEmbedPrintWebKM")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallEmbedPrintWebKM")) == 0)
	{
		return UnInstallEmbedPrintWebKM();
	}
	//安装嵌入式web(惠普)站点
    else if(_tcsicmp(__Argv[1],_T("-InstallEmbedPrintWebHP")) == 0 || _tcsicmp(__Argv[1],_T("/InstallEmbedPrintWebHP")) == 0)
    {
		return InstallEmbedPrintWebHP();
    }
	else if(_tcsicmp(__Argv[1],_T("-UnInstallEmbedPrintWebHP")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallEmbedPrintWebHP")) == 0)
	{
		return UnInstallEmbedPrintWebHP();
	}
	//安装Apache和PHP运行环境
    else if(_tcsicmp(__Argv[1],_T("-InstallApacheAndPHP")) == 0 || _tcsicmp(__Argv[1],_T("/InstallApacheAndPHP")) == 0)
    {
		return InstallApacheAndPHP();
    }
	else if(_tcsicmp(__Argv[1],_T("-UnInstallApacheAndPHP")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallApacheAndPHP")) == 0)
	{
		return UnInstallApacheAndPHP();
	}
	//安装嵌入式打印WEB(佳能)服务
    else if(_tcsicmp(__Argv[1],_T("-InstallEmbedPrintWebCanon")) == 0 || _tcsicmp(__Argv[1],_T("/InstallEmbedPrintWebCanon")) == 0)
    {
		return InstallEmbedPrintWebCanon();
    }
	else if(_tcsicmp(__Argv[1],_T("-UnInstallEmbedPrintWebCanon")) == 0 || _tcsicmp(__Argv[1],_T("/UnInstallEmbedPrintWebCanon")) == 0)
	{
		return UnInstallEmbedPrintWebCanon();
	}
	else if(_tcsicmp(__Argv[1],_T("-Install_VC_Redist_x86_2015")) == 0 || _tcsicmp(__Argv[1],_T("/Install_VC_Redist_x86_2015")) == 0)
	{
		return Install_VC_Redist_x86_2015();
	}
	else if (_tcsicmp(__Argv[1], _T("-InstallDotNet4x")) == 0 || _tcsicmp(__Argv[1], _T("/InstallDotNet4x")) == 0)
	{
		return InstallDotNet4x();
	}
	else if (_tcsicmp(__Argv[1], _T("-CheckDotNet4xInstallEventment")) == 0 || _tcsicmp(__Argv[1], _T("/CheckDotNet4xInstallEventment")) == 0)
	{
		return CheckDotNet4xInstallEventment();
	}
	else if (_tcsicmp(__Argv[1], _T("-InstallDotNet4xCert")) == 0 || _tcsicmp(__Argv[1], _T("/InstallDotNet4xCert")) == 0)
	{
		return InstallDotNet4xCert();
	}
	else if (_tcsicmp(__Argv[1], _T("-UpdateBegin")) == 0 || _tcsicmp(__Argv[1], _T("/UpdateBegin")) == 0)
	{
		if (__argc < 3)
		{
			theLog.Write(_T("CInstallApp::ParseParam,缺少参数"));
		}
		else
		{
			CString szArgc2(__Argv[2]);
			if (szArgc2.Find(_T("2")) >= 0)
				UpdateDataBaseBegin();
			if (szArgc2.Find(_T("3")) >= 0)
				UpdateCoreServerBegin();
			if (szArgc2.Find(_T("4")) >= 0)
				UpdateWebServerBegin();
			if (szArgc2.Find(_T("5")) >= 0)
				UpdateSharePrintServerBegin();
			if (szArgc2.Find(_T("6")) >= 0)
				UpdatePrintStationBegin();
			if (szArgc2.Find(_T("7")) >= 0)
				UpdateEmbedPrintWebHPBegin();
			if (szArgc2.Find(_T("8")) >= 0)
				UpdateEmbedPrintWebKMBegin();
		}
	}
	else if (_tcsicmp(__Argv[1], _T("-UpdateEnd")) == 0 || _tcsicmp(__Argv[1], _T("/UpdateEnd")) == 0)
	{
		if (__argc < 3)
		{
			theLog.Write(_T("CInstallApp::ParseParam,缺少参数"));
		}
		else
		{
			CString szArgc2(__Argv[2]);
			if (szArgc2.Find(_T("2")) >= 0)
				UpdateDataBaseEnd();
			if (szArgc2.Find(_T("3")) >= 0)
				UpdateCoreServerEnd();
			if (szArgc2.Find(_T("4")) >= 0)
				UpdateWebServerEnd();
			if (szArgc2.Find(_T("5")) >= 0)
				UpdateSharePrintServerEnd();
			if (szArgc2.Find(_T("6")) >= 0)
				UpdatePrintStationEnd();
			if (szArgc2.Find(_T("7")) >= 0)
				UpdateEmbedPrintWebHPEnd();
			if (szArgc2.Find(_T("8")) >= 0)
				UpdateEmbedPrintWebKMEnd();
		}
	}
	else
	{
		theLog.Write(_T("!!!!!!!!!!!!!!!!未知的安装命令，%s"), __Argv[1]);
	}

	return FALSE;
}
int CInstallApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	::CoUninitialize();
	CWinAppEx::ExitInstance();
	theLog.Write(_T("ExitInstance %d"),m_nExitCode);
	return m_nExitCode;
}
PVOID m_pOldValue = NULL;
void DisableFileRedirect()
{
	typedef BOOL (WINAPI* PFUN)(PVOID *OldValue);
	PFUN pFun = (PFUN)GetProcAddress(GetModuleHandle(_T("kernel32")),"Wow64DisableWow64FsRedirection");
	if(NULL != pFun)
	{
		pFun(&m_pOldValue);
	}
}

void EnableFileRedirect()
{
	typedef BOOL (WINAPI* PFUN)(PVOID OldValue);
	PFUN pFun = (PFUN)GetProcAddress(GetModuleHandle(_T("kernel32")),"Wow64RevertWow64FsRedirection");
	if(NULL != pFun)
	{
		pFun(m_pOldValue);
	}
}

BOOL Enable32BitAppOnWin64(BOOL bEnable)
{
	TCHAR chPath[MAX_PATH];
	GetWindowsDirectory(chPath,MAX_PATH);

	CString sPath;
	sPath.Format(_T("%c:\\inetpub\\adminscripts\\adsutil.vbs"),chPath[0]);
	CFileFind fd;

	if( !fd.FindFile(sPath) )
	{
		MessageBox(NULL,_T("没有找到")+sPath+_T("文件，设置32位运行环境失败。"),_T("提示"),MB_OK);
		return FALSE;
	}
	CString sTmp;
	if( bEnable )
	{
		sTmp.Format(_T("%s set W3SVC/AppPools/Enable32BitAppOnWin64 \"true\""),sPath);
	}
	else
	{
		sTmp.Format(_T("%s set W3SVC/AppPools/Enable32BitAppOnWin64 \"false\""),sPath);
	}

	TCHAR szDir[MAX_PATH*2+16]={0};

	_tcscat(szDir,_T("CSCRIPT.EXE "));
	_tcscat(szDir,sTmp);

	STARTUPINFO StartupInfo= {0};
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb=sizeof(STARTUPINFO); 

	if (!CreateProcess(NULL, (LPTSTR)szDir, NULL,NULL,FALSE,0,NULL,NULL, &StartupInfo, &ProcessInfo))
		return FALSE;

	WaitForSingleObject(ProcessInfo.hProcess ,3000);
	CloseHandle (ProcessInfo.hThread);
	CloseHandle (ProcessInfo.hProcess); 

	return TRUE;
}

BOOL MyExecuteCmdComd()
{
	if(!theApp.m_bQueit)
		MessageBox(0,_T("正在注册IIS相关组件，需要几分钟时间，请耐心等待...."),_T("提示"),MB_ICONINFORMATION | MB_OK);

	DisableFileRedirect();


	EnableFileRedirect();

#if 0	//以后WEB全部是基于最低.Net4.5版本
	if (CCommonFun::IsWinWin8OrLater())
	{
		//Windows Server 2012之后，不要注册iis .net2.0
		return TRUE;
	}

	TCHAR chPath[MAX_PATH];
	GetWindowsDirectory(chPath,MAX_PATH);
	CString szPath;
	szPath.Format(_T("%s\\Microsoft.NET\\framework\\v4.0.30319\\aspnet_regiis.exe -i"),chPath);

	TCHAR szDir[MAX_PATH*2+16]={0};
	_tcscat(szDir,szPath);

	STARTUPINFO StartupInfo= {0};

	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb=sizeof(STARTUPINFO); 
	if (!CreateProcess(NULL, (LPTSTR)szDir, NULL,NULL,FALSE,0,NULL,NULL, &StartupInfo, &ProcessInfo))
	{
		return FALSE;
	}
	if(WAIT_TIMEOUT == WaitForSingleObject(ProcessInfo.hProcess ,1000*100))
	{
		AfxMessageBox(_T("注册IIS相关组件超时!"));
		return FALSE;
	}
	CloseHandle (ProcessInfo.hThread);
	CloseHandle (ProcessInfo.hProcess); 
#endif

	return TRUE;
}

BOOL MyStartAspnet_StateService()
{
	BOOL bResult = FALSE;

	CRegistryEx pReg;
	pReg.SetRootKey(HKEY_LOCAL_MACHINE);
	if(pReg.Open(_T("SYSTEM\\CurrentControlSet\\Services\\aspnet_state")))
	{
		pReg.Write(_T("Start"),(DWORD)2);
		pReg.Close();
	}
	else
	{
		return bResult;
	}

	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM != NULL)
	{
		SC_HANDLE hService = OpenService(hSCM, _T("aspnet_state"), SERVICE_START);
		if (hService != NULL)
		{
			if(StartService(hService,0,NULL))
			{
				bResult = TRUE;
			}
			else
			{
				if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
				{
					bResult = TRUE;
				}
			}
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hSCM);
	}
	return bResult;
}

void CInstallApp::DelEncrypFile()
{
	//删除加密文件
	CStringArray sAry;
	sAry.Add(_T("winacl.dll"));
	sAry.Add(_T("winemm.dll"));
	sAry.Add(_T("winfc8.dll"));
	sAry.Add(_T("winmc8.dll"));

	CString sDes;
	for( int i=0;i<sAry.GetSize();i++)
	{
		sDes = g_sDefaultPath + _T("\\") + sAry.GetAt(i);
		BOOL b = ::DeleteFile(sDes);
		theLog.Write(_T("del secfile %d,%s"),b,sDes);
	}
}
BOOL CInstallApp::CopyEnCryptFiles()
{
	CStringArray sAry;
	sAry.Add(_T("winacl.dll"));
	sAry.Add(_T("winemm.dll"));
	sAry.Add(_T("winfc8.dll"));
	sAry.Add(_T("winmc8.dll"));

	CString sDes,sSour;
	for( int i=0;i<sAry.GetSize();i++)
	{
		sSour = g_sInstallSourcePath + _T("\\") + sAry.GetAt(i);	
		sDes = g_sDefaultPath + _T("\\") + sAry.GetAt(i);

		theLog.Write(_T("sSour=%s,sDes=%s"),sSour,sDes);

		if(sSour.CompareNoCase(sDes) == 0) // 修复时会出现此情况
			break;
		//去掉只读属性
		CFileStatus status;
		if(CFile::GetStatus(sDes, status))
		{
			status.m_attribute &= ~0x01;
			try
			{
				CFile::SetStatus(sDes, status);
			}
			catch(...)
			{
				//CFileException 拒绝访问？
			}
		}
 		if( !CopyFile(sSour,sDes,FALSE) )
		{
			if( IDRETRY == MessageBox(NULL,_T("拷贝加密文件失败！"),_T("提示"),MB_ICONERROR|MB_RETRYCANCEL))
			{
				i+=-1;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

typedef int (* LP_SetupA)(HWND hwnd,LPLONG pInput1,LPSTR pInput2); 
typedef int (* LP_SetupB)(HWND hwnd,LPLONG pInput1,LPSTR pInput2); 
typedef int (* LP_SetupB)(HWND hwnd,LPLONG pInput1,LPSTR pInput2); 
BOOL CInstallApp::EncryptCheck()
{     
	BOOL bOk = FALSE;
	HMODULE hmd = LoadLibrary(_T("xinstall.dll"));
	if(hmd == NULL)
		return FALSE;

	//判断版本
	LP_SetupB pSetupB = NULL;
	pSetupB = (LP_SetupB)GetProcAddress((HMODULE)hmd,"setupb");
	if(pSetupB == NULL)
	{
		FreeModule(hmd);
		return FALSE;
	}

	LONG nVer,nValue;
	CString sSetupExe;

	sSetupExe.Format(_T("%s\\setup.exe"),g_sDefaultPath);		
	nVer = pSetupB(NULL,&nValue,(LPSTR)(LPCTSTR)sSetupExe);

	CString sTmp;
	sTmp.Format(_T("file=%s version=%d"),sSetupExe,nVer);
	theLog.Write(sTmp);

	if(nVer == 101)//demo version		
	{
		FreeModule(hmd);
		//m_nExitCode = 0;
		return	TRUE;//demo version need nod to check serial number
	}

	//安装加密狗驱动
	IntallDogDrv();

	if(nVer != 1)
	{
		MBHW(NULL, IDS_WRONG_VERSION);
		FreeModule(hmd);
		return FALSE;
	}

	//判断序列号
	LP_SetupA pSetupA = NULL;
	pSetupA = (LP_SetupA)GetProcAddress((HMODULE)hmd,"setupa");
	if(pSetupA == NULL)
	{
		FreeModule(hmd);
		return FALSE;
	}

	int nTry = 3;	
	CString sSerial;
	while(nTry > 0)
	{
		CEncryptDlg dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			sSerial = dlg.m_sSerial;
			long nTemp,nRet;
			nRet = pSetupA(NULL,&nTemp,(LPSTR)(LPCTSTR)sSerial);

			sTmp.Format(_T("Serial=%s version=%d"),sSerial,nRet);
			theLog.Write(sTmp);
			if(nRet == 101)
			{
				bOk = TRUE;
				break;
			}
			else
			{
				MBHW(NULL, IDS_WRONG_SERIAL);
			}			
		}
		else if (nResponse == IDCANCEL)
		{
			break;
		}	
		nTry--;	
	}

	FreeModule(hmd);

	if(!bOk)
	{
		MBHW(NULL, IDS_WRONG_SERIAL);
	}
	else
	{
		//将序列号写入注册表
// 		CRegistry2 general(2,FALSE);
// 		CString sKey = PROJECT_REGISTY_KEY;	 
// 		if( general.CreateKey(sKey) )
// 		{		
// 			// Database
// 			general.Write(_T("SerialNumber"),sSerial); 
// 		}
	}

	return bOk;
}

CString LoadS(int nText)
{
	CString s;
	s.LoadString(nText);
	return s;
}
//安装加密狗驱动
void CInstallApp::IntallDogDrv()
{
	theLog.Write(_T("install dog begin"));
	CString sPath;
	CString sparam;
#if 0	//del by zfq,2013.07.08
	sPath.Format(_T("%ssetup.exe"),g_sDefaultPath);
#else	//add by zfq,2013.07.08
	if(CCommonFun::IsWow64())
	{
		sPath.Format(_T("%s\\x64\\setup.exe"),g_sDefaultPath);
		theLog.Write(_T("CInstallApp::IntallDogDrv,2,wow64"));
	}
	else
	{
		sPath.Format(_T("%s\\setup.exe"),g_sDefaultPath);
		theLog.Write(_T("CInstallApp::IntallDogDrv,3,x86"));
	}
#endif
	sparam.Format(_T(" /i/u/s"));
	Install(sPath,sparam,INFINITE,SW_HIDE);
	theLog.Write(_T("install dog suc"));
}

void CInstallApp::CreateServerKey(LPCTSTR pszData, LPCTSTR pszData2)
{
	HKEY hKey = NULL;
	DWORD dw;
	do 
	{
		LONG ReturnValue = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\iSecUpdaterSvr\\Parameters"), 0L, NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
		if(ReturnValue != ERROR_SUCCESS)
			break;

		DWORD dwSize = (_tcsclen(pszData) + 1) * sizeof(TCHAR);
		ReturnValue = RegSetValueEx (hKey, _T("Application"), 0L, REG_SZ,(CONST BYTE*) pszData, dwSize);
		if(ReturnValue != ERROR_SUCCESS)
			break;

		RegCloseKey(hKey);
		ReturnValue = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\iSecUpdaterHttp\\Parameters"), 0L, NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
		if(ReturnValue != ERROR_SUCCESS)
			break;

		DWORD dwSize2 = (_tcsclen(pszData2) + 1) * sizeof(TCHAR);
		ReturnValue = RegSetValueEx (hKey, _T("Application"), 0L, REG_SZ,(CONST BYTE*) pszData2, dwSize2);
		if(ReturnValue != ERROR_SUCCESS)
			break;
	} while (0);
	if (hKey)
		RegCloseKey(hKey);
}

void CInstallApp::DeleteServerKey()
{
	HKEY hKey = NULL;
	DWORD dw;

	LONG ReturnValue = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\iSecUpdaterSvr\\Parameters"), 0L, NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
	if(ReturnValue == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey,_T("Application"));
		RegCloseKey(hKey);
	}
	
	ReturnValue = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\iSecUpdaterHttp\\Parameters"), 0L, NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);
	if(ReturnValue == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey,_T("Application"));
		RegCloseKey(hKey);
	}
}

//获取第三方数据库配置信息
void CInstallApp::GetAdoInfo()
{
	memset(&g_oAdoInfo, 0x0, sizeof(ADO_INFO));

	CString szServerIP;
	CString szServerPort;
	CString szDbName;
	CString szDbInstance;
	CString szUser;
	CString szPassword;
	CString szDbType;
	CString szDbBuildIn;

	CString sKey;
	sKey.Format(_T("%s%s"), PROJECT_REGISTY_KEY, PROJECT_PATH);

	CRegistry2 reg(2, FALSE);	//HKEY_LOCAL_MACHINE
	BOOL bRet = reg.Open(sKey);
	if (bRet)
	{
		reg.Read(_T("DbIP"), szServerIP);
		if (szServerIP.GetLength() <= 0)
		{
			szServerIP = _T("localhost");
		}

		reg.Read(_T("DbPort"), szServerPort);
		if (szServerPort.GetLength() <= 0)
		{
			szServerPort = _T("1433");
		}

#if 0
		reg.Read(_T("DbName"), szDbName);
		if (szDbName.GetLength() <= 0)
		{
			szDbName = _T("master");
		}
#else
		szDbName = PRINT_DB_NAME;
#endif

		reg.Read(_T("DbInstance"), szDbInstance);
		if (szDbInstance.GetLength() <= 0)
		{
			szDbInstance = _T("XABSQLEXPRESS");
		}

		reg.Read(_T("DbUser"), szUser);
		if (szUser.GetLength() <= 0)
		{
			szUser = _T("sa");
		}

		reg.Read(_T("DbPassword"),szPassword);
		if (szPassword.GetLength() > 0)
		{
			char cPwd[MAX_PATH] = {0};
			int nSize = MAX_PATH;
			CPWDCry cry;
#ifdef UNICODE
			CStringA szPasswordA = CCommonFun::UnicodeToMultiByte(szPassword);
			cry.UnCryptPWD(szPasswordA.GetBuffer(), cPwd, nSize);
			szPasswordA.ReleaseBuffer();
			szPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
			cry.UnCryptPWD(szPassword.GetBuffer(), cPwd, nSize);
			szPassword.ReleaseBuffer();
			szPassword = cPwd;
#endif // UNICODE
		}

		reg.Read(_T("DbType"), szDbType);
		if (szDbType.GetLength() <= 0)
		{
			szDbType = _T("0");
		}

		reg.Read(_T("DbBuildIn"), szDbBuildIn);
		if (szDbBuildIn.GetLength() <= 0)
		{
			szDbBuildIn = _T("1");	//默认为内置SqlServer数据库
		}
	}

	_tcscpy_s(g_oAdoInfo.szIP, szServerIP);
	_tcscpy_s(g_oAdoInfo.szPort, szServerPort);
	_tcscpy_s(g_oAdoInfo.szInstanceName, szDbInstance);
	_tcscpy_s(g_oAdoInfo.szDbName, szDbName);
	_tcscpy_s(g_oAdoInfo.szUser, szUser);
	_tcscpy_s(g_oAdoInfo.szPwd, szPassword);
	g_oAdoInfo.nDbType = _tstoi(szDbType);	

	g_bDbBuildIn = _tstoi(szDbBuildIn);
}

//安装IIS,要求win7和win server2008
BOOL CInstallApp::InstallIIS()
{
	theLog.Write(_T("InstallIIS, begin"));

	if (CCommonFun::IsWinWin8OrLater())
	{
		theLog.Write(_T("InstallIIS, IsWinWin8OrLater"));
		return InstallIIS2012();
	}
	else
	{
		theLog.Write(_T("InstallIIS, IsWinWin7Or2008"));
		//参考文档
		//https://technet.microsoft.com/zh-cn/library/cc725762(v=ws.11).aspx
		//https://technet.microsoft.com/zh-cn/library/cc771209(v=ws.11).aspx
		CString szExePath(_T("pkgmgr"));
		CString szParam;
		szParam.Format(_T("/iu:IIS-WebServerRole;IIS-WebServer;IIS-CommonHttpFeatures;IIS-StaticContent;")
			_T("IIS-DefaultDocument;IIS-DirectoryBrowsing;IIS-HttpErrors;IIS-HttpRedirect;IIS-ApplicationDevelopment;")
			_T("IIS-ASPNET;IIS-NetFxExtensibility;IIS-ASP;IIS-ISAPIExtensions;IIS-ISAPIFilter;IIS-ServerSideIncludes;")
			_T("IIS-HealthAndDiagnostics;IIS-HttpLogging;IIS-LoggingLibraries;IIS-RequestMonitor;IIS-HttpTracing;")
			_T("IIS-CustomLogging;IIS-ODBCLogging;IIS-Security;IIS-BasicAuthentication;IIS-WindowsAuthentication;")
			_T("IIS-DigestAuthentication;IIS-ClientCertificateMappingAuthentication;IIS-IISCertificateMappingAuthentication;")
			_T("IIS-URLAuthorization;IIS-RequestFiltering;IIS-IPSecurity;IIS-Performance;IIS-WebServerManagementTools;")
			_T("IIS-ManagementConsole;IIS-ManagementScriptingTools;IIS-ManagementService;IIS-IIS6ManagementCompatibility;")
			_T("IIS-Metabase;IIS-WMICompatibility;IIS-LegacyScripts;IIS-LegacySnapIn;WAS-WindowsActivationService;")
			_T("WAS-ProcessModel;WAS-NetFxEnvironment;WAS-ConfigurationAPI;IIS-HttpCompressionDynamic;IIS-CGI;IIS-WebDAV;")
			_T("IIS-FTPServer;IIS-FTPSvc;IIS-FTPExtensibility"));
		Install(szExePath, szParam, INFINITE, SW_HIDE);

		InstallDotNet4x();
	}

	theLog.Write(_T("InstallIIS, end"));
	return TRUE;
}

//安装IIS,要求win8和win server2012
BOOL CInstallApp::InstallIIS2012()
{
	theLog.Write(_T("InstallIIS2012 begin"));

	//先安装4.x,再安装iis，否则没有.NET4.0的应用程序池
	//InstallDotNet4x();

	//参考文档
	//http://blog.csdn.net/xdryhp164/article/details/41045795
	//https://docs.microsoft.com/en-us/iis/install/installing-iis-85/installing-iis-85-on-windows-server-2012-r2
	CString szExePath(_T("DISM"));
	//安装IIS
	CString szParamIIS(_T(" /NoRestart /enable-feature /all /online /featureName:IIS-WebServerRole /featureName:IIS-WebServer ")
		_T(" /featureName:IIS-CommonHttpFeatures /featureName:IIS-StaticContent /featureName:IIS-DefaultDocument ")
		_T(" /featureName:IIS-DirectoryBrowsing /featureName:IIS-HttpErrors /featureName:IIS-HttpRedirect ")
		//去除.NET3.5的相关组件，IIS-ASPNET和IIS-NetFxExtensibility，默认就是3.5组件。
		//_T(" /featureName:IIS-ApplicationDevelopment /featureName:IIS-ASPNET /featureName:IIS-NetFxExtensibility ")
		_T(" /featureName:IIS-ApplicationDevelopment ")
		_T(" /featureName:IIS-ASPNET45 /featureName:IIS-NetFxExtensibility45 /featureName:IIS-ASP /featureName:IIS-CGI ")
		_T(" /featureName:IIS-ISAPIExtensions /featureName:IIS-ISAPIFilter /featureName:IIS-ServerSideIncludes ")
		_T(" /featureName:IIS-HealthAndDiagnostics /featureName:IIS-HttpLogging /featureName:IIS-LoggingLibraries ")
		_T(" /featureName:IIS-RequestMonitor /featureName:IIS-HttpTracing /featureName:IIS-CustomLogging ")
		_T(" /featureName:IIS-ODBCLogging /featureName:IIS-Security /featureName:IIS-BasicAuthentication ")
		_T(" /featureName:IIS-WindowsAuthentication /featureName:IIS-DigestAuthentication ")
		_T(" /featureName:IIS-ClientCertificateMappingAuthentication /featureName:IIS-IISCertificateMappingAuthentication ")
		_T(" /featureName:IIS-URLAuthorization /featureName:IIS-RequestFiltering /featureName:IIS-IPSecurity ")
		_T(" /featureName:IIS-Performance /featureName:IIS-HttpCompressionStatic /featureName:IIS-HttpCompressionDynamic ")
		_T(" /featureName:IIS-WebDAV /featureName:IIS-WebServerManagementTools /featureName:IIS-ManagementScriptingTools ")
		_T(" /featureName:IIS-ManagementService /featureName:IIS-IIS6ManagementCompatibility /featureName:IIS-Metabase ")
		_T(" /featureName:IIS-WMICompatibility /featureName:IIS-LegacyScripts /featureName:IIS-FTPServer /featureName:IIS-FTPSvc ")
		_T(" /featureName:IIS-FTPExtensibility /featureName:NetFx4Extended-ASPNET45 /featureName:IIS-ApplicationInit ")
		_T(" /featureName:IIS-WebSockets /featureName:IIS-CertProvider /featureName:IIS-ManagementConsole /featureName:IIS-LegacySnapIn"));
	Install(szExePath, szParamIIS, INFINITE, SW_HIDE);

	//参考文档
	//https://stackoverflow.com/questions/11116134/wcf-on-iis8-svc-handler-mapping-doesnt-work
	/*
	REM install the needed Windows IIS features for WCF
	dism /Online /Enable-Feature /all /FeatureName:WAS-WindowsActivationService
	dism /Online /Enable-Feature /all /FeatureName:WAS-ProcessModel
	dism /Online /Enable-Feature /all /FeatureName:WAS-NetFxEnvironment
	dism /Online /Enable-Feature /all /FeatureName:WAS-ConfigurationAPI
	dism /Online /Enable-Feature /all /FeatureName:WCF-HTTP-Activation
	dism /Online /Enable-Feature /all /FeatureName:WCF-HTTP-Activation45

	REM Feature Install Complete
	pause

	或者：

	dism /Online /Enable-Feature /all /FeatureName:WCF-HTTP-Activation		//此功能应该是.NET Framework 3.5
	dism /Online /Enable-Feature /all /FeatureName:WCF-HTTP-Activation45	//此功能应该是.NET Framework 4.5
	*/
	//启用【.NET Framework 4.5】下面的【WCF服务】中的【HTTP激活】功能，此功能在windows server2012中，惠普嵌入式web会使用到。
	CString szParamWCF;
	szParamWCF.Format(_T(" /NoRestart /online /enable-feature /all /featureName:WCF-HTTP-Activation45 "));
	Install(szExePath, szParamWCF, INFINITE, SW_HIDE);

	theLog.Write(_T("InstallIIS2012 end"));
	return TRUE;
}

//安装.NET3.5
BOOL CInstallApp::InstallDotNet35()
{
	theLog.Write(_T("InstallDotNet35, begin"));

	//dism /online /enable-feature /featurename:NetFx3 
	CString szExePath(_T("dism"));
	CString szParam;
	szParam.Format(_T("/online /enable-feature /featurename:NetFx3"));

	if (CCommonFun::IsWinWin8OrLater())
	{
		//使用本地源来安装DotNet35
		//dism.exe /online /enable-feature /featurename:NetFX3 /All /Source:d:\sources\sxs
		theLog.Write(_T("CInstallApp::InstallDotNet35,IsWinWin8OrLater"));
		CString szSource;
		szSource.Format(_T("%s\\WinRes\\win2012r2\\sources\\sxs"), g_sInstallSourcePath);
		szParam.AppendFormat(_T(" /All /Source:\"%s\" "), szSource);
	}
	Install(szExePath, szParam, INFINITE, SW_HIDE);
	
	theLog.Write(_T("InstallDotNet35, end"));

	return TRUE;
}

//获取已经安装.NET4.x的版本号
CString CInstallApp::GetDotNet4xVersion()
{
	CString szVersion;
	CString sKey(_T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"));
	CRegistryEx reg;
	//设置为只读操作，否则可能一些仅仅是需要读取的操作会在Open时失败
	reg.SetParam(TRUE, TRUE);
	reg.SetRootKey(HKEY_LOCAL_MACHINE);
	if (reg.Open(sKey))
	{
		reg.Read(_T("Version"), szVersion);
	}
	reg.Close();

	theLog.Write(_T("CInstallApp::GetDotNet4xVersion, szVersion=%s"), szVersion);
	return szVersion;
}

//安装.NET4.x,如4.0，4.6等
BOOL CInstallApp::InstallDotNet4x()
{
	theLog.Write(_T("InstallDotNet4x, begin"));

	CString szDotNet4xVersion = GetDotNet4xVersion();
	if (szDotNet4xVersion > _T("4.6"))
	{
		theLog.Write(_T("##InstallDotNet4x, 已经安装%s"), szDotNet4xVersion);
	}
	else
	{
		//安装Net4.x要求的证书，否则Net4.6可能无法安装
		InstallDotNet4xCert();

		CString szDotNet4xPackPath;
		szDotNet4xPackPath.Format(_T("%s\\NDP462-KB3151800-x86-x64-AllOS-ENU.exe"), g_sInstallPath);
		theLog.Write(_T("InstallDotNet4x, szDotNet4xPackPath=%s"), szDotNet4xPackPath);

		//dotNetFx40_Full_x86_x64.exe /q /norestart /ChainingPackage FullX64Bootstrapper
		CString szExePath = szDotNet4xPackPath;
		CString szParam = _T("/q /norestart /ChainingPackage FullX64Bootstrapper");
		Install(szExePath, szParam, INFINITE, SW_HIDE);
	}

	theLog.Write(_T("InstallDotNet4x, end"));

	return TRUE;
}

//检测安装.NET4.x,如4.0，4.6等的系统环境是否符合要求
BOOL CInstallApp::CheckDotNet4xInstallEventment()
{
	theLog.Write(_T("CheckDotNet4xInstallEventment, begin"));

	BOOL bSucc = TRUE;

	//先判断是否已经安装了4.6及以上版本
	CString szDotNet4xVersion = GetDotNet4xVersion();
	if (szDotNet4xVersion > _T("4.6"))
	{
		theLog.Write(_T("##CheckDotNet4xInstallEventment, 已经安装%s"), szDotNet4xVersion);
	}
	else
	{
		bSucc = FALSE;
		//https://docs.microsoft.com/zh-cn/windows/desktop/SysInfo/operating-system-version
		//https://www.cnblogs.com/tough/p/5103713.html
		//操作系统版本和默认安装的.NET版本
		//Windows 10 				10.0*		4.6-4.7 installed by default
		//Windows Server 2019 		10.0*		4.7?
		//Windows Server 2016 		10.0*		4.6?
		//Windows 8.1 				6.3*		4.5.1 installed by default
		//Windows Server 2012 R2 	6.3*		4.5.1 installed by default
		//Windows 8 				6.2			4.5 installed by default
		//Windows Server 2012 		6.2			4.5 installed by default
		//Windows 7 				6.1			includes 3.5.1，but not installed
		//Windows Server 2008 R2 	6.1			includes 3.5.1，but not installed
		//Windows Server 2008 		6.0
		//Windows Vista 			6.0

		//判断当操作系统为/Windows 8.1/Server 2012 R2时，则检测是包含补丁KB2919355
		//如果没有此补丁，则无法安装.NET4.6.x
		OSVERSIONINFO osv;
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osv);
		theLog.Write(_T("*##CheckDotNet4xInstallEventment, dwMajorVersion=%d,dwMinorVersion=%d")
			, osv.dwMajorVersion, osv.dwMinorVersion);
		if (osv.dwMajorVersion == 6 && osv.dwMinorVersion == 3)
		{
#if 0
			CString szResult;
			CCommonFun::RunAndGetResult(_T("systeminfo.exe"), szResult);
			if (szResult.Find(_T("KB2919355")) >= 0)
			{
			}
			else
			{
				bSucc = FALSE;
				theLog.Write(_T("##CheckDotNet4xInstallEventment, 未安装补丁KB2919355"));
			}
#else
			theLog.Write(_T("##CheckDotNet4xInstallEventment, 要求手动安装.NET4.6.x"));
#endif
		}
	}

	theLog.Write(_T("CheckDotNet4xInstallEventment, end, bSucc=%d"), bSucc);

	return bSucc;
}

//安装.NET4.x在安装时要求的证书
BOOL CInstallApp::InstallDotNet4xCert()
{
	theLog.Write(_T("InstallDotNet4xCert, begin"));

	BOOL bSucc = TRUE;

	/*
	CA		【当前用户\中级证书颁发机构\证书】	Certification authority certificates.
	MY		【当前用户\个人\证书】	A certificate store that holds certificates with associated private keys.
	ROOT	【当前用户\受信任的根证书颁发机构\证书】	Root certificates.
	SPC		Software Publisher Certificate.
	*/

	//先检测证书是否存在，如果存在则不要再次安装
	if (CCommonFun::CheckCertificateExist(_T("ROOT")
		, _T("Microsoft Root Certificate Authority 2010")
		, _T("Microsoft Root Certificate Authority 2010")))
	{
		theLog.Write(_T("InstallDotNet4xCert,2,证书[Microsoft Root Certificate Authority 2010]已经安装"));
	}
	else
	{
		CString szCertPath;
		szCertPath.Format(_T("%s\\MicrosoftRootCertificateAuthority2010.cer"), g_sInstallPath);
		theLog.Write(_T("InstallDotNet4xCert,3,szCertPath=%s"), szCertPath);
		bSucc = CCommonFun::ImoprtCertificate(_T("ROOT"), szCertPath);
		if (bSucc)
		{
			//theLog.Write(_T("InstallDotNet4xCert,4,ImoprtCertificate succ"));
		}
		else
		{
			theLog.Write(_T("!!InstallDotNet4xCert,5,ImoprtCertificate fail"));
		}
	}

	//先检测证书是否存在，如果存在则不要再次安装
	if (CCommonFun::CheckCertificateExist(_T("ROOT")
		, _T("Microsoft Root Certificate Authority 2011")
		, _T("Microsoft Root Certificate Authority 2011")))
	{
		theLog.Write(_T("InstallDotNet4xCert,6,证书[Microsoft Root Certificate Authority 2011]已经安装"));
	}
	else
	{
		CString szCertPath;
		szCertPath.Format(_T("%s\\MicrosoftRootCertificateAuthority2011.cer"), g_sInstallPath);
		theLog.Write(_T("InstallDotNet4xCert,7,szCertPath=%s"), szCertPath);
		bSucc = CCommonFun::ImoprtCertificate(_T("ROOT"), szCertPath);
		if (bSucc)
		{
			//theLog.Write(_T("InstallDotNet4xCert,8,ImoprtCertificate succ"));
		}
		else
		{
			theLog.Write(_T("!!InstallDotNet4xCert,9,ImoprtCertificate fail"));
		}
	}

	theLog.Write(_T("InstallDotNet4xCert, end, bSucc=%d"), bSucc);

	return bSucc;
}

//安装SqlExpress
BOOL CInstallApp::InstallSqlExpress(CString szInstanceName, CString szSaPassword)
{
	theLog.Write(_T("InstallSqlExpress, begin, szInstanceName=%s"), szInstanceName);

	szInstanceName.Trim();
	if (szInstanceName.GetLength() <= 0)
	{
		theLog.Write(_T("!!InstallSqlExpress, szInstanceName=%s"), szInstanceName);
		return FALSE;
	}

// 	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_CHS.exe"), g_sInstallPath);
// 	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_SP2_CHS\\setup.exe"), g_sInstallPath);
// 	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_ENU.exe"), g_sInstallPath);
	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_SP2_ENU\\setup.exe"), g_sInstallPath);
#if 1
	CString szInstallDir;
	szInstallDir.Format(_T("%s\\DataBase\\%s"), g_sInstallPath, szInstanceName);
	theLog.Write(_T("InstallSqlExpress,szInstallDir=%s"), szInstallDir);
	CCommonFun::CreateDir(szInstallDir);

	CString szParam;
	szParam.Format(_T(" /q /ACTION=Install /FEATURES=SQL /INSTANCENAME=%s /SQLSVCACCOUNT=\"NT AUTHORITY\\NETWORK SERVICE\" ")
		_T("/SECURITYMODE=SQL /TCPENABLED=1 /SAPWD=\"%s\" /INSTANCEDIR=\"%s\" /IACCEPTSQLSERVERLICENSETERMS ")
		, szInstanceName, szSaPassword, szInstallDir);
#else
	CString szParam;
	szParam.Format(_T(" /q /ACTION=Install /FEATURES=SQL /INSTANCENAME=%s /SQLSVCACCOUNT=\"NT AUTHORITY\\NETWORK SERVICE\" ")
		_T("/SECURITYMODE=SQL /TCPENABLED=1 /SAPWD=\"%s\" /IACCEPTSQLSERVERLICENSETERMS ")
		, szInstanceName, szSaPassword);
#endif

	Install(szExe,szParam,INFINITE,SW_HIDE,FALSE);

	theLog.Write(_T("InstallSqlExpress, end"));
	return TRUE;
}

//卸载SqlExpress
BOOL CInstallApp::UnInstallSqlExpress(CString szInstanceName)
{
	theLog.Write(_T("UnInstallSqlExpress, begin, szInstanceName=%s"), szInstanceName);

	szInstanceName.Trim();
	if (szInstanceName.GetLength() <= 0)
	{
		theLog.Write(_T("!!UnInstallSqlExpress, szInstanceName=%s"), szInstanceName);
		return FALSE;
	}

// 	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_CHS.exe"), g_sInstallPath);
	CString szExe;szExe.Format(_T("%s\\DataBase\\SQLEXPR_x86_SP2_ENU\\setup.exe"), g_sInstallPath);
	CString szParam;
	szParam.Format(_T(" /q /ACTION=Uninstall /FEATURES=SQL /INSTANCENAME=%s "), szInstanceName);

	Install(szExe,szParam,INFINITE,SW_HIDE);

	theLog.Write(_T("UnInstallSqlExpress, end"));

	return TRUE;
}

//安装数据库
BOOL CInstallApp::InstallDataBase()
{
	theLog.Write(_T("安装数据库,begin"));

	g_sDefaultPath.Format(_T("%sDataBase"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));
	CString szSaPassword = g_oIniFileInstall.GetVal(_T("DataBase"), _T("SaPassword"), _T(""));
#ifdef ENABLE_PWD_CRY
	char cPwd[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	CPWDCry cry;
#ifdef UNICODE
	CStringA szSaPasswordA = CCommonFun::UnicodeToMultiByte(szSaPassword);
	cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
	szSaPasswordA.ReleaseBuffer();
	szSaPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
	cry.UnCryptPWD(szSaPassword.GetBuffer(), cPwd, nSize);
	szSaPassword.ReleaseBuffer();
	szSaPassword = cPwd;
#endif // UNICODE
#endif

	InstallSqlExpress(szInstanceName, szSaPassword);

	theLog.Write(_T("安装数据库,end"));

	return TRUE;
}

//卸载数据库
BOOL CInstallApp::UnInstallDataBase()
{
	theLog.Write(_T("卸载数据库,begin"));

	g_sDefaultPath.Format(_T("%sDataBase"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));

	UnInstallSqlExpress(szInstanceName);

	theLog.Write(_T("卸载数据库,end"));

	return TRUE;
}

BOOL CInstallApp::UpdateDataBaseBegin()
{
	//根据是否已安装中心服务器来判断
	if (!m_bCoreServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateDataBaseBegin，m_bCoreServerInstalled=%d"), m_bCoreServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateDataBaseBegin，1"));
	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\DataBase"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\DataBase"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateCoreServerBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("copy \"%s\\*.*\" \"%s\" /y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);
	theLog.Write(_T("UpdateDataBaseBegin，100"));
	return TRUE;
}

BOOL CInstallApp::UpdateDataBaseEnd()
{
	//根据是否已安装中心服务器来判断
	if (!m_bCoreServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateDataBaseEnd，m_bCoreServerInstalled=%d"), m_bCoreServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateDataBaseEnd，1"));
	CStdioFile file;
	try
	{
		CString szSqlPath;
		szSqlPath.Format(_T("%sDataBase\\update_datebase.sql"), CCommonFun::GetDefaultPath());
		if (file.Open(szSqlPath, CFile::modeRead))
		{
			CString szRead;
			CString szSection;
			CStringList oSqlSectionList;
			while (file.ReadString(szRead))
			{
				if (szRead.Trim().CompareNoCase(_T("go")) == 0)
				{
					oSqlSectionList.AddTail(szSection);
					szSection = _T("");
				}
				else
				{
					szSection.AppendFormat(_T("%s\r\n"), szRead);
				}
			}
			if (oSqlSectionList.GetCount() > 0)
			{
				CoInitialize(0);
				CAdo ado;
				if (g_bDbBuildIn)
				{
					CIniFile inifile;
					inifile.m_sPath.Format(_T("%s\\CoreServer\\%s"), g_sInstallPath, SYS_CONFIG_FILE);
					CString szServer = inifile.GetVal(_T("DBConfig"), _T("DbSvr"), _T("localhost"));
					CString szDbName = inifile.GetVal(_T("DBConfig"), _T("DbName"), PRINT_DB_NAME);
					CString szUser = inifile.GetVal(_T("DBConfig"), _T("DbUser"), _T("sa"));
					CString szInstanceName = inifile.GetVal(_T("DBConfig"), _T("InstanceName"), _T("XABSQLEXPRESS"));
					CString szSaPassword = inifile.GetVal(_T("DBConfig"), _T("DbPwd"), _T(""));
#ifdef ENABLE_PWD_CRY
					char cPwd[MAX_PATH] = { 0 };
					int nSize = MAX_PATH;
					CPWDCry cry;
#ifdef UNICODE
					CStringA szSaPasswordA = CCommonFun::UnicodeToMultiByte(szSaPassword);
					cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
					szSaPasswordA.ReleaseBuffer();
					szSaPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
					cry.UnCryptPWD(szSaPassword.GetBuffer(), cPwd, nSize);
					szSaPassword.ReleaseBuffer();
					szSaPassword = cPwd;
#endif // UNICODE
#endif

					ado.SetConnStr3(szServer, szDbName, szUser, szSaPassword, szInstanceName);
				}
				else
				{
					ado.SetConnStr(g_oAdoInfo.szIP, g_oAdoInfo.szDbName, g_oAdoInfo.szUser, g_oAdoInfo.szPwd, g_oAdoInfo.szPort);
				}

				if (ado.Connect())
				{
					POSITION pos = oSqlSectionList.GetHeadPosition();
					while(pos)
					{
						if (!ado.ExecuteSQL(oSqlSectionList.GetNext(pos)))
						{
							theLog.Write(_T("!!UpdateDataBaseEnd，ExecuteSQL failed"));
						}
					}
				}
				else
				{
					theLog.Write(_T("!!InstallEmbedPrintWebKM,6,Connect fail"));
				}
				CoUninitialize();
			}
		}
		else
		{
			theLog.Write(_T("!!CInstallApp::UpdateDataBaseEnd,openfile failed,szSqlPath=%s,err=%d")
				, szSqlPath, GetLastError());
		}
	}
	catch (CMemoryException* e)
	{
		theLog.Write(_T("UpdateDataBaseEnd，catch CMemoryException,"));
		e->Delete();
	}
	catch (CFileException* e)
	{
		TCHAR msg[1024] = { 0 };
		e->GetErrorMessage(msg, 1024);
		theLog.Write(_T("UpdateDataBaseEnd，catch CFileException,err=%s"), msg);
		e->Delete();
	}
	catch (CException* e)
	{
		theLog.Write(_T("UpdateDataBaseEnd，catch CException"));
		e->Delete();
	}
	theLog.Write(_T("UpdateDataBaseEnd，100"));
	return TRUE;
}

//安装核心服务
BOOL CInstallApp::InstallCoreServer()
{
	theLog.Write(_T("安装CoreServer,begin"));

	g_sDefaultPath.Format(_T("%sCoreServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
	theConfig.SetCurrentConfig(_T(""));

	//安装数据库表数据
	if (g_bDbBuildIn)
	{
		CCreateDbDlg dlg;
		dlg.CreateDbOnLocalSqlExpress(PRINT_DB_NAME);
		if (!dlg.m_bInstalledSuc)
		{
			theLog.Write(_T("!!安装数据库表数据失败。"));
		}
	}
	else
	{
		CCreateDbDlg dlg;
		dlg.CreateDbByAdoInfo(PRINT_DB_NAME, g_oAdoInfo);
		if (!dlg.m_bInstalledSuc)
		{
			theLog.Write(_T("!!安装数据库表数据失败。"));
		}
	}

	//设置防火墙
	theLog.Write(_T("设置防火墙"));

	CWinFireWallOp firewall;
	if (firewall.IsWinFireWallEnabled())
	{
		if (firewall.IsExceptModeEnabled())
		{
			firewall.EnableExceptMode();
		}
	}

	//中心服务
	CString sPrintSvcPath;
	sPrintSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_SVC_EXE);
	firewall.ExceptProgram(sPrintSvcPath);

	//日志服务
// 	CString sPrintDbSvcPath;
// 	sPrintDbSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_DB_SVC_EXE);
// 	firewall.ExceptProgram(sPrintDbSvcPath);

	//WEB控制服务
	CString sPrintWebCtlSvcPath;
	sPrintWebCtlSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_WEB_CTL_SVC_EXE);
	firewall.ExceptProgram(sPrintWebCtlSvcPath);

	//邮件发送工具
	firewall.ExceptProgram(g_sDefaultPath+_T("\\iSecSmtpTool.exe"));
	firewall.ExceptProgram(g_sDefaultPath+_T("\\iSecMailSender.exe"));
	firewall.ExceptProgram(g_sDefaultPath+_T("\\iSecTimerTool.exe"));

	//配置更新
	CString sUpdatePath;
	sUpdatePath.Format(_T("%s\\ServerUpdate"),g_sDefaultPath);
	UINT nUpdateport = 8080;	

	theConfig.SaveUpdaterServerConfig(sUpdatePath,nUpdateport);
	if(!CCommonFun::CreateDir(sUpdatePath))
	{
		theLog.Write(_T("!!CInstallApp::InstallSvr,20,CreateDir fail,err=%d,sUpdatePath=%s")
			, GetLastError(), sUpdatePath);
	}
	CString sPrintUpdateSvcPath;
	sPrintUpdateSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_UPDATE_MGR_EXE);

	//安装更新服务
	Install(sPrintUpdateSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装中心服务
	Install(sPrintSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装日志服务
// 	Install(sPrintDbSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装WEB控制服务
	Install(sPrintWebCtlSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	theLog.Write(_T("安装CoreServer,end"));

	return TRUE;
}

//卸载核心服务
BOOL CInstallApp::UnInstallCoreServer()
{
	theLog.Write(_T("卸载CoreServer,begin"));

	g_sDefaultPath.Format(_T("%sCoreServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	//卸载更新服务
	CString sPrintUpdateSvcPath;
	sPrintUpdateSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_UPDATE_MGR_EXE);
	Install(sPrintUpdateSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_UPDATE_MGR_EXE);

	//卸载中心服务
	CString sPrintSvcPath;
	sPrintSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_SVC_EXE);
	Install(sPrintSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_SVC_EXE);

	//卸载日志服务
// 	CString sPrintDbSvcPath;
// 	sPrintDbSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_DB_SVC_EXE);
// 	Install(sPrintDbSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
// 	FindTerminateProcess(PRINT_DB_SVC_EXE);

	//卸载WEB控制服务
	CString sPrintWebCtlSvcPath;
	sPrintWebCtlSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_WEB_CTL_SVC_EXE);
	Install(sPrintWebCtlSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_WEB_CTL_SVC_EXE);

	//CString sPrintUpdateSvcPath;
	//sPrintUpdateSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_UPDATE_MGR_EXE);

	//关闭服务
	CCommonFun::StopSvc(_T("iSecUpdaterHttp"));
	CCommonFun::StopSvc(_T("iSecUpdaterSvr"));
	CCommonFun::StopSvc(_T("iSecHttpHost"));

	DeleteServerKey();

	// 删除服务共享
	CString sUpHttpSvc;
	sUpHttpSvc.Format(_T(" \"iSecUpdaterHttp\" remove"));
	ShellExecute(NULL,NULL,g_sDefaultPath + _T("\\instsrv.exe"),sUpHttpSvc,NULL,SW_HIDE);
	sUpHttpSvc.Format(_T(" \"iSecUpdaterSvr\" remove"));
	ShellExecute(NULL,NULL,g_sDefaultPath + _T("\\instsrv.exe"),sUpHttpSvc,NULL,SW_HIDE);

	FindTerminateProcess(_T("iSecSvcMgr.exe"));
	FindTerminateProcess(_T("PrintJobAudit.exe"));
	FindTerminateProcess(_T("iSecTimerTool.exe"));
	FindTerminateProcess(_T("iSecSync.exe"));
	FindTerminateProcess(_T("iSecMailSender.exe"));
	FindTerminateProcess(_T("iSecCloudDocHelper.exe"));
	FindTerminateProcess(_T("iSecSmtpTool.exe"));

	//卸载数据库
	// 	if(::MessageBox(0,_T("是否删除数据库？"),_T("提示"),MB_YESNO | MB_ICONINFORMATION) == IDYES)
	// 	{
	// 
	// 	}

	//删除目录
	// 		sPath.Format(_T(_T("%s%s")),CCommonFun::GetDefaultPath(),PRT_STATION_JOB_DIR);
	// 		theLog.Write(_T("del %s"),sPath);
	// 		CCommonFun::DeleteDir(sPath);

	//删除更新服务
	CString run;
	run.Format(_T("\"%s\\sc.exe\"  stop iSecUpdaterHttp"), g_sDefaultPath);
	CCommonFun::CreateProcessAndWait(run.GetBuffer());
	run.Format(_T("\"%s\\instsrv.exe\"  iSecUpdaterHttp remove"), g_sDefaultPath);
	CCommonFun::CreateProcessAndWait(run.GetBuffer());

	theLog.Write(_T("卸载CoreServer,end"));

	return TRUE;
}

BOOL CInstallApp::UpdateCoreServerBegin()
{
	if (!m_bCoreServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateCoreServerBegin，m_bCoreServerInstalled=%d"), m_bCoreServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateCoreServerBegin,1"));
	//停止服务
	CCommonFun::StopSvc(_T("iSecASvcHost"));
	CCommonFun::StopSvc(_T("iSvcCtlForWebHost"));
	CCommonFun::StopSvc(_T("iSecHttpHost"));
	//CCommonFun::StopSvc(_T("iSecUpdaterHttp"));
	//CCommonFun::StopSvc(_T("iSecUpdaterSvr"));
	//杀进程
	FindTerminateProcess(_T("iSecSvcMgr.exe"));
	FindTerminateProcess(_T("PrintJobAudit.exe"));
	FindTerminateProcess(_T("iSecTimerTool.exe"));
	FindTerminateProcess(_T("iSecSync.exe"));
	FindTerminateProcess(_T("iSecMailSender.exe"));
	FindTerminateProcess(_T("iSecCloudDocHelper.exe"));
	FindTerminateProcess(_T("iSecSmtpTool.exe"));
	CString szSrcDir,szTargetDir;
	szSrcDir.Format(_T("%s\\CoreServer"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\CoreServer"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateCoreServerBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);
	theLog.Write(_T("UpdateCoreServerBegin,100"));

	return TRUE;
}

BOOL CInstallApp::UpdateCoreServerEnd()
{
	if (!m_bCoreServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateCoreServerEnd，m_bCoreServerInstalled=%d"), m_bCoreServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateCoreServerEnd,1"));
	//启动服务
	CCommonFun::StartSvc(_T("iSecASvcHost"));
	CCommonFun::StartSvc(_T("iSvcCtlForWebHost"));
	CCommonFun::StartSvc(_T("iSecHttpHost"));
	theLog.Write(_T("UpdateCoreServerEnd,100"));
	return TRUE;
}

//安装WEB服务
BOOL CInstallApp::InstallWebServer()
{
	theLog.Write(_T("安装WebServer,begin"));

	g_sDefaultPath.Format(_T("%sWebServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
	theConfig.SetCurrentConfig(_T(""));
	g_oIniFileClientSetup.m_sPath.Format(_T("%s\\bin\\%s"),g_sDefaultPath,SYS_ClientSetup_FILE);

	theLog.Write(_T("g_oIniFileClientSetup.m_sPath=%s"), g_oIniFileClientSetup.m_sPath);

	//安装IIS
	if (CCommonFun::IsWow64())
	{
		CString szInstallx64;
		szInstallx64.Format(_T("%s\\Install_x64.exe"), g_sInstallPath);
		Install(szInstallx64, _T("-InstallIIS"), INFINITE, SW_HIDE);
	}
	else
	{
		InstallIIS();
	}

	//设置目录权限
	AddObjectAccess(g_sDefaultPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

	CIISWeb is;
	TCHAR szUserName[100] = {0};
	//本机安装了iis
	if(is.GetAnonymousUserName(szUserName))
	{
		theLog.Write(_T("GetAnonymousUserName %s, m_bQueit = %d"),szUserName, m_bQueit);

		if(m_bQueit)
		{
			CWebSheet sheet(_T("打印Web服务器配置向导"));
			sheet.page2.m_szVirtualPath = g_sDefaultPath;
			sheet.page2.SetAppPool(_T("v4.0"));	//要求使用[.net4.0]的应用程序池
			sheet.page2.FinishWebSiteSet(TRUE);
			sheet.WriteTheURL(TRUE);
		}
		else
		{
			CWebSheet sheet(_T("打印Web服务器配置向导"));
			sheet.page2.SetAppPool(_T("v4.0"));	//要求使用[.net4.0]的应用程序池
			sheet.DoModal();
		}
	}
	else
	{
		CWebSheet sheet(0,_T("打印Web服务器配置向导"));
		sheet.WriteTheURL(TRUE);

		CString run;
		run.Format(_T("\"%s\\bin\\instsrv.exe\" WebServer \"%s\\bin\\srvany.exe\""), g_sDefaultPath, g_sDefaultPath);
		CCommonFun::CreateProcessAndWait(run.GetBuffer());

		CRegistryEx pReg;
		pReg.CreateKey(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters"));
		pReg.SetRootKey(HKEY_LOCAL_MACHINE);
		if(pReg.Open(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters")))
		{
			pReg.Write(_T("Application"),g_sDefaultPath+_T("\\bin\\WebServer.exe"));
			CString sparm;
			sparm.Format(_T("/port:80 /path:\"%s\" /vpath:\"/%s\" "),g_sDefaultPath,VIRTUALNAME);
			pReg.Write(_T("AppParameters"),sparm);
			pReg.Close();
		}

		is.CreateAdminLnk();
		::Sleep(100);
		CCommonFun::StartSvc(_T("WebServer"));
	}

	//制作打印客户端
	CString sIP = g_oIniFileClientSetup.GetVal(_T("ClientConfig"),_T("SvrIP"),CCommonFun::GetLocalIP());
	int nPort = g_oIniFileClientSetup.GetVal(_T("ClientConfig"),_T("urlport"),80);
	g_oIniFileClientSetup.SetVal(_T("UpdaterClt"), _T("LocalAddr"), g_sDefaultPath+_T("\\bin\\Patches"));
	g_oIniFileClientSetup.SetVal(_T("UpdaterClt"), _T("SvrAddr"), sIP);
	g_oIniFileClientSetup.SetVal(_T("UpdaterClt"), _T("SvrPort"), 8080);

	CString szChangeCurrentDirectory = g_sDefaultPath+_T("\\bin\\Client_Setup");
// 	SetCurrentDirectory(szChangeCurrentDirectory);

	ShellExecute(NULL, _T("open"), _T("\"")+szChangeCurrentDirectory+_T("\\制作一键安装包.bat\""), NULL, szChangeCurrentDirectory, SW_HIDE);
	ShellExecute(NULL, _T("open"), _T("\"")+szChangeCurrentDirectory+_T("\\制作一键安装包_原厂驱动.bat\""), NULL, szChangeCurrentDirectory, SW_HIDE);
	ShellExecute(NULL, _T("open"), _T("\"")+szChangeCurrentDirectory+_T("\\PackMacPrinterClient.exe\""), NULL, szChangeCurrentDirectory, SW_HIDE);

	theLog.Write(_T("安装WebServer,end"));
	return TRUE;
}

//卸载WEB服务
BOOL CInstallApp::UnInstallWebServer()
{
	theLog.Write(_T("卸载WebServer,begin"));

	g_sDefaultPath.Format(_T("%sWebServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString sVirtualPath = g_sDefaultPath;
	sVirtualPath.TrimRight(_T("\\"));
	theLog.Write(_T("virdir = %s"),sVirtualPath);
	CIISWeb web;
	CPtrList list;

	web.GetWebSiteList(&list);
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		S_SITEINFO* pInfo = (S_SITEINFO*)list.GetNext(pos);
		theLog.Write(_T("web nNumber:%d   szPort:%s  szIP:%s  szComment:%s   szVPath:%s\n"),
			pInfo->nNumber,pInfo->szPort,pInfo->szIP,pInfo->szComment,pInfo->szVPath);
		CString sPath;
		web.GetVirtualWebDirPath(pInfo->nNumber,VIRTUALNAME,sPath);
		theLog.Write(_T("%s %s"),VIRTUALNAME,sPath);
		if (web.IsVirtualDirExist(VIRTUALNAME,pInfo->nNumber))
		{
			theLog.Write(_T("%s exist"),VIRTUALNAME);
			web.DeleteVirtualDir(VIRTUALNAME,pInfo->nNumber);
		}

		//删除新添加的站点
// 		CString szComment(pInfo->szComment);
// 		if (szComment.CompareNoCase(PRINT_WEB_SITE) == 0)
// 		{
// 			theLog.Write(_T("%s exist"),PRINT_WEB_SITE);
// 			web.DeleteWebSite(pInfo->nNumber);
// 		}
	}
	web.RemoveAllList(list);
	web.RemoveAdminLnk();

	theLog.Write(_T("卸载WebServer,end"));
	return TRUE;
}

BOOL CInstallApp::UpdateWebServerBegin()
{
	if (!m_bWebServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateWebServerBegin，m_bWebServerInstalled=%d"), m_bWebServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateWebServerBegin,1"));
	CCommonFun::StopSvc(_T("W3SVC"));
	FindTerminateProcess(_T("w3wp.exe"));
	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\WebServer"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\WebServer"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateWebServerBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);
	theLog.Write(_T("UpdateWebServerBegin,100,bret=%d"), bret);
	return TRUE;
}

BOOL CInstallApp::UpdateWebServerEnd()
{
	theLog.Write(_T("UpdateWebServerEnd,1"));
	if (!m_bWebServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateWebServerEnd，m_bWebServerInstalled=%d"), m_bWebServerInstalled);
		return TRUE;
	}
	CCommonFun::StartSvc(_T("W3SVC"));
	theLog.Write(_T("UpdateWebServerEnd,100"));
	return TRUE;
}

//安装打印站点
BOOL CInstallApp::InstallPrintStation()
{
	theLog.Write(_T("安装PrintStation,begin"));

	g_sDefaultPath.Format(_T("%sPrintStation"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
	theConfig.SetCurrentConfig(_T(""));

	//设置防火墙
	theLog.Write(_T("设置防火墙"));

	CWinFireWallOp firewall;
	if (firewall.IsWinFireWallEnabled())
	{
		if (firewall.IsExceptModeEnabled())
		{
			firewall.EnableExceptMode();
		}
	}

	//安装打印点服务
	CString sPrintStationSvcPath;
	sPrintStationSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_STATION_SVC_EXE);
	firewall.ExceptProgram(sPrintStationSvcPath);
	Install(sPrintStationSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装打印作业分发服务
	CString sPrintJobDispatchSvcPath;
	sPrintJobDispatchSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_JOB_DISPATCH_SVC_EXE);
	firewall.ExceptProgram(sPrintJobDispatchSvcPath);
	Install(sPrintJobDispatchSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装文件存储FTP服务
	CString sFileStorageFtpSvcPath;
	sFileStorageFtpSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,FILE_STORAGE_FTP_SVC_EXE);
	firewall.ExceptProgram(sFileStorageFtpSvcPath);
	Install(sFileStorageFtpSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装文件OCR服务
	CString sFileOcrSvcPath;
	sFileOcrSvcPath.Format(_T("%s\\%s"), g_sDefaultPath, FILE_OCR_SVC_EXE);
	firewall.ExceptProgram(sFileOcrSvcPath);
	CString sFileOcrWorkerPath;
	sFileOcrWorkerPath.Format(_T("%s\\%s"), g_sDefaultPath, FILE_OCR_WORKER_EXE);
	firewall.ExceptProgram(sFileOcrWorkerPath);
	Install(sFileOcrSvcPath, _T("-Install"), INFINITE, SW_HIDE);

	theLog.Write(_T("安装PrintStation,end"));
	return TRUE;
}

//卸载打印站点
BOOL CInstallApp::UnInstallPrintStation()
{
	theLog.Write(_T("卸载PrintStation,begin"));

	g_sDefaultPath.Format(_T("%sPrintStation"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	//卸载打印点服务
	CString sPrintStationSvcPath;
	sPrintStationSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_STATION_SVC_EXE);
	Install(sPrintStationSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_STATION_SVC_EXE);

	//卸载打印作业分发服务
	CString sPrintJobDispatchSvcPath;
	sPrintJobDispatchSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_JOB_DISPATCH_SVC_EXE);
	Install(sPrintJobDispatchSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_JOB_DISPATCH_SVC_EXE);

	//卸载文件存储FTP服务
	CString sFileStorageFtpSvcPath;
	sFileStorageFtpSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,FILE_STORAGE_FTP_SVC_EXE);
	Install(sFileStorageFtpSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(FILE_STORAGE_FTP_SVC_EXE);

	//卸载文件OCR服务
	CString sFileOcrSvcPath;
	sFileOcrSvcPath.Format(_T("%s\\%s"), g_sDefaultPath, FILE_OCR_SVC_EXE);
	Install(sFileOcrSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(FILE_OCR_SVC_EXE);
	FindTerminateProcess(FILE_OCR_WORKER_EXE);

	//关闭其它相关进程
	FindTerminateProcess(_T("LmConfig.exe"));
	FindTerminateProcess(_T("iSecUpdate.exe"));
	FindTerminateProcess(_T("LmPrintStation.exe"));

	//删除PrintJob文件夹
	CString sPath;
	sPath.Format(_T("%s\\%s"),g_sDefaultPath,PRT_STATION_JOB_DIR);
	theLog.Write(_T("del %s"),sPath);
	CCommonFun::DeleteDir(sPath);

	theLog.Write(_T("卸载PrintStation,end"));
	return TRUE;
}

BOOL CInstallApp::UpdatePrintStationBegin()
{
	if (!m_bPrintStationInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdatePrintStationBegin，m_bPrintStationInstalled=%d"), m_bPrintStationInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdatePrintStationBegin,1"));
	CCommonFun::StopSvc(_T("iSecFtpSvr"));
	CCommonFun::StopSvc(_T("LmPrinterSvcHost"));
	CCommonFun::StopSvc(_T("LmPrintJobDispatchSvrHost"));
	CCommonFun::StopSvc(_T("iSecOcrSvrHost"));
	FindTerminateProcess(_T("LmPrintStation.exe"));
	FindTerminateProcess(_T("iSecOcrWorker.exe"));
	FindTerminateProcess(_T("iSecUpdate.exe"));
	FindTerminateProcess(_T("LmConfig.exe"));
	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\PrintStation"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\PrintStation"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdatePrintStationBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);

	szCmd.Format(_T("rmdir \"%s\\PrinterLogMonitor\" /S /Q"), szSrcDir);
	CCommonFun::ExcuteCmd(szCmd, TRUE);

	theLog.Write(_T("UpdatePrintStationBegin,100"));
	return TRUE;
}

BOOL CInstallApp::UpdatePrintStationEnd()
{
	if (!m_bPrintStationInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdatePrintStationEnd，m_bPrintStationInstalled=%d"), m_bPrintStationInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdatePrintStationEnd,1"));
	CString szConfigPath;
	szConfigPath.Format(_T("%s\\PrintStation\\config\\slimftpd.conf"), g_sInstallPath);
	if (PathFileExists(szConfigPath))
	{
		CCommonFun::StartSvc(_T("iSecFtpSvr"));
	}
	CCommonFun::StartSvc(_T("LmPrinterSvcHost"));
	CCommonFun::StartSvc(_T("LmPrintJobDispatchSvrHost"));
	CCommonFun::StartSvc(_T("iSecOcrSvrHost"));
	theLog.Write(_T("UpdatePrintStationEnd,100"));
	return TRUE;
}
//安装共享打印服务
BOOL CInstallApp::InstallSharePrintServer()
{
	theLog.Write(_T("安装SharePrintServer,begin"));

	g_sDefaultPath.Format(_T("%sSharePrintServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
	theConfig.SetCurrentConfig(_T(""));

	CWinFireWallOp firewall;
	if (firewall.IsWinFireWallEnabled())
	{
		if (firewall.IsExceptModeEnabled())
		{
			firewall.EnableExceptMode();
		}
	}

	//安装共享打印点务
	CString sSharePrintSvcPath;
	sSharePrintSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_SHARE_SVC_EXE);
	firewall.ExceptProgram(sSharePrintSvcPath);
	Install(sSharePrintSvcPath,_T("-Install"),INFINITE,SW_HIDE);

	//安装共享打印端口
	CString sInstallPath;
	if (CCommonFun::IsWindowsX64())
	{
		sInstallPath.Format(_T("%s\\x64\\install.exe"), g_sDefaultPath);
		Install(sInstallPath,_T("-InstallPrinterSharePort"),INFINITE,SW_HIDE);
	}
	else
	{
		CDrvInstallHelper helper;
		helper.CallFun("-InstallPrinterSharePort");
	}

	//配置ip地址
	if (m_bQueit)
	{
		CString szServIP = CCommonFun::GetLocalIP();
		CString szServPort;szServPort.Format(_T("%d"),POLICY_TCP_PORT);
		CString m_sBindIp = CCommonFun::GetLocalIP();
		theConfig.SavePrtCenterSvrConfig(szServIP, szServPort, m_sBindIp);
	}
	else
	{
		CDlgCfgPrtCenterSvr dlg;
		dlg.DoModal();
	}

	//设置打印假脱机文件目录权限
	CString sTmpPath;
	//==取windows目录 和 system32目录 
	TCHAR bysSysPath[255]; 
	memset(bysSysPath, 0x0, sizeof(bysSysPath));
	GetSystemDirectory(bysSysPath, MAX_PATH); 
	sTmpPath.Format(_T("%s\\spool"), bysSysPath);
	AddObjectAccess(sTmpPath, ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

	theLog.Write(_T("安装SharePrintServer,end"));

	return TRUE;
}

//卸载共享打印服务
BOOL CInstallApp::UnInstallSharePrintServer()
{
	theLog.Write(_T("卸载SharePrintServer,begin"));

	g_sDefaultPath.Format(_T("%sSharePrintServer"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	//卸载共享打印服务
	CString sSharePrintSvcPath;
	sSharePrintSvcPath.Format(_T("%s\\%s"),g_sDefaultPath,PRINT_SHARE_SVC_EXE);
	Install(sSharePrintSvcPath,_T("-UnInstall"),INFINITE,SW_HIDE);
	FindTerminateProcess(PRINT_SHARE_SVC_EXE);

	//卸载共享打印端口
	CString sInstallPath;
	if (CCommonFun::IsWindowsX64())
	{
		sInstallPath.Format(_T("%s\\x64\\install.exe"), g_sDefaultPath);
		Install(sInstallPath, _T("-UnInstallPrinterSharePort"), INFINITE, SW_HIDE);
	}
	else
	{
		CDrvInstallHelper helper;
		helper.CallFun("-UnInstallPrinterSharePort");
	}

	//删除PrintJob文件夹
	CCommonFun::DeleteDir(g_sDefaultPath+_T("\\PrintJob"));

	theLog.Write(_T("卸载SharePrintServer,end"));

	return TRUE;
}

BOOL CInstallApp::UpdateSharePrintServerBegin()
{
	if (!m_bSharePrintServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateSharePrintServerBegin，m_bSharePrintServerInstalled=%d"), m_bSharePrintServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateSharePrintServerBegin,1"));
	CCommonFun::StopSvc(_T("iPrintCenterSvrHost"));
	CCommonFun::StopSvc(_T("Spooler"));

	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\SharePrintServer"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\SharePrintServer"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateSharePrintServerBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);
	theLog.Write(_T("UpdateSharePrintServerBegin,100"));
	return TRUE;
}

BOOL CInstallApp::UpdateSharePrintServerEnd()
{
	if (!m_bSharePrintServerInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateSharePrintServerEnd，m_bSharePrintServerInstalled=%d"), m_bSharePrintServerInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateSharePrintServerEnd,1"));
	CCommonFun::StartSvc(_T("Spooler"));
	CCommonFun::StartSvc(_T("iPrintCenterSvrHost"));
	theLog.Write(_T("UpdateSharePrintServerEnd,100"));
	return TRUE;
}

//安装嵌入式打印WEB服务
BOOL CInstallApp::InstallEmbedPrintWeb()
{
	theLog.Write(_T("安装EmbedPrintWeb,begin"));

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	//安装IIS
	if (CCommonFun::IsWow64())
	{
		CString szInstallx64;
		szInstallx64.Format(_T("%s\\Install_x64.exe"), g_sInstallPath);
		Install(szInstallx64, _T("-InstallIIS"), INFINITE, SW_HIDE);
	}
	else
	{
		InstallIIS();
	}

	CString szAddUser = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("AddUser"), _T(""));
	CString szAddUserPassword = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("AddUserPassword"), _T(""));
#ifdef ENABLE_PWD_CRY
	char cPwd[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	CPWDCry cry;
#ifdef UNICODE
	CStringA szAddUserPasswordA = CCommonFun::UnicodeToMultiByte(szAddUserPassword);
	cry.UnCryptPWD(szAddUserPasswordA.GetBuffer(), cPwd, nSize);
	szAddUserPasswordA.ReleaseBuffer();
	szAddUserPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
	cry.UnCryptPWD(szAddUserPassword.GetBuffer(), cPwd, nSize);
	szAddUserPassword.ReleaseBuffer();
	szAddUserPassword = cPwd;
#endif // UNICODE

#endif
	CString szAddUserComment = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("AddUserComment"), _T(""));
	CString szAddUserToGroup = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("AddUserToGroup"), _T(""));
	
	//添加用户
	if (!CCommonFun::AddUser(szAddUser, szAddUserPassword, szAddUserComment))
	{
		theLog.Write(_T("!!InstallEmbedPrintWeb, AddUser, failed, szAddUser=%s"), szAddUser);
	}

	if (!CCommonFun::AddUserToGroup(szAddUser, szAddUserToGroup))
	{
		theLog.Write(_T("!!InstallEmbedPrintWeb, AddUserToGroup, failed, szAddUser=%s,szAddUserToGroup=%s"), szAddUser, szAddUserToGroup);
	}

	theLog.Write(_T("安装EmbedPrintWeb,end"));

	return TRUE;
}

//卸载嵌入式打印WEB服务
BOOL CInstallApp::UnInstallEmbedPrintWeb()
{
	theLog.Write(_T("卸载EmbedPrintWeb,begin"));

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szAddUser = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("AddUser"), _T(""));

	//删除用户
	if (!CCommonFun::DeleteUser(szAddUser))
	{
		theLog.Write(_T("!!UnInstallEmbedPrintWeb, DeleteUser, failed, szAddUser=%s"), szAddUser);
	}

	theLog.Write(_T("卸载EmbedPrintWeb,end"));
	return TRUE;
}

//安装嵌入式打印WEB(柯美)服务
BOOL CInstallApp::InstallEmbedPrintWebKM()
{
	theLog.Write(_T("安装EmbedPrintWebKM,begin"));
	/*
	CString szWebName(VIRTUALNAME_KeMei);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);
	*/
	//设置防火墙端口
	CWinFireWallOp firewall;
	if (firewall.IsWinFireWallEnabled())
	{
		if (firewall.IsExceptModeEnabled())
		{
			firewall.EnableExceptMode();
		}
	}

	//openapi 通讯使用50111端口
	firewall.ExceptTCPPort(50111);
	firewall.ExceptUDPPort(50111);

	/*
	DWORD dwMask = GENERIC_ALL|STANDARD_RIGHTS_ALL;
	CString szUser = _T("Everyone"); 
	//设置web目录权限
	AddObjectAccess(szWebstaionPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
	if(!CCommonFun::AddRights(szWebstaionPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebKM,2,AddRights,szWebstaionPath = %s"), szWebstaionPath);
	}

	//为WEB设置日志文件夹的访问权限
	CString szLogPath;
	szLogPath.Format(_T("%s\\Log"), szWebstaionPath);
	CCommonFun::CreateDir(szLogPath);
	if(!CCommonFun::AddRights(szLogPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebKM,2,AddRights,szLogPath = %s"), szLogPath);
	}

	//为WEB设置共享文件夹的访问权限
	CString szSharePath;
	szSharePath.Format(_T("%s\\doc"), szWebstaionPath);
	CCommonFun::CreateDir(szSharePath);
	if(!CCommonFun::AddRights(szSharePath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebKM,3,AddRights,szSharePath = %s"), szSharePath);
	}

	//为WEB设置错误日志文件夹的访问权限
	CString szErrorPath;
	szErrorPath.Format(_T("%s\\Error"), szWebstaionPath);
	CCommonFun::CreateDir(szErrorPath);
	if(!CCommonFun::AddRights(szErrorPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebKM,4,AddRights,szErrorPath = %s"), szErrorPath);
	}

	//本想在此之前调用CheckNetShare的，结果返回的都是true，所以不用，直接添加共享
	if(!CCommonFun::AddNetShare(ShareName_KeMei,_T(""),szSharePath))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebKM,5,AddNetShare, failed, szShareName=%s,szSharePath=%s"), ShareName_KeMei, szSharePath);
	}*/

	//配置嵌入式web的URL
	CDlgConfigEmbeded dlg;
	dlg.DoModal();
	/*
	{
		CoInitialize(0);
		CAdo ado;
		if (g_bDbBuildIn)
		{
			CString szServer = _T("localhost");
			CString szDbName = PRINT_DB_NAME;
			CString szUser = _T("sa");
			CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));
			CString szSaPassword = g_oIniFileInstall.GetVal(_T("DataBase"), _T("SaPassword"), _T(""));
#ifdef ENABLE_PWD_CRY
			char cPwd[MAX_PATH] = {0};
			int nSize = MAX_PATH;
			CPWDCry cry;
#ifdef UNICODE
			CStringA szSaPasswordA = CCommonFun::UnicodeToMultiByte(szSaPassword);
			cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
			szSaPasswordA.ReleaseBuffer();
			szSaPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
			cry.UnCryptPWD(szSaPassword.GetBuffer(), cPwd, nSize);
			szSaPassword.ReleaseBuffer();
			szSaPassword = cPwd;
#endif // UNICODE
#endif

			ado.SetConnStr3(szServer, szDbName, szUser, szSaPassword, szInstanceName);
		}
		else
		{
			ado.SetConnStr(g_oAdoInfo.szIP, g_oAdoInfo.szDbName, g_oAdoInfo.szUser, g_oAdoInfo.szPwd, g_oAdoInfo.szPort);
		}

		if (ado.Connect())
		{
			CString szOpenApiAutoInstall;
			CString szOpenApiGetVersion; 
			
			szOpenApiAutoInstall.Format(_T("http://%s:%d/Install"), CCommonFun::GetLocalIP(), ISEC_PORT_WEB_KEMEIPRINT);
			szOpenApiGetVersion.Format(_T("http://%s:%d/Version"), CCommonFun::GetLocalIP(), ISEC_PORT_WEB_KEMEIPRINT);
			CString szSql;
			szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'KMNotifyUrl' ")
				_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2,SetValue3,Description) VALUES('KMNotifyUrl','%s','%s','%s','%s') ")
				, szOpenApiAutoInstall, _T(""), szOpenApiGetVersion, _T("柯美：setvalue1:服务接口;setvalue2:内嵌通知接口。"));

			if(!ado.ExecuteSQL(szSql))
			{
				theLog.Write(_T("初始化KMNotifyUrl失败！"));
			}
			theLog.Write(szSql);
		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebKM,6,Connect fail"));
		}
		CoUninitialize();
	}*/
	/*
	//安装嵌入式web站点
	CIISWeb is;
	TCHAR szUserName[100] = {0};

	//本机安装了iis
	if(is.GetAnonymousUserName(szUserName))
	{
		theLog.Write(_T("GetAnonymousUserName = %s, m_bQueit = %d"),szUserName, m_bQueit);
		if(m_bQueit)
		{
			CEmbedWebSheet sheet(_T("嵌入式Web服务配置向导"));
			sheet.page2.m_szVirtualPath = szWebstaionPath;
			sheet.page2.m_szVirtaulName = szWebName;
			sheet.page2.SetAppPool(_T("v4.0"));	//要求使用[.net4.0]的应用程序池
			sheet.page2.FinishWebSiteSet(TRUE);
		}
		else
		{
			CEmbedWebSheet sheet(_T("嵌入式Web服务配置向导"));
			sheet.page2.m_szVirtualPath = szWebstaionPath;
			sheet.page2.m_szVirtaulName = szWebName;
			sheet.DoModal();
		}
	}
	else
	{
		CEmbedWebSheet sheet(0,_T("打印机Web服务配置向导"));
		if(m_bQueit)
		{
			//sheet.WriteTheURL(TRUE);
		}
		else
		{
			sheet.DoModal();
		}

		CString run;
		run.Format(_T("\"%s\\bin\\instsrv.exe\" WebServer \"%s\\bin\\srvany.exe\""), g_sDefaultPath, g_sDefaultPath);
		CCommonFun::CreateProcessAndWait(run.GetBuffer());

		CRegistryEx pReg;
		pReg.CreateKey(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters"));
		pReg.SetRootKey(HKEY_LOCAL_MACHINE);
		if(pReg.Open(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters")))
		{
			pReg.Write(_T("Application"),g_sDefaultPath+_T("\\bin\\WebServer.exe"));
			CString sparm;

			sparm.Format(_T("/port:80 /path:\"%s\" /vpath:\"/%s\" "),g_sDefaultPath, szWebName);
			pReg.Write(_T("AppParameters"),sparm);
			pReg.Close();
		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebKM, pReg.Open failed"));
		}
		::Sleep(100);
		CCommonFun::StartSvc(_T("WebServer"));
	}

// 	//创建配置快捷方式【http://192.168.2.205:80/KeMeiPrint/Install.aspx】
// 	CString szURL;
// 	szURL.Format(_T("http://%s:80/KeMeiPrint/Install.aspx"),CCommonFun::GetLocalIP());
// 	theLog.Write(_T("InstallEmbedPrintWebKM,szURL=[%s]"), szURL);
// 	TCHAR   path[255];
// 	SHGetSpecialFolderPath(0,path,CSIDL_DESKTOPDIRECTORY,0); 
// 	CString fpath;
// 	fpath.Format(_T("%s\\OpenAPI安装向导.url"), path);
// 	CString iconPath;
// 	iconPath.Format(_T("%s\\favicon.ico"), szWebstaionPath);
// 	CCommonFun::CreateURLLink(szURL, fpath, iconPath);

	//安装iws工具
	CString szIwsInstallTool;
	if (CCommonFun::IsWow64())
	{
		szIwsInstallTool.Format(_T("%s\\IwsInstallTool\\IwsInstallTool_x64.exe"), szWebstaionPath);
	}
	else
	{
		szIwsInstallTool.Format(_T("%s\\IwsInstallTool\\IwsInstallTool_x86.exe"), szWebstaionPath);
	}
	//好像只有win7以上版本可以安装，2008R2sp1无法安装，暂时先取消自动安装功能
	//Install(szIwsInstallTool,_T(""),INFINITE,SW_SHOW);
	*/
	theLog.Write(_T("安装EmbedPrintWebKM,end"));

	return TRUE;
}

//卸载嵌入式打印WEB(柯美)服务
BOOL CInstallApp::UnInstallEmbedPrintWebKM()
{
	theLog.Write(_T("卸载EmbedPrintWebKM,begin"));
	/*
	CString szWebName(VIRTUALNAME_KeMei);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);

	//删除共享
	if(!CCommonFun::DelNetShare(ShareName_KeMei))
	{
		theLog.Write(_T("!!UnInstallEmbedPrintWebKM,DelNetShare failed, szShareName=%s"), ShareName_KeMei);
	}

	//卸载嵌入式web站点
	CIISWeb web;
	CPtrList list;
	web.GetWebSiteList(&list);
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		S_SITEINFO* pInfo = (S_SITEINFO*)list.GetNext(pos);
		theLog.Write(_T("web nNumber:%d   szPort:%s  szIP:%s  szComment:%s   szVPath:%s\n"),
			pInfo->nNumber,pInfo->szPort,pInfo->szIP,pInfo->szComment,pInfo->szVPath);
		CString sPath;
		web.GetVirtualWebDirPath(pInfo->nNumber,szWebName,sPath);
		theLog.Write(_T("%s %s"),szWebName,sPath);
		if (web.IsVirtualDirExist(szWebName,pInfo->nNumber))
		{
			theLog.Write(_T("%s exist"),szWebName);
			web.DeleteVirtualDir(szWebName,pInfo->nNumber);
		}
	}

	web.RemoveAllList(list);
*/
	//删除配置快捷方式
// 	TCHAR   path[255];
// 	SHGetSpecialFolderPath(0,path,CSIDL_DESKTOPDIRECTORY,0); 
// 	CString fpath;
// 	fpath.Format(_T("%s\\OpenAPI安装向导.url"), path);
// 	DeleteFile(fpath);

	CCommonFun::StopSvc(_T("iSecPrintOpenApi"));
	theLog.Write(_T("卸载EmbedPrintWebKM,end"));
	return TRUE;
}

BOOL CInstallApp::UpdateEmbedPrintWebKMBegin()
{
	if (!m_bKMEmbedPrintInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateEmbedPrintWebKMBegin，m_bKMEmbedPrintInstalled=%d"), m_bKMEmbedPrintInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateEmbedPrintWebKMBegin,1"));
	CCommonFun::StopSvc(_T("iSecPrintOpenApi"));
	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\EmbedPrintWeb\\KeMeiPrint"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\EmbedPrintWeb\\KeMeiPrint"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateEmbedPrintWebKMBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);

#define UPDATE_FORM_5602_SETUP_TO_NEW
#ifdef UPDATE_FORM_5602_SETUP_TO_NEW
	{
		//自动创建bin文件夹
		CString szBinPath;
		szBinPath.Format(_T("%s\\KeMeiPrint\\bin"), g_sInstallPath);
		//删除目录下的错误bin文件
		DeleteFile(szBinPath);
		if (!CCommonFun::IfDirValidThenCreateIt(szBinPath))
		{
			theLog.Write(_T("!!UpdateEmbedPrintWebKMBegin,IfDirValidThenCreateIt failed,szBinPath=%s"), szBinPath);
		}
	}
	
#endif
	theLog.Write(_T("UpdateEmbedPrintWebKMBegin,100"));
	return TRUE;
}

BOOL CInstallApp::UpdateEmbedPrintWebKMEnd()
{
	theLog.Write(_T("UpdateEmbedPrintWebKMEnd,1"));
	if (!m_bKMEmbedPrintInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateEmbedPrintWebKMEnd，m_bKMEmbedPrintInstalled=%d"), m_bKMEmbedPrintInstalled);
		return TRUE;
	}
	CCommonFun::StartSvc(_T("iSecPrintOpenApi"));
	CWebServiceHelper oWebService;

	CIniFile oIniFile;
	oIniFile.m_sPath.Format(_T("%s\\EmbedPrintWeb\\%s"), g_sInstallPath, SYS_CONFIG_FILE);
	CString szServerIP  = oIniFile.GetVal(_T("CoreSvr"), _T("SvrHost"), _T("localhost"));
	int nPort = oIniFile.GetVal(_T("CoreSvr"), _T("SvrPort"), 5020);
	int nWebSvrPort = oIniFile.GetVal(_T("WebSvr"), _T("ServicePort"), 80);
	CString szWebserviceKey = oIniFile.GetVal(_T("WebSvr"), _T("WebserviceKey"), _T("xab!@#$%^&&*()?"));
	CString szServiceUrl = oIniFile.GetVal(_T("WebSvr"), _T("ServiceUrl"), _T(""));


	if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() <= 0))
	{
		szServiceUrl.Format(_T("http://localhost/xabprint/services"));
	}
	else if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() > 0))
	{
		szServiceUrl.Format(_T("http://%s:%d/xabprint/services"), szServerIP, nWebSvrPort);
	}

	int nServiceTimeout = oIniFile.GetVal(_T("WebSvr"), _T("ServiceTimeout"), 15000);
	BOOL bIEProxy = oIniFile.GetVal(_T("WebSvr"), _T("IEProxy"), FALSE);
	CString szIEProxyAddr = oIniFile.GetVal(_T("WebSvr"), _T("IEProxyAddr"), _T(""));
	int nIEProxyPort = oIniFile.GetVal(_T("WebSvr"), _T("IEProxyPort"), 808);

	oWebService.ConfigWebService(szWebserviceKey, szServiceUrl, nServiceTimeout, bIEProxy, szIEProxyAddr, nIEProxyPort);
	if (!oWebService.ConnWebService())
	{
		theLog.Write(_T("!!CLmMFPMgr::SetEmbeddedServer,1,ConnWebService fail"));

		//尝试使用https连接
		szServiceUrl = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("ServiceUrl"), _T(""));
		if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() <= 0))
		{
			szServiceUrl.Format(_T("https://localhost/xabprint/services"));
		}
		else if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() > 0))
		{
			szServiceUrl.Format(_T("https://%s:%d/xabprint/services"), szServerIP, nWebSvrPort);
		}
		oWebService.ConfigWebService(szWebserviceKey, szServiceUrl, nServiceTimeout, bIEProxy, szIEProxyAddr, nIEProxyPort);
		if (!oWebService.ConnWebService())
		{
			theLog.Write(_T("!!CLmMFPMgr::SetEmbeddedServer,1.2,ConnWebService fail"));
		}
		else
		{
			if (oWebService.KMNotifyUrl(CCommonFun::GetLocalIP()))
				theLog.Write(_T("CLmMFPMgr::SetEmbeddedServer,1.3,KMNotifyUrl succ"));
		}
	}
	else
	{
		if (oWebService.KMNotifyUrl(CCommonFun::GetLocalIP()))
			theLog.Write(_T("CLmMFPMgr::SetEmbeddedServer,1.4,KMNotifyUrl succ"));
	}
	theLog.Write(_T("UpdateEmbedPrintWebKMEnd,100"));
	return TRUE;
}

//安装嵌入式打印WEB(惠普Ent)服务
BOOL CInstallApp::InstallEmbedPrintWebHP()
{
	theLog.Write(_T("安装EmbedPrintWebHP,begin"));

	CString szWebName(VIRTUALNAME_HP);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);

	//安装.Net4.6
	InstallDotNet4x();

	DWORD dwMask = GENERIC_ALL|STANDARD_RIGHTS_ALL;
	CString szUser = _T("Everyone"); 
	//设置web目录权限
	AddObjectAccess(szWebstaionPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
	if(!CCommonFun::AddRights(szWebstaionPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebHP,2,AddRights,szWebstaionPath = %s"), szWebstaionPath);
	}

	//为WEB设置作业文件夹的访问权限
	CString szJobFilePath;
	szJobFilePath.Format(_T("%s\\Upload\\JobFile"), szWebstaionPath);
	CCommonFun::CreateDir(szJobFilePath);
	if(!CCommonFun::AddRights(szJobFilePath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebHP,3,AddRights,szJobFilePath = %s"), szJobFilePath);
	}

	//为WEB设置错误日志文件夹的访问权限
	CString szLogPath;
	szLogPath.Format(_T("%s\\Log"), szWebstaionPath);
	CCommonFun::CreateDir(szLogPath);
	if(!CCommonFun::AddRights(szLogPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebHP,4,AddRights,szLogPath = %s"), szLogPath);
	}

	//为WEB设置二维码文件夹的访问权限
	CString szQrPath;
	szQrPath.Format(_T("%s\\FrmHPEmb\\Images\\QR"), szWebstaionPath);
	CCommonFun::CreateDir(szQrPath);
	if(!CCommonFun::AddRights(szQrPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebHP,4,AddRights,szQrPath = %s"), szQrPath);
	}

	//配置【Web.config】文件
	{
		//<configuration>
		//	<appSettings>
		//		<add key="ApplicationIP" value="本机IP"/>

		CString szXmlFilePath;szXmlFilePath.Format(_T("%s\\Web.config"), szWebstaionPath);
		CXmlParse xml;
		if (xml.ParseXml(szXmlFilePath))
		{
			MSXML2::IXMLDOMDocument2Ptr doc = xml.GetXmlDocment();
			MSXML2::IXMLDOMNodePtr configNode = xml.SelectSingleNode(doc, _T("configuration"));
			MSXML2::IXMLDOMNodePtr appNode = xml.SelectSingleNode(configNode, _T("appSettings"));
			MSXML2::IXMLDOMNodePtr ApplicationIPNode = xml.SelectSingleNode(appNode, _T("add[@key='ApplicationIP']"));
			xml.SetAttribute(ApplicationIPNode, _T("value"), CCommonFun::GetLocalIP());	//修改IP
			MSXML2::IXMLDOMNodePtr ApplicationNameNode = xml.SelectSingleNode(appNode, _T("add[@key='ApplicationName']"));
			xml.SetAttribute(ApplicationNameNode, _T("value"), szWebName);	//修改网站虚拟目录
			MSXML2::IXMLDOMNodePtr CentralServerIPNode = xml.SelectSingleNode(appNode, _T("add[@key='CentralServerIP']"));
			xml.SetAttribute(CentralServerIPNode, _T("value"), CCommonFun::GetLocalIP());	//修改中心服务器IP
			MSXML2::IXMLDOMNodePtr CentralServerPortNode = xml.SelectSingleNode(appNode, _T("add[@key='CentralServerPort']"));
			xml.SetAttribute(CentralServerPortNode, _T("value"), _T("5020"));	//修改中心服务器IP

			xml.Save(doc, szXmlFilePath);	//回写Web.config
		}
	}

	//配置HPENT嵌入式web的URL
	{
		CoInitialize(0);
		CAdo ado;
		if (g_bDbBuildIn)
		{
			CString szServer = _T("localhost");
			CString szDbName = PRINT_DB_NAME;
			CString szUser = _T("sa");
			CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));
			CString szSaPassword = g_oIniFileInstall.GetVal(_T("DataBase"), _T("SaPassword"), _T(""));
#ifdef ENABLE_PWD_CRY
			char cPwd[MAX_PATH] = {0};
			int nSize = MAX_PATH;
			CPWDCry cry;
#ifdef UNICODE
			CStringA szSaPasswordA = CCommonFun::UnicodeToMultiByte(szSaPassword);
			cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
			szSaPasswordA.ReleaseBuffer();
			szSaPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
			cry.UnCryptPWD(szSaPassword.GetBuffer(), cPwd, nSize);
			szSaPassword.ReleaseBuffer();
			szSaPassword = cPwd;
#endif // UNICODE
#endif

			ado.SetConnStr3(szServer, szDbName, szUser, szSaPassword, szInstanceName);
		}
		else
		{
			ado.SetConnStr(g_oAdoInfo.szIP, g_oAdoInfo.szDbName, g_oAdoInfo.szUser, g_oAdoInfo.szPwd, g_oAdoInfo.szPort);
		}

		if (ado.Connect())
		{
			CString szRegister;
			szRegister.Format(_T("http://%s:%d/%s/hp/device"), CCommonFun::GetLocalIP(), 80, VIRTUALNAME_HP);
			CString szSql;
			szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'HPNotifyUrl' ")
				_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,Description) VALUES('HPNotifyUrl','%s','%s') ")
				, szRegister, _T("惠普：setvalue1:附件注册服务。"));

			if(!ado.ExecuteSQL(szSql))
			{
				theLog.Write(_T("初始化HPNotifyUrl失败！"));
			}
			theLog.Write(szSql);

		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebHP,6,Connect fail"));
		}
		CoUninitialize();
	}

	//安装嵌入式web站点
	CIISWeb is;
	TCHAR szUserName[100] = {0};

	//本机安装了iis
	if(is.GetAnonymousUserName(szUserName))
	{
		theLog.Write(_T("GetAnonymousUserName = %s, m_bQueit = %d"),szUserName, m_bQueit);
		if(m_bQueit)
		{
			CEmbedWebSheet sheet(_T("嵌入式Web服务配置向导"));
			sheet.page2.m_szVirtualPath = szWebstaionPath;
			sheet.page2.m_szVirtaulName = szWebName;
			sheet.page2.SetAppPool(_T("v4.0"));	//要求使用[.net4.0]的应用程序池
			sheet.page2.FinishWebSiteSet(TRUE);
		}
		else
		{
			CEmbedWebSheet sheet(_T("嵌入式Web服务配置向导"));
			sheet.page2.m_szVirtualPath = szWebstaionPath;
			sheet.page2.m_szVirtaulName = szWebName;
			sheet.page2.SetAppPool(_T("v4.0"));	//要求使用[.net4.0]的应用程序池
			sheet.DoModal();
		}
	}
	else
	{
		CEmbedWebSheet sheet(0,_T("打印机Web服务配置向导"));
		if(m_bQueit)
		{
			//sheet.WriteTheURL(TRUE);
		}
		else
		{
			sheet.DoModal();
		}

		CString run;
		run.Format(_T("\"%s\\bin\\instsrv.exe\" WebServer \"%s\\bin\\srvany.exe\""), g_sDefaultPath, g_sDefaultPath);
		CCommonFun::CreateProcessAndWait(run.GetBuffer());

		CRegistryEx pReg;
		pReg.CreateKey(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters"));
		pReg.SetRootKey(HKEY_LOCAL_MACHINE);
		if(pReg.Open(_T("SYSTEM\\CurrentControlSet\\Services\\WebServer\\Parameters")))
		{
			pReg.Write(_T("Application"),g_sDefaultPath+_T("\\bin\\WebServer.exe"));
			CString sparm;

			sparm.Format(_T("/port:80 /path:\"%s\" /vpath:\"/%s\" "),g_sDefaultPath, szWebName);
			pReg.Write(_T("AppParameters"),sparm);
			pReg.Close();
		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebHP, pReg.Open failed"));
		}
		::Sleep(100);
		CCommonFun::StartSvc(_T("WebServer"));
	}

	theLog.Write(_T("安装EmbedPrintWebHP,end"));

	return TRUE;
}

//卸载嵌入式打印WEB(惠普Ent)服务
BOOL CInstallApp::UnInstallEmbedPrintWebHP()
{
	theLog.Write(_T("卸载EmbedPrintWebHP,begin"));

	CString szWebName(VIRTUALNAME_HP);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);

	//卸载嵌入式web站点
	CIISWeb web;
	CPtrList list;
	web.GetWebSiteList(&list);
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		S_SITEINFO* pInfo = (S_SITEINFO*)list.GetNext(pos);
		theLog.Write(_T("web nNumber:%d   szPort:%s  szIP:%s  szComment:%s   szVPath:%s\n"),
			pInfo->nNumber,pInfo->szPort,pInfo->szIP,pInfo->szComment,pInfo->szVPath);
		CString sPath;
		web.GetVirtualWebDirPath(pInfo->nNumber,szWebName,sPath);
		theLog.Write(_T("%s %s"),szWebName,sPath);
		if (web.IsVirtualDirExist(szWebName,pInfo->nNumber))
		{
			theLog.Write(_T("%s exist"),szWebName);
			web.DeleteVirtualDir(szWebName,pInfo->nNumber);
		}
	}

	web.RemoveAllList(list);

	theLog.Write(_T("卸载EmbedPrintWebHP,end"));
	return TRUE;
}

BOOL CInstallApp::UpdateEmbedPrintWebHPBegin()
{
	if (!m_bHPEmbedPrintInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateEmbedPrintWebHPBegin，m_bHPEmbedPrintInstalled=%d"), m_bHPEmbedPrintInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateEmbedPrintWebHPBegin,1"));
	CCommonFun::StopSvc(_T("W3SVC"));
	FindTerminateProcess(_T("w3wp.exe"));
	CString szSrcDir, szTargetDir;
	szSrcDir.Format(_T("%s\\EmbedPrintWeb\\HpPrint"), g_sInstallPath);
	COleDateTime time = COleDateTime::GetCurrentTime();
	szTargetDir.Format(_T("%s\\EmbedPrintWeb\\HpPrint"), m_szThisBackupDir);
	if (!CCommonFun::IfDirValidThenCreateIt(szTargetDir))
	{
		theLog.Write(_T("!!UpdateEmbedPrintWebHPBegin,IfDirValidThenCreateIt failed,szTargetDir=%s"), szTargetDir);
	}
	CString szCmd;
	szCmd.Format(_T("xcopy \"%s\" \"%s\" /E /Y"), szSrcDir, szTargetDir);
	BOOL bret = CCommonFun::ExcuteCmd(szCmd, TRUE);
	theLog.Write(_T("UpdateEmbedPrintWebHPBegin,100"));
	return TRUE;
}

BOOL CInstallApp::UpdateEmbedPrintWebHPEnd()
{
	if (!m_bHPEmbedPrintInstalled)
	{
		theLog.Write(_T("CInstallApp::UpdateEmbedPrintWebHPEnd，m_bHPEmbedPrintInstalled=%d"), m_bHPEmbedPrintInstalled);
		return TRUE;
	}
	theLog.Write(_T("UpdateEmbedPrintWebHPEnd,1"));
	CCommonFun::StartSvc(_T("W3SVC"));
	theLog.Write(_T("UpdateEmbedPrintWebHPEnd,100"));
	return TRUE;
}

//安装Apache和PHP运行环境
BOOL CInstallApp::InstallApacheAndPHP()
{
	theLog.Write(_T("安装ApacheAndPHP,begin"));

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb\\ApacheAndPHP"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);
	theConfig.SetCurrentConfig(_T(""));

	CString szDirectory;szDirectory.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	szDirectory.Replace(_T("\\"), _T("/"));
	theLog.Write(_T("szDirectory %s"), szDirectory);

	int nApachePort = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("ApachePort"), 8081);
	theLog.Write(_T("InstallApacheAndPHP,nApachePort %d"), nApachePort);

	CString szApacheRoot = g_sDefaultPath;
	szApacheRoot.Replace(_T("\\"), _T("/"));

	//Apache
	CStringArray sSouAry, sRepAry;
	CString sTemp, sDest;

	// ServerRoot
	sTemp = _T("ServerRoot \"D:/phpStudyB/apache\"");
	sDest.Format(_T("ServerRoot \"%s/apache\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// Listen
	sTemp = _T("Listen 808");
	sDest.Format(_T("Listen %d"), nApachePort);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// DocumentRoot
	sTemp = _T("DocumentRoot  \"D:\\phpStudyB\\WWW\"");
	sDest.Format(_T("DocumentRoot  \"%s\""), szDirectory);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// Directory
	sTemp = _T("<Directory \"D:\\phpStudyB\\WWW\">");
	sDest.Format(_T("<Directory \"%s\">"), szDirectory);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// ScriptAlias
	sTemp = _T("    ScriptAlias /cgi-bin/ \"D:/phpStudyB/apache/cgi-bin/\"");
	sDest.Format(_T("    ScriptAlias /cgi-bin/ \"%s/apache/cgi - bin/\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// Directory
	sTemp = _T("<Directory \"D:/phpStudyB/apache/cgi-bin\">");
	sDest.Format(_T("<Directory \"%s/apache/cgi-bin\">"), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// LoadFile php5ts
	sTemp = _T("LoadFile \"D:/phpStudyB/PHP/php5ts.dll\"");
	sDest.Format(_T("LoadFile \"%s/PHP/php5ts.dll\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	// LoadModule php5_module "D:/phpStudyB/PHP/php5apache2_2.dll"
	sTemp = _T("LoadModule php5_module \"D:/phpStudyB/PHP/php5apache2_2.dll\"");
	sDest.Format(_T("LoadModule php5_module \"%s/PHP/php5apache2_2.dll\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//     PHPIniDir "D:/phpStudyB/PHP/"
	sTemp = _T("    PHPIniDir \"D:/phpStudyB/PHP/\"");
	sDest.Format(_T("    PHPIniDir \"%s/PHP/\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//LoadFile "D:/phpStudyB/PHP/libmysql.dll"
	sTemp = _T("LoadFile \"D:/phpStudyB/PHP/libmysql.dll\"");
	sDest.Format(_T("LoadFile \"%s/PHP/libmysql.dll\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//LoadFile "D:/phpStudyB/PHP/libmcrypt.dll"
	sTemp = _T("LoadFile \"D:/phpStudyB/PHP/libmcrypt.dll\"");
	sDest.Format(_T("LoadFile \"%s/PHP/libmcrypt.dll\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//禁用Apache的目录浏览功能，否则可能出现外部非法访问文件。
	sTemp = _T("Options Indexes FollowSymLinks");
	sDest = _T("Options -Indexes FollowSymLinks");
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	CString sCfg;
	sCfg.Format(_T("%s\\Apache\\conf\\httpd.conf.1"), szApacheRoot);
	theLog.Write(sCfg);
	CString sNewCfg;
	sNewCfg.Format(_T("%s\\Apache\\conf\\httpd.conf"), szApacheRoot);
	theLog.Write(sCfg);
	CStdioFile file,newFile;
	if(newFile.Open(sNewCfg, CFile::modeCreate|CFile::modeWrite) && file.Open(sCfg, CFile::modeRead)) 
	{
		CString sLine;
		while (file.ReadString(sLine))
		{
			for (int i = 0; i < sSouAry.GetCount(); i++)
			{
				if (sLine.Find(sSouAry.GetAt(i)) >= 0)
				{
					theLog.Write(sLine);
					sLine.Replace(sSouAry.GetAt(i), sRepAry.GetAt(i));
					break;
				}
			}
			newFile.WriteString(sLine + _T("\r\n"));
		}
		file.Close();
		newFile.Close();
	}
	else
	{
		theLog.Write(_T("CInstallApp::InstallApacheAndPHP,Open file,error=%u,sNewCfg=%s"), GetLastError(), sNewCfg);
	}

	//////////////////////////////////////////////////////////////////////////
	//php
	sSouAry.RemoveAll();
	sRepAry.RemoveAll();

	//extension_dir ="D:\phpStudyB\PHP\ext"
	sTemp = _T("extension_dir =\"D:\\phpStudyB\\PHP\\ext\"");
	sDest.Format(_T("extension_dir =\"%s\\PHP\\ext\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//xdebug.profiler_output_dir ="D:\phpStudyB\tmp"
	sTemp = _T("xdebug.profiler_output_dir =\"D:\\phpStudyB\\tmp\"");
	sDest.Format(_T("xdebug.profiler_output_dir =\"%s\\tmp\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//xdebug.trace_output_dir ="D:\phpStudyB\tmp"
	sTemp = _T("xdebug.trace_output_dir =\"D:\\phpStudyB\\tmp\"");
	sDest.Format(_T("xdebug.trace_output_dir =\"%s\\tmp\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//eaccelerator.cache_dir ="D:\phpStudyB\tmp"
	sTemp = _T("eaccelerator.cache_dir =\"D:\\phpStudyB\\tmp\"");
	sDest.Format(_T("eaccelerator.cache_dir =\"%s\\tmp\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//zend_extension_manager.optimizer_ts ="D:\phpStudyB\ZendOptimizer\lib\Optimizer"
	sTemp = _T("zend_extension_manager.optimizer_ts =\"D:\\phpStudyB\\ZendOptimizer\\lib\\Optimizer\"");
	sDest.Format(_T("zend_extension_manager.optimizer_ts =\"%s\\ZendOptimizer\\lib\\Optimizer\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	//zend_extension_ts ="D:\phpStudyB\ZendOptimizer\lib\ZendExtensionManager.dll"
	sTemp = _T("zend_extension_ts =\"D:\\phpStudyB\\ZendOptimizer\\lib\\ZendExtensionManager.dll\"");
	sDest.Format(_T("zend_extension_ts =\"%s\\ZendOptimizer\\lib\\ZendExtensionManager.dll\""), szApacheRoot);
	sSouAry.Add(sTemp);
	sRepAry.Add(sDest);

	sCfg.Format(_T("%s\\PHP\\php.ini.1"), szApacheRoot);
	theLog.Write(sCfg);
	sNewCfg.Format(_T("%s\\PHP\\php.ini"), szApacheRoot);
	theLog.Write(sNewCfg);
	if (newFile.Open(sNewCfg, CFile::modeCreate | CFile::modeWrite) && file.Open(sCfg, CFile::modeRead))
	{
		CString sLine;
		while (file.ReadString(sLine))
		{
			for (int i = 0; i < sSouAry.GetCount(); i++)
			{
				if (sLine.Find(sSouAry.GetAt(i)) >= 0)
				{
					theLog.Write(sLine);
					sLine.Replace(sSouAry.GetAt(i), sRepAry.GetAt(i));
					break;
				}
			}
			newFile.WriteString(sLine + _T("\r\n"));
		}
		file.Close();
		newFile.Close();
	}
	else
	{
		theLog.Write(_T("CInstallApp::InstallApacheAndPHP,Open file,error=%u,newFile=%s"), GetLastError(), newFile);
	}

	// 安装并服务
	CString sPath;
	sPath.Format(_T("%s\\Apache\\bin\\httpd.exe"), szApacheRoot);
	Install(sPath,_T("-k install"),INFINITE,SW_HIDE);

	//设置失败后重启
	CCommonFun::SetFailedAction(_T("Apache2.2"));

	//启动服务
	Install(sPath,_T("-k start"),INFINITE,SW_HIDE);
	
	theLog.Write(_T("安装ApacheAndPHP,end"));
	return TRUE;
}

//卸载Apache和PHP运行环境
BOOL CInstallApp::UnInstallApacheAndPHP()
{
	theLog.Write(_T("卸载ApacheAndPHP,begin"));

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb\\ApacheAndPHP"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString sPath;
	sPath.Format(_T("%s\\Apache\\bin\\httpd.exe"), g_sDefaultPath);
	Install(sPath,_T("-k stop"),INFINITE,SW_HIDE);
	Install(sPath,_T("-k uninstall"),INFINITE,SW_HIDE);

	theLog.Write(_T("卸载ApacheAndPHP,end"));
	return TRUE;
}

//安装嵌入式打印WEB(佳能)服务
BOOL CInstallApp::InstallEmbedPrintWebCanon()
{
	theLog.Write(_T("安装EmbedPrintWebCanon,begin"));

	CString szWebName(VIRTUALNAME_Canon);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);

	DWORD dwMask = GENERIC_ALL|STANDARD_RIGHTS_ALL;
	CString szUser = _T("Everyone"); 
	//设置web目录权限
	AddObjectAccess(szWebstaionPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
	if(!CCommonFun::AddRights(szWebstaionPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebCanon,2,AddRights,szWebstaionPath = %s"), szWebstaionPath);
	}

	//为WEB设置日志文件夹的访问权限
	CString szLogPath;
	szLogPath.Format(_T("%s\\Log"), szWebstaionPath);
	CCommonFun::CreateDir(szLogPath);
	if(!CCommonFun::AddRights(szLogPath.GetBuffer(),szUser.GetBuffer(),dwMask))
	{
		theLog.Write(_T("!!InstallEmbedPrintWebCanon,2,AddRights,szLogPath = %s"), szLogPath);
	}

	//配置HPENT嵌入式web的URL
	{
		CoInitialize(0);
		CAdo ado;
		if (g_bDbBuildIn)
		{
			CString szServer = _T("localhost");
			CString szDbName = PRINT_DB_NAME;
			CString szUser = _T("sa");
			CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));
			CString szSaPassword = g_oIniFileInstall.GetVal(_T("DataBase"), _T("SaPassword"), _T(""));
#ifdef ENABLE_PWD_CRY
			char cPwd[MAX_PATH] = {0};
			int nSize = MAX_PATH;
			CPWDCry cry;
#ifdef UNICODE
			CStringA szSaPasswordA = CCommonFun::UnicodeToMultiByte(szSaPassword);
			cry.UnCryptPWD(szSaPasswordA.GetBuffer(), cPwd, nSize);
			szSaPasswordA.ReleaseBuffer();
			szSaPassword = CCommonFun::MultiByteToUnicode(cPwd);
#else
			cry.UnCryptPWD(szSaPassword.GetBuffer(), cPwd, nSize);
			szSaPassword.ReleaseBuffer();
			szSaPassword = cPwd;
#endif // UNICODE

#endif

			ado.SetConnStr3(szServer, szDbName, szUser, szSaPassword, szInstanceName);
		}
		else
		{
			ado.SetConnStr(g_oAdoInfo.szIP, g_oAdoInfo.szDbName, g_oAdoInfo.szUser, g_oAdoInfo.szPwd, g_oAdoInfo.szPort);
		}

		if (ado.Connect())
		{
			//CanonNotifyUrl
			//https://192.168.3.196:8081/CanonPrint

			int nApachePort = g_oIniFileInstall.GetVal(_T("EmbedPrintWeb"), _T("ApachePort"), 8081);
			theLog.Write(_T("InstallEmbedPrintWebCanon,nApachePort %d"), nApachePort);

			CString szCanonNotifyUrl;
			szCanonNotifyUrl.Format(_T("http://%s:%d/%s"), CCommonFun::GetLocalIP(), nApachePort, VIRTUALNAME_Canon);
			CString szSql;
			szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'CanonNotifyUrl' ")
				_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2,Description) VALUES('CanonNotifyUrl','%s','%s','%s') ")
				, szCanonNotifyUrl, szCanonNotifyUrl, _T("佳能：setvalue2:服务接口。"));

			if(!ado.ExecuteSQL(szSql))
			{
				theLog.Write(_T("初始化CanonNotifyUrl失败！"));
			}
			theLog.Write(szSql);

		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebCanon,6,Connect fail"));
		}
		CoUninitialize();
	}

#if 0
	//配置网络连接
	{
		CStringArray ary;
		CString szConfig;szConfig.Format(_T("%s\\comm\\config.php"), szWebstaionPath);
		//define('SYS_SERVER','localhost');
		CString szSysServer;szSysServer.Format(_T("define('SYS_SERVER','%s');"), CCommonFun::GetLocalIP());
		ary.Add(szSysServer);

		CStdioFile oFile;
		if (oFile.Open(szConfig, CFile::modeNoTruncate|CFile::modeReadWrite))
		{
			CString szRead;
			while (oFile.ReadString(szRead))
			{
				if (szRead.Find(_T("'SYS_SERVER'")) >= 0)
				{
					//放弃原来参数，使用新参数
				}
				else
				{
					ary.Add(szRead);
				}
			}

			//关闭文件
			oFile.Close();
		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebCanon,4,Open fail,err=%u, szConfig=%s"), GetLastError(), szConfig);
		}

		//重新打印文件
		if (oFile.Open(szConfig, CFile::modeWrite))
		{
			//回写参数到文件
			for (int i=0; i<ary.GetCount(); i++)
			{
				oFile.WriteString(ary.GetAt(i));
				oFile.WriteString(_T("\r\n"));
			}

			//关闭文件
			oFile.Close();
		}
		else
		{
			theLog.Write(_T("!!InstallEmbedPrintWebCanon,4,Open fail,err=%u, szConfig=%s"), GetLastError(), szConfig);
		}
	}
#endif

	theLog.Write(_T("安装EmbedPrintWebCanon,end"));

	return TRUE;
}

//卸载嵌入式打印WEB(佳能)服务
BOOL CInstallApp::UnInstallEmbedPrintWebCanon()
{
	theLog.Write(_T("卸载EmbedPrintWebCanon,begin"));

	CString szWebName(VIRTUALNAME_Canon);
	theLog.Write(_T("szWebName=%s"), szWebName);

	g_sDefaultPath.Format(_T("%sEmbedPrintWeb"), CCommonFun::GetDefaultPath());
	theLog.Write(_T("g_sDefaultPath %s"), g_sDefaultPath);

	CString szWebstaionPath;
	szWebstaionPath.Format(_T("%s\\%s"), g_sDefaultPath, szWebName);
	theLog.Write(_T("szWebstaionPath %s"), szWebstaionPath);

	theLog.Write(_T("卸载EmbedPrintWebCanon,begin"));
	return TRUE;
}

BOOL CInstallApp::Install_VC_Redist_x86_2015()
{
	theLog.Write(_T("开始安装VC_Redist_x86_2015"));
	CString szPath;
	CString szParameter;
	szPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),_T("vc2015\\vc_redist.x86.exe"));
	szParameter.Format(_T("/q"));
	Install(szPath,szParameter,INFINITE,SW_HIDE);
	theLog.Write(_T("结束安装VC_Redist_x86_2015"));
	return TRUE;
}

//===========================
BOOL CDrvInstallHelper::Load()
{
	if (m_hModule)
	{
		return TRUE;
	}

	CString szDefPath = CCommonFun::GetDefaultPath();
	CString szDll;
	szDll.Format(_T("%sDrvInstall.dll"), szDefPath);

//	m_hModule = ::LoadLibrary(_T(_T("DrvInstall.dll")));
	m_hModule = ::LoadLibrary(szDll);
	if (m_hModule)
	{
		theLog.Write(_T("CDrvInstallHelper::Load,1,LoadLibrary DrvInstall.dll OK,szDll=%s")
			, szDll);

		return TRUE;
	}

	theLog.Write(_T("!!CDrvInstallHelper::Load,2.0,LoadLibrary DrvInstall.dll fail,err=%d,szDll=%s")
		, GetLastError(), szDll);
	return FALSE;
}
bool CDrvInstallHelper::CallFun(char* szFun)
{
	typedef bool ( *XAB_INSTALL)(TCHAR *);
	XAB_INSTALL  pInstall=NULL;
	if(Load())
	{
		pInstall = (XAB_INSTALL)GetProcAddress (m_hModule, szFun);
		if (pInstall)
		{
			bool bRet = pInstall(m_szPath);
			if(!bRet)
			{
				theLog.Write(_T("!!CDrvInstallHelper::CallFun,1,pInstall %s fail,err=%d"), m_szPath, GetLastError());
			}
			else
			{
				theLog.Write(_T("CDrvInstallHelper::CallFun,2,pInstall %s OK"), m_szPath);
			}
			return bRet;
		}
		else
		{
			theLog.Write(_T("!!CDrvInstallHelper::CallFun,3,GetProcAddress %s fail,err=%d,DefPath=%s")
				, szFun, GetLastError(), CCommonFun::GetDefaultPath());
			return false;
		}
	}

	theLog.Write(_T("!!CDrvInstallHelper::CallFun,4,Load DrvInstall.dll fail,err=%d"), GetLastError());
	return false;
}


//===========================

CString CProductInfo::GetEduPrintProductName()
{
	CString sVal = g_oIniFileSet.GetVal(_T("General"),_T("product"),_T(""));
	if(sVal.IsEmpty())
	{
		sVal = _T("信安之星校园打印复印管理系统V5.0");
	}

	return sVal;
}

CString CProductInfo::GetPrintProductName()
{
	CString sVal = g_oIniFileSet.GetVal(_T("General"),_T("product"),_T(""));
	if(sVal.IsEmpty())
	{
		sVal = _T("信安之星打印管理系统V5.0");
	}

	return sVal;
}

//add by zfq,从程序目录中的到产品目录,即含有“打印复印管理系统”的目录
BOOL CProductInfo::GetProductDirNameInProgramDir(CString &szProgramDir, CString &szProductDir)
{
	if(szProgramDir.IsEmpty())
	{
		theLog.Write(_T("!!CProductInfo::GetProductDirNameInProgramDir,1,szProgramDir=%s"), szProgramDir);
		return FALSE;
	}
	if(!::PathFileExists(szProgramDir))
	{
		theLog.Write(_T("!!CProductInfo::GetProductDirNameInProgramDir,2,szProgramDir=%s,NOT Exists"), szProgramDir);
		return FALSE;
	}

	//遍历该目录，找到含有产品的目录
	CString szDestName = _T("打印复印管理系统");	//校园打印
	CString szDestName2 = _T("云打印服务平台");
	CString szDestName3 = _T("创时云自助终端管理系统");	//OEM
	CString szDestName4 = _T("鑫大为");	//OEM
	CString szDestName5 = _T("普亚恒捷");	//OEM
	CString szDestName6 = _T("正元智慧");	//OEM
	CString szDestName7 = _T("噢易");		//OEM
	CString szDestName8 = _T("打印管理系统");	//企业打印
	CFileFind	finder;				
	CString		sFullPath = _T("");					
	CString		strWildcard;

	BOOL BGet = FALSE;
	CString sDir = szProgramDir;
	sDir.TrimRight(_T("\\"));

	strWildcard = sDir + _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots())
			continue;

		if (!finder.IsDirectory())
		{//不是目录
			continue;
		}
	
		CString sName = finder.GetFileName();
		CString sPath = finder.GetFilePath();

		if((-1 == sName.Find(szDestName)) && (-1 == sName.Find(szDestName2)) 
			&& (-1 == sName.Find(szDestName3)) && (-1 == sName.Find(szDestName4))
			&& (-1 == sName.Find(szDestName5)) && (-1 == sName.Find(szDestName6))
			&& (-1 == sName.Find(szDestName7)) && (-1 == sName.Find(szDestName8))
			)
		{//不是要找的目录
			continue;
		}
	
		BGet = TRUE;
		szProductDir = sName;
		break;
	}
	finder.Close();	

	return BGet;
}
