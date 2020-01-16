// WebSheet.cpp : implementation file
//

#include "stdafx.h"
#include "WebSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebSheet

IMPLEMENT_DYNAMIC(CWebSheet, CPropertySheet)

CWebSheet::CWebSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&page1);
	AddPage(&page2);
	m_bSuc = FALSE;

	m_psh.dwFlags &= ~(PSH_HASHELP);
	m_psh.dwFlags |= PSH_WIZARD;

	page1.m_psp.dwFlags &= ~(PSP_HASHELP);
	page2.m_psp.dwFlags &= ~(PSP_HASHELP);
}

CWebSheet::CWebSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&page1);
	AddPage(&page2);
	m_bSuc = FALSE;

	m_psh.dwFlags &= ~(PSH_HASHELP);
	m_psh.dwFlags |= PSH_WIZARD;

	page1.m_psp.dwFlags &= ~(PSP_HASHELP);
	page2.m_psp.dwFlags &= ~(PSP_HASHELP);
}

CWebSheet::CWebSheet( UINT iSelectPage,LPCTSTR pszCaption)
:CPropertySheet(pszCaption, NULL, 0)
{
	m_bSuc = FALSE;
	if (iSelectPage == 0)
	{
		AddPage(&page1);
		page1.m_psp.dwFlags &= ~(PSP_HASHELP);
	}
	else if (iSelectPage == 1)
	{
		AddPage(&page2);
		page2.m_psp.dwFlags &= ~(PSP_HASHELP);
	}
	else if (iSelectPage == 2)
	{
		AddPage(&page3);
		page3.m_psp.dwFlags &= ~(PSP_HASHELP);
	}
	else if (iSelectPage == 3)
	{
		AddPage(&page1);
		AddPage(&page2);
		AddPage(&page3);
		page1.m_psp.dwFlags &= ~(PSP_HASHELP);
		page2.m_psp.dwFlags &= ~(PSP_HASHELP);
		page3.m_psp.dwFlags &= ~(PSP_HASHELP);
	}
	//AddPage(&page2);

	m_psh.dwFlags &= ~(PSH_HASHELP);
	m_psh.dwFlags |= PSH_WIZARD;
}

CWebSheet::~CWebSheet()
{
}


BEGIN_MESSAGE_MAP(CWebSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWebSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebSheet message handlers
void CWebSheet::WriteTheURL(BOOL bQueit,CString szSerIp/* = _T("")*/, CString szSerPort/* = _T("80")*/)
{
	theLog.Write(_T("WriteTheURL,bQueit=%d,szSerIp=%s,szSerPort=%s"), bQueit, szSerIp, szSerPort);

	CoInitialize(0);
	CAdo ado;
	if(bQueit)
	{
		if (g_bDbBuildIn)
		{
			CString szMsg;
			CCommonFun::GetDoNetDBStr(szMsg);
			CString sConnStr;
			sConnStr.Format(_T("Provider=sqloledb; %s"), szMsg);
			ado.SetConnStr(sConnStr);
		}
		else
		{
			CString szIP = g_oAdoInfo.szIP;
			CString szDbName = g_oAdoInfo.szDbName;
			CString szUserName = g_oAdoInfo.szUser;
			CString szUserPwd = g_oAdoInfo.szPwd;
			CString szPort = g_oAdoInfo.szPort;

			theLog.Write(_T("WriteTheURL,2---szIP:%s---szDbName:%s---szUserName:%s---szPort:%s"),
				szIP,szDbName,szUserName,szPort);
			ado.SetConnStr(szIP,szDbName,szUserName,szUserPwd,szPort);
		}
	}
	else
	{
		CString szIP = page1.m_szIP;
		CString szDbName = page1.m_szDbName;
		CString szUserName = page1.m_szUserName;
		CString szUserPwd = page1.m_szUserPwd;
		CString szPort = page1.m_szPort;

		theLog.Write(_T("WriteTheURL---szIP:%s---szDbName:%s---szUserName:%s---szUserPwd:%s---szPort:%s"),
			szIP,szDbName,szUserName,szUserPwd,szPort);
		ado.SetConnStr(szIP,szDbName,szUserName,szUserPwd,szPort);
	}
	
	if (ado.Connect())
	{
		CString szSql;

		//添加扫描默认目录
		CString sHttpScanDir;
		CString sScanDir;
		CString sDownload;
		CString sPath = g_sDefaultPath;

		sScanDir.Format(_T("%s\\%s"),sPath,SCAN_DIR);
		CCommonFun::CreateDir(sScanDir);
		AddObjectAccess(sScanDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
		
		sDownload.Format(_T("%s\\%s"),sPath,DOWN_DIR);
		CCommonFun::CreateDir(sDownload);
		AddObjectAccess(sDownload,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		if (szSerIp.IsEmpty())
		{
			szSerIp = CCommonFun::GetLocalIP();
		}

		sHttpScanDir.Format(_T("http://%s:%s/%s/%s"),szSerIp,szSerPort,VIRTUALNAME,SCAN_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'ScanDir' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('ScanDir','%s','%s') "),sScanDir,sHttpScanDir);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化scandir失败！"));
		}
		theLog.Write(szSql);

		//给UpFile文件夹赋权限
		CString sUpFileDir;
		sUpFileDir.Format(_T("%s\\%s"),sPath,UPFILR_DIR);
		if(!PathFileExists(sUpFileDir))
		{
			CCommonFun::CreateDir(sUpFileDir);
		}
		AddObjectAccess(sUpFileDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		//添加打印目录
		CString sHttpPrintDir;
		CString sPrintDir;
		sPrintDir.Format(_T("%s\\%s"),sPath,PRINT_DIR);
		CCommonFun::CreateDir(sPrintDir);
		AddObjectAccess(sPrintDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		sHttpPrintDir.Format(_T("http://%s:%s/%s/%s"),szSerIp,szSerPort,VIRTUALNAME,PRINT_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'PrintDir' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('PrintDir','%s','%s') "),sPrintDir,sHttpPrintDir);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化sPrintDir失败！"));
		}
		theLog.Write(szSql);
		
		//添加复印留底目录
		CString sHttpCopyBakDir;
		CString sCopyBakDir;
		sCopyBakDir.Format(_T("%s\\%s"),sPath,COPYBAK_DIR);
		CCommonFun::CreateDir(sCopyBakDir);
		AddObjectAccess(sCopyBakDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		sHttpCopyBakDir.Format(_T("http://%s:%s/%s/%s"),szSerIp,szSerPort,VIRTUALNAME,COPYBAK_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'CopyBakDir' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('CopyBakDir','%s','%s') "),sCopyBakDir,sHttpCopyBakDir);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化CopyBakDir失败！"));
		}
		theLog.Write(szSql);

		//添加报表目录
		CString sHttpReportDir;
		CString sReportDir;
		sReportDir.Format(_T("%s\\%s"),sPath,REPORT_DIR);
		CCommonFun::CreateDir(sReportDir);
		AddObjectAccess(sReportDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		sHttpReportDir.Format(_T("http://%s:%s/%s/%s"),szSerIp,szSerPort,VIRTUALNAME,REPORT_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'ReportDir' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('ReportDir','%s','%s') "),sReportDir,sHttpReportDir);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化sReportDir失败！"));
		}
		theLog.Write(szSql);

		//添加OCR目录
		CString sViolationFileDir; sViolationFileDir.Format(_T("%s\\IndexFile\\ViolationFile"), sPath);
		CString sPrintOcrDir; sPrintOcrDir.Format(_T("%s\\IndexFile\\Print"), sPath);
		CString sCopyOcrDir; sCopyOcrDir.Format(_T("%s\\IndexFile\\Copy"), sPath);
		CString sScanOcrDir; sScanOcrDir.Format(_T("%s\\IndexFile\\Scan"), sPath);
		CString sDataOcrDir; sDataOcrDir.Format(_T("%s\\IndexFile\\Data"), sPath);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = N'OcrDir' "
			" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue3,SetValue4,SetValue5,SetValue6,SetValue7)"
			" VALUES(N'OcrDir',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s') ")
			, sViolationFileDir, sPrintOcrDir, sCopyOcrDir, sScanOcrDir, _T("0"), sDataOcrDir);

		if (!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化OcrDir失败！"));
		}
		theLog.Write(szSql);

		szSql.Empty();
		//设置默认的打印系统后端语言
		UiLanguage language = UILANG_CHINESE;
		LANGID lid = GetUserDefaultUILanguage();
		//0x0409 - 英文；0x0804 - 中文；
		if (lid != 0x0804)	//中文
		{
			language = UILANG_ENGLISH;
			//暂时将一些特别的内容修改成英文内容
			szSql.Append(_T(" update t_department set name=N'Org' where id=1; "));	//组织
			szSql.Append(_T(" update t_department set name=N'Guest' where id=2; "));	//访客
			szSql.Append(_T(" update P_AdminInfo set Comment=N'Super administrator can not delete' where ID=1; "));	//admin超级管理员注释
		}
		szSql.AppendFormat(_T(" update t_s_Systemset set SetValue1=N'%d' WHERE SetName = N'BackendLanguage'; "), language);
		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化BackendLanguage失败！"));
		}
		theLog.Write(szSql);

		//添加客户端升级目录
		CString sHttpClientDir;
		CString sClientDir;
		sClientDir.Format(_T("%s\\%s"),sPath,CLIENT_DIR);
		CCommonFun::CreateDir(sClientDir);
		AddObjectAccess(sClientDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

		sHttpClientDir.Format(_T("http://%s:%s/%s/%s"),szSerIp,szSerPort,VIRTUALNAME,CLIENT_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'ClientDir' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('ClientDir','%s','%s') "),sClientDir,sHttpClientDir);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化sClientDir失败！"));
		}
		theLog.Write(szSql);

		//通知服务器配置变更
		CCommonFun::NotifyFromWeb(0);
	}

	CoUninitialize();
}

void CWebSheet::WriteTheFtp(BOOL bQueit)
{
	CString szSql;
	CoInitialize(0);
	CAdo ado;
	if(bQueit)
	{
		CString szMsg;
		CCommonFun::GetDoNetDBStr(szMsg);
		CString sConnStr;
		sConnStr.Format(_T("Provider=sqloledb; %s"), szMsg);
		ado.SetConnStr(sConnStr);
	}
	else
	{
		CString szIP = page1.m_szIP;
		CString szDbName = page1.m_szDbName;
		CString szUserName = page1.m_szUserName;
		CString szUserPwd = page1.m_szUserPwd;
		CString szPort = page1.m_szPort;

		theLog.Write(_T("WriteTheURL---szIP:%s---szDbName:%s---szUserName:%s---szUserPwd:%s---szPort:%s"),
			szIP,szDbName,szUserName,szUserPwd,szPort);
		ado.SetConnStr(szIP,szDbName,szUserName,szUserPwd,szPort);
	}

	if (ado.Connect())
	{
		//添加扫描默认目录
		CString sHttpScanDir;
		CString sScanDir;
		CString sPath = g_sDefaultPath;
		sPath.MakeLower();
		int nFind = sPath.Find(_T("bin"));
		if (nFind > 0)
		{
			sScanDir.Format(_T("%s\\%s"),sPath.Left(nFind),SCAN_DIR);
		}
		CCommonFun::CreateDir(sScanDir);

		sHttpScanDir.Format(_T("http://%s/%s/%s"),CCommonFun::GetLocalIP(),VIRTUALNAME,SCAN_DIR);
		szSql.Format(_T(" DELETE FROM t_s_Systemset WHERE SetName = 'Ftp' ")
			_T(" INSERT INTO t_s_Systemset (SetName,SetValue1,SetValue2) VALUES('ftp','%s','%s') "),FTP_USERNAME,FTP_PASSWORD);

		if(!ado.ExecuteSQL(szSql))
		{
			theLog.Write(_T("初始化scandir失败！"));
		}
		theLog.Write(szSql);


		//通知服务器配置变更
		CCommonFun::NotifyFromWeb(1);
	}

	CoUninitialize();
}