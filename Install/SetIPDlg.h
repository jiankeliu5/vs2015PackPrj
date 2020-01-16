#pragma once


// CSetIPDlg �Ի���

class CSetIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetIPDlg)

public:
	CSetIPDlg(PRODUCT_TYPE nShowType,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetIPDlg();

// �Ի�������
	enum { IDD = IDD_DLG_SETIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSetPath();

	void Load();
	BOOL Save();
	virtual BOOL OnInitDialog();

	CString m_sUpdataPath;
	UINT	m_nUpdataPort;
	PRODUCT_TYPE m_nShowType;
	CString m_sIP;
};
