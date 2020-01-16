// RestorePage.cpp : ʵ���ļ�
#include "stdafx.h"
#include "DbBackupConfig.h"
#include "RestorePage.h"

// CRestorePage �Ի���
IMPLEMENT_DYNAMIC(CRestorePage, CPropertyPage)
CRestorePage::CRestorePage()
	: CPropertyPage(CRestorePage::IDD)
{

}

CRestorePage::~CRestorePage()
{
}

void CRestorePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRestorePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CRestorePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CRestorePage::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &CRestorePage::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_BUTTON3, &CRestorePage::OnBnClickedButton3)
END_MESSAGE_MAP()


// CRestorePage ��Ϣ�������
void CRestorePage::OnBnClickedButton1()
{
	CFileDialog dlgsearch(true,_T("·��"),NULL,OFN_READONLY,_T("*.bak|*.bak||"));
	if(dlgsearch.DoModal() != IDOK)
		return;

	SetDlgItemText(IDC_EDIT4,dlgsearch.GetPathName());
}

void CRestorePage::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT9,m_sDBServerName);
	GetDlgItemText(IDC_EDIT1,m_sDBName);
	GetDlgItemText(IDC_EDIT5,m_sUserAccount);
	GetDlgItemText(IDC_EDIT6,m_sUserPsw);
	GetDlgItemText(IDC_EDIT4,m_sRestoreDBPath);
	if (m_sDBServerName.IsEmpty() || m_sDBName.IsEmpty() || m_sRestoreDBPath.IsEmpty())
	{
		MessageBox(_T("���������ݿ�������"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		if (m_sDBServerName.IsEmpty())
			GetDlgItem(IDC_EDIT9)->SetFocus();
		if (m_sDBName.IsEmpty())
			GetDlgItem(IDC_EDIT1)->SetFocus();
		if (m_sRestoreDBPath.IsEmpty())
			GetDlgItem(IDC_BUTTON1)->SetFocus();
		return;
	}
	else if (PathIsDirectory(m_sRestoreDBPath))
	{
		MessageBox(_T("��ָ�����ݿ⻹ԭ�ļ�"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		GetDlgItem(IDC_BUTTON1)->SetFocus();
		return;
	}

	if (MessageBox(_T("��ԭ���ݿ�ʱ�Դ����ݿ�����ӽ��Ͽ�,�Ƿ������"),_T("��ʾ"),MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	CDbInstaller dbins;
	if (m_sInstanceName.IsEmpty())
	{
		if (m_sPort.IsEmpty())
			dbins.GetDefaultPort(m_sPort);
		dbins.RestoreDBFromFile(m_sDBServerName,m_sDBName,m_sUserAccount,m_sUserPsw,m_sPort,m_sRestoreDBPath);
	}
	else
	{
		dbins.RestoreDBFromFile2(m_sDBServerName,m_sDBName,m_sUserAccount,m_sUserPsw,m_sInstanceName,m_sRestoreDBPath);
	}
}

BOOL CRestorePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CDBBackFun::GetDatebaseConfig(m_sDBServerName,m_sDBName,m_sPort,m_sInstanceName,m_sUserAccount,m_sUserPsw);
	SetDlgItemText(IDC_EDIT9,m_sDBServerName);
	SetDlgItemText(IDC_EDIT1,m_sDBName);
	SetDlgItemText(IDC_EDIT5,m_sUserAccount);
	SetDlgItemText(IDC_EDIT6,m_sUserPsw);
	CDBBackFun::GetDBRestoreConfig(m_sRestoreDBPath,m_sInitDBPath);
	if (!m_sRestoreDBPath.IsEmpty())
		CCommonFun::CreateDir(m_sRestoreDBPath);
	SetDlgItemText(IDC_EDIT4,m_sRestoreDBPath);

	if (!m_sInitDBPath.IsEmpty())
		CCommonFun::CreateDir(m_sInitDBPath);
	SetDlgItemText(IDC_EDIT10,m_sInitDBPath);

	CString sParam = _T("��ָ·��Ϊ�ļ�*.mdf��*.ldf���ڵ�·������ʹ��COPY�ļ����������ݿ�����̨�������ת��ʱʹ�ñ� \n���ܡ�����������ļ�COPY��MSSQLSERVER�µ�DATAĿ¼�С�         \r\nSql Server ·�� \r\n     $:\\Program Files\\Microsoft SQL Server\\MSSQL\\Data");
	SetDlgItemText(IDC_EDIT2,sParam);
	return TRUE; 
}

void CRestorePage::OnBnClickedOk2()
{
	UpdateData();
	GetDlgItemText(IDC_EDIT9,m_sDBServerName);
	GetDlgItemText(IDC_EDIT1,m_sDBName);
	GetDlgItemText(IDC_EDIT5,m_sUserAccount);
	GetDlgItemText(IDC_EDIT6,m_sUserPsw);
	GetDlgItemText(IDC_EDIT10,m_sInitDBPath);
	if (m_sDBServerName.IsEmpty() || m_sDBName.IsEmpty() || m_sInitDBPath.IsEmpty())
	{
		MessageBox(_T("���������ݿ�������"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		if (m_sDBServerName.IsEmpty())
			GetDlgItem(IDC_EDIT9)->SetFocus();
		if (m_sDBName.IsEmpty())
			GetDlgItem(IDC_EDIT1)->SetFocus();
		if (m_sInitDBPath.IsEmpty())
			GetDlgItem(IDC_EDIT10)->SetFocus();
		return;
	}

	CDbInstaller dbins;
	if (m_sInstanceName.IsEmpty())
	{
		if (m_sPort.IsEmpty())
			dbins.GetDefaultPort(m_sPort);
		dbins.RevertDBFromFile(m_sDBServerName,m_sDBName,m_sUserAccount,m_sUserPsw,m_sPort,m_sInitDBPath);
	}
	else
	{
		dbins.RevertDBFromFile2(m_sDBServerName,m_sDBName,m_sUserAccount,m_sUserPsw,m_sInstanceName,m_sInitDBPath);
	}
}

void CRestorePage::OnBnClickedButton3()
{
	CString sPath = CCommonFun::GetDirFromShell(FALSE);
	if (sPath.IsEmpty())
		return;

	// ���õ�ǰĿ¼
	if (sPath.Right(1) != '\\' && sPath.Right(1) != '/')
		sPath += _T("\\");

	SetDlgItemText(IDC_EDIT10,sPath);
}

void CRestorePage::OnOK()
{
	UpdateData();
	GetDlgItemText(IDC_EDIT10,m_sInitDBPath);
	GetDlgItemText(IDC_EDIT4,m_sRestoreDBPath);
	CDBBackFun::SetDBRestoreConfig(m_sRestoreDBPath,m_sInitDBPath);
}
