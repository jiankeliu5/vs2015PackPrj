// FileLog.h: interface for the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOG_H__B55FD62E_C02C_435E_8E22_681955294BC1__INCLUDED_)
#define AFX_FILELOG_H__B55FD62E_C02C_435E_8E22_681955294BC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------
//日志

#define Enable_Write_Log_Mutex	1	//0-禁用写日志互斥，1-启用写日志互斥

class _AA_DLL_EXPORT_ CFileLog //: public CObject
{
public:

	void WriteA(LPCSTR lpszFormat, ...);
	void WriteA(CStringA sLog);

	void Write(BYTE* pData,int nLen,CString sPath);
	void Read(BYTE*& pData,int& nLen,CString sPath);
	
	void SetLogName(CString sName);
	static BOOL IsDebug();
	
//	void Write(LPCSTR lpszFormat, ...);
	void Write(LPCTSTR lpszFormat, ...);
	void Write(CString sLog);
	void WriteW(LPWSTR sLog);

	static void EnableLog(BOOL b = FALSE);
	CFileLog();
	virtual ~CFileLog();

	void WriteEx(LPCTSTR lpszFormat, ...);
	void Writeld(LPCTSTR lpszFormat, ...);
	void Writeli(LPCTSTR lpszFormat, ...);
	void Writelw(LPCTSTR lpszFormat, ...);
	void Writele(LPCTSTR lpszFormat, ...);

protected:
	void WriteEx(int nlevel, CString slog);
	CString LevelToStr(int nLevel);
	void InitCritSection();
	void ReleaseCritSection();

	static void DbgPrint(LPCTSTR lpszFormat, ...);

private:
	CString m_sLogName;
	//BOOL	m_bIsDebug;

#if Enable_Write_Log_Mutex
	CRITICAL_SECTION m_logCrit, *m_pLogCrit;
#endif
};

#endif // !defined(AFX_FILELOG_H__B55FD62E_C02C_435E_8E22_681955294BC1__INCLUDED_)
