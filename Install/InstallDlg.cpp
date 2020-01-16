
// InstallDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Install.h"
#include "InstallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CInstallDlg �Ի���




CInstallDlg::CInstallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CInstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInstallDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CInstallDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CInstallDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CInstallDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CInstallDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CInstallDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CInstallDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON10, &CInstallDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CInstallDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CInstallDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CInstallDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CInstallDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CInstallDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CInstallDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CInstallDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CInstallDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CInstallDlg::OnBnClickedButton19)
END_MESSAGE_MAP()


// CInstallDlg ��Ϣ�������

BOOL CInstallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CInstallDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CInstallDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CInstallDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CInstallDlg::OnBnClickedButton1()
{
	CDrvInstallHelper helper;
	helper.CallFun("InstallNdis");
}

void CInstallDlg::OnBnClickedButton2()
{
	CDrvInstallHelper helper;
	helper.CallFun("UnInstallNdis");
}

void CInstallDlg::OnBnClickedButton3()
{
	CDrvInstallHelper helper;
	helper.CallFun("InstallPrinter");

}

void CInstallDlg::OnBnClickedButton4()
{

	CDrvInstallHelper helper;
	helper.CallFun("UnInstallPrinter");
}

void CInstallDlg::OnBnClickedButton5()
{
	CDrvInstallHelper helper;
	helper.CallFun("InstallMirrDisp");
}

void CInstallDlg::OnBnClickedButton6()
{
	CDrvInstallHelper helper;
	helper.CallFun("UnInstallMirrDisp");
}

void CInstallDlg::OnBnClickedButton10()
{
	CDrvInstallHelper helper;
	helper.CallFun("InstallDeviceFilter");
}

void CInstallDlg::OnBnClickedButton11()
{
	CDrvInstallHelper helper;
	helper.CallFun("UnInstallDeviceFilter");
}

void CInstallDlg::OnBnClickedButton12()
{
	CDrvInstallHelper helper;
	helper.CallFun("InstallFileFilter");
}

void CInstallDlg::OnBnClickedButton13()
{
	CDrvInstallHelper helper;
	helper.CallFun("UnInstallFileFilter");
}

void CInstallDlg::OnBnClickedButton14()
{
	CDbInstaller db;
//	db.InstallDbBySqlFile(_T("192.168.2.12"),_T("iSec_Platform"),_T("sa"),_T("sa"),_T("1433"),_T("e:\\iSec_Platform.sql"));
}

void CInstallDlg::OnBnClickedButton15()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CInstallDlg::OnBnClickedButton16()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

LPDEVMODE GetLandscapeDevMode(HWND hWnd, TCHAR *pDevice)
{

	HANDLE      hPrinter;
	LPDEVMODE   pDevMode;
	DWORD       dwNeeded, dwRet;

	/* Start by opening the printer */ 
	if (!OpenPrinter(pDevice, &hPrinter, NULL))
		return NULL;

	/*
	* Step 1:
	* Allocate a buffer of the correct size.
	*/ 
	dwNeeded = DocumentProperties(hWnd,
		hPrinter,       /* Handle to our printer. */ 
		pDevice,        /* Name of the printer. */ 
		NULL,           /* Asking for size, so */ 
		NULL,           /* these are not used. */ 
		0);             /* Zero returns buffer size. */ 
	pDevMode = (LPDEVMODE)malloc(dwNeeded);

	/*
	* Step 2:
	* Get the default DevMode for the printer and
	* modify it for your needs.
	*/ 
	dwRet = DocumentProperties(hWnd,
		hPrinter,
		pDevice,
		pDevMode,       /* The address of the buffer to fill. */ 
		NULL,           /* Not using the input buffer. */ 
		DM_OUT_BUFFER); /* Have the output buffer filled. */ 
	if (dwRet != IDOK)
	{
		/* If failure, cleanup and return failure. */ 
		free(pDevMode);
		ClosePrinter(hPrinter);
		return NULL;
	}

	/*
	* Make changes to the DevMode which are supported.
	*/ 
	if (pDevMode->dmFields & DM_ORIENTATION)
	{
		/* If the printer supports paper orientation, set it.*/ 
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
	}

	if (pDevMode->dmFields & DM_DUPLEX)
	{
		/* If it supports duplex printing, use it. */ 
		pDevMode->dmDuplex = DMDUP_HORIZONTAL;
	}
	if (pDevMode->dmFields & DM_ICMMETHOD)
	{
		//pDevMode->dmICMMethod = ;
	}
	if (pDevMode->dmFields & DM_ICMINTENT)
	{
		pDevMode->dmICMIntent = DMICM_COLORIMETRIC;
	}
	pDevMode->dmColor = DMCOLOR_MONOCHROME;
	/*
	* Step 3:
	* Merge the new settings with the old.
	* This gives the driver an opportunity to update any private
	* portions of the DevMode structure.
	*/ 
	dwRet = DocumentProperties(hWnd,
		hPrinter,
		pDevice,
		pDevMode,       /* Reuse our buffer for output. */ 
		pDevMode,       /* Pass the driver our changes. */ 
		DM_IN_BUFFER |  /* Commands to Merge our changes and */ 
		DM_OUT_BUFFER); /* write the result. */ 

	/* Finished with the printer */ 
	ClosePrinter(hPrinter);

	if (dwRet != IDOK)
	{
		/* If failure, cleanup and return failure. */ 
		free(pDevMode);
		return NULL;
	}

	/* Return the modified DevMode structure. */ 
	return pDevMode;

}


void CInstallDlg::OnBnClickedButton17()
{
	DMICMMETHOD_SYSTEM;
	DM_INTERLACED;
#if 1
	HANDLE h = 0;
	OpenPrinter(_T("XabVPrinter"),&h,0);
	BYTE szBuf[4096] = {0};
	DWORD dwSize = 4096;
	DWORD dwOut = 0;
	GetPrinter(h,2,szBuf,dwSize,&dwOut);
	PRINTER_INFO_2* pInfo = (PRINTER_INFO_2*)szBuf;
	LPDEVMODE pIn = (LPDEVMODE)pInfo->pDevMode;
	BYTE szOut[4096] = {0};
	LPDEVMODE pOut = (LPDEVMODE)pIn;
	//CopyMemory(pOut,)
//	AdvancedDocumentProperties(m_hWnd,h,_T("XabVPrinter"),pOut,pIn);
	PRINTER_INFO_8 p8;
	p8.pDevMode = pOut;
	DMICMMETHOD_SYSTEM;
	if (pOut->dmFields & DM_ICMMETHOD)
	{
		pOut->dmICMIntent = DMICMMETHOD_DRIVER;
	}
	
	
	SetPrinter(h,8,(BYTE*)&p8,0);
	ClosePrinter(h);
#endif
	//GetLandscapeDevMode(m_hWnd,_T("XabVPrinter"));
}

void CInstallDlg::OnBnClickedButton18()
{
	CIISWeb web;
	CString sPath;
	web.GetWebSitePath(1,sPath);
	
	AfxMessageBox(sPath);
}


#include <atlsecurity.h>
void CInstallDlg::OnBnClickedButton19()
{

}
