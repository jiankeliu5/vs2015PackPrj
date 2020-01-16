
// iPrintMgrTestDlg.h : ͷ�ļ�
//

#pragma once

class CXabPrinterMonitor;

// CiPrintMgrTestDlg �Ի���
class CiPrintMgrTestDlg : public CDialog, public ICltLogCallback
{
// ����
public:
	CiPrintMgrTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IPRINTMGRTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	//��ӡ��־
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
