#pragma once


// CWebPageOne �Ի���

class CWebPageOne : public CPropertyPage
{
	DECLARE_DYNAMIC(CWebPageOne)

public:
	CWebPageOne();
	virtual ~CWebPageOne();
	CString	m_szDbName;
	CString	m_szIP;
	CString	m_szPort;
	CString	m_szUserName;
	CString	m_szUserPwd;
	CComboBox m_wndCmb;
	int m_nDbType;
// �Ի�������
	enum { IDD = IDD_PAGE_ONE };

	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();
	afx_msg void OnTest();

	void InitDbSet();
	BOOL WriteDbSet();
    BOOL TestConnect();

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnWizardFinish();
	afx_msg void OnSelchangeComboDbType();
};
