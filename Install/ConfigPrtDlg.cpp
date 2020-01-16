// ConfigPrtDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Install.h"
#include "ConfigPrtDlg.h"
#include "YTJAdvDlg.h"
#include "SetHpDlg.h"


// CConfigPrtDlg 对话框

IMPLEMENT_DYNAMIC(CConfigPrtDlg, CDialog)

CConfigPrtDlg::CConfigPrtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigPrtDlg::IDD, pParent)
{
	m_nEmbedConnectionType = 0;
	m_nBrandType = 0;
}

CConfigPrtDlg::~CConfigPrtDlg()
{
}

void CConfigPrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IP, m_wndCmbIP);
	DDX_Control(pDX, IDC_COMBO_BRAND, m_cmbBrand);
	DDX_Text(pDX, IDC_LISTEN_PORT, m_nListenPort);
	DDX_Control(pDX, IDC_BTN_YTJ_ADV, m_YTJAdv);
	DDX_Text(pDX, IDC_EDIT_CopyBak, m_szCopyDir);
}


BEGIN_MESSAGE_MAP(CConfigPrtDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigPrtDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_EMBED_TYPE_LISTEN, &CConfigPrtDlg::OnBnClickedRadioEmbedTypeListen)
	ON_BN_CLICKED(IDC_RADIO_EMBED_TYPE_CONNECT, &CConfigPrtDlg::OnBnClickedRadioEmbedTypeConnect)
	ON_BN_CLICKED(IDC_RADIO_EMBED_TYPE_DB, &CConfigPrtDlg::OnBnClickedRadioEmbedTypeDb)
	ON_BN_CLICKED(IDC_RADIO_EMBED_TYPE_NO, &CConfigPrtDlg::OnBnClickedRadioEmbedTypeNo)
	ON_BN_CLICKED(IDC_BTN_YTJ_ADV, &CConfigPrtDlg::OnBnClickedBtnYtjAdv)
	ON_CBN_SELCHANGE(IDC_COMBO_BRAND, &CConfigPrtDlg::OnCbnSelchangeComboBrand)
	ON_BN_CLICKED(IDC_BUTTON_CopyBak, &CConfigPrtDlg::OnBnClickedButtonCopyBak)
	ON_BN_CLICKED(IDC_CHECK_CopyBak, &CConfigPrtDlg::OnBnClickedCheckCopybak)
END_MESSAGE_MAP()


// CConfigPrtDlg 消息处理程序


void CConfigPrtDlg::Load()
{
	CString szServIP = _T("");
	CString sPort;
	theConfig.LoadPrtStationConfig(szServIP,sPort);
	if (szServIP.IsEmpty())
	{
		szServIP = CCommonFun::GetLocalIP();
	}
	BYTE nField0=199,nField1=99,nField2=99,nField3=1;
	int Ipoint0,Ipoint1,Ipoint2;
	Ipoint0=szServIP.Find('.',0);
	nField0=_tstoi(szServIP.Mid(0,Ipoint0));
	Ipoint1=szServIP.Find('.',Ipoint0+1);
	nField1=_tstoi(szServIP.Mid(Ipoint0+1,Ipoint1));
	Ipoint2=szServIP.Find('.',Ipoint1+1);
	nField2=_tstoi(szServIP.Mid(Ipoint1+1,Ipoint2));
	nField3=_tstoi(szServIP.Mid(Ipoint2+1,szServIP.GetLength()));
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP))->SetAddress(nField0,nField1,nField2,nField3);

	theConfig.LoadPrtCopyBak(m_bEnableCopyBak);
	theConfig.LoadCopyDir(m_szCopyDir);
	theConfig.LoadBindIP(m_sBindIp);
	theConfig.LoadBrand(m_nBrandType);
	theConfig.LoadIsEmbed(m_bEnableEmbedWeb);
	theConfig.LoadSNMPPrintMon(m_bEnableSnmpPrintMonitor);
	theConfig.LoadEmbedConnectionType(m_nEmbedConnectionType);
	theConfig.LoadEmbedListenPort(m_nListenPort);
    
	//del by szy 20161216
	//CheckDlgButton(IDC_RADIO_Enable_Embed, m_bEnableEmbedWeb);
	//CheckDlgButton(IDC_RADIO_Disable_Embed, !m_bEnableEmbedWeb);


	//#define IDC_RADIO_EMBED_TYPE_NO         1104
	//#define IDC_RADIO_EMBED_TYPE_LISTEN     1105
	//#define IDC_RADIO_EMBED_TYPE_CONNECT    1106
	//#define IDC_RADIO_EMBED_TYPE_DB         1107
	int resid = IDC_RADIO_EMBED_TYPE_NO+m_nEmbedConnectionType;
	theLog.Write(_T("##CConfigPrtDlg::Load,m_nEmbedConnectionType=%d"), m_nEmbedConnectionType);
	theLog.Write(_T("##CConfigPrtDlg::Load,resid=%d"), resid);
	theLog.Write(_T("##CConfigPrtDlg::Load,m_nListenPort=%d"), m_nListenPort);
	CheckDlgButton(IDC_RADIO_EMBED_TYPE_NO, resid==IDC_RADIO_EMBED_TYPE_NO);
	CheckDlgButton(IDC_RADIO_EMBED_TYPE_LISTEN, resid==IDC_RADIO_EMBED_TYPE_LISTEN);
	CheckDlgButton(IDC_RADIO_EMBED_TYPE_CONNECT, resid==IDC_RADIO_EMBED_TYPE_CONNECT);
	CheckDlgButton(IDC_RADIO_EMBED_TYPE_DB, resid==IDC_RADIO_EMBED_TYPE_DB);
	GetDlgItem(IDC_STATIC_LISTENPORT)->ShowWindow(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_LISTEN));
	GetDlgItem(IDC_LISTEN_PORT)->ShowWindow(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_LISTEN));


	CheckDlgButton(IDC_RADIO_Enable_SNMP_Print_Mon, m_bEnableSnmpPrintMonitor);
	CheckDlgButton(IDC_RADIO_Disable_SNMP_Print_Mon, !m_bEnableSnmpPrintMonitor);

	int nBrandCount = m_cmbBrand.GetCount();
	theLog.Write(_T("##CConfigPrtDlg::Load,9,m_nBrandType=%d,nBrandCount=%d"), m_nBrandType, nBrandCount);
	if(m_nBrandType < 0 || m_nBrandType > BT_MAX)
	{
		m_nBrandType = BT_THIN_CLT;	//默认未知
	}
	for (int i=0; i<nBrandCount; i++)
	{
		if (m_cmbBrand.GetItemData(i) == m_nBrandType)
		{
			m_cmbBrand.SetCurSel(i);
			break;
		}
	}

	if (!m_bEnableCopyBak)
	{
		CheckDlgButton(IDC_CHECK_CopyBak,BST_UNCHECKED);
	}
	else
	{
		CheckDlgButton(IDC_CHECK_CopyBak,BST_CHECKED);
	}
	OnBnClickedCheckCopybak();
	OnCbnSelchangeComboBrand();
	UpdateData(FALSE);
}

BOOL CConfigPrtDlg::Save()
{
	CString szServIP;
	BYTE nField0=199,nField1=99,nField2=99,nField3=1;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP))->GetAddress(nField0,nField1,nField2,nField3);
	szServIP.Format(_T("%d.%d.%d.%d"),(int)nField0,(int)nField1,(int)nField2,(int)nField3);
	if(szServIP.CompareNoCase(_T("0.0.0.0")) == 0 )
		return FALSE;
	m_szServIP = szServIP;
	
	GetDlgItem(IDC_EDIT_CopyBak)->GetWindowText(m_szCopyDir);

	int nSel = m_wndCmbIP.GetCurSel();
	if (nSel >= 0)
	{
		m_wndCmbIP.GetLBText(nSel,m_sBindIp);
	}

	return TRUE;
}

void CConfigPrtDlg::OnBnClickedOk()
{
	UpdateData();
	if (!Save())
	{
		MessageBox(_T("请输入正确的参数！"),_T("提示"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if (m_szCopyDir.IsEmpty() && IsDlgButtonChecked(IDC_CHECK_CopyBak))
	{
		MessageBox(_T("请选择复印留底文件夹！"),_T("提示"),MB_OK | MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_CopyBak)->SetFocus();
		return ;
	}
	theConfig.SavePrtYTJ(FALSE);
	if (IsDlgButtonChecked(IDC_CHECK_CopyBak))
	{
		theConfig.SavePrtCopyBak(TRUE);
	}
	else
	{
		theConfig.SavePrtCopyBak(FALSE);
	}
	CString sScanData(_T(".jpg,.jpeg,.tif,.tiff,.pdf"));
	theConfig.SaveScanData(sScanData);
	theConfig.SaveCopyDir(m_szCopyDir);
	theConfig.SavePrtComIndex(0);	
	theConfig.SavePrtBindIP(m_sBindIp);
	theConfig.SavePrtStationConfig(m_szServIP);
	CString sPaperConfig(_T(""));
	theConfig.SetPrtPaperConfig(sPaperConfig);
	m_nBrandType = m_cmbBrand.GetItemData(m_cmbBrand.GetCurSel());
	theConfig.SaveBrand(m_nBrandType);
	theConfig.SaveSignalCtrlSet(FALSE);

	if(IsDlgButtonChecked(IDC_RADIO_Enable_SNMP_Print_Mon))
	{
		theConfig.SaveSNMPPrintMon(TRUE);
	}

	if(IsDlgButtonChecked(IDC_RADIO_Disable_SNMP_Print_Mon))
	{
		theConfig.SaveSNMPPrintMon(FALSE);
	}

	theConfig.SaveSNMPPrinterMeter(TRUE);

	if(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_NO))
	{
		theConfig.SaveEmbedConnectionType(0);
	}
	else if(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_LISTEN))
	{
		theConfig.SaveEmbedConnectionType(1);
		theConfig.SaveEmbedListenPort(m_nListenPort);

	}
	else if(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_CONNECT))
	{
		theConfig.SaveEmbedConnectionType(2);
	}
	else if(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_DB))
	{
		theConfig.SaveEmbedConnectionType(3);
	}

	OnOK();
}

BOOL CConfigPrtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	InitBrandUI();
	Load();

	CStringArray ary;
	ary.RemoveAll();
	CCommonFun::GetLocalIPList(ary);
	int nSel = 0;
	if (ary.GetCount() > 0)
	{
		for (int n = 0; n < ary.GetCount(); n++)
		{
			if(m_sBindIp.CompareNoCase(ary.GetAt(n)) ==0)
			{
				nSel = n;
			}
			m_wndCmbIP.AddString(ary.GetAt(n));
		}
		m_wndCmbIP.SetCurSel(nSel);
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CConfigPrtDlg::InitBrandUI()
{
#if Enable_OEM_ONE_PRINTER_BRAND
	#if (OEM_PRINTER_BRAND == DEF_BT_HP)
		m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("惠普")), BT_HP);
	#elif (OEM_PRINTER_BRAND == DEF_BT_KM)
		m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("柯尼卡美能达")), BT_KM);
	#endif
		m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("未知品牌")), BT_THIN_CLT);
#else
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("富士施乐")), BT_SL);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("东芝")), BT_DZ);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("柯尼卡美能达")), BT_KM);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("利盟")), BT_LM);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("未知品牌")), BT_THIN_CLT);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("理光")), BT_LG);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("富士施乐(新版)")), BT_SL2);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("京瓷")), BT_JC);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("震旦")), BT_ZD);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("夏普")), BT_XP);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("三星")), BT_SX);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("惠普")), BT_HP);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("爱普生")), BT_EPSON);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("佳能")), BT_CANON);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("兄弟")), BT_BROTHER);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("京图")), BT_JT);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("OKI")), BT_OKI);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("精工")), BT_SEIKO);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("奔图")), BT_PANTUM);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("映美")), BT_JOLIMARK);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("佳博")), BT_GPRINTER);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("松下")), BT_PANASONIC);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("联想")), BT_LENOVO);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("实达")), BT_START);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("芯烨")), BT_XPRINTER);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("索尼")), BT_SONY);
	m_cmbBrand.SetItemData(m_cmbBrand.AddString(_T("戴尔")), BT_DELL);
#endif

	m_cmbBrand.SetCurSel(0);
}

void CConfigPrtDlg::OnBnClickedRadioEmbedTypeListen()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATIC_LISTENPORT)->ShowWindow(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_LISTEN));
	GetDlgItem(IDC_LISTEN_PORT)->ShowWindow(IsDlgButtonChecked(IDC_RADIO_EMBED_TYPE_LISTEN));
}

void CConfigPrtDlg::OnBnClickedRadioEmbedTypeConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedRadioEmbedTypeListen();
}

void CConfigPrtDlg::OnBnClickedRadioEmbedTypeDb()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedRadioEmbedTypeListen();
}

void CConfigPrtDlg::OnBnClickedRadioEmbedTypeNo()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedRadioEmbedTypeListen();
}

void CConfigPrtDlg::OnBnClickedBtnYtjAdv()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel = m_cmbBrand.GetItemData(m_cmbBrand.GetCurSel());
	if (nSel == BT_SL || nSel == BT_SL2)	//施乐和施乐新版
	{
		CYTJAdvDlg dlg;
		dlg.DoModal();
	}
	else if(nSel == BT_HP)//惠普打印机需要设置是企业版还是专业版
	{
		CSetHpDlg dlg;
		dlg.DoModal();
	}
}

void CConfigPrtDlg::OnCbnSelchangeComboBrand()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel = m_cmbBrand.GetItemData(m_cmbBrand.GetCurSel());
	if (nSel == BT_SL || nSel == BT_SL2)	//施乐和施乐新版
	{
		m_YTJAdv.EnableWindow(TRUE);
	}
	else if(nSel == BT_HP)//惠普打印机需要设置是企业版还是专业版
	{
		m_YTJAdv.EnableWindow(TRUE);
	}
	else
	{
		m_YTJAdv.EnableWindow(FALSE);
	}
}

void CConfigPrtDlg::OnBnClickedButtonCopyBak()
{
	// TODO: 在此添加控件通知处理程序代码
	CoInitialize(NULL);
	BROWSEINFO bi; 
	ITEMIDLIST * pidl; 
	TCHAR szDir[MAX_PATH] = {0};
	bi.hwndOwner=0; 
	bi.pidlRoot=NULL; 
	bi.pszDisplayName=szDir; 
	bi.lpszTitle= _T("选择文件夹"); 
	bi.ulFlags=BIF_RETURNONLYFSDIRS;

	bi.ulFlags |= BIF_NEWDIALOGSTYLE; 
	bi.lpfn=NULL; 
	bi.lParam=0; 
	bi.iImage=0; 

	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL)
		szDir[0] = 0;
	else
	{
		if(!SHGetPathFromIDList(pidl,szDir))
			szDir[0] = 0; 
	}
	// AfxMessageBox(szDir);

	m_szCopyDir = szDir;
	GetDlgItem(IDC_EDIT_CopyBak)->SetWindowText(m_szCopyDir);
	CoUninitialize();
}

void CConfigPrtDlg::OnBnClickedCheckCopybak()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bEnableCopyBak = IsDlgButtonChecked(IDC_CHECK_CopyBak);
	GetDlgItem(IDC_EDIT_CopyBak)->EnableWindow(bEnableCopyBak);
	GetDlgItem(IDC_BUTTON_CopyBak)->EnableWindow(bEnableCopyBak);
}
