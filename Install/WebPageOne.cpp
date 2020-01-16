// WebPageOne.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Install.h"
#include "WebPageOne.h"


// CWebPageOne �Ի���

IMPLEMENT_DYNAMIC(CWebPageOne, CPropertyPage)

CWebPageOne::CWebPageOne()
	: CPropertyPage(CWebPageOne::IDD)
{
	//{{AFX_DATA_INIT(CWebPageOne)
    m_szDbName = PRINT_DB_NAME;
    m_szIP = CCommonFun::GetLocalIP();
    m_szPort = _T("1433");
    m_szUserName = _T("sa");
    m_szUserPwd = _T("");
    //}}AFX_DATA_INIT
    m_nDbType = 1;
}

CWebPageOne::~CWebPageOne()
{
}

void CWebPageOne::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageOne)
	DDX_Text(pDX, IDC_EDIT_DBNAME, m_szDbName);
	DDX_Text(pDX, IDC_EDIT_IP, m_szIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_szPort);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUserName);
	DDX_Text(pDX, IDC_EDIT_USERPWD, m_szUserPwd);
	DDX_Control(pDX,IDC_COMBO_DBTYPE,m_wndCmb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageOne, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogPageOne)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_CBN_SELCHANGE(IDC_COMBO_DBTYPE, &CWebPageOne::OnSelchangeComboDbType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CWebPageOne ��Ϣ�������
BOOL CWebPageOne::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CPropertySheet *pMyWizard = (CPropertySheet *)GetParent();
	pMyWizard->SetWindowText(_T("���ݿ���������"));
	// TODO: Add extra initialization here
	//theConfig.LoadConnStr(m_szIP,m_szDbName,m_szUserName,m_szPort);

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
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CWebPageOne::OnSelchangeComboDbType()
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

BOOL CWebPageOne::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet *pMyWizard = (CPropertySheet *)GetParent();
	if (pMyWizard->GetPageCount() > 1)
	{
		pMyWizard->SetWizardButtons(PSWIZB_NEXT);
	}
	else
		pMyWizard->SetWizardButtons(PSWIZB_FINISH);
	


	CWnd *pWnd = pMyWizard->GetDlgItem(ID_WIZBACK);		//����һ����ť���ص�
	if(NULL != pWnd)
		pWnd->ShowWindow(SW_HIDE);

	return CPropertyPage::OnSetActive();
}

LRESULT CWebPageOne::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

	theLog.Write(_T("CWebPageOne::OnWizardNext,1"));

	if(!WriteDbSet())
	{
		theLog.Write(_T("!!CWebPageOne::OnWizardNext,2,WriteDbSet fail"));
		return 1;
	}

	theLog.Write(_T("CWebPageOne::OnWizardNext,3"));

	return CPropertyPage::OnWizardNext();
}

BOOL CWebPageOne::WriteDbSet()
{
	if(!UpdateData(TRUE))
		return FALSE;

	if(m_szIP == _T(""))
	{
		MessageBox(_T("���ݿ�IP��ַ����Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	if(m_szUserName == _T(""))
	{
		MessageBox(_T("���ݿ��û�������Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	if(m_szPort == _T(""))
	{
		MessageBox(_T("���Ӷ˿ڲ���Ϊ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	if(!TestConnect())
	{
		MessageBox(_T("�������ݿ�ʧ�ܣ���ȷ����������ݿ�������Ϣ�Ƿ���ȷ?"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	theLog.Write(_T("CWebPageOne::WriteDbSet,1"));

	CString szMsg;
	szMsg.Format(_T("Data Source=%s,%s;Initial Catalog=%s;User ID=%s;Password=%s"),
		m_szIP,m_szPort,m_szDbName,m_szUserName,m_szUserPwd);

	CCommonFun::SetDoNetDBStr(szMsg);

	theLog.Write(_T("CWebPageOne::WriteDbSet,5"));

	return TRUE;
}

BOOL CWebPageOne::TestConnect()
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

void CWebPageOne::OnTest() 
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

BOOL CWebPageOne::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!WriteDbSet())
		return FALSE;

	return CPropertyPage::OnWizardFinish();
}
