// ExportDataDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Install.h"
#include "ExportDataDlg.h"

#include "../Public/src/SecText.h"
// CExportDataDlg �Ի���

IMPLEMENT_DYNAMIC(CExportDataDlg, CDialog)

CExportDataDlg::CExportDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportDataDlg::IDD, pParent)
{
	m_szDbName = "compmang2";
	m_szIP = CCommonFun::GetLocalIP();
	m_szPort = _T("1433");
	m_szUserName = _T("sa");
	m_szUserPwd = _T("");
}

CExportDataDlg::~CExportDataDlg()
{
}

void CExportDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDataDlg)
	DDX_Text(pDX, IDC_EDIT_DBNAME, m_szDbName);
	DDX_Text(pDX, IDC_EDIT_IP, m_szIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_szPort);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUserName);
	DDX_Text(pDX, IDC_EDIT_USERPWD, m_szUserPwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDataDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CNNT, &CExportDataDlg::OnBnClickedBtnCnnt)
	ON_BN_CLICKED(IDC_BUTTON2, &CExportDataDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CExportDataDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CExportDataDlg ��Ϣ�������

void CExportDataDlg::OnBnClickedBtnCnnt()
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
BOOL CExportDataDlg::TestConnect()
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

void CExportDataDlg::OnBnClickedButton2()
{
	if(!UpdateData(TRUE))
		return ;
	BOOL bRet = TRUE;
	CoInitialize(0);
	CAdo ado;
	ado.SetConnStr(m_szIP,m_szDbName,m_szUserName,m_szUserPwd,m_szPort);
	bRet = ado.Connect();

	if (bRet)
	{
		CString sSQL;
		int nType = 0;
		CStringArray ary;
		long lRow = 0;
		long lCol = 0;
		sSQL.Format(_T("select username,userpsw,userrealname,depname from cardinfo a left join "
			" departmenttype b on  a.depid = b.depid "));
		
		if(ado.GetRecordset(sSQL,ary,lRow,lCol) && lRow > 0)
		{
			//
			CString sCode = _T("");
			CString sFileName;
			CString		sType = _T("�ļ� (*.txt)|*.txt|Execel (*.xls)|*.xls||");

			CString		sFilePath;
			CFileDialog dlg(FALSE,_T(""),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sType,NULL);
			if(dlg.DoModal()==IDOK)
			{
				TRY
				{
					int nType = 0;
					sFileName=dlg.GetFileName();
					CString sTmp = dlg.GetFileExt();
					if(sTmp.CompareNoCase(_T("txt")) == 0)
					{
						nType = 0;
					}
					else
					{
						nType = 1;
					}
				}
				CATCH(CException,e)
				{
					return;
				}
				END_CATCH
			}

			//���±�
			if(nType == 0)
			{
				sCode = _T("\t");
			}
			//excel
			else if(nType == 1)
			{
				sCode.Format(_T("%c"),9);
			}
			CStdioFile sf;
			BOOL bRet = sf.Open(sFileName,CFile::modeReadWrite | CFile::modeCreate | CFile::typeText);
			if(!bRet)
			{
				AfxMessageBox(sFileName + _T("��ʧ�ܣ�"));
				return;
			}
			

			for(int nIndex = 0; nIndex < lRow; nIndex ++)
			{
				CString sInfo,sTmp;
				for(int nSubIndex = 0; nSubIndex < lCol; nSubIndex ++)
				{
					sTmp = ary.GetAt(nIndex * lCol + nSubIndex);
					if (nSubIndex == 1)
					{
						CSecText sec;
						sTmp = sec.unsecretstr(sTmp);
					}
					sInfo += sTmp;
					sInfo += sCode;
				}
				sInfo.TrimRight(sCode);
				sInfo += _T("\n");
				sf.WriteString(sInfo);
			}
			sf.Close();
			MessageBox(_T("���ݵ����ɹ���"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		}
	}

	CoUninitialize();
}

void CExportDataDlg::OnBnClickedBtnSave()
{
	if(!UpdateData(TRUE))
		return ;

	theConfig.SaveSyxConnStr(m_szIP,m_szDbName,m_szUserName,m_szUserPwd,m_szPort);
}
