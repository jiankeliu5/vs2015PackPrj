// CreateDbDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Install.h"
#include "CreateDbDlg.h"


// CCreateDbDlg 对话框

IMPLEMENT_DYNAMIC(CCreateDbDlg, CDialog)

CCreateDbDlg::CCreateDbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateDbDlg::IDD, pParent)
	, m_sDbUser(_T("sa"))
	, m_sDbPwd(_T(""))
	, m_sDbPort(_T("1433"))
	, m_sDbAddr(_T("localhost"))
{
	m_bInstalledSuc = TRUE;
}

CCreateDbDlg::~CCreateDbDlg()
{
}

void CCreateDbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DBNAME, m_sDbUser);
	DDX_Text(pDX, IDC_EDIT_DBPWD, m_sDbPwd);
	DDX_Text(pDX, IDC_EDIT_DBPORT, m_sDbPort);
	DDX_Text(pDX, IDC_EDIT_DBADDR, m_sDbAddr);
	DDX_Control(pDX,IDC_COMBO_DBTYPE,m_wndCmb);
}


BEGIN_MESSAGE_MAP(CCreateDbDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCreateDbDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCreateDbDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1, &CCreateDbDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_TEST, &CCreateDbDlg::OnBnClickedTest)
	ON_CBN_SELCHANGE(IDC_COMBO_DBTYPE, &CCreateDbDlg::OnSelchangeComboDbType)
END_MESSAGE_MAP()


// CCreateDbDlg 消息处理程序
void CCreateDbDlg::OnSelchangeComboDbType()
{
	int nSel = m_wndCmb.GetCurSel();
	int nData = m_wndCmb.GetItemData(nSel);
	if (nData == 0)
	{
		m_sDbAddr = CCommonFun::GetLocalIP();
		m_sDbPort;
		CDbInstaller::GetMsdeListenPort(_T("iSecStar"),m_sDbPort);
		m_sDbUser = _T("sa");
		m_sDbPwd = _T("sa");
		GetDlgItem(IDC_EDIT_DBNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DBPWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DBPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DBADDR)->EnableWindow(FALSE);
	}
	else
	{
		m_sDbPort = _T("1433");
		m_sDbUser = _T("sa");
		GetDlgItem(IDC_EDIT_DBNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_DBPWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_DBPORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_DBADDR)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

void CCreateDbDlg::OnBnClickedOk()
{
	bool bSuccess = false;

	UpdateData(true);

	m_sDbUser.TrimLeft();
	m_sDbUser.TrimRight();

	if(m_sDbUser.CompareNoCase(_T("")) == 0) 
	{
		MessageBox(_T("数据库用户名不能为空!"),_T("提示"),MB_ICONEXCLAMATION);
		return;
	}
	m_sDbPwd.TrimLeft();
	m_sDbPwd.TrimRight();

	BOOL bLocal = FALSE;
#if 0
	if (IsDlgButtonChecked(IDC_CHECK1))
	{
		
		BYTE nField0=192,nField1=168,nField2=2,nField3=1;
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_DBIP))->GetAddress(nField0,nField1,nField2,nField3);
		m_sDbIP.Format(_T("%d.%d.%d.%d"),(int)nField0,(int)nField1,(int)nField2,(int)nField3); 
	}
	else
	{
		 m_sDbIP = _T("localhost");//CCommonFun::GetLocalIP();
		 bLocal = TRUE;
	}
#endif
	if (m_sDbAddr.CompareNoCase(_T("localhost"))==0)
	{
		bLocal = TRUE;
	}
	else
	{
		bLocal = CCommonFun::IsLocalIP(m_sDbAddr);
	}
	if (bLocal)
	{
		m_sDbAddr = _T("localhost");
	}
	if (m_sDbAddr.IsEmpty() || m_sDbAddr.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		MessageBox(_T("请输入数据库服务器地址！"),_T("提示"),MB_ICONEXCLAMATION);
		return ;
	}
	HCURSOR waitcursor;
	HCURSOR oldcursor;
	CString szPath = g_sDefaultPath;
	szPath += _T("\\res\\wait.ani");
	waitcursor=LoadCursorFromFile(szPath);
	oldcursor=SetCursor(waitcursor);
	//安装平台数据库
	CString sPath; 
	OLDDB_INSTALL_TYPE eType = OIT_QUERY;
	CDbInstaller db;
	int nSucCount = 0;
	do
	{
		if (bLocal)
		{
			CString sDbName;
			for (int n = 0; n < m_aryDb.GetCount() ; n++)
			{
				sDbName = m_aryDb.GetAt(n);
				sPath.Format(_T("%s\\data\\%s.bak"),g_sDefaultPath,sDbName);
				if(!db.InstallDbByDbbackupFile(m_sDbAddr,sDbName,m_sDbUser,m_sDbPwd,m_sDbPort,sPath,eType))
				{
					MessageBox(db.m_sErr,_T("提示"),MB_ICONEXCLAMATION);
					break;
				}
				nSucCount ++;
			}
			
		}
		else
		{
			CString sDbName;
			for (int n = 0; n < m_aryDb.GetCount() ; n++)
			{
				sDbName = m_aryDb.GetAt(n);
				sPath.Format(_T("%s\\data\\%s.sql"),g_sDefaultPath,sDbName);
				if(!db.InstallDbBySqlFile(m_sDbAddr,sDbName,m_sDbUser,m_sDbPwd,m_sDbPort,sPath,eType))
				{
					MessageBox(db.m_sErr,_T("提示"),MB_ICONEXCLAMATION);
					break;
				}
				nSucCount ++;
			}
		}
		
	} while (0);
	
	SetCursor(oldcursor);
	m_bInstalledSuc = (nSucCount == m_aryDb.GetCount()) ? TRUE :FALSE;
	if (!m_bInstalledSuc)
	{
		return ;
	}
	theLog.Write(_T("安装成功"));
	int nSel = m_wndCmb.GetCurSel();
	int nData = m_wndCmb.GetItemData(nSel);

	theConfig.SaveConnStr(m_sDbAddr,m_aryDb.GetAt(0),m_sDbUser,m_sDbPwd,m_sDbPort,nData);
	
	OnOK();
}

void CCreateDbDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

BOOL CCreateDbDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString sPort;
	if(CDbInstaller::GetMsdeListenPort(_T("iSecStar"),sPort))
	{
		int n = m_wndCmb.AddString(_T("桌面数据库（msde）"));
		m_wndCmb.SetItemData(n,0);
	}
	int n = m_wndCmb.AddString(_T("Microsoft SQL Server 2000及以后"));
	m_wndCmb.SetItemData(n,1);
	m_wndCmb.SetCurSel(0);
	OnSelchangeComboDbType();

// 	this->CheckDlgButton(IDC_CHECK1,BST_UNCHECKED);
// 
// 	GetDlgItem(IDC_IPADDRESS_DBIP)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCreateDbDlg::OnBnClickedCheck1()
{
// 	if (IsDlgButtonChecked(IDC_CHECK1))
// 	{
// 		GetDlgItem(IDC_IPADDRESS_DBIP)->EnableWindow();
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_IPADDRESS_DBIP)->EnableWindow(FALSE);
// 	}
}

void CCreateDbDlg::OnBnClickedTest()
{
	UpdateData(true);

	m_sDbUser.TrimLeft();
	m_sDbUser.TrimRight();

	if(m_sDbUser.CompareNoCase(_T("")) == 0) 
	{
		MessageBox(_T("数据库用户名不能为空!"),_T("提示"),MB_ICONEXCLAMATION);
		return;
	}
	m_sDbPwd.TrimLeft();
	m_sDbPwd.TrimRight();
	BOOL bLocal = FALSE;
#if 0
	CString m_sDbIP;
	if (IsDlgButtonChecked(IDC_CHECK1))
	{

		BYTE nField0=192,nField1=168,nField2=2,nField3=1;
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_DBIP))->GetAddress(nField0,nField1,nField2,nField3);
		m_sDbIP.Format(_T("%d.%d.%d.%d"),(int)nField0,(int)nField1,(int)nField2,(int)nField3); 
	}
	else
	{
		m_sDbIP = CCommonFun::GetLocalIP();
		bLocal = TRUE;
	}
#endif
	bLocal = CCommonFun::IsLocalIP(m_sDbAddr);
	if (bLocal)
	{
		m_sDbAddr = _T("localhost");
	}
	if (m_sDbAddr.IsEmpty() || m_sDbAddr.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		MessageBox(_T("请输入数据库服务器地址！"),_T("提示"),MB_ICONEXCLAMATION);
		return ;
	}
	CoInitialize(0);
	CAdo ado;
	BOOL bRet = FALSE;
	do
	{
		CString sDbName;
		for (int n = 0; n < m_aryDb.GetCount() ; n++)
		{
			sDbName = m_aryDb.GetAt(n);
			ado.SetConnStr(m_sDbAddr,sDbName,m_sDbUser,m_sDbPwd,m_sDbPort);
			bRet = ado.Connect();
			if (!bRet)
			{
				break;
			}
		}

	}while(0);
	if (!bRet)
	{
		MessageBox(_T("连接失败！"),_T("提示"),MB_ICONEXCLAMATION);
	}
	else
	{
		MessageBox(_T("连接成功！"),_T("提示"),MB_ICONEXCLAMATION);
	}
	CoUninitialize();
}

BOOL CCreateDbDlg::CreateDbOnLocalSqlExpress(CString sDbName)
{
	CString szServer = _T("localhost");
	CString szDbName = sDbName;
	CString szUser = _T("sa");
	CString szInstanceName = g_oIniFileInstall.GetVal(_T("DataBase"), _T("InstanceName"), _T(""));
	CString szSaPassword = g_oIniFileInstall.GetVal(_T("DataBase"), _T("SaPassword"), _T(""));
	//数据库的安装类型
	OLDDB_INSTALL_TYPE oDbInstallType = (OLDDB_INSTALL_TYPE)g_oIniFileInstall.GetVal(_T("DataBase"),_T("InstallType"),OIT_QUERY);
	theLog.Write(_T("CreateDbOnLocalSqlExpress,oDbInstallType=%d"), oDbInstallType);

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
	cry.UnCryptPWD(szSaPassword.GetBuffer(),cPwd,nSize);
	szSaPassword.ReleaseBuffer();
	szSaPassword = cPwd;
#endif // UNICODE
#endif

	CDbInstaller db;
	CString szDataBakPath;
	szDataBakPath.Format(_T("%s\\data\\%s.bak"),g_sDefaultPath,szDbName);
	if(!db.InstallDbByDbbackupFile2(szServer,szDbName,szUser,szSaPassword,szInstanceName,szDataBakPath,oDbInstallType))
	{
		m_bInstalledSuc = FALSE;
		theLog.Write(_T("!!InstallDbByDbbackupFile2 fail,szDataBakPath=%s,szInstanceName=%s"), szDataBakPath, szInstanceName);
	}
	else
	{
		m_bInstalledSuc = TRUE;

		theConfig.SaveConnStr2(szServer,szDbName,szUser,szSaPassword,szInstanceName);

		CString szMsg;
		szMsg.Format(_T("Data Source=%s\\%s;Initial Catalog=%s;User ID=%s;Password=%s;"),
			szServer,szInstanceName,szDbName,szUser,szSaPassword);

		CCommonFun::SetDoNetDBStr(szMsg);
	}

	return m_bInstalledSuc;
}

BOOL CCreateDbDlg::CreateDbByAdoInfo(CString sDbName, ADO_INFO& info)
{
	m_bInstalledSuc = TRUE;

	//数据库的安装类型
	OLDDB_INSTALL_TYPE oDbInstallType = (OLDDB_INSTALL_TYPE)g_oIniFileInstall.GetVal(_T("DataBase"),_T("InstallType"),OIT_QUERY);
	theLog.Write(_T("CreateDbByAdoInfo,oDbInstallType=%d"), oDbInstallType);

	CDbInstaller db;
	CString szDataBakPath;
	szDataBakPath.Format(_T("%s\\data\\%s.bak"),g_sDefaultPath,sDbName);
	if(!db.InstallDbByDbbackupFile(info.szIP,sDbName,info.szUser,info.szPwd,info.szPort,szDataBakPath,oDbInstallType))
	{
		m_bInstalledSuc = FALSE;
		theLog.Write(_T("!!InstallDbByDbbackupFile fail,szDataBakPath=%s"), szDataBakPath);
	}
// 	else
// 	{
		theConfig.SaveConnStr(info.szIP,sDbName,info.szUser,info.szPwd,info.szPort,info.nDbType);

		CString szMsg;
		szMsg.Format(_T("Data Source=%s,%s;Initial Catalog=%s;User ID=%s;Password=%s;"),
			info.szIP,info.szPort,sDbName,info.szUser,info.szPwd);

		CCommonFun::SetDoNetDBStr(szMsg);
// 	}

	return m_bInstalledSuc;
}
