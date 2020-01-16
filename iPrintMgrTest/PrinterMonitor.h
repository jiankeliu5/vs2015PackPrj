
#pragma once
#include <Winspool.h>
//#include "../PrintHookProc/PrintHookexport.h"
//#define DEF_UseThis
#ifdef DEF_UseThis	//========================================================================================

#define XAB_PRINTER_KEY_PATH	_T("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\XabVPrinter")
#define IST_PRINTER_NAME		_T("XabVPrinter")

#define SEC_PRINTER_KEY_PATH	_T("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\SecVPrinter")
#define SEC_PRINTER_NAME		_T("SecVPrinter")



class _AA_DLL_EXPORT_ CPrinterJobHelper
{
public:
	CPrinterJobHelper();
	~CPrinterJobHelper();
public:
	static void KeepJob(HANDLE hPrint,BOOL bEnable,BOOL& bKeep);
	void RemoveJob(int nJobId);
	void RemoveSecJob(int nJobId);
	BOOL PauseJob(CString sName,int nJobId);
	BOOL ResumeJob(CString sName,int nJobId);
	BOOL RemoveJob(CString sName,int nJobId);

	BOOL GetJobInfo(LPTSTR sPrinter,int nJobID,OutGoingLog& log);
	BOOL GetJobInfo(LPTSTR sPrinter,int nJobID,SubmitPrintJob& log);
	BOOL GetJobInfo(LPTSTR sPrinter,int nJobID,PrintLog& log);
	BOOL GetJobInfo(LPTSTR sPrinter,int nJobID,JOB_INFO_2*& pVal);
	BOOL GetSpoolFileName(LPTSTR sPrinter,int nJobId,CString &sPath);
	BOOL ZipSpoolFile(CString sPath);
	BOOL SpoolToEmf(const char* pFile,const char* pOut);
	BOOL EmfToBmp(CString sEmf,CString sBmp);
	BOOL EmfToJpg(CString sPath);
	BOOL CopySpoolFile(CString sPrinter,int nJobId,CString sPath,BOOL bZip = FALSE);
	BOOL GetTiffFile(LPTSTR sPrinter,int nJobId,CString& sZipPath,int& nSize,int& nPageCount);
	

};

class _AA_DLL_EXPORT_ CJobInfo
{
public:
	CJobInfo( const int nJobId );

	int GetJobId( void ) const;
	LPCTSTR GetUser( void ) const;
	LPCTSTR GetMachine( void ) const;
	LPCTSTR GetPort( void ) const;
	LPCTSTR GetDocument( void ) const;
	SYSTEMTIME GetTimeSubmitted( void ) const;
	int GetTotalPages( void ) const;
	int GetPagesPrinted( void ) const;
	int GetTotalBytes( void ) const;
	int GetBytesPrinted( void ) const;
	LPCTSTR GetStatus( void ) const;
	void UpdateInfo( const PPRINTER_NOTIFY_INFO_DATA pNotifyData );
private:
	static CMap<int, int, LPCTSTR, LPCTSTR> m_mapJobStatus;    

	int m_nJobId;
	CString m_strUser;
	CString m_strMachine;
	CString m_strPort;
	CString m_strDocument;
	CString m_strSpoolPath;
	SYSTEMTIME m_timeSubmitted;
	int m_nTotalPages;
	int m_nPagesPrinted;
	int m_nTotalBytes;
	int m_nBytesPrinted;
	int m_nStatus;
};

class _AA_DLL_EXPORT_ CJobInfoMap : public CMap<int, int, CJobInfo*, CJobInfo*>
{
public:
	void Cleanup( void )
	{
		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			int nKey = 0;
			CJobInfo* pValue = NULL;
			GetNextAssoc(pos, nKey, pValue);
			if(pValue)
			{
				delete pValue;
			}
		}
		RemoveAll();
	}

	static CJobInfoMap& GetInstance()
	{
		static CJobInfoMap themap;
		return themap;
	}

};

#define PRT_KEEP_JOB 1
class _AA_DLL_EXPORT_ CPrinterMonitor
{
public:
	CPrinterMonitor();
	~CPrinterMonitor();
public:
	void EnumeratePrinters( void );
	void StartMonitor(CString sPrinter = _T(""));
	void StopMonitor();
	
	void SetCallback(ICltLogCallback* pCall,BOOL bOpStarted = FALSE)
	{
		m_pCallBack = pCall;
		m_bExternDo = bOpStarted;
	}
	void OnJobCreated(CJobInfo* pJob);
	void OnJobCreating(int nPrtIdx,DWORD dwId);
	void OnJobCreated(int nPrtIdx,DWORD dwId);
	static DWORD WINAPI ThreadFunc(void* pVoid);
	void SetPolicy(void** pAry,int nCount);
	BOOL LogActive()
	{
		return m_bLogging;
	}
	BOOL OnFilter(CString& sPrinter,JOB_INFO_2* pJob,int& nMode,CString& sTips);
	void StopLog();
	void StartLog();
	void StartFilter();
	void StopFilter();
private:
	void Enable(BOOL bEnable = TRUE);
	void Release();
	
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	HANDLE m_hEventStopRequested;
	HMODULE m_hPrintHookDll; 
	ICltLogCallback* m_pCallBack;
	CStringArray m_aryPrinter;
	CArray<HANDLE,HANDLE> m_aryPrinterHandle;
	CArray<BOOL,BOOL> m_aryKeepJob;
	BOOL m_bLogging;
	BOOL m_bFilter;
	BOOL m_bXabPrinterAcitve;
	BOOL m_bExternDo;

	CArray<CPrinterFilterNode*,CPrinterFilterNode*> m_oFltAry;
};

#endif //#if 0	//========================================================================================

