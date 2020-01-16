
// iPrintMgrTestDlg.h : 头文件
//

#pragma once

class CXabPrinterMonitor;

// CiPrintMgrTestDlg 对话框
class CiPrintMgrTestDlg : public CDialog, public ICltLogCallback
{
// 构造
public:
	CiPrintMgrTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IPRINTMGRTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	//打印日志
	virtual void OnPrinted(void*);
	afx_msg void OnBnClickedButton1();
protected:
	CXabPrinterMonitor *m_pMonitor;
public:
	CString m_szSplPath;
	afx_msg void OnBnClickedBtnAddJob();
protected:
	BOOL SpoolToEmf(const char* pFile,const char* pOut); 
public:
	afx_msg void OnBnClickedBtnprintpdf();

private:
	short m_dmOrientation;
	short m_dwColor;
	short m_dwPaperSize;
	short m_dwDuplex;
public:
	CString m_szPrinterName;
	CString m_szDocName;
	CString m_szUserName;
	afx_msg void OnBnClickedBtnaddjob2();
};
