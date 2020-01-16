#pragma once
#include "afxwin.h"
// CConfigPrtDlg 对话框

class CConfigPrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigPrtDlg)

public:
	CConfigPrtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigPrtDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CONFIG_PRT };
	enum BRAND_TYPE
	{
		BT_SL,
		BT_DZ,
		BT_KM,
		BT_LM,
		BT_THIN_CLT,
		BT_LG,		//add by zxl,2013.03.08,理光打印机
		BT_SL2,		//add by zxl,2013.07.01,施乐的新电路板
		BT_JC,		//add by zxl,2013.09.16,京瓷打印机
		BT_ZD,		//add by zxl,2013.09.17,震旦打印机
		BT_XP,		//add by zxl,2013.08.09,夏普打印机
		BT_SX,		//add by zxl,2015.04.27,三星打印机
		BT_HP,		//add by zxl,2015.07.16,惠普打印机
		BT_EPSON,	//add by szy 2016.04.20,爱普生打印机
		BT_CANON,	//add by szy 2016.04.20,佳能打印机
		BT_BROTHER, //add by zfq 2016.05.04,兄弟打印机
		BT_JT,		//add by zfq 2016.05.04,京图打印机
		BT_OKI,		//add by zfq 2016.05.04,OKI打印机
		BT_SEIKO,	//add by zxl 2016.05.18,精工打印机
		BT_PANTUM,		//add by zxl 2016.05.24,奔图
		BT_JOLIMARK,	//add by zxl 2016.05.24,映美
		BT_GPRINTER,	//add by zxl 2016.05.24,佳博
		BT_PANASONIC,	//add by zxl 2016.05.24,松下
		BT_LENOVO,		//add by zxl 2016.05.24,联想
		BT_START,		//add by zxl 2016.05.24,实达
		BT_XPRINTER,	//add by zxl 2016.05.24,芯烨
		BT_SONY,		//add by zxl 2016.05.24,索尼
		BT_DELL,		//add by zxl 2016.05.24,戴尔
		BT_MAX = BT_DELL
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
