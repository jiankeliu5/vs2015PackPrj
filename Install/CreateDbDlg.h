#pragma once


// CCreateDbDlg �Ի���

class CCreateDbDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateDbDlg)

public:
	CCreateDbDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateDbDlg();

// �Ի�������
	enum { IDD = IDD_DLG_CREATEDB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_sDbUser;
	CString m_sDbPwd;
	CString m_sDbPort;
	BOOL m_bInstalledSuc;
	CStringArray m_aryDb;
	CString m_sDbAddr;
	CComboBox m_wndCmb;
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedTest();
	afx_msg void OnSelchangeComboDbType();

	BOOL CreateDbOnLocalSqlExpress(CString sDbName);
	BOOL CreateDbByAdoInfo(CString sDbName, ADO_INFO& info);
};
