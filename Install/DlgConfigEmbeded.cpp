// DlgConfigEmbeded.cpp : 实现文件
//

#include "stdafx.h"
#include "install.h"
#include "DlgConfigEmbeded.h"
#include "afxdialogex.h"
#include "WebServiceHelper.h"


// CDlgConfigEmbeded 对话框

IMPLEMENT_DYNAMIC(CDlgConfigEmbeded, CDialogEx)

CDlgConfigEmbeded::CDlgConfigEmbeded(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG4, pParent)
	, m_nPort(5020)
	, m_nWebSvrPort(80)
{

}

CDlgConfigEmbeded::~CDlgConfigEmbeded()
{
}

void CDlgConfigEmbeded::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ENBEDED_CORESVRPORT, m_nPort);
	DDV_MinMaxInt(pDX, m_nPort, 1, 65535);
	DDX_Text(pDX, IDC_ENBEDED_WEBSVRPORT, m_nWebSvrPort);
	DDV_MinMaxInt(pDX, m_nWebSvrPort, 1, 65535);
}


BEGIN_MESSAGE_MAP(CDlgConfigEmbeded, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_TESTCNNT, &CDlgConfigEmbeded::OnBnClickedBtnTestcnnt)
	ON_BN_CLICKED(IDOK, &CDlgConfigEmbeded::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_TESTCNNT2, &CDlgConfigEmbeded::OnBnClickedBtnTestcnnt2)
END_MESSAGE_MAP()


// CDlgConfigEmbeded 消息处理程序


BOOL CDlgConfigEmbeded::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CIniFile oIniFile;
	CString szServIP = _T("");
	oIniFile.m_sPath.Format(_T("%s%s"), CCommonFun::GetDefaultPath(), SYS_CONFIG_FILE);
	szServIP = oIniFile.GetVal(_T("CoreSvr"), _T("SvrHost"), _T("127.0.0.1"));
	m_nPort = oIniFile.GetVal(_T("CoreSvr"), _T("SvrPort"), 5020);
	m_nWebSvrPort = oIniFile.GetVal(_T("WebSvr"), _T("ServicePort"), 80);

	BYTE nField0 = 199, nField1 = 99, nField2 = 99, nField3 = 1;
	int Ipoint0, Ipoint1, Ipoint2;
	Ipoint0 = szServIP.Find('.', 0);
	nField0 = _tstoi(szServIP.Mid(0, Ipoint0));
	Ipoint1 = szServIP.Find('.', Ipoint0 + 1);
	nField1 = _tstoi(szServIP.Mid(Ipoint0 + 1, Ipoint1));
	Ipoint2 = szServIP.Find('.', Ipoint1 + 1);
	nField2 = _tstoi(szServIP.Mid(Ipoint1 + 1, Ipoint2));
	nField3 = _tstoi(szServIP.Mid(Ipoint2 + 1, szServIP.GetLength()));
	((CIPAddressCtrl*)GetDlgItem(IDC_ENBEDED_CORESVRIP))->SetAddress(nField0, nField1, nField2, nField3);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgConfigEmbeded::OnBnClickedBtnTestcnnt()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString szServIP;
	BYTE nField0 = 199, nField1 = 99, nField2 = 99, nField3 = 1;
	((CIPAddressCtrl*)GetDlgItem(IDC_ENBEDED_CORESVRIP))->GetAddress(nField0, nField1, nField2, nField3);
	szServIP.Format(_T("%d.%d.%d.%d"), (int)nField0, (int)nField1, (int)nField2, (int)nField3);
	if (szServIP.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		MessageBox(_T("IP非法！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	CSocket sock;
	if (sock.Create())
	{
		if (!sock.Connect(szServIP, m_nPort))
		{
			MessageBox(_T("连接失败！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("连接成功！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		}

		sock.Close();
	}
	else
	{
		DWORD dw = ::WSAGetLastError();
		MessageBox(_T("创建连接失败！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
}


void CDlgConfigEmbeded::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString szServerIP;
	BYTE nField0 = 199, nField1 = 99, nField2 = 99, nField3 = 1;
	((CIPAddressCtrl*)GetDlgItem(IDC_ENBEDED_CORESVRIP))->GetAddress(nField0, nField1, nField2, nField3);
	szServerIP.Format(_T("%d.%d.%d.%d"), (int)nField0, (int)nField1, (int)nField2, (int)nField3);
	if (szServerIP.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		MessageBox(_T("IP非法！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CString szIniFilePath;
	szIniFilePath.Format(_T("%s\\EmbedPrintWeb\\%s"), g_sInstallPath, SYS_CONFIG_FILE);
	if (!PathFileExists(szIniFilePath))
	{
		CString szDir = CCommonFun::GetFileDirByFullPath(szIniFilePath);
		if (!PathFileExists(szDir))
		{
			CreateDirectory(CCommonFun::GetFileDirByFullPath(szIniFilePath), NULL);
		}
		HANDLE hFile = ::CreateFile(szIniFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(hFile);
		}
	}

	CIniFile oIniFile;
	oIniFile.m_sPath = szIniFilePath;
	oIniFile.SetVal(_T("CoreSvr"), _T("SvrHost"), szServerIP);
	oIniFile.SetVal(_T("CoreSvr"), _T("SvrPort"), m_nPort);
	oIniFile.SetVal(_T("WebSvr"), _T("ServicePort"), m_nWebSvrPort);

	//修改柯美内嵌的连接地址
	CXmlParse xml;
	CString szPath = CCommonFun::GetDefaultPath();
	szPath.Format(_T("%s\\EmbedPrintWeb\\KeMeiPrint\\ops.exe.config"), g_sInstallPath);
	if (PathFileExists(szPath))
	{
		if (xml.ParseXml(szPath))
		{
			MSXML2::IXMLDOMDocument2Ptr doc = xml.GetXmlDocment();
			MSXML2::IXMLDOMNodePtr configuration = xml.SelectSingleNode(doc, _T("configuration"));
			MSXML2::IXMLDOMNodePtr appSettings = xml.SelectSingleNode(configuration, _T("appSettings"));
			MSXML2::IXMLDOMNodeListPtr addsList = xml.SelectNodes(appSettings, _T("add"));
			for (int i = 0; i < addsList->Getlength(); i++)
			{
				MSXML2::IXMLDOMNodePtr add = addsList->Getitem(i);
				CString szKey, szIP;
				xml.GetAttribute(add, _T("key"), szKey);
				xml.GetAttribute(add, _T("value"), szIP);
				if (szKey.CompareNoCase(_T("SerHost")) == 0)
				{
					xml.SetAttribute(add, _T("value"), szServerIP);
					BOOL ret = xml.Save(doc, szPath);
					continue;
				}
				if (szKey.CompareNoCase(_T("SerPort")) == 0)
				{
					CString szPort;
					szPort.Format(_T("%d"), m_nPort);
					xml.SetAttribute(add, _T("value"), szPort);
					xml.Save(doc, szPath);
					continue;
				}
			}
		}
		CXmlParse xml2;
		if (xml2.ParseXml(szPath))
		{
			MSXML2::IXMLDOMDocument2Ptr doc = xml2.GetXmlDocment();
			MSXML2::IXMLDOMNodePtr configuration = xml2.SelectSingleNode(doc, _T("configuration"));
			MSXML2::IXMLDOMNodePtr appSettings = xml2.SelectSingleNode(configuration, _T("appSettings"));
			MSXML2::IXMLDOMNodeListPtr addsList = xml2.SelectNodes(appSettings, _T("add"));
			for (int i = 0; i < addsList->Getlength(); i++)
			{
				MSXML2::IXMLDOMNodePtr add = addsList->Getitem(i);
				CString szKey, szIP;
				xml2.GetAttribute(add, _T("key"), szKey);
				xml2.GetAttribute(add, _T("value"), szIP);
			}
		}
	}
	else
	{
		theLog.Write(_T("!!CLmMFPMgr::SetEmbeddedServer,inifile not exist,szPath=%s"), szPath);
	}


	CWebServiceHelper oWebService;
	CString szWebserviceKey = oIniFile.GetVal(_T("WebSvr"), _T("WebserviceKey"), _T("xab!@#$%^&&*()?"));
	CString szServiceUrl = oIniFile.GetVal(_T("WebSvr"), _T("ServiceUrl"), _T(""));


	if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() <= 0))
	{
		szServiceUrl.Format(_T("http://localhost/xabprint/services"));
	}
	else if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() > 0))
	{
		szServiceUrl.Format(_T("http://%s:%d/xabprint/services"), szServerIP, m_nWebSvrPort);
	}

	int nServiceTimeout = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("ServiceTimeout"), 15000);
	BOOL bIEProxy = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxy"), FALSE);
	CString szIEProxyAddr = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxyAddr"), _T(""));
	int nIEProxyPort = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxyPort"), 808);

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
			szServiceUrl.Format(_T("https://%s:%d/xabprint/services"), szServerIP, m_nWebSvrPort);
		}
		oWebService.ConfigWebService(szWebserviceKey, szServiceUrl, nServiceTimeout, bIEProxy, szIEProxyAddr, nIEProxyPort);
		if (!oWebService.ConnWebService())
		{
			theLog.Write(_T("!!CLmMFPMgr::SetEmbeddedServer,1.2,ConnWebService fail"));
		}
		else
		{
			oWebService.KMNotifyUrl(CCommonFun::GetLocalIP());
		}
	}
	else
	{
		oWebService.KMNotifyUrl(CCommonFun::GetLocalIP());
	}
	CDialogEx::OnOK();
}

void CDlgConfigEmbeded::OnBnClickedBtnTestcnnt2()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	CWebServiceHelper oWebService;
	CIniFile oIniFile;
	oIniFile.m_sPath.Format(_T("%s%s"), CCommonFun::GetDefaultPath(), SYS_CONFIG_FILE);
	CString szWebserviceKey = oIniFile.GetVal(_T("WebSvr"), _T("WebserviceKey"), _T("xab!@#$%^&&*()?"));
	CString szServiceUrl = oIniFile.GetVal(_T("WebSvr"), _T("ServiceUrl"), _T(""));


	CString szServerIP;
	BYTE nField0 = 199, nField1 = 99, nField2 = 99, nField3 = 1;
	((CIPAddressCtrl*)GetDlgItem(IDC_ENBEDED_CORESVRIP))->GetAddress(nField0, nField1, nField2, nField3);
	szServerIP.Format(_T("%d.%d.%d.%d"), (int)nField0, (int)nField1, (int)nField2, (int)nField3);

	if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() <= 0))
	{
		szServiceUrl.Format(_T("http://localhost/xabprint/services"));
	}
	else if ((szServiceUrl.GetLength() <= 0) && (szServerIP.GetLength() > 0))
	{
		szServiceUrl.Format(_T("http://%s:%d/xabprint/services"), szServerIP, m_nWebSvrPort);
	}

	int nServiceTimeout = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("ServiceTimeout"), 15000);
	BOOL bIEProxy = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxy"), FALSE);
	CString szIEProxyAddr = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxyAddr"), _T(""));
	int nIEProxyPort = oIniFile.GetVal(_T("PrtCtrlConfig"), _T("IEProxyPort"), 808);

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
			szServiceUrl.Format(_T("https://%s:%d/xabprint/services"), szServerIP, m_nWebSvrPort);
		}
		oWebService.ConfigWebService(szWebserviceKey, szServiceUrl, nServiceTimeout, bIEProxy, szIEProxyAddr, nIEProxyPort);
		if (!oWebService.ConnWebService())
		{
			theLog.Write(_T("!!CLmMFPMgr::SetEmbeddedServer,1.2,ConnWebService fail"));
		}
		MessageBox(_T("连接失败"));
	}
	else
	{
		MessageBox(_T("连接成功"));
	}
}
