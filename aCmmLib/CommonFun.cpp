// CommonFun.cpp: implementation of the CCommonFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommonFun.h"
#include <comutil.h>
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Rpcrt4.lib")
//#include <Nb30.h>
//#include "Ado.h"
#include <WinSvc.h>
#include <stdlib.h>
#include <MMSystem.h>
#include "SecurityTool.h"
#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#pragma comment(lib,"..\\public\\lib\\debug\\strmbasd.lib")
#else
#pragma comment(lib,"..\\public\\lib\\release\\strmbase.lib")
#endif

#include <Winspool.h>

#include   <lm.h>
#pragma comment(lib,"Netapi32.lib")

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include "md5wrapper.h"
#include "wmmsg.h"
#include "FileLog.h"
#include "pagerSizemsg.h"
#define SAFE_RELEASE(p) if(p) {p->Release();p = NULL;}

#include <Shobjidl.h>
#include <intshcut.h>

//RunProcessByGUISessionId接口内部需要用到Wtsapi32.lib和Userenv.lib
#include <UserEnv.h>
#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

//实现Ping接口的依赖库
#include <winsock2.h>
#include <IPExport.h>
#include <icmpapi.h>
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

// adsi header
#include <activeds.h>
#include <Iads.h>
#include <Lm.h>
#include <Adshlp.h>

// adsi lib
#pragma comment (lib, "Activeds")
#pragma comment(lib, "adsiid")

#include "CardAnalyst.h"
#include "Md5Encode.h"
#include "Sha1Ex.h"

#include <atlconv.h>
#include <atlsecurity.h>

#ifndef IOCTL_STORAGE_QUERY_PROPERTY
//#include <ntddstor.h>
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum _STORAGE_PROPERTY_ID {
	StorageDeviceProperty = 0,
	StorageAdapterProperty,
	StorageDeviceIdProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

typedef enum _STORAGE_QUERY_TYPE {
	PropertyStandardQuery = 0, 
	PropertyExistsQuery, 
	PropertyMaskQuery, 
	PropertyQueryMaxDefined 
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;


typedef struct _STORAGE_PROPERTY_QUERY {
	STORAGE_PROPERTY_ID  PropertyId;
	STORAGE_QUERY_TYPE  QueryType;
	UCHAR  AdditionalParameters[1];
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;


typedef struct _STORAGE_DEVICE_DESCRIPTOR {
	ULONG  Version;
	ULONG  Size;
	UCHAR  DeviceType;
	UCHAR  DeviceTypeModifier;
	BOOLEAN  RemovableMedia;
	BOOLEAN  CommandQueueing;
	ULONG  VendorIdOffset;
	ULONG  ProductIdOffset;
	ULONG  ProductRevisionOffset;
	ULONG  SerialNumberOffset;
	STORAGE_BUS_TYPE  BusType;
	ULONG  RawPropertiesLength;
	UCHAR  RawDeviceProperties[1];
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)

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

void CCommonFun::StringSplit(CString str, CStringArray *psArray, TCHAR chDelimiter, int nCount)
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
#if 0
void CCommonFun::StringSplit(const CString sSource, CStringArray *parArray , CString sSplitter)
{
	int nSplitLen = sSplitter.GetLength();

	CString sTemp(sSource);

	while(sTemp)
	{
		int nSite = sTemp.Find(sSplitter);
		if(nSite == -1)
		{
			parArray->Add(sTemp);
			break;
		}

		CString sNew = sTemp.Left(nSite);
		parArray->Add(sNew);

		sTemp = (LPCTSTR)sTemp + nSplitLen +nSite;
	}
}
#endif
void CCommonFun::StringSplit(const CString sSource, CStringArray *parArray , CString sSplitter,BOOL bTriNUll)
{
	int nSplitLen = sSplitter.GetLength();
	// [11/18/2011 Songqx]
	// 死循环
	if (0 == nSplitLen)
		return;

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

BOOL CCommonFun::DeleteFileOrDir(CString szPath)	//add by zfq, 删除一个文件或目录
{
	BOOL BRet = FALSE;
	if(!::PathFileExists(szPath))
	{
		ASSERT(0);
		return FALSE;
	}

	if(!PathIsDirectory(szPath))
	{//是目录
		DeleteDir(szPath, FALSE);
		BRet = TRUE;
	}
	else
	{//是文件
		BRet = ::DeleteFile(szPath);
	}
	if (!BRet)
	{
		WriteFileLog(_T("!!CCommonFun::DeleteFileOrDir,fail,err=%d"),::GetLastError());
	}
	return BRet;
}


CString CCommonFun::GetNowTime()	//add by zfq,得到当前时间字符串"2009-10-01 09:00:00"
{
	CTime oTime = CTime::GetCurrentTime();
	CString szTime;
	szTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"), oTime.GetYear(), oTime.GetMonth(), oTime.GetDay()
		, oTime.GetHour(), oTime.GetMinute(), oTime.GetSecond());
	return szTime;
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

//add by zfq,判断某个磁盘驱动器是否存在,且是正常的硬盘驱动器分区
BOOL CCommonFun::IsDiskExists(BYTE byDisk)	
{
	bool bGet = false;
	if((byDisk >= 'C' && byDisk <= 'Z')
		|| (byDisk >= 'c' && byDisk <= 'z'))
	{
		bGet = true;
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}

	CString szDisk;
	szDisk.Format(_T("%c:\\"), byDisk);

	BOOL BRet = FALSE;
	if(DRIVE_FIXED == ::GetDriveType(szDisk))
	{
		BRet = TRUE;
	}

	return BRet;
}

BOOL CCommonFun::IsDebug()
{
	LPCTSTR strPath = PROJECT_REGISTY_KEY;
	LPCTSTR strKey=_T("DebugLog");
	DWORD dwRet, dwSize=sizeof(DWORD), dwType=REG_DWORD;
	HKEY hKey;

	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		(LPCTSTR)strPath,
		0,
		KEY_READ,
		&hKey)!= ERROR_SUCCESS)
		return 0;

	if(::RegQueryValueEx(hKey,
		(LPCTSTR)strKey,
		NULL,
		&dwType,
		(LPBYTE)&dwRet,
		&dwSize) != ERROR_SUCCESS)
		dwRet = 0;

	RegCloseKey(hKey);

	return dwRet;
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
void CCommonFun::WriteFileLog(CString sLog)
{
	if( sLog.GetLength() > 250 )
		TRACE(sLog.Left(250));
	else
		TRACE(sLog);
	TRACE(_T("\n"));

	if( !CFileLog::IsDebug() )
		return;

	CString sSysDir;
	sSysDir = GetLogDir();	

	CFileFind fd;

	if( !fd.FindFile(sSysDir) )
	{
		if( !::CreateDirectory(sSysDir,NULL) )
			return;
	}

	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));

	CString sFileName;
	sFileName.Format(_T("%s\\Xab_Common_%s.Log"),sSysDir,sDate);


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
		sTmp.Format(_T("%s,	 %s\r\n"),sTime,sLog);
		fileLog.SeekToEnd();
		fileLog.Write(sTmp,sTmp.GetLength()*sizeof(TCHAR));
		fileLog.Close();
	}
	catch (...) 
	{
		;
	}

}
// 查验头几个子节是否为所需要的格式
BOOL CCommonFun::CheckHeaderWithParameter(CString sFilePath,CString sParameter)
{
	BOOL bRet = FALSE;
	do 
	{
		CFileException fe;
		CFile mSaveFile;
		CString sType;
		if (!mSaveFile.Open(sFilePath,CFile::modeRead,&fe))
			break;
		CArchive ar(&mSaveFile,CArchive::load);
		ar>>sType;
		if (sType.Compare(sParameter) == 0)
			bRet = TRUE;
		ar.Flush();
		ar.Close();
		mSaveFile.Close();
	} while (0);
	return bRet;
}

CString CCommonFun::GetInvalidData()
{
	CString sData;
	sData.LoadString(IDS_INVALID_TIME);
	return sData;
}

void CCommonFun::ReplaceAll(CString &sInfo)
{
	sInfo.Replace(_T("\r\n"),_T("<br>"));
	sInfo.Replace(_T(" "),_T("&nbsp;"));
}
//--------------------------------
//将数字转变成字符串
//--------------------------------
CString CCommonFun::NToS(int nNum)
{
	CString sTmp;
	sTmp.Format(_T("%d"),nNum);
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

//--------------------------------
//将字符串转变成BSTR
//--------------------------------
BSTR CCommonFun::SToB(const char* pSrc)
{
	return _com_util::ConvertStringToBSTR(pSrc);
}

CString CCommonFun::GetVariantStr(_variant_t varValue)
{
	if( varValue.vt == VT_NULL || varValue.vt == VT_EMPTY )
		varValue.SetString("");
	varValue.ChangeType(VT_BSTR);
	return CString(varValue.bstrVal);

}

int CCommonFun::SToN(CString str)
{
	return _tstoi(str);
}

double CCommonFun::SToF(CString str)
{
	return _tstof(str);
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
	StringSplit(sPath,&sAry,_T("\\"));

	if( sAry.GetSize() < 2 )
	{
		TRACE(_T("请输入一个绝对路径！\n"));
		return FALSE;
	}

	CString sDriver = sAry.GetAt(0);
	if( sDriver.GetLength() != 2 || sDriver.GetAt(sDriver.GetLength()-1) != ':' )
	{
		TRACE(_T("请输入一个带盘符的路径！\n"));
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
			TRACE(_T("创建目录")+sDir+_T("失败！\n"));
			return FALSE;
		}
	}
	TRACE(_T("创建目录")+sDir+_T("成功！\n"));
	return TRUE;
}


CString CCommonFun::ChangePathToShort(CString sPath)
{
	TCHAR chShortPathName[MAX_PATH];
	DWORD dw = GetShortPathName(sPath,chShortPathName,MAX_PATH);
	DWORD dwErr = GetLastError();
	CString sTmp = chShortPathName;

	return sTmp;
}

BOOL CCommonFun::RegOneCom(LPCTSTR lpszPathName)
{
	TCHAR szDir[MAX_PATH*2+16]={0};

	_tcscat(szDir,_T("regsvr32.exe /s \""));
	_tcscat(szDir,lpszPathName);
	_tcscat(szDir,_T("\""));

	STARTUPINFO StartupInfo= {0};
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb=sizeof(STARTUPINFO);	

	if (!CreateProcess(NULL, (LPTSTR)szDir, NULL,NULL,FALSE,0,NULL,NULL, &StartupInfo, &ProcessInfo))
	{		 
		return FALSE;
	}
	CCommonFun::WriteFileLog(_T("RegOneCom szDir = %s"),szDir);
	WaitForSingleObject(ProcessInfo.hProcess ,3000);
	CloseHandle (ProcessInfo.hThread);
	CloseHandle (ProcessInfo.hProcess);

	return TRUE;
}

BOOL CCommonFun::UnRegOneCom(LPCTSTR lpszPathName)
{
	TCHAR szDir[MAX_PATH*2+16]={0};

	_tcscat(szDir,_T("regsvr32.exe /u /s \""));
	_tcscat(szDir,lpszPathName);
	_tcscat(szDir,_T("\""));

	STARTUPINFO StartupInfo= {0};
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb=sizeof(STARTUPINFO);	

	if (!CreateProcess(NULL, (LPTSTR)szDir, NULL,NULL,FALSE,0,NULL,NULL, &StartupInfo, &ProcessInfo))
		return FALSE;
	WaitForSingleObject(ProcessInfo.hProcess ,3000);
	CloseHandle (ProcessInfo.hThread);
	CloseHandle (ProcessInfo.hProcess);

	return TRUE;
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

CString CCommonFun::GetFileNameByPathName(CString sPathName)
{
	if( sPathName.IsEmpty() ) return _T("");
	CStringArray sAry;
	sPathName.Replace(_T("//"), _T("\\"));
	StringSplit(sPathName,&sAry,_T("\\"));
	if( sAry.GetSize() > 0 )
		return sAry.GetAt(sAry.GetSize()-1);
	return sPathName;
}

CString CCommonFun::GetFileNameWithoutExt(CString sPathName)
{
	if (sPathName.IsEmpty()) return _T("");
	CStringArray sAry;
	sPathName.Replace(_T("//"), _T("\\"));
	StringSplit(sPathName, &sAry, _T("\\"));
	CString szFileName = sAry.GetSize() > 0 ? sAry.GetAt(sAry.GetSize() - 1) : sPathName;
	sAry.RemoveAll();
	StringSplit(szFileName, &sAry, _T("."));
	szFileName = sAry.GetSize() > 1 ? sAry.GetAt(sAry.GetSize() - 2) : sPathName;
	return szFileName;
}

BOOL CCommonFun::IsWin2003Enterprise()
{
	HKEY hKey;
	TCHAR szProductType[80];
	TCHAR szProductSuite[200];
	DWORD dwBufLen = sizeof(szProductType);
	unsigned long lType = 0;
	RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
		0, KEY_QUERY_VALUE, &hKey );
	lType = REG_SZ;
	RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
		(LPBYTE) szProductType, &dwBufLen);
	dwBufLen = sizeof(szProductSuite);
	lType = REG_MULTI_SZ;
	RegQueryValueEx( hKey, _T("ProductSuite"), NULL, &lType,
		(LPBYTE) szProductSuite, &dwBufLen);

	//自定义一个键，控制是否Win2003Enterprise
	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest = 0;
	LONG lReturn = RegQueryValueEx (hKey, _T("vod"), NULL,
		&dwType, (BYTE *) &dwDest, &dwSize);
	int nVal = (int)dwDest;

	RegCloseKey( hKey );   

	CString str;
	for( int i=0;i<(int)dwBufLen;i++)
	{
		TCHAR ch = szProductSuite[i];
		if( ch == '\0' )
		{
			str += _T(" ");
		}
		else
		{
			str += ch;
		}

	}
	if( str.Find(_T("Enterprise")) >= 0 && nVal != 1)
	{
		return TRUE;
	}

	return FALSE;

}

//得到文件的扩展名称
//如abc.txt返回txt
CString CCommonFun::GetFileExt(CString sFile)
{
	CString sTmp;
	int nPos = sFile.ReverseFind('.');
	sTmp = sFile.Right(sFile.GetLength()-nPos-1);
	return sTmp;
}

//得到文件的扩展名称
//如"abc.txt"返回".txt"
CString CCommonFun::GetFileExt3(CString sFile)
{
	CString sTmp;
	int nPos = sFile.ReverseFind('.');
	sTmp = sFile.Right(sFile.GetLength()-nPos);
	return sTmp;
}

BOOL CCommonFun::GetFileExt2(CString szFile, CString &szFileName, CString &szExtName)//得到文件的扩展名称,如"abc.txt"返回".txt"
{
	int nPos = szFile.ReverseFind('.');
	if(0 >= nPos)
	{
		return FALSE;
	}

	int nLen = szFile.GetLength();
	szFileName = szFile.Left(nPos);
	szExtName = szFile.Right(nLen - nPos);
	return TRUE;
}

/********************************************************************/
/*																	*/
/* Function name : CreateLocalDirectory								*/
/* Description   : Create directory tree.							*/
/*																	*/
/********************************************************************/
BOOL CCommonFun::CreateLocalDirectory(LPCTSTR lpszDirectory)
{
	CString strResult = lpszDirectory;

	CString strDir;
	BOOL bResult;
	// create directory structure one part at a time
	while (strResult != _T(""))
	{
		strDir += strResult.Left(strResult.Find(_T("\\"))+1);
		strResult = strResult.Mid(strResult.Find(_T("\\"))+1);
		bResult = CreateDirectory(strDir, 0);
	}
	return bResult;
}



DWORD CCommonFun::GetFileSizeByte(CString sPathName)
{

	CFile file;
	DWORD dwSize=0;
	if( sPathName.IsEmpty() )
		return 0;

	try
	{
		CFileException ex;

		if( file.Open( sPathName, CFile::modeRead|CFile::shareDenyNone, &ex))
		{
			dwSize = file.GetLength();
			file.Close();
		}
	}
	catch(...)
	{
	}
	return dwSize;
}

BOOL CCommonFun::GetFileSizeEx(CString szPathName, UINT64& un64FileSize)	//add by zfq,得到文件长度,支持大文件
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(szPathName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	VERIFY(FindClose(hFind));


	//支持超过4G的文件
	UINT64 un64Tmp1 = MAXDWORD;
	un64Tmp1++;
	UINT64 un64High = findFileData.nFileSizeHigh;
	un64High *= un64Tmp1;
	UINT64 un64Low = findFileData.nFileSizeLow;
	un64FileSize = un64High + un64Low;

	return TRUE;
}

double CCommonFun::GetFileSizeM(CString sPathName)
{
	DWORD dwSize = GetFileSizeByte(sPathName);
	double dbSize = ((double)dwSize)/(double(1024*1024));

	return dbSize;
}

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

HANDLE CCommonFun::GetProcessHandle(CString szProcessName)
{//外部需要释放句柄
	DWORD dwPid = 0;
	HANDLE hProcess = NULL;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//printf("Error: CreateToolhelp32Snapshot (of processes)\r\n");
		return NULL;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		//printf("Error: Process32First\r\n"); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return NULL;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	int namelen = 200;
	char name[201] = { 0 };
	do
	{
		if (!wcscmp(pe32.szExeFile, szProcessName)) {
			dwPid = pe32.th32ProcessID;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
			break;
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return hProcess;
}

BOOL CCommonFun::IsWinXp()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osv.dwMajorVersion == 5 
		&& osv.dwMinorVersion == 1)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CCommonFun::IsWinXpOrLater()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osv.dwMajorVersion >= 5 // >= 5
		&& osv.dwMinorVersion >= 1)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CCommonFun::IsWin2003()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwMajorVersion == 5 && osv.dwMinorVersion == 2 )
	{
		return TRUE;
	}
	return FALSE;
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

//win8		6.2	
//win8.1	6.3	
BOOL CCommonFun::IsWin8OrLater()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwMajorVersion >= 6 && osv.dwMinorVersion >= 2)
	{
		return TRUE;
	}
	return FALSE;
}

//win10 10.x
//win10 系统下获取系统版本号为6.2的问题，
//参考http://blog.csdn.net/k1988/article/details/47614529
BOOL CCommonFun::IsWin10()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if( osv.dwMajorVersion == 10)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CCommonFun::IsSameFile(CString sFile1, CString sFile2)
{
	BOOL bRet = FALSE;
	CFileStatus st1,st2;
	if(CFile::GetStatus(sFile1,st1) && CFile::GetStatus(sFile2,st2))
	{
		if(st1.m_mtime == st2.m_mtime)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CCommonFun::IsWin9X()
{
	OSVERSIONINFO osv={sizeof(OSVERSIONINFO)};
	// osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bSuc = GetVersionEx(&osv);
	if (!bSuc)
	{
	}
	if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CCommonFun::IsWindowsNT4()
{
	OSVERSIONINFO vi = { sizeof(vi) };

	::GetVersionEx(&vi);

	return ( (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
		(vi.dwMajorVersion == 4) );
}

CString CCommonFun::GetProcessFullPath(DWORD dwPID)
{
	if(!IsWin9X())
	{
		if(ModifyPrivilege(SE_DEBUG_NAME ,TRUE) != S_OK )
		{
			TRACE(_T("ModifyPrivilege Faile"));
		}
	}

	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	// DWORD dw = ::GetLastError();
	if(!hProcess)
	{
		return _T("");
	}
	HINSTANCE	hLib = LoadLibrary(_T("PSAPI.DLL"));
	if(!hLib)
	{
		if(hProcess)
		{
			::CloseHandle(hProcess);
			hProcess = NULL;
		}
		return _T("");
	}
	CString sRet = _T("");
	TCHAR szFilename[MAX_PATH + 1] = {0};
	HMODULE hModule[1024];
	DWORD	dwSize = 0;
	BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *, DWORD, LPDWORD );
	DWORD (WINAPI *lpfGetModuleFileNameEx)( HANDLE, HMODULE, LPTSTR, DWORD );			

	lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *, DWORD, LPDWORD)) GetProcAddress( hLib, "EnumProcessModules" ) ;
#ifdef UNICODE
	lpfGetModuleFileNameEx =(DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD )) GetProcAddress( hLib, "GetModuleFileNameExW") ;	    
#else
	lpfGetModuleFileNameEx =(DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD )) GetProcAddress( hLib, "GetModuleFileNameExA") ;	    
#endif // UNICODE

	if(lpfEnumProcessModules)
	{
		if(lpfEnumProcessModules(hProcess, hModule, sizeof(hModule), &dwSize))
		{
			if(lpfGetModuleFileNameEx)
			{
				if (lpfGetModuleFileNameEx(hProcess, hModule[0],szFilename, MAX_PATH))
				{
					sRet = szFilename;	
					//CCommonFun::WriteFileLog(_T("lpfGetModuleFileNameEx = %s,"),szFilename);
				}
			}
		}
		else
		{
			//CCommonFun::WriteFileLog(_T("LastError = %d,dwSize=%d"),::GetLastError(), dwSize);
		}
	}
	if(hProcess)
	{
		::CloseHandle(hProcess);
		hProcess = NULL;
	}
	if(hLib)
	{
		::FreeLibrary(hLib);
		hLib = NULL;
	}
	return sRet;
}

BOOL CCommonFun::GetResName(CString sPathName, CString &sResName)
{
	BOOL bRet = FALSE;
	HRSRC HRSRChRes;
	HINSTANCE hInst = ::LoadLibrary(sPathName);// AfxGetResourceHandle();
	if(!hInst)
		return FALSE;

	HRSRChRes = FindResourceA(hInst,(LPCSTR)MAKEINTRESOURCE(1),(LPSTR)RT_VERSION);	
	DWORD dwSize = SizeofResource (hInst , HRSRChRes);  //获得指定资源的大小
	HGLOBAL hMemoryHandle = (HRSRC)LoadResource(hInst, (HRSRC)HRSRChRes);//将资源载入内存
	WCHAR *pMemPtr = (WCHAR *)LockResource(hMemoryHandle); //定位资源位置

	TCHAR * pByte = new TCHAR[dwSize];
	ZeroMemory(pByte,dwSize*sizeof(TCHAR));
#ifndef UNICODE
	// WideCharToMultiByte(CP_ACP,0,pMemPtr,dwSize,pByte,dwSize,NULL,NULL);
	WideCharToMultiByte(CP_ACP,0,pMemPtr,dwSize/2,pByte,dwSize,NULL,NULL); // 第4个参数 dwSize/2 表示宽字符数，为双字节
	// 第6个参数 dwSize 表示buffer区的字节数
#else
	pByte = pMemPtr;
#endif // !UNICODE

	TCHAR* pTmp = pByte;
	// modify by ql 2008/2/26
	TCHAR *pSub = _T("OriginalFilename");
	BOOL bFinded = FALSE;
	while((pTmp - pByte) < (int)dwSize && !bFinded)
	{
		while(true)
		{
			if(*pTmp == '\0')
			{
				pTmp ++;				
				if(_tcsstr(pTmp,pSub))
				{
					bFinded = TRUE;
				}
				else
				{
					pTmp += _tcslen(pTmp);
				}
				break;
			}
			else
			{
				pTmp ++;
			}
		}
	}
	int nRet = pTmp - pByte;
	if(nRet>0)
	{
		sResName = pByte + nRet + _tcslen(pTmp) + 1;
		CString s;
		s.Format(_T("资源名为：%s"),sResName);
		WriteFileLog(s);
		//	AfxMessageBox(s);
		bRet = TRUE;
	}

	delete []pByte;
	pByte = NULL;
	::FreeLibrary(hInst);
	return bRet;
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

//
// 读取文件内容到**pBuf中
//
// 参数:	strFileName [in] 要读取的文件名
//			*pBuf		[in] 存放文件内容的缓冲区指针
//			dwFileLen	[out]要读取的文件大小
//
// 返回:	读取成功返回TRUE, 失败返回FALSE
//
// extern BOOL ReadFileToBuffer(CString strFileName,PBYTE *pBuf,/*[out]*/DWORD &dwFileLen);
//
BOOL CCommonFun::ReadFileToBuffer(CString strFileName, PBYTE *pBuf, DWORD &dwFileLen)
{
	BOOL bRet=TRUE;
	dwFileLen=0;
	CFile *pFile=NULL;
	try
	{
		pFile=new CFile(strFileName,CFile::modeRead);
		dwFileLen=pFile->GetLength();

		if(dwFileLen>0)
		{
			*pBuf=new BYTE[dwFileLen];
			ZeroMemory(*pBuf,dwFileLen);			
			pFile->SeekToBegin();
			pFile->Read(*pBuf,dwFileLen);
		}
		else
		{
			bRet=FALSE;
		}
		pFile->Close();
		delete pFile;
		pFile=NULL;		
	}
	catch(const CFileException& e)
	{
		if(pFile!=NULL)
		{
			pFile->Close();
			delete pFile;
			pFile=NULL;
		}
		if(*pBuf!=NULL)
		{
			delete[] *pBuf;
			*pBuf=NULL;
		}
		CString strFormatted;
		TCHAR   szCause[255];
		e.GetErrorMessage(szCause, 255);
		strFormatted = _T("[ReadFileToBuffer] 读取文件失败! 原因: ");
		strFormatted += szCause;

		TRACE(strFormatted);
		bRet=FALSE;
	}
	catch(...)
	{
		TRACE(_T("ReadFileToBuffer Error! FileName=%s	ErrCode=%d\n"),strFileName,GetLastError());
		bRet=FALSE;
	}

	return bRet;
}

BOOL CCommonFun::IsUsbDisk(TCHAR cDisk)
{
	HANDLE hDevice;               // handle to the drive to be examined 
	BOOL bResult;                 // results flag
	CString sRootPath;
	sRootPath.Format(_T("\\\\.\\%c:"),cDisk);
	DISK_GEOMETRY disk = {0};

	hDevice = CreateFile(sRootPath,			// drive to open
		0,									// don't need any access to the drive
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
		NULL,								 // default security attributes
		OPEN_EXISTING,						// disposition
		0,									// file attributes
		NULL);								// don't copy any file's attributes
	if (hDevice == INVALID_HANDLE_VALUE)	// we can't open the drive
	{
		return FALSE;
	}

	//---------------------
	STORAGE_PROPERTY_QUERY Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length

	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	STORAGE_DEVICE_DESCRIPTOR pDevDesc;

	pDevDesc.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

	// 用 IOCTL_STORAGE_QUERY_PROPERTY

	bResult = ::DeviceIoControl(
		hDevice,						// device handle
		IOCTL_STORAGE_QUERY_PROPERTY,	// info of device property
		&Query, 
		sizeof(STORAGE_PROPERTY_QUERY), // input data buffer
		&pDevDesc, 
		pDevDesc.Size,					// output data buffer
		&dwOutBytes,					// out's length
		(LPOVERLAPPED)NULL); 

	UINT Type = pDevDesc.BusType;

	//             Unknown                                                           0x00   
	//             SCSI                                                                 0x01   
	//             ATAPI                                                               0x02   
	//             ATA                                                                   0x03   
	//             IEEE1394                                                         0x04   
	//             SSA(Serial   storage   architecture)         0x05   
	//             Fibre   Channel,                                             0x06   
	//             USB,                                                                 0x07   
	//             RAID,                 0x08   


	//---------------------

	/*	bResult = DeviceIoControl(hDevice,  // device we are querying
	IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
	NULL, 
	0, // no input buffer, so pass zero
	&disk, 
	sizeof(disk),  // output buffer
	&junk, // discard count of bytes returned
	(LPOVERLAPPED) NULL);  // synchronous I/O
	*/	
	CloseHandle(hDevice);         // we're done with the handle

	if(bResult && (Type == 0x07))
	{
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

//add by zxl,2013.07.18,日志清理
//szLogDirPath:	日志所在目录
//nLeaveHours:	日志保留时间,单位为(小时),	
void CCommonFun::ClearLog(int nLeaveHours)
{
	CString szLogDirPath = GetLogDir();
	CTime TimeNow = CTime::GetCurrentTime(); 
	CFileFind	finder;				
	CString		sFullPath = _T("");					
	CString		strWildcard;

	szLogDirPath.TrimRight(_T("\\"));

	strWildcard = szLogDirPath + _T("\\*.log");
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
			CString sPath = finder.GetFilePath();
			CFileStatus st ;
			CFile::GetStatus(sPath,st);
			CTimeSpan timespan = TimeNow - st.m_mtime;

			if((timespan.GetTotalHours()>=nLeaveHours) && !DeleteFile(sPath) && (GetLastError() == 5))
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
}

// 内存分配要使用公用平台提供的方法
// 释放的时候也要使用公共平台的方法释放，保证内存在同一个crt库中
// 分配内存
BYTE* CCommonFun::Alloc(int nSize)
{
	BYTE* pData = NULL;
	try
	{
		ASSERT(nSize);
		if(nSize <= 0)
		{
			WriteFileLog(_T("CCommonFun::Alloc nSize <= 0"));
			return pData;
		}
		pData = new BYTE[nSize];
	}
	catch(...)
	{
		WriteFileLog(_T("CCommonFun::Alloc 异常"));
	}
	ASSERT(pData);
	if(!pData)
	{
		WriteFileLog(_T("CCommonFun::Alloc new 失败"));
		return pData;
	}
	ZeroMemory(pData,nSize);

	return pData;
}

// 释放内存
int CCommonFun::Free(BYTE* pData)
{
	if(pData)
	{
		delete []pData;
		pData = NULL;
	}
	return 0;
}

CString CCommonFun::GetLocalIP(void)
{
	CString sIP;
	if(InitSockDll())
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
		ReleaseSockDll();
	}
	return sIP;
}

int CCommonFun::InitSockDll()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		err = WSAStartup(wsaData.wVersion,&wsaData);
		return err != 0 ? FALSE : TRUE;
	}
	return TRUE;
}

void CCommonFun::ReleaseSockDll()
{
	WSACleanup();
}

CString CCommonFun::GetLoaclHost(void)
{
	CString sHost = _T("");
	if(InitSockDll())
	{
		char localHost[255];
		memset(localHost,0,255);
		gethostname(localHost, 255);
		sHost = localHost;
		ReleaseSockDll();
	}
	return sHost;
}

CString CCommonFun::GetLocalMac(void)
{
	DWORD	dwRet;
	IPAddr  ipAddr;
	ULONG  pulMac[ 2 ] = {0};
	ULONG  ulLen;
	CString sIP = GetLocalIP();
	CString	sMac;

#ifdef UNICODE
	CStringA sIPA = CCommonFun::UnicodeToMultiByte(sIP);
	ipAddr = inet_addr(sIPA.GetString());
#else
	ipAddr = inet_addr(sIP.GetString());
#endif // UNICODE
	memset( pulMac,0xff,sizeof( pulMac ) );
	ulLen = 6;

	dwRet = SendARP( ipAddr,0,pulMac,&ulLen );
	if( NO_ERROR != dwRet || ulLen == 0 ) 
		return _T("null");

	//	ULONG i;
	PBYTE pbHexMac = ( PBYTE )pulMac;

	if(ulLen < 6)
		return _T("null");
	// Convert the binary MAC address into human-readable
	sMac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5]);


	return sMac;
}

BOOL CCommonFun::IsLocalAdapter(char *pAdapterName)
{
	BOOL ret_value = FALSE;

#define NET_CARD_KEY ("System\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
	CStringA szData = "";
	char szDataBuf[MAX_PATH + 1];
	DWORD dwDataLen = MAX_PATH;
	DWORD dwType = REG_SZ;
	HKEY hNetKey = NULL;
	HKEY hLocalNet = NULL;

	if (ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, NET_CARD_KEY, 0, KEY_READ, &hNetKey))
		return FALSE;
	sprintf(szDataBuf, "%s\\Connection", pAdapterName);
	if (ERROR_SUCCESS != RegOpenKeyExA(hNetKey, szDataBuf, 0, KEY_READ, &hLocalNet))
	{
		RegCloseKey(hNetKey);
		return FALSE;
	}
	dwDataLen = MAX_PATH;
	if (ERROR_SUCCESS != RegQueryValueExA(hLocalNet, "PnpInstanceID", 0, &dwType, (BYTE *)szDataBuf, &dwDataLen))
	{
		goto ret;
	}
	szData = szDataBuf;
	szData.MakeUpper();
	//if (szData.Find("VMWARE") >= 0)	
	if (szData.Find("PCI") < 0)
		goto ret;

	ret_value = TRUE;

ret:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);

	return ret_value;
}

int CCommonFun::GetLocalMacList(CStringArray& ary)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		//GlobalFree (pAdapterInfo);
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}

	CStringA sMac;
	BOOL bFond = FALSE;
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			BYTE* pbHexMac = pAdapter->Address;
			sMac.Format("%02X-%02X-%02X-%02X-%02X-%02X", pbHexMac[0], pbHexMac[1], pbHexMac[2], pbHexMac[3], pbHexMac[4], pbHexMac[5]);
			if (IsLocalAdapter(pAdapter->AdapterName))
			{
	
				ary.Add(CCommonFun::MultiByteToUnicode(sMac));
				//printf("物理网卡：%s\t[%s]\t[%s]\n", sMac, pAdapter->AdapterName, pAdapter->Description);
			}
			else
			{
				//虚拟网卡不添加
				//printf("虚拟网卡：%s\t[%s]\t[%s]\n", sMac, pAdapter->AdapterName, pAdapter->Description);
			}

			pAdapter = pAdapter->Next;
		}
	}
	else
	{
		ASSERT(0);
	}
	free(pAdapterInfo);
	return ary.GetCount();
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

bool CCommonFun::RemoteStartComputer(CString sRemoteMac,CString sBCIP)		//远程开机
{
	if (sBCIP.IsEmpty())
	{
		sBCIP = _T("255.255.255.255");
	}
	BYTE m_MapMacAddr[6] = {0};
	TCHAR * tstr;
	CString tempmac;
	for(int ii=0;ii<6;ii++)
	{
		tempmac.Empty();
		for(int jj=0;jj<2;jj++)
		{
			tempmac+=sRemoteMac.GetAt(ii*2+jj);
		}
		m_MapMacAddr[ii]=(unsigned char)_tcstol((LPCTSTR)tempmac,&tstr,16);
	}

	LPBYTE pBuff=(LPBYTE)malloc(102);
	ASSERT(pBuff);
	ZeroMemory(pBuff,102);
	//定义传递的值前六位为六个（OXFF）后96位为16个顺序排列的目的网卡地址
	for(int k=0;k<6;k++)
	{
		pBuff[k]=0xff;
	}

	pBuff+=6;
	for(int i=0;i<16;i++)
	{
		CopyMemory(pBuff,m_MapMacAddr,6);
		pBuff+=6;
	}
	pBuff-=102;

	//广播102字节
	BOOL bvalue=TRUE;
	int err(0);
	CString selfadr;

	CStringArray ary;
	GetLocalIPList(ary);
	//if (ary.GetCount() > 1)
	{
#if 1
		
		for (int n = 0; n < ary.GetCount(); n ++)
		{
			CString sIP = ary.GetAt(n);
			SOCKET m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if(m_hSocket==INVALID_SOCKET)
			{
				return FALSE; 
			}
			err = setsockopt(m_hSocket,SOL_SOCKET,SO_BROADCAST,(char *)&bvalue,sizeof(int));
			if(err==SOCKET_ERROR)
			{
				return FALSE;
			}

			SOCKADDR_IN sockAddr;
			memset(&sockAddr,0,sizeof(SOCKADDR_IN));
			sockAddr.sin_family=AF_INET;
			sockAddr.sin_port=htons(REMOTE_START_PORT);
#ifndef UNICODE
			sockAddr.sin_addr.s_addr=inet_addr(sIP);
#else
			CStringA szIPA = UnicodeToUTF8(sIP);
			sockAddr.sin_addr.s_addr = inet_addr(szIPA);
	
#endif // !UNICODE

			err = bind(m_hSocket, (SOCKADDR*)&sockAddr,sizeof(SOCKADDR_IN));

			if(err==SOCKET_ERROR)
			{

				return FALSE;
			}
			
			//定义广播需要的参数的格式和内容
			SOCKADDR_IN addrSnd;	
			struct hostent * host=NULL;
			memset(&addrSnd,0,sizeof(SOCKADDR_IN));
			//参数付值
			addrSnd.sin_family=AF_INET;
			addrSnd.sin_port=htons(REMOTE_START_PORT);		//将系统付与的端口号付给结构
#ifndef UNICODE
			addrSnd.sin_addr.s_addr = inet_addr(sBCIP);;	//定义为“-1”全体局域网中的成员都可以收到
#else
			CStringA szBCIPA = UnicodeToUTF8(sBCIP);
			addrSnd.sin_addr.s_addr = inet_addr(szBCIPA);

#endif // !UNICODE

			err = sendto(m_hSocket, (char*)pBuff,102,0,(SOCKADDR*)&addrSnd,sizeof(addrSnd));
			/*
			传送通知网卡启动机器的信息，只能在对方使用软件的情况下启动
			因为只有软关机网卡上才是还有电的，可以接收到信号
			*/
			if(err!=102)
			{
				TRACE("\n错误代号是%d\n",WSAGetLastError());
				continue;
			}
			Sleep(10);
			closesocket(m_hSocket);

		}
#else
		for (int n = 0; n < ary.GetCount(); n ++)
		{
			CString sIP = ary.GetAt(n);
			CSocket socket;
			BOOL c = socket.Create(REMOTE_START_PORT,SOCK_DGRAM,sIP);	//创建CSocket	
			socket.GetSockName(selfadr,selfport);			//获得系统分配的端口号   
			int nres=socket.SetSockOpt(SO_BROADCAST,(char *)&bvalue,sizeof(int));	//赋予m_psocket可以广播的权限

			//定义广播需要的参数的格式和内容
			SOCKADDR_IN addr;	
			struct hostent * host=NULL;
			memset(&addr,0,sizeof(SOCKADDR_IN));
			//参数付值
			addr.sin_family=AF_INET;

			addr.sin_port=selfport;		//将系统付与的端口号付给结构
			addr.sin_addr.s_addr=inet_addr(sBCIP);;	//定义为“-1”全体局域网中的成员都可以收到

			/*
			传送通知网卡启动机器的信息，只能在对方使用软件的情况下启动
			因为只有软关机网卡上才是还有电的，可以接收到信号
			*/
			err=socket.SendTo(pBuff,102,(LPSOCKADDR)&addr,sizeof(addr),0);
			if(err!=102)
			{
				TRACE("\n错误代号是%d\n",WSAGetLastError());
				continue;
			}
			socket.Close(); 

		}
#endif
	}

	free(pBuff);
	return true;
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
// 将字符型ip转换成dword类型
DWORD CCommonFun::ConverIPToDWORD(CString sIP)
{
	DWORD dwIP = 0;
	CStringA szIPA = UnicodeToUTF8(sIP);
	dwIP = inet_addr(szIPA);
	BYTE b1,b2,b3,b4;
	b4 = HIBYTE(HIWORD(dwIP));
	b3 = LOBYTE(HIWORD(dwIP));
	b2 = HIBYTE(LOWORD(dwIP));
	b1 = LOBYTE(LOWORD(dwIP));

	return MAKELONG(MAKEWORD(b4,b3),MAKEWORD(b2,b1));
}

CString CCommonFun::ConverIPToSTR(UINT nIP)
{
	BYTE b0, b1, b2, b3;
	b0 = FIRST_IPADDRESS(nIP);
	b1 = SECOND_IPADDRESS(nIP);
	b2 = THIRD_IPADDRESS(nIP);
	b3 = FOURTH_IPADDRESS(nIP);

	CString str;
	str.Format(_T("%d.%d.%d.%d"), b0, b1, b2, b3);

	return str;
}
//判断路径是否存在
BOOL CCommonFun::IsValidatePath(CString sPath)
{
#if 1
	return PathIsDirectory(sPath);
#else
	//CFile::GetStatus不能处理根目录
	if(sPath.IsEmpty())	
		return FALSE;

	sPath.TrimRight(_T("\\"));
	CFileStatus fileStatus;
	if(CFile::GetStatus(sPath, fileStatus) && (CFile::directory & fileStatus.m_attribute))
	{//目录已经存在
		return TRUE;
	}
	return FALSE;
#endif
}
//判断路径是否存在
BOOL CCommonFun::ValidatePath(CString sPath)
{
	//如果存在，返回TRUE
	if(PathFileExists(sPath))
		return TRUE;

	sPath.Replace(_T("/"),_T("\\"));
	//创建该目录
	CStringArray sAry;
	CCommonFun::StringSplit(sPath,&sAry,_T('\\'));

	if( sAry.GetSize() < 2 )
	{
		return FALSE;
	}

	CString sDir;
	CString sTmp;
	for( int i = 0;i < sAry.GetSize() - 1; i++)
	{
		sDir += sAry.GetAt(i);
		sTmp = sDir;
		sDir += _T("\\");

		CFileFind fd;
		if( fd.FindFile(sTmp) )
		{
			fd.FindNextFile();
			if (fd.IsDirectory())
			{				
				continue;			
			}		
		}

		if( !CreateDirectory(sDir,NULL) )
		{
			//	CCommonFun::WriteFileLog(_T("创建目录")+sDir+_T("失败！"));
		}
		else
		{
			//	CCommonFun::WriteFileLog(_T("创建目录")+sDir+_T("成功！"));
		}
	}	
	return TRUE;
}

BOOL CCommonFun::ExcuteCmd(CString szCmdLine, BOOL bWait /*= FALSE*/, DWORD waitsecond/* = INFINITE*/)
{
	//CString szAppName = _T("C:\\Windows\\System32\\cmd.exe");
	STARTUPINFO si = { sizeof si, 0, _T("") };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	BOOL ret = FALSE;
	PROCESS_INFORMATION pi;
	TCHAR lpCmd[1024] = { 0 };
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
			WaitForSingleObject(pi.hProcess, waitsecond*1000);
		}
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		return TRUE;
	}
	return FALSE;
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


HRESULT CCommonFun::CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink, LPCTSTR lpszParam, LPCTSTR lpszDesc,WORD wHotkey)
{
	HRESULT hres; 
	IShellLink* psl; 

	try
	{
		CoInitialize(NULL);
		// Get a pointer to the IShellLink interface. 
		hres = CoCreateInstance(CLSID_ShellLink, NULL, 
			CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl); 
		if (SUCCEEDED(hres)) 
		{ 
			IPersistFile* ppf; 

			// Set the path to the shortcut target and add the 
			// description. 
			hres = psl->SetPath(lpszPathObj);
			hres = psl->SetDescription(lpszDesc); 
			hres = psl->SetArguments(lpszParam);
			if(wHotkey)
			{
				hres = psl->SetHotkey(wHotkey);
			}
			// Query IShellLink for the IPersistFile interface for saving the 
			// shortcut in persistent storage. 
			hres = psl->QueryInterface(IID_IPersistFile, 
				(LPVOID*)&ppf); 

			if (SUCCEEDED(hres)) 
			{ 
				
#ifdef _UNICODE
				hres = ppf->Save(lpszPathLink, TRUE); 
#else
				WCHAR wsz[MAX_PATH] = {0}; 
				// Ensure that the string is Unicode. 
				MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
					wsz, MAX_PATH); 

				// Save the link by calling IPersistFile::Save. 
				hres = ppf->Save(wsz, TRUE); 
				DWORD dw = ::GetLastError();
#endif	
				ppf->Release(); 
			} 
			psl->Release(); 
		} 
		::CoUninitialize();
	}
	catch(...)
	{
		DWORD dw = ::GetLastError();
		::CoUninitialize();
	}
	return hres; 
}

CString CCommonFun::ConverGuidToStr(GUID guid)
{
	LPOLESTR   pstrGuid;   
	int   i   =   StringFromCLSID(guid,   &pstrGuid);     
	CString   s(pstrGuid);    
	if   (NULL   !=   pstrGuid)           
		CoTaskMemFree   (pstrGuid);
	return s;
}
//产生Guid
CString CCommonFun::NewGuid()
{
	GUID   guid;   
	CoCreateGuid(&guid);   
	LPOLESTR   pstrGuid;   
	int   i   =   StringFromCLSID(guid,   &pstrGuid);     
	CString   s(pstrGuid);    
	if   (NULL   !=   pstrGuid)           
		CoTaskMemFree   (pstrGuid);
	return s;
}

BOOL CCommonFun::NewGuidByStr(CString sID,GUID &ID)
{
	ASSERT(!sID.IsEmpty());
	//	ASSERT(sID.GetLength() != 16);
#ifndef UNICODE
	int nLen = sID.GetLength() + 1;
	WCHAR* pBuf = new WCHAR[nLen];
	memset(pBuf,0,nLen * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, sID.GetBuffer(), -1, pBuf, nLen);
	CLSIDFromString((LPOLESTR)pBuf,&ID);
	sID.ReleaseBuffer();
#else
	CLSIDFromString((LPOLESTR)sID.GetString(), &ID);
#endif // !UNICODE
	return TRUE;
}

CString CCommonFun::GetDefalutIE()
{
#define READ_ONLY_KEYS	(KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY)
	HKEY hKey = NULL;
	DWORD dwType;
	TCHAR szPath[MAX_PATH + 1] = {0};
	DWORD dwSize = sizeof (szPath);
	//	DWORD dwDest;
	CString sPath = _T("HTTP\\shell\\open\\command");
	LONG ReturnValue = RegOpenKeyEx (HKEY_CLASSES_ROOT, sPath, 0L,
		READ_ONLY_KEYS , &hKey);

	do
	{
		if(ReturnValue != ERROR_SUCCESS)
			break;

		ReturnValue = RegQueryValueEx (hKey, _T(""), NULL,
			&dwType, (BYTE *)szPath, &dwSize);

		if(ReturnValue != ERROR_SUCCESS)
		{
			break;
		}
	}while(false);

	if(hKey)
	{
		::RegCloseKey(hKey);
	}

	CString sRet(szPath);
	if(sRet.IsEmpty())
	{
		TCHAR szDir[MAX_PATH + 1] = {0};
		::GetSystemDirectory(szDir,MAX_PATH + 1);

		sRet.Format(_T("%c:\\Program Files\\Internet Explorer\\IEXPLORE.EXE"),szDir[0]);
	}
	else
	{
		sRet.TrimLeft(_T("\""));
		int n = sRet.Find(_T("\""));
		if(n > 0 )
		{
			sRet = sRet.Left(n);
		}
	}
	return sRet;
}
BOOL CCommonFun::TerminalProcess(CString sProc)
{
	DWORD dwRet = 0;

	//查找
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		sProc.MakeUpper();
		PROCESSENTRY32 ProcessEntry;  
		ZeroMemory(&ProcessEntry,sizeof(PROCESSENTRY32));
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		BOOL bFlag = Process32First(hSnapshot,&ProcessEntry);
		while(bFlag)
		{
			CString strExePath
				= ProcessEntry.szExeFile;
			strExePath.MakeUpper();
			if(strExePath.Find(sProc) != -1)
			{
				dwRet = ProcessEntry.th32ProcessID;
				break;
			}
			ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
			bFlag = Process32Next(hSnapshot,&ProcessEntry);
		}
		CloseHandle(hSnapshot);
	}
	if (dwRet == 0)
		return 0;

	//终结
	//提升权限
	CCommonFun::ModifyPrivilege(SE_DEBUG_NAME,TRUE);
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwRet);
	if (hProcess == NULL)
		return 0;
	dwRet = TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	return dwRet ;
}

BOOL CCommonFun::TerminalOnePathProcess(CString strPath)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	HANDLE hPath = NULL;
	MODULEENTRY32 mod32 = { 0 };
	mod32.dwSize = sizeof(MODULEENTRY32);

	BOOL bResult = Process32First(hProcessSnap, &pe32);
	while (bResult)
	{
		CString strExe = pe32.szExeFile;
		//theLog.Write(_T("CCommonFun::TerminalOnePathProcess,strExe=%s"), strExe);
		if (strExe.CompareNoCase(L"explore.exe") != 0) {
			//theLog.Write(_T("CCommonFun::TerminalOnePathProcess,hPath,strExe=%s"), strExe);
			hPath = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
			if (hPath != NULL) {
				//theLog.Write(_T("CCommonFun::TerminalOnePathProcess,hPath=%x"), hPath);
				if (Module32First(hPath, &mod32)) {
					CString strExePath = mod32.szExePath;
					//theLog.Write(_T("CCommonFun::TerminalOnePathProcess,strExePath=%s"), strExePath);
					if (!strExePath.CompareNoCase(strPath)) {
						HANDLE hProc = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
						if (hProc != INVALID_HANDLE_VALUE) {
							::TerminateProcess(hProc, 0);
						}
					}
				}
				else {
				}
			}
			else {
			}
		}

		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);

	return TRUE;
}
BOOL CCommonFun::TerminaPID(DWORD dw)
{
	DWORD fResult  = FALSE;
	HANDLE hProc  = NULL;

	__try
	{
		ModifyPrivilege(SE_DEBUG_NAME,TRUE);
		// Open the process with PROCESS_QUERY_INFORMATION access
		hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dw);
		if (hProc == NULL)
		{
			__leave;
		}
		fResult = ::TerminateProcess(hProc,0);
		::CloseHandle(hProc);
	}
	__finally
	{
	}
	return fResult;
}

BOOL CCommonFun::SetProcessLevel(DWORD dw,int nLevel)
{
	try
	{
		HANDLE hPS = OpenProcess(PROCESS_ALL_ACCESS, false, dw); 
		if (!hPS)
			return FALSE;
		
		DWORD dwLevel = 0;
		switch (nLevel)
		{
		case -2:
			dwLevel = IDLE_PRIORITY_CLASS;
			break;

		case -1:
			dwLevel = BELOW_NORMAL_PRIORITY_CLASS;
			break;

		case 0:
			dwLevel = NORMAL_PRIORITY_CLASS;
			break;

		case 1:
			dwLevel = ABOVE_NORMAL_PRIORITY_CLASS;
			break;

		case 2:
			dwLevel = HIGH_PRIORITY_CLASS;
			break;

		case 3:
			dwLevel = REALTIME_PRIORITY_CLASS;
			break;
		}
		
		BOOL bRet = SetPriorityClass(hPS,dwLevel); 
		CloseHandle(hPS); 
		return bRet;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

CString CCommonFun::GetSubnetMask(CString sIP)
{
	CString sTmp;
	CString sSubnet;
	TCHAR   cBuffer[10000] = {0};   
	::ZeroMemory(&cBuffer,sizeof(cBuffer));   
	PMIB_IPADDRTABLE   pTable   =   (PMIB_IPADDRTABLE)cBuffer;   
	DWORD   dwSize   =   sizeof(cBuffer);   

	if(GetIpAddrTable(pTable,&dwSize,FALSE) == NO_ERROR)   
	{   

		DWORD  dwCounter   =   pTable->dwNumEntries;   
		for(DWORD   i=0;i<   dwCounter;i++)   
		{                           
			DWORD   addr   =   pTable->table[i].dwAddr;   
			DWORD   subnet   =   pTable->table[i].dwMask;   
			DWORD   broadcast   =   pTable->table[i].dwBCastAddr;   
			//   Filter   127.0.0.1   
			//sSubnet.Format("%d.%d.%d.%d",subnet & 0xFF,(subnet >> 8) & 0xFF,(subnet >> 16) & 0xFF,(subnet >> 24) & 0xFF);
			sTmp.Format(_T("%d.%d.%d.%d"),addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,(addr >> 24) & 0xFF);
			//TRACE("net =%s ,ip = %s--\n",sSubnet,sIp);
			if   (addr   !=   0x0100007f && sIP.CompareNoCase(sTmp) == 0)   
			{
				sSubnet.Format(_T("%d.%d.%d.%d"),subnet & 0xFF,(subnet >> 8) & 0xFF,(subnet >> 16) & 0xFF,(subnet >> 24) & 0xFF);
				break;
			}
		}   
	}   
	return sSubnet;
}

HBITMAP CCommonFun::LoadBmp(CString sPath,UINT nType)
{
	ASSERT(!sPath.IsEmpty());
	HBITMAP h = (HBITMAP)::LoadImage(NULL,sPath,nType,0,0,
		/*LR_CREATEDIBSECTION*/LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	return h;
}

BOOL CCommonFun::ConvertCTime2FILETIME(const CTime &cTime, FILETIME &fileTime)
{
	SYSTEMTIME timeDest;
	cTime.GetAsSystemTime(timeDest);
	::SystemTimeToFileTime(&timeDest, &fileTime);
	return TRUE;
}

void CCommonFun::CStringFormatUint64(UINT64 un64, CString &szValue)	//把UINT64格式化到CString中
{
	if(MAXDWORD >= un64)
	{//高四字为空
		UINT unTmp = (UINT)un64;
		szValue.Format(_T("%u"), unTmp);	//变长参数列表中不能出现64位的参数
		return;
	}

	TCHAR* pByte = (TCHAR*)&un64;
	TCHAR byBuf[80];
	memset(byBuf, 0x0, sizeof(byBuf));
	_ui64tot(un64, byBuf, 10);

	szValue.Format(_T("%s"), byBuf);
	return;
}

BOOL CCommonFun::IfDirValidThenCreateIt(CString szDir)
{
	if(szDir.IsEmpty())
		return FALSE;

	//把诸如:"C:\tmp1\tmp2\"变为"C:\tmp1\tmp2"
	int nPathLen = szDir.GetLength();
	while('\\' == szDir.GetAt(nPathLen - 1))	//注意CString中第一个字符的index是0
	{//若最后一个字符是'\\',则去掉它
		szDir = szDir.Left(nPathLen - 1);
		nPathLen--;
	}

	CFileStatus fileStatus;
	if(TRUE == CFile::GetStatus(szDir, fileStatus)
		&& (CFile::directory & fileStatus.m_attribute))
	{//目录已经存在
		return TRUE;
	}

	//逐级创建这个目录
	CStringArray dirAry;
	CString szTmp = _T("");
	int i = 0;
	int nDirLen = szDir.GetLength();
	for(i = 0; i < nDirLen; i++)
	{
		if('\\' != szDir.GetAt(i))
			szTmp += szDir.GetAt(i);
		else
		{
			dirAry.Add(szTmp);
			szTmp += '\\';
		}
		if(i + 1 == nDirLen)
			dirAry.Add(szTmp);
	}//for(i)

	int nNum = dirAry.GetSize();
	for(i = 0; i < nNum; i++)
	{
		szTmp = dirAry.GetAt(i);
		CreateDirectory(szTmp, NULL);
	}//for(i)

	//创建之后再次判断
	if(TRUE == CFile::GetStatus(szDir, fileStatus)
		&& (CFile::directory & fileStatus.m_attribute))
	{//目录已经存在
		return TRUE;
	}
	else
	{//不是合法目录,或是一个文件
		return FALSE;
	}
}

//得到文件夹下所有文件的总大小
unsigned __int64 CCommonFun::GetFolderTotalSize(CString sFolderPath)
{
	WIN32_FIND_DATA FindFileData;
	CString stemp = sFolderPath;
	if(stemp.ReverseFind('\\') != stemp.GetLength() - 1)
	{
		stemp += _T("\\*.*");
	}
	else
	{
		stemp += _T("*.*");
	}
	HANDLE hFile;
	unsigned __int64 iFolderSize = 0;
	DWORD dwFlag = 0;
	CString str;
	CString sSubFolder;
	UINT64 un64Tmp1 = MAXDWORD;
	un64Tmp1++;
	UINT64 un64High = 0;
	UINT64 un64Low = 0;
	if(hFile=FindFirstFile(stemp,&FindFileData))
	{
		if (hFile== INVALID_HANDLE_VALUE)
		{
			//没找到则直接返回
			return iFolderSize;
		}
		do
		{
			//文件属性
			dwFlag=FindFileData.dwFileAttributes;
			//定义时用的是2的阶乘的结果来表示一种属性，所以可以这样判断，不会出错
			if( dwFlag & FILE_ATTRIBUTE_DIRECTORY)//fold——文件夹
			{
				str.Format(_T("%s"),FindFileData.cFileName);

				if(str==_T(".")||str==_T(".."))
				{
					;		//不处理
				}
				else
				{
					sSubFolder = sFolderPath;
					if(sSubFolder.ReverseFind(_T('\\')) != sSubFolder.GetLength() - 1)
					{
						sSubFolder += _T('\\');
					}
					sSubFolder += FindFileData.cFileName;
					iFolderSize += GetFolderTotalSize(sSubFolder);//递归调用自身
				}
			}
			else	//文件
			{
				/*//错误做法,四则运算中用的是32位，若遇到超过4G文件,溢出导致错误
				iFolderSize += (unsigned __int64 )(FindFileData.nFileSizeHigh * (MAXDWORD + 1) + FindFileData.nFileSizeLow);
				*/
				//支持超过4G的文件
				un64High = FindFileData.nFileSizeHigh;
				un64High *= un64Tmp1;
				un64Low = FindFileData.nFileSizeLow;
				iFolderSize += (un64High + un64Low);
			}
		}while(FindNextFile(hFile,&FindFileData));
	}

	if(hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = NULL;
	}
	return iFolderSize;
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

CString CCommonFun::GetIPByHostname(CString szHostname)
{
	CString sIP;
	if (InitSockDll())
	{
		struct hostent *host = gethostbyname(CCommonFun::UnicodeToUTF8(szHostname));
		if (host)
		{
			sIP = inet_ntoa(*(struct in_addr*)*host->h_addr_list);
		}
		ReleaseSockDll();
	}
	return sIP;
}

CString CCommonFun::GetLocalMac(CString sIP)
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
	CString sMac;
	BOOL bFond = FALSE;
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{

			BYTE* pbHexMac = pAdapter->Address;
			sMac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5]);

			PIP_ADDR_STRING pAddr = &(pAdapter->IpAddressList);
			int nCurIPIndex = 0;
			while (pAddr)
			{
#ifndef UNICODE
				CString szIp = pAddr->IpAddress.String;
#else
				CString szIp = CCommonFun::MultiByteToUnicode(pAddr->IpAddress.String);
#endif // !UNICODE


				if(sIP.CompareNoCase(szIp) == 0 )
				{
					bFond = TRUE;
					break;
				}
				pAddr = pAddr->Next;
			}

			if (bFond)
			{
				break;
			}
			pAdapter = pAdapter->Next;

		}
	}
	else 
	{
		ASSERT(0);
	}
	free(pAdapterInfo);
	return sMac;
}

CString CCommonFun::GetUninstallString(CString sDisplayName)
{
	HKEY   hKey = NULL;   
	CString sKeyPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");  
	DWORD   dwAttr = REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;  
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

				TCHAR szUninstall[MAX_PATH] = {0};
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
			//sPath += " /s";
		}
		theLog.Write(_T("卸载数据库组件 path = %s,param = %s"),sPath,sParam);
		bRet = CSecurityTool::CreateProcByCurUsr(sPath.GetBuffer(),sParam.GetBuffer(),SW_SHOWNORMAL);
	} while (0);
	return bRet;
}

#pragma message(__LOC__"返回的字节流不同于保存的文件，但可显示。")
// 存储结构：
// DWORD dwOffBits	从pByte到bits的偏移
// BITMAPINFOHEADER
// PALLETTE
// Bits
BOOL CCommonFun::GetBitmapBytes(HBITMAP hBitmap, BYTE*& pByte, int& nByteLen)
{
	if(!hBitmap)
		return FALSE;

	BITMAP bm;
	CBitmap bmp;
	bmp.Attach(hBitmap);
	bmp.GetBitmap(&bm);

	pByte = NULL;
	nByteLen = 0;
	HDC hDC = ::GetDC(NULL);

	//调色板
	int nBitCount = bm.bmBitsPixel;
	RGBQUAD* pQuad = NULL;
	DWORD dwPaletteSize = 0;
	if(nBitCount <= 8)
	{
		int nColors = 1 << nBitCount;
		dwPaletteSize = nColors * sizeof(RGBQUAD);
		pQuad = new RGBQUAD[nColors];
		if(!pQuad)
		{
			bmp.Detach();
			return FALSE;
		}
		ZeroMemory(pQuad, dwPaletteSize);

		PALETTEENTRY pe[256] = {0};
		GetSystemPaletteEntries(hDC, 0, 256, pe);
		for (int i = 0; i < nColors; i++)
		{
			pQuad[i].rgbRed = pe[i].peRed;
			pQuad[i].rgbGreen = pe[i].peGreen;
			pQuad[i].rgbBlue = pe[i].peBlue;
			pQuad[i].rgbReserved = 0;
		}
	}
	else if(nBitCount != 24)
	{
		dwPaletteSize = 3*sizeof(DWORD);
		pQuad = new RGBQUAD[3];
		if(!pQuad)
		{
			bmp.Detach();
			return FALSE;
		}
		ZeroMemory(pQuad, dwPaletteSize);

		DWORD dRcolor = 0,dGcolor = 0,dBcolor = 0;
		if(nBitCount == 32)
		{
			dRcolor=16711680;
			dGcolor=65280;
			dBcolor=255;
		}
		else
		{
			dRcolor=63488;//0x7C00;//使用镜像取图要强制赋值有555和565之分
			dGcolor=2016;//0x03E0;//;
			dBcolor=31;	//		
		}
		*(DWORD*)&pQuad[0]=dRcolor;
		*(DWORD*)&pQuad[1]=dGcolor;
		*(DWORD*)&pQuad[2]=dBcolor;
	}

	//设置位图信息头结构
	BITMAPINFOHEADER bih={0};
	bih.biSize            = sizeof(BITMAPINFOHEADER);
	bih.biWidth           = bm.bmWidth;
	bih.biHeight          = bm.bmHeight;
	bih.biPlanes          = 1;
	bih.biBitCount         = nBitCount;
	bih.biCompression      = BI_RGB;
	bih.biSizeImage        = 0;
	bih.biXPelsPerMeter     = 0;
	bih.biYPelsPerMeter     = 0;
	bih.biClrUsed         = 0;
	bih.biClrImportant      = 0;

	DWORD dwBmBitsSize = WIDTHBYTES(bm.bmWidth*nBitCount) * bm.bmHeight;
	DWORD dwOffBits = sizeof(DWORD)+sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	nByteLen = dwOffBits + dwBmBitsSize;
	pByte = new BYTE[nByteLen];
	if(!pByte)
	{
		bmp.Detach();
		::ReleaseDC(NULL, hDC);
		return FALSE;
	}
	ZeroMemory(pByte, nByteLen);

	*(DWORD*)pByte = dwOffBits;
	CopyMemory(pByte+sizeof(DWORD), &bih, sizeof(BITMAPINFOHEADER));
	if(dwPaletteSize > 0)
	{
		CopyMemory(pByte+sizeof(DWORD)+sizeof(BITMAPINFOHEADER), pQuad, dwPaletteSize);
		delete[] pQuad;
		pQuad = NULL;
	}

	// 处理调色板
	HPALETTE hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	HPALETTE hOldPal = NULL;
	if(hPal)
	{
		hOldPal = SelectPalette(hDC, hPal, FALSE);
		RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值
	int nLine = GetDIBits(hDC, 
		hBitmap, 
		0, 
		(UINT)bm.bmHeight,
		pByte + dwOffBits,
		(BITMAPINFO *)(pByte+sizeof(DWORD)), 
		DIB_RGB_COLORS);

	//恢复调色板
	if(hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
	}

	//清理
	bmp.Detach();	// 不在此函数中销毁hBitmap
	::ReleaseDC(NULL, hDC);

	return TRUE;
}
#ifndef SM_OEPN_FAILED
#define SM_OEPN_FAILED							0x00000000
#endif
/*
#define SERVICE_STOPPED                        0x00000001
#define SERVICE_START_PENDING                  0x00000002
#define SERVICE_STOP_PENDING                   0x00000003
#define SERVICE_RUNNING                        0x00000004
#define SERVICE_CONTINUE_PENDING               0x00000005
#define SERVICE_PAUSE_PENDING                  0x00000006
#define SERVICE_PAUSED                         0x00000007
*/
#ifndef SERVICE_ST_UNINSTALLED
#define SERVICE_ST_UNINSTALLED						0x00000008
#endif
DWORD CCommonFun::GetSvcStatus(CString strSvcName)
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm == NULL)
	{
		return SM_OEPN_FAILED;
	}
	SC_HANDLE svc = OpenService(scm,strSvcName, SERVICE_QUERY_STATUS);
	if (svc != NULL)
	{
		QueryServiceStatus(svc,&ssa);
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);	//add by zfq,2013.06.08
		return ssa.dwCurrentState;
	}
	else
	{
		CloseServiceHandle(scm);
		return SERVICE_ST_UNINSTALLED;
	}
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
		CloseServiceHandle(SchSCManager);	//add by zfq,2013.06.08
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

DWORD CCommonFun::PauseSvc(CString sname)
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm!=NULL)
	{
		SC_HANDLE svc = OpenService(scm,sname,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc,&ssa);
			if (ssa.dwCurrentState == SERVICE_RUNNING)
			{
				int nret=ControlService(svc, SERVICE_CONTROL_PAUSE, &ssa);
				if(!nret)
				{
					int nerr=GetLastError();
					CloseServiceHandle(svc);	//add by zfq,2013.06.08
					CloseServiceHandle(scm);	//add by zfq,2013.06.08
					return nerr;
				}
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return 0;
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

DWORD CCommonFun::RecoverSvc(CString strSvcName)
{
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm!=NULL)
	{
		SC_HANDLE svc = OpenService(scm,strSvcName,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc,&ssa);
			if (ssa.dwCurrentState == SERVICE_PAUSED)
			{
				int nret=ControlService(svc, SERVICE_CONTROL_CONTINUE, &ssa);
				if(!nret)
				{
					int nerr=GetLastError();
					CloseServiceHandle(svc);	//add by zfq,2013.06.08
					CloseServiceHandle(scm);	//add by zfq,2013.06.08
					return nerr;
				}
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return 0;
}
DWORD CCommonFun::SetStartSvcType(CString strSvcName,DWORD dwstarttype)
{
	SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if (scm!=NULL)
	{
		SC_HANDLE svc = OpenService(scm,strSvcName,SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			BOOL bchange = ChangeServiceConfig(svc,
				SERVICE_NO_CHANGE,
				dwstarttype,
				SERVICE_NO_CHANGE,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL);
			if (!bchange)
			{
				int nerr=GetLastError();
				CloseServiceHandle(svc);	//add by zfq,2013.06.08
				CloseServiceHandle(scm);	//add by zfq,2013.06.08
				return nerr;
			}
			CloseServiceHandle(svc);
		}                           
		CloseServiceHandle(scm);
	}
	return 0;
}

BOOL CCommonFun::StartSMService(CString sServiceName)
{
	CCommonFun::WriteFileLog(_T("StartSMService,1,sServiceName=[%s]"), sServiceName);
	BOOL bRet = FALSE;
	SC_HANDLE SchSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (SchSCManager != NULL)
	{
		SC_HANDLE  schService;
		schService = OpenService(SchSCManager, sServiceName, SERVICE_ALL_ACCESS);
		if (schService != NULL)
		{
			bRet = StartService(schService, 0, NULL);
			if (bRet)
			{
				bRet = WaitForState(SERVICE_RUNNING, schService);
				if (!bRet)
				{
					CCommonFun::WriteFileLog(_T("!!StartSMService,6,WaitForState fail,err=[%u]"), GetLastError());
				}
			}
			else
			{
				DWORD dwErr = GetLastError();
				if (dwErr == ERROR_SERVICE_ALREADY_RUNNING)
				{
					bRet = TRUE;
					CCommonFun::WriteFileLog(_T("StartSMService,5,StartService fail,err=[ERROR_SERVICE_ALREADY_RUNNING]"));
				}
				else
				{
					CCommonFun::WriteFileLog(_T("!!StartSMService,4,StartService fail,err=[%u]"), dwErr);
				}
			}
			CloseServiceHandle(schService);
		}
		else
		{
			CCommonFun::WriteFileLog(_T("!!StartSMService,3,OpenService fail,err=[%u]"), GetLastError());
		}
		CloseServiceHandle(SchSCManager);
	}
	else
	{
		CCommonFun::WriteFileLog(_T("!!StartSMService,2,OpenSCManager fail,err=[%u]"), GetLastError());
	}

	return bRet;
}

BOOL CCommonFun::SetServiceDelayedAutoStart(CString sServiceName)
{
	CCommonFun::WriteFileLog(_T("SetServiceDelayedAutoStart,1,sServiceName=[%s]"), sServiceName);

	BOOL bchange = FALSE;
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (scm != NULL)
	{
		SC_HANDLE svc = OpenService(scm, sServiceName, SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			//Client Requires Windows Vista.
			//Server Requires Windows Server 2008.
			SERVICE_DELAYED_AUTO_START_INFO sdBuf = { 0 };
			sdBuf.fDelayedAutostart = TRUE;

			bchange = ChangeServiceConfig2(
				svc,
				SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
				&sdBuf);
			if (!bchange)
			{
				 CCommonFun::WriteFileLog(_T("!!SetServiceDelayedAutoStart,2,ChangeServiceConfig2 fail,err=[%u]"), GetLastError());
			}
			CloseServiceHandle(svc);
		}
		CloseServiceHandle(scm);
	}
	return bchange;
}

BOOL CCommonFun::StopSMService(CString sServiceName)
{
	CCommonFun::WriteFileLog(_T("StopSMService,1,sServiceName=[%s]"), sServiceName);
	BOOL bRet = FALSE;
	SERVICE_STATUS ssa;
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (scm != NULL)
	{
		SC_HANDLE svc = OpenService(scm, sServiceName, SERVICE_ALL_ACCESS);
		if (svc != NULL)
		{
			QueryServiceStatus(svc, &ssa);
			if (ssa.dwCurrentState == SERVICE_RUNNING)
			{
				bRet = ControlService(svc, SERVICE_CONTROL_STOP, &ssa);
				if (bRet)
				{
					bRet = WaitForState(SERVICE_STOPPED, svc);
					if (!bRet)
					{
						CCommonFun::WriteFileLog(_T("!!StopSMService,6,WaitForState fail,err=[%u]"), GetLastError());
					}
				}
				else
				{
					CCommonFun::WriteFileLog(_T("!!StopSMService,5,ControlService fail,err=[%u]"), GetLastError());
				}
			}
			else
			{
				CCommonFun::WriteFileLog(_T("!!StopSMService,4,QueryServiceStatus fail,err=[%u]"), GetLastError());
			}
			CloseServiceHandle(svc);
		}
		else
		{
			CCommonFun::WriteFileLog(_T("!!StopSMService,3,OpenService fail,err=[%u]"), GetLastError());
		}
		CloseServiceHandle(scm);
	}
	else
	{
		CCommonFun::WriteFileLog(_T("!!StopSMService,2,OpenSCManager fail,err=[%u]"), GetLastError());
	}
	return bRet;
}

BOOL CCommonFun::SetFailedAction(CString strSvcName)
{
	BOOL bSuccess = FALSE;
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

void CCommonFun::EnumFile(CString &cDirectory, CStringArray &cFileNameAry)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	// 开始查找
	cDirectory.Append(_T("\\*.*"));
	hFind = FindFirstFile(cDirectory, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString Namefile;
			Namefile.Format(_T("%s"),FindFileData.cFileName);
			if (!Namefile.Compare(_T(".")) || !Namefile.Compare(_T("..")) || !Namefile.Compare(_T("desktop.ini")))
			{}
			else
				cFileNameAry.Add(Namefile);
		}
		while (FindNextFile(hFind, &FindFileData) != 0);
	}
	else
		return;

	// 查找结束
	FindClose(hFind);
}

BOOL CCommonFun::IsDirEmpty(CString sDir)
{
	BOOL bRet = TRUE;
	CFileFind	finder;				
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
			bRet = IsDirEmpty(finder.GetFilePath());
			if (!bRet)
				break;
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}
	finder.Close();	
	return bRet;
}

#define WM_QUERYCENTERWND   0x036B  // lParam = HWND to use as centering parent
void CCommonFun::CenterWindow(CWnd* pChildWnd, CWnd* pAlternateOwner, CenterType ct /* = CENTER_BOTH */)
{
	ASSERT(pChildWnd);
	ASSERT(::IsWindow(pChildWnd->m_hWnd));

	// determine owner window to center against
	DWORD dwStyle = pChildWnd->GetStyle();
	HWND hWndCenter = pAlternateOwner->GetSafeHwnd();
	if (pAlternateOwner == NULL)
	{
		if (dwStyle & WS_CHILD)
			hWndCenter = ::GetParent(pChildWnd->m_hWnd);
		else
			hWndCenter = ::GetWindow(pChildWnd->m_hWnd, GW_OWNER);
		if (hWndCenter != NULL)
		{
			// let parent determine alternate center window
			HWND hWndTemp =
				(HWND)::SendMessage(hWndCenter, WM_QUERYCENTERWND, 0, 0);
			if (hWndTemp != NULL)
				hWndCenter = hWndTemp;
		}
	}

	// get coordinates of the window relative to its parent
	CRect rcDlg;
	POINT pt;
	pChildWnd->GetWindowRect(&rcDlg);
	CRect rcArea;
	CRect rcCenter;
	HWND hWndParent;
	if (!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if (hWndCenter != NULL)
		{
			DWORD dwAlternateStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if (!(dwAlternateStyle & WS_VISIBLE) || (dwAlternateStyle & WS_MINIMIZE))
				hWndCenter = NULL;
		}

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		// center within appropriate monitor coordinates
		if (hWndCenter == NULL)
		{
			HWND hwDefault = AfxGetMainWnd()->GetSafeHwnd();

			GetMonitorInfo(
				MonitorFromWindow(hwDefault, MONITOR_DEFAULTTOPRIMARY), &mi);
			rcCenter = mi.rcWork;
			rcArea = mi.rcWork;
		}
		else
		{
			::GetWindowRect(hWndCenter, &rcCenter);
			GetMonitorInfo(
				MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;
		}
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(pChildWnd->m_hWnd);
		ASSERT(::IsWindow(hWndParent));

		::GetClientRect(hWndParent, &rcArea);
		ASSERT(::IsWindow(hWndCenter));
		::GetClientRect(hWndCenter, &rcCenter);
		pt.x = rcDlg.left;
		pt.y = rcDlg.top;
		::ScreenToClient(hWndCenter, &pt);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	// find dialog's upper left based on rcCenter
	int xLeft = pt.x;
	if (ct & CENTER_X )
		xLeft = (rcCenter.left + rcCenter.right) / 2 - rcDlg.Width() / 2;
	int yTop = pt.y;
	if (ct & CENTER_Y)
		yTop = (rcCenter.top + rcCenter.bottom) / 2 - rcDlg.Height() / 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;
	else if (xLeft + rcDlg.Width() > rcArea.right)
		xLeft = rcArea.right - rcDlg.Width();

	if (yTop < rcArea.top)
		yTop = rcArea.top;
	else if (yTop + rcDlg.Height() > rcArea.bottom)
		yTop = rcArea.bottom - rcDlg.Height();

	// map screen coordinates to child coordinates
	pChildWnd->SetWindowPos(NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CCommonFun::HasInvalidChar(CString str)
{
	CString sInv = _T("\\/:*?\"<>|");
	return str.FindOneOf(sInv)!=-1;
}

// 添加窗口属性，可以防止CoolSkin Hook此窗口
BOOL CCommonFun::UnSkinTheWnd(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	return SetPropA(hWnd, "CoolSkinHwndPtr", (HANDLE)0xABCD);
}

//获取用户默认目录
CString CCommonFun::GetUserDefaultPath()
{
	CString sPath;
	//CCommonFun::SHGetSpecialPath(CSIDL_PERSONAL,sPath);
	CCommonFun::SHGetSpecialPath(CSIDL_COMMON_APPDATA, sPath);

	sPath += PROJECT_PATH;
	CCommonFun::ValidatePath(sPath);
	return sPath;
}
void DeleteDirectory(const CString &strPath)
{
	CFileFind tempFind;
	TCHAR sTempFileFind[MAX_PATH] = { 0 };

	wsprintf(sTempFileFind, _T("%s\\*.*"), strPath);
	BOOL IsFinded = tempFind.FindFile(sTempFileFind);

	while (IsFinded) 
	{ 
		IsFinded = tempFind.FindNextFile(); 

		if (!tempFind.IsDots()) 
		{ 
			TCHAR sFoundFileName[200] = { 0 }; 
			_tcscpy(sFoundFileName, tempFind.GetFileName().GetBuffer(200)); 

			if (tempFind.IsDirectory()) 
			{ 
				TCHAR sTempDir[200] = { 0 }; 
				wsprintf(sTempDir, _T("%s\\%s"),strPath, sFoundFileName); 
				// 				DeleteDirectory(sTempDir); 
				CCommonFun::DeleteDir(sTempDir);
			} 
			else 
			{ 
				TCHAR sTempFileName[200] = { 0 }; 
				wsprintf(sTempFileName, _T("%s\\%s"), strPath, sFoundFileName); 
				DeleteFile(sTempFileName); 
			} 
		} 
	} 
	tempFind.Close(); 
}

std::string CCommonFun::CStringToString(CString szStr)
{
#ifdef _UNICODE
	//如果是unicode工程
	USES_CONVERSION;
	std::string str(W2A(szStr));
	return str;
#else
	//如果是多字节工程 
	std::string str(cs.GetBuffer());
	cs.ReleaseBuffer();
	return str;
#endif
}


//动态获取录制文件
CString CCommonFun::GetUserFilePathName(CString sFileName,CString sSub /*= ""*/,CString sRoot /*= ""*/,BOOL bAutoWithTime /*= TRUE*/)
{
	ASSERT(!sFileName.IsEmpty());
	CString sPath;
	if (sRoot.IsEmpty() || !IsValidatePath(sRoot))
	{
		//CCommonFun::SHGetSpecialPath(CSIDL_PERSONAL,sPath);
		CCommonFun::SHGetSpecialPath(CSIDL_COMMON_APPDATA, sPath);
		sPath += PROJECT_PATH;
	}
	else
	{
		sPath = sRoot;
		sPath.TrimRight(_T("\\"));
		sPath += _T("\\");
	}
	if (!sSub.IsEmpty())
	{
		sPath += sSub;
		sPath += _T("\\");
	}
	if (bAutoWithTime)
	{
		int nIndex = sFileName.ReverseFind(_T('.'));
		CString sLeft = sFileName.Left(nIndex);
		CString sDot = sFileName.Right(sFileName.GetLength() - nIndex - 1);
		SYSTEMTIME st;
		::GetLocalTime(&st);
		sFileName.Format(_T("%s(%d-%d-%d %d-%d-%d).%s"),sLeft,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,sDot);
		sPath += sFileName;
	}
	CCommonFun::ValidatePath(sPath);

	return sPath;
}

//获取指定文件的缩略图
HBITMAP CCommonFun::GetThumbnailImage(CString sFilePathName, int nCX,int nCY, int colorDepth/* = 24*/)
{
	// divide the file name into a folder path and file name.
	WCHAR wDirName[MAX_PATH + 1] = {0};
	WCHAR wFileName[MAX_PATH + 1] = {0};
	TCHAR tName[MAX_PATH + 1] = {0};

	CString dir = sFilePathName.Left(sFilePathName.ReverseFind(_T('\\')));
	CString file = sFilePathName.Right(sFilePathName.GetLength() - dir.GetLength() - 1);

	//根目录要以\\结尾
	if (dir.Right(1) == _T(":"))
		dir.Append(_T("\\"));

	_tcscpy(tName, dir);
#ifdef _UNICODE
	wcscpy(wDirName, tName);
#else
	wcscpy(wDirName, CT2W(tName));
#endif
	_tcscpy(tName, file);
#ifdef _UNICODE
	wcscpy(wDirName, tName);
#else
	wcscpy(wFileName, CT2W(tName));
#endif

	IShellFolder* pDesktop = NULL;
	IShellFolder* pSub = NULL;
	IExtractImage* pIExtract = NULL;
	LPITEMIDLIST pList = NULL;
	HBITMAP hBmp = NULL;

	HRESULT hr = S_OK;
	do 
	{
		//获取根目录
		hr = SHGetDesktopFolder(&pDesktop);
		if (FAILED(hr))
		{
			TRACE(_T("Failed to SHGetDesktopFolder"));
			break;
		}
		//获取父目录节点
		hr = pDesktop->ParseDisplayName(NULL, NULL, wDirName, NULL, &pList, NULL);
		if (FAILED(hr))
		{ 
			TRACE(_T("Failed to ParseDisplayName"));
			break;
		}
		//获取目录列表
		hr = pDesktop->BindToObject(pList, NULL, IID_IShellFolder, (void**)&pSub);
		if (FAILED(hr))
		{
			TRACE(_T("Failed to bind to the directory"));
			break;
		}
		hr = pSub->ParseDisplayName(NULL, NULL, wFileName, NULL, &pList, NULL);
		if (FAILED(hr))
		{
			TRACE(_T("Failed to parse the file name"));
			break;
		}
		// get the IExtractImage interface
		LPCITEMIDLIST pidl = pList;
		hr = pSub->GetUIObjectOf(NULL, 1, &pidl, IID_IExtractImage, NULL, (void**)&pIExtract);

		// set our desired image size
		SIZE size;
		size.cx = nCX;
		size.cy = nCY; 

		if(pIExtract == NULL)
		{ 
			break;
		} 

		// The IEIFLAG_ORIGSIZE flag tells it to use the original aspect
		// ratio for the image size. The IEIFLAG_QUALITY flag tells the 
		// interface we want the image to be the best possible quality.
		DWORD dwFlags = IEIFLAG_ORIGSIZE | IEIFLAG_QUALITY; 

		OLECHAR pathBuffer[MAX_PATH + 1] = {0};
		hr = pIExtract->GetLocation(pathBuffer, MAX_PATH + 1, NULL, &size, colorDepth, &dwFlags); 
		if (FAILED(hr))
		{
			TRACE(_T("The call to GetLocation failed"));
			break;
		}

		hr = pIExtract->Extract(&hBmp); //THIS IS WHERE I GET THE ERROR MESSAGE
		pIExtract->Release();
	} while(0);

	// Release the COM objects we have a reference to.
	if (pSub)
	{
		pSub->Release();
	}
	if (pDesktop)
	{
		pDesktop->Release();
	}

	return hBmp;
}

BOOL CCommonFun::AddAccessRights(TCHAR *lpszFileName, DWORD dwAccessMask)
{
#define heapalloc(x) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x))
#define heapfree(x)  (HeapFree(GetProcessHeap(), 0, x))
	// SID variables.SECURITY_DESCRIPTOR
	SID_NAME_USE   snuType;
	TCHAR *         szDomain       = NULL;
	DWORD          cbDomain       = 0;
	LPVOID         pUserSID       = NULL;
	DWORD          cbUserSID      = 0;

	// User name variables.
	TCHAR *         szUserName     = NULL;
	DWORD          cbUserName     = 0;

	// File SD variables.
	PSECURITY_DESCRIPTOR pFileSD  = NULL;
	DWORD          cbFileSD       = 0;

	// New SD variables.
	PSECURITY_DESCRIPTOR pNewSD   = NULL;

	// ACL variables.
	PACL           pACL           = NULL;
	BOOL           fDaclPresent;
	BOOL           fDaclDefaulted;
	ACL_SIZE_INFORMATION AclInfo;

	// New ACL variables.
	PACL           pNewACL        = NULL;
	DWORD          cbNewACL       = 0;

	// Temporary ACE.
	LPVOID         pTempAce       = NULL;
	UINT           CurrentAceIndex;

	// Assume function will fail.
	BOOL           fResult        = FALSE;
	BOOL           fAPISuccess;

	__try {

		// 
		// STEP 1: Get the logged on user name.
		// 
		/*
		fAPISuccess = GetUserName(szUserName, &cbUserName);

		// API should have failed with insufficient buffer.
		if (fAPISuccess)
		__leave;
		else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		printf("GetUserName() failed.  Error %d\n", GetLastError());
		__leave;
		}

		szUserName = (char *) heapalloc(cbUserName);
		if (!szUserName) {
		printf("HeapAlloc() failed.  Error %d\n", GetLastError());
		__leave;
		}

		fAPISuccess = GetUserName(szUserName, &cbUserName);
		if (!fAPISuccess) {
		printf("GetUserName() failed.  Error %d\n", GetLastError());
		__leave;
		}
		*/

		// 
		// STEP 2: Get SID for current user.
		// 
		szUserName = _T("everyone");
		fAPISuccess = LookupAccountName(NULL, szUserName,
			pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);

		// API should have failed with insufficient buffer.
		if (fAPISuccess)
			__leave;
		else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			WriteFileLog(_T("LookupAccountName() failed.  Error %d\n"), 
				GetLastError());
			__leave;
		}

		pUserSID = heapalloc(cbUserSID);
		if (!pUserSID) {
			WriteFileLog(_T("HeapAlloc() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		szDomain = (TCHAR *) heapalloc(cbDomain * sizeof(TCHAR));
		if (!szDomain) {
			WriteFileLog(_T("HeapAlloc() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		fAPISuccess = LookupAccountName(NULL, szUserName,
			pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);
		if (!fAPISuccess) {
			WriteFileLog(_T("LookupAccountName() failed.  Error %d\n"), 
				GetLastError());
			__leave;
		}

		// 
		// STEP 3: Get security descriptor (SD) for file.
		// 
		fAPISuccess = GetFileSecurity(lpszFileName, 
			DACL_SECURITY_INFORMATION, pFileSD, 0, &cbFileSD);

		// API should have failed with insufficient buffer.
		if (fAPISuccess)
			__leave;
		else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			WriteFileLog(_T("GetFileSecurity() failed.  Error %d\n"), 
				GetLastError());
			__leave;
		}

		pFileSD = heapalloc(cbFileSD);
		if (!pFileSD) {
			WriteFileLog(_T("HeapAlloc() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		fAPISuccess = GetFileSecurity(lpszFileName, 
			DACL_SECURITY_INFORMATION, pFileSD, cbFileSD, &cbFileSD);
		if (!fAPISuccess) {
			WriteFileLog(_T("GetFileSecurity() failed.  Error %d\n"), 
				GetLastError());
			__leave;
		}

		// 
		// STEP 4: Initialize new SD.
		// 
		pNewSD = heapalloc(cbFileSD); // Should be same size as FileSD.为什么？？？？？？？？？？？？？？
		if (!pNewSD) {
			WriteFileLog(_T("HeapAlloc() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		if (!InitializeSecurityDescriptor(pNewSD, 
			SECURITY_DESCRIPTOR_REVISION)) {
				WriteFileLog(_T("InitializeSecurityDescriptor() failed.  Error %d\n"), 
					GetLastError());
				__leave;
		}

		// 
		// STEP 5: Get DACL from SD.
		// 
		if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
			&fDaclDefaulted)) {
				WriteFileLog(_T("GetSecurityDescriptorDacl() failed.  Error %d\n"), 
					GetLastError());
				__leave;
		}

		// 
		// STEP 6: Get size information for DACL.
		// 
		AclInfo.AceCount = 0; // Assume NULL DACL.
		AclInfo.AclBytesFree = 0;
		AclInfo.AclBytesInUse = sizeof(ACL);

		// If not NULL DACL, gather size information from DACL.
		if (fDaclPresent && pACL) {    

			if(!GetAclInformation(pACL, &AclInfo, 
				sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
					WriteFileLog(_T("GetAclInformation() failed.  Error %d\n"),
						GetLastError());
					__leave;
			}
		}

		// 
		// STEP 7: Compute size needed for the new ACL.
		// 
		cbNewACL = AclInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) 
			+ GetLengthSid(pUserSID) - sizeof(DWORD);

		// 
		// STEP 8: Allocate memory for new ACL.
		// 
		pNewACL = (PACL) heapalloc(cbNewACL);
		if (!pNewACL) {
			WriteFileLog(_T("HeapAlloc() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		// 
		// STEP 9: Initialize the new ACL.
		// 
		if(!InitializeAcl(pNewACL, cbNewACL, ACL_REVISION2)) {
			WriteFileLog(_T("InitializeAcl() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		// SID
		// STEP 10: If DACL is present, copy it to a new DACL.
		// 
		if (fDaclPresent) 
		{

			// 
			// STEP 11: Copy the file's ACEs to the new ACL.
			// 
			if (AclInfo.AceCount) 
		 {

			 for (CurrentAceIndex = 0; 
				 CurrentAceIndex < AclInfo.AceCount;
				 CurrentAceIndex++) {

					 // 
					 // STEP 12: Get an ACE.
					 // ACE
					 if(!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
						 WriteFileLog(_T("GetAce() failed.  Error %d\n"), 
							 GetLastError());
						 __leave;
					 }


					 // 
					 // STEP 13: Add the ACE to the new ACL.
					 // 
					 if(!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
						 ((PACE_HEADER) pTempAce)->AceSize)) {
							 WriteFileLog(_T("AddAce() failed.  Error %d\n"), 
								 GetLastError());
							 __leave;
					 }
			 }
			}
		}

		// 
		// STEP 14: Add the access-allowed ACE to the new DACL.
		// 
		///

		if (!AddAccessAllowedAce(pNewACL, ACL_REVISION2, dwAccessMask,
			pUserSID)) {
				WriteFileLog(_T("AddAccessAllowedAce() failed.  Error %d\n"),
					GetLastError());
				__leave;
		}
		//-==================设置继承性===========================

		if( GetAce(pNewACL,AclInfo.AceCount,&pTempAce) )
		{
			((PACE_HEADER) pTempAce)->AceFlags = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		}

		//====================================================

		/*
		if( !AddAccessAllowedAceEx(pNewACL,ACL_REVISION2,INHERITED_ACE,dwAccessMask,pUserSID) )
		{
		printf("AddAccessAllowedAce() failed.  Error %d\n",
		GetLastError());
		__leave;
		}

		*/
		// 
		// STEP 15: Set the new DACL to the file SD.
		// 
		if (!SetSecurityDescriptorDacl(pNewSD, TRUE, pNewACL,FALSE)) 
		{
			WriteFileLog(_T("() failed.  Error %d\n"), GetLastError());
			__leave;
		}

		// 
		// STEP 16: Set the SD to the File.
		// 
		if (!SetFileSecurity(lpszFileName, DACL_SECURITY_INFORMATION,pNewSD)) 
		{
			WriteFileLog(_T("SetFileSecurity() failed.  Error %d\n"), 
				GetLastError());
			__leave;
		}

		fResult = TRUE;

	} __finally {

		// 
		// STEP 17: Free allocated memory
		// 
		//   if (szUserName)
		///     heapfree(szUserName);

		if (pUserSID)
			heapfree(pUserSID);

		if (szDomain)
			heapfree(szDomain);

		if (pFileSD)
			heapfree(pFileSD);

		if (pNewSD)
			heapfree(pNewSD);

		if (pNewACL)
			heapfree(pNewACL);
	}

	return fResult;
}

HBITMAP CCommonFun::CreateBITMAPFromDIB(int nWidth, int nHeight, int nBits/*=32*/)
{
	HBITMAP hbmp = NULL;

	BITMAPINFO bi = {0};
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biBitCount = nBits;
	bi.bmiHeader.biHeight = nHeight;
	bi.bmiHeader.biWidth = nWidth;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biCompression = BI_RGB;

	LPVOID pbits;
	hbmp = CreateDIBSection(NULL, &bi, 0, &pbits, NULL, 0);
	ASSERT(hbmp);

	return hbmp;
}

BOOL CCommonFun::ChangeIEProxy(BOOL bEnale, CStringA sServer, CStringA sOverride)
{
	HKEY hKeyIn = HKEY_CURRENT_USER, hKeyOut;
	if(ERROR_SUCCESS != RegOpenKeyExA(hKeyIn, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY, &hKeyOut))
		return FALSE;

	if(ERROR_SUCCESS != RegSetValueExA(hKeyOut, "ProxyEnable", 0, REG_DWORD, (BYTE*)&bEnale, sizeof(DWORD)))
		return FALSE;
	if(ERROR_SUCCESS != RegSetValueExA(hKeyOut, "ProxyServer", 0, REG_SZ, (const unsigned char *)sServer.LockBuffer(), sServer.GetLength() + 1))
		return FALSE;
	if(ERROR_SUCCESS != RegSetValueExA(hKeyOut, "ProxyOverride", 0, REG_SZ, (const unsigned char *)sOverride.LockBuffer(), sOverride.GetLength() + 1))
		return FALSE;

	RegCloseKey(hKeyOut);

	//使设置生效	//下面这句在win7下可能会导致前面的设置被还原
	//return InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	return TRUE;
}

//参考	HOWTO: Programmatically Query and Set Proxy Settings Under Internet Explorer 
//		Q226473
BOOL CCommonFun::ChangeIEProxy2(BOOL bEnale, CString sServer, CString sOverride)
{
	INTERNET_PER_CONN_OPTION_LIST list;
	INTERNET_PER_CONN_OPTION op[3];

	op[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	op[0].Value.pszValue = sServer.GetBuffer();
	op[1].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	op[1].Value.pszValue = sOverride.GetBuffer();
	op[2].dwOption = INTERNET_PER_CONN_FLAGS;
	op[2].Value.dwValue = bEnale?PROXY_TYPE_PROXY:PROXY_TYPE_DIRECT;

	list.dwSize = sizeof(list);
	list.pszConnection = NULL;
	list.dwOptionCount = 3;
	list.dwOptionError = 0;
	list.pOptions = op;

	if(!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, sizeof(list)))
	{
		CCommonFun::WriteFileLog(_T("设置IE代理失败！(%d)"), GetLastError());
		return FALSE;
	}

	CCommonFun::WriteFileLog(_T("设置IE代理成功！"));
	return TRUE;
}

//检查路径格式开关是否符合，如C:\或C:/等，不判断网络路径
BOOL CCommonFun::ValidatePathFormat(CString str)
{
	if(str.GetLength() < 3)
		return FALSE;

	TCHAR c = str.GetAt(0);
	if(c<_T('a') && c>_T('z') && c<_T('A') && c>_T('Z'))	//可只检测系统已有的盘符GetLogicalDrives
		return FALSE;

	c = str.GetAt(1);
	if(c != _T(':'))
		return FALSE;

	c = str.GetAt(2);
	if(c != _T('/') && c != _T('\\'))
		return FALSE;

	return TRUE;
}

BOOL CCommonFun::RunProcess(CString sPath,CString sParam,DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
	BOOL bOk = FALSE;
	int nLen = sPath.GetLength() + sParam.GetLength() + 10;
	TCHAR* pszPath = new TCHAR[nLen];
	ASSERT(pszPath);
	memset(pszPath,0,nLen*sizeof(TCHAR));

	_stprintf(pszPath, _T("\"%s\" %s"),sPath,sParam);
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
BOOL CCommonFun::RunProcess(CString sPath,CString sParam,int& nPid,DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
	BOOL bOk = FALSE;
	int nLen = sPath.GetLength() + sParam.GetLength() + 10;
	TCHAR* pszPath = new TCHAR[nLen];
	ASSERT(pszPath);
	memset(pszPath, 0, nLen * sizeof(TCHAR));

	_stprintf(pszPath, _T("\"%s\" %s"),sPath,sParam);
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
		nPid = pi.dwProcessId;
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

BOOL GetTokenByName(HANDLE &hToken, TCHAR* lpName)
{
	if (!lpName)
	{
		return FALSE;
	}
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			TCHAR *pExeFile = _tcsupr(pe32.szExeFile);
			TCHAR *pName = _tcsupr(lpName);
			if (!_tcsicmp(pExeFile, pName))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				CloseHandle(hProcess);
				return (bRet);
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	CloseHandle(hProcessSnap);
	return (bRet);
}


//RunProcessByGUISessionId是提供给服务进程调用，其它进程可能失败。
BOOL CCommonFun::RunProcessByGUISessionId(CString sPath, CString sParam, DWORD dwSessionId /*= 0xFFFFFFFF*/, DWORD dwWaitMilliseconds /*= INFINITE*/,WORD dwShowCmd /*= SW_SHOW*/)
{
#if 0
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
#endif

	WriteFileLog(_T("CCommonFun::RunProcessByGUISessionId,1,sPath=%s,sParam=%s,dwSessionId=%u,dwWaitMilliseconds=%u,dwShowCmd=%d")
		, sPath, sParam, dwSessionId, dwWaitMilliseconds, dwShowCmd);

	HANDLE hToken = NULL;
	HANDLE hTokenDup = NULL;
	LPVOID pEnv = NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if (!GetTokenByName(hToken, _T("EXPLORER.EXE")))
	{
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
	si.lpDesktop = _TEXT("winsta0\\default");
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

// 从srcArr中排除掉excArr的内容
BOOL CCommonFun::GetFilteredArray(CDWordArray& srcArr, CDWordArray& excArr)
{
	ASSERT(srcArr.GetCount() > 0);
	if(srcArr.GetCount() < 1)
		return FALSE;

	for(int i=srcArr.GetCount()-1; i>=0; i--)
	{
		for(int j=0; j<excArr.GetCount(); j++)
		{
			if(srcArr[i] == excArr[j])
			{
				srcArr.RemoveAt(i);
				break;
			}
		}
	}

	return TRUE;
}

BOOL CCommonFun::EnableCoolSkin(BOOL bEnable)
{
	CRegistry2 reg(2, FALSE);
	if(!reg.Open(PROJECT_REGISTY_KEY))
		return FALSE;

	if(!reg.Write(_T("NoCoolSkin"), !bEnable))
		return FALSE;

	return TRUE;
}
BOOL CCommonFun::OpenDirAndSelectFile(CString strFilePath,DWORD dwDelay)
{
	CString strCmd;
	strCmd.Format(_T(" /n,/select,%s"), strFilePath);
	CString strWindows;
	CCommonFun::SHGetSpecialPath(CSIDL_WINDOWS, strWindows);
	//ShellExecute(NULL, "open", , "", NULL, SW_SHOW);
	HINSTANCE hInst = ShellExecute(NULL, _T("open"), strWindows +_T("\\explorer.exe"), strCmd, NULL, SW_SHOW);
	::Sleep(dwDelay);
	HWND hWnd = ::GetForegroundWindow();
	if (hWnd)
	{
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	return TRUE;
}

CStringA CCommonFun::GetEncryptPwd(CStringA spwd)
{
	md5wrapper md5;
	std::string hash1 = md5.getHashFromString((LPCSTR)spwd);

	return CStringA(hash1.c_str());
}

BOOL CCommonFun::IsAdmin(void)   
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

CString CCommonFun::FormatString(LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	return sTmp;
}

CString CCommonFun::FormatString(UINT nID, ...)
{
	//  [10/15/2010 Song]
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString s;
	if(!s.LoadString(nID))
	{
		return _T("");
	}

	va_list argList;
	va_start(argList, nID);
	CString sTmp;
	sTmp.FormatV(s, argList);
	va_end(argList);

	return sTmp;
}

CString CCommonFun::GetMessageBoxTitle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString s;
	s.LoadString(IDS_TIP);

	return s;
}

int CCommonFun::MessageBox(HWND hwnd, CString sText, UINT nType)
{
	return ::MessageBox(hwnd, sText, GetMessageBoxTitle(), nType);
}

int CCommonFun::MessageBox(HWND hwnd, UINT nText, UINT nType)
{
	CString s;
	if(s.LoadString(nText))
		return ::MessageBox(hwnd, s, GetMessageBoxTitle(), nType);
	else
	{
		ASSERT(FALSE);
		return -1;
	}
}

CString CCommonFun::GetFaceName()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef AFX_TARG_CHT
	return _T("Tahoma");
#else
	return CFFS(IDS_SONGTI);
#endif
}

CString CCommonFun::GetCommonStr(COMMON_STR nType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString sRet;
	switch(nType)
	{
	case CS_BYTE:
		sRet.LoadString(IDS_BYTE);
		break;
	}

	return sRet;
}

BOOL CCommonFun::IsCHT()
{
#ifdef AFX_TARG_CHT
	return TRUE;
#else
	return FALSE;
#endif
}

CString CCommonFun::GetCapturePinStr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString s;
	s.LoadString(IDS_CAPTURE_PIN_STR);

	return s;
	//return s.AllocSysString();
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

CString CCommonFun::GetMsgName(UINT msg)
{
	CString s;
	if(msg > MAX_WMMSG)
	{
		//s.Format("UNKNOWN %d(0x%x)", msg, msg);
		s = CAkMsg::GetMsgName(msg);
	}
	else
		s = wmtab[msg];

	return s;
}

BOOL CCommonFun::CreateShellLink(LPCTSTR filePath, LPCTSTR lnkPath, LPCTSTR iconPath, int iconIndex, LPCTSTR params)
{
	::CoInitialize(NULL);
	//theLog.Write(_T("CreateShellLink [%s][%s][%s][%d][%s]", filePath, lnkPath, iconPath?iconPath:"", iconIndex, params?params:""));

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
#ifndef UNICODE
	WCHAR wsz[256] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, lnkPath, -1, wsz, MAX_PATH);
#else
	TCHAR wsz[256] = { 0 };
	_tcscpy(wsz, lnkPath);
#endif // !UNICODE

	hres = pPf->Save(wsz, TRUE);
	if(FAILED(hres))
		goto cleanup;

	if(iconPath != NULL) 
	{
		hres = psl->SetIconLocation(iconPath, iconIndex);
		if(FAILED(hres))
			goto cleanup;
		pPf->Save(wsz, TRUE);
	}
cleanup:
	if(pPf)
		pPf->Release();
	if(psl)
		psl->Release();

	::CoUninitialize();
	return TRUE;
}

BOOL CCommonFun::CreateURLLink(LPCTSTR url, LPCTSTR lnkPath, LPCTSTR iconPath, int iconIndex)
{
	::CoInitialize(NULL);
	//theLog.Write(_T("CreateShellLink [%s][%s][%s][%d]", url, lnkPath, iconPath, iconIndex));

	HRESULT hres;
	IUniformResourceLocator *purl;
	IPersistFile *ppf;
	IPropertySetStorage* psps;
	IPropertyStorage *pps;
	WCHAR wsz[256] = {0}; // buffer for Unicode string
	hres = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER,
		IID_IUniformResourceLocator, (void **)&purl);
	hres = purl->QueryInterface(IID_IPersistFile, (void **)&ppf);
	// Set URL
	hres = purl->SetURL(url, IURL_SETURL_FL_GUESS_PROTOCOL );
#ifndef UNICODE
	MultiByteToWideChar(CP_ACP, 0, lnkPath, -1, wsz, MAX_PATH);
#else
	_tcscpy(wsz, lnkPath);
#endif // !UNICODE
	// Save shortcut (with remember)
	hres = ppf->Save( wsz, TRUE );

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
#ifndef UNICODE
		MultiByteToWideChar(CP_ACP, 0, iconPath, -1, wsz, MAX_PATH);
#else
		_tcscpy(wsz, iconPath);
#endif
		// Initialize PropVars
		PropVariantInit( ppvar );
		PropVariantInit( ppvar + 1 );
		ppvar[0].vt = VT_I4; // Index is I4
		ppvar[0].lVal = iconIndex;
		ppvar[1].vt = VT_LPWSTR; // Iconfile is LPWSTR
		ppvar[1].pwszVal = wsz; // Name of Iconfile (shell32.dll)
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

	::CoUninitialize();
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

BOOL CCommonFun::CheckUserPasswordInDomain(CString strAccountName, CString strPassword, CString strDomain /*= ""*/)
{
	HRESULT hr; 
	if (strDomain.GetLength() == 0)
	{
		HRESULT hr;
		IADsADSystemInfo *pSys;
		hr = CoCreateInstance(CLSID_ADSystemInfo,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IADsADSystemInfo,
			(void**)&pSys);
		BSTR bstr;
		if (SUCCEEDED(hr) && SUCCEEDED(pSys->get_DomainDNSName(&bstr)))
		{
			strDomain = bstr;
			SysFreeString(bstr);
		}
		else
			strDomain = "rootDSE";

		if(pSys)
			pSys->Release();
	}
	CString str, str1, str2; 
	IADs *pADs = NULL; 
	BSTR bstrUrl, bstrName, bstrPass; 
	str=_T("LDAP://")+ strDomain; 
	bstrUrl = str.AllocSysString(); 
	str1 = strDomain + _T("\\") + strAccountName; 
	bstrName = str1.AllocSysString(); 
	str2 = strPassword; 
	bstrPass = str2.AllocSysString(); 
	hr = ADsOpenObject( bstrUrl, bstrName, bstrPass, ADS_SECURE_AUTHENTICATION, IID_IADs, (void**)&pADs); 
	SysFreeString( bstrUrl );  
	SysFreeString( bstrName );  
	SysFreeString( bstrPass );  
	if ( hr != S_OK || NULL == pADs)  
	{
		CCommonFun::WriteFileLog(_T("CheckUserPasswordInDomain hr = %X"),hr);
		return FALSE;
	}
	if (pADs)
	{
		pADs->Release();
		pADs = NULL;
	}
	return TRUE;
}

#include <windows.h>
#include <winldap.h>
#include <winber.h>
#include <rpcdce.h>
#include <stdio.h>
#pragma comment(lib, "Wldap32.lib")

BOOL CCommonFun::LDAPCheckUser(TCHAR* pUserName, TCHAR* pPassword, TCHAR* hostName, ULONG PortNumber, TCHAR* pDn)
{
	CCommonFun::WriteFileLog(_T("LDAPCheckUser pUserName=%s pPassword=%s PortNumber=%d pDn=%s hostName=%s"), 
		pUserName,pPassword,PortNumber,pDn,hostName);

	//PCHAR hostName = "fabrikam.com";
	LDAP* pLdapConnection = NULL;
	if (LDAP_SSL_PORT == PortNumber)
		pLdapConnection = ldap_sslinit(hostName,PortNumber,1);
	else
		pLdapConnection = ldap_init(hostName, PortNumber);

	if (pLdapConnection == NULL)
	{
		CCommonFun::WriteFileLog(_T("ldap_init failed with 0x%x."),LdapGetLastError());
		ldap_unbind(pLdapConnection);
		return FALSE;
	}

	//-------------------------------------------------------
	// Set session options.
	//-------------------------------------------------------
	ULONG version = LDAP_VERSION3;
	ULONG numReturns = 10;
	ULONG lRtn = 0;

	// Set the version to 3.0 (default is 2.0).
	lRtn = ldap_set_option(
		pLdapConnection,           // Session handle
		LDAP_OPT_PROTOCOL_VERSION, // Option
		(void*) &version);         // Option value

	if(lRtn != LDAP_SUCCESS)
	{
		CCommonFun::WriteFileLog(_T("SetOption Error:%0lX\n"), lRtn);
		ldap_unbind(pLdapConnection);
		return FALSE;
	}

	if (LDAP_SSL_PORT != PortNumber)
	{
		// Set the limit on the number of entries returned to 10.
		lRtn = ldap_set_option(
			pLdapConnection,       // Session handle
			LDAP_OPT_SIZELIMIT,    // Option
			(void*) &numReturns);  // Option value

		if(lRtn != LDAP_SUCCESS)
		{
			CCommonFun::WriteFileLog(_T("SetOption Error:%0lX\n"), lRtn);
			ldap_unbind(pLdapConnection);
			return FALSE;
		}
	}

	//--------------------------------------------------------
	// Connect to the server.
	//--------------------------------------------------------
	lRtn = ldap_connect(pLdapConnection, NULL);
	if(lRtn != LDAP_SUCCESS)
	{
		CCommonFun::WriteFileLog(_T("ldap_connect failed with 0x%lx.\n"),lRtn);
		ldap_unbind(pLdapConnection);
		return FALSE;
	}

	TCHAR cDn[260] = {0};
	if (LDAP_SSL_PORT == PortNumber)
		_stprintf(cDn, _T("%s"), pUserName);
	else
		_stprintf(cDn, _T("uid=%s,%s"), pUserName, pDn);
	lRtn = ldap_simple_bind_s(
		pLdapConnection,      // Session Handle
		cDn,                // Domain DN
		pPassword);		// pws
	if(lRtn == LDAP_SUCCESS)
	{
		pPassword = NULL;         // Remove password pointer
	}
	else
	{
		CCommonFun::WriteFileLog(_T("ldap_simple_bind_s failed with 0x%lx.\n"),lRtn);
		ldap_unbind(pLdapConnection);
		return FALSE;
	}

	ldap_unbind(pLdapConnection);
	return TRUE;

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
	p.shi2_permissions = ACCESS_READ;    
	p.shi2_max_uses = -1;
	p.shi2_current_uses = 0; 
	p.shi2_path = pwcDirPath;
	p.shi2_passwd = NULL; 

	// Call the NetShareAdd function,
	res=NetShareAdd(NULL,2,(LPBYTE)&p,&parm_err);
	if(res == NERR_Success)
		return TRUE;
	else
		return FALSE;
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

// 得到例如：notepad.exe路径
CString CCommonFun::GShortcutRunPath(CString sName)
{
	CString sRet = _T("");
	HKEY hKey = NULL;

	// 如果最后4位不是.exe，自动添加
	if (sName.Right(4).Compare(_T(".exe")) != 0)
		sName += _T(".exe");

	// 先通过注册表来查找，如果找不到再到系统目录下面查找，如果再找不到，则不处理
	do 
	{
		// 注册表查询
		CString sPath;
		sPath.Format(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s"),sName);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,sPath,NULL,KEY_ALL_ACCESS,&hKey) == ERROR_SUCCESS)
		{
			DWORD dwType;
			DWORD dwSize = MAX_PATH;
			char  szString[MAX_PATH];
			memset(szString,0,MAX_PATH);

			// 如果找到对应的路径，则直接返回
			if (RegQueryValueEx(hKey,NULL,NULL,&dwType,(BYTE*)szString,&dwSize) == ERROR_SUCCESS)
			{
				sRet.Format(_T("%s"),szString);
				sRet.TrimLeft();
				sRet.TrimRight();

				// 如果多"",删除之
				if (sRet.Left(1).Compare(_T("\"")) == 0 || sRet.Right(1).Compare(_T("\"")) == 0)
					sRet = sRet.Mid(1,sRet.GetLength() - 2);

				// 如果存在%ProgramFiles%
				if (sRet.Find(_T("%ProgramFiles%")) != -1)
				{
					CString sProgramPath;
					SHGetSpecialPath(CSIDL_PROGRAM_FILES,sProgramPath);
					sRet.Replace(_T("%ProgramFiles%"),sProgramPath);
				}

				// 如果存在%ProgramFiles%
				if (sRet.Find(_T("%SystemRoot%")) != -1)
				{
					CString sSysPath;
					SHGetSpecialPath(CSIDL_SYSTEM,sSysPath);
					sRet.Replace(_T("%ProgramFiles%"),sSysPath);
				}
				break;
			}
		}

		// 在没有找到的前提下，获取当前操作系统目录
		CString sSystemPath;
		SHGetSpecialPath(CSIDL_SYSTEM,sSystemPath);
		if (sSystemPath.IsEmpty())
			break;

		sPath.Format(_T("%s\\%s"),sSystemPath,sName);
		if (PathFileExists(sPath))
		{
			sRet = sPath;
			break;
		}

		// 在没有找到的前提下，获取当前操作系统windows目录
		sSystemPath.Empty();
		SHGetSpecialPath(CSIDL_WINDOWS,sSystemPath);
		if (sSystemPath.IsEmpty())
			break;

		sPath.Format(_T("%s\\%s"),sSystemPath,sName);
		if (PathFileExists(sPath))
		{
			sRet = sPath;
			break;
		}
	} while (0);
	return sRet;
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

#ifdef AFX_TARG_JPN
	bi.lpszTitle= _T("フォルダを選択してください"); 
#else
	bi.lpszTitle= _T("选择文件夹"); 
#endif

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
	
	CoUninitialize();

	return szDir;
}

BOOL CCommonFun::IsIPHost(CString sHost)
{
	BOOL bRet = TRUE;
	CStringArray ary;
	CCommonFun::StringSplit(sHost,&ary,_T("."));
	if (ary.GetSize() == 4)
	{
		for (int n = 0; n < ary.GetSize(); n++)
		{
			int b = _tstoi(ary.GetAt(n));
			if (b > 0 && b < 255)
			{
				continue;
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}



BOOL CCommonFun::IsInCredit(CString sHost)
{
	CString sKeyPath;
	CString sKeyPath2;
	BOOL bIsIp = FALSE;
	if (bIsIp = IsIPHost(sHost))
	{
		sKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\EscRanges");
		sKeyPath2 = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Ranges");
	}
	else
	{
		sKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\EscDomains");
	}
	CRegistry2* pReg = NULL;
	CString sWinUser;
	CSecurityTool::GetCurrProcessUser(sWinUser);
	if(sWinUser.CompareNoCase(_T("system"))==0)
	{
		CString sSID;
		CSecurityTool::GetCurUserSID(sSID);
		if (sSID.IsEmpty())
		{
			return FALSE;
		}
		pReg = new CRegistry2(3,FALSE);
		sKeyPath.Format(_T("%s\\%s"),sSID,sKeyPath);
		sKeyPath2.Format(_T("%s\\%s"),sSID,sKeyPath2);
	}
	else
	{
		pReg = new CRegistry2(0,FALSE);
	}
	ASSERT(pReg);
	if (!pReg)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	if (bIsIp)
	{
		if(pReg->Open(sKeyPath) || pReg->Open(sKeyPath2))
		{
			bRet = pReg->FindSubKeyDataVal(_T(":Range"),sHost);
		}
	}
	else
	{
		bRet = pReg->FindSubKey(sHost);
	}
	delete pReg;
	return bRet;
}

BOOL CCommonFun::AddHostToCredit(CString sHost)
{
	if (IsInCredit(sHost))
	{
		return TRUE;
	}
	CString sKeyPath;
	CString sKeyPath2;
	BOOL bIsIp = FALSE;
	if (bIsIp = IsIPHost(sHost))
	{
		sKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\EscRanges");
		sKeyPath2 = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Ranges");
	}
	else
	{
		sKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\EscDomains");
	}
	CRegistry2* pReg = NULL;
	CString sWinUser;
	CSecurityTool::GetCurrProcessUser(sWinUser);
	if(sWinUser.CompareNoCase(_T("system"))==0)
	{
		CString sSID;
		CSecurityTool::GetCurUserSID(sSID);
		if (sSID.IsEmpty())
		{
			return FALSE;
		}
		pReg = new CRegistry2(3,FALSE);
		sKeyPath.Format(_T("%s\\%s"),sSID,sKeyPath);
		sKeyPath2.Format(_T("%s\\%s"),sSID,sKeyPath2);
	}
	else
	{
		pReg = new CRegistry2(0,FALSE);
	}
	ASSERT(pReg);
	if (!pReg)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	DWORD dwSubKeyCount = 0;


	if(pReg->Open(sKeyPath) || (!sKeyPath2.IsEmpty() && pReg->Open(sKeyPath2)))
	{
		if (bIsIp)
		{
			if(pReg->QuerySubKeyCount(dwSubKeyCount))
			{
				CString sSubKey;
				sSubKey.Format(_T("Range%d"),dwSubKeyCount + 1);
				if(pReg->CreateKey(sSubKey))
				{
					pReg->Write(_T(":Range"),sHost);
					pReg->Write(_T("http"),2);
					bRet = TRUE;
				}
			}
		}
		else
		{
			if (pReg->CreateKey(sHost))
			{
				pReg->Write(_T("http"),2);
				bRet = TRUE;
			}
		}
	}
	delete pReg;
	return bRet;
}

CString CCommonFun::GetPagerSize(int nIndex)
{
	int nCount = (sizeof(pagerSizetab)/sizeof(pagerSizetab[0]));
	if (nIndex > 0 && nIndex < nCount )
	{
		return pagerSizetab[nIndex].cPaperName;
	}
	return _T("unknow");
}

int CCommonFun::GetPagerSize(CString szPageSizeStr)
{
	int nPaperSize = 0;
	int nCount = (sizeof(pagerSizetab) / sizeof(pagerSizetab[0]));
	for (int nIndex = 1; nIndex < nCount; nIndex++)
	{
		if (szPageSizeStr.CompareNoCase(pagerSizetab[nIndex].cPaperName) == 0 )
		{
			nPaperSize = pagerSizetab[nIndex].nPaperSize;
			break;
		}
	}
	return nPaperSize;
}

int CCommonFun::GetPagerSize2(int nWidthTenthsOfMillimeter, int nHeightTenthsOfMillimeter)
{
	int nPaperSize = 0;
	int nPhysicalArea = nWidthTenthsOfMillimeter * nHeightTenthsOfMillimeter;
	int nCount = (sizeof(pagerSizetab)/sizeof(pagerSizetab[0]));
	for (int nIndex=1; nIndex<nCount; nIndex++)
	{
		if (pagerSizetab[nIndex].nPhysicalArea == nPhysicalArea)
		{
			nPaperSize = pagerSizetab[nIndex].nPaperSize;
			break;
		}
	}
	return nPaperSize;
}

int CCommonFun::GetPagerSize2Ex(int nWidthTenthsOfMillimeter, int nHeightTenthsOfMillimeter)
{
	int nPaperDiffFactor = 0x7FFFFFFF;	//纸型误差因子
	int nPaperSize = 0;
	int nCount = (sizeof(pagerSizetab)/sizeof(pagerSizetab[0]));
	for (int nIndex=1; nIndex<nCount; nIndex++)
	{
		int nWidthDif = pagerSizetab[nIndex].nPhysicalWidthMMT-nWidthTenthsOfMillimeter;
		int nHeigthDif = pagerSizetab[nIndex].nPhysicalHeightMMT-nHeightTenthsOfMillimeter;
		int nTemp = (nWidthDif * nWidthDif) + (nHeigthDif * nHeigthDif);	
		if (nTemp < nPaperDiffFactor)//取差平方最小的匹配纸型
		{
			nPaperDiffFactor = nTemp;
			nPaperSize = nIndex;
		}

		if (nPaperDiffFactor <= 50)	//即长宽分别最大不能误差0.5mm，则认为与此纸型匹配
		{
			break;
		}
	}
	return nPaperSize;
}

CString CCommonFun::GetPagerSize3(int nWidthTenthsOfMillimeter, int nHeightTenthsOfMillimeter)
{
	return GetPagerSize(GetPagerSize2(nWidthTenthsOfMillimeter, nHeightTenthsOfMillimeter));
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

//屏保操作
BOOL CCommonFun::IsScreenSaverRunning()
{
	BOOL bValue = FALSE;
	if (!SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &bValue, 0))
	{
		WriteFileLog(_T("!!CCommonFun::IsScreenSaverRunning,SystemParametersInfo err=%u"), GetLastError());
	}
	return bValue;
}

void CCommonFun::ExitScreenSaverRunning()
{
	//简单的构拟键盘【CTRL】键，退出屏保状态
	keybd_event(VK_CONTROL, 0, 0, 0);	//按下
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);	//弹起，否则会一直处于按下状态
}

BOOL CCommonFun::GetScreenSaverInfo(BOOL& bActive, DWORD& dwTimeOutSeconds)
{
	if (!SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bActive, 0))
	{
		WriteFileLog(_T("!!CCommonFun::GetScreenSaverInfo,1,SystemParametersInfo err=%u"), GetLastError());
		return FALSE;
	}
	if (!SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &dwTimeOutSeconds, 0))
	{
		WriteFileLog(_T("!!CCommonFun::GetScreenSaverInfo,2,SystemParametersInfo err=%u"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL CCommonFun::SetScreenSaverInfo(BOOL bActive, DWORD dwTimeOutSeconds)
{
	if (!SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, bActive, 0, 0))
	{
		WriteFileLog(_T("!!CCommonFun::SetScreenSaverInfo,1,SystemParametersInfo err=%u"), GetLastError());
		return FALSE;
	}
	if (!SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, dwTimeOutSeconds, 0, 0))
	{
		WriteFileLog(_T("!!CCommonFun::SetScreenSaverInfo,2,SystemParametersInfo err=%u"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

//反斜杠Unicode转换成多字节,//【测试\u6d4b\u8bd5】=【测试测试】
CStringA CCommonFun::BslashUnicodeToMultiByte(CStringA szUnicode)
{
	CStringA szRet = "";
	unsigned char ucUnicode[4] = {0};	//可以保存一个Unicode字符和一个结束符
	int nCurrent = 0;
	int nUnicodeLen = szUnicode.GetLength();
	while (nCurrent < nUnicodeLen)
	{
		//"file_path":"E:\\\\\u672c.doc"
		//"file_path":"E:\\usb.doc"
		//在以上两种路径中，可能出现解析出问题的情况，json对【\】会转义成【\\】,所以跳过【\\】
		if (szUnicode.GetAt(nCurrent) == '\\' && szUnicode.GetAt(nCurrent+1) == '\\')
		{
			szRet.Append(szUnicode.Mid(nCurrent, 2));
			nCurrent += 2;
			continue;
		}

		if ((szUnicode.Mid(nCurrent, 2).Compare("\\u") == 0) && ((nCurrent+6) <= nUnicodeLen))
		{
			ucUnicode[1] = strtol(szUnicode.Mid(nCurrent+2, 2), NULL, 16);	//小端在前
			ucUnicode[0] = strtol(szUnicode.Mid(nCurrent+4, 2), NULL, 16);
			szRet.Append(CStringA((wchar_t*)ucUnicode));
			nCurrent += 6;
		}
		else
		{
			szRet.Append(szUnicode.Mid(nCurrent, 1));
			nCurrent += 1;
		}
	}
	return szRet;
}

CString CCommonFun::GetCardIdInt(CString szCardIdHex, BOOL bReverse/* = FALSE*/)
{
	unsigned long lCardInt = _tcstoul(szCardIdHex,NULL,16);
	if (bReverse)
	{
		lCardInt = ReverseCard(lCardInt);
	}
	CString sTemp;
	sTemp.Format(_T("%u"), lCardInt);
	return sTemp;
}

unsigned long CCommonFun::GetCardIdInt2(CString szCardIdHex, BOOL bReverse /*= FALSE*/)
{
	unsigned long lCardInt = _tcstoul(szCardIdHex,NULL,16);
	if (bReverse)
	{
		lCardInt = ReverseCard(lCardInt);
	}
	return lCardInt;
}

unsigned long CCommonFun::GetCardIdInt3(CString szCardIdInt, BOOL bReverse /*= FALSE*/)
{
	unsigned long lCardInt = _tcstoul(szCardIdInt,NULL,10);
	if (bReverse)
	{
		lCardInt = ReverseCard(lCardInt);
	}
	return lCardInt;
}

unsigned long CCommonFun::ReverseCard(unsigned long ulCard)
{
	return (((ulCard&0x000000FF)<<24) + ((ulCard&0x0000FF00)<<8) + ((ulCard&0x00FF0000)>>8) + ((ulCard&0xFF000000)>>24));
}

int CCommonFun::Wiegand34To26(unsigned long ulWiegand34)
{
	return ((ulWiegand34 % (256 * 256)) + (ulWiegand34 / (256 * 256) % 256) * 100000);
}

CString CCommonFun::HandleCard(CString szStatement, CString szOrgCardNo)
{
	szStatement.Replace(_T(" "),_T(""));
	szStatement.Replace(_T("CARD"), szOrgCardNo);
	if (szStatement.IsEmpty())
	{
		return szOrgCardNo;
	}
	return CCardAnalyst::GetInstance().HandleCardNo(szStatement);
}

//将double转换成时间字符串,格式为yyyy-mm-dd HH:MM:SS
CString CCommonFun::DoubleToTimeString(double fTime)
{
	COleDateTime time;
	time.m_dt = fTime;
	CString szDate = time.Format(_T("%Y-%m-%d %H:%M:%S"));
	return szDate;
}

//获取CPU信息：http://www.cnblogs.com/zyl910/archive/2012/08/06/getcpuid_gcc.html
void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)    // GCC
	__cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
#if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
	if (NULL==CPUInfo)    return;
	_asm{
		// load. 读取参数到寄存器.
		mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID
		cpuid;
		// save. 将寄存器保存到CPUInfo
		mov    [edi], eax;
		mov    [edi+4], ebx;
		mov    [edi+8], ecx;
		mov    [edi+12], edx;
	}
#endif
#endif    // #if defined(__GNUC__)
}

#include <intrin.h>
void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)    // GCC
	__cpuid(InfoType, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
#if _MSC_VER>=1400    // VC2005才支持__cpuid
	__cpuid((int*)(void*)CPUInfo, (int)InfoType);
#else
	getcpuidex(CPUInfo, InfoType, 0);
#endif
#endif    // #if defined(__GNUC__)
}

// 取得CPU厂商（Vendor）.
//
// result: 成功时返回字符串的长度（一般为12）。失败时返回0.
// pvendor: 接收厂商信息的字符串缓冲区。至少为13字节.
int cpu_getvendor(TCHAR* pvendor)
{
	unsigned int dwBuf[4];
	if (NULL==pvendor)    return 0;
	// Function 0: Vendor-ID and Largest Standard Function
	getcpuid(dwBuf, 0);
	// save. 保存到pvendor
	*(unsigned int *)&pvendor[0] = dwBuf[1];    // ebx: 前四个字符.
	*(unsigned int *)&pvendor[4] = dwBuf[3];    // edx: 中间四个字符.
	*(unsigned int *)&pvendor[8] = dwBuf[2];    // ecx: 最后四个字符.
	pvendor[12] = '\0';
	return 12;
}

// 取得CPU商标（Brand）.
//
// result: 成功时返回字符串的长度（一般为48）。失败时返回0.
// pbrand: 接收商标信息的字符串缓冲区。至少为49字节.
int cpu_getbrand(char* pbrand)
{
	unsigned int dwBuf[4];
	if (NULL==pbrand)    return 0;
	// Function 0x80000000: Largest Extended Function Number
	getcpuid(dwBuf, 0x80000000U);
	if (dwBuf[0] < 0x80000004U)    return 0;
	// Function 80000002h,80000003h,80000004h: Processor Brand String
	getcpuid((unsigned int *)&pbrand[0], 0x80000002U);    // 前16个字符.
	getcpuid((unsigned int *)&pbrand[16], 0x80000003U);    // 中间16个字符.
	getcpuid((unsigned int *)&pbrand[32], 0x80000004U);    // 最后16个字符.
	pbrand[48] = '\0';
	return 48;
}

CString GetCPUID2()
{
	CString CPUID;
	CString CPUID1;
	CString CPUID2;
	unsigned long s1;
	unsigned long s2;

	__asm
	{
		mov eax,01h   //eax=1:取CPU序列号
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,eax
	}
	CPUID1.Format(_T("%08X%08X"),s1,s2);

	__asm
	{
		mov eax,03h
			xor ecx,ecx
			xor edx,edx
			cpuid
			mov s1,edx
			mov s2,ecx
	}
	CPUID2.Format(_T("%08X%08X"),s1,s2);

	CPUID = CPUID1 + CPUID2;
	return CPUID;
}

CString CCommonFun::GetCPUID()
{
	//当时测试了三台机器,发现intel的两个机器在方法一中是一样的结果，
	//而方法二中三台都不一样,所以好像方法二更靠谱点。
	//Intel(R) Core(TM) i3-4170 CPU @ 3.70GHz
	//Pentium(R) Dual-Core	CPU	E5700 @ 3.00Hz
	//AMD FX(tm)-6300 Six-Core Processor
#if 0	
	//方法一
	CString CPUID;
	unsigned int dwBuf[4] = {0};
	getcpuid(dwBuf, 0);
	CPUID.Format("%08X%08X%08X%08X", dwBuf[0],dwBuf[1],dwBuf[2],dwBuf[3]);
	return CPUID;
#else
	//方法二
	return GetCPUID2();
#endif
}

CString CCommonFun::GetCPUVendor()
{
	CString CPUVendor;
	TCHAR szBuf[64] = {0};
	cpu_getvendor(szBuf);
	CPUVendor.Format(_T("%s"), szBuf);
	return CPUVendor;
}

CString CCommonFun::GetCPUBrand()
{
	CString CPUBrand;
	char szBuf[64] = {0};
	cpu_getbrand(szBuf);
#ifndef UNICODE
	CPUBrand.Format(_T("%s"), szBuf);
#else
	CStringA CPUBrandA;
	CPUBrandA.Format("%s", szBuf);
	CPUBrand.Format(_T("%s"), MultiByteToUnicode(CPUBrandA));
#endif
	return CPUBrand;
}

TCHAR* CCommonFun::ConvertChineseUnicodeToPyt(std::string szU8)
{
	wchar_t* chrstr;
	int wcsLen = MultiByteToWideChar(CP_ACP, NULL, szU8.c_str(), strlen( szU8.c_str()), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];  
	MultiByteToWideChar(CP_ACP, NULL,  szU8.c_str(), strlen( szU8.c_str()), wszString, wcsLen);
	wszString[wcsLen] = '\0'; 
	chrstr = wszString;
	const  int pyvalue[] = {-20319,-20317,-20304,-20295,-20292,-20283,-20265,-20257,-20242,-20230,-20051,-20036,-20032,-20026,
		-20002,-19990,-19986,-19982,-19976,-19805,-19784,-19775,-19774,-19763,-19756,-19751,-19746,-19741,-19739,-19728,
		-19725,-19715,-19540,-19531,-19525,-19515,-19500,-19484,-19479,-19467,-19289,-19288,-19281,-19275,-19270,-19263,
		-19261,-19249,-19243,-19242,-19238,-19235,-19227,-19224,-19218,-19212,-19038,-19023,-19018,-19006,-19003,-18996,
		-18977,-18961,-18952,-18783,-18774,-18773,-18763,-18756,-18741,-18735,-18731,-18722,-18710,-18697,-18696,-18526,
		-18518,-18501,-18490,-18478,-18463,-18448,-18447,-18446,-18239,-18237,-18231,-18220,-18211,-18201,-18184,-18183,
		-18181,-18012,-17997,-17988,-17970,-17964,-17961,-17950,-17947,-17931,-17928,-17922,-17759,-17752,-17733,-17730,
		-17721,-17703,-17701,-17697,-17692,-17683,-17676,-17496,-17487,-17482,-17468,-17454,-17433,-17427,-17417,-17202,
		-17185,-16983,-16970,-16942,-16915,-16733,-16708,-16706,-16689,-16664,-16657,-16647,-16474,-16470,-16465,-16459,
		-16452,-16448,-16433,-16429,-16427,-16423,-16419,-16412,-16407,-16403,-16401,-16393,-16220,-16216,-16212,-16205,
		-16202,-16187,-16180,-16171,-16169,-16158,-16155,-15959,-15958,-15944,-15933,-15920,-15915,-15903,-15889,-15878,
		-15707,-15701,-15681,-15667,-15661,-15659,-15652,-15640,-15631,-15625,-15454,-15448,-15436,-15435,-15419,-15416,
		-15408,-15394,-15385,-15377,-15375,-15369,-15363,-15362,-15183,-15180,-15165,-15158,-15153,-15150,-15149,-15144,
		-15143,-15141,-15140,-15139,-15128,-15121,-15119,-15117,-15110,-15109,-14941,-14937,-14933,-14930,-14929,-14928,
		-14926,-14922,-14921,-14914,-14908,-14902,-14894,-14889,-14882,-14873,-14871,-14857,-14678,-14674,-14670,-14668,
		-14663,-14654,-14645,-14630,-14594,-14429,-14407,-14399,-14384,-14379,-14368,-14355,-14353,-14345,-14170,-14159,
		-14151,-14149,-14145,-14140,-14137,-14135,-14125,-14123,-14122,-14112,-14109,-14099,-14097,-14094,-14092,-14090,
		-14087,-14083,-13917,-13914,-13910,-13907,-13906,-13905,-13896,-13894,-13878,-13870,-13859,-13847,-13831,-13658,
		-13611,-13601,-13406,-13404,-13400,-13398,-13395,-13391,-13387,-13383,-13367,-13359,-13356,-13343,-13340,-13329,
		-13326,-13318,-13147,-13138,-13120,-13107,-13096,-13095,-13091,-13076,-13068,-13063,-13060,-12888,-12875,-12871,
		-12860,-12858,-12852,-12849,-12838,-12831,-12829,-12812,-12802,-12607,-12597,-12594,-12585,-12556,-12359,-12346,
		-12320,-12300,-12120,-12099,-12089,-12074,-12067,-12058,-12039,-11867,-11861,-11847,-11831,-11798,-11781,-11604,
		-11589,-11536,-11358,-11340,-11339,-11324,-11303,-11097,-11077,-11067,-11055,-11052,-11045,-11041,-11038,-11024,
		-11020,-11019,-11018,-11014,-10838,-10832,-10815,-10800,-10790,-10780,-10764,-10587,-10544,-10533,-10519,-10331,
		-10329,-10328,-10322,-10315,-10309,-10307,-10296,-10281,-10274,-10270,-10262,-10260,-10256,-10254};
	const TCHAR pystr[396][7] = {_T("a"),_T("ai"),_T("an"),_T("ang"),_T("ao"),_T("ba"),_T("bai"),_T("ban"),_T("bang"),_T("bao"),_T("bei"),_T("ben"),_T("beng"),_T("bi"),_T("bian"),_T("biao"),_T(
		"bie"),_T("bin"),_T("bing"),_T("bo"),_T("bu"),_T("ca"),_T("cai"),_T("can"),_T("cang"),_T("cao"),_T("ce"),_T("ceng"),_T("cha"),_T("chai"),_T("chan"),_T("chang"),_T("chao"),_T("che"),_T("chen"),_T(
		"cheng"),_T("chi"),_T("chong"),_T("chou"),_T("chu"),_T("chuai"),_T("chuan"),_T("chuang"),_T("chui"),_T("chun"),_T("chuo"),_T("ci"),_T("cong"),_T("cou"),_T("cu"),_T("cuan"),_T("cui"),_T(
		"cun"),_T("cuo"),_T("da"),_T("dai"),_T("dan"),_T("dang"),_T("dao"),_T("de"),_T("deng"),_T("di"),_T("dian"),_T("diao"),_T("die"),_T("ding"),_T("diu"),_T("dong"),_T("dou"),_T("du"),_T("duan"),_T(
		"dui"),_T("dun"),_T("duo"),_T("e"),_T("en"),_T("er"),_T("fa"),_T("fan"),_T("fang"),_T("fei"),_T("fen"),_T("feng"),_T("fo"),_T("fou"),_T("fu"),_T("ga"),_T("gai"),_T("gan"),_T("gang"),_T("gao"),_T(
		"ge"),_T("gei"),_T("gen"),_T("geng"),_T("gong"),_T("gou"),_T("gu"),_T("gua"),_T("guai"),_T("guan"),_T("guang"),_T("gui"),_T("gun"),_T("guo"),_T("ha"),_T("hai"),_T("han"),_T("hang"),_T(
		"hao"),_T("he"),_T("hei"),_T("hen"),_T("heng"),_T("hong"),_T("hou"),_T("hu"),_T("hua"),_T("huai"),_T("huan"),_T("huang"),_T("hui"),_T("hun"),_T("huo"),_T("ji"),_T("jia"),_T("jian"),_T(
		"jiang"),_T("jiao"),_T("jie"),_T("jin"),_T("jing"),_T("jiong"),_T("jiu"),_T("ju"),_T("juan"),_T("jue"),_T("jun"),_T("ka"),_T("kai"),_T("kan"),_T("kang"),_T("kao"),_T("ke"),_T("ken"),_T(
		"keng"),_T("kong"),_T("kou"),_T("ku"),_T("kua"),_T("kuai"),_T("kuan"),_T("kuang"),_T("kui"),_T("kun"),_T("kuo"),_T("la"),_T("lai"),_T("lan"),_T("lang"),_T("lao"),_T("le"),_T("lei"),_T(
		"leng"),_T("li"),_T("lia"),_T("lian"),_T("liang"),_T("liao"),_T("lie"),_T("lin"),_T("ling"),_T("liu"),_T("long"),_T("lou"),_T("lu"),_T("lv"),_T("luan"),_T("lue"),_T("lun"),_T("luo"),_T(
		"ma"),_T("mai"),_T("man"),_T("mang"),_T("mao"),_T("me"),_T("mei"),_T("men"),_T("meng"),_T("mi"),_T("mian"),_T("miao"),_T("mie"),_T("min"),_T("ming"),_T("miu"),_T("mo"),_T("mou"),_T("mu"),_T(
		"na"),_T("nai"),_T("nan"),_T("nang"),_T("nao"),_T("ne"),_T("nei"),_T("nen"),_T("neng"),_T("ni"),_T("nian"),_T("niang"),_T("niao"),_T("nie"),_T("nin"),_T("ning"),_T("niu"),_T("nong"),_T(
		"nu"),_T("nv"),_T("nuan"),_T("nue"),_T("nuo"),_T("o"),_T("ou"),_T("pa"),_T("pai"),_T("pan"),_T("pang"),_T("pao"),_T("pei"),_T("pen"),_T("peng"),_T("pi"),_T("pian"),_T("piao"),_T("pie"),_T(
		"pin"),_T("ping"),_T("po"),_T("pu"),_T("qi"),_T("qia"),_T("qian"),_T("qiang"),_T("qiao"),_T("qie"),_T("qin"),_T("qing"),_T("qiong"),_T("qiu"),_T("qu"),_T("quan"),_T("que"),_T("qun"),_T(
		"ran"),_T("rang"),_T("rao"),_T("re"),_T("ren"),_T("reng"),_T("ri"),_T("rong"),_T("rou"),_T("ru"),_T("ruan"),_T("rui"),_T("run"),_T("ruo"),_T("sa"),_T("sai"),_T("san"),_T("sang"),_T(
		"sao"),_T("se"),_T("sen"),_T("seng"),_T("sha"),_T("shai"),_T("shan"),_T("shang"),_T("shao"),_T("she"),_T("shen"),_T("sheng"),_T("shi"),_T("shou"),_T("shu"),_T("shua"),_T(
		"shuai"),_T("shuan"),_T("shuang"),_T("shui"),_T("shun"),_T("shuo"),_T("si"),_T("song"),_T("sou"),_T("su"),_T("suan"),_T("sui"),_T("sun"),_T("suo"),_T("ta"),_T("tai"),_T(
		"tan"),_T("tang"),_T("tao"),_T("te"),_T("teng"),_T("ti"),_T("tian"),_T("tiao"),_T("tie"),_T("ting"),_T("tong"),_T("tou"),_T("tu"),_T("tuan"),_T("tui"),_T("tun"),_T("tuo"),_T(
		"wa"),_T("wai"),_T("wan"),_T("wang"),_T("wei"),_T("wen"),_T("weng"),_T("wo"),_T("wu"),_T("xi"),_T("xia"),_T("xian"),_T("xiang"),_T("xiao"),_T("xie"),_T("xin"),_T("xing"),_T(
		"xiong"),_T("xiu"),_T("xu"),_T("xuan"),_T("xue"),_T("xun"),_T("ya"),_T("yan"),_T("yang"),_T("yao"),_T("ye"),_T("yi"),_T("yin"),_T("ying"),_T("yo"),_T("yong"),_T("you"),_T(
		"yu"),_T("yuan"),_T("yue"),_T("yun"),_T("za"),_T("zai"),_T("zan"),_T("zang"),_T("zao"),_T("ze"),_T("zei"),_T("zen"),_T("zeng"),_T("zha"),_T("zhai"),_T("zhan"),_T("zhang"),_T(
		"zhao"),_T("zhe"),_T("zhen"),_T("zheng"),_T("zhi"),_T("zhong"),_T("zhou"),_T("zhu"),_T("zhua"),_T("zhuai"),_T("zhuan"),_T("zhuang"),_T("zhui"),_T("zhun"),_T("zhuo"),_T(
		"zi"),_T("zong"),_T("zou"),_T("zu"),_T("zuan"),_T("zui"),_T("zun"),_T("zuo")};

	int chrasc = 0;        

	TCHAR* pcReturnString = NULL;

	//unicode转换为ANSI
	int length = WideCharToMultiByte(CP_ACP, 0, chrstr, -1, 0, 0, 0, 0 ); 
	char* nowchar = new char[length+1];
	memset(nowchar,0,length+1);
	WideCharToMultiByte( CP_ACP, 0, chrstr, -1, nowchar,length, 0, 0 );

	//转换ANSI,字符部分不变，汉字转换成相应的拼音
	TCHAR *returnstr = new TCHAR[6*length +1];
	memset(returnstr,0,(6*length +1)*sizeof(TCHAR));

	int offset = 0; 
	for (int j = 0; j < length;) // 循环处理字节数组
	{
		if (nowchar[j] >= 0 && nowchar[j] < 128) // 非汉字处理
		{

			returnstr[offset] = nowchar[j];
			offset++;
			j++;
			continue;
		}

		// 汉字处理
		chrasc = nowchar[j] * 256 + nowchar[j+1]+256;

		if (chrasc > 0 && chrasc < 160)
		{
			returnstr[offset]= nowchar[j];
			offset++;
			j++;
		}
		else
		{
			for (int i = (sizeof(pyvalue)/sizeof(pyvalue[0]) - 1); i >= 0; i--)
			{
				if (pyvalue[i] <= chrasc)
				{
					_tcscpy(returnstr+offset ,pystr[i]);

					offset +=_tcslen(pystr[i]);  
					break;
				}
			}
			j +=2;
		}
	}
	if(_tcslen(returnstr)>0)
	{
		pcReturnString = new TCHAR[_tcslen(returnstr)+1];
		memset(pcReturnString,0,(_tcslen(returnstr)+1)*sizeof(TCHAR));
		_tcscpy(pcReturnString,returnstr);
	}
	delete []returnstr;
	delete []nowchar;
	delete []wszString;

	return pcReturnString;

}

/*
*    判断字符串内指定字符数是否为utf8
*    输入
*        const char *pszStr: 要检查的字符串
*        int nLength: 要检查的字符串的长度
*/
BOOL CCommonFun::IsUTF8(const char *pszStr, int nLength)
{
	DWORD nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节  
	UCHAR chr;  
	BOOL bAllAscii=TRUE; //如果全部都是ASCII, 说明不是UTF-8  
	for(int i=0; i<nLength; ++i)
	{  
		chr= *(pszStr+i);
		if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx  
			bAllAscii= FALSE;  
		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数  
		{  
			if(chr>=0x80)  
			{  
				if(chr>=0xFC&&chr<=0xFD)  
					nBytes=6;  
				else if(chr>=0xF8)  
					nBytes=5;  
				else if(chr>=0xF0)  
					nBytes=4;  
				else if(chr>=0xE0)  
					nBytes=3;  
				else if(chr>=0xC0)  
					nBytes=2;  
				else  
					return FALSE;  

				nBytes--;  
			}  
		}  
		else //多字节符的非首字节,应为 10xxxxxx  
		{  
			if( (chr&0xC0) != 0x80 )  
				return FALSE;  

			nBytes--;  
		}  
	}  
	if( nBytes > 0 ) //违返规则  
		return FALSE;  
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8  
		return FALSE;  

	return TRUE;  
}

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
		WriteFileLog(_T("AddUser fail,status=%u,dwError=%u\n"), status, dwError);
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
	WCHAR pwcUserName[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
#endif // UNICODE

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetUserDel(NULL, pwcUserName);
	if (status != NERR_Success /*&& status != NERR_UserNotFound*/)
	{
		WriteFileLog(_T("DeleteUser fail,status=%u\n"), status);
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
		WriteFileLog(_T("AddUserToGroup fail,status=%u\n"), status);
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
	MultiByteToWideChar(CP_GB2312, 0, szUserName.GetString(), -1, pwcUserName, szUserName.GetLength());
	MultiByteToWideChar(CP_GB2312, 0, szGroupName.GetString(), -1, pwcGroupName, szGroupName.GetLength());
#endif // UNICODE

	LOCALGROUP_MEMBERS_INFO_3 account; 
	account.lgrmi3_domainandname = pwcUserName;

	//设置权限
	// 	ModifyPrivilege(SE_MACHINE_ACCOUNT_NAME,TRUE);

	NET_API_STATUS status = NetLocalGroupDelMembers(NULL,pwcGroupName,3,(LPBYTE)&account,1);
	if (status != NERR_Success /*&& status!=ERROR_MEMBER_NOT_IN_ALIAS*/)
	{
		WriteFileLog(_T("DeleteUserFromGroup fail,status=%u\n"), status);
		return FALSE;
	}

	return TRUE;
}

//判断打印机是否存在
BOOL CCommonFun::IsPrinterExist(CString szPrinterName)
{
	HANDLE hPrinter = NULL;
	if(OpenPrinter(szPrinterName.GetBuffer(),&hPrinter,NULL))
	{
		ClosePrinter(hPrinter);
		return TRUE;
	}
	else
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_INVALID_PRINTER_NAME)
		{
			CCommonFun::WriteFileLog(_T("!!IsPrinterExist,OpenPrinter fail,dwError=%u,szPrinterName=%s")
				, dwError, szPrinterName);
		}
		return FALSE;
	}
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
			sDefaultPath += "\\xlog";
		}
		
		if (sDefaultPath.IsEmpty())
		{
			sDefaultPath = ".\\xlog";
		}

		g_szLogDir = sDefaultPath;
	}

	return g_szLogDir;
}

//Ping一个IP，并且返回成功和耗时时间（毫秒）
BOOL CCommonFun::Ping(CString szIP, DWORD& dwMS)
{
	BOOL bRet = FALSE;

	HANDLE hIcmpFile = NULL;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;
	dwMS = 5000;

	do 
	{
#ifndef UNICODE
		ipaddr = inet_addr(szIP.GetString());
#else
		CStringA szIPA = UnicodeToUTF8(szIP);
		ipaddr = inet_addr(szIPA.GetString());
#endif // !UNICODE

		if (ipaddr == INADDR_NONE) 
		{
			CCommonFun::WriteFileLog(_T("!!CCommonFun::Ping,1,usage: %s IP address"), szIP);
			break;
		}

		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE) 
		{
			CCommonFun::WriteFileLog(_T("!!CCommonFun::Ping,2,IcmpCreateFile fail. error: %ld"), GetLastError() );
			break;
		}    

		ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
		ReplyBuffer = (VOID*) malloc(ReplySize);
		if (ReplyBuffer == NULL) 
		{
			CCommonFun::WriteFileLog(_T("!!CCommonFun::Ping,3,Unable to allocate memory."));
			break;
		}    

		DWORD dwTickBegin = GetTickCount();
		dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
		if (dwRetVal != 0) 
		{
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
			struct in_addr ReplyAddr;
			ReplyAddr.S_un.S_addr = pEchoReply->Address;
			if (pEchoReply->Status == IP_SUCCESS)
			{
				dwMS = GetTickCount() - dwTickBegin;
				bRet = TRUE;
			}
			else
			{
				CString sIP(inet_ntoa( ReplyAddr ));
				theLog.Write(_T("CCommonFun::Ping,4,Received from %s, dwRetVal=%ld, Status=%ld, time=%ldms")
					, sIP
					, dwRetVal
					, pEchoReply->Status
					, pEchoReply->RoundTripTime);
			}
		}
		else 
		{
			theLog.Write(_T("!!CCommonFun::Ping,5,IcmpSendEcho failed. error: %ld, m_szIP=%s,dwRetVal=%d"), GetLastError(), szIP, dwRetVal);
			break;
		}
	} while (FALSE);

	if (hIcmpFile)
	{
		IcmpCloseHandle(hIcmpFile);
		hIcmpFile = NULL;
	}

	if (ReplyBuffer)
	{
		free(ReplyBuffer);
		ReplyBuffer = NULL;
	}

	return bRet;
}

CString CCommonFun::EncryptMD5ToStr(unsigned char* pSource, int nLen)
{
	CString outData = _T("");
	if (!pSource)
	{
		return _T("");
	}
	//CSP指针
	HCRYPTPROV hProv = 0;
	//MD5 HASH
	HCRYPTHASH hHash = 0;

	BYTE bHash[32];
	DWORD dwHashLen = 16; // The MD5 algorithm always returns 16 bytes.
	DWORD cbContent = nLen;
	BYTE* pbContent = (BYTE*)pSource;

	if (!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return _T("");
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
		return _T("");
	if (!CryptHashData(hHash, pbContent, cbContent, 0))
		return _T("");
	if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
		return _T("");

	CString tmp;
	for (unsigned int i = 0; i < dwHashLen; i += 4)
	{
		tmp.Format(_T("%02x%02x%02x%02x"), bHash[i], bHash[i + 1], bHash[i + 2], bHash[i + 3]);
		outData += tmp;
	}
	outData = _T("");
	const char EnBase64Tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	for (unsigned int i = 0; i < dwHashLen - 1; i += 3)
	{

		DWORD dwValue = bHash[i] * 256 * 256 + bHash[i + 1] * 256 + bHash[i + 2];
		int p4 = dwValue << 26 >> 26;
		int p3 = dwValue << 20 >> 26;
		int p2 = dwValue << 14 >> 26;
		int p1 = dwValue << 8 >> 26;
		tmp.Format(_T("%c%c%c%c"), EnBase64Tab[p1], EnBase64Tab[p2], EnBase64Tab[p3], EnBase64Tab[p4]);
		outData += tmp;
	}
	//处理最后一位
	{
		DWORD dwValue = bHash[15];
		int p4 = dwValue << 26 >> 26;
		int p3 = dwValue << 20 >> 26;
		int p2 = dwValue << 14 >> 26;
		int p1 = dwValue << 8 >> 26;
		tmp.Format(_T("%c%c%c%c"), EnBase64Tab[p1], EnBase64Tab[p2], EnBase64Tab[p3], EnBase64Tab[p4]);
		outData += tmp;
	}

	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);

	return outData;
}

std::string CCommonFun::EncryptMD5ToStr2(std::string sIn)
{
	Md5Encode oEncode;
	std::string sRet = oEncode.Encode(sIn);
	return sRet;
}

std::wstring CCommonFun::Shar1(std::wstring sIn)
{
	CSha1Ex sha1;
	std::wstring strReport;
	const size_t uAnsiLen = wcstombs(NULL, sIn.c_str(), 0) + 1;
	char* pszAnsi = new char[uAnsiLen + 1];
	wcstombs(pszAnsi, sIn.c_str(), uAnsiLen);

	sha1.Update((UINT_8*)&pszAnsi[0], strlen(&pszAnsi[0]));
	sha1.Final();
	sha1.ReportHashStl(strReport, CSha1Ex::REPORT_HEX_SHORT);
	delete[] pszAnsi;
	sha1.Reset();
	CStringW sTmp = strReport.c_str();
	return sTmp.MakeLower().GetString();
}

BOOL CCommonFun::IsWinSimplifiedChinese()
{
	LCID lcid = GetSystemDefaultLCID();
	if (lcid == 0x804)
	{
		return TRUE;
	}
	return FALSE;
}

CString CCommonFun::GetDateString()
{
	COleDateTime dt = COleDateTime::GetCurrentTime();
	return dt.Format(_T("%Y%m%d"));
}

BOOL CCommonFun::AtlEnableFileAccountPrivilege(CString strFileDir, CString strAccout)
{
	//设置目录的安全权限，增加everyone
	CDacl dacl;
	CSid sid;
	if (!sid.LoadAccount(strAccout))
	{
		theLog.Write(_T("!!CPrintStation::LoadAccount fail!!!"));
		return FALSE;
	}
	// 获取文件(夹)的DACL    
	if (!AtlGetDacl(strFileDir.GetBuffer(), SE_FILE_OBJECT, &dacl))
	{
		theLog.Write(_T("!!CPrintStation::AtlGetDacl fail!!!"));
		return FALSE;
	}
	// 在DACL中添加新的ACE项    
	dacl.AddAllowedAce(sid, GENERIC_ALL);
	// 设置文件(夹)的DACL   
	return AtlSetDacl(strFileDir.GetBuffer(), SE_FILE_OBJECT, dacl);
}

BOOL CCommonFun::AddNetShare502(CString sShareName, CString sShareDesc, CString sDirPath)
{
	USES_CONVERSION;
	SHARE_INFO_502   si502;

	NET_API_STATUS   nas;
	si502.shi502_netname = (LPTSTR)sShareName.GetBuffer();
	si502.shi502_type = STYPE_DISKTREE;
	si502.shi502_remark = NULL;
	si502.shi502_permissions = ACCESS_ALL;
	si502.shi502_max_uses = 5;
	si502.shi502_current_uses = 0;
	si502.shi502_path = (LPTSTR)sDirPath.GetBuffer();
	si502.shi502_passwd = NULL;
	si502.shi502_reserved = 0;
	si502.shi502_security_descriptor = NULL;
	nas = NetShareAdd(NULL,//   share   is   on   local   machine
		502,       //   info-level                     
		(LPBYTE)&si502,   //   info-buffer
		NULL  //   don't   bother   with   parm                    
	);
	theLog.Write(_T("!!CPrintStation::AddShareDir,nas=%d"), nas);
	return TRUE;
}

int CCommonFun::SplitString(const CString str, TCHAR *split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if (iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength() - iIndex - 1);
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);
	return strArray.GetSize();
}



