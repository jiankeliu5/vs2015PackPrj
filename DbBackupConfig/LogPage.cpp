// LogPage.cpp : ʵ���ļ�
#include "stdafx.h"
#include "DbBackupConfig.h"
#include "LogPage.h"

// CLogPage �Ի���
IMPLEMENT_DYNAMIC(CLogPage, CPropertyPage)

CLogPage::CLogPage()
	: CPropertyPage(CLogPage::IDD)
{

}

CLogPage::~CLogPage()
{
}

void CLogPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CLogPage::OnBnClickedButton1)
END_MESSAGE_MAP()

// CLogPage ��Ϣ�������
void CLogPage::OnBnClickedButton1()
{
	ShellExecute(NULL,_T("Open"),theLog.m_sLogPath,NULL,NULL,SW_SHOW);
}

BOOL CLogPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ����־�ļ��ж�ȡ��־�ڽ�������ʾ
	CString sFilePath = theLog.m_sLogPath;
	CFileException fe;
	CFile mSaveFile;
	CString strTemp,strEnd;

	do 
	{
		if (sFilePath.IsEmpty())
			break;

		// ����Ĭ��ֵ
		if (!mSaveFile.Open(sFilePath,CFile::modeRead,&fe))
			break;

		CArchive ar(&mSaveFile,CArchive::load);
		while (ar.ReadString(strTemp))
		{
			strEnd += strTemp;
			strEnd += _T("\r\n");
		}
		SetDlgItemText(IDC_EDIT1,strEnd);
		ar.Flush();
		ar.Close();
		mSaveFile.Close();
	} while (0);
	return TRUE;
}
