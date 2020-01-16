// ScanMonitor.h: interface for the CScanMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANMONITOR_H__9B2360E0_7B0D_479A_9792_E1BF869B5C6B__INCLUDED_)
#define AFX_SCANMONITOR_H__9B2360E0_7B0D_479A_9792_E1BF869B5C6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IScanChargeInterface
{
public:
	virtual void OnOneScanCmplt(void*){ASSERT(0);}
	virtual void OnScanErr(int ){ASSERT(0);}
};

class _AA_DLL_EXPORT_ CScanInfo
{
public:
	CScanInfo()
	{
		m_nSize = 0;
		m_nColor = 1;
		m_nUnit = 1024;
		m_nPageCount = 0;
		m_fPrice = 0;
		m_nUserId = 0;
		m_bIsLastFile = FALSE;
		m_sPath = _T("");
		m_sZipFilePath = _T("");
		m_sFileName = _T("");
		m_sOrgFileName = _T("");
		m_sDir = _T("");
	}
	CString m_sPath;
	CString m_sZipFilePath;
	CString m_sFileName;
	CString m_sOrgFileName;
	CString m_sDir;

	int m_nUserId;
	int m_nSize;
	int m_nColor;
	int m_nPageCount;
	double m_dt;
	double m_fPrice;
	int m_nUnit;
	BOOL m_bIsLastFile;


	BOOL Attach(CString& sPath);
protected:
	BOOL CompressTif();
};

class _AA_DLL_EXPORT_ CScanMonitor  
{
public:
	void Stop();
	BOOL Start(CString sDir);
	void SetCallback(IScanChargeInterface*);
	CScanMonitor();
	virtual ~CScanMonitor();
	BOOL IsTiff(CString& sPath);
	void SetScanData(CString& s);
	void Shutdown();
	BOOL IsActive();
	CString m_sDir;
private:
	HANDLE m_hShutdown;
	HANDLE	m_hThread;
	HANDLE	m_hThreadStarted;
	long	m_lActive;
	IScanChargeInterface* m_pCallback;
	CStringArray m_aryData;

	void ReadChanges2(CString sDir);
// 	BOOL IsActive();
	void DumpFNIBuffer(FILE_NOTIFY_INFORMATION* pInfo);
	void WriteToMemFile(FILE_NOTIFY_INFORMATION* pInfo);
	
	static DWORD WINAPI ThreadFn(void* pVoid);
};

#endif // !defined(AFX_SCANMONITOR_H__9B2360E0_7B0D_479A_9792_E1BF869B5C6B__INCLUDED_)
