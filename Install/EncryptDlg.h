#pragma once


// CEncryptDlg �Ի���

class CEncryptDlg : public CDialog
{
	DECLARE_DYNAMIC(CEncryptDlg)

public:
	CEncryptDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEncryptDlg();

// �Ի�������
	enum { IDD = IDD_DLG_ENCYPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_sSerial;
	int m_nTry ;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
