#pragma once


// CDlgConfigEmbeded �Ի���

class CDlgConfigEmbeded : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfigEmbeded)

public:
	CDlgConfigEmbeded(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConfigEmbeded();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETEMBRD_SVR };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nPort;
	int m_nWebSvrPort;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnTestcnnt();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnTestcnnt2();
};