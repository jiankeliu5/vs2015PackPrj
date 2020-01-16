#pragma once


// CWebPageTwo �Ի���

class CWebPageTwo : public CPropertyPage
{
	DECLARE_DYNAMIC(CWebPageTwo)

public:
	CWebPageTwo();
	virtual ~CWebPageTwo();

// �Ի�������
	enum { IDD = IDD_PAGE_TWO };
	CComboBox	m_ctWeb;
	CString	m_szWebPort;
	CString m_szNewWebName;
	CString m_szNewWebIp;
	CString m_szNewWebPort;
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboZhan();
	afx_msg void OnTest();
	BOOL InitWebSiteInfo();
	BOOL FinishWebSiteSet(BOOL bQuiet = FALSE);
	void ShowWebPara(int index);
	void OnTestIIS();
	void MakeHttpCon(char *cinfo, int imax);
	void StartWebServer(int nNum);
	BOOL IsCheck(int uID);
	void EnableAddNewWebSite(BOOL bEanble);
	//����Ӧ�ó��������
	void SetAppPool(CString szVersion = _T(""), BOOL bClassic = FALSE);

public:
	CIISWeb m_Web;
	CPtrList m_WebList;
	bool m_bInitSuc;
	CString m_szVirtualPath;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckNewWeb();
};
