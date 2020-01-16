// WebPageTwo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Install.h"
#include "WebPageThree.h"
#include "BaseSock.h"
#include "WebSheet.h"

// CWebPageTwo �Ի���

IMPLEMENT_DYNAMIC(CWebPageThree, CPropertyPage)

CWebPageThree::CWebPageThree()
	: CPropertyPage(CWebPageThree::IDD)
{
	//{{AFX_DATA_INIT(CWebPageTwo)
	m_szWebPort = _T("21");
	m_bInitSuc = false;
	m_szVirtualPath = _T("");
	//}}AFX_DATA_INIT
	::CoInitialize(NULL);
}

CWebPageThree::~CWebPageThree()
{
	m_Web.RemoveAllList(m_WebList);
	::CoUninitialize();
}

void CWebPageThree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageTwo)
	DDX_Control(pDX, IDC_COMBO_ZHAN, m_ctWeb);
	DDX_Text(pDX, IDC_EDIT_WPORT, m_szWebPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageThree, CPropertyPage)
	//{{AFX_MSG_MAP(CWebPageTwo)
	ON_CBN_SELCHANGE(IDC_COMBO_ZHAN, OnSelchangeComboZhan)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CWebPageTwo ��Ϣ�������
BOOL CWebPageThree::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CPropertySheet *pMyWizard = (CPropertySheet *)GetParent();
	pMyWizard->SetWindowText(_T("Webվ������"));
	// TODO: Add extra initialization here
	if(!InitWebSiteInfo())
	{
		theLog.Write(_T("!!CWebPageThree::OnInitDialog,2,InitWebSiteInfo fail,err=%d"), GetLastError());
		MessageBox(_T("��ʼ��Webվ��ʧ�ܣ�"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWebPageThree::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet *pMyWizard = (CPropertySheet *)GetParent();
	pMyWizard->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}

BOOL CWebPageThree::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class

	theLog.Write(_T("##CWebPageThree::OnWizardFinish,0.1"));

	if(!FinishWebSiteSet())
	{
		theLog.Write(_T("!!CWebPageThree::OnWizardFinish,1,FinishWebSiteSet fail,err=%d"), GetLastError());
		return FALSE;
	}

	theLog.Write(_T("##CWebPageThree::OnWizardFinish,2.1"));

	return CPropertyPage::OnWizardFinish();
}

BOOL CWebPageThree::InitWebSiteInfo()
{
	theLog.Write(_T("##CWebPageThree::InitWebSiteInfo,1"));
	try
	{
		m_Web.GetFtpSiteList(&m_WebList);
		POSITION pos = NULL;
		bool bFirst = true;
		CString str = _T("");
		pos = m_WebList.GetHeadPosition();

		theLog.Write(_T("##CWebPageThree::InitWebSiteInfo,2"));

		while(pos)
		{
			S_SITEINFO* pInfo = (S_SITEINFO*)m_WebList.GetNext(pos);
			str.Format(_T("nNumber:%d   szPort:%s  szIP:%s  szComment:%s   szVPath:%s\n"),
				pInfo->nNumber,pInfo->szPort,pInfo->szIP,pInfo->szComment,pInfo->szVPath);

			CString sTemp = pInfo->szComment;
			if(bFirst)
			{
				bFirst = false;
				m_szWebPort.Format(_T("%s"),pInfo->szPort);
			}
			int nCount = m_ctWeb.GetCount();
			m_ctWeb.AddString(sTemp);
			m_ctWeb.SetItemData(nCount,(DWORD)pInfo);
		}
		int nCount = m_ctWeb.GetCount();

		theLog.Write(_T("##CWebPageThree::InitWebSiteInfo,3"));

		if(nCount>0)
		{
			m_ctWeb.SetCurSel(0);
			UpdateData(false);

		}

		theLog.Write(_T("##CWebPageThree::InitWebSiteInfo,4"));

		if(bFirst)
		{
			m_bInitSuc = false;
			m_ctWeb.EnableWindow(false);
		}
		else
			m_bInitSuc = true;
	}
	catch(...)
	{
		theLog.Write(_T("!!CWebPageThree::InitWebSiteInfo,10,catch sth,err=%d"), GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL CWebPageThree::FinishWebSiteSet(BOOL bQueit)
{
	S_SITEINFO* pWebInfo = NULL;
// 	Enable32BitAppOnWin64(true);
// 	MyExecuteCmdComd();

	theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,1,bQueit=%d"), bQueit);

	if(bQueit)
	{
		m_Web.GetFtpSiteList(&m_WebList);
		POSITION pos = NULL;

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,2"));

		CString str = _T("");
		pos = m_WebList.GetHeadPosition();
		while(pos)
		{
			S_SITEINFO* pInfo = (S_SITEINFO*)m_WebList.GetNext(pos);
			str.Format(_T("nNumber:%d   szPort:%s  szIP:%s  szComment:%s   szVPath:%s\n"),
				pInfo->nNumber,pInfo->szPort,pInfo->szIP,pInfo->szComment,pInfo->szVPath);
			CString sTemp = pInfo->szComment;
			if(pInfo)
			{
				m_bInitSuc = true;
				pWebInfo = pInfo;
				break;
			}
		}
		if (!pWebInfo)
		{
			theLog.Write(_T("!!CWebPageThree::FinishWebSiteSet,10,pWebInfo=%p"), pWebInfo);
			return FALSE;
		}
		int nNum = pWebInfo->nNumber;

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,10.1"));

		StartWebServer(nNum);

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,10.2"));

		BOOL bRet = m_Web.CreateVirtualFtpDir(nNum,m_szVirtualPath,VIRTUALNAME);

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,10.3"));

		if(!bRet)
		{
			theLog.Write(_T("��������Ŀ¼ʧ��!"),_T("��ʾ"));
			theLog.Write(_T("!!CWebPageThree::FinishWebSiteSet,11,CreateVirtualFtpDir fail,err=%d,m_szVirtualPath=%s")
				, GetLastError(), m_szVirtualPath);
			return FALSE;
		}
		else
		{
			StartWebServer(nNum);
			theLog.Write(_T("����Webվ��ɹ���"),_T("��ʾ"));
		}
		
		
	}
	else
	{
		if(!m_bInitSuc)
		{
			MessageBox(_T("��ȷ���Ƿ���ȷ��װWeb������!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			theLog.Write(_T("!!CWebPageThree::FinishWebSiteSet,20,m_bInitSuc=%d"), m_bInitSuc);
			return FALSE;
		}
		UpdateData();
		int nIndex = m_ctWeb.GetCurSel();
		if(nIndex == CB_ERR)
		{
			theLog.Write(_T("!!CWebPageThree::FinishWebSiteSet,21,nIndex=%d"), nIndex);
			return FALSE;
		}

		pWebInfo = (S_SITEINFO*)m_ctWeb.GetItemData(nIndex);
		int nNum = pWebInfo->nNumber;

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,21.2"));

		StartWebServer(nNum);

		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,21.3"));

		BOOL bRet = m_Web.CreateVirtualFtpDir(nNum,m_szVirtualPath,SCAN_DIR);
		
		theLog.Write(_T("##CWebPageThree::FinishWebSiteSet,21.4,bRet=%d"), bRet);

		m_Web.AddOperator();
		m_Web.GrantRightToUser(m_szVirtualPath);

		CWebSheet *pSheet = (CWebSheet *)GetParent();
		if (pSheet)
		{
			pSheet->m_bSuc = TRUE;
			pSheet->WriteTheFtp();
		}

		if(!bRet)
		{
			MessageBox(_T("����Ftp����Ŀ¼ʧ��!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			theLog.Write(_T("!!CWebPageThree::FinishWebSiteSet,22,CreateVirtualFtpDir fail,err=%d,nNum=%d,m_szVirtualPath=%s")
				, GetLastError(), nNum, m_szVirtualPath);
			return FALSE;
		}
		else
		{
			//StartWebServer(nNum);
			MessageBox(_T("����Ftp����Ŀ¼�ɹ���"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		}
	}

	
	return TRUE;
}

void CWebPageThree::OnSelchangeComboZhan() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ctWeb.GetCurSel();
	if(nIndex == CB_ERR)
	{
		theLog.Write(_T("!!CWebPageThree::OnSelchangeComboZhan,1,nIndex=%d"), nIndex);
		return ;
	}

	ShowWebPara(nIndex);
}

void CWebPageThree::ShowWebPara(int index)
{
	S_SITEINFO* pInfo = (S_SITEINFO*)m_ctWeb.GetItemData(index);

	if(pInfo)
	{
		m_szWebPort.Format(_T("%s"),pInfo->szPort);
		UpdateData(false);
	}
}

void CWebPageThree::OnTest() 
{
	// TODO: Add your control notification handler code here
	OnTestIIS();
}

void CWebPageThree::OnTestIIS()
{
	UpdateData();
	int inum=0;
	CBaseSock    ServerSocket;
	int nWebPort = 0;
	nWebPort = _tstoi(m_szWebPort);

#if 1	//delete by zfq,2012.12.11
	if(!ServerSocket.Create() && nWebPort == 0)
#else	//add by zfq,2012.12.11
	if(!ServerSocket.Create() || nWebPort == 0)
#endif
	{
		ServerSocket.m_bTimeOut = true;
		TRACE("�������׽��ִ���ʧ��\n");
		theLog.Write(_T("!!CWebPageThree::OnTestIIS,1,Create fail,err=%d,nWebPort=%d"), GetLastError(), nWebPort);
		MessageBox(_T("�������׽��ִ���ʧ��"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		return;
	}
	ServerSocket.SetTimeOut(6000);
	if(!ServerSocket.Connect(_T("127.0.0.1"),nWebPort))//����ֻ�Ǵ�����80�˿��Ժ����޸��ж˿ں�Ҫ�����ݱ���pBufferSend���еó�
	{
		ServerSocket.m_bTimeOut = true;
		MessageBox(_T("Ftp����������������!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);

		theLog.Write(_T("!!CWebPageThree::OnTestIIS,2,Connect fail,err=%d,nWebPort=%d"), GetLastError(), nWebPort);
	}
	else
	{
		MessageBox(_T("Ftp��������������!"),_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
	}
	
	ServerSocket.Close();
}

void CWebPageThree::MakeHttpCon(char *cinfo, int imax)
{
	ZeroMemory(cinfo,imax);
	strcpy(cinfo, "GET http://127.0.0.1 HTTP/1.0\r\n");
	strcat(cinfo, "Referer: http://127.0.0.1\r\n");
	strcat(cinfo, "Accept-Language: zh-cn\r\n");
	strcat(cinfo, "Accept-Encoding: gzip, deflate\r\n");
	strcat(cinfo, "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n");
	strcat(cinfo, "Host: 127.0.0.1\r\n");
	strcat(cinfo, "Proxy-Connection: Keep-Aliv\r\n");
	strcat(cinfo, "Pragma: no-cache\r\n\r\n");
}


void CWebPageThree::StartWebServer(int nNum)
{
	CString szDefaultPath;
	szDefaultPath.Format(_T("%s\\reg.vbs"),g_sDefaultPath);
	TCHAR cShortPath[300];
	GetShortPathName(szDefaultPath,cShortPath,300);

	CString szPath;
	szPath.Format(_T("CSCRIPT.EXE %s -c localhost -a %d"),cShortPath,nNum);

	STARTUPINFO StartupInfo= {0};
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb=sizeof(STARTUPINFO);	

	if (!CreateProcess(NULL, szPath.GetBuffer(szPath.GetLength()), NULL,NULL,FALSE,0,NULL,NULL, &StartupInfo, &ProcessInfo))
	{
		theLog.Write(_T("create web start process error"));
		theLog.Write(_T("!!CWebPageThree::StartWebServer,2,CreateProcess fail,err=%d,szPath=%s"), GetLastError(), szPath);
		return ;
	}
	szPath.ReleaseBuffer();
	WaitForSingleObject(ProcessInfo.hProcess ,5000);
	CloseHandle (ProcessInfo.hThread);
	CloseHandle (ProcessInfo.hProcess);	
}

