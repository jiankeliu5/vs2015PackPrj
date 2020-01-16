// SetDbDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Install.h"
#include "SetDbDlg.h"


// CSetDbDlg �Ի���

IMPLEMENT_DYNAMIC(CSetDbDlg, CDialog)

CSetDbDlg::CSetDbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDbDlg::IDD, pParent)
{
	m_szDbName = PRINT_DB_NAME;
	m_szIP = CCommonFun::GetLocalIP();
	m_szPort = _T("1433");
	m_szUserName = _T("sa");
	m_szUserPwd = _T("");
}

CSetDbDlg::~CSetDbDlg()
{
}

void CSetDbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDbDlg)
	DDX_Text(pDX, IDC_EDIT_DBNAME, m_szDbName);
	DDX_Text(pDX, IDC_EDIT_IP, m_szIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_szPort);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUserName);
	DDX_Text(pDX, IDC_EDIT_USERPWD, m_szUserPwd);
	DDX_Control(pDX,IDC_COMBO_DBTYPE,m_wndCmb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDbDlg, CDialog)
	ON_BN_CLICKED(IDC_TEST, &CSetDbDlg::OnBnClickedTest)
	ON_BN_CLICKED(IDOK, &CSetDbDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_DBTYPE, &CSetDbDlg::OnSelchangeComboDbType)
END_MESSAGE_MAP()

void CSetDbDlg::OnSelchangeComboDbType()
{
	int nSel = m_wndCmb.GetCurSel();
	int nData = m_wndCmb.GetItemData(nSel);
	if (nData == 0)
	{
		m_szIP = CCommonFun::GetLocalIP();
		m_szPort;
		CDbInstaller::GetMsdeListenPort(_T("iSecStar"),m_szPort);
		m_szUserName = _T("sa");
		m_szUserPwd = _T("sa");
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USERPWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_IP)->EnableWindow(FALSE);
	}
	else
	{
		m_szPort = _T("1433");
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_USERPWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_IP)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

// CSetDbDlg ��Ϣ�������
BOOL CSetDbDlg::TestConnect()
{
	if(!UpdateData(TRUE))
		return FALSE;

	BOOL bRet = TRUE;

	CoInitialize(0);
	CAdo ado;


	ado.SetConnStr(m_szIP,m_szDbName,m_szUserName,m_szUserPwd,m_szPort);
	bRet = ado.Connect();


	CoUninitialize();

	return bRet;
}

void CSetDbDlg::OnBnClickedTest()
{
	if(!UpdateData(TRUE))
		return ;
	if(m_szIP == _T(""))
	{
		MessageBox(_T("���ݿ�IP��ַ����Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if(m_szUserName == _T(""))
	{
		MessageBox(_T("���ݿ��û�������Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if(m_szPort == _T(""))
	{
		MessageBox(_T("���Ӷ˿ڲ���Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if(TestConnect())
		MessageBox(_T("�������ݿ�ɹ���"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(_T("�������ݿ�ʧ�ܣ�"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
}


void CSetDbDlg::OnBnClickedOk()
{
	if(!UpdateData(TRUE))
		return ;

	if(m_szIP == _T(""))
	{
		MessageBox(_T("���ݿ�IP��ַ����Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}
	if(m_szUserName == _T(""))
	{
		MessageBox(_T("���ݿ��û�������Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if(m_szPort == _T(""))
	{
		MessageBox(_T("���Ӷ˿ڲ���Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return ;
	}

	if(!TestConnect())
	{
		int nRet = MessageBox(_T("�������ݿ�ʧ�ܣ��Ƿ�Ҫ������������ݿ�������Ϣ��"),_T("��ʾ"),MB_YESNO | MB_ICONINFORMATION);
		if (nRet == IDNO)
		{
			return ;
		}
	}
	int nSel = m_wndCmb.GetCurSel();
	int nData = m_wndCmb.GetItemData(nSel);
	theConfig.SaveConnStr(m_szIP,m_szDbName,m_szUserName,m_szUserPwd,m_szPort,nData);

	CString szMsg;
	szMsg.Format(_T("Data Source=%s,%s;Initial Catalog=%s;User ID=%s;Password=%s"),
		m_szIP,m_szPort,m_szDbName,m_szUserName,m_szUserPwd);

	CCommonFun::SetDoNetDBStr(szMsg);

	OnOK();
}

BOOL CSetDbDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	theConfig.LoadConnStr(m_szIP,m_szDbName,m_szUserName,m_szUserPwd,m_szPort,m_nDbType);
	if(m_szDbName.IsEmpty())
	{
		m_szDbName = PRINT_DB_NAME;
	}
	CString sPort;
	int nSel = 0;
	if(CDbInstaller::GetMsdeListenPort(_T("iSecStar"),sPort))
	{
		int n = m_wndCmb.AddString(_T("�������ݿ⣨msde��"));
		m_wndCmb.SetItemData(n,0);
		if (m_nDbType == 0)
		{
			nSel = n;
		}
	}
	int n = m_wndCmb.AddString(_T("Microsoft SQL Server 2000���Ժ�"));
	m_wndCmb.SetItemData(n,1);
	if (m_nDbType == 1)
	{
		nSel = n;
	}
	

	m_wndCmb.SetCurSel(nSel);
	OnSelchangeComboDbType();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
