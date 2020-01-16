// DbInstaller.h: interface for the CDbInstaller class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBINSTALLER_H__94193770_7EFA_4C2E_BD83_50019B5AA4BD__INCLUDED_)
#define AFX_DBINSTALLER_H__94193770_7EFA_4C2E_BD83_50019B5AA4BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ado.h"
enum OLDDB_INSTALL_TYPE
{
	OIT_QUERY,		//ѯ��
	OIT_KEEP,		//����
	OIT_DELETE,		//ɾ��
	OIT_BACKANDNEW	//���ݲ��½�
};

class CDbInstaller  
{
public:
	CDbInstaller(BOOL bShowErr = TRUE);
	virtual ~CDbInstaller();

	static BOOL GetDefaultPort(CString& sPort);
	static BOOL GetMsdeListenPort(CString sInstance,CString& sPort);
	BOOL BackupDb(CString sDbName,CString sDbPath);
	BOOL BackupDb2(CString sDbName,CString sDbPath);	//��Զ�����ݿⱸ�ݵ�������bak
	BOOL DropDb(CString sDbName,BOOL bDrop = TRUE);
	BOOL InstallDbBySqlFile(CString sDbSvr,CString sDbName,CString sDbUser,
		CString sDbPwd,CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType);
	BOOL InstallDbByDbbackupFile(CString sDbSvr,CString sDbName,CString sDbUser,
		CString sDbPwd,CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType);
	BOOL InstallDbByDbbackupFile2(CString sDbSvr,CString sDbName,CString sDbUser,
		CString sDbPwd,CString sInstanceName,CString sPath,OLDDB_INSTALL_TYPE& eType);
	BOOL CreateDb(CString sDbName);
	BOOL ExecuteSqlTxt(CString sIP,CString sDbName,CString sUser, CString sPsw, CString sPort, CString sPath);
	BOOL EnableXpCmdshell();	//���á�xp_cmdshell��
	BOOL DisableXpCmdshell();	//�رա�xp_cmdshell��
	BOOL StartSqlAgent();
	BOOL CheckDbExist(CString sDbName);
	BOOL CheckSP4();
	BOOL CheckVersion(int& nMajor);
	CString GetDbInstallPath(CString sDbName);	//��ȡ���ݿⰲװ·��
	BOOL IsLocalDb(CString sDbSvr);	//�ж����ݿ��Ƿ��ڱ���
	BOOL RestoreDB(CString sDbName,CString sDbPath);
	BOOL RestoreDB2(CString sDbName,CString sDbPath);	//�ӱ�����bak��ԭ��Զ�����ݿ�
	BOOL UnIstallDb(CString sIP,CString sDbName,CString sUser, CString sPsw, CString sPort);
	
public:
	CString m_sErr;
protected:
	CAdo m_ado;
	BOOL m_bLocalDb;	//���ݿ��Ƿ��ڱ���
	BOOL m_bShowErr;
	
};

#endif // !defined(AFX_DBINSTALLER_H__94193770_7EFA_4C2E_BD83_50019B5AA4BD__INCLUDED_)
