// DbBackupConfig.cpp : ����Ӧ�ó��������Ϊ��
#include "stdafx.h"
#include "DbBackupConfig.h"
#include "DBBakSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDbBackupConfigApp
BEGIN_MESSAGE_MAP(CDbBackupConfigApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CDbBackupConfigApp ����
CDbBackupConfigApp::CDbBackupConfigApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

CFileLog theLog;

// Ψһ��һ�� CDbBackupConfigApp ����
CDbBackupConfigApp theApp;

// CDbBackupConfigApp ��ʼ��
BOOL CDbBackupConfigApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	theLog.SetLogName(_T("database"));

	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CString o1;
	AfxExtractSubString(o1,m_lpCmdLine,0,' ');
	o1.MakeLower();
	if(o1 == "-s" || o1 == "/s") 
	{
		CString sDBServerName;
		CString sDBName;
		CString sUserAccount;
		CString sUserPsw;
		CString sPort;
		CString sInstanceName;
		CDBBackFun::GetDatebaseConfig(sDBServerName,sDBName,sPort,sInstanceName,sUserAccount,sUserPsw);
		
		CString sBackDBPath;
		BOOL bAutoBackupOpen;
		BACKTYPE nAutoBackType;
		int nInterval;
		double dAutoBackTime;
		CDBBackFun::GetDBBackConfig(sBackDBPath,bAutoBackupOpen,nAutoBackType,nInterval,dAutoBackTime);

		CString sBackSavePath;
		COleDateTime dt = COleDateTime::GetCurrentTime();
		sBackSavePath.Format(_T("%s%s_%04d-%02d-%02d-%02d-%02d-%02d.bak"),sBackDBPath,sDBName,
			dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
		CDbInstaller dbins;
		if (sInstanceName.IsEmpty())
		{
			if (sPort.IsEmpty())
				dbins.GetDefaultPort(sPort);
			dbins.BackUpDBToFile(sDBServerName,sDBName,sUserAccount,sUserPsw,sPort,sBackSavePath);
		}
		else
		{
			dbins.BackUpDBToFile2(sDBServerName,sDBName,sUserAccount,sUserPsw,sInstanceName,sBackSavePath);
		}
	} 
	else
	{
		CDBBakSheet dlg(_T("���ݿⱸ��/��ԭ"),NULL,NULL);
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
	}
	return FALSE;
}
