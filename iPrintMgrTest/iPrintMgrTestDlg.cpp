
// iPrintMgrTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "iPrintMgrTest.h"
#include "iPrintMgrTestDlg.h"
#include "XabPrinterMonitor.h"

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


// CiPrintMgrTestDlg �Ի���




CiPrintMgrTestDlg::CiPrintMgrTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CiPrintMgrTestDlg::IDD, pParent)
	, m_szSplPath(_T("C:\\Users\\Administrator\\Desktop\\00036.SPL"))
	, m_szPrinterName(_T("HP Universal Printing PS"))
// 	, m_szSplPath(_T("F:\\testdoc\\gs\\00044.SPL"))
// 	, m_szPrinterName(_T("\\\\192.168.4.147\\KONICA MINOLTA C364SeriesPCL"))
	, m_szDocName(_T("Test.doc"))
	, m_szUserName(_T("TestUser"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMonitor = NULL;
}

void CiPrintMgrTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EditSplPath, m_szSplPath);
	DDX_Text(pDX, IDC_EditPrinterName, m_szPrinterName);
	DDX_Text(pDX, IDC_EditDocName, m_szDocName);
	DDX_Text(pDX, IDC_EditUserName, m_szUserName);
}

BEGIN_MESSAGE_MAP(CiPrintMgrTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CiPrintMgrTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BtnAddJob, &CiPrintMgrTestDlg::OnBnClickedBtnAddJob)
	ON_BN_CLICKED(IDC_BtnPrintPDF, &CiPrintMgrTestDlg::OnBnClickedBtnprintpdf)
	ON_BN_CLICKED(IDC_BtnAddJob2, &CiPrintMgrTestDlg::OnBnClickedBtnaddjob2)
END_MESSAGE_MAP()


// CiPrintMgrTestDlg ��Ϣ�������

BOOL CiPrintMgrTestDlg::OnInitDialog()
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

	//m_dmOrientation = ;
	m_dwColor = DMCOLOR_MONOCHROME;
	m_dwPaperSize = DMPAPER_A4;
	m_dwDuplex = DMDUP_VERTICAL;

	m_szPrinterName = theIniFile.GetVal("PrintJob", "PrintName", "");
	m_szDocName = theIniFile.GetVal("PrintJob", "DocName", "");
	m_szUserName = theIniFile.GetVal("PrintJob", "UserName", "");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CiPrintMgrTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CiPrintMgrTestDlg::OnPaint()
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
HCURSOR CiPrintMgrTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//��ӡ��־
void CiPrintMgrTestDlg::OnPrinted(void*)
{
	theLog.Write("##======CiPrintMgrTestDlg::OnPrinted,1");
}


void CiPrintMgrTestDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	IST_PRINTER_NAME = m_szPrinterName;
	if(m_pMonitor)
	{
		m_pMonitor->StartMonitor();
		delete m_pMonitor;
		m_pMonitor = NULL;
	}

	m_pMonitor = new CXabPrinterMonitor;
	m_pMonitor->SetCallback(this);
	m_pMonitor->StartMonitor();
}

// #include "spl_h.h"
BOOL CiPrintMgrTestDlg::SpoolToEmf(const char* pFile,const char* pOut)
{
	ASSERT(pFile);
	ASSERT(pOut);
	if(!pFile || !pOut)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	FILE *fp = NULL;
	do 
	{
		fp = fopen ( pFile, "rb" );
		assert ( fp && "�ļ���ʧ��!" );
		if(!fp)
		{
			break;
		}
		BYTE buf[2048] = {0};
		int nSize = fread (buf, 1, 2048, fp);
		SPL_HEADER *pSpl = reinterpret_cast<SPL_HEADER*>(buf);
		if ( nSize < sizeof(SPL_HEADER) )
		{
			break;
		}

		//doc name
		WCHAR * pName = (WCHAR*)((BYTE*)pSpl+pSpl->offDocumentName);

		int nSeek = pSpl->nSize;
		int nFileTag = 0;
		for (;0 == fseek ( fp, nSeek ,SEEK_SET);)
		{
			int nRet = fread (buf, 1, sizeof(SMR), fp);
			if ( nRet < sizeof(SMR) ) 
			{
				break;
			}

			SMR *pSMR = reinterpret_cast<SMR *>(buf);
			if ( SRT_METAFILE_DATA == pSMR->iType )
			{
				int nReadSize = pSMR-> nSize;// - sizeof(SMR);
				BYTE *pRead = new BYTE[nReadSize];
				assert(pRead && "pread = null");
				if(pRead)
				{
					int nReaded = fread (pRead, 1, nReadSize, fp );
					if ( nReadSize == nReaded )
					{
						ENHMETAHEADER *pKK = reinterpret_cast<ENHMETAHEADER*>( pRead );
						assert ( nReadSize == nReaded );
						char szWriteName[MAX_PATH] = {0};
						sprintf_s (szWriteName,sizeof(szWriteName), "%s\\%02d.emf",pOut,nFileTag++);
						FILE *fpWrite = fopen (szWriteName, "wb+" );
						assert ( fpWrite && "�ļ�д��ʧ��!" );
						if(fpWrite)
						{
							DWORD dwW = pKK->nBytes;// + sizeof(ENHMETAHEADER);//  + sizeof(ENHMETAHEADER);
							if ( dwW <= pSMR->nSize )
							{
								nReaded = fwrite (pRead, 1, dwW , fpWrite );
								assert ( nReaded == dwW );
							}
							fclose (fpWrite);
						}
					}
				}
			}
			else
			{
				//other type;
			}

			nSeek += pSMR->nSize + sizeof(SMR);
		}

	} while (0);

	if (fp)
	{
		fclose (fp);
		fp = NULL;
	}
	return TRUE;
}

void CiPrintMgrTestDlg::OnBnClickedBtnAddJob()
{
	UpdateData(TRUE);

	m_szPrinterName.Trim();
	m_szDocName.Trim();
	m_szUserName.Trim();

	if(m_szPrinterName.IsEmpty())
	{
		MessageBox("��ӡ��������Ϊ�գ�");
		return;
	}

	if(m_szSplPath.IsEmpty())
	{
		MessageBox("spl·������Ϊ�գ�");
		return;
	}

#if 0
	if(!SpoolToEmf(m_szSplPath, "C:\\xlog"))
	{
		CString szMsg;
		szMsg.Format("SpoolToEmf fail,err=%d", GetLastError());
		MessageBox(szMsg);
	}
	return;
#endif

	HANDLE hPrinter = NULL;

//	char* pJobType = "NT EMF 1.008";
	char* pJobType = "RAW";

	CString m_szCurSPLFile = m_szSplPath;
	CString m_szCurPrinterName = m_szPrinterName;
	CString szMsg;
	szMsg.Format("��ǰ��ӡ�ļ�����%s",m_szCurSPLFile);
	//      AfxMessageBox(szMsg);

	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = pJobType;

	char cPrinterName[100];
	strcpy(cPrinterName,m_szCurPrinterName);
	if(!OpenPrinter(cPrinterName,&hPrinter,&pd ) )
	{
		int nError = GetLastError();
		MessageBox("OpenPrinter fail");
		return ;
	}

	BYTE pData[1024] = {0};


	DWORD dwNeed = 0;
	BOOL bRet = AddJob(hPrinter,1,pData,1024,&dwNeed);
	if(!bRet)
	{
		szMsg.Format("AddJob fail,err=%d", GetLastError());
		MessageBox(szMsg);
		return ;
	}

	ADDJOB_INFO_1* pInfo = (ADDJOB_INFO_1*)pData;

	CString szFile = pInfo->Path;
	int nJobID = pInfo->JobId;

	szMsg.Format("AddJob�ɹ�,szFile=%s,nJobID=%d", szFile, nJobID);
	MessageBox(szMsg);

	// 	/*
	//----------------������ҵ����---------------------
	if(!GetJob(hPrinter,nJobID,2,NULL,0,&dwNeed))
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			TRACE("GetJobDetailInfo-----GetJob1 error �������Ϊ��%d",GetLastError());
			return ;
		}
	}        
	BYTE *pJobData = new BYTE[dwNeed];
	if(!pJobData)
	return ;
	memset(pJobData,0,dwNeed);
	DWORD dwState = 0;

	if(!GetJob(hPrinter,nJobID,2,pJobData,dwNeed,&dwNeed))//JOB_STATUS_SPOOLING
	{
		TRACE("GetJobDetailInfo-----GetJob2 error �������Ϊ��%d",GetLastError());
		return ;
	}        
	JOB_INFO_2* pJobInfo = (JOB_INFO_2 *)pJobData;
// 	pJobInfo->pDatatype = pJobType;
// 	pJobInfo->TotalPages = 1;
// 	pJobInfo->PagesPrinted = 1;//JOB_STATUS_COMPLETE
	//pJobInfo->pDevMode->dmCopies = 2;//DMCOLOR_COLOR
	if (m_szUserName.GetLength() > 0)
	{
		pJobInfo->pUserName = m_szUserName.GetBuffer();
	}
	if (m_szDocName.GetLength() > 0)
	{
		pJobInfo->pDocument = m_szDocName.GetBuffer();
	}

	pJobInfo->pDevMode->dmColor = DMCOLOR_MONOCHROME;

	//------------------------------------------

	if(!SetJob(hPrinter,nJobID,2,pJobData,0))
	{
		int nError = GetLastError();
		szMsg.Format("SetJob��ҵʧ�ܣ�%d",nError);
		AfxMessageBox(szMsg);
	}
// 	*/

	//----------------------------------------------
	CFile file;
	if(file.Open(m_szCurSPLFile,CFile::modeRead) == 0)
	{
//		AfxMessageBox("�򿪴�ӡ�ļ�ʧ�ܣ�");
		int nError = GetLastError();
		szMsg.Format("�򿪴�ӡ�ļ�ʧ��,err=%d", GetLastError());
		MessageBox(szMsg);
		return ;
	}
	int nFileLen = file.GetLength();
	BYTE* pTemp = new BYTE[nFileLen];
	if(!pTemp)
	{
		return ;
	}

	int nRet = file.Read(pTemp,nFileLen);

	szMsg.Format("Դspl�ļ���:%d,��ȡ:%d", nFileLen, nRet);
	MessageBox(szMsg);


	file.Close();

//	PRINT_DETAIL* pPrintDetailInfo = (PRINT_DETAIL*)pTemp;
	CFile oFile2;
	if(oFile2.Open(szFile,CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		int nError = GetLastError();
		szMsg.Format("�򿪴�ӡ�ļ�ʧ��,err=%d", GetLastError());
		MessageBox(szMsg);
		return ;
	}
//	file.Write((BYTE*)pTemp+sizeof(PRINT_DETAIL),nFileLen+sizeof(PRINT_DETAIL));
	oFile2.Write((BYTE*)pTemp, nFileLen);
	oFile2.Close();

	szMsg.Format("д��Ŀ��spl�ļ��ɹ�,%s", szFile);
	MessageBox(szMsg);

	//----------------------------------------
#if 1
	bRet = ScheduleJob(hPrinter,nJobID);
	if(bRet)
	{
		//AfxMessageBox("������ҵ�ɹ�");
		int nError = GetLastError();
		szMsg.Format("������ҵ�ɹ��ɹ�");
		MessageBox(szMsg);
	}
	else
	{
		int nError = GetLastError();
		szMsg.Format("ScheduleJobf��ҵʧ�ܣ�%d",nError);
		MessageBox(szMsg);
	}
#endif

	ClosePrinter(hPrinter);
	if(pTemp)
		delete[] pTemp;

}

BOOL GetPrinterDevMode(CString& pszPrinterName, DEVNAMES *&pDevNames, BYTE*& pDevMode)
{
	BOOL bRet = FALSE;
	// Open printer
	HANDLE hPrinter;
	if (OpenPrinter(pszPrinterName.GetBuffer(), &hPrinter, NULL) == FALSE)
	{
		theLog.Write("OpenPrinter err ");
		return FALSE;
	}
	// obtain PRINTER_INFO_2 structure and close printer
	DWORD dwBytesReturned, dwBytesNeeded;
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)new BYTE[dwBytesNeeded];

	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,
		&dwBytesReturned) == 0) 
	{
		delete p2;
		p2 = 0;
		theLog.Write("GetPrinter err ");
		ClosePrinter(hPrinter);
		return FALSE;
	}

	DEVMODE* pDev = 0;
	do 
	{
		//Զ�̴�ӡ��û��Ȩ�޴�
		if(p2->pDevMode)
		{
			// Allocate a global handle for DEVMODE
			pDevMode = new BYTE[sizeof(*p2->pDevMode) +
				p2->pDevMode->dmDriverExtra];

			// copy DEVMODE data from PRINTER_INFO_2::pDevMode
			memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) +
				p2->pDevMode->dmDriverExtra);


			// Compute size of DEVNAMES structure from PRINTER_INFO_2's data
			DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name
			DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name
			DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name

			// Allocate a global handle big enough to hold DEVNAMES.
			pDevNames = (DEVNAMES*)new BYTE[ sizeof(DEVNAMES) +
				(drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR)];


			// Copy the DEVNAMES information from PRINTER_INFO_2
			// tcOffset = TCHAR Offset into structure
			int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);
			ASSERT(sizeof(DEVNAMES) == tcOffset*sizeof(TCHAR));

			pDevNames->wDriverOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,
				drvNameLen*sizeof(TCHAR));
			tcOffset += drvNameLen;

			pDevNames->wDeviceOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,
				ptrNameLen*sizeof(TCHAR));
			tcOffset += ptrNameLen;

			pDevNames->wOutputOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,
				porNameLen*sizeof(TCHAR));
			pDevNames->wDefault = 0;

			bRet = TRUE;
			break;
		}

		theLog.Write("remote printer,no right,so check by doc");
		//-----------------------------
		//

		DWORD dwNeeded = ::DocumentProperties(NULL, hPrinter, pszPrinterName.GetBuffer(), NULL, NULL, 0);
		pDev = (DEVMODE*)new BYTE[dwNeeded];
		if ( IDOK != ::DocumentProperties(NULL, hPrinter, pszPrinterName.GetBuffer(), (DEVMODE*)pDev, NULL, DM_OUT_BUFFER) )
		{
			theLog.Write(_T("Error: DocumentProperties failed"));
			break;
		}
		else
		{
			// Allocate a global handle for DEVMODE
			pDevMode = new BYTE[sizeof(*pDev) +
				pDev->dmDriverExtra];

			// copy DEVMODE data from PRINTER_INFO_2::pDevMode
			memcpy(pDevMode, pDev, sizeof(*pDev) +
				pDev->dmDriverExtra);


			// Compute size of DEVNAMES structure from PRINTER_INFO_2's data
			DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name
			DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name
			DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name

			// Allocate a global handle big enough to hold DEVNAMES.
			pDevNames = (DEVNAMES*)new BYTE[ sizeof(DEVNAMES) +
				(drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR)];


			// Copy the DEVNAMES information from PRINTER_INFO_2
			// tcOffset = TCHAR Offset into structure
			int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);
			ASSERT(sizeof(DEVNAMES) == tcOffset*sizeof(TCHAR));

			pDevNames->wDriverOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,
				drvNameLen*sizeof(TCHAR));
			tcOffset += drvNameLen;

			pDevNames->wDeviceOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,
				ptrNameLen*sizeof(TCHAR));
			tcOffset += ptrNameLen;

			pDevNames->wOutputOffset = tcOffset;
			memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,
				porNameLen*sizeof(TCHAR));
			pDevNames->wDefault = 0;

			bRet = TRUE;
		}

	} while (0);

	if(pDev)
	{
		delete pDev;
	}

	if(p2)
	{
		delete p2;
	}
	ClosePrinter(hPrinter);
	return bRet;
}
void CiPrintMgrTestDlg::OnBnClickedBtnprintpdf()
{
	UpdateData(TRUE);
	if(m_szSplPath.IsEmpty())
	{
		MessageBox("pdf·��Ϊ��");
		return;
	}

	HANDLE hPrinter = NULL;

//	char* pJobType = "NT EMF 1.008";
	char* pJobType = "RAW";

	CString m_szCurSPLFile = m_szSplPath;
	CString m_szCurPrinterName = m_szPrinterName;
	CString szMsg;
	szMsg.Format("��ǰ��ӡ�ļ�����%s",m_szCurSPLFile);
	//AfxMessageBox(szMsg);

	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = pJobType;

	char cPrinterName[100];
	strcpy(cPrinterName,m_szCurPrinterName);
	if(!OpenPrinter(cPrinterName,&hPrinter,&pd ) )
	{
		int nError = GetLastError();
		MessageBox("OpenPrinter fail");
		return ;
	}

	char pPJLCommand[1024] = {0};
// 	pPJLCommand[0] = 0x1B;
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "%-12345X\r\n");
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET COPIES = 1\r\n");
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET DUPLEX = OFF\r\n");		//OFF,ON
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET ORIENTATION = PORTRAIT\r\n");		//��ӡ����LANDSCAPE-���,PORTRAIT-����
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET BINDING = TOP\r\n");		//1.����ʱ��LEFT=���ߡ�TOP=�̱�;	2.���ʱ��LEFT=�̱ߡ�TOP=����
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET PLANESINUSE = 1\r\n");		//color 3 ��ɫ 1 �ڰ�
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET MEDIASOURCE = AUTO\r\n");	//AUTO,TRAY1,TRAY2,TRAY3,TRAY4,MANUALFEED
// 	strcpy(pPJLCommand+strlen(pPJLCommand), "@PJL SET PAPER = A4\r\n");	//A4,A3,B5

	CFile file;
	if(file.Open(m_szCurSPLFile,CFile::modeRead) == 0)
	{
		int nError = GetLastError();
		szMsg.Format("�򿪴�ӡ�ļ�ʧ��,err=%d", GetLastError());
		MessageBox(szMsg);
		return ;
	}
	int nFileLen = file.GetLength();
	BYTE* pTemp = new BYTE[nFileLen];
	if(!pTemp)
	{
		return ;
	}

	int nRet = file.Read(pTemp,nFileLen);

	szMsg.Format("ԴPDF�ļ���:%d,��ȡ:%d", nFileLen, nRet);
	MessageBox(szMsg);
	file.Close();

	DOC_INFO_1 docinfo;
	docinfo.pDocName = m_szCurSPLFile.GetBuffer();
	docinfo.pOutputFile = "";
	docinfo.pDatatype = "RAW";
	if (!StartDocPrinter(hPrinter,1,(LPBYTE)&docinfo))
	{
		szMsg.Format("StartDocPrinter fail, err=%d", GetLastError());
		MessageBox(szMsg);
	}

	if (!StartPagePrinter(hPrinter))
	{
		szMsg.Format("StartPagePrinter fail, err=%d", GetLastError());
		MessageBox(szMsg);
	}
	DWORD dwWrite;
	int nPJLCommandLen = strlen(pPJLCommand);
	if (WritePrinter(hPrinter,pPJLCommand,nPJLCommandLen,&dwWrite))
	{
		szMsg.Format("��ӡ����д���ӡ��[%s]�ɹ�. nPJLCommandLen=%d,Write=%d", m_szCurPrinterName, nPJLCommandLen, dwWrite);
	}
	else
	{
		szMsg.Format("��ӡ����д���ӡ��[%s]ʧ��.err=%d", m_szCurPrinterName, GetLastError());
	}
	MessageBox(szMsg);

	if (WritePrinter(hPrinter,pTemp,nFileLen,&dwWrite))
	{
		szMsg.Format("PDF�ļ�д���ӡ��[%s]�ɹ�. FileLen=%d,Write=%d", m_szCurPrinterName, nFileLen, dwWrite);
	}
	else
	{
		szMsg.Format("PDF�ļ�д���ӡ��[%s]ʧ��.err=%d", m_szCurPrinterName, GetLastError());
	}
	if (!EndDocPrinter(hPrinter))
	{
		szMsg.Format("EndDocPrinter fail, err=%d", GetLastError());
		MessageBox(szMsg);
	}
	MessageBox(szMsg);

	ClosePrinter(hPrinter);
	if(pTemp)
		delete[] pTemp;

}

void CiPrintMgrTestDlg::OnBnClickedBtnaddjob2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		UpdateData(TRUE);

	m_szPrinterName.Trim();
	m_szDocName.Trim();
	m_szUserName.Trim();

	if(m_szPrinterName.IsEmpty())
	{
		MessageBox("��ӡ��������Ϊ�գ�");
		return;
	}

	if(m_szSplPath.IsEmpty())
	{
		MessageBox("spl·������Ϊ�գ�");
		return;
	}


	HANDLE hPrinter = NULL;

//	char* pJobType = "NT EMF 1.008";
	char* pJobType = "RAW";

	CString m_szCurSPLFile = m_szSplPath;
	CString m_szCurPrinterName = m_szPrinterName;
	CString szMsg;
	szMsg.Format("��ǰ��ӡ�ļ�����%s",m_szCurSPLFile);
	//      AfxMessageBox(szMsg);

	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = pJobType;

	char cPrinterName[100];
	strcpy(cPrinterName,m_szCurPrinterName);
	if(!OpenPrinter(cPrinterName,&hPrinter,&pd ) )
	{
		int nError = GetLastError();
		MessageBox("OpenPrinter fail");
		return ;
	}

	DOC_INFO_1 docinfo;
	docinfo.pDocName = m_szDocName.GetBuffer();
	docinfo.pOutputFile = "";
	docinfo.pDatatype = "RAW";
	DWORD nJobID = StartDocPrinter(hPrinter,1,(LPBYTE)&docinfo);
	if (nJobID <= 0)
	{
		szMsg.Format("StartDocPrinter fail, err=%d", GetLastError());
		MessageBox(szMsg);
		return;
	}

	DWORD dwNeed = 0;
// 	/*
	//----------------������ҵ����---------------------
	if(!GetJob(hPrinter,nJobID,1,NULL,0,&dwNeed))
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			TRACE("GetJobDetailInfo-----GetJob1 error �������Ϊ��%d",GetLastError());
			return ;
		}
	}        
	BYTE *pJobData = new BYTE[dwNeed];
	if(!pJobData)
	return ;
	memset(pJobData,0,dwNeed);
	DWORD dwState = 0;

	if(!GetJob(hPrinter,nJobID,1,pJobData,dwNeed,&dwNeed))//JOB_STATUS_SPOOLING
	{
		TRACE("GetJobDetailInfo-----GetJob2 error �������Ϊ��%d",GetLastError());
		return ;
	}        
	JOB_INFO_1* pJobInfo = (JOB_INFO_1 *)pJobData;
// 	pJobInfo->pDatatype = pJobType;
// 	pJobInfo->TotalPages = 1;
// 	pJobInfo->PagesPrinted = 1;//JOB_STATUS_COMPLETE
	//pJobInfo->pDevMode->dmCopies = 2;//DMCOLOR_COLOR
	if (m_szUserName.GetLength() > 0)
	{
		pJobInfo->pUserName = m_szUserName.GetBuffer();
	}
	if (m_szDocName.GetLength() > 0)
	{
		pJobInfo->pDocument = m_szDocName.GetBuffer();
	}

	//------------------------------------------

	if(!SetJob(hPrinter,nJobID,1,pJobData,0))
	{
		int nError = GetLastError();
		szMsg.Format("SetJob��ҵʧ�ܣ�%d",nError);
		AfxMessageBox(szMsg);
	}
// 	*/

	//----------------------------------------------
	CFile file;
	if(file.Open(m_szCurSPLFile,CFile::modeRead) == 0)
	{
//		AfxMessageBox("�򿪴�ӡ�ļ�ʧ�ܣ�");
		int nError = GetLastError();
		szMsg.Format("�򿪴�ӡ�ļ�ʧ��,err=%d", GetLastError());
		MessageBox(szMsg);
		return ;
	}
	int nFileLen = file.GetLength();
	BYTE* pTemp = new BYTE[nFileLen];
	if(!pTemp)
	{
		return ;
	}

	int nRet = file.Read(pTemp,nFileLen);

	szMsg.Format("Դspl�ļ���:%d,��ȡ:%d", nFileLen, nRet);
	MessageBox(szMsg);


	file.Close();

	DWORD dwWrite = 0;
	if (WritePrinter(hPrinter,pTemp,nFileLen,&dwWrite))
	{
		szMsg.Format("д���ӡ��[%s]���ݳɹ�,Write=%d", m_szCurPrinterName, dwWrite);
	}
	else
	{
		szMsg.Format("д���ӡ��[%s]����ʧ��.err=%d", m_szCurPrinterName, GetLastError());
	}
	MessageBox(szMsg);

	if (!EndDocPrinter(hPrinter))
	{
		szMsg.Format("EndDocPrinter fail, err=%d", GetLastError());
		MessageBox(szMsg);
	}

	ClosePrinter(hPrinter);
	if(pTemp)
		delete[] pTemp;
}
