// SQliteDataSvc.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SQliteDataSvc.h"
#include <aclapi.h>

#define SQLITE_MUTEX_NAME TEXT("Global\\@#$SQLITEDATASVC$#@%")
CFileLog theLog;
#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)
#define FILE_AND_LINE __FILE__ _T("@") QUOTE(__LINE__)

void Err(LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);
	theLog.Write(_T("Err"));
	theLog.Write(sTmp);
}

void Log(LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);
	theLog.Write(_T("Log"));
	theLog.Write(sTmp);
}

CSQliteDataSvc::CSQliteDataSvc()
{
	m_hDB = NULL;
	m_hMutex = NULL;
	m_dwTimeout = 10 * 1000;
	memset(m_szDbFile,0,MAX_PATH);
	memset(m_szPwd,0,MAX_PATH);
	theLog.SetLogName(_T("sqlite"));
}

CSQliteDataSvc::~CSQliteDataSvc()
{
	Release();
}

CSQliteDataSvc& CSQliteDataSvc::GetInstance()
{
	static CSQliteDataSvc theOne;
	return theOne;
}

BOOL CSQliteDataSvc::CreateDB(TCHAR* szDb)
{
	if(!Connect() || !szDb)
		return FALSE;

	return ExecuteSQL(szDb);
}

void CSQliteDataSvc::InitConnt(TCHAR* szDbFile /*= NULL*/,TCHAR* szPwd/* = NULL*/,DWORD dwTimeout /*= 10 * 1000*/)
{
	ASSERT(szDbFile);
	//ASSERT(szPwd);
	if(szDbFile)
	{
		_tcscpy_s(m_szDbFile,sizeof(m_szDbFile),szDbFile);
	}
	if(szPwd)
	{
		_tcscpy_s(m_szPwd,sizeof(m_szPwd),szPwd);
	}
	m_dwTimeout = dwTimeout;
	Log(_T("CSQliteDataSvc::InitConnt,szDbFile=[%s], version=[%s]"),szDbFile, CCommonFun::MultiByteToUnicode(sqlite3_libversion()));
	
}
BOOL CSQliteDataSvc::Connect()
{
	if(m_hDB && m_hMutex)
		return TRUE;
#if 0
	TCHAR szPath[MAX_PATH];
	if(!GetModuleFileName(GetModuleHandle(_T(_T("SQLiteDataSvc.dll"))), szPath, MAX_PATH ))
		return FALSE;
	TCHAR* psz = (TCHAR*)_tcsrchr(szPath, _T('\\'));
	if(psz)
		_tcscpy_s(psz, MAX_PATH-(psz-szPath)*sizeof(WCHAR), _T("\\ABInfoB.dll")); // 伪装的DB名称
#endif
	//int n = sqlite3_open(m_szDbFile, &m_hDB);
	//int n = sqlite3_open16(L":memory:", &g_db);	// 内存数据库，效率差别非常大，参考profile测试用例
#ifdef UNICODE
	int n = sqlite3_open(CCommonFun::UnicodeToUTF8(m_szDbFile), &m_hDB);	//传入参数是UTF8格式，否则中文路径会打开失败
#else
	int n = sqlite3_open(CCommonFun::MultiByteToUTF8(m_szDbFile), &m_hDB);	//传入参数是UTF8格式，否则中文路径会打开失败
#endif
	if(n!=SQLITE_OK) {
		Err(_T("!!CSQliteDataSvc::Connect,1,open db fail(%s)! err:%d\n"), m_szDbFile, n);
		return FALSE;
	}
	//加密
	if(_tcslen(m_szPwd))
	{
		sqlite3_key(m_hDB,m_szPwd,_tcslen(m_szPwd));
	}

	//sqlite3_exec(m_hDB, "pragma synchronous=off", 0, 0, 0);	//默认是full，关闭后速度有很大提升，利弊见sqlite faq


	if(m_hMutex==NULL && !InitMutex()) 
	{
		Err(_T("!!CSQliteDataSvc::Connect,2,ad_create_mutex失败，err:%d\n"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL CSQliteDataSvc::InitMutex()
{
	DWORD dwRes;
	PSID pEveryoneSID = NULL, pAdminSID = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea[2];
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
		SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	SECURITY_ATTRIBUTES sa;

	// Create a well-known SID for the Everyone group.
	if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		Err(_T("!!CSQliteDataSvc::InitMutex,1,AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow Everyone read access to the key.
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = KEY_ALL_ACCESS|MUTEX_ALL_ACCESS;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance= NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

	// Create a SID for the BUILTIN\Administrators group.
	if(! AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdminSID)) 
	{
		Err(_T("!!CSQliteDataSvc::InitMutex,2,AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup; 
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow the Administrators group full access to
	// the key.
	ea[1].grfAccessPermissions = KEY_ALL_ACCESS|MUTEX_ALL_ACCESS;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance= NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;

	// Create a new ACL that contains the new ACEs.
	dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes) 
	{
		Err(_T("!!CSQliteDataSvc::InitMutex,3,SetEntriesInAcl Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize a security descriptor.  
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
		SECURITY_DESCRIPTOR_MIN_LENGTH); 
	if (NULL == pSD) 
	{ 
		Err(_T("!!CSQliteDataSvc::InitMutex,4,LocalAlloc Error %u\n"), GetLastError());
		goto Cleanup; 
	} 

	if (!InitializeSecurityDescriptor(pSD,
		SECURITY_DESCRIPTOR_REVISION)) 
	{  
		Err(_T("!!CSQliteDataSvc::InitMutex,5,InitializeSecurityDescriptor Error %u\n"),
			GetLastError());
		goto Cleanup; 
	} 

	// Add the ACL to the security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD, 
		TRUE,     // bDaclPresent flag   
		pACL, 
		FALSE))   // not a default DACL 
	{  
		Err(_T("!!CSQliteDataSvc::InitMutex,6,SetSecurityDescriptorDacl Error %u\n"),
			GetLastError());
		goto Cleanup; 
	} 

	// Initialize a security attributes structure.
	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;

	m_hMutex = CreateMutex(&sa, FALSE, SQLITE_MUTEX_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		Log(_T("##CSQliteDataSvc::InitMutex,7,CreateMutex ERROR_ALREADY_EXISTS"));
	}
Cleanup:

	if (pEveryoneSID) 
		FreeSid(pEveryoneSID);
	if (pAdminSID) 
		FreeSid(pAdminSID);
	if (pACL) 
		LocalFree(pACL);
	if (pSD) 
		LocalFree(pSD);

	return m_hMutex!=NULL;
}

void CSQliteDataSvc::Release()
{
	if(m_hDB) 
	{
		sqlite3_close(m_hDB);
		m_hDB=NULL;
	}

	if(m_hMutex) 
	{
		CloseHandle(m_hMutex);
		m_hMutex=NULL;
	}

}

BOOL CSQliteDataSvc::Insert(TCHAR* szSQL)
{
	return 0;
}

BOOL CSQliteDataSvc::ExecuteSQL(TCHAR* szSQL)
{
	if(!Connect() || !szSQL)
		return FALSE;

	int nCode=-1;

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
		for(int i=0; i<10; i++) 
		{
#ifdef UNICODE
			nCode = sqlite3_exec(m_hDB, CCommonFun::UnicodeToUTF8(szSQL), 0, 0, 0);
#else
			nCode = sqlite3_exec(m_hDB, CCommonFun::MultiByteToUTF8(szSQL), 0, 0, 0);
#endif
			if(nCode != SQLITE_BUSY)
				break;

			Sleep(20);

		}
		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::ExecuteSQL,WaitForSingleObject, err:%d\n"), GetLastError());
	}

	if(nCode != SQLITE_OK)
		Err(_T("!!CSQliteDataSvc::ExecuteSQL,exec_sql FAILED! sql:%s code:%d\n"), szSQL, nCode);

	return nCode==SQLITE_OK;
}

BOOL CSQliteDataSvc::GetRecordStream(TCHAR* szSQL,BYTE*& pBuf,int &nBufSize)
{
	if(!szSQL|| !Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	isafetec::datastream data;
	int nRowSize = 0;
	int nColSize = 0;
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code=sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_step(pst);
			if(code==SQLITE_ROW) 
			{
				nColSize = sqlite3_column_count(pst);
				for(int n = 0; n < nColSize; n ++)
				{
					const char* pRs = (const char*)sqlite3_column_text(pst,n);
					data.add_val(pRs);
				}
				nRowSize++;
				
			}
			else if(code != SQLITE_DONE)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,get_value, sqlite3_step, err:%d\n"), code);
			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,get_value, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,get_value, sqlite3_finalize, err:%d\n"), code);
			}
			
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetRecordStream,get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetRecordStream,WaitForSingleObject, err:%d\n"), GetLastError());
	}
	data.set_col(nColSize);
	data.set_row(nRowSize);
	nBufSize = data.ByteSize();
	pBuf = new BYTE[nBufSize];
	ASSERT(pBuf);
	
	if(pBuf)
	{
		memset(pBuf,0,nBufSize);
		bRet = data.SerializeToArray(pBuf,nBufSize);
	}
	return bRet;
}

BOOL CSQliteDataSvc::GetRecordStream(TCHAR* szSQL,isafetec::datastream& ary)
{
	if(!szSQL|| !Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	isafetec::datastream data;
	int nRowSize = 0;
	int nColSize = 0;
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_step(pst);
			if(code==SQLITE_ROW) 
			{
				nColSize = sqlite3_column_count(pst);
				for(int n = 0; n < nColSize; n ++)
				{
					const char* pRs = (const char*)sqlite3_column_text(pst,n);
					data.add_val(pRs);
				}
				nRowSize++;

			}
			else if(code != SQLITE_DONE)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,B,get_value, sqlite3_step, err:%d\n"), code);
			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,B,get_value, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetRecordStream,B,get_value, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetRecordStream,B,get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetRecordStream,B,get_value, WaitForSingleObject, err:%d\n"), GetLastError());
	}
	data.set_col(nColSize);
	data.set_row(nRowSize);
	return bRet;
}

BOOL CSQliteDataSvc::GetSimplePolicy(POLICY_TYPE type,SimplePolicyList& list)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T("select id,last_modified_time from  t_c_policy where type = %d order by id;"),type);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					double dtime = sqlite3_column_double(pst,1);
					SimplePolicy* node = list.add_list();
					node->set_id(nID);
					node->set_type(type);
					node->set_last_time(dtime);
				}
				else
					break;
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetSimplePolicy,get_value, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetSimplePolicy,get_value, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetSimplePolicy,get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetSimplePolicy,get_value, WaitForSingleObject, err:%d\n"), GetLastError());
	}
	return bRet;
}

BOOL CSQliteDataSvc::CheckSimplePolicy(SimplePolicyList* list,SimplePolicyList& rs,SimplePolicyList& rs2)
{
	if(!Connect())
		return FALSE;

	CString sWhere;
	CString sTmp;
	TCHAR szSQL[1024] = {0};
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
		for (int n = 0 ; n < list->list_size(); n++)
		{
			sTmp.Format(_T("%d"),list->list(n).id());
			sWhere += sTmp;
			sWhere += _T(",");
			_stprintf_s(szSQL,sizeof(szSQL),_T(" select last_modified_time from  t_c_policy where id = %d"),list->list(n).id());
		
			pst = NULL;
			pc = NULL;
#ifdef UNICODE
			CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
			CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
			int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
			if(code==SQLITE_OK) 
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{	
					double dtime = sqlite3_column_double(pst,0);
					COleDateTime dtLocal,dtRemote;
					dtLocal.m_dt = dtime;
					dtRemote.m_dt = list->list(n).last_time();
					COleDateTimeSpan sp = dtRemote - dtLocal;
					theLog.Write(_T("CSQliteDataSvc::CheckSimplePolicy,B, %f"),sp.GetTotalSeconds());
					//存在，需要更新
					if(sp.GetTotalSeconds() > 1)
					{
						theLog.Write(_T("CSQliteDataSvc::CheckSimplePolicy,B, need update %d,%f,%f"),list->list(n).id(),dtime,list->list(n).last_time());
						SimplePolicy* node = rs.add_list();
						node->set_id(list->list(n).id());
						node->set_type(list->list(n).type());
					}
					else
					{
						theLog.Write(_T("CSQliteDataSvc::CheckSimplePolicy,B, can use %d"),list->list(n).id());
						//已经是最新的，可以立即生效
						SimplePolicy* node = rs2.add_list();
						node->set_id(list->list(n).id());
						node->set_type(list->list(n).type());
					}
				}
				else if(code == SQLITE_DONE)
				{
					theLog.Write(_T("CSQliteDataSvc::CheckSimplePolicy,B, need get new ,not exist %d"),list->list(n).id());
					//不存在，需要更新
					SimplePolicy* node = rs.add_list();
					node->set_id(list->list(n).id());
					node->set_type(list->list(n).type());
				}
				else
				{
					ASSERT(0);
				}
				code = sqlite3_reset(pst);
				if (code!=SQLITE_OK)
				{
					Err(_T("!!CSQliteDataSvc::GetSimplePolicy,B, sqlite3_reset, err:%d\n"), code);
				}
				code = sqlite3_finalize(pst);
				if (code!=SQLITE_OK)
				{
					Err(_T("!!CSQliteDataSvc::GetSimplePolicy,B, sqlite3_finalize, err:%d\n"), code);
				}
				bRet = TRUE;
			}
			else
			{
				Err(_T("!!CSQliteDataSvc::CheckSimplePolicy,B,get_value, sqlite3_prepare, err:%d\n"), code);
			}
		}

		sWhere.TrimRight(_T(","));
		//清除不需要的策略
		if (!sWhere.IsEmpty())
		{
			_stprintf_s(szSQL,sizeof(szSQL),_T(" delete from t_c_Policy where id not in (%s);"),sWhere);
			ExecuteSQL(szSQL);
		}
		
		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::CheckSimplePolicy,B,get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

//获取本地策略信息
BOOL CSQliteDataSvc::GetPolicy(int nId,BYTE*& pData,int& nSize)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select detail_info from t_c_policy where id = %d; "),nId);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_step(pst);
			if(code == SQLITE_ROW) 
			{
				const void* pTmp = sqlite3_column_blob(pst,0);
				nSize = sqlite3_column_bytes(pst,0);
				if(nSize > 0)
				{
					pData = new BYTE[nSize];
					if(pData)
					{
						memcpy(pData,pTmp,nSize);
					}
				}
				bRet = TRUE;
			}
			else if(code != SQLITE_DONE)
			{
				Err(_T("CSQliteDataSvc::GetPolicy,get_value, step, err:%d\n"), code);
			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetPolicy, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetPolicy, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("CSQliteDataSvc::GetPolicy,get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("CSQliteDataSvc::GetPolicy,get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::UpdatePolicy(int nId,int nType,double time,BYTE* pData,int nSize)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" insert or replace into t_c_policy (id,type,detail_info,last_modified_time) values (%d,%d,@AA , %f)"),nId,nType,time);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout) == WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_bind_blob(pst,1,pData,nSize,0);
			if(code == SQLITE_OK)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_DONE)
				{
					bRet = TRUE;
				}
			}

// 			code = sqlite3_clear_bindings(pst);
// 			if (code!=SQLITE_OK)
// 			{
// 				Err(_T("!!CSQliteDataSvc::UpdatePolicy, sqlite3_clear_bindings, err:%d\n"), code);
// 			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::UpdatePolicy, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::UpdatePolicy, sqlite3_finalize, err:%d\n"), code);
			}
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::UpdatePolicy,get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::UpdatePolicy,get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::UpdateAssets(SYSTEM_INFO_TYPE eType,double time,BYTE* pData,int nSize,int nUploadSuc /*= 1*/)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};

	_stprintf_s(szSQL,sizeof(szSQL),_T(" insert or replace into t_c_assets (type,last_scan_time,desc,Uploadsuc) values (%d,%f,@AA,%d )"),
		eType,time,nUploadSuc);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout) == WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_bind_blob(pst,1,pData,nSize,0);
			if(code == SQLITE_OK)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_DONE)
				{
					bRet = TRUE;
				}
			}
			
			code = sqlite3_finalize(pst);

		}
		else
		{
			Err(_T("get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

//获取硬件信息
BOOL CSQliteDataSvc::GetAssets(SYSTEM_INFO_TYPE eType,BYTE*& pData,int &nSize,BOOL& bUpSucced)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T("select desc,Uploadsuc from t_c_assets where type = %d"),eType);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_step(pst);
			if(code == SQLITE_ROW) 
			{
				const void* pTmp = sqlite3_column_blob(pst,0);
				nSize = sqlite3_column_bytes(pst,0);
				pData = new BYTE[nSize];
				if(pData)
				{
					memcpy(pData,pTmp,nSize);
				}
				bUpSucced = sqlite3_column_int(pst,1);
				bRet = TRUE;
			}
			else if(code != SQLITE_DONE)
			{
				Err(_T("get_value, step, err:%d\n"), code);
			}
			sqlite3_finalize(pst);
			bRet = TRUE;
		}
		else
		{
			Err(_T("get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::CleanAssets()
{
	if(!Connect())
		return FALSE;

	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T("delete from t_c_assets"));
	return ExecuteSQL(szSQL);
}

BOOL CSQliteDataSvc::InsertLog(LOG_TYPE eType,BYTE* pData,int nSize,int &nId)
{
	if(!Connect())
		return FALSE;

	nId = 0;
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" insert into t_c_log (type,data) values (%d,@AA)"),eType);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout) == WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_bind_blob(pst,1,pData,nSize,0);
			if(code == SQLITE_OK)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_DONE)
				{
					bRet = TRUE;
				}
				else
				{
					Err(_T("!!CSQliteDataSvc::InsertLog,B,1 sqlite3_step, err:%d\n"), code);
				}
			}
			else
			{
				Err(_T("!!CSQliteDataSvc::InsertLog,B,2 sqlite3_bind_blob, err:%d\n"), code);
			}

// 			code = sqlite3_clear_bindings(pst);
// 			if (code!=SQLITE_OK)
// 			{
// 				Err(_T("!!CSQliteDataSvc::InsertLog,B,3 sqlite3_clear_bindings, err:%d\n"), code);
// 			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::InsertLog,B,4 sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::InsertLog,B,5 sqlite3_finalize, err:%d\n"), code);
			}
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::InsertLog,B,6 sqlite3_prepare, err:%d\n"), code);
		}

		//插入成功,获取最好插入的id
		if (bRet)
		{
			_stprintf_s(szSQL,sizeof(szSQL),_T(" select id from t_c_log where type = %d order by id desc limit 0,1;"),eType);
#ifdef UNICODE
			CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
			CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
			int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
			if(code==SQLITE_OK) 
			{
				
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					nId = sqlite3_column_int(pst,0);
					
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::InsertLog,B,7,get_value, step, err:%d\n"), code);
					
				}
				code = sqlite3_reset(pst);
				if (code!=SQLITE_OK)
				{
					Err(_T("!!CSQliteDataSvc::InsertLog,B,8 sqlite3_reset, err:%d\n"), code);
				}
				code = sqlite3_finalize(pst);
				if (code!=SQLITE_OK)
				{
					Err(_T("!!CSQliteDataSvc::InsertLog,B,9 sqlite3_finalize, err:%d\n"), code);
				}
				bRet = TRUE;
			}
			else
			{
				Err(_T("!!CSQliteDataSvc::InsertLog,B,10 sqlite3_prepare, err:%d\n"), code);
			}
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::InsertLog,B,11 wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::InsertLog(LOG_TYPE eType,BYTE* pData,int nSize)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" insert into t_c_log (type,data) values (%d,@AA)"),eType);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout) == WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_bind_blob(pst,1,pData,nSize,0);
			if(code == SQLITE_OK)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_DONE)
				{
					bRet = TRUE;
				}
				else
				{
					Err(_T("!!CSQliteDataSvc::InsertLog, sqlite3_step, err:%d\n"), code);
				}
			}
			else
			{
				Err(_T("!!CSQliteDataSvc::InsertLog, sqlite3_bind_blob, err:%d\n"), code);
			}

// 			code = sqlite3_clear_bindings(pst);
// 			if (code!=SQLITE_OK)
// 			{
// 				Err(_T("!!CSQliteDataSvc::InsertLog, sqlite3_clear_bindings, err:%d\n"), code);
// 			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::InsertLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::InsertLog, sqlite3_finalize, err:%d\n"), code);
			}
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::InsertLog, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::InsertLog,  wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::DelLog(LOG_TYPE eType,int nId /*= 0*/)
{
	TCHAR szSQL[1024] = {0};
	if(nId > 0)
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" delete from t_c_log where id = %d and type = %d; "),nId,eType);
	}
	else
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" delete from t_c_log where type = %d; "),eType);
	}
	return ExecuteSQL(szSQL);
}

BOOL CSQliteDataSvc::GetOnePrintLog(SubmitPrintJob* job,int &nId)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	ASSERT(job);
	if (!job)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	if (nId == 0)
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d order by id limit 0,1; "),PRINT_LOG);
	}
	else
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d and id = %d ; "),PRINT_LOG,nId);
	}

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					nId = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					const void* pTmp = sqlite3_column_blob(pst,2);
					int nSize = sqlite3_column_bytes(pst,2);
					
					job->ParseFromArray((BYTE*)pTmp,nSize);

					bRet = TRUE;
				}
				else 
				{
					if (code != SQLITE_DONE)
					{
						Err(_T("!!CSQliteDataSvc::GetOnePrintLog, sqlite3_step, err:%d\n"), code);
					}
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetOnePrintLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetOnePrintLog, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;


		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetOnePrintLog, get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetOnePrintLog, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}
BOOL CSQliteDataSvc::QueryLog(ICltLogCallback* pCall,LOG_TYPE eType)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d; "),eType);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					const void* pTmp = sqlite3_column_blob(pst,2);
					int nSize = sqlite3_column_bytes(pst,2);
					if(pCall )
					{
						pCall->OnLogCallback(nID,nType,pTmp,nSize);
						
					}
					bRet = TRUE;
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::QueryLog, sqlite3_step, err:%d\n"), code);
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::QueryLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::QueryLog, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::QueryLog,get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::QueryLog, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}
int CSQliteDataSvc::GetLogCount(LOG_TYPE eType /*= NULL_LOG*/)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nCount = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	if (eType == NULL_LOG)
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select count(*) from t_c_log ; "));
	}
	else
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select count(*) from t_c_log where type = %d; "),eType);
	}

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					nCount = sqlite3_column_int(pst,0);
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::GetLogCount,get_value, step, err:%d\n"), code);
					break;
				}
			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetLogCount, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetLogCount, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetLogCount,get_value, prepare, err:%d\n"), code);
		}

		
		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetLogCount,get_value, wait, err:%d\n"), GetLastError());
	}

	return nCount;
}
BOOL CSQliteDataSvc::PopOnePrintLog(ICltLogCallback* pCall,int nid,void* context /*= NULL*/)
{

	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d and id = %d order by id ; "),PRINT_LOG,nid);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					const void* pTmp = sqlite3_column_blob(pst,2);
					int nSize = sqlite3_column_bytes(pst,2);
					if(pCall )
					{
						if(pCall->OnLogUpload(nID,nType,pTmp,nSize,context))
						{
							sTmp.Format(_T("%d,"),nID);
							sWhere += sTmp;
						}
					}
					bRet = TRUE;
				}
				else 
				{
					if (code != SQLITE_DONE)
					{
						Err(_T("!!CSQliteDataSvc::PopOnePrintLog,  step, err:%d\n"), code);
					}
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopOnePrintLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopOnePrintLog, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;

		}
		else
		{
			Err(_T("!!CSQliteDataSvc::PopOnePrintLog,  prepare, err:%d\n"), code);
		}

		//更新状态
		theLog.Write(_T("CSQliteDataSvc::PopOnePrintLog, after poplog,then del log where = %s "),sWhere);
		sWhere.TrimRight(_T(","));
		if(!sWhere.IsEmpty())
		{
			int nLen = sWhere.GetLength() + 100;
			TCHAR *pSql = new TCHAR[nLen];
			ASSERT(pSql);
			if (pSql)
			{
				_stprintf_s(pSql,nLen,_T(" delete from t_c_log  where id in (%s);"),sWhere);

				ExecuteSQL(pSql);

				delete pSql;
			}
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("CSQliteDataSvc::PopOnePrintLog, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}
BOOL CSQliteDataSvc::PopOneLog(ICltLogCallback* pCall,LOG_TYPE eType/* = NULL_LOG*/,void* context /*= NULL*/)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	if (eType == NULL_LOG)
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log order by id limit 0,1; "));
	}
	else
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d order by id limit 0,1; "),eType);
	}

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					const void* pTmp = sqlite3_column_blob(pst,2);
					int nSize = sqlite3_column_bytes(pst,2);
					if(pCall )
					{
						if(pCall->OnLogUpload(nID,nType,pTmp,nSize,context))
						{
							sTmp.Format(_T("%d,"),nID);
							sWhere += sTmp;
						}
					}
					bRet = TRUE;
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::PopOneLog, step, err:%d\n"), code);
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopOneLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopOneLog, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;


		}
		else
		{
			Err(_T("!!CSQliteDataSvc::PopOneLog, prepare, err:%d\n"), code);
		}

		//更新状态
		theLog.Write(_T("CSQliteDataSvc::PopOneLog, after poplog,then del log where = %s "),sWhere);
		sWhere.TrimRight(_T(","));
		if(!sWhere.IsEmpty())
		{
			int nLen = sWhere.GetLength() + 100;
			TCHAR *pSql = new TCHAR[nLen];
			ASSERT(pSql);
			if (pSql)
			{
				_stprintf_s(pSql,nLen,_T(" delete from t_c_log  where id in (%s);"),sWhere);

				ExecuteSQL(pSql);

				delete pSql;
			}
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::PopOneLog, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}
BOOL CSQliteDataSvc::PopLog(ICltLogCallback* pCall,LOG_TYPE eType,void* context /*= NULL*/)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	int nRowSize = 0;
	int nColSize = 0;
	TCHAR szSQL[1024] = {0};
	CString sTmp;
	CString sWhere;
	if (eType == NULL_LOG)
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log order by id; "));
	}
	else
	{
		_stprintf_s(szSQL,sizeof(szSQL),_T(" select id, type,data from t_c_log where type = %d order by id; "),eType);
	}
	
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					const void* pTmp = sqlite3_column_blob(pst,2);
					int nSize = sqlite3_column_bytes(pst,2);
					if(pCall )
					{
						if(pCall->OnLogUpload(nID,nType,pTmp,nSize,context))
						{
							sTmp.Format(_T("%d,"),nID);
							sWhere += sTmp;
						}
					}
					bRet = TRUE;
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::PopLog, sqlite3_step, err:%d\n"), code);
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopLog, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::PopLog, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::PopLog, sqlite3_prepare, err:%d\n"), code);
		}

		//更新状态
		theLog.Write(_T("CSQliteDataSvc::PopLog,after poplog,then del log where = %s "),sWhere);
		sWhere.TrimRight(_T(","));
		if(!sWhere.IsEmpty())
		{
			int nLen = sWhere.GetLength() + 100;
			TCHAR *pSql = new TCHAR[nLen];
			ASSERT(pSql);
			if (pSql)
			{
				_stprintf_s(pSql,nLen,_T(" delete from t_c_log  where id in (%s);"),sWhere);

				ExecuteSQL(pSql);

				delete pSql;
			}
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::PopLog, WaitForSingleObject, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::SaveConfig(CString sName,CString sVal,BYTE* pData,int nSize)
{
	if(!Connect())
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" insert or replace into t_c_LocalConfig (name,val,data) values ('%s','%s',@AA)"),sName,sVal);
	if(WaitForSingleObject(m_hMutex,m_dwTimeout) == WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_bind_blob(pst,1,pData,nSize,0);
			if(code == SQLITE_OK)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_DONE)
				{
					bRet = TRUE;
				}
				else
				{
					Err(_T("!!CSQliteDataSvc::SaveConfig, sqlite3_step, err:%d\n"), code);
				}
			}
			else
			{
				Err(_T("!!CSQliteDataSvc::SaveConfig, sqlite3_bind_blob, err:%d\n"), code);
			}

// 			code = sqlite3_clear_bindings(pst);
// 			if (code!=SQLITE_OK)
// 			{
// 				Err(_T("!!CSQliteDataSvc::SaveConfig, sqlite3_clear_bindings, err:%d\n"), code);
// 			}
			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::SaveConfig, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::SaveConfig, sqlite3_finalize, err:%d\n"), code);
			}
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::SaveConfig, get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::SaveConfig, get_value, WaitForSingleObject, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::GetConfigVal(CString sName,CString& sVal)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};

	_stprintf_s(szSQL,sizeof(szSQL),_T(" select val from t_c_LocalConfig where name = '%s'; "),sName);
	

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					char* pVal = (char*)sqlite3_column_text(pst,0);
					sVal.Format(_T("%s"),pVal);

					bRet = TRUE;
					break;
				}
				else 
				{
					Err(_T("get_value, step, err:%d\n"), code);
					break;
				}
			}

			sqlite3_finalize(pst);
			bRet = TRUE;


		}
		else
		{
			Err(_T("get_value, prepare, err:%d\n"), code);
		}

		
		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::GetConfigData(CString sName,CBuffer* pBuf)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select data from t_c_LocalConfig where name = '%s'; "),sName);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					const void* pTmp = sqlite3_column_blob(pst,0);
					int nSize = sqlite3_column_bytes(pst,0);
					pBuf->AddData((BYTE*)pTmp,nSize);
					bRet = TRUE;
					break;
				}
				else 
				{
					Err(_T("get_value, step, err:%d\n"), code);
					break;
				}
			}

			sqlite3_finalize(pst);
			bRet = TRUE;


		}
		else
		{
			Err(_T("get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::GetConfigData(CString sName,BYTE*& pData,int& nSize)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select data from t_c_LocalConfig where name = '%s'; "),sName);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					const void* pTmp = sqlite3_column_blob(pst,0);
					nSize = sqlite3_column_bytes(pst,0);
					pData = new BYTE[nSize];
					ASSERT(pData);
					if (pData)
					{
						CopyMemory(pData,pTmp,nSize);
					}
					bRet = TRUE;
					break;
				}
				else 
				{
					Err(_T("get_value, step, err:%d\n"), code);
					break;
				}
			}

			sqlite3_finalize(pst);
			bRet = TRUE;


		}
		else
		{
			Err(_T("get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}

BOOL CSQliteDataSvc::GetEquipInfo(EQUIP_INFO* pInfo)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	ASSERT(pInfo);
	if (!pInfo)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select data from t_c_LocalConfig where name = '%s'; "),_T("equip"));

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					const void* pTmp = sqlite3_column_blob(pst,0);
					int nSize = sqlite3_column_bytes(pst,0);
					CopyMemory(pInfo,pTmp,nSize);
					bRet = TRUE;
					break;
				}
				else 
				{
					Err(_T("!!CSQliteDataSvc::GetEquipInfo,get_value, sqlite3_step, err:%d\n"), code);
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetEquipInfo, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetEquipInfo, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetEquipInfo,get_value, sqlite3_prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetEquipInfo,get_value, wait, err:%d\n"), GetLastError());
	}

	return bRet;
}
BOOL CSQliteDataSvc::GetProductInfo(CString sName, ProductInfo* pInfo)
{
	if(!Connect() /*|| !pCall*/)
		return FALSE;

	ASSERT(pInfo);
	if (!pInfo)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;

	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select data from t_c_LocalConfig where name = '%s'; "),sName);

	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					const void* pTmp = sqlite3_column_blob(pst,0);
					int nSize = sqlite3_column_bytes(pst,0);
					pInfo->ParseFromArray((BYTE*)pTmp,nSize);
					bRet = TRUE;
					break;
				}
				else 
				{
					if (code != SQLITE_DONE)
					{
						Err(_T("!!CSQliteDataSvc::GetProductInfo,get_value, sqlite3_step, err:%d\n"), code);
					}
					break;
				}
			}

			code = sqlite3_reset(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetProductInfo, sqlite3_reset, err:%d\n"), code);
			}
			code = sqlite3_finalize(pst);
			if (code!=SQLITE_OK)
			{
				Err(_T("!!CSQliteDataSvc::GetProductInfo, sqlite3_finalize, err:%d\n"), code);
			}
			bRet = TRUE;
		}
		else
		{
			Err(_T("!!CSQliteDataSvc::GetProductInfo,get_value, prepare, err:%d\n"), code);
		}

		ReleaseMutex(m_hMutex);
	}
	else
	{
		Err(_T("!!CSQliteDataSvc::GetProductInfo,get_value, WaitForSingleObject, err:%d\n"), GetLastError());
	}

	return bRet;
}
//----------------------------------------------
CSQliteMemDb::CSQliteMemDb()
{
	m_hDB = 0;
	m_hMutex = 0;
	m_dwTimeout = 1000*60;
	m_pInitDb = 0;
}

CSQliteMemDb::~CSQliteMemDb()
{
	Release();
}
void CSQliteMemDb::InitDB(TCHAR* pBuf)
{
	if(m_pInitDb)
	{
		delete m_pInitDb;
	}
	m_pInitDb = new TCHAR[_tcslen(pBuf) + 1];
	ASSERT(m_pInitDb);
	if (m_pInitDb)
	{
		_tcscpy(m_pInitDb,pBuf);
	}

}
BOOL CSQliteMemDb::Connect()
{
	if(m_hDB && m_hMutex)
		return TRUE;
	if (!m_pInitDb)
	{
		ASSERT(0);
		return FALSE;
	}
#if 0
	TCHAR szPath[MAX_PATH];
	if(!GetModuleFileName(GetModuleHandle(_T("SQLiteDataSvc.dll")), szPath, MAX_PATH ))
		return FALSE;
	TCHAR* psz = (TCHAR*)_tcsrchr(szPath, _T('\\'));
	if(psz)
		_tcscpy_s(psz, MAX_PATH-(psz-szPath)*sizeof(WCHAR), _T("\\ABInfoB.dll")); // 伪装的DB名称
#endif
	//int n = sqlite3_open(m_szDbFile, &m_hDB);
	int n = sqlite3_open16(":memory:", &m_hDB);	// 内存数据库，效率差别非常大，参考profile测试用例
	if(n!=SQLITE_OK) {
		Err(_T("open db fail(%s)! err:%d\n"), m_szDbFile, n);
		return FALSE;
	}
	
	sqlite3_exec(m_hDB, "pragma synchronous=off", 0, 0, 0);	//默认是full，关闭后速度有很大提升，利弊见sqlite faq


	if(m_hMutex==NULL && !InitMutex()) 
	{
		Err(_T("ad_create_mutex失败，err:%d\n"), GetLastError());
		return FALSE;
	}
	

	return CreateDB(m_pInitDb);
	return TRUE;
}

BOOL CSQliteMemDb::InitMutex()
{
	DWORD dwRes;
	PSID pEveryoneSID = NULL, pAdminSID = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea[2];
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
		SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	SECURITY_ATTRIBUTES sa;

	// Create a well-known SID for the Everyone group.
	if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		Err(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow Everyone read access to the key.
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = KEY_ALL_ACCESS|MUTEX_ALL_ACCESS;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance= NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

	// Create a SID for the BUILTIN\Administrators group.
	if(! AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdminSID)) 
	{
		Err(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup; 
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow the Administrators group full access to
	// the key.
	ea[1].grfAccessPermissions = KEY_ALL_ACCESS|MUTEX_ALL_ACCESS;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance= NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;

	// Create a new ACL that contains the new ACEs.
	dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes) 
	{
		Err(_T("SetEntriesInAcl Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize a security descriptor.  
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
		SECURITY_DESCRIPTOR_MIN_LENGTH); 
	if (NULL == pSD) 
	{ 
		Err(_T("LocalAlloc Error %u\n"), GetLastError());
		goto Cleanup; 
	} 

	if (!InitializeSecurityDescriptor(pSD,
		SECURITY_DESCRIPTOR_REVISION)) 
	{  
		Err(_T("InitializeSecurityDescriptor Error %u\n"),
			GetLastError());
		goto Cleanup; 
	} 

	// Add the ACL to the security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD, 
		TRUE,     // bDaclPresent flag   
		pACL, 
		FALSE))   // not a default DACL 
	{  
		Err(_T("SetSecurityDescriptorDacl Error %u\n"),
			GetLastError());
		goto Cleanup; 
	} 

	// Initialize a security attributes structure.
	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;
	m_hMutex = CreateMutex(&sa, FALSE, NULL/*SQLITE_MUTEX_NAME*/);

Cleanup:

	if (pEveryoneSID) 
		FreeSid(pEveryoneSID);
	if (pAdminSID) 
		FreeSid(pAdminSID);
	if (pACL) 
		LocalFree(pACL);
	if (pSD) 
		LocalFree(pSD);

	return m_hMutex!=NULL;
}

void CSQliteMemDb::Release()
{
	if(m_hDB) 
	{
		sqlite3_close(m_hDB);
		m_hDB=NULL;
	}

	if(m_hMutex) 
	{
		CloseHandle(m_hMutex);
		m_hMutex=NULL;
	}
	if (m_pInitDb)
	{
		delete m_pInitDb;
		m_pInitDb = 0;
	}
}

BOOL CSQliteMemDb::CreateDB(TCHAR* szSql)
{
	return ExecuteSQL(szSql);
	return TRUE;
}
BOOL CSQliteMemDb::ExecuteSQL(TCHAR* szSQL)
{
	if(!Connect() || !szSQL)
		return FALSE;

	int nCode=-1;
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
		for(int i=0; i<10; i++) 
		{
#ifdef UNICODE
			CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
			CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
			nCode = sqlite3_exec(m_hDB, szaSQL.GetString(), 0, 0, 0);
			if(nCode != SQLITE_BUSY)
				break;
			Sleep(20);
		}
		ReleaseMutex(m_hMutex);
	}

	if(nCode != SQLITE_OK)
	{
		theLog.Write(_T("CSQliteMemDb  ExecuteSQL FAILED! sql:%s code:%d\n"), szSQL, nCode);
	}
	return nCode==SQLITE_OK;
}

BOOL CSQliteMemDb::GetOneData(TCHAR* szSQL,int& nRet)
{
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			code = sqlite3_step(pst);
			if(code == SQLITE_ROW) 
			{
				nRet = sqlite3_column_int(pst,0);
				bRet = TRUE;
			}
			else 
			{
				theLog.Write(_T("CSQliteMemDb GetOneData %s no data"),szSQL);
				nRet = -1;
			}
			sqlite3_finalize(pst);
		}
		else
		{
			theLog.Write(_T("CSQliteMemDb GetOneData %s err"),szSQL);
		}
		ReleaseMutex(m_hMutex);
	}
	return bRet;
}

void CSQliteMemDb::TestInsert()
{
	TCHAR szSql[1024] = {0};
	_stprintf_s(szSql,sizeof(szSql),_T(" insert into t_c_process (id,listtype,name) values (1,1,'aa')"));
	int nCode = ExecuteSQL(szSql);
}

void CSQliteMemDb::TestSelect()
{
	BOOL bRet = FALSE;
	sqlite3_stmt* pst = NULL;
	const char* pc = NULL;
	TCHAR szSQL[1024] = {0};
	_stprintf_s(szSQL,sizeof(szSQL),_T(" select * from t_c_process where name = 'aa'; "));
	if(WaitForSingleObject(m_hMutex,m_dwTimeout)==WAIT_OBJECT_0) 
	{
#ifdef UNICODE
		CStringA szaSQL = CCommonFun::UnicodeToUTF8(szSQL);
#else
		CStringA szaSQL = CCommonFun::MultiByteToUTF8(szSQL);
#endif
		int code = sqlite3_prepare(m_hDB, szaSQL.GetString(), szaSQL.GetLength(), &pst, &pc);
		if(code==SQLITE_OK) 
		{
			while (1)
			{
				code = sqlite3_step(pst);
				if(code == SQLITE_ROW) 
				{
					int nID = sqlite3_column_int(pst,0);
					int nType = sqlite3_column_int(pst,1);
					char* pTmp = (char*)sqlite3_column_text(pst,2);
					bRet = TRUE;
				}
				else 
				{
					//Err(_T("get_value, step, err:%d\n"), code);
					break;
				}
			}

			sqlite3_finalize(pst);
			bRet = TRUE;
		}
		else
		{
			//Err(_T("get_value, prepare, err:%d\n"), code);
		}
		ReleaseMutex(m_hMutex);
	}

}