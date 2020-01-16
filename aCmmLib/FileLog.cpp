// FileLog.cpp: implementation of the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL m_bIsDebug = FALSE;

CFileLog::CFileLog()
{
	m_sLogName = _T("");
#if Enable_Write_Log_Mutex
	m_pLogCrit = NULL;
	InitCritSection();	
#endif
}

CFileLog::~CFileLog()
{
#if Enable_Write_Log_Mutex
	ReleaseCritSection();
#endif
}

void CFileLog::EnableLog(BOOL b)
{	
	m_bIsDebug = b;
}

void CFileLog::Write(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	Write(sTmp);
}


void CFileLog::Write(CString sLog)
{
#if 0	//TRACE字符串中有中文会报错
	if( sLog.GetLength() > 250 )
		TRACE(sLog.Left(250));
	else
		TRACE(sLog);
	TRACE(_T("\n"));
#endif
	
	if (!IsDebug())
	{
		return;
	}
	
	CString sSysDir = CCommonFun::GetLogDir();	
	CFileFind fd;
	if( !fd.FindFile(sSysDir) )
	{
		if (!::CreateDirectory(sSysDir, NULL))
		{
			fd.Close();
			return;
		}
			
	}
	fd.Close();
	
	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));
	
	CString sFileName;
	sFileName.Format(_T("%s\\%s_%s.Log"),sSysDir,m_sLogName,sDate);
	
#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		EnterCriticalSection(m_pLogCrit);
#endif

#ifdef UNICODE
	BOOL bFileExsit = PathFileExists(sFileName);
#endif // UNICODE

	CFile fileLog;
	if(fileLog.Open(sFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
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
	
#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		LeaveCriticalSection(m_pLogCrit);
#endif
}


void CFileLog::WriteA(LPCSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CStringA sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteA(sTmp);
}

void CFileLog::WriteA(CStringA sLog)
{
#if 0	//TRACE字符串中有中文会报错
	if (sLog.GetLength() > 250)
		TRACE(sLog.Left(250));
	else
		TRACE(sLog);
	TRACE(_T("\n"));
#endif

	if (!IsDebug())
	{
		return;
	}

	CString sSysDir = CCommonFun::GetLogDir();
	CFileFind fd;
	if (!fd.FindFile(sSysDir))
	{
		if (!::CreateDirectory(sSysDir, NULL))
		{
			fd.Close();
			return;
		}
			
	}
	fd.Close();

	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));

	CString sFileName;
	sFileName.Format(_T("%s\\%s_%s.Log"), sSysDir, m_sLogName, sDate);

#if Enable_Write_Log_Mutex
	if (m_pLogCrit)
		EnterCriticalSection(m_pLogCrit);
#endif

#ifdef UNICODE
	BOOL bFileExsit = PathFileExists(sFileName);
#endif // UNICODE

	CFile fileLog;
	if (fileLog.Open(sFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
#ifdef UNICODE
		if (!bFileExsit)
		{
			// 新创建的日志文件，则写入Unicode头
			BYTE chUnicodeHead[2] = { 0xff, 0xfe }; // Unicode头
			fileLog.Write(chUnicodeHead, 2);
			fileLog.Flush();
		}
#endif // UNICODE

		CString sTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));
		CString sTmp;
		sTmp.Format(_T("%s,	 %s\r\n"), sTime, CCommonFun::MultiByteToUnicode(sLog));
		fileLog.SeekToEnd();
		fileLog.Write(sTmp, sTmp.GetLength() * sizeof(TCHAR));
		fileLog.Close();
	}

#if Enable_Write_Log_Mutex
	if (m_pLogCrit)
		LeaveCriticalSection(m_pLogCrit);
#endif
}

void CFileLog::WriteW(LPWSTR sLog)
{
	if (!IsDebug())
	{
		return;
	}

	CString sSysDir = CCommonFun::GetLogDir();	
	CFileFind fd;
	if( !fd.FindFile(sSysDir) )
	{
		if (!::CreateDirectory(sSysDir, NULL))
		{
			fd.Close();
			return;
		}
			
	}
	fd.Close();

	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));

	CString sFileName;
	sFileName.Format(_T("%s\\%s_%s.Log"),sSysDir,m_sLogName,sDate);

#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		EnterCriticalSection(m_pLogCrit);
#endif

	CFile fileLog;
	if(fileLog.Open(sFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		CString sTime = time.Format( _T("%Y-%m-%d %H:%M:%S") );
		CString sTmp;
		sTmp.Format(_T("%s,	 "),sTime);
		fileLog.SeekToEnd();

		fileLog.Write(sTmp,sTmp.GetLength());
		fileLog.Write(sLog, wcslen(sLog));
		fileLog.Write(_T("\r\n"), 2);

		fileLog.Close();
	}

#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		LeaveCriticalSection(m_pLogCrit);
#endif
}

BOOL CFileLog::IsDebug()
{
	return m_bIsDebug;
}

void CFileLog::SetLogName(CString sName)
{
	ASSERT(!sName.IsEmpty());
	m_sLogName = sName;
}

void CFileLog::Read(BYTE*& pData, int& nLen,CString sLogName)
{
	CFile fileLog;
	if(fileLog.Open(sLogName,CFile::modeRead))
	{
		if (!pData && nLen == 0)
		{
			nLen = fileLog.GetLength();
			pData = new BYTE[nLen];
		}

		fileLog.Read(pData,nLen);
		fileLog.Close();
	}
}

void CFileLog::Write(BYTE *pData, int nLen,CString sLogName)
{
	if (!IsDebug())
	{
		return;
	}
		
	CFile fileLog;
	if(fileLog.Open(sLogName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		fileLog.SeekToEnd();
		fileLog.Write(pData,nLen);
		fileLog.Close();
	}
}

void CFileLog::DbgPrint(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	//TRACE(_T("%s\n"),sTmp);
	OutputDebugString(sTmp);
}

void CFileLog::Writeld(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteEx(1, sTmp);
}

void CFileLog::Writeli(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteEx(2, sTmp);
}

void CFileLog::Writelw(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteEx(4, sTmp);
}

void CFileLog::Writele(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteEx(8, sTmp);
}

void CFileLog::WriteEx(LPCTSTR lpszFormat, ...)
{
	if (!IsDebug())
	{
		return;
	}

	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteEx(2, sTmp);
}

void CFileLog::WriteEx(int nlevel, CString slog)
{
	if (!IsDebug())
	{
		return;
	}

	SYSTEMTIME curTime;
	GetLocalTime(&curTime);
	CString shead;
	shead.Format(_T("[%s][%03x.%03x][%02d:%02d:%02d.%03d] "), LevelToStr(nlevel), 
		GetCurrentProcessId(), GetCurrentThreadId(),
		curTime.wHour,curTime.wMinute,curTime.wSecond,curTime.wMilliseconds);
	CString sline = shead+slog;

	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));

	CFile file;
	CString sfile;
	sfile.Format(_T("%s\\%s_%s.Log"), CCommonFun::GetLogDir(), m_sLogName.IsEmpty()?_T("UNKNOWN"):m_sLogName, sDate);

#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		EnterCriticalSection(m_pLogCrit);
#endif

	if(file.Open(sfile, CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
	{
		file.SeekToEnd();
		file.Write(sline,sline.GetLength());
		file.Write(_T("\r\n"),_tcslen(_T("\r\n"))*sizeof(TCHAR));
		file.Close();
	}

#if Enable_Write_Log_Mutex
	if(m_pLogCrit)
		LeaveCriticalSection(m_pLogCrit);
#endif
}

CString CFileLog::LevelToStr(int nLevel)
{
	if(nLevel == 1)
		return _T("d");
	else if(nLevel == 2)
		return _T("i");
	else if(nLevel == 4)
		return _T("w");
	else if(nLevel == 8)
		return _T("e");
	else
		return _T("u");
}

#if Enable_Write_Log_Mutex
void CFileLog::InitCritSection()
{
	if(!m_pLogCrit)
	{
		InitializeCriticalSection(&m_logCrit);
		m_pLogCrit = &m_logCrit;
	}
}

void CFileLog::ReleaseCritSection()
{
	if(m_pLogCrit)
	{
		DeleteCriticalSection(m_pLogCrit);
		m_pLogCrit = NULL;
	}
}
#endif

