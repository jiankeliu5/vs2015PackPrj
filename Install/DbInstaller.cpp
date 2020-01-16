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
	m_bShowErr = bShowErr;
	m_bLocalDb = TRUE;
}

CDbInstaller::~CDbInstaller()
{

}

BOOL CDbInstaller::BackupDb(CString sDbName,CString sDbPath)
{
	if (!m_bLocalDb)
	{
		return BackupDb2(sDbName, sDbPath);
	}

	ASSERT(!sDbName.IsEmpty());
	ASSERT(!sDbPath.IsEmpty());

	CString szBackupDir = CCommonFun::GetFileDirByFullPath(sDbPath);

	//设置目录权限,否则可能备份失败
	AddObjectAccess(szBackupDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);

	if (!CheckDbExist(sDbName))
	{
		theLog.Write(_T("!!CDbInstaller::BackupDb,sDbName=%s,数据库不存在。"), sDbName);
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

	//在备份机建立共享文件夹
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

	//添加临时共享用户
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

	//开启’xp_cmdshell’，否则xp_cmdshell失败
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));

	//建立共享信用关系
	sSQL.AppendFormat(_T(" exec master..xp_cmdshell 'net use %s %s /user:%s\\%s' ")
		, szShareDir, szAddUserPassword, CCommonFun::GetLocalIP(), szAddUser);

	//还原数据库
	sSQL.AppendFormat(_T(" EXEC sp_addumpdevice 'disk', 'XabDbBackDisk', '%s' ")
		_T(" BACKUP DATABASE %s TO XabDbBackDisk with format ")
		_T(" exec sp_dropdevice 'XabDbBackDisk' "),szSharePath,sDbName);

	//执行SQL
	bRet = m_ado.ExecuteSQL(sSQL);
	if(!bRet)
	{
		theLog.WriteEx(_T("!!CDbInstaller::BackupDb2,6"));
	}

	//清理现场
	CCommonFun::DelNetShare(szShareName);
	CCommonFun::DeleteUser(szAddUser);

	return bRet;
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
    if(0 == sDbName.CompareNoCase(_T("iSec_PrinterApp")))
    {//App数据库
        sSQL.Format(_T(" Restore Database %s from disk = '%s' ")
            _T(" WITH REPLACE,MOVE '%s' TO '%s\\%s_Data.mdf',")
            _T(" MOVE '%s_log' TO '%s\\%s_Log.ldf'")
            , sDbName, sDbPath
            , sDbName, sInstallPath, sDbName
            , sDbName, sInstallPath, sDbName);    
    }
    else
    {
    	sSQL.Format(_T(" Restore Database %s from disk = '%s' ")
    		_T(" WITH REPLACE,MOVE '%s_Data' TO '%s\\%s.mdf',")
    		_T(" MOVE '%s_log' TO '%s\\%s.ldf'"),
    		sDbName,sDbPath,
    		sDbName,sInstallPath,sDbName,
    		sDbName,sInstallPath,sDbName);
    }

	BOOL BRet = m_ado.ExecuteSQL(sSQL);
	if(!BRet)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB,5,sInstallPath=%s"), sInstallPath);
	}
	return BRet;
}

//参考
//http://blog.csdn.net/cpu_2/article/details/46481735
//http://www.cnblogs.com/iinsnian/archive/2008/06/06/1214821.html
//SQLSERVER服务实例名称:192.168.0.2
//需要备份的数据库名称: a
//备份机器名称（Client端）：192.168.0.3
//备份机用户:zf 密码：123
//备份机域名：domain
//备份机提供备份需求的文件夹:D:\sqlbak
//exec master..xp_cmdshell 'net use  file://192.168.0.3/sqlbak 123 /user:domain\zf'
BOOL CDbInstaller::RestoreDB2(CString sDbName,CString sDbPath)
{
	theLog.Write(_T("CDbInstaller::RestoreDB2,1,sDbName=%s,sDbPaht=%s"), sDbName, sDbPath);

	//获取数据库安装路径
	CString sInstallPath = GetDbInstallPath(sDbName);
	if (sInstallPath.GetLength() <= 0)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,2,GetDbInstallPath fail,sInstallPath=%s"), sInstallPath);
		return FALSE;
	}

	//在备份机建立共享文件夹
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

	//添加临时共享用户
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

	//开启’xp_cmdshell’，否则xp_cmdshell失败
	CString sSQL;
	sSQL.Format(_T(" USE master ")
		_T(" EXEC sp_configure 'show advanced options', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure 'xp_cmdshell', 1 ")
		_T(" RECONFIGURE WITH OVERRIDE ")
		_T(" EXEC sp_configure   'show advanced options', 0 ")
		_T(" RECONFIGURE WITH OVERRIDE "));

	//建立共享信用关系
	sSQL.AppendFormat(_T(" exec master..xp_cmdshell 'net use %s %s /user:%s\\%s' ")
		, szShareDir, szAddUserPassword, CCommonFun::GetLocalIP(), szAddUser);

	//还原数据库
	if(0 == sDbName.CompareNoCase(_T("iSec_PrinterApp")))
	{//App数据库
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

	//执行SQL
	bRet = m_ado.ExecuteSQL(sSQL);
	if(!bRet)
	{
		theLog.Write(_T("!!CDbInstaller::RestoreDB2,7"));
	}

	//清理现场
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
		theLog.Write(_T("读取数据库系统表失败!"));
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
		theLog.Write(_T("读取数据库系统表失败!"));
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
	EnableXpCmdshell();	//启用xp_cmdshell，否则xp_cmdshell失败。
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
	//设置参数
	TCHAR szShortPath[MAX_PATH + 1] = {0};
	CString sParam;
	GetShortPathName(sPath,szShortPath,MAX_PATH + 1);
	if(sPort == _T("1433"))
		sParam.Format(_T(" -S%s -l 300 -d%s -U%s -P%s -i%s"),sIP,sDbName,sUser,sPsw,szShortPath);
	else
		sParam.Format(_T(" -S%s,%s -l 300 -d%s -U%s -P%s -i%s"),sIP,sPort,sDbName,sUser,sPsw,szShortPath);
	
	//获取执行路径
	CString sExePath;
	sExePath.Format(_T("%s\\osql.exe"),g_sDefaultPath);
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


BOOL CDbInstaller::InstallDbByDbbackupFile(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,
										   CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//设置数据库是否在本地
	m_ado.SetConnStr(sDbSvr,_T("master"),sDbUser,sDbPwd,sPort);
	if(!m_ado.Connect())
	{
		theLog.Write(_T("连接数据库失败！"));
		m_sErr = _T("连接数据库失败！");
		return FALSE;
	}
	int nMajor = 0;
	if(!CheckVersion(nMajor))
	{
		theLog.Write(_T("检测数据库版本失败！"));
		m_sErr = _T("检测数据库版本失败！");
		return FALSE;
	}
	//mssql 2000
	if(nMajor == 8)
	{
		//检测sp4
		if(!CheckSP4())
		{
			theLog.Write(_T("数据库没有打SP4补丁！"));
			m_sErr = _T("数据库没有打SP4补丁！");
			return FALSE;
		}
	}
	//sql2005 以上
	else if (nMajor >= 9)
	{

	}
	else
	{
		ASSERT(0);
	}

	//启动代理
	StartSqlAgent();

	//检测数据库还存在
 	if(CheckDbExist(sDbName))
 	{
		CString szBackupDir = CCommonFun::GetFileDirByFullPath(sPath);

		m_sErr = _T("要创建的数据库已经存在，请先备份后，再删除此数据库！");
 		//AfxMessageBox("要创建的数据库已经存在，请先备份后，再删除此数据库！");
		
		if (eType == OIT_QUERY)
		{
			CString sMsg;
			sMsg.Format(_T("系统检测到数据库服务器上已经存在该产品的数据库。\n"
				"建议您先备份数据库，然后再做进一步操作。是否继续？\n\n"
				"如果选择“是”，系统将自动将数据备份到“%s”目录，然后重新安装数据库，原有数据将被删除！\n"
				"如果选择“否”，系统将继续使用现有数据库，原有数据将保留。"), szBackupDir);

			int nId = ::MessageBox(0,sMsg,_T("提示"),MB_ICONQUESTION |MB_YESNO);
			if (nId == IDYES)
			{
				CString sBackPath;
				COleDateTime dt = COleDateTime::GetCurrentTime();
				sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
					dt.GetYear(),
					dt.GetMonth(),
					dt.GetDay(),
					dt.GetHour(),
					dt.GetMinute(),
					dt.GetSecond());
				if(!BackupDb(sDbName,sBackPath))
				{
					return FALSE;
				}
	
				eType = OIT_DELETE;
			}
			else
			{
				eType = OIT_KEEP;
				return TRUE;
			}
		}
		else if (OIT_BACKANDNEW == eType)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}

			eType = OIT_DELETE;
		}
		else if (OIT_KEEP == eType)
		{
			return TRUE;
		}
		else if (OIT_DELETE == eType)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}
		}
 		//return FALSE;
 	}

	//检测数据库还存在,如果存在直接删除
	while(CheckDbExist(sDbName))
	{
		DropDb(sDbName);
		::Sleep(100);
	}

	int nIndex = 0;
	//创建数据库
	while(!CheckDbExist(sDbName) && nIndex < 2)
	{
		::Sleep(100);
		if(!CreateDb(sDbName))
		{
			theLog.Write(_T("创建数据库失败！"));
			m_sErr = _T("创建数据库失败！");
			return FALSE;
		}
		nIndex ++;
		theLog.Write(_T("CheckDbExist CreateDb %s,%d"),sDbName,nIndex);

	}
	//恢复数据库
	if(!RestoreDB(sDbName,sPath))
	{
		theLog.Write(_T("恢复数据库失败！sDbName=[%s], sPath=[%s]"), sDbName, sPath);
		m_sErr = _T("恢复数据库失败！");
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDbInstaller::InstallDbByDbbackupFile2(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,
										   CString sInstanceName,CString sPath,OLDDB_INSTALL_TYPE& eType)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//设置数据库是否在本地
	m_ado.SetConnStr3(sDbSvr,_T("master"),sDbUser,sDbPwd,sInstanceName);
	if(!m_ado.Connect())
	{
		theLog.Write(_T("连接数据库失败！"));
		m_sErr = _T("连接数据库失败！");
		return FALSE;
	}
	int nMajor = 0;
	if(!CheckVersion(nMajor))
	{
		theLog.Write(_T("检测数据库版本失败！"));
		m_sErr = _T("检测数据库版本失败！");
		return FALSE;
	}
	//mssql 2000
	if(nMajor == 8)
	{
		//检测sp4
		if(!CheckSP4())
		{
			theLog.Write(_T("数据库没有打SP4补丁！"));
			m_sErr = _T("数据库没有打SP4补丁！");
			return FALSE;
		}
	}
	//sql2005 以上
	else if (nMajor >= 9)
	{

	}
	else
	{
		ASSERT(0);
	}

	//启动代理
	StartSqlAgent();

	//检测数据库还存在
 	if(CheckDbExist(sDbName))
 	{
		CString szBackupDir = CCommonFun::GetFileDirByFullPath(sPath);

		m_sErr = _T("要创建的数据库已经存在，请先备份后，再删除此数据库！");
 		//AfxMessageBox(_T("要创建的数据库已经存在，请先备份后，再删除此数据库！"));
		
		if (eType == OIT_QUERY)
		{
			CString sMsg;
			sMsg.Format(_T("系统检测到数据库服务器上已经存在该产品的数据库。\n"
				"建议您先备份数据库，然后再做进一步操作。是否继续？\n\n"
				"如果选择“是”，系统将自动将数据备份到“%s”目录，然后重新安装数据库，原有数据将被删除！\n"
				"如果选择“否”，系统将继续使用现有数据库，原有数据将保留。"), szBackupDir);

			int nId = ::MessageBox(0,sMsg,_T("提示"),MB_ICONQUESTION |MB_YESNO);
			if (nId == IDYES)
			{
				CString sBackPath;
				COleDateTime dt = COleDateTime::GetCurrentTime();
				sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
					dt.GetYear(),
					dt.GetMonth(),
					dt.GetDay(),
					dt.GetHour(),
					dt.GetMinute(),
					dt.GetSecond());
				if(!BackupDb(sDbName,sBackPath))
				{
					return FALSE;
				}
	
				eType = OIT_DELETE;
			}
			else
			{
				eType = OIT_KEEP;
				return TRUE;
			}
		}
		else if (OIT_BACKANDNEW == eType)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}

			eType = OIT_DELETE;
		}
		else if (OIT_KEEP == eType)
		{
			return TRUE;
		}
		else if (OIT_DELETE == eType)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}
		}
 		//return FALSE;
 	}

	//检测数据库还存在,如果存在直接删除
	while(CheckDbExist(sDbName))
	{
		DropDb(sDbName);
		::Sleep(100);
	}

	int nIndex = 0;
	//创建数据库
	while(!CheckDbExist(sDbName) && nIndex < 2)
	{
		::Sleep(100);
		if(!CreateDb(sDbName))
		{
			theLog.Write(_T("创建数据库失败！"));
			m_sErr = _T("创建数据库失败！");
			return FALSE;
		}
		nIndex ++;
		theLog.Write(_T("CheckDbExist CreateDb %s,%d"),sDbName,nIndex);

	}
	//恢复数据库
	if(!RestoreDB(sDbName,sPath))
	{
		theLog.Write(_T("恢复数据库失败！sDbName=[%s], sPath=[%s]"), sDbName, sPath);
		m_sErr = _T("恢复数据库失败！");
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDbInstaller::InstallDbBySqlFile(CString sDbSvr,CString sDbName,CString sDbUser,
									  CString sDbPwd,CString sPort,CString sPath,OLDDB_INSTALL_TYPE& eType)
{
	m_bLocalDb = IsLocalDb(sDbSvr);	//设置数据库是否在本地
	m_ado.SetConnStr(sDbSvr,_T("master"),sDbUser,sDbPwd,sPort);
	if(!m_ado.Connect())
	{
		m_sErr = _T("连接数据库失败！");
		theLog.Write(_T("连接数据库失败！"));
		
		return FALSE;
	}

	int nMajor = 0;
	if(!CheckVersion(nMajor))
	{
		theLog.Write(_T("检测数据库版本失败！"));
		m_sErr = _T("检测数据库版本失败！");
		return FALSE;
	}
	//mssql 2000
	if(nMajor == 8)
	{
		//检测sp4
		if(!CheckSP4())
		{
			theLog.Write(_T("数据库没有打SP4补丁！"));
			m_sErr = _T("数据库没有打SP4补丁！");
			return FALSE;
		}
	}
	//sql2005 以上
	else if (nMajor >= 9)
	{

	}
	else
	{
		ASSERT(0);
	}

	//启动代理
	StartSqlAgent();

	if(CheckDbExist(sDbName))
	{
		CString szBackupDir = CCommonFun::GetFileDirByFullPath(sPath);

		theLog.Write(_T("要创建的数据库已经存在，请先备份后，再删除此数据库！"));
		m_sErr = _T("要创建的数据库已经存在，请先备份后，再删除此数据库！");
		if (eType == OIT_QUERY)
		{
			CString sMsg;
			sMsg.Format(_T("系统检测到数据库服务器上已经存在该产品的数据库。\n"
				"建议您先备份数据库，然后再做进一步操作。是否继续？\n\n"
				"如果选择“是”，系统将自动将数据备份到“%s”目录，然后重新安装数据库，原有数据将被删除！\n"
				"如果选择“否”，系统将继续使用现有数据库，原有数据将保留。"), szBackupDir);

			int nId = ::MessageBox(0,sMsg,_T("提示"),MB_ICONQUESTION |MB_YESNO);
			if (nId == IDYES)
			{
				CString sBackPath;
				COleDateTime dt = COleDateTime::GetCurrentTime();
				sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
					dt.GetYear(),
					dt.GetMonth(),
					dt.GetDay(),
					dt.GetHour(),
					dt.GetMinute(),
					dt.GetSecond());
				if(!BackupDb(sDbName,sBackPath))
				{
					return FALSE;
				}

				eType = OIT_DELETE;
			}
			else
			{
				eType = OIT_KEEP;
				return TRUE;
			}
		}
		else if (eType == OIT_BACKANDNEW)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}

			eType = OIT_DELETE;
			
		}
		else if (OIT_KEEP == eType)
		{
			return TRUE;
		}
		else if (OIT_DELETE == eType)
		{
			CString sBackPath;
			COleDateTime dt = COleDateTime::GetCurrentTime();
			sBackPath.Format(_T("%s\\%s_%d%d%d%d%d%d.bak"),szBackupDir,sDbName,
				dt.GetYear(),
				dt.GetMonth(),
				dt.GetDay(),
				dt.GetHour(),
				dt.GetMinute(),
				dt.GetSecond());
			if(!BackupDb(sDbName,sBackPath))
			{
				return FALSE;
			}
		}
		//return FALSE;
	}

	//检测数据库还存在,如果存在直接删除
	while(CheckDbExist(sDbName))
	{
		DropDb(sDbName);
		::Sleep(100);
	}

	//创建数据库
	if(!CreateDb(sDbName))
	{
		theLog.Write(_T("创建数据库失败！"));
		m_sErr = _T("创建数据库失败！");
		return FALSE;
	}

	//脚本初始化数据库
	if(!ExecuteSqlTxt(sDbSvr,sDbName,sDbUser,sDbPwd,sPort,sPath))
	{
		theLog.Write(_T("初始化数据库失败！"));
		m_sErr = _T("初始化数据库失败！");
		return FALSE;
	}
	theLog.Write(_T("数据库安装成功"));
	return TRUE;
}

BOOL CDbInstaller::GetDefaultPort(CString& sPort)
{
	sPort.Empty();
	CRegistryEx reg(2,TRUE);
	CString sRegPath;
	//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\MSSQLServer\MSSQLServer\SuperSocketNetLib\Tcp
	sRegPath.Format(_T("SOFTWARE\\Microsoft\\MSSQLServer\\MSSQLServer\\SuperSocketNetLib\\Tcp"));
	if(reg.Open(sRegPath))
	{

		if(!reg.Read(_T("TcpPort"),sPort))
		{
			
			return FALSE;
		}
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
		{
			
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}