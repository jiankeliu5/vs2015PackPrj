#pragma once
#define MEM_MAP_NAME		_T("Global\\{C2F483C2-0457-44f5-86DD-CDF095EABBCC}")
#define MEM_MAP_SIZE		1024*1024*200
class CPrinterJobHelper
{
public:
	CPrinterJobHelper();
	~CPrinterJobHelper();
public:
	void RemoveJob(int nJobId);
	BOOL GetJobInfo(LPTSTR sPrinter,int nJobID,SubmitPrintJob& log);

	BOOL PauseThisJob(LPTSTR sPrinter,int nJobID);	//add by zfq,暂停这个任务

	BOOL GetSpoolFileName(LPTSTR sPrinter,int nJobId,CString &sPath);
	BOOL ZipSpoolFile(CString sPath);

	BOOL CopySpoolFile(CString sPrinter,int nJobId,CString sPath,BOOL bZip = FALSE);
//	BOOL GetTiffFile(int nJobId,CString& sZipPath,int& nSize,int& nPageCount);	//delete by zfq,2013.01.04,解决单页多份计数问题
	BOOL GetTiffFile(int nJobId,CString& sZipPath,int& nSize,int& nPageCount
		, DWORD dwCountOfCopies = 0
		, DWORD dwPagePerCopy = 0
		, BOOL bTxt = FALSE	//是否为TXT文档
		);	//add by zfq,2013.01.04,解决单页多份计数问题


};
class CXabPrinterMonitor
{
public:
	CXabPrinterMonitor(void);
	~CXabPrinterMonitor(void);

public:
	void EnumeratePrinters( void );
	void StartMonitor();
	void StopMonitor();
	void SetCallback(ICltLogCallback* pCall)
	{
		m_pCallBack = pCall;
	}
	void OnJobCreated(int nPrtIdx,DWORD dwId);
	static DWORD WINAPI ThreadFunc(void* pVoid);
private:
	void SetWorkDir();
	void Enable(BOOL bEnable = TRUE);
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	HANDLE m_hEventStopRequested;
	ICltLogCallback* m_pCallBack;
	CStringArray m_aryPrinter;
	CArray<HANDLE,HANDLE> m_aryPrinterHandle;
	CFileMap	m_oFileMap;
	CString		m_szPrinterName;
};
