#pragma once
#include "afxwin.h"


// CDlgCfgPrtCenterSvr �Ի���

class CDlgCfgPrtCenterSvr : public CDialog
{
	DECLARE_DYNAMIC(CDlgCfgPrtCenterSvr)

public:
	CDlgCfgPrtCenterSvr(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCfgPrtCenterSvr();

// �Ի�������
	enum { IDD = IDD_DLG_CFG_PrtCenterSvr };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
protected:
	CComboBox m_wndCmbIP;

	CString m_sBindIp;
};
