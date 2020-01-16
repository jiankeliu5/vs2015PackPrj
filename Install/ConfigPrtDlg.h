#pragma once
#include "afxwin.h"
// CConfigPrtDlg �Ի���

class CConfigPrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigPrtDlg)

public:
	CConfigPrtDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigPrtDlg();

// �Ի�������
	enum { IDD = IDD_DLG_CONFIG_PRT };
	enum BRAND_TYPE
	{
		BT_SL,
		BT_DZ,
		BT_KM,
		BT_LM,
		BT_THIN_CLT,
		BT_LG,		//add by zxl,2013.03.08,����ӡ��
		BT_SL2,		//add by zxl,2013.07.01,ʩ�ֵ��µ�·��
		BT_JC,		//add by zxl,2013.09.16,���ɴ�ӡ��
		BT_ZD,		//add by zxl,2013.09.17,�𵩴�ӡ��
		BT_XP,		//add by zxl,2013.08.09,���մ�ӡ��
		BT_SX,		//add by zxl,2015.04.27,���Ǵ�ӡ��
		BT_HP,		//add by zxl,2015.07.16,���մ�ӡ��
		BT_EPSON,	//add by szy 2016.04.20,��������ӡ��
		BT_CANON,	//add by szy 2016.04.20,���ܴ�ӡ��
		BT_BROTHER, //add by zfq 2016.05.04,�ֵܴ�ӡ��
		BT_JT,		//add by zfq 2016.05.04,��ͼ��ӡ��
		BT_OKI,		//add by zfq 2016.05.04,OKI��ӡ��
		BT_SEIKO,	//add by zxl 2016.05.18,������ӡ��
		BT_PANTUM,		//add by zxl 2016.05.24,��ͼ
		BT_JOLIMARK,	//add by zxl 2016.05.24,ӳ��
		BT_GPRINTER,	//add by zxl 2016.05.24,�Ѳ�
		BT_PANASONIC,	//add by zxl 2016.05.24,����
		BT_LENOVO,		//add by zxl 2016.05.24,����
		BT_START,		//add by zxl 2016.05.24,ʵ��
		BT_XPRINTER,	//add by zxl 2016.05.24,о��
		BT_SONY,		//add by zxl 2016.05.24,����
		BT_DELL,		//add by zxl 2016.05.24,����
		BT_MAX = BT_DELL
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	BOOL Save();
	void Load();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void InitBrandUI();
	CString m_szServIP;
	CString m_sBindIp;
	BOOL m_bEnableCopyBak;
	CString m_szCopyDir;
	CComboBox m_wndCmbIP;
	BOOL m_bEnableSnmpPrintMonitor;
	BOOL m_bEnableEmbedWeb;
	CComboBox m_cmbBrand;
	int m_nBrandType;
	int m_nEmbedConnectionType;
	afx_msg void OnBnClickedRadioEmbedTypeListen();
	int m_nListenPort;
	afx_msg void OnBnClickedRadioEmbedTypeConnect();
	afx_msg void OnBnClickedRadioEmbedTypeDb();
	afx_msg void OnBnClickedRadioEmbedTypeNo();
	afx_msg void OnBnClickedBtnYtjAdv();
	afx_msg void OnCbnSelchangeComboBrand();
	CButton m_YTJAdv;
	afx_msg void OnBnClickedButtonCopyBak();
	afx_msg void OnBnClickedCheckCopybak();
};
