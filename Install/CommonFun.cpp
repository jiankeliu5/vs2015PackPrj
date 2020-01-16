// CommonFun.cpp: implementation of the CCommonFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <comutil.h>
#include "CommonFun.h"
#include <Shobjidl.h>
#include <intshcut.h>

#include "Lm.h"
#pragma comment(lib, "Netapi32.lib")

//RunProcessByGUISessionId接口内部需要用到Wtsapi32.lib和Userenv.lib
#include <UserEnv.h>
#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



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


CCommonFun::CCommonFun()
{

}

CCommonFun::~CCommonFun()
{

}

/*****************************************************************************
Author		:	zhouhonglei
CopyRight	:	zhouhonglei 2002
函数名		:	Split( CString str, CStringArray *psArray , 
					char chDelimiter=' ', int nCount=-1 )
说明		:	分割字符串函数
参数		:	
str			:	传入参数,必选项。
				字符串表达式，包含子字符串和分隔符。
				如果 str 为零长度字符串，Split 返回空数组，即不包含元素和数据的数组。
psArray:		传出参数,返回分割后的字符串数组，其中包含指定数目的子字符串.
chDelimiter	:	可选项。用于标识子字符串界限的字符。如果省略，使用空格 (",") 作为分隔符。		
nCount		:	可选项。被返回的子字符串数目，-1 指示返回所有子字符串。

返回值		:	void
*******************************************************************************/

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
	sFileName.Format(_T("%s\\xab_Install_%s.Log"),sSysDir,sDate);
	
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
			// 新创建的日志文件，则写入Unicode头
			BYTE chUnicodeHead[2] = { 0xff, 0xfe }; // Unicode头
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

//--------------------------------
//将数字转变成字符串
//--------------------------------
CStringA CCommonFun::NToS(int nNum)
{
	CStringA sTmp;
	sTmp.Format("%d",nNum);
	return sTmp;
}

//--------------------------------
//将数字转变成字符串
//--------------------------------
CString CCommonFun::NToS(double dbNum)
{
	CString sTmp;
	sTmp.Format(_T("%f"),dbNum);
	return sTmp;

}
//--------------------------------
//将数字转变成字符串
//--------------------------------
CString CCommonFun::NToS(DWORD dwNum)
{
	CString sTmp;
	sTmp.Format(_T("%u"),dwNum);
	return sTmp;
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

int CCommonFun::SToN(CString str)
{
	return _tstoi(str);
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
		TRACE("请输入一个绝对路径！");
		return FALSE;
	}

	CString sDriver = sAry.GetAt(0);
	if( sDriver.GetLength() != 2 || sDriver.GetAt(sDriver.GetLength()-1) != ':' )
	{
		TRACE("请输入一个带盘符的路径！");
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
			//TRACE("创建目录" + sDir + "失败！");
			return FALSE;
		}
	}
	//TRACE("创建目录"+sDir+"成功！");
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

/************************************************************************/
/* 
NT 6.0	Longhorn			Windows Vista
NT 6.0	Longhorn Server		Windows Server 2008
NT 6.1	Blackcomb,Vienna,	Windows 7
NT 6.1	Windows Server 7	Windows Server 2008 R2
NT 6.2	Windows 8			Windows 8
NT 6.2	Windows Server 8	Windows Server 2012
NT 6.3	Windows Blue		Windows 8.1
NT 6.3	无					Windows Server 2012 R2
NT 6.3.9600.17031	Windows 8.1 Spring Update	Windows 8.1 with Update
NT 10.0	Windows Threshold　	Windows 10	Windows Server 2016
NT 10.1	Windows10 Autumn Update	Windows10 Update 1
NT 10.2	无					Windows10 redstone
*/
/************************************************************************/
BOOL CCommonFun::IsWinWin8OrLater()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if((osv.dwMajorVersion > 6) || ((osv.dwMajorVersion == 6) && (osv.dwMinorVersion >= 2)))
	{
		return TRUE;
	}
	return FALSE;
}


//-----------add by zhandongbao 2005.12.7--------------
//-----------获得目前程序运行的路径
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

//------------------判断当前系统是否为NT系统
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

BOOL CCommonFun::IfDirValidThenCreateIt(CString szDir)
{
	if (szDir.IsEmpty())
		return FALSE;

	//把诸如:"C:\tmp1\tmp2\"变为"C:\tmp1\tmp2"
	int nPathLen = szDir.GetLength();
	while ('\\' == szDir.GetAt(nPathLen - 1))	//注意CString中第一个字符的index是0
	{//若最后一个字符是'\\',则去掉它
		szDir = szDir.Left(nPathLen - 1);
		nPathLen--;
	}

	CFileStatus fileStatus;
	if (TRUE == CFile::GetStatus(szDir, fileStatus)
		&& (CFile::directory & fileStatus.m_attribute))
	{//目录已经存在
		return TRUE;
	}

	//逐级创建这个目录
	CStringArray dirAry;
	CString szTmp = _T("");
	int i = 0;
	int nDirLen = szDir.GetLength();
	for (i = 0; i < nDirLen; i++)
	{
		if ('\\' != szDir.GetAt(i))
			szTmp += szDir.GetAt(i);
		else
		{
			dirAry.Add(szTmp);
			szTmp += '\\';
		}
		if (i + 1 == nDirLen)
			dirAry.Add(szTmp);
	}//for(i)

	int nNum = dirAry.GetSize();
	for (i = 0; i < nNum; i++)
	{
		szTmp = dirAry.GetAt(i);
		CreateDirectory(szTmp, NULL);
	}//for(i)

	 //创建之后再次判断
	if (TRUE == CFile::GetStatus(szDir, fileStatus)
		&& (CFile::directory & fileStatus.m_attribute))
	{//目录已经存在
		return TRUE;
	}
	else
	{//不是合法目录,或是一个文件
		return FALSE;
	}
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
			//CFileException 拒绝访问？
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
			theLog.Write(_T("del file %s"),sPath);
			if(!DeleteFile(sPath) && (GetLastError() == 5))
			{
				ModifyFileAttribute(sPath, 0x01 | 0x02 | 0x04, NULL);
				if (!DeleteFile(sPath))
				{
					CCommonFun::WriteFileLog(_T("%s删除失败，err = %d, 重启后删除"), sPath, GetLastError());//?s
					MoveFileEx(sPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
				}
			}
		}
	}
	finder.Close();	
	//删除目录
	if(!bOnlyDelFile)
	{
		if(!::RemoveDirectory(sDir) && (GetLastError() == 5))
		{
			ModifyFileAttribute(sDir, 0x01 | 0x02 | 0x04, NULL);
			::RemoveDirectory(sDir);
		}
	}
}

BOOL CCommonFun::DeleteFileOrDir(CString szPath)	//add by zfq, 删除一个文件或目录
{
	BOOL BRet = FALSE;
	if(!::PathFileExists(szPath))
	{
		ASSERT(0);
		theLog.Write(_T("DeleteFileOrDir %s not exist"),szPath);
		return FALSE;
	}

	if(PathIsDirectory(szPath))
	{//是目录
		DeleteDir(szPath, FALSE);
		BRet = TRUE;
	}
	else
	{//是文件
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

BOOL CCommonFun::CreateShellLink(LPCTSTR filePath, LPCTSTR lnkPath, LPCTSTR iconPath, int iconIndex, LPCTSTR params)
{
	//theLog.Write(_T("CreateShellLink [%s][%s][%s][%d][%s]"), filePath, lnkPath, iconPath?iconPath:_T(""), iconIndex, params?params:_T(""));

	HRESULT hres;
	IShellLink *psl = NULL;
	IPersistFile *pPf = NULL;
	hres = CoCreateInstance(CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		(LPVOID*)&psl);
	if(FAILED(hres))
		goto cleanup;
	hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&pPf);
	if(FAILED(hres))
		goto cleanup;
	hres = psl->SetPath(filePath);
	if(FAILED(hres))
		goto cleanup;
	if(params!=NULL) {
		hres = psl->SetArguments(params);
		if(FAILED(hres))
			goto cleanup;
	}

#ifdef UNICODE
	LPWSTR pwLnkPath = (LPWSTR)lnkPath;
#else
	WCHAR pwLnkPath[MAX_PATH];
	MultiByteToWideChar(CP_GB2312, 0, lnkPath, -1, wsz, MAX_PATH);
#endif // UNICODE
	hres = pPf->Save(pwLnkPath, TRUE);
	if(FAILED(hres))
		goto cleanup;

	if(iconPath != NULL) 
	{
#ifdef UNICODE
		LPWSTR pwIconPath = (LPWSTR)iconPath;
#else
		WCHAR pwIconPath[MAX_PATH];
		MultiByteToWideChar(CP_GB2312, 0, iconPath, -1, pwIconPath, MAX_PATH);
#endif // UNICODE

		hres = psl->SetIconLocation(pwIconPath, iconIndex);
		if(FAILED(hres))
			goto cleanup;
		pPf->Save(pwLnkPath, TRUE);
	}
cleanup:
	if(pPf)
		pPf->Release();
	if(psl)
		psl->Release();

	return TRUE;
}

BOOL CCommonFun::CreateURLLink(LPCTSTR url, LPCTSTR lnkPath, LPCTSTR iconPath, int iconIndex)
{
	//theLog.Write(_T("CreateShellLink [%s][%s][%s][%d]"), url, lnkPath, iconPath, iconIndex);

	HRESULT hres;
	IUniformResourceLocator *purl;
	IPersistFile *ppf;
	IPropertySetStorage* psps;
	IPropertyStorage *pps;
	hres = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER,
		IID_IUniformResourceLocator, (void **)&purl);
	hres = purl->QueryInterface(IID_IPersistFile, (void **)&ppf);
	// Set URL
	hres = purl->SetURL(url, IURL_SETURL_FL_GUESS_PROTOCOL );

#ifdef UNICODE
	LPWSTR pwLnkPath = (LPWSTR)lnkPath;
#else
	WCHAR pwLnkPath[MAX_PATH];
	MultiByteToWideChar(CP_GB2312, 0, lnkPath, -1, wsz, MAX_PATH);
#endif // UNICODE

	// Save shortcut (with remember)
	hres = ppf->Save(pwLnkPath, TRUE );

	if(iconPath != NULL)
	{
		// get PropertySetStorage-Interface
		hres = purl->QueryInterface(IID_IPropertySetStorage, (void**)&psps );
		// Open ProertySet
		hres = psps->Open(FMTID_Intshcut, STGM_READWRITE, &pps );
		// Array of Propertyps to write (by PROP_ID)
		PROPSPEC ppids[2] = { {PRSPEC_PROPID, PID_IS_ICONINDEX}, {PRSPEC_PROPID, PID_IS_ICONFILE} };
		// Array of Property-Variants
		PROPVARIANT ppvar[2];

#ifdef UNICODE
		LPWSTR pwIconPath = (LPWSTR)iconPath;
#else
		WCHAR pwIconPath[MAX_PATH];
		MultiByteToWideChar(CP_GB2312, 0, iconPath, -1, pwIconPath, MAX_PATH);
#endif // UNICODE

		// Initialize PropVars
		PropVariantInit( ppvar );
		PropVariantInit( ppvar + 1 );
		ppvar[0].vt = VT_I4; // Index is I4
		ppvar[0].lVal = iconIndex;
		ppvar[1].vt = VT_LPWSTR; // Iconfile is LPWSTR
		ppvar[1].pwszVal = pwIconPath; // Name of Iconfile (shell32.dll)
		// Write Propertydata
		hres = pps->WriteMultiple(2, ppids, ppvar, 0 );
		// Commit Propertydata (flush)
		hres = pps->Commit(STGC_DEFAULT);
		// Save icon (remembered)
		hres = ppf->Save(NULL, TRUE);
		psps->Release();
		pps->Release();
	}

	ppf->Release();
	purl->Release(); 

	return TRUE;
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
// 弹出对话框, 允许用户选择一个文件夹, 并返回该文件夹路径
// 参数:bNewStyle	为TRUE时使用新的对话框风格
//					为FALSE时使用旧的对话框风格
//					参数MSDN: SHBrowseForFolder
CString CCommonFun::GetDirFromShell(BOOL bNewStyle, HWND hOwner/*=NULL*/)
{
	CoInitialize(NULL);
	BROWSEINFO bi; 
	ITEMIDLIST * pidl; 
	TCHAR szDir[MAX_PATH] = {0};
	bi.hwndOwner=hOwner; 
	bi.pidlRoot=NULL; 
	bi.pszDisplayName=szDir; 
	bi.lpszTitle= _T("选择文件夹"); 
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

BOOL CCommonFun::ExcuteCmd(CString szCmdLine, BOOL bWait /*= FALSE*/, DWORD waitlong/* = INFINITE*/)
{
	//CString szAppName = _T("C:\\Windows\\System32\\cmd.exe");
	STARTUPINFO si = { sizeof si, 0, _T("") };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	BOOL ret = FALSE;
	PROCESS_INFORMATION pi;
	TCHAR lpCmd[1024] = {0};
	_stprintf(lpCmd, _T("%s"), szCmdLine);

	if (CreateProcess(NULL,		//pszImageName
		lpCmd, // pszCmdLine
		0, 0,  // security attributes
		FALSE, // handle inheritance
		0,     // creation flags
		0,     // environment block
		0,     // current directory
		&si, &pi))
	{
		if (bWait)
		{
			WaitForSingleObject(pi.hProcess, waitlong);
		}
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		return TRUE;
	}
	return FALSE;
}

static BOOL /*CCommonFun::*/WaitForState(DWORD dwDesiredState, SC_HANDLE svc) 
{
	BOOL bResult = FALSE;
	// Loop until driver reaches desired state or error occurs
	while (1)
	{
		SERVICE_STATUS ssa;
		// Get current state of driver
		bResult = ::QueryServiceStatus(svc, &ssa);
		// If we can't query the driver, we're done
		if (!bResult)
		{
			CCommonFun::WriteFileLog(_T("!!WaitForState QueryServiceStatus fail err=%d,svc=%X"),GetLastError(), svc);
			break;
		}
		// If the driver reaches the desired state
		if (ssa.dwCurrentState == dwDesiredState) 
			break;
		// We're not done, wait the specified period of time
		DWORD dwWaitHint = ssa.dwWaitHint / 10;    // Poll 1/10 of the wait hint
		if (dwWaitHint <  1000) dwWaitHint = 1000;  // At most once a second
		if (dwWaitHint > 10000) dwWaitHint = 10000; // At least every 10 seconds
		::Sleep(dwWaitHint);
	} // while

	return bResult;
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
		ret = WaitForState(SERVICE_RUNNING, schService);
		if (!ret)
		{
			WriteFileLog(_T("!!StartSMService,6,WaitForState fail,err=[%u]"),GetLastError());
		}
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
	CloseServiceHandle(SchSCManager);	//add by zfq,2013.06.08
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
					CloseServiceHandle(svc);	//add by zfq,2013.06.08
					CloseServiceHandle(scm);	//add by zfq,2013.06.08
					return nerr;
				}
				else
				{
					nret = WaitForState(SERVICE_STOPPED, svc);
					if (!nret)
					{
						CCommonFun::WriteFileLog(_T("!!StopSvc,6,WaitForState fail,err=[%u]"),GetLastError());
					}
				}
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return 0;
}

BOOL CCommonFun::SetFailedAction(CString strSvcName)
{
	BOOL bSuccess = FALSE;
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm!=NULL)
	{
		SC_HANDLE svc = OpenService(scm,strSvcName,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			SERVICE_FAILURE_ACTIONS sdBuf={0};

			sdBuf.lpRebootMsg=NULL;
			sdBuf.dwResetPeriod=3600*24;

			SC_ACTION action[3];

			action[0].Delay=5*1000;
			action[0].Type=SC_ACTION_RESTART;

			action[1].Delay=5*1000;
			action[1].Type=SC_ACTION_RESTART;
			action[2].Delay=5*1000;
			action[2].Type=SC_ACTION_RESTART;

			sdBuf.cActions=3;
			sdBuf.lpsaActions=action;
			sdBuf.lpCommand=NULL;

			if( !ChangeServiceConfig2(
				svc,                 
				SERVICE_CONFIG_FAILURE_ACTIONS, 
				&sdBuf) )                   
			{
				CCommonFun::WriteFileLog(_T("ChangeServiceConfig2 failed"));
			}
			else
			{
				bSuccess = TRUE;
				CCommonFun::WriteFileLog(_T("ChangeServiceConfig2 succeeded"));
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return bSuccess;
}

void CCommonFun::EnumeratePrinters(CStringArray& ary)
{
	DWORD dwNeeded = 0,
		dwReturned = 0;
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
	if (dwNeeded <= 0)
	{
		return;
	}

	LPBYTE lpBuffer = new BYTE[dwNeeded];
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, lpBuffer, dwNeeded, &dwNeeded, &dwReturned);

	PPRINTER_INFO_2 p2 = (PPRINTER_INFO_2)lpBuffer;

	for (DWORD x = 0; x < dwReturned; x++)
	{
		ary.Add(p2->pPrinterName);
		// 			if (p2->Attributes & PRINTER_ATTRIBUTE_DEFAULT)
		// 			{
		// 				nDefaultIndex = x;
		// 			}
		p2++;
	}

	delete[] lpBuffer;
}

BOOL CCommonFun::CreateProcessAndWait(LPTSTR cmd)
{
	STARTUPINFO si={0};
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
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

	//WaitForSingleObject(pi.hProcess, INFINITE);
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

	//1-注销当前用户；2-重新启动；3-关闭;

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
CString CCommonFun::GetUninstallString(CString sDisplayName)
{
	HKEY   hKey = NULL; 
	CString sKeyPath = _T("");

	if (IsWow64() && IsWinVistaOrLater())
		sKeyPath = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");  
	else
		sKeyPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");  
	DWORD  dwAttr = REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;  
	CString sUninstallString;
	DWORD dwIndex = 0;
	TCHAR szSubKeyName[MAX_PATH] = {0};
	DWORD dwSubKeyNameLen = MAX_PATH;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,sKeyPath,0,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)   
	{   
		while(RegEnumKeyEx(hKey,dwIndex ++,szSubKeyName,&dwSubKeyNameLen,0,NULL,NULL,0) != ERROR_NO_MORE_ITEMS)   
		{   
			if (_tcscmp(szSubKeyName ,_T("")) != NULL)
			{  
				CString sSubKeyPath;
				sSubKeyPath.Format(_T("%s\\%s"),sKeyPath,szSubKeyName);

				TCHAR szDisplayName[MAX_PATH] = {0};
				DWORD dwDisplayNameSize = sizeof(szDisplayName); 

				char szUninstall[MAX_PATH] = {0};
				DWORD dwUnistallSize = sizeof(szUninstall);

				HKEY hSubKey = NULL;
				//再打开，查找软件显示现实名称和卸载命令行   
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,sSubKeyPath,0,KEY_ALL_ACCESS,&hSubKey) == ERROR_SUCCESS)   
				{   
					RegQueryValueEx(hSubKey,_T("DisplayName"),0,&dwAttr,LPBYTE(szDisplayName),&dwDisplayNameSize);   
					RegQueryValueEx(hSubKey,_T("UninstallString"),0,&dwAttr,LPBYTE(szUninstall),&dwUnistallSize);

					if ((_tcsicmp(szDisplayName,sDisplayName) == 0))   
					{  
						sUninstallString = szUninstall;
						::RegCloseKey(hSubKey);
						break;
					}   
					::RegCloseKey(hSubKey);
				}   

			}   
			dwSubKeyNameLen = MAX_PATH;
			memset(szSubKeyName,0, sizeof(szSubKeyName));
		}//end   while   
		RegCloseKey(hKey);   
	} 
	theLog.Write(_T("last error is %d"),::GetLastError());
	return sUninstallString;
}

BOOL CCommonFun::UninstallSoftware(CString sDisplayName)
{
	CString s = GetUninstallString(sDisplayName);
	BOOL bRet = TRUE;
	do 
	{
		if (s.IsEmpty())
		{
			bRet = FALSE;
			break;
		}

		theLog.Write(_T("卸载数据库字符串 %s"),s);
		CStringArray ary;
		CCommonFun::StringSplit(s,&ary,_T(".exe "));

		// 得到卸载的参数
		CString sPath = _T("");
		CString sParam = _T("");
		if (ary.GetCount() > 1)
		{
			TCHAR szSysWindow[MAX_PATH] = {0};
			::GetSystemDirectory(szSysWindow,MAX_PATH);
			sPath.Format(_T("%s\\%s.exe"),szSysWindow,ary.GetAt(0));
			sParam.Format(_T("%s /q"),ary.GetAt(1));
			sParam.Replace(_T("/I"),_T("/X"));
		}
		else
		{
			sPath.Format(_T("%s"),ary.GetAt(0));
			sPath.TrimLeft(_T("\""));
			sPath.TrimRight(_T("\""));
		}

		theLog.Write(_T("卸载数据库组件 path = %s,param = %s"),sPath,sParam);	
		Install(sPath,sParam,INFINITE,SW_SHOW);
		//	bRet = CSecurityTool::CreateProcByCurUsr(sPath.GetBuffer(),sParam.GetBuffer(),SW_SHOWNORMAL);
		sParam.ReleaseBuffer();
		sParam.ReleaseBuffer();
	} while (0);
	return bRet;
}

// 添加共享目录
BOOL CCommonFun::AddNetShare(CString sShareName,CString sShareDesc,CString sDirPath)
{	
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // 如果目录不存在
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

// 判断共享目录是否存在
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
// 删除共享目录
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
    //lookupAccountName相关变量
    PSID pUserID=NULL;
    DWORD dUserID=0;
    TCHAR* pDomainName=NULL;
    DWORD dDomainName=0;
    SID_NAME_USE snuType;

    //第一步，取得传入的用户或用户组的SID
    BOOL check1=LookupAccountName(NULL,lpAccountName,pUserID,&dUserID,pDomainName,&dDomainName,&snuType);
    pUserID=(PSID)myHeapAlloc(dUserID);
    pDomainName=(TCHAR*)myHeapAlloc(dDomainName*sizeof(TCHAR));
    check1=LookupAccountName(NULL,lpAccountName,pUserID,&dUserID,pDomainName,&dDomainName,&snuType);

    //第二步，取得文件的安全描述符(SD)
    PSECURITY_DESCRIPTOR pFileSD=NULL;
    DWORD dFileSD=0;
    SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

    check1=GetFileSecurity(lpFileName,secInfo,pFileSD,0,&dFileSD);
    pFileSD=(PSECURITY_DESCRIPTOR)myHeapAlloc(dFileSD);
    check1=GetFileSecurity(lpFileName,secInfo,pFileSD,dFileSD,&dFileSD);

    //第三步，初始一个新的SD
    SECURITY_DESCRIPTOR newSD;
    InitializeSecurityDescriptor(&newSD,SECURITY_DESCRIPTOR_REVISION);

    //第四步，从SD中取出DACL
    BOOL bPresent;
    BOOL bDaclDefault;
    PACL pACL=NULL;
    GetSecurityDescriptorDacl(pFileSD,&bPresent,&pACL,&bDaclDefault);

    //第五步，取DACL的内存大小
    ACL_SIZE_INFORMATION aclInfo;
    aclInfo.AceCount=0;
    aclInfo.AclBytesFree=0;
    aclInfo.AclBytesInUse=sizeof(ACL);

    // 如果DACL不为空，则取其信息
    if(bPresent){
        GetAclInformation(pACL,&aclInfo,sizeof(ACL_SIZE_INFORMATION),AclSizeInformation);
    }

    //第六步，计算新的ACL的size
    DWORD dNewAcl=aclInfo.AclBytesInUse+sizeof(ACL_SIZE_INFORMATION)+GetLengthSid(pUserID)-2;

    //第七步，为新ACL分配内存
    PACL pNewAcl=(PACL)myHeapAlloc(dNewAcl);

    //第八步，初始化新的acl
    InitializeAcl(pNewAcl,dNewAcl,ACL_REVISION2);

    //第九步，如果文件（目录）的DACL有数据则拷贝数据到新的DACL
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
                continue;
            }

            AddAce(pNewAcl,ACL_REVISION,MAXDWORD,pTempAce,((PACE_HEADER)pTempAce)->AceSize);
            newAclIndex++;
        }
    }

    SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;
    AddAccessAllowedAceExFnPtr _AddAccessAllowedAceEx = NULL;
    //动态装载函数
    _AddAccessAllowedAceEx = (AddAccessAllowedAceExFnPtr)
        GetProcAddress(GetModuleHandle(_T("advapi32.dll")),
        "AddAccessAllowedAceEx");
    _AddAccessAllowedAceEx(pNewAcl, ACL_REVISION2,
        CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ,
        dw, pUserID);

    //拷贝继承的ACE
    if (bPresent&&aclInfo.AceCount)
    {
        for (;curAclIndex!=aclInfo.AceCount;curAclIndex++)
        {
            GetAce(pACL,curAclIndex,&pTempAce);
            AddAce(pNewAcl,ACL_REVISION,MAXDWORD,pTempAce,((PACE_HEADER)pTempAce)->AceSize);
        }
    }

    //把新的acl设置到新的sd中
    check1=SetSecurityDescriptorDacl(&newSD,TRUE,pNewAcl,FALSE);
    check1=SetFileSecurity(lpFileName,secInfo,&newSD);
    DWORD err=GetLastError();
    return TRUE;
}

#include "Lm.h"
#pragma comment(lib, "Netapi32.lib")

//添加用户
BOOL CCommonFun::AddUser(CString szUserName, CString szPassword, CString szComment /*= _T("")*/)
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
	ui.usri1_flags = UF_SCRIPT|UF_DONT_EXPIRE_PASSWD;	//默认密码永不过期
	ui.usri1_script_path = NULL;

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	DWORD dwError = 0;
	NET_API_STATUS status = NetUserAdd(NULL, 1, (LPBYTE)&ui, &dwError);
	if (status != NERR_Success /*&& status != NERR_UserExists*/)
	{
		CCommonFun::WriteFileLog(_T("AddUser fail,status=%u,dwError=%u\n"), status, dwError);
		return FALSE;
	}

	return TRUE;
}

//删除用户
BOOL CCommonFun::DeleteUser(CString szUserName)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = {0};
	MultiByteToWideChar(CP_GB2312,0,szUserName.GetString(),-1,pwcUserName,szUserName.GetLength());
#endif // UNICODE

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetUserDel(NULL, pwcUserName);
	if (status != NERR_Success /*&& status != NERR_UserNotFound*/)
	{
		theLog.Write(_T("DeleteUser fail,status=%u\n"), status);
		return FALSE;
	}

	return TRUE;
}

//添加用户到组中
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

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetLocalGroupAddMembers(NULL,pwcGroupName,3,(LPBYTE)&account,1);
	if (status != NERR_Success /*&& status!=ERROR_MEMBER_IN_ALIAS*/)
	{
		theLog.Write(_T("AddUserToGroup fail,status=%u\n"), status);
		return FALSE;
	}

	return TRUE;
}

//从组中删除用户
BOOL CCommonFun::DeleteUserFromGroup(CString szUserName, CString szGroupName)
{
#ifdef UNICODE
	LPWSTR pwcUserName = szUserName.GetBuffer();
	LPWSTR pwcGroupName = szGroupName.GetBuffer();
#else
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	WCHAR pwcGroupName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312,0,szUserName.GetString(),-1,pwcUserName,szUserName.GetLength());
	MultiByteToWideChar(CP_GB2312,0,szGroupName.GetString(),-1,pwcGroupName,szGroupName.GetLength());
#endif // UNICODE

	LOCALGROUP_MEMBERS_INFO_3 account; 
	account.lgrmi3_domainandname = pwcUserName;

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetLocalGroupDelMembers(NULL,pwcGroupName,3,(LPBYTE)&account,1);
	if (status != NERR_Success /*&& status!=ERROR_MEMBER_NOT_IN_ALIAS*/)
	{
		theLog.Write(_T("DeleteUserFromGroup fail,status=%u\n"), status);
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
		s.Format(_T("Install 失败 err = %d \n"),::GetLastError());
		WriteFileLog(s);
	}
	if (pszPath)
	{
		delete pszPath;
		pszPath = NULL;
	}
	return bOk;
}

//RunProcessByGUISessionId是提供给服务进程调用，其它进程可能失败。
BOOL CCommonFun::RunProcessByGUISessionId(CString sPath, CString sParam, DWORD dwSessionId /*= 0xFFFFFFFF*/, DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,1,sPath=%s,sParam=%s,dwSessionId=%u,dwWaitMilliseconds=%u,dwShowCmd=%d")
		, sPath, sParam, dwSessionId, dwWaitMilliseconds, dwShowCmd);
	if (dwSessionId == 0xFFFFFFFF)
	{
		//获取当前处于活动状态SessionId
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

	//获取当前处于活动状态用户的Token
	//To call this function successfully, the calling application must be running within 
	//the context of the LocalSystem account and have the SE_TCB_NAME privilege. 
	if (!WTSQueryUserToken(dwSessionId, &hToken))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,3,WTSQueryUserToken fail,dwSessionId=%u,err=%d"), dwSessionId, GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	//复制新的Token
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,4,DuplicateTokenEx fail,err=%d"), GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	//创建环境信息
	if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
	{
		WriteFileLog(_T("!!CCommonFun::RunProcessByGUISessionId,5,CreateEnvironmentBlock fail,err=%d"), GetLastError());
		CloseHandle(hTokenDup);
		CloseHandle(hToken);
		return FALSE;
	}

	//设置启动参数
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = _T("winsta0\\default");
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = dwShowCmd;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	//开始创建进程
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

	//附加操作,回收资源

	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,7,CreateProcessAsUser succ,dwWaitMilliseconds=%u"), dwWaitMilliseconds);

	//等待启动的进程结束
	WaitForSingleObject(pi.hProcess, dwWaitMilliseconds);

	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,8,end"));

	DestroyEnvironmentBlock(pEnv);
	CloseHandle(hTokenDup);
	CloseHandle(hToken);

	return TRUE;
}

//获取指定文件所在的目录,"C:\test\test.doc" => ""C:\test""
CString CCommonFun::GetFileDirByFullPath(CString szFullPath)
{
	szFullPath.Replace(_T("/"), _T("\\"));
	szFullPath.TrimRight(_T("\\"));
	int nPos = szFullPath.ReverseFind('\\');
	return szFullPath.Left(nPos);
}

//获取指定文件名,"C:\test\test.doc" => "test.doc"
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

//获取当前活动的session ID
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

//获取进程的session ID
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

typedef void (WINAPI *LPFN_GETNATIVESYSTEMINFO) (LPSYSTEM_INFO);
BOOL CCommonFun::IsWindowsX64()
{
	LPFN_GETNATIVESYSTEMINFO fnGetNativeSystemInfo =
		(LPFN_GETNATIVESYSTEMINFO)GetProcAddress(
			GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");

	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));

	BOOL bIsWindowsX64 = FALSE;
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		bIsWindowsX64 = TRUE;
	}

	return bIsWindowsX64;
}

BOOL CCommonFun::IsProcAlive(DWORD dwProID)	//add by zfq,判断某个进程是否存在
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

//获取本地路径的一个网络路径，方便其它计算机能够正常访问
BOOL CCommonFun::GetSharePath(CString szLocalPath, CString szShareName, CString& szShareDir, CString& szSharePath)
{
	BOOL bRet = TRUE;
	CString szLocalDir = GetFileDirByFullPath(szLocalPath);
	CString szLocalFile = GetFileNameByFullPath(szLocalPath);
	CCommonFun::WriteFileLog(_T("CCommonFun::GetSharePath,szLocalPath=%s,szShareName=%s,szLocalDir=%s,szLocalFile=%s")
		, szLocalPath, szShareName, szLocalDir, szLocalFile);

	//设置权限
	CCommonFun::AddRights(szLocalDir.GetBuffer(), _T("Everyone"), GENERIC_ALL|STANDARD_RIGHTS_ALL);
	if (PathFileExists(szLocalPath))
	{
		CCommonFun::AddRights(szLocalPath.GetBuffer(), _T("Everyone"), GENERIC_ALL|STANDARD_RIGHTS_ALL);
	}

	//删除共享目录
	if (CheckNetShare(szLocalDir))
	{
		DelNetShare(szShareName);
	}

	//创建共享目录
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
		WideCharToMultiByte(CP_GB2312,0, wszText, -1, szText, len, NULL, NULL);

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
		WideCharToMultiByte(CP_UTF8,0, wszText, -1, szText, len, NULL, NULL);

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
		WideCharToMultiByte(CP_UTF8,0, pwszSource, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] szText;
	}
	return szRetA;
}

//获取日志目录
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

void CCommonFun::SetDoNetDBStr(CString szDbStr)
{
	TCHAR pSysPath[MAX_PATH];
	memset(pSysPath, 0, MAX_PATH);
	if (FAILED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, pSysPath)))
	{
		WriteFileLog(_T("CCommonFun::SetDoNetDBStr,SHGetFolderPath fail(%s)：%d"), pSysPath, GetLastError());
		return;
	}
	//GetSystemDirectory(pSysPath,MAX_PATH);
	CString szPath = pSysPath;
	CString sDir = szPath + _T("\\iSecStar");

	if (!PathFileExists(sDir))
		CreateDirectory(sDir, NULL);

	szPath = sDir + _T("\\iSec_PrintConfig.Dat");

	::DeleteFile(szPath);

	CPWDCry cry;
	char* pStr = cry.CryptPWD((char*)CCommonFun::UnicodeToMultiByte(szDbStr).GetString());

	CFile file;
	if (file.Open(szPath, CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate))
	{
		file.Write(pStr, strlen(pStr));
		file.Close();
	}
	else
	{
		WriteFileLog(_T("CCommonFun::SetDoNetDBStr,Open fail(%s)：%d"), szPath, GetLastError());
	}
	delete[] pStr;
}

void CCommonFun::GetDoNetDBStr(CString& szDbStr)
{
	TCHAR pSysPath[MAX_PATH];
	memset(pSysPath, 0, MAX_PATH);
	if (FAILED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, pSysPath)))
	{
		WriteFileLog(_T("CCommonFun::GetDoNetDBStr,SHGetFolderPath fail(%s)：%d"), pSysPath, GetLastError());
		return;
	}
	//GetSystemDirectory(pSysPath,MAX_PATH);

	CString szPath = pSysPath;
	szPath += _T("\\iSecStar\\iSec_PrintConfig.Dat");

	CFile file;
	if (file.Open(szPath, CFile::modeRead))
	{
		int nFileLen = file.GetLength();
		char* pByte = new char[nFileLen + 1];
		memset(pByte, 0, nFileLen + 1);
		int nRead = file.Read(pByte, nFileLen);
		file.Close();

		char* pStr = new char[nFileLen + 1];
		memset(pStr, 0, nFileLen + 1);

		CPWDCry cry;
		cry.UnCryptPWD(pByte, pStr, nFileLen);
		
		szDbStr = CCommonFun::MultiByteToUnicode(pStr);

		delete[] pByte;
		delete[] pStr;

		if ((nRead <= 0) || (nRead < nFileLen))
		{
			WriteFileLog(_T("##CCommonFun::GetDoNetDBStr,nRead=%d,nFileLen=%d"), nRead, nFileLen);
		}
	}
	else
	{
		WriteFileLog(_T("!!CCommonFun::GetDoNetDBStr,Open fail(%s)：%d"), szPath, GetLastError());
	}
}

BOOL CCommonFun::NotifyFromWeb(int nMask)
{
	WriteFileLog(_T("CCommonFun::NotifyFromWeb %d"), nMask);
	//return 1;
	BOOL bSuc = FALSE;
	CBaseSock ServerSocket;
	int nWebPort = 0;

	int nRet = 0;
	CStringA sLocalIP, sHost, sIP;
	EQUIP_INFO equip = { 0 };
	sLocalIP = "127.0.0.1";
	sHost = "IIS-Host";
	CString sServerIP = _T("127.0.0.1");
	nWebPort = POLICY_TCP_PORT;

	int nSize = sizeof(NET_PACK_HEAD) + sizeof(equip);
	BYTE* pBuf = new BYTE[nSize];
	if (!pBuf)
	{
		WriteFileLog(_T("NotifyFromWeb err 000 %d"), nMask);
		return FALSE;
	}

	ZeroMemory(pBuf, nSize);
	NET_PACK_HEAD *pHead = (NET_PACK_HEAD*)pBuf;
	pHead->nPackMask = PACK_SMALL;
	pHead->nIndentify = PACKINDENTIFY;

	try
	{
		do
		{
			if (!ServerSocket.Create())
			{
				WriteFileLog(_T("Create error"));
				break;
			}
			
			ServerSocket.SetTimeOut(6000);

			//通知policyserver
			if (!ServerSocket.Connect(sServerIP, nWebPort))//这里只是处理了80端口以后在修改中端口号要从数据报（pBufferSend）中得出
			{
				WriteFileLog(_T("Connect error"));
				break;
			}

			ServerSocket.KillTimeOut();

			equip.nEquipType = ISEC_PLT_WEB;
			strcpy_s(equip.szIP, sLocalIP);
			strcpy_s(equip.szHost, sHost);
			pHead->nMainCmd = WM_CHECK_EQUIP;
			CopyMemory(pHead + 1, &equip, sizeof(equip));
			pHead->nPackBodySize = sizeof(equip);
			if (ServerSocket.Send(pBuf, sizeof(NET_PACK_HEAD) + pHead->nPackBodySize) < 1)
			{
				WriteFileLog(_T("Send 22 err %d"), ::WSAGetLastError());
				break;
			}
			pHead->nMainCmd = WM_POLICY_CHANGED;
			pHead->nSubCmd = nMask;
			pHead->nPackBodySize = 0;
			if (ServerSocket.Send(pBuf, sizeof(NET_PACK_HEAD) + pHead->nPackBodySize) < 1)
			{
				WriteFileLog(_T("Send 33 err %d"), ::WSAGetLastError());
				break;
			}

			::Sleep(2000);
			bSuc = TRUE;
			WriteFileLog(_T("NotifyFromWeb suc 000 %d"), nMask);
		} while (0);

	}
	catch (...)
	{
	}

	if (pBuf)
	{
		delete[] pBuf;
	}

	WriteFileLog(_T("NotifyFromWeb suc 222"));
	return bSuc;

}

//运行命令行并获取输出结果
BOOL CCommonFun::RunAndGetResult(CString szRunCmd, CString& szResult)
{
	BOOL bRet = FALSE;
	HANDLE hReadPipe = INVALID_HANDLE_VALUE;
	HANDLE hWritePipe = INVALID_HANDLE_VALUE;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;  //使用系统默认的安全描述符	
	sa.bInheritHandle = TRUE;  //创建的进程继承句柄

	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))  //创建匿名管道
	{
		WriteFileLog(_T("!!CCommonFun::RunAndGetResult, CreatePipe fail, error=%u"), GetLastError());
		return FALSE;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;	 //新创建进程的标准输出连在写管道一端
	si.wShowWindow = SW_HIDE;  //隐藏窗口	
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if (!CreateProcess(NULL, szRunCmd.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))  //创建子进程
	{
		WriteFileLog(_T("!!CCommonFun::RunAndGetResult, CreateProcess fail, szRunCmd=[%s],error=%u"), szRunCmd, GetLastError());
		return FALSE;
	}
	else
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	CloseHandle(hWritePipe);  //关闭管道句柄

	szResult.Empty();
	while (TRUE)
	{
		BYTE pBuffer[4096] = { 0 };
		DWORD dwRead = 0;
		//读取管道
		if (!ReadFile(hReadPipe, pBuffer, 4095, &dwRead, NULL))
			break;

		CString szBuffer(pBuffer);
		szResult += szBuffer;
	}
	CloseHandle(hReadPipe);
	return TRUE;
}

#include <Wincrypt.h>
#pragma comment(lib,"Crypt32.lib")

CString GetCertString(PCCERT_CONTEXT pCertContext, DWORD dwType, DWORD dwFlags)
{
	CString szName;

	if (!pCertContext)
	{
		CCommonFun::WriteFileLog(_T("!!GetCertString, pCertContext=%p"), pCertContext);
		return szName;
	}

	DWORD dwBuffer = CertGetNameString(pCertContext, dwType, dwFlags, NULL, NULL, 0);
	TCHAR* pBuffer = new TCHAR[dwBuffer];
	memset(pBuffer, 0x0, sizeof(TCHAR)*dwBuffer);
	CertGetNameString(pCertContext, dwType, dwFlags, NULL, pBuffer, dwBuffer);

	szName.Format(_T("%s"), pBuffer);
	delete[] pBuffer;

	return szName;
}

CString GetTimeStr(COleDateTime time)
{
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

//检测指定证书是否存在
BOOL CCommonFun::CheckCertificateExist(CString szSubsystemProtocol, CString szIssuer, CString szUser)
{
	WriteFileLog(_T("CCommonFun::CheckCertificateExist,1,szSubsystemProtocol=%s,szIssuer=%s,szUser=%s")
		, szSubsystemProtocol, szIssuer, szUser);

	BOOL bExist = FALSE;
	HCERTSTORE hSystemStore = NULL;
	PCCERT_CONTEXT pDesiredCert = NULL;

	/*
	CA		【当前用户\中级证书颁发机构\证书】	Certification authority certificates.
	MY		【当前用户\个人\证书】	A certificate store that holds certificates with associated private keys.
	ROOT	【当前用户\受信任的根证书颁发机构\证书】	Root certificates.
	SPC		Software Publisher Certificate.
	*/
	hSystemStore = CertOpenSystemStore(NULL, szSubsystemProtocol);	//CA,MY,ROOT,SPC
	if (!hSystemStore)
	{
		WriteFileLog(_T("!!CCommonFun::CheckCertificateExist,2,CertOpenSystemStore fail,error=%u"), GetLastError());
	}
	else
	{
		CStringW wszIssuer(szIssuer);
		while (TRUE)
		{
			pDesiredCert = CertFindCertificateInStore(hSystemStore, X509_ASN_ENCODING, 0, CERT_FIND_ISSUER_STR, wszIssuer.GetString(), pDesiredCert);
			if (!pDesiredCert)
			{
				WriteFileLog(_T("!!CCommonFun::CheckCertificateExist,3,CertFindCertificateInStore fail,error=%u"), GetLastError());
				break;
			}

			/*
			COleDateTime oNotBefore(pDesiredCert->pCertInfo->NotBefore);
			COleDateTime oNotAfter(pDesiredCert->pCertInfo->NotAfter);
			COleDateTime oNow = COleDateTime::GetCurrentTime();
			if ((oNow < oNotBefore) || (oNow > oNotAfter))
			{
				WriteFileLog(_T("!!CCommonFun::GetCertUser,4,证书过期,oNow=%s,oNotBefore=%s,oNotAfter=%s")
					, GetTimeStr(oNow), GetTimeStr(oNotBefore), GetTimeStr(oNotAfter));
				continue;
			}
			*/

			/* 颁发者信息
			CString szIssuerName = GetCertString(pDesiredCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerMail = GetCertString(pDesiredCert, CERT_NAME_EMAIL_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerRnd = GetCertString(pDesiredCert, CERT_NAME_RDN_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerAttr = GetCertString(pDesiredCert, CERT_NAME_ATTR_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerNameFriendly = GetCertString(pDesiredCert, CERT_NAME_FRIENDLY_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerDns = GetCertString(pDesiredCert, CERT_NAME_DNS_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerUrl = GetCertString(pDesiredCert, CERT_NAME_URL_TYPE, CERT_NAME_ISSUER_FLAG);
			CString szIssuerUpn = GetCertString(pDesiredCert, CERT_NAME_UPN_TYPE, CERT_NAME_ISSUER_FLAG);
			*/

			//颁发给信息
			CString szUserName = GetCertString(pDesiredCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0);
			if (szUserName.CompareNoCase(szUser) == 0)
			{
				bExist = TRUE;
				break;
			}
		}
	}

	if (pDesiredCert)
		CertFreeCertificateContext(pDesiredCert);
	if (hSystemStore)
		CertCloseStore(hSystemStore, CERT_CLOSE_STORE_CHECK_FLAG);

	return bExist;
}

//导入证书
BOOL CCommonFun::ImoprtCertificate(CString szSubsystemProtocol, CString szCertPath, BOOL bReplaceExisting /*= FALSE*/)
{
	//WriteFileLog(_T("CCommonFun::ImoprtCertificate,1,szSubsystemProtocol=%s,szCertPath=%s,bReplaceExisting=%d")
	//	, szSubsystemProtocol, szCertPath, bReplaceExisting);

	BOOL bSucc = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BYTE* pFileData = NULL;
	DWORD dwFileSize = 0;
	BYTE* pBinaryData = NULL;
	DWORD dwBinarySize = 0;
	DWORD dwError = 0;
	HCERTSTORE hFileStore = 0;
	PCCERT_CONTEXT pCertContext = NULL;
		
	do 
	{
		hFile = CreateFile(szCertPath, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,2,CreateFile fail,error=%u"), GetLastError());
			break;
		}

		dwFileSize = GetFileSize(hFile, NULL);
		pFileData = new BYTE[dwFileSize];

		ReadFile(hFile, pFileData, dwFileSize, &dwFileSize, NULL);

		CryptStringToBinary((LPCTSTR)pFileData, dwFileSize, CRYPT_STRING_BINARY, NULL, &dwBinarySize, NULL, NULL);
		pBinaryData = new BYTE[dwBinarySize];
		if (!CryptStringToBinary((LPCTSTR)pFileData, dwFileSize, CRYPT_STRING_BINARY, pBinaryData, &dwBinarySize, NULL, NULL))
		{
			WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,3,CryptStringToBinary fail,error=%u"), GetLastError());
			break;
		}

		pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING, pBinaryData, dwBinarySize);
		if (!pCertContext)
		{
			WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,4,CertCreateCertificateContext fail,error=%u"), GetLastError());
			break;
		}

		CStringW szSubsystemProtocolW(szSubsystemProtocol);
		hFileStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, szSubsystemProtocolW);
		if (!hFileStore)
		{
			WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,5,CertOpenStore fail,error=%u"), GetLastError());
			break;
		}

		if (!CertAddCertificateContextToStore(hFileStore, pCertContext, CERT_STORE_ADD_NEW, 0))
		{
			dwError = GetLastError();
			if (CRYPT_E_EXISTS == dwError)
			{
				if(bReplaceExisting)
				{
					if (!CertAddCertificateContextToStore(hFileStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, 0))
					{
						WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,6,CertAddCertificateContextToStore fail,error=%u"), GetLastError());
						break;
					}
				}
			}
			else
			{
				WriteFileLog(_T("!!CCommonFun::ImoprtCertificate,7,CertAddCertificateContextToStore fail,error=%u"), GetLastError());
				break;
			}
		}

		bSucc = TRUE;
		//WriteFileLog(_T("CCommonFun::ImoprtCertificate,succ"));
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	if (pFileData)
	{
		delete[] pFileData;
	}
	if (pBinaryData)
	{
		delete[] pBinaryData;
	}

	if (hFileStore)
	{
		CertCloseStore(hFileStore, 0);
	}

	if (pCertContext)
	{
		CertFreeCertificateContext(pCertContext);
	}
	
	return bSucc;
}

CString CCommonFun::ConverGuidToStr(GUID guid)
{
	LPOLESTR   pstrGuid;
	int   i = StringFromCLSID(guid, &pstrGuid);
	CString   s(pstrGuid);
	if (NULL != pstrGuid)
		CoTaskMemFree(pstrGuid);
	return s;
}
//产生Guid
CString CCommonFun::NewGuid()
{
	GUID   guid;
	CoCreateGuid(&guid);
	LPOLESTR   pstrGuid;
	int   i = StringFromCLSID(guid, &pstrGuid);
	CString   s(pstrGuid);
	if (NULL != pstrGuid)
		CoTaskMemFree(pstrGuid);
	return s;
}

BOOL CCommonFun::NewGuidByStr(CString sID, GUID &ID)
{
	ASSERT(!sID.IsEmpty());
	//	ASSERT(sID.GetLength() != 16);
#ifndef UNICODE
	int nLen = sID.GetLength() + 1;
	WCHAR* pBuf = new WCHAR[nLen];
	memset(pBuf, 0, nLen * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, sID.GetBuffer(), -1, pBuf, nLen);
	CLSIDFromString((LPOLESTR)pBuf, &ID);
	sID.ReleaseBuffer();
#else
	CLSIDFromString((LPOLESTR)sID.GetString(), &ID);
#endif // !UNICODE
	return TRUE;
}
