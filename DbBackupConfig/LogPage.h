#pragma once


// CLogPage �Ի���

class CLogPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLogPage)

public:
	CLogPage();
	virtual ~CLogPage();

// �Ի�������
	enum { IDD = IDD_PAGE_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedButton1();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};
