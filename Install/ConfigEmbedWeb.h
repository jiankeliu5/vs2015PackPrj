#pragma once


// ConfigEmbedWeb �Ի���

class ConfigEmbedWeb : public CDialog
{
	DECLARE_DYNAMIC(ConfigEmbedWeb)

public:
	ConfigEmbedWeb(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ConfigEmbedWeb();

// �Ի�������
	enum { IDD = IDD_DLG_EMBEDWEB };
	
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_szTitle;
	CString m_szColor;
	CString m_szSize;
	int m_nJump;
	int m_nPrtStationId;
	afx_msg void OnBnClickedOk();
};
