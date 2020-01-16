#pragma once


// CEmbedWebPageTwo �Ի���

class CEmbedWebPageTwo : public CPropertyPage
{
	DECLARE_DYNAMIC(CEmbedWebPageTwo)

public:
	CEmbedWebPageTwo();
	virtual ~CEmbedWebPageTwo();

// �Ի�������
	enum { IDD = IDD_EMBED_PAGE_TWO };
    CComboBox	m_ctWeb;
    CString	m_szWebPort;
    CString m_szNewWebName;
    CString m_szNewWebIp;
    CString m_szNewWebPort;
    
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
    afx_msg void OnBnClickedCheckNewWeb();    
    
    CIISWeb m_Web;
    CPtrList m_WebList;
    bool m_bInitSuc;
    CString m_szVirtualPath;
    CString m_szVirtaulName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
