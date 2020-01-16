// MainThread.cpp : 实现文件
//

#include "stdafx.h"
#include "MainThread.h"

IMPLEMENT_DYNCREATE(CMainThread, CWinThread)

CMainThread::CMainThread()
{
	m_nConnDBTimer = NULL;
	m_nCheckDbCloudDocTimer = NULL;
	m_nCheckConfigChangeTimer = NULL;
	m_pCloudDocDealMgr = NULL;
}

CMainThread::~CMainThread()
{
}

BOOL CMainThread::InitInstance()
{
#ifdef _DEBUG//测试文档转换功能
			DWORD dwPageCount = 0;
			CPrintableDocumentDeal::GetPdfPageCount(_T("f:\\testdoc\\test.pdf"), dwPageCount);
			CPrintableDocumentDeal::GetPdfPageCount(_T("f:\\testdoc\\test123.pdf"), dwPageCount);
			CPrintableDocumentDeal::GetPdfPageCount(_T("f:\\testdoc\\test20151130.pdf"), dwPageCount);
			CPrintableDocumentDeal::GetPdfPageCount(_T("f:\\testdoc\\test-lm-1145.pdf"), dwPageCount);
			CPrintableDocumentDeal::WordToPdf(_T("f:\\testdoc\\test.doc"), _T("f:\\testdoc\\test.doc.pdf"), FALSE);
			CPrintableDocumentDeal::PPTToPdf(_T("f:\\testdoc\\test.pptx"), _T("f:\\testdoc\\test.ppt.pdf"), FALSE);
			CPrintableDocumentDeal::ExcelToPdf(_T("f:\\testdoc\\test.xlsx"), _T("f:\\testdoc\\test.xls.pdf"), FALSE);
#endif

	theLog.Write(_T("CMainThread::InitInstance"));
	if (CDbOperateHelper::InitDataBase())
	{
		GlobalData.LoadConfig();	//加载配置参数
		EnableCheckConfigChange(TRUE);

		CDbOperateHelper helper;
		if (helper.IsNeedDealCloudJob())
		{
			m_pCloudDocDealMgr = new CCloudDocDealMgr;
			m_pCloudDocDealMgr->InitWork();
			EnableCheckDbCloudDoc(TRUE);
		}
		else
		{
			KillThis();
		}
	}
	else
	{
		theLog.Write(_T("CMainThread::InitInstance,2,InitDataBase fail."));
		EnableConnDataBase(TRUE);
	}

	return TRUE;
}

int CMainThread::ExitInstance()
{
	theLog.Write(_T("CMainThread::ExitInstance"));
	EnableCheckDbCloudDoc(FALSE);
	EnableCheckConfigChange(FALSE);
	EnableConnDataBase(FALSE);
	if (m_pCloudDocDealMgr)
	{
		m_pCloudDocDealMgr->Release();
		delete m_pCloudDocDealMgr;
		m_pCloudDocDealMgr = NULL;
	}
	CDbOperateHelper::ReleaseDataBase();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMainThread, CWinThread)
END_MESSAGE_MAP()


//结束线程
void CMainThread::OnEndTh(WPARAM wp, LPARAM lp)
{
	theLog.Write(_T("CMainThread::OnEndTh"));
	ExitInstance();
	AfxEndThread(0);
}

BOOL CMainThread::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_TIMER:
		{
			OnTimer(pMsg->wParam,pMsg->lParam);		
		}
		break;
	case WM_THREAD_EXIT:
		{
			OnEndTh(pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
		break;
	}

	return CWinThread::PreTranslateMessage(pMsg);
}

void CMainThread::OnTimer(WPARAM wp, LPARAM lp)
{
	if (wp == m_nConnDBTimer)
	{
		if (CDbOperateHelper::InitDataBase())
		{
			EnableConnDataBase(FALSE);
			GlobalData.LoadConfig();	//加载配置参数
			EnableCheckConfigChange(TRUE);

			CDbOperateHelper helper;
			if (helper.IsNeedDealCloudJob())
			{
				m_pCloudDocDealMgr = new CCloudDocDealMgr;
				m_pCloudDocDealMgr->InitWork();
				EnableCheckDbCloudDoc(TRUE);
			}
			else
			{
				KillThis();
			}
		}
	}
	else if (wp == m_nCheckDbCloudDocTimer)
	{
		CheckDbCloudDoc();
	}
	else if (wp == m_nCheckConfigChangeTimer)
	{
		GlobalData.LoadConfig();
	}
}

void CMainThread::EnableConnDataBase(BOOL bEnable)
{
	//theLog.Write(_T("CMainThread::EnableConnDataBase,bEnable=%d,m_nConnDBTimer=%u"), bEnable, m_nConnDBTimer);
	if (bEnable)
	{
		if (!m_nConnDBTimer)
		{
			m_nConnDBTimer = ::SetTimer(NULL,0,Conn_DataBase_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nConnDBTimer)
		{
			KillTimer(NULL, m_nConnDBTimer);
			m_nConnDBTimer = NULL;
		}
	}
}

void CMainThread::EnableCheckDbCloudDoc(BOOL bEnable)
{
	//theLog.Write(_T("CMainThread::EnableCheckDbCloudDoc,bEnable=%d,m_nCheckDbCloudDocTimer=%u"), bEnable, m_nCheckDbCloudDocTimer);
	if (bEnable)
	{
		if (!m_nCheckDbCloudDocTimer)
		{
			m_nCheckDbCloudDocTimer = ::SetTimer(NULL,0,Check_Cloud_Doc_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nCheckDbCloudDocTimer)
		{
			KillTimer(NULL, m_nCheckDbCloudDocTimer);
			m_nCheckDbCloudDocTimer = NULL;
		}
	}
}

void CMainThread::EnableCheckConfigChange(BOOL bEnable)
{
	//theLog.Write(_T("CMainThread::EnableCheckConfigChange,bEnable=%d,m_nCheckConfigChangeTimer=%u"), bEnable, m_nCheckConfigChangeTimer);
	if (bEnable)
	{
		if (!m_nCheckConfigChangeTimer)
		{
			m_nCheckConfigChangeTimer = ::SetTimer(NULL,0,Check_Cloud_Config_Change_TimeOut_MS,NULL);
		}
	}
	else
	{
		if (m_nCheckConfigChangeTimer)
		{
			KillTimer(NULL, m_nCheckConfigChangeTimer);
			m_nCheckConfigChangeTimer = NULL;
		}
	}
}


void CMainThread::onCmdFromUi(int nMainCmd, int nSubCmd, void* pData)
{
	theLog.Write(_T("CMainThread::onCmdFromUi,1,nMainCmd=%d,nSubCmd=%d,pData=%p"), nMainCmd, nSubCmd, pData);
}

void CMainThread::CheckDbCloudDoc()
{
	//theLog.Write(_T("CMainThread::CheckDbCloudDoc,1,begin"));

	if (m_pCloudDocDealMgr)
	{
		m_pCloudDocDealMgr->CheckNeedDealCloudJob();
	}

	//theLog.Write(_T("CMainThread::CheckDbCloudDoc,2,end"));
}

void CMainThread::KillThis()
{
	theLog.Write(_T("CMainThread::KillThis,1,begin"));
	BOOL bRet = CThreadMgr::PostThreadMsg(m_nThreadID, WM_THREAD_EXIT, 0, 0);
	theLog.Write(_T("CMainThread::KillThis,2,begin,bRet=%d"), bRet);
}
