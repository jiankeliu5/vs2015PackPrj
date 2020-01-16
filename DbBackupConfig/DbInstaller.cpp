// DbInstaller.cpp: implementation of the CDbInstaller class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DbInstaller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbInstaller::CDbInstaller(BOOL bShowErr)
{
	CoInitialize(NULL);
	m_bShowErr = bShowErr;
	m_bLocalDb = TRUE;
}

CDbInstaller::~CDbInstaller()
{
	CoUninitialize();
}

BOOL CDbInstaller::BackupDb(CString sDbName,CString sDbPath)
{
	if (!m_bLocalDb)
	{
		return BackupDb2(sDbName, sDbPath);
	}

	if (!CheckDbExist(sDbName))
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb,sDbName=%s,���ݿⲻ���ڡ�"), sDbName);
		return FALSE;
	}

	CString sSQL;
	sSQL.Format(_T(" EXEC sp_addumpdevice 'disk', 'XabDbBackDisk', '%s' ")
				_T(" BACKUP DATABASE %s TO XabDbBackDisk with format ")
				_T(" exec sp_dropdevice 'XabDbBackDisk' "),sDbPath,sDbName);
	return m_ado.ExecuteSQL(sSQL);
}

BOOL CDbInstaller::BackupDb2(CString sDbName,CString sDbPath)
{
	theLog.Write(_T("CDbInstaller::BackupDb2,1,sDbName=%s,sDbPaht=%s"), sDbName, sDbPath);

	//�ڱ��ݻ����������ļ���
	CString szShareName(_T("TempDataBaseShareDir"));
	CString szShareDir;
	CString szSharePath;
	BOOL bRet = CCommonFun::GetSharePath(sDbPath, szShareName, szShareDir, szSharePath);
	theLog.Write(_T("CDbInstaller::BackupDb2,2,bRet=%d,szSharePath=%s"), bRet, szSharePath);
	if (!bRet)
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb2,3,bRet=%d,szSharePath=%s"), bRet, szSharePath);
		return FALSE;
	}

	//�����ʱ�����û�
	CString szAddUser = _T("XabTempUser");
	CString szAddUserPassword = _T("XabTempPwd123456@*Abc");
	CString szAddUserComment = _T("Xab Temp User, Will Delete.");
	CString szAddUserToGroup = _T("Administrators");
	if (!CCommonFun::AddUser(szAddUser, szAddUserPassword, szAddUserComment))
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb2,4, AddUser, failed, szAddUser=%s"), szAddUser);
	}

	if (!CCommonFun::AddUserToGroup(szAddUser, szAddUserToGroup))
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb2,5, AddUserToGroup, failed, szAddUser=%s,")
			_T("szAddUserToGroup=%s"), szAddUser, szAddUserToGroup);
	}

	//������xp_cmdshell��������xp_cmdshellʧ��
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));

	//�����������ù�ϵ
	sSQL.AppendFormat(_T(" exec master..xp_cmdshell 'net use %s %s /user:%s\\%s' ")
		, szShareDir, szAddUserPassword, CCommonFun::GetLocalIP(), szAddUser);

	//��ԭ���ݿ�
	sSQL.AppendFormat(_T(" EXEC sp_addumpdevice 'disk', 'XabDbBackDisk', '%s' ")
		_T(" BACKUP DATABASE %s TO XabDbBackDisk with format ")
		_T(" exec sp_dropdevice 'XabDbBackDisk' "),szSharePath,sDbName);

	//ִ��SQL
	bRet = m_ado.ExecuteSQL(sSQL);
	if(!bRet)
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb2,6"));
	}

	//�����ֳ�
	CCommonFun::DelNetShare(szShareName);
	CCommonFun::DeleteUser(szAddUser);

	return bRet;
}

// ���ļ���ʼ�����ݿ�
BOOL CDbInstaller::RevertDB(CString sDbName,CString sDBPath)
{
	CString sSQL;
	sSQL.Format(_T("EXEC sp_attach_db @dbname = N'%s',@filename1 = N'%s%s.mdf',@filename2 = N'%s%s.ldf'"),
		sDbName,sDBPath,sDbName,sDBPath,sDbName);
	return m_ado.ExecuteSQL(sSQL);
}

CString CDbInstaller::GetDbInstallPath(CString sDbName)
{
	CString sInstallPath;
	CString sSQL;
	sSQL.Format(_T("select filename from master.dbo.sysdatabases where name = '%s'"),sDbName);
	sInstallPath = m_ado.GetOneString(sSQL);
	theLog.Write(_T("CDbInstaller::GetDbInstallPath,sInstallPath=%s"), sInstallPath);
	int nFind = sInstallPath.ReverseFind('\\');
	if(nFind >= 0)
		sInstallPath = sInstallPath.Left(nFind);
	return sInstallPath;
}

BOOL CDbInstaller::IsLocalDb(CString sDbSvr)
{
	if ((sDbSvr.Find(_T("localhost")) >= 0) || (sDbSvr.Find(_T("127.0.0.1")) >= 0))
	{
		return TRUE;
	}

	CStringArray ary;
	CCommonFun::GetLocalIPList(ary);
	for (int i=0; i<ary.GetCount(); i++)
	{
		if (sDbSvr.Find(ary.GetAt(i)) >= 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDbInstaller::RestoreDB(CString sDbName,CString sDbPath)
{
	if (!m_bLocalDb)
	{
		return RestoreDB2(sDbName, sDbPath);
	}

	CString sInstallPath = GetDbInstallPath(sDbName);
	if (sInstallPath.GetLength() <= 0)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB,GetDbInstallPath fail,sInstallPath=%s"), sInstallPath);
		return FALSE;
	}

	CString sSQL;
	sSQL.Format(_T(" Restore Database %s from disk = '%s' ")
				 _T(" WITH REPLACE,MOVE '%s_Data' TO '%s\\%s.mdf',")
				 _T(" MOVE '%s_log' TO '%s\\%s.ldf'"),
				sDbName,sDbPath,
				sDbName,sInstallPath,sDbName,
				sDbName,sInstallPath,sDbName);

	BOOL BRet = m_ado.ExecuteSQL(sSQL);
	if(!BRet)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB,5,sSQL=%s"), sSQL);
	}
	return BRet;
}

//�ο�
//http://blog.csdn.net/cpu_2/article/details/46481735
//http://www.cnblogs.com/iinsnian/archive/2008/06/06/1214821.html
//SQLSERVER����ʵ������:192.168.0.2
//��Ҫ���ݵ����ݿ�����: a
//���ݻ������ƣ�Client�ˣ���192.168.0.3
//���ݻ��û�:zf ���룺123
//���ݻ�������domain
//���ݻ��ṩ����������ļ���:D:\sqlbak
//exec master..xp_cmdshell 'net use  file://192.168.0.3/sqlbak 123 /user:domain\zf'
BOOL CDbInstaller::RestoreDB2(CString sDbName,CString sDbPath)
{
	theLog.Write(_T("CDbInstaller::RestoreDB2,1,sDbName=%s,sDbPaht=%s"), sDbName, sDbPath);

	//��ȡ���ݿⰲװ·��
	CString sInstallPath = GetDbInstallPath(sDbName);
	if (sInstallPath.GetLength() <= 0)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,2,GetDbInstallPath fail,sInstallPath=%s"), sInstallPath);
		return FALSE;
	}

	//�ڱ��ݻ����������ļ���
	CString szShareName(_T("TempDataBaseShareDir"));
	CString szShareDir;
	CString szSharePath;
	BOOL bRet = CCommonFun::GetSharePath(sDbPath, szShareName, szShareDir, szSharePath);
	theLog.Write(_T("CDbInstaller::RestoreDB2,3,bRet=%d,szSharePath=%s"), bRet, szSharePath);
	if (!bRet)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,4,bRet=%d,szSharePath=%s"), bRet, szSharePath);
		return FALSE;
	}

	//�����ʱ�����û�
	CString szAddUser = _T("XabTempUser");
	CString szAddUserPassword = _T("XabTempPwd123456@*Abc");
	CString szAddUserComment = _T("Xab Temp User, Will Delete.");
	CString szAddUserToGroup = _T("Administrators");
	if (!CCommonFun::AddUser(szAddUser, szAddUserPassword, szAddUserComment))
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,5, AddUser, failed, szAddUser=%s"), szAddUser);
	}

	if (!CCommonFun::AddUserToGroup(szAddUser, szAddUserToGroup))
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,6, AddUserToGroup, failed, szAddUser=%s,")
			_T("szAddUserToGroup=%s"), szAddUser, szAddUserToGroup);
	}

	//������xp_cmdshell��������xp_cmdshellʧ��
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));

	//�����������ù�ϵ
	sSQL.AppendFormat(_T(" exec master..xp_cmdshell 'net use %s %s /user:%s\\%s' ")
		, szShareDir, szAddUserPassword, CCommonFun::GetLocalIP(), szAddUser);

	//��ԭ���ݿ�
	if(0 == sDbName.CompareNoCase(_T("iSec_PrinterApp")))
	{//App���ݿ�
		sSQL.AppendFormat(_T(" Restore Database %s from disk = '%s' ")
			_T(" WITH REPLACE,MOVE '%s' TO '%s\\%s_Data.mdf',")
			_T(" MOVE '%s_log' TO '%s\\%s_Log.ldf'")
			, sDbName, szSharePath
			, sDbName, sInstallPath, sDbName
			, sDbName, sInstallPath, sDbName);    
	}
	else
	{
		sSQL.AppendFormat(_T(" Restore Database %s from disk = '%s' ")
			_T(" WITH REPLACE,MOVE '%s_Data' TO '%s\\%s.mdf',")
			_T(" MOVE '%s_log' TO '%s\\%s.ldf'"),
			sDbName,szSharePath,
			sDbName,sInstallPath,sDbName,
			sDbName,sInstallPath,sDbName);
	}

	//ִ��SQL
	bRet = m_ado.ExecuteSQL(sSQL);
	if(!bRet)
	{
		theLog.WriteEx(_T("!!CDbInstaller::RestoreDB2,7"));
	}

	//�����ֳ�
	CCommonFun::DelNetShare(szShareName);
	CCommonFun::DeleteUser(szAddUser);

	return bRet;
}

BOOL CDbInstaller::CheckVersion(int& nMajor)
{
	CString sSQL;
	sSQL.Format(_T("select Level = serverproperty('ProductVersion')"));

	_variant_t vt;
	vt.Clear();
	BOOL bRet = m_ado.GetOneData(sSQL,vt);
	if(!bRet)
	{
		theLog.Write(_T("��ȡ���ݿ�ϵͳ��ʧ��!"));
		return FALSE;
	}
	CString sVal = CCommonFun::GetVariantStr(vt);
	CStringArray ary;
	CCommonFun::StringSplit(sVal,&ary,'.');
	if (ary.GetCount() > 0)
	{
		nMajor = _tstoi(ary.GetAt(0));
	}
	
	return TRUE;
}

BOOL CDbInstaller::CheckSP4()
{
	CString sSQL;
	sSQL.Format(_T("select Level = serverproperty('productlevel')"));
	
	_variant_t vt;
	vt.Clear();
	BOOL bRet = m_ado.GetOneData(sSQL,vt);
	if(!bRet)
	{
		theLog.Write(_T("��ȡ���ݿ�ϵͳ��ʧ��!"));
		return FALSE;
	}
	CString sVal = CCommonFun::GetVariantStr(vt);
	if(sVal.CompareNoCase(_T("SP4")) != 0)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDbInstaller::CheckDbExist(CString sDbName)
{
	CString sSQL;
	_variant_t vt;
	vt.Clear();
	sSQL.Format(_T("SELECT name FROM master.dbo.sysdatabases WHERE name = N'%s'"),sDbName);
	BOOL bRet = m_ado.GetOneData(sSQL,vt);
	return bRet;
}

BOOL CDbInstaller::EnableXpCmdshell()
{
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));
	return m_ado.ExecuteSQL(sSQL);
}

BOOL CDbInstaller::DisableXpCmdshell()
{
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));
	return m_ado.ExecuteSQL(sSQL);
}

BOOL CDbInstaller::StartSqlAgent()
{
	EnableXpCmdshell();	//����xp_cmdshell������xp_cmdshellʧ�ܡ�
	CString sSQL = _T("exec xp_cmdshell 'net start sqlserveragent'");
	return m_ado.ExecuteSQL(sSQL);
}

BOOL CDbInstaller::ExecuteSqlTxt(CString sIP,CString sDbName,CString sUser, CString sPsw, CString sPort, CString sPath)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	PROCESS_INFORMATION pi;
	memset(&pi,0,sizeof(pi));

	//------------------------
	//���ò���
	TCHAR szShortPath[MAX_PATH + 1] = {0};
	CString sParam;
	GetShortPathName(sPath,szShortPath,MAX_PATH + 1);
	if(sPort == _T("1433"))
		sParam.Format(_T(" -S%s -l 300 -d%s -U%s -P%s -i%s"),sIP,sDbName,sUser,sPsw,szShortPath);
	else
		sParam.Format(_T(" -S%s,%s -l 300 -d%s -U%s -P%s -i%s"),sIP,sPort,sDbName,sUser,sPsw,szShortPath);
	
	//��ȡִ��·��
	CString sExePath;
	sExePath.Format(_T("%sosql.exe"),CCommonFun::GetDefaultPath());
	TCHAR szSqlPath[MAX_PATH + 1] = {0};
	GetShortPathName(sExePath,szSqlPath,MAX_PATH + 1);
	
	//
	BOOL bRet = CreateProcess(szSqlPath,sParam.GetBuffer(0),NULL,NULL,TRUE,0,NULL,NULL,&si,&pi);
	if(bRet == 0)
	{
		return FALSE;
	}
	HANDLE handle = pi.hProcess;
	DWORD dwRet = ::WaitForSingleObject(handle,1000*200);

	if(dwRet == WAIT_OBJECT_0 )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDbInstaller::CreateDb(CString sDbName)
{
	CString sSQL;
	sSQL.Format(_T("create database %s "),sDbName);
	return m_ado.ExecuteSQL(sSQL);
}

BOOL CDbInstaller::UnIstallDb(CString sDbSvr,CString sDbName,CString sUser, CString sPsw, CString sPort)
{
	BOOL bRet = FALSE;
	m_ado.SetConnStr(sDbSvr,_T("master"),sUser,sPsw,sPort);
	if (m_ado.Connect())
	{
		bRet = DropDb(sDbName);
	}
	else
	{
		theLog.Write(_T("UnIstallDb connect faile"));
		ASSERT(0);
	}
	return bRet;
}

BOOL CDbInstaller::DropDb(CString sDbName,BOOL bDrop /* = TRUE*/)
{
	CString sSQL;
	_variant_t varspid;
	BOOL bRet = FALSE;
	sSQL.Format(_T(" select spid from sysprocesses where dbid=db_id('%s') "),sDbName);
	_RecordsetPtr pRs = NULL;
	_bstr_t bsKillSql;
	try
	{
		bRet = m_ado.GetRecordset(sSQL,&pRs);
		if(!bRet || pRs == NULL)
			return FALSE;

		while(!pRs->EndOfFile)
		{
			_variant_t varspid;
			varspid = pRs->Fields->GetItem(_variant_t("spid"))->Value;
			
			bsKillSql = "kill  " + _bstr_t(varspid);
			m_ado.ExecuteSQL(bsKillSql);
			pRs->MoveNext();

		}

		if(bDrop)
		{
			sSQL.Format(_T("DROP Database %s"),sDbName);
			bRet = m_ado.ExecuteSQL(sSQL);
		}
		else
		{
			bRet = TRUE;
		}
	}
	catch (...) 
	{
	}
	return bRet;
}

BOOL CDbInstaller::ConnectDBServer(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString sPort,BOOL bTips)
{
	do 
	{
		m_ado.SetConnStr(sDbSvr,_T("master"),sDbUser,sDbPwd,sPort);
		if(!m_ado.Connect())
		{
			m_sErr = _T("�������ݿ�ʧ�ܣ�");
			break;
		}

		int nMajor = 0;
		if(!CheckVersion(nMajor))
		{
			m_sErr = _T("������ݿ�汾ʧ�ܣ�");
			break;
		}

		//mssql 2000
		if(nMajor == 8 && !CheckSP4())
		{
			m_sErr = _T("���ݿ�û�д�SP4������");
			break;
		}

		//��������
		StartSqlAgent();
		return TRUE;
	} while (0);
	theLog.Write(m_sErr);
	if (bTips)
		::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
	return FALSE;
}

BOOL CDbInstaller::ConnectDBServer2(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString sInstanceName,BOOL bTips)
{
	do 
	{
		m_ado.SetConnStr3(sDbSvr,_T("master"),sDbUser,sDbPwd,sInstanceName);
		if(!m_ado.Connect())
		{
			m_sErr = _T("�������ݿ�ʧ�ܣ�");
			break;
		}

		int nMajor = 0;
		if(!CheckVersion(nMajor))
		{
			m_sErr = _T("������ݿ�汾ʧ�ܣ�");
			break;
		}

		//mssql 2000
		if(nMajor == 8 && !CheckSP4())
		{
			m_sErr = _T("���ݿ�û�д�SP4������");
			break;
		}

		//��������
		StartSqlAgent();
		return TRUE;
	} while (0);
	theLog.Write(m_sErr);
	if (bTips)
		::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
	return FALSE;
}

BOOL CDbInstaller::InstallDbByDbbackupFile(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,
										   CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer(sDbSvr,sDbName,sDbUser,sDbPwd,sPort))
		{
			bRet = FALSE;
			break;
		}

		//������ݿ⻹����
		if(CheckDbExist(sDbName))
		{
			if (eType == OIT_QUERY)
			{
				CString sMsg;
				sMsg.Format(_T("ϵͳ��⵽���ݿ���������Ѿ����ڸò�Ʒ�����ݿ⡣\n"
					"�������ȱ������ݿ⣬Ȼ��������һ���������Ƿ������\n\n"
					"���ѡ���ǡ���ϵͳ���Զ������ݱ��ݵ���C:\\��Ŀ¼��Ȼ�����°�װ���ݿ⣬ԭ�����ݽ���ɾ����\n"
					"���ѡ�񡰷񡱣�ϵͳ������ʹ���������ݿ⣬ԭ�����ݽ�������"));
				int nId = ::MessageBox(0,sMsg,_T("��ʾ"),MB_ICONQUESTION |MB_YESNO);
				if (nId == IDYES)
				{
					CString sBackPath;
					COleDateTime dt = COleDateTime::GetCurrentTime();
					sBackPath.Format(_T("%s%s_%04d-%02d-%02d-%02d-%02d-%02d.bak"),_T("c:\\"),sDbName,
						dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
					if(!BackupDb(sDbName,sBackPath))
					{
						m_sErr.Format(_T("���Ѵ��ڵ����ݿ�\"%s\"���ݵ�\"%s\"ʧ�ܣ�"),sDbName,sBackPath);
						theLog.Write(m_sErr);
						::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
						bRet = FALSE;
						break;
					}
					eType = OIT_DELETE;
				}
				else
				{
					eType = OIT_KEEP;
					break;
				}
			}
			else if (OIT_KEEP == eType)
			{
				break;
			}
			else if (OIT_DELETE == eType)
			{
				CString sBackPath;
				COleDateTime dt = COleDateTime::GetCurrentTime();
				sBackPath.Format(_T("%s%s_%04d-%02d-%02d-%02d-%02d-%02d.bak"),_T("c:\\"),sDbName,
					dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
				if(!BackupDb(sDbName,sBackPath))
				{
					m_sErr.Format(_T("���Ѵ��ڵ����ݿ�\"%s\"���ݵ�\"%s\"ʧ�ܣ�"),sDbName,sBackPath);
					theLog.Write(m_sErr);
					::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
					bRet = FALSE;
					break;
				}
			}
		}

		//������ݿ⻹����,�������ֱ��ɾ��
		while(CheckDbExist(sDbName))
		{
			DropDb(sDbName);
			::Sleep(100);
		}

		//�������ݿ�
		if(!CreateDb(sDbName))
		{
			m_sErr.Format(_T("�������ݿ�\"%s\"ʧ�ܣ�"),sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			bRet = FALSE;
			break;
		}

		//�ָ����ݿ�
		if(!RestoreDB(sDbName,sPath))
		{
			m_sErr.Format(_T("��\"%s\"�ָ����ݿ�\"%s\"ʧ�ܣ�"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("��\"%s\"�ָ����ݿ�\"%s\"�ɹ���"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		}
	} while (0);
	return bRet;
}

BOOL CDbInstaller::InstallDbBySqlFile(CString sDbSvr,CString sDbName,CString sDbUser,
									  CString sDbPwd,CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer(sDbSvr,sDbName,sDbUser,sDbPwd,sPort))
		{
			bRet = FALSE;
			break;
		}

		//������ݿ⻹����
		if(CheckDbExist(sDbName))
		{
			if (eType == OIT_QUERY)
			{
				CString sMsg;
				sMsg.Format(_T("ϵͳ��⵽���ݿ���������Ѿ����ڸò�Ʒ�����ݿ⡣\n"
					"�������ȱ������ݿ⣬Ȼ��������һ���������Ƿ������\n\n"
					"���ѡ���ǡ���ϵͳ���Զ������ݱ��ݵ���C:\\��Ŀ¼��Ȼ�����°�װ���ݿ⣬ԭ�����ݽ���ɾ����\n"
					"���ѡ�񡰷񡱣�ϵͳ������ʹ���������ݿ⣬ԭ�����ݽ�������"));
				int nId = ::MessageBox(0,sMsg,_T("��ʾ"),MB_ICONQUESTION |MB_YESNO);
				if (nId == IDYES)
				{
					CString sBackPath;
					COleDateTime dt = COleDateTime::GetCurrentTime();
					sBackPath.Format(_T("%s%s_%04d-%02d-%02d-%02d-%02d-%02d.bak"),_T("c:\\"),sDbName,
						dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
					if(!BackupDb(sDbName,sBackPath))
					{
						m_sErr.Format(_T("���Ѵ��ڵ����ݿ�\"%s\"���ݵ�\"%s\"ʧ�ܣ�"),sDbName,sBackPath);
						theLog.Write(m_sErr);
						::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
						bRet = FALSE;
						break;
					}
					eType = OIT_DELETE;
				}
				else
				{
					eType = OIT_KEEP;
					break;
				}
			}
			else if (OIT_KEEP == eType)
			{
				break;
			}
			else if (OIT_DELETE == eType)
			{
				CString sBackPath;
				COleDateTime dt = COleDateTime::GetCurrentTime();
				sBackPath.Format(_T("%s%s_%04d-%02d-%02d-%02d-%02d-%02d.bak"),_T("c:\\"),sDbName,
					dt.GetYear(),dt.GetMonth(),dt.GetDay(),dt.GetHour(),dt.GetMinute(),dt.GetSecond());
				if(!BackupDb(sDbName,sBackPath))
				{
					m_sErr.Format(_T("���Ѵ��ڵ����ݿ�\"%s\"���ݵ�\"%s\"ʧ�ܣ�"),sDbName,sBackPath);
					theLog.Write(m_sErr);
					::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
					bRet = FALSE;
					break;
				}
			}
		}

		//������ݿ⻹����,�������ֱ��ɾ��
		while(CheckDbExist(sDbName))
		{
			DropDb(sDbName);
			::Sleep(100);
		}

		//�������ݿ�
		if(!CreateDb(sDbName))
		{
			m_sErr.Format(_T("�������ݿ�\"%s\"ʧ�ܣ�"),sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			bRet = FALSE;
			break;
		}

		//�ű���ʼ�����ݿ�
		if(!ExecuteSqlTxt(sDbSvr,sDbName,sDbUser,sDbPwd,sPort,sPath))
		{
			m_sErr.Format(_T("�ӽű��ļ�\"%s\"�ָ����ݿ�\"%s\"ʧ�ܣ�"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("�ӽű��ļ�\"%s\"�ָ����ݿ�\"%s\"�ɹ���"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(NULL,m_sErr,_T("��ʾ"),MB_OK | MB_ICONINFORMATION);
		}
	} while (0);
	return bRet;
}

// �������ݿ�
BOOL CDbInstaller::BackUpDBToFile(CString sDbSvr,CString sDbName,CString sDbUser,
					CString sDbPwd,CString sPort,CString sPath,BOOL bTips)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		// �������ݿ�
		if (!ConnectDBServer(sDbSvr,sDbName,sDbUser,sDbPwd,sPort,bTips))
		{
			bRet = FALSE;
			break;
		}

		// ������ݿⲻ����
		if(!CheckDbExist(sDbName))
		{
			m_sErr.Format(_T("��Ҫ���ݵ����ݿ�:\"%s\"������!"),sDbName);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}

		// �������ݿ�
		if(!BackupDb(sDbName,sPath))
		{
			m_sErr.Format(_T("�������ݿ�:\"%s\"��\"%s\"ʧ��!"),sDbName,sPath);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("�������ݿ�:\"%s\"��\"%s\"�ɹ�!"),sDbName,sPath);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

// �������ݿ�
BOOL CDbInstaller::BackUpDBToFile2(CString sDbSvr,CString sDbName,CString sDbUser,
					CString sDbPwd,CString sInstanceName,CString sPath,BOOL bTips)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		// �������ݿ�
		if (!ConnectDBServer2(sDbSvr,sDbName,sDbUser,sDbPwd,sInstanceName,bTips))
		{
			bRet = FALSE;
			break;
		}

		// ������ݿⲻ����
		if(!CheckDbExist(sDbName))
		{
			m_sErr.Format(_T("��Ҫ���ݵ����ݿ�:\"%s\"������!"),sDbName);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}

		// �������ݿ�
		if(!BackupDb(sDbName,sPath))
		{
			m_sErr.Format(_T("�������ݿ�:\"%s\"��\"%s\"ʧ��!"),sDbName,sPath);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("�������ݿ�:\"%s\"��\"%s\"�ɹ�!"),sDbName,sPath);
			theLog.Write(m_sErr);
			if (bTips)
				::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

// ��ԭ���ݿ�
BOOL CDbInstaller::RestoreDBFromFile(CString sDbSvr,CString sDbName,CString sDbUser,
					   CString sDbPwd,CString sPort,CString sPath)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer(sDbSvr,sDbName,sDbUser,sDbPwd,sPort))
		{
			bRet = FALSE;
			break;
		}
		if (!DropDb(sDbName,FALSE))
		{
			bRet = FALSE;
			break;
		}

		//�ָ����ݿ�
		if(!RestoreDB(sDbName,sPath))
		{
			m_sErr.Format(_T("��\"%s\"��ԭ���ݿ�:\"%s\"ʧ��!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("��\"%s\"��ԭ���ݿ�:\"%s\"�ɹ�!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

// ��ԭ���ݿ�
BOOL CDbInstaller::RestoreDBFromFile2(CString sDbSvr,CString sDbName,CString sDbUser,
					   CString sDbPwd,CString sInstanceName,CString sPath)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer2(sDbSvr,sDbName,sDbUser,sDbPwd,sInstanceName))
		{
			bRet = FALSE;
			break;
		}
		if (!DropDb(sDbName,FALSE))
		{
			bRet = FALSE;
			break;
		}

		//�ָ����ݿ�
		if(!RestoreDB(sDbName,sPath))
		{
			m_sErr.Format(_T("��\"%s\"��ԭ���ݿ�:\"%s\"ʧ��!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("��\"%s\"��ԭ���ݿ�:\"%s\"�ɹ�!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

// ��ʼ���ݿ�
BOOL CDbInstaller::RevertDBFromFile(CString sDbSvr,CString sDbName,CString sDbUser,
					  CString sDbPwd,CString sPort,CString sPath)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer(sDbSvr,sDbName,sDbUser,sDbPwd,sPort))
		{
			bRet = FALSE;
			break;
		}

		//�ָ����ݿ�
		if(!RevertDB(sDbName,sPath))
		{
			m_sErr.Format(_T("��\"%s\"��ʼ���ݿ�:\"%s\"ʧ��!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("��\"%s\"��ʼ���ݿ�:\"%s\"�ɹ�!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

// ��ʼ���ݿ�
BOOL CDbInstaller::RevertDBFromFile2(CString sDbSvr,CString sDbName,CString sDbUser,
					  CString sDbPwd,CString sInstanceName,CString sPath)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//�������ݿ��Ƿ��ڱ���

	BOOL bRet = TRUE;
	do 
	{
		if (!ConnectDBServer2(sDbSvr,sDbName,sDbUser,sDbPwd,sInstanceName))
		{
			bRet = FALSE;
			break;
		}

		//�ָ����ݿ�
		if(!RevertDB(sDbName,sPath))
		{
			m_sErr.Format(_T("��\"%s\"��ʼ���ݿ�:\"%s\"ʧ��!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
			bRet = FALSE;
			break;
		}
		else
		{
			m_sErr.Format(_T("��\"%s\"��ʼ���ݿ�:\"%s\"�ɹ�!"),sPath,sDbName);
			theLog.Write(m_sErr);
			::MessageBox(0,m_sErr,_T("��ʾ"),MB_ICONINFORMATION |MB_OK);
		}
	} while (0);
	return bRet;
}

BOOL CDbInstaller::GetDefaultPort(CString& sPort)
{
	sPort.Empty();
	CRegistryEx reg(2,TRUE);
	CString sRegPath;
	sRegPath.Format(_T("SOFTWARE\\Microsoft\\MSSQLServer\\MSSQLServer\\SuperSocketNetLib\\Tcp"));
	if(reg.Open(sRegPath))
	{
		if(!reg.Read(_T("TcpPort"),sPort))			
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}

BOOL CDbInstaller::GetMsdeListenPort(CString sInstance,CString& sPort)
{
	sPort.Empty();
	CRegistryEx reg(2,TRUE);
	CString sRegPath;
	sRegPath.Format(_T("SOFTWARE\\Microsoft\\Microsoft SQL Server\\%s\\MSSQLServer\\SuperSocketNetLib\\Tcp"),sInstance);
	if(reg.Open(sRegPath))
	{
		if(!reg.Read(_T("TcpPort"),sPort))			
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}