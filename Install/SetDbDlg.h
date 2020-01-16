#pragma once


// CSetDbDlg �Ի���

class CSetDbDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetDbDlg)

public:
	CSetDbDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetDbDlg();

// �Ի�������
	enum { IDD = IDD_DLG_SETDB };

	CString	m_szDbName;
	CString	m_szIP;
	CString	m_szPort;
	CString	m_szUserName;
	CString	m_szUserPwd;
	CComboBox m_wndCmb;
	int m_nDbType;
	BOOL TestConnect();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboDbType();

};
