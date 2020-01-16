#pragma once


// CDlgConfigEmbeded 对话框

class CDlgConfigEmbeded : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfigEmbeded)

public:
	CDlgConfigEmbeded(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgConfigEmbeded();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETEMBRD_SVR };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nPort;
	int m_nWebSvrPort;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnTestcnnt();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnTestcnnt2();
};