// CommonFun.cpp: implementation of the CCommonFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <comutil.h>
#include "CommonFun.h"
#include <Shobjidl.h>
#include <intshcut.h>
#include "shlwapi.h"
#include "atlbase.h"


#include "Lm.h"
#pragma comment(lib, "Netapi32.lib")

//RunProcessByGUISessionId�ӿ��ڲ���Ҫ�õ�Wtsapi32.lib��Userenv.lib
#include <UserEnv.h>
#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#define myHeapAlloc(x) (HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,x))

typedef BOOL (WINAPI *SetSecurityDescriptorControlFnPtr)(
	IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
	IN SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
	IN SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet);

typedef BOOL (WINAPI *AddAccessAllowedAceExFnPtr)(
	PACL pAcl,
	DWORD dwAceRevision,
	DWORD AceFlags,
	DWORD AccessMask,
	PSID pSid
	);


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommonFun::CCommonFun()
{

}

CCommonFun::~CCommonFun()
{

}

/*****************************************************************************
Author		:	zhouhonglei
CopyRight	:	zhouhonglei 2002
������		:	Split( CString str, CStringArray *psArray , 
					char chDelimiter=' ', int nCount=-1 )
˵��		:	�ָ��ַ�������
����		:	
str			:	�������,��ѡ�
				�ַ������ʽ���������ַ����ͷָ�����
				��� str Ϊ�㳤���ַ�����Split ���ؿ����飬��������Ԫ�غ����ݵ����顣
psArray:		��������,���طָ����ַ������飬���а���ָ����Ŀ�����ַ���.
chDelimiter	:	��ѡ����ڱ�ʶ���ַ������޵��ַ������ʡ�ԣ�ʹ�ÿո� (",") ��Ϊ�ָ�����		
nCount		:	��ѡ������ص����ַ�����Ŀ��-1 ָʾ�����������ַ�����

����ֵ		:	void
*******************************************************************************/
//��ȡ��ǰ���session ID
DWORD CCommonFun::WTSGetActiveConsoleSessionIdEx()
{
	HMODULE hModule = ::LoadLibrary(_T("Kernel32.dll"));
	typedef DWORD (WINAPI* PFNWTSGetActiveConsoleSessionId)(VOID);
	PFNWTSGetActiveConsoleSessionId m_fnWTSGetActiveConsoleSessionId = NULL;
	if(hModule)
	{
		m_fnWTSGetActiveConsoleSessionId = 
			(PFNWTSGetActiveConsoleSessionId)::GetProcAddress(hModule,"WTSGetActiveConsoleSessionId");
		::FreeLibrary(hModule);
	}
	if (m_fnWTSGetActiveConsoleSessionId)
		return m_fnWTSGetActiveConsoleSessionId();
	return 0;
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL CCommonFun::IsWow64()
{
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")),"IsWow64Process");

	BOOL bIsWow64 = FALSE;
	if (NULL != fnIsWow64Process)
	{
		//AfxMessageBox(_T("aa"));
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
			//AfxMessageBox(_T("errr"));
		}
	}
	return bIsWow64;
}

//��ȡ���̵�session ID
DWORD CCommonFun::GetProcessSessionID(DWORD dwPID)
{
	DWORD dSessionID(0);
	HANDLE Kenelhandle=::LoadLibrary(_T("kernel32.dll"));
	if(Kenelhandle)
	{		
		BOOL (WINAPI *HandleProcessIdToSessionId)(DWORD,DWORD *);		
		HandleProcessIdToSessionId =(BOOL(WINAPI *)(DWORD,DWORD *))GetProcAddress((HMODULE)Kenelhandle,"ProcessIdToSessionId");
		if(HandleProcessIdToSessionId)
		{
			HandleProcessIdToSessionId(dwPID,&dSessionID);
		}
		FreeLibrary((HMODULE)Kenelhandle);
		Kenelhandle =NULL;
	}
	return dSessionID;
}

BOOL CCommonFun::IsProcAlive(DWORD dwProID)	//add by zfq,�ж�ĳ�������Ƿ����
{
	BOOL BRet = FALSE;
	HANDLE hProc = NULL;
	try
	{
		hProc = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProID);
	}
	catch (...)
	{
		ASSERT(0);
		CCommonFun::WriteFileLog(_T("CCommonFun::IsProcAlive(), dwProID=%u, OpenProcess() catch"), dwProID);
		return FALSE;
	}

	if(!hProc)
	{
		return FALSE;
	}

	DWORD dwExitCode = 0;
	try
	{
		BRet = ::GetExitCodeProcess(hProc, &dwExitCode);
	}
	catch (...)
	{
		ASSERT(0);
		CCommonFun::WriteFileLog(_T("CCommonFun::IsProcAlive(), dwProID=%u, GetExitCodeProcess() catch"), dwProID);
		BRet = FALSE;
	}

	if(BRet)
	{
		BRet = (STILL_ACTIVE == dwExitCode);
	}

	::CloseHandle(hProc);
	return BRet;
}
void CCommonFun::StringSplit(CString str, CStringArray *psArray, char chDelimiter, int nCount)
{
	CString sTmp=_T("");
	if( str.IsEmpty( ) )
	{
		return;
	}
	for( int i=0 ; i<str.GetLength( ) ; i++ )
	{
		if( str.GetAt( i ) != chDelimiter )
		{
			sTmp += str.GetAt( i );
		}
		else
		{
			if( nCount == 0 || nCount == 1 )
				break;

			psArray->Add( sTmp );
			sTmp=_T("");
		}
		if( nCount > 1  )
		{
			if( psArray->GetSize( ) >= nCount-1 )
			{
				sTmp = str.Right( str.GetLength( ) - i - 1 );
				break;
			}			
		}
	}	
	if( nCount != 0 )
		psArray->Add(sTmp);	
}

void CCommonFun::StringSplit(const CString sSource, CStringArray *parArray , CString sSplitter,BOOL bTriNUll)
{
	int nSplitLen = sSplitter.GetLength();

	CString sTemp(sSource);
	if(sTemp.IsEmpty())
		return ;
	while(sTemp)
	{
		int nSite = sTemp.Find(sSplitter);
		if(nSite == -1)
		{
			parArray->Add(sTemp);
			break;
		}

		CString sNew = sTemp.Left(nSite);
		if(bTriNUll)
		{
			sNew.TrimLeft(_T(" "));
			sNew.TrimLeft(_T("\t"));
			sNew.TrimRight(_T(" "));
			sNew.TrimRight(_T("\t"));
		}
		parArray->Add(sNew);

		sTemp = (LPCTSTR)sTemp + nSplitLen +nSite;
	}
}


void CCommonFun::WriteFileLog(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteFileLog(sTmp);
}
extern BOOL g_bIsDebug ;
void CCommonFun::WriteFileLog(CString sLog)
{
	if( sLog.GetLength() > 250 )
		TRACE(sLog.Left(250));
	else
		TRACE(sLog);
	TRACE("\n");




	CString sSysDir = GetLogDir();

	CFileFind fd;

	if( !fd.FindFile(sSysDir) )
	{
		if( !::CreateDirectory(sSysDir,NULL) )
		return;
	}

	CString sFileName;
	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));
	sFileName.Format(_T("%s\\Install_%s.Log"),sSysDir,sDate);
	
	try
	{
#ifdef UNICODE
		BOOL bFileExsit = PathFileExists(sFileName);
#endif // UNICODE

		CFile fileLog;
		if(!fileLog.Open(sFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
			return;
		
#ifdef UNICODE
		if (!bFileExsit)
		{
			// �´�������־�ļ�����д��Unicodeͷ
			BYTE chUnicodeHead[2] = { 0xff, 0xfe }; // Unicodeͷ
			fileLog.Write(chUnicodeHead, 2);
			fileLog.Flush();
		}
#endif // UNICODE

		CString sTime = time.Format( _T("%Y-%m-%d %H:%M:%S") );
		CString sTmp;
		
		sTmp.Format(_T("%s,	 %s"),sTime,sLog);
		sTmp += _T("\r\n");
		fileLog.SeekToEnd();
		fileLog.Write(sTmp,sTmp.GetLength()*sizeof(TCHAR));
		fileLog.Close();
	}
	catch (...) 
	{
		;
	}

}

void CCommonFun::ClearArrayElement(CObArray *pAry)
{
	if( pAry != NULL )
	{
		for( int i=0; i<pAry->GetSize(); i++ )
		{
			delete pAry->GetAt(i);
		}
		pAry->RemoveAll();
	}
	pAry = NULL;
}


BOOL CCommonFun::SHGetSpecialPath(int nFolder,CString &sPath)
{
	::CoInitialize(NULL);
	BOOL	bRet = TRUE;
	try
	{
		LPITEMIDLIST pidl;	
		TCHAR	szBuf[MAX_PATH] = {0};

		HRESULT hRet = NOERROR;
		hRet = SHGetSpecialFolderLocation(NULL, nFolder, &pidl);	
		bRet = ( hRet == NOERROR ) ? SHGetPathFromIDList(pidl, szBuf) : FALSE;
		sPath.Format(_T("%s"),szBuf);
	}
	catch(...)
	{
	}
	::CoUninitialize();
	return bRet;
}

BOOL CCommonFun::CreateDir(CString sPath)
{
	sPath.TrimRight(_T("\\"));
	CFileFind fd;
	if( fd.FindFile(sPath) )
	{
		return TRUE;
	}
	CStringArray sAry;
	StringSplit(sPath,&sAry,'\\');
	
	if( sAry.GetSize() < 2 )
	{
		TRACE("������һ������·����");
		return FALSE;
	}

	CString sDriver = sAry.GetAt(0);
	if( sDriver.GetLength() != 2 || sDriver.GetAt(sDriver.GetLength()-1) != ':' )
	{
		TRACE("������һ�����̷���·����");
		return FALSE;;
	}
	CString sDir=sAry.GetAt(0);
	for( int i= 1;i<sAry.GetSize();i++)
	{
		sDir += _T("\\") +sAry.GetAt(i) ;
		CFileFind fd;
		if( fd.FindFile(sDir) )
		{
			fd.FindNextFile();
			if (fd.IsDirectory())
			{				
				continue;			
			}		
		}

		
		if( !CreateDirectory(sDir,NULL) )
		{
			//TRACE("����Ŀ¼"+sDir+"ʧ�ܣ�");
			return FALSE;
		}
	}
	//TRACE("����Ŀ¼"+sDir+"�ɹ���");
	return TRUE;
}

BOOL CCommonFun::IsWin2003()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwMinorVersion == 2 )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CCommonFun::IsWin9X()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		return TRUE;
	}
	return FALSE;
}

PLATFORM CCommonFun::GetPlatform()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osvi))
		return UNKNOWN;

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		return WIN_9X;
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion == 4)
			return WIN_NT;
		if((osvi.dwMajorVersion == 5)&&(osvi.dwMinorVersion==0))
			return WIN_2K;
		if((osvi.dwMajorVersion == 5)&&(osvi.dwMinorVersion==1))
			return WIN_XP;
		if((osvi.dwMajorVersion == 5)&&(osvi.dwMinorVersion==2))
			return WIN_2003;
	}
	return UNKNOWN;
}

CString CCommonFun::ChangePathToShort(CString sPath)
{
	TCHAR chShortPathName[MAX_PATH];
	DWORD dw = GetShortPathName(sPath,chShortPathName,MAX_PATH);
	DWORD dwErr = GetLastError();
	CString sTmp = chShortPathName;

	return sTmp;
}


CString CCommonFun::GetLastErrorInfo()
{
	CString sInfo;
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	
	// Display the string.
	sInfo = (LPTSTR)lpMsgBuf;

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return sInfo;
}
BOOL CCommonFun::IsWinVistaOrLater()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwMajorVersion >= 6)
	{
		return TRUE;
	}
	return FALSE;
}

//-----------add by zhandongbao 2005.12.7--------------
//-----------���Ŀǰ�������е�·��
CString CCommonFun::GetDefaultPath()
{
	TCHAR path[MAX_PATH];
	CString sDefaultPath;
	if(GetModuleFileName(0,path,MAX_PATH)==0) 
		return _T("");
	TCHAR *pdest;
	pdest=_tcsrchr(path,_T('\\'));
	if(pdest) 
		*pdest=0;
	sDefaultPath=path;
	sDefaultPath+=_T("\\");
	return sDefaultPath;
}

//------------------�жϵ�ǰϵͳ�Ƿ�ΪNTϵͳ
BOOL CCommonFun::IsWinNT()
{
	OSVERSIONINFO osv;
	ZeroMemory(&osv,sizeof(OSVERSIONINFO));

	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		return TRUE;
	}
	return FALSE;
}


/******************************
 * Enable/Disable privilege.  *
 * Called with SE_DEBUG_NAME. *
 ******************************/
BOOL CCommonFun::EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable)
{
	HANDLE				hToken;
	TOKEN_PRIVILEGES	tp;
	LUID				luid;
	BOOL				ret;

	if (!OpenProcessToken(GetCurrentProcess(),
					      TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ,
					      &hToken))
		return FALSE;

	if (!LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid))
		return FALSE;

	tp.PrivilegeCount           = 1;
	tp.Privileges[0].Luid       = luid;
	tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	ret = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

	CloseHandle(hToken);

	return ret;
}

DWORD CCommonFun::GetSystemPageSize()
{
	SYSTEM_INFO Info = {0};
	GetSystemInfo(&Info);
	return Info.dwPageSize;
}

BOOL CCommonFun::IsAdmin()
{
    HANDLE                   hAccessToken;
    BYTE                     *InfoBuffer;
    PTOKEN_GROUPS            ptgGroups;
    DWORD                    dwInfoBufferSize;
    PSID                     psidAdministrators;
    SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
    UINT                     i;
    BOOL                     bRet = FALSE;

    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hAccessToken))
       goto cleanup;
    
    InfoBuffer = new BYTE[1024];
    if(!InfoBuffer)
       goto cleanup;

    bRet = GetTokenInformation(hAccessToken,
                               TokenGroups,
                               InfoBuffer,
                               1024,
                               &dwInfoBufferSize);

    CloseHandle( hAccessToken );

    if(!bRet)
       goto cleanup;

    if(!AllocateAndInitializeSid(&siaNtAuthority,
                                 2,
                                 SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS,
                                 0,0,0,0,0,0,
                                 &psidAdministrators))
       goto cleanup;

    bRet = FALSE;

    ptgGroups = (PTOKEN_GROUPS)InfoBuffer;

    for(i=0;i<ptgGroups->GroupCount;i++)
    {
        if(EqualSid(psidAdministrators,ptgGroups->Groups[i].Sid))
        {
            bRet = TRUE;
            break;
        }
    }

    FreeSid(psidAdministrators);

cleanup:

    if (InfoBuffer)
        delete InfoBuffer;

    return bRet;
}

BOOL CCommonFun::StrToTime(CString &sStr, CTime &tm)
{
	if(sStr.Find(_T(".")) > 0)
	{
		CStringArray ary;
		StringSplit(sStr,&ary,_T("."));
		if(ary.GetSize() != 3)
			return FALSE;
		int nYear = _tstoi(ary.GetAt(0));
		int nMonth = _tstoi(ary.GetAt(1));
		int nDay = _tstoi(ary.GetAt(2));
		
		CTime tmp(nYear,nMonth,nDay,0,0,0);
		tm = tmp;
		return TRUE;
	}
	CString sDate,
			sTime;
	int nIndex = sStr.Find(_T(" "),0);
	sDate = sStr.Left(nIndex);
	sTime = sStr.Right(sStr.GetLength() - nIndex - 1);

	CStringArray ary;
	StringSplit(sDate,&ary,_T("-"));
	if(ary.GetSize() != 3)
		return FALSE;
	int nYear = _tstoi(ary.GetAt(0));
	int nMonth = _tstoi(ary.GetAt(1));
	int nDay = _tstoi(ary.GetAt(2));
	
	ary.RemoveAll();
	StringSplit(sTime,&ary,_T(":"));
	if(ary.GetSize() != 3)
		return FALSE;
	int nHour = _tstoi(ary.GetAt(0));
	int nMinute = _tstoi(ary.GetAt(1));
	int nSecond = _tstoi(ary.GetAt(2));
	CTime tmp(nYear,nMonth,nDay,nHour,nMinute,nSecond);
	tm = tmp;
	return TRUE;
}

BOOL CCommonFun::Is256ColorSupported()
{
	BOOL bRetval = FALSE;

	// return TRUE if screen deivce supports 256 colors or better

	HDC hdc = GetDC(NULL);
	if (hdc != NULL)
	{
		if(GetDeviceCaps(hdc, BITSPIXEL) >= 8)
			bRetval = TRUE;
		ReleaseDC(NULL, hdc);
	}

	return bRetval;
}

CString CCommonFun::GetVariantStr(_variant_t varValue)
{
	if( varValue.vt == VT_NULL || varValue.vt == VT_EMPTY )
		varValue.SetString("");
	varValue.ChangeType(VT_BSTR);
	return CString(varValue.bstrVal);
}

int CCommonFun::CmpDate(CTime &tm1, CTime &tm2)
{
	CTimeSpan ts = tm1 - tm2;
	int n = ts.GetTotalHours();
	return n > 24 ? 1 : ( n <= 24 && n >= 0 ? 0 : -1);
}

CString CCommonFun::GetLocalIP(void)
{
	CString sIP;
	/*if(InitSockDll())*/
	{
		char localHost[255];
		memset(localHost,0,255);
		gethostname(localHost, 255);
		int nError = GetLastError();
		struct hostent *host = gethostbyname(localHost);	//Get local host IP / 
		if(host)
		{
			sIP = inet_ntoa (*(struct in_addr*)*host->h_addr_list);
		}
		/*ReleaseSockDll();*/
	}
	return sIP;
}
CString CCommonFun::GetLoaclHost(void)
{
	CString sHost = _T("");
	/*if(InitSockDll())*/
	{
		char localHost[255];
		memset(localHost,0,255);
		gethostname(localHost, 255);
		sHost = localHost;
		/*ReleaseSockDll();*/
	}
	return sHost;
}

int CCommonFun::GetLocalIPList(CStringArray& ary)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) 
	{
		//GlobalFree (pAdapterInfo);
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
	}

	MIB_IF_TYPE_ETHERNET;
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			CString sMac;
			BYTE* pbHexMac = pAdapter->Address;
			sMac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5]);



			PIP_ADDR_STRING pAddr = &(pAdapter->IpAddressList);
			int nCurIPIndex = 0;
			while (pAddr)
			{

				CString sIP(pAddr->IpAddress.String);
				if (!sIP.IsEmpty() && sIP.CompareNoCase(_T("0.0.0.0")) != 0)
				{
					ary.Add(sIP);
				}

				pAddr = pAddr->Next;
			}

			pAdapter = pAdapter->Next;

		}
	}
	else 
	{
		ASSERT(0);
	}
	free(pAdapterInfo);
	return ary.GetSize();
}


BOOL CCommonFun::IsLocalIP(CString sIP)
{
	CStringArray ary;
	GetLocalIPList(ary);
	for(int n = 0; n < ary.GetSize(); n++)
	{
		if(ary.GetAt(n).CompareNoCase(sIP) == 0 )
			return TRUE;
	}
	return FALSE;
}
void CCommonFun::ModifyFileAttribute(CString strPath, BYTE bToRemove, BYTE bToAdd)
{
	CFileStatus status;
	if(CFile::GetStatus(strPath, status))
	{
		status.m_attribute &= ~bToRemove;
		status.m_attribute |= bToAdd;
		try
		{
			CFile::SetStatus(strPath, status);
		}
		catch(...)
		{
			//CFileException �ܾ����ʣ�
		}
	}
}
void CCommonFun::DeleteDir(CString sDir, BOOL bOnlyDelFile)
{
	CFileFind	finder;				
	CString		sFullPath = _T("");					
	CString		strWildcard;

	sDir.TrimRight(_T("\\"));

	strWildcard = sDir + _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			DeleteDir(finder.GetFilePath());		
		}
		else
		{
			CString sName = finder.GetFileName();
			CString sPath = finder.GetFilePath();
			/*CFileStatus st ;
			CFile::GetStatus(sTmp,st);
			st.m_attribute = 0;
			CFile::SetStatus(sTmp,st);*/
			CCommonFun::WriteFileLog(_T("del file %s"),sPath);
			if(!DeleteFile(sPath) && (GetLastError() == 5))
			{
				ModifyFileAttribute(sPath, 0x01 | 0x02 | 0x04, NULL);
				if (!DeleteFile(sPath))
				{
					CCommonFun::WriteFileLog(_T("%sɾ��ʧ�ܣ�err = %d, ������ɾ��"), sPath, GetLastError());//?s
					MoveFileEx(sPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
				}
			}
		}
	}
	finder.Close();	
	//ɾ��Ŀ¼
	if(!bOnlyDelFile)
	{
		if(!::RemoveDirectory(sDir) && (GetLastError() == 5))
		{
			ModifyFileAttribute(sDir, 0x01 | 0x02 | 0x04, NULL);
			::RemoveDirectory(sDir);
		}
	}
}

BOOL CCommonFun::DeleteFileOrDir(CString szPath)	//add by zfq, ɾ��һ���ļ���Ŀ¼
{
	BOOL BRet = FALSE;
	if(!PathFileExists(szPath))
	{
		ASSERT(0);
		CCommonFun::WriteFileLog(_T("DeleteFileOrDir %s not exist"),szPath);
		return FALSE;
	}

	if(PathIsDirectory(szPath))
	{//��Ŀ¼
		DeleteDir(szPath, FALSE);
		BRet = TRUE;
	}
	else
	{//���ļ�
		BRet = ::DeleteFile(szPath);
	}

	return BRet;
}


BOOL CCommonFun::ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable)
{
	HRESULT hr = S_OK;
	TOKEN_PRIVILEGES NewState;
	LUID             luid;
	HANDLE hToken    = NULL;

	// Open the process token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken ))
	{
		TRACE(_T("Failed OpenProcessToken\n"));
		return ERROR_FUNCTION_FAILED;
	}

	// Get the local unique id for the privilege.
	if ( !LookupPrivilegeValue( NULL,
		szPrivilege,
		&luid ))
	{
		CloseHandle( hToken );
		TRACE(_T("Failed LookupPrivilegeValue\n"));
		return ERROR_FUNCTION_FAILED;
	}

	// Assign values to the TOKEN_PRIVILEGE structure.
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luid;
	NewState.Privileges[0].Attributes = 
		(fEnable ? SE_PRIVILEGE_ENABLED : 0);

	// Adjust the token privilege.
	if (!AdjustTokenPrivileges(hToken,
		FALSE,
		&NewState,
		sizeof(NewState),
		NULL,
		NULL))
	{
		TRACE(_T("Failed AdjustTokenPrivileges\n"));
		hr = ERROR_FUNCTION_FAILED;
	}

	// Close the handle.
	CloseHandle(hToken);

	return hr;
}


CString CCommonFun::GetProgramDir()
{
	TCHAR buf[MAX_PATH];
	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl);
	SHGetPathFromIDList(pidl, buf);
	CoTaskMemFree(pidl);

	return CString(buf);
}

CString CCommonFun::GetDesktopDir()
{
	TCHAR buf[MAX_PATH];
	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
	SHGetPathFromIDList(pidl, buf);
	CoTaskMemFree(pidl);

	return CString(buf);
}

CString CCommonFun::GetCommonDesktopDir()
{
	TCHAR buf[MAX_PATH];
	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, &pidl);
	SHGetPathFromIDList(pidl, buf);
	CoTaskMemFree(pidl);

	return CString(buf);
}


//
// �����Ի���, �����û�ѡ��һ���ļ���, �����ظ��ļ���·��
// ����:bNewStyle	ΪTRUEʱʹ���µĶԻ�����
//					ΪFALSEʱʹ�þɵĶԻ�����
//					����MSDN: SHBrowseForFolder
CString CCommonFun::GetDirFromShell(BOOL bNewStyle, HWND hOwner/*=NULL*/)
{
	CoInitialize(NULL);
	BROWSEINFO bi; 
	ITEMIDLIST * pidl; 
	TCHAR szDir[MAX_PATH] = {0};
	bi.hwndOwner=hOwner; 
	bi.pidlRoot=NULL; 
	bi.pszDisplayName=szDir; 
	bi.lpszTitle= _T("ѡ���ļ���"); 
	bi.ulFlags=BIF_RETURNONLYFSDIRS;
	if(bNewStyle)
		bi.ulFlags |= BIF_NEWDIALOGSTYLE; 
	bi.lpfn=NULL; 
	bi.lParam=0; 
	bi.iImage=0; 

	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL)
		szDir[0] = 0;
	else
	{
		if(!SHGetPathFromIDList(pidl,szDir))
			szDir[0] = 0; 
	}
	// AfxMessageBox(szDir);

	CoUninitialize();

	return szDir;
}

DWORD CCommonFun::StartSvc(CString strSvcName)
{
	SC_HANDLE  schService;
	BOOL       ret;
	DWORD      err;

	SC_HANDLE SchSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	schService = OpenService (SchSCManager,
		strSvcName,
		SERVICE_ALL_ACCESS
		);

	if (schService == NULL)
	{
		TRACE (("failure: OpenService error\n"));
		return FALSE;
	}
	ret = StartService (schService,    // service identifier
		0,             // number of arguments
		NULL           // pointer to arguments
		);
	err = GetLastError();
	if (ret)
	{
		TRACE (("StartService SUCCESS\n"));
	}
	else
	{
		if (err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			//
			// A common cause of failure (easier to read than an error code)
			//

			TRACE (("failure: StartService, ERROR_SERVICE_ALREADY_RUNNING\n"));
		}
		else
		{
			TRACE (("failure: StartService error\n"));
		}
	}
	CloseServiceHandle (schService);
	return ret;
}

DWORD CCommonFun::StopSvc(CString strSvcName)
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm!=NULL)
	{
		SC_HANDLE svc = OpenService(scm,strSvcName,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc,&ssa);
			if (ssa.dwCurrentState == SERVICE_RUNNING || ssa.dwCurrentState == SERVICE_PAUSED)
			{
				int nret=ControlService(svc, SERVICE_CONTROL_STOP, &ssa);
				if(!nret)
				{
					int nerr=GetLastError();
					return nerr;
				}
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return 0;
}

BOOL CCommonFun::CreateProcessAndWait(LPTSTR cmd)
{
	STARTUPINFO si={0};
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi={0};

	if(!CreateProcess(0,
		cmd,
		0,
		0,
		0,
		0,
		0,
		0,
		&si,
		&pi))
		return FALSE;

	AtlWaitWithMessageLoop(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return TRUE;
}

/*
#define EX_WIN_LOGOFF	1
#define	EX_WIN_REBOOT	2
#define EX_WIN_SHUTDOWN	3
*/
BOOL CCommonFun::ExitWin(int iMode)
{

	//1-ע����ǰ�û���2-����������3-�ر�;

	OSVERSIONINFOEX stOSVI ;
	ZeroMemory(&stOSVI,sizeof(OSVERSIONINFO)) ;
	stOSVI.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	BOOL bRet = FALSE;
	CCommonFun::WriteFileLog(_T("----Come in OperateMachine-------"));
	if(!CCommonFun::IsWin9X())
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp;  
		if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) 
		{
			CCommonFun::WriteFileLog(_T("----OpenProcessToken faile-------"));
			return FALSE;
		}

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);  
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;  

		AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);

		switch(iMode)
		{
		case EX_WIN_LOGOFF:
			bRet = ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);
			break;
		case EX_WIN_REBOOT:
			CCommonFun::WriteFileLog(_T("ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0)"));
			bRet = ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
			if(!bRet)
			{
				DWORD dw = ::GetLastError();
				CCommonFun::WriteFileLog(_T("---------bRet = %d ----"),bRet);
			}
			break;
		case EX_WIN_SHUTDOWN:
			CCommonFun::WriteFileLog(_T("EWX_SHUTDOWN|EWX_FORCE|EWX_POWEROFF,0)"));
			bRet = ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE|EWX_POWEROFF,0);
			if(!bRet)
			{
				DWORD dw = ::GetLastError();
				CCommonFun::WriteFileLog(_T("---------bRet = %d ----"),bRet);
			}
			break;
		default:
			break;
		}

	}
	else
	{
		switch(iMode)
		{
		case EX_WIN_LOGOFF:
			bRet = ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);
			break;
		case EX_WIN_REBOOT:
			bRet = ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
			break;
		case EX_WIN_SHUTDOWN:
			bRet = ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0);
			break;
		default:
			break;
		}
	}
	CCommonFun::WriteFileLog(_T("bRet = %d"), bRet);
	return bRet;
}

// ��ӹ���Ŀ¼
BOOL CCommonFun::AddNetShare(CString sShareName,CString sShareDesc,CString sDirPath)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// ���Ŀ¼������
	if (!PathFileExists(sDirPath))
		return FALSE;

#ifdef UNICODE
	LPWSTR pwcShareName = sShareName.GetBuffer();
	LPWSTR pwcShareDesc = sShareDesc.GetBuffer();
	LPWSTR pwcDirPath = sDirPath.GetBuffer();
#else
	WCHAR pwcShareName[MAX_PATH] = { 0 };
	WCHAR pwcShareDesc[MAX_PATH] = { 0 };
	WCHAR pwcDirPath[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, sShareName.GetString(), -1, pwcShareName, sShareName.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, sShareDesc.GetString(), -1, pwcShareDesc, sShareDesc.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, sDirPath.GetString(), -1, pwcDirPath, sDirPath.GetLength());
#endif // UNICODE

	NET_API_STATUS res;
	SHARE_INFO_2 p;
	DWORD parm_err = 0;

	// Fill in the SHARE_INFO_2 structure.
	p.shi2_netname = pwcShareName;
	p.shi2_type = STYPE_DISKTREE; // disk drive
	p.shi2_remark = pwcShareDesc;
	p.shi2_permissions = ACCESS_ALL;
	p.shi2_max_uses = -1;
	p.shi2_current_uses = 0; 
	p.shi2_path = pwcDirPath;
	p.shi2_passwd = NULL; 

	// Call the NetShareAdd function,
	res=NetShareAdd(NULL,2,(LPBYTE)&p,&parm_err);
	if(res == NERR_Success)
		return TRUE;
	else
	{
		CCommonFun::WriteFileLog(_T("!!CCommonFun::AddNetShare,fail,res=%d,sShareName=%s,sDirPath=%s")
			, res, sShareName, sDirPath);
		return FALSE;
	}
}

// �жϹ���Ŀ¼�Ƿ����
BOOL CCommonFun::CheckNetShare(CString sDirPath)
{
#ifdef UNICODE
	LPWSTR pwcDirPath = sDirPath.GetBuffer();
#else
	WCHAR pwcDirPath[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, sDirPath.GetString(), -1, pwcDirPath, sDirPath.GetLength());
#endif // UNICODE

	NET_API_STATUS res;
	DWORD devType = 0;
	res=NetShareCheck(NULL, pwcDirPath, &devType);
	if(res == NERR_Success)
		return TRUE;
	else
		return FALSE;
}
// ɾ������Ŀ¼
BOOL CCommonFun::DelNetShare(CString sShareName)
{
#ifdef UNICODE
	LPWSTR pwcShareName = sShareName.GetBuffer();
#else
	WCHAR pwcShareName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, sShareName.GetString(), -1, pwcShareName, sShareName.GetLength());
#endif // UNICODE

	NET_API_STATUS res;
	res=NetShareDel(NULL, pwcShareName, 0);
	if(res == NERR_Success)
		return TRUE;
	else
		return FALSE;
	return TRUE;
}



BOOL CCommonFun::AddRights(TCHAR* lpFileName,TCHAR* lpAccountName,DWORD dw)
{
	//lookupAccountName��ر���
	PSID pUserID=NULL;
	DWORD dUserID=0;
	TCHAR* pDomainName=NULL;
	DWORD dDomainName=0;
	SID_NAME_USE snuType;

	//��һ����ȡ�ô�����û����û����SID
	BOOL check1=LookupAccountName(NULL,lpAccountName,pUserID,&dUserID,pDomainName,&dDomainName,&snuType);
	pUserID=(PSID)myHeapAlloc(dUserID);
	pDomainName=(TCHAR*)myHeapAlloc(dDomainName*sizeof(TCHAR));
	check1=LookupAccountName(NULL,lpAccountName,pUserID,&dUserID,pDomainName,&dDomainName,&snuType);

	//�ڶ�����ȡ���ļ��İ�ȫ������(SD)
	PSECURITY_DESCRIPTOR pFileSD=NULL;
	DWORD dFileSD=0;
	SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

	check1=GetFileSecurity(lpFileName,secInfo,pFileSD,0,&dFileSD);
	pFileSD=(PSECURITY_DESCRIPTOR)myHeapAlloc(dFileSD);
	check1=GetFileSecurity(lpFileName,secInfo,pFileSD,dFileSD,&dFileSD);

	//����������ʼһ���µ�SD
	SECURITY_DESCRIPTOR newSD;
	InitializeSecurityDescriptor(&newSD,SECURITY_DESCRIPTOR_REVISION);

	//���Ĳ�����SD��ȡ��DACL
	BOOL bPresent;
	BOOL bDaclDefault;
	PACL pACL=NULL;
	GetSecurityDescriptorDacl(pFileSD,&bPresent,&pACL,&bDaclDefault);

	//���岽��ȡDACL���ڴ��С
	ACL_SIZE_INFORMATION aclInfo;
	aclInfo.AceCount=0;
	aclInfo.AclBytesFree=0;
	aclInfo.AclBytesInUse=sizeof(ACL);

	// ���DACL��Ϊ�գ���ȡ����Ϣ
	if(bPresent){
		GetAclInformation(pACL,&aclInfo,sizeof(ACL_SIZE_INFORMATION),AclSizeInformation);
	}

	//�������������µ�ACL��size
	DWORD dNewAcl=aclInfo.AclBytesInUse+sizeof(ACL_SIZE_INFORMATION)+GetLengthSid(pUserID)-2;

	//���߲���Ϊ��ACL�����ڴ�
	PACL pNewAcl=(PACL)myHeapAlloc(dNewAcl);

	//�ڰ˲�����ʼ���µ�acl
	InitializeAcl(pNewAcl,dNewAcl,ACL_REVISION2);

	//�ھŲ�������ļ���Ŀ¼����DACL�������򿽱����ݵ��µ�DACL
	UINT newAclIndex=0;
	UINT curAclIndex=0;
	LPVOID pTempAce=NULL;
	if (bPresent&&aclInfo.AceCount)
	{
		for (curAclIndex=0;curAclIndex!=aclInfo.AceCount;curAclIndex++)
		{
			GetAce(pACL,curAclIndex,&pTempAce);
			if (((ACCESS_ALLOWED_ACE*)pTempAce)->Header.AceFlags==INHERITED_ACE)
				break;

			if (EqualSid(pUserID,&(((ACCESS_ALLOWED_ACE*)pTempAce)->SidStart)))
			{
				printf("suc");
				continue;
			}

			AddAce(pNewAcl,ACL_REVISION,MAXDWORD,pTempAce,((PACE_HEADER)pTempAce)->AceSize);
			newAclIndex++;
		}
	}

	SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;
	AddAccessAllowedAceExFnPtr _AddAccessAllowedAceEx = NULL;
	//��̬װ�غ���
	_AddAccessAllowedAceEx = (AddAccessAllowedAceExFnPtr)
		GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
		"AddAccessAllowedAceEx");
	_AddAccessAllowedAceEx(pNewAcl, ACL_REVISION2,
		CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ,
		dw, pUserID);

	//�����̳е�ACE
	if (bPresent&&aclInfo.AceCount)
	{
		for (;curAclIndex!=aclInfo.AceCount;curAclIndex++)
		{
			GetAce(pACL,curAclIndex,&pTempAce);
			AddAce(pNewAcl,ACL_REVISION,MAXDWORD,pTempAce,((PACE_HEADER)pTempAce)->AceSize);
		}
	}

	//���µ�acl���õ��µ�sd��
	check1=SetSecurityDescriptorDacl(&newSD,TRUE,pNewAcl,FALSE);
	check1=SetFileSecurity(lpFileName,secInfo,&newSD);
	DWORD err=GetLastError();
	return TRUE;
}

#include "Lm.h"
#pragma comment(lib, "Netapi32.lib")

//����û�
BOOL CCommonFun::AddUser(CString szUserName, CString szPassword, CString szComment /*= ""*/)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
	LPWSTR pwcPassword = szPassword.GetBuffer();
	LPWSTR pwcComment = szComment.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	WCHAR pwcPassword[MAX_PATH] = { 0 };
	WCHAR pwcComment[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, szPassword.GetString(), -1, pwcPassword, szPassword.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, szComment.GetString(), -1, pwcComment, szComment.GetLength());
#endif // UNICODE

	USER_INFO_1 ui;
	ui.usri1_name = pwcUserName;
	ui.usri1_password = pwcPassword;
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = NULL; 
	ui.usri1_comment = pwcComment; 
	ui.usri1_flags = UF_SCRIPT|UF_DONT_EXPIRE_PASSWD;	//Ĭ��������������
	ui.usri1_script_path = NULL;

	//����Ȩ��
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	DWORD dwError = 0;
	NET_API_STATUS status = NetUserAdd(NULL, 1, (LPBYTE)&ui, &dwError);
	if (status != NERR_Success /*&& status != NERR_UserExists*/)
	{
		CCommonFun::WriteFileLog(_T("AddUser fail,status=%u,dwError=%u"), status, dwError);
		return FALSE;
	}

	return TRUE;
}

//ɾ���û�
BOOL CCommonFun::DeleteUser(CString szUserName)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
#endif // UNICODE

	//����Ȩ��
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetUserDel(NULL, pwcUserName);
	if (status != NERR_Success /*&& status != NERR_UserNotFound*/)
	{
		printf("DeleteUser fail,status=%u\n", status);
		return FALSE;
	}

	return TRUE;
}

//����û�������
BOOL CCommonFun::AddUserToGroup(CString szUserName, CString szGroupName)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
	LPWSTR pwcGroupName = szGroupName.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	WCHAR pwcGroupName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, szGroupName.GetString(), -1, pwcGroupName, szGroupName.GetLength());
#endif // UNICODE

	LOCALGROUP_MEMBERS_INFO_3 account; 
	account.lgrmi3_domainandname = pwcUserName;

	//����Ȩ��
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetLocalGroupAddMembers(NULL,pwcGroupName,3,(LPBYTE)&account,1);
	if (status != NERR_Success /*&& status!=ERROR_MEMBER_IN_ALIAS*/)
	{
		printf("AddUserToGroup fail,status=%u\n", status);
		return FALSE;
	}

	return TRUE;
}

//������ɾ���û�
BOOL CCommonFun::DeleteUserFromGroup(CString szUserName, CString szGroupName)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
	LPWSTR pwcGroupName = szGroupName.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	WCHAR pwcGroupName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, szGroupName.GetString(), -1, pwcGroupName, szGroupName.GetLength());
#endif // UNICODE

	LOCALGROUP_MEMBERS_INFO_3 account; 
	account.lgrmi3_domainandname = pwcUserName;

	//����Ȩ��
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetLocalGroupDelMembers(NULL,pwcGroupName,3,(LPBYTE)&account,1);
	if (status != NERR_Success /*&& status!=ERROR_MEMBER_NOT_IN_ALIAS*/)
	{
		printf("DeleteUserFromGroup fail,status=%u\n", status);
		return FALSE;
	}

	return TRUE;
}

BOOL CCommonFun::RunProcess(CString sPath,CString sParam,DWORD dwWaitMilliseconds /* = INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
	BOOL bOk = FALSE;
	int nLen = sPath.GetLength() + sParam.GetLength() + 10;
	TCHAR* pszPath = new TCHAR[nLen];
	ASSERT(pszPath);
	memset(pszPath,0,nLen*sizeof(TCHAR));

	_stprintf(pszPath,_T("\"%s\" %s"),sPath,sParam);
	STARTUPINFO si = { sizeof si, 0, _T("") };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = dwShowCmd;

	WriteFileLog(_T("Install pathandParam = %s"),pszPath);
	PROCESS_INFORMATION pi;
	if(CreateProcess(0, 
		pszPath,// command line
		0, 0,  // security attributes
		FALSE, // handle inheritance
		0,     // creation flags
		0,     // environment block
		0,     // current directory
		&si, &pi ))
	{
		WriteFileLog(_T("createprocess pid = %d"),pi.dwProcessId);
		//	if(bWait)
		{
			::WaitForSingleObject(pi.hProcess,dwWaitMilliseconds);
		}
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		bOk = TRUE;
	}
	else
	{
		CString s;
		s.Format(_T("Install ʧ�� err = %d \n"),::GetLastError());
		WriteFileLog(s);
	}
	if (pszPath)
	{
		delete pszPath;
		pszPath = NULL;
	}
	return bOk;
}

//RunProcessByGUISessionId���ṩ��������̵��ã��������̿���ʧ�ܡ�
BOOL CCommonFun::RunProcessByGUISessionId(CString sPath, CString sParam, DWORD dwSessionId /*= 0xFFFFFFFF*/, DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,1,sPath=%s,sParam=%s,dwSessionId=%u,dwWaitMilliseconds=%u,dwShowCmd=%d")
		, sPath, sParam, dwSessionId, dwWaitMilliseconds, dwShowCmd);
	if (dwSessionId == 0xFFFFFFFF)
	{
		//��ȡ��ǰ���ڻ״̬SessionId
		dwSessionId = WTSGetActiveConsoleSessionId();
		if (dwSessionId == 0xFFFFFFFF)
		{
			WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,2,WTSGetActiveConsoleSessionId fail,error=%u"), GetLastError());
			return FALSE;
		}
	}

	HANDLE hToken = NULL;
	HANDLE hTokenDup = NULL;
	LPVOID pEnv = NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//��ȡ��ǰ���ڻ״̬�û���Token
	//To call this function successfully, the calling application must be running within 
	//the context of the LocalSystem account and have the SE_TCB_NAME privilege. 
	if (!WTSQueryUserToken(dwSessionId, &hToken))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,3,WTSQueryUserToken fail,dwSessionId=%u,err=%d"), dwSessionId, GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	//�����µ�Token
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,4,DuplicateTokenEx fail,err=%d"), GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	//����������Ϣ
	if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,5,CreateEnvironmentBlock fail,err=%d"), GetLastError());
		CloseHandle(hTokenDup);
		CloseHandle(hToken);
		return FALSE;
	}

	//������������
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = _TEXT("winsta0\\default");
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = dwShowCmd;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	//��ʼ��������
	CString szCommandLine;
	szCommandLine.Format(_T("\"%s\" %s"), sPath, sParam);
	DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
	if (!CreateProcessAsUser(hTokenDup, NULL, szCommandLine.GetBuffer(), NULL, NULL, FALSE, dwCreateFlag, pEnv, NULL, &si, &pi))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,6,CreateProcessAsUser fail,err=%d"), GetLastError());
		DestroyEnvironmentBlock(pEnv);
		CloseHandle(hTokenDup);
		CloseHandle(hToken);
		return FALSE;
	}

	//���Ӳ���,������Դ

	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,7,CreateProcessAsUser succ,dwWaitMilliseconds=%u"), dwWaitMilliseconds);

	//�ȴ������Ľ��̽���
	WaitForSingleObject(pi.hProcess, dwWaitMilliseconds);

	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,8,end"));

	DestroyEnvironmentBlock(pEnv);
	CloseHandle(hTokenDup);
	CloseHandle(hToken);

	return TRUE;
}

//��ȡָ���ļ����ڵ�Ŀ¼,"C:\test\test.doc" => ""C:\test""
CString CCommonFun::GetFileDirByFullPath(CString szFullPath)
{
	szFullPath.Replace(_T("/"), _T("\\"));
	szFullPath.TrimRight(_T("\\"));
	int nPos = szFullPath.ReverseFind('\\');
	return szFullPath.Left(nPos);
}

//��ȡָ���ļ���,"C:\test\test.doc" => "test.doc"
CString CCommonFun::GetFileNameByFullPath(CString szFullPath)
{
	szFullPath.Replace(_T("/"), _T("\\"));
	szFullPath.TrimRight(_T("\\"));
	int nPos = szFullPath.ReverseFind('\\');
	return szFullPath.Right(szFullPath.GetLength() - nPos - 1);
}

BOOL CCommonFun::RegisterSessionNotification(HWND hWnd)
{
	ASSERT(hWnd && ::IsWindow(hWnd));
	HMODULE hModule = ::LoadLibrary(_T("Wtsapi32"));
	if(!hModule)
		return FALSE;
	typedef BOOL(WINAPI* PFNWTSRegisterSessionNotification)(HWND,DWORD);
	BOOL bRet = FALSE;
	PFNWTSRegisterSessionNotification pfnWTSRegisterSessionNotification =
		(PFNWTSRegisterSessionNotification)::GetProcAddress(hModule,"WTSRegisterSessionNotification");
	if(pfnWTSRegisterSessionNotification)
	{
		bRet = pfnWTSRegisterSessionNotification(hWnd,NOTIFY_FOR_ALL_SESSIONS);//NOTIFY_FOR_THIS_SESSION
	}
	if(hModule)
		::FreeLibrary(hModule);
	return bRet;
}

void CCommonFun::UnRegisterSessionNotification(HWND hWnd)
{
	ASSERT(hWnd && ::IsWindow(hWnd));
	HMODULE hModule = ::LoadLibrary(_T("Wtsapi32"));
	if(!hModule)
		return ;

	typedef BOOL(WINAPI* PFNWTSUnRegisterSessionNotification)(HWND);
	PFNWTSUnRegisterSessionNotification pfnWTSUnRegisterSessionNotification =
		(PFNWTSUnRegisterSessionNotification)::GetProcAddress(hModule,"WTSUnRegisterSessionNotification");
	if(pfnWTSUnRegisterSessionNotification)
	{
		pfnWTSUnRegisterSessionNotification(hWnd);
	}
	if(hModule)
		::FreeLibrary(hModule);
}

//��ȡ����·����һ������·������������������ܹ���������
BOOL CCommonFun::GetSharePath(CString szLocalPath, CString szShareName, CString& szShareDir, CString& szSharePath)
{
	BOOL bRet = TRUE;
	CString szLocalDir = GetFileDirByFullPath(szLocalPath);
	CString szLocalFile = GetFileNameByFullPath(szLocalPath);
	CCommonFun::WriteFileLog(_T("CCommonFun::GetSharePath,szLocalPath=%s,szShareName=%s,szLocalDir=%s,szLocalFile=%s")
		, szLocalPath, szShareName, szLocalDir, szLocalFile);

	//����Ȩ��
	CCommonFun::AddRights(szLocalDir.GetBuffer(), _T("Everyone"), GENERIC_ALL|STANDARD_RIGHTS_ALL);
	if (PathFileExists(szLocalPath))
	{
		CCommonFun::AddRights(szLocalPath.GetBuffer(), _T("Everyone"), GENERIC_ALL|STANDARD_RIGHTS_ALL);
	}

	//ɾ������Ŀ¼
	if (CheckNetShare(szLocalDir))
	{
		DelNetShare(szShareName);
	}

	//��������Ŀ¼
	bRet = AddNetShare(szShareName, _T(""), szLocalDir);
	if (bRet)
	{
		szShareDir.Format(_T("\\\\%s\\%s"), CCommonFun::GetLocalIP(), szShareName);
		szSharePath.Format(_T("\\\\%s\\%s\\%s"), CCommonFun::GetLocalIP(), szShareName, szLocalFile);
	}

	return bRet;
}

CStringA CCommonFun::WideCharToMultiByte2(const wchar_t* pwszSource)
{
	CStringA szRetA = "";
	if (pwszSource)
	{
		DWORD dwNum = WideCharToMultiByte(CP_GB2312, NULL, pwszSource, -1, NULL, 0, NULL, FALSE);
		char* pszText = new char[dwNum];
		memset(pszText, 0x0, sizeof(char)*dwNum);
		WideCharToMultiByte(CP_GB2312, NULL, pwszSource, -1, pszText, dwNum, NULL, FALSE);
		szRetA = pszText;
		delete[] pszText;
	}

	return szRetA;
}

CStringW CCommonFun::MultiByteToWideChar2(const char* pszSource)
{
	CStringW szRetW = L"";
	if (pszSource)
	{
		DWORD dwNum = MultiByteToWideChar(CP_GB2312, NULL, pszSource, -1, NULL, 0);
		wchar_t* pwszText = new wchar_t[dwNum];
		memset(pwszText, 0x0, sizeof(wchar_t)*dwNum);
		MultiByteToWideChar(CP_GB2312, NULL, pszSource, -1, pwszText, dwNum);
		szRetW = pwszText;
		delete[] pwszText;
	}

	return szRetW;
}

CStringA CCommonFun::UnicodeToMultiByte(const wchar_t* pwszSource)
{
	return WideCharToMultiByte2(pwszSource);
}

CStringW CCommonFun::MultiByteToUnicode(const char* pszSource)
{
	return MultiByteToWideChar2(pszSource);
}

CStringA CCommonFun::UTF8ToMultiByte(const char* puszSource)
{
	CStringA szRetA = "";
	if (puszSource)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)puszSource, -1, NULL, 0);
		wchar_t* wszText = new wchar_t[len + 1];
		memset(wszText, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)puszSource, -1, wszText, len);

		len = WideCharToMultiByte(CP_GB2312, 0, wszText, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_GB2312, 0, wszText, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] wszText;
		delete[] szText;
	}
	return szRetA;
}

CStringA CCommonFun::MultiByteToUTF8(const char* puszSource)
{
	CStringA szRetA = "";
	if (puszSource)
	{
		int len = MultiByteToWideChar(CP_GB2312, 0, (LPCSTR)puszSource, -1, NULL, 0);
		wchar_t* wszText = new wchar_t[len + 1];
		memset(wszText, 0, len * 2 + 2);
		MultiByteToWideChar(CP_GB2312, 0, (LPCSTR)puszSource, -1, wszText, len);

		len = WideCharToMultiByte(CP_UTF8, 0, wszText, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wszText, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] wszText;
		delete[] szText;
	}
	return szRetA;
}

CStringW CCommonFun::UTF8ToUnicode(const char* puszSource)
{
	CStringW szRetW = L"";
	if (puszSource)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)puszSource, -1, NULL, 0);
		wchar_t * wszText = new wchar_t[len + 1];
		memset(wszText, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)puszSource, -1, wszText, len);

		szRetW = wszText;
		delete[] wszText;
	}
	return szRetW;
}

CStringA CCommonFun::UnicodeToUTF8(const wchar_t* pwszSource)
{
	CStringA szRetA = "";
	if (pwszSource)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, pwszSource, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, pwszSource, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] szText;
	}
	return szRetA;
}

//��ȡ��־Ŀ¼
CString& CCommonFun::GetLogDir()
{
	static CString g_szLogDir = _T("");
	if (g_szLogDir.IsEmpty())
	{
		OutputDebugString(_T("##CCommonFun::GetLogDir"));

		TCHAR path[MAX_PATH];
		CString sDefaultPath;
		if(GetModuleFileName(0,path,MAX_PATH))
		{
			TCHAR *pdest;
			pdest=_tcsrchr(path,_T('\\'));
			if(pdest) 
				*pdest=0;
			sDefaultPath=path;
			sDefaultPath += _T("\\xlog");
		}

		if (sDefaultPath.IsEmpty())
		{
			sDefaultPath = _T(".\\xlog");
		}

		g_szLogDir = sDefaultPath;
	}

	return g_szLogDir;
}
