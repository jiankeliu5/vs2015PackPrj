#include "StdAfx.h"
#include "XabPrinterMonitor.h"
#include <Winspool.h>
#include <atlsecurity.h>

#define XAB_PRINTER_KEY_PATH	_T("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\XabVPrinter")
#define XAB_PRINTER_REGISTY_KEY _T("SYSTEM\\CurrentControlSet\\Control\\Print\\Printers\\XabVPrinter")
#define XAB_JOB_DIR				_T("JobDir")


#define IST_PRINTER_DRIVER		_T("XabVPrinterDriver")
#define IST_PRINTER_PORT		_T("XabVPrinterPort")
#define IST_PRINTER_DATATYPE	_T("NT EMF 1.008")
#define IST_PRINTER_PROC		_T("XabVPrinterProc")
#define IST_PRINTER_PROC_DLL	_T("XabVPrinterProc.dll")
//////////////////////////////////////////////////////////////////////////
//
CPrinterJobHelper::CPrinterJobHelper()
{

}

CPrinterJobHelper::~CPrinterJobHelper()
{

}

//add by zfq,暂停这个任务
BOOL CPrinterJobHelper::PauseThisJob(LPTSTR sPrinter,int nJobID)
{
	HANDLE hPrinter = NULL;
	BYTE *pJobData = NULL;
	JOB_INFO_2* pJobInfo = NULL;
	DWORD cbNeed = 0;
	BOOL bRet = FALSE;
	DWORD dwState = 0;

	if(!OpenPrinter(sPrinter,&hPrinter,NULL))
	{
		theLog.Write("!!CPrinterJobHelper::PauseThisJob,1,OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
		goto ERR1;
	}

	if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
	{//看是否能正常打开该JobID的作业
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			theLog.Write("!!CPrinterJobHelper::PauseThisJob,2,GetJob1 error err：%d",GetLastError());
			goto ERR2;
		}
	}	

	//该JobID的作业可以打开，暂停它
	if(!SetJob(hPrinter, nJobID, 0, 0, JOB_CONTROL_PAUSE))
	{
		theLog.Write("!!CPrinterJobHelper::PauseThisJob,3,SetJob fail,nJobID=%d,err=%d", nJobID, GetLastError());
		goto ERR2;
	}

	//======================================add by zfq,2013.01.30,begin
	pJobData = new BYTE[cbNeed];
	if(!pJobData)
	{
		theLog.Write("!!CPrinterJobHelper::GetJobInfo,4,new BYTE[%d] error err：%d", cbNeed, GetLastError());
		goto ERR2;
	}
	memset(pJobData,0,cbNeed);

	if(!GetJob(hPrinter, nJobID, 2, pJobData, cbNeed, &cbNeed))//JOB_STATUS_SPOOLING
	{
		theLog.Write("!!CPrinterJobHelper::GetJobInfo,5,GetJob error: %d",GetLastError());
		goto ERR3;
	}	
	pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

	theLog.Write("##CPrinterJobHelper::GetJobInfo,10,pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
		" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
		" dmColor:%d  dmDuplex:%d orig = %d",
		pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
		pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
		pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
		pJobInfo->pDevMode->dmColor,
		pJobInfo->pDevMode->dmDuplex,
		pJobInfo->pDevMode->dmOrientation 
		);
	delete []pJobData;
	pJobData = NULL;
	//======================================add by zfq,2013.01.30,end

	if(hPrinter)
	{
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
	return TRUE;
ERR3:
	if(pJobData)
	{
		delete []pJobData;
		pJobData = NULL;
	}
ERR2:
	if(hPrinter)
	{
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
ERR1:
	return FALSE;
}


//获取打印信息
BOOL CPrinterJobHelper::GetJobInfo(LPTSTR sPrinter,int nJobID,SubmitPrintJob& log)
{
	DWORD cbNeed = 0;
	HANDLE hPrinter = NULL;
	BYTE *pJobData = NULL;
	JOB_INFO_2* pJobInfo = NULL;
	BOOL bRet = FALSE;
	do 
	{
		if(!OpenPrinter(sPrinter,&hPrinter,NULL))
		{
			theLog.Write("!!CPrinterJobHelper::GetJobInfo,1,OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
			break;
		}
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				ClosePrinter(hPrinter);
				theLog.Write("!!CPrinterJobHelper::GetJobInfo,2,GetJob1 error err：%d",GetLastError());
				break;
			}
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
		{
			theLog.Write("!!CPrinterJobHelper::GetJobInfo,3,new BYTE[%d] error err：%d", cbNeed, GetLastError());
			break;
		}
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			ClosePrinter(hPrinter);
			theLog.Write("!!CPrinterJobHelper::GetJobInfo,4,GetJob2 error: %d",GetLastError());
			break;
		}	
		pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

		theLog.Write("##CPrinterJobHelper::GetJobInfo,10,pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation 
			);

#if 0
		{//测试代码
			DEVMODE stDevMode;
			::ZeroMemory(&stDevMode, sizeof(stDevMode));
		}
#endif

		//获取文件
		CString sZipPath ;
		int nJobSize = 0;
		int nPageCount = 0;
#if 0	//delete by zfq,2013.01.04
		if(GetTiffFile(nJobID,sZipPath,nJobSize,nPageCount))	
#else	//add by zfq,2013.01.04,解决TXT单页多份计数问题
		DWORD dwPagePerCopy = pJobInfo->TotalPages;
		DWORD dwCountOfCopies = pJobInfo->pDevMode->dmCopies;
		CString szDocName = pJobInfo->pDocument;
		CString szExtName = CCommonFun::GetFileExt(szDocName);
		BOOL bTxt = (0 <= szExtName.Find("txt")) ? TRUE : FALSE;

		theLog.Write("##CPrinterJobHelper::GetJobInfo,11,log.total_page=%d,log.copies=%d,szExtName=%s"
			, log.total_page(), log.copies(), szExtName);

		if(GetTiffFile(nJobID,sZipPath,nJobSize,nPageCount, dwCountOfCopies, dwPagePerCopy, bTxt))	//add by zfq,2013.01.04
#endif
		{
			theLog.Write("zipPath %s",sZipPath);
			CFileStatus st;
			CFile::GetStatus(sZipPath,st);
			//SubmitPrintJob log;
			log.set_win_user(pJobInfo->pUserName);
			log.set_equip_host(pJobInfo->pMachineName);
			//
			log.set_color(pJobInfo->pDevMode->dmColor);
			log.set_copies(pJobInfo->pDevMode->dmCopies);
			log.set_doc_name(pJobInfo->pDocument);
			//log.set_doc_size();
			log.set_job_id(nJobID);
			log.set_zip_job_size(st.m_size);
			log.set_job_path(sZipPath);
			log.set_win_user(pJobInfo->pUserName);
#if 0
			//如果打印机《文档高级功能》关闭，这个值是得不到的
			log.set_doc_size(pJobInfo->Size);
			log.set_total_page(pJobInfo->TotalPages);
#else
			log.set_doc_size(nJobSize);
			log.set_total_page(nPageCount);
#endif
			SYSTEMTIME lst;
			SystemTimeToTzSpecificLocalTime(0,&pJobInfo->Submitted,&lst);
			COleDateTime dt(lst);
			log.set_submit_time(dt.m_dt);
			theLog.Write("submittime :(%d-%d-%d %d:%d:%d)",lst.wYear,lst.wMonth,lst.wDay,lst.wHour,lst.wMinute,lst.wSecond);

			DMPAPER_A3;
			log.set_paper_size(pJobInfo->pDevMode->dmPaperSize);
			log.set_page_type(CCommonFun::GetPagerSize(pJobInfo->pDevMode->dmPaperSize));
			/*
			if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A3)
				log.set_page_type("A3");
			else if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A4)
				log.set_page_type("A4");
			else
				log.set_page_type("A4");
			*/

			/*
			if(pJobInfo->pDevMode->dmDuplex != DMDUP_SIMPLEX )
			{
				log.set_double_print(pJobInfo->pDevMode->dmDuplex);
			}
			else
			{
				log.set_double_print(pJobInfo->pDevMode->dmDuplex);
			}
			*/
			log.set_double_print(pJobInfo->pDevMode->dmDuplex);
			log.set_orientation( pJobInfo->pDevMode->dmOrientation);
			//删除作业
			RemoveJob(nJobID);

			bRet = TRUE;
		}

		theLog.Write("##CPrinterJobHelper::GetJobInfo,12,log.total_page=%d,log.copies=%d,bRet=%d", log.total_page(), log.copies(), bRet);

		theLog.Write("pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d,bRet=%d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation 
			, bRet);

	} while (0);




	if(hPrinter)
	{
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
	if(pJobData)
	{
		delete pJobData;
		pJobData = NULL;
	}
	return bRet;
}


//获取打印队列中的文档路径
BOOL CPrinterJobHelper::GetSpoolFileName(LPTSTR sPrinter,int nJobId,CString &sPath)
{
	HANDLE            hPrinter = NULL;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_READ;

	BOOL bRet = FALSE;
	LPBYTE  pSpoolDirectory = NULL;
	do 
	{
		if(!OpenPrinter(sPrinter,&hPrinter,&pd ) )
		{
			int nError = GetLastError();
			break;
		}
		DWORD cbNeeded = 0;   
		DWORD dwType = REG_SZ;           // data type
		GetPrinterData(
			hPrinter,                         // handle to printer or print server
			SPLREG_DEFAULT_SPOOL_DIRECTORY,
			&dwType,                         // data type
			NULL,                            // configuration data buffer
			0,                               // size of configuration data buffer
			&cbNeeded                        // bytes received or required
			);
		if(cbNeeded <= 0)
		{
			break;
		}

		pSpoolDirectory = (LPBYTE)new BYTE[cbNeeded]; 
		if(!pSpoolDirectory)
		{
			break;
		}

		GetPrinterData(
			hPrinter,                        // handle to printer or print server
			SPLREG_DEFAULT_SPOOL_DIRECTORY,
			&dwType,                        // data type
			pSpoolDirectory,                // configuration data buffer
			cbNeeded,                       // size of configuration data buffer
			&cbNeeded                       // bytes received or required
			);

		sPath.Format("%s\\%05d.SPL",pSpoolDirectory,nJobId);
		bRet = TRUE;

	} while (0);

	if(hPrinter)
	{
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
	if(pSpoolDirectory)
	{
		delete[] pSpoolDirectory;
		pSpoolDirectory = NULL;
	}
	return bRet;
}
BOOL CPrinterJobHelper::CopySpoolFile(CString sPrinter,int nJobId,CString sPath,BOOL bZip)
{
	char requestString[256] = {0};
	sprintf(requestString, "%s, Job %i", sPrinter, nJobId);
	HANDLE hPrinter = 0;
	if(!OpenPrinter(requestString, &hPrinter, NULL))
	{
		return FALSE;
	}

	DWORD bytesRead = 0;
	DWORD totalRead = 0;
	BYTE buf[2048] = {0};
	DWORD dwRead = 0;
	CFile file;
	if(!file.Open(sPath,CFile::modeCreate | CFile::modeWrite))
	{
		ClosePrinter(hPrinter);
		return FALSE;
	}
	do
	{
		if(ReadPrinter(hPrinter, buf, 2048, &dwRead) && dwRead > 0)
		{
			file.Write(buf,dwRead);
		}
		else
		{
			break;
		}
	}while(1);
	ClosePrinter(hPrinter);
	file.Close();
	return TRUE;
}

#if 0 //delete by zfq,2013.01.04
BOOL CPrinterJobHelper::GetTiffFile(int nJobId,CString& sZipPath,int& nSize,int& nPageCount)
#else //add by zfq,2013.01.04
BOOL CPrinterJobHelper::GetTiffFile(int nJobId,CString& sZipPath,int& nSize,int& nPageCount
									, DWORD dwCountOfCopies
									, DWORD dwPagePerCopy
									, BOOL bTxt)
#endif
{
	CString sPath;
	CString sDir;
	CRegistry2 reg(2,TRUE);
	if(reg.Open(XAB_PRINTER_KEY_PATH))
	{
		reg.Read(_T("JobDir"),sDir);
	}
	if (sDir.IsEmpty())
	{
		sDir = _T("c:\\Xab");
	}
	sPath.Format(_T("%s\\job_%06d.tif"),sDir,nJobId);
	CString sDestPath;
	//	sDestPath.Format(_T("%sPrintJob\\job_%06d.tif"),CCommonFun::GetDefaultPath(),nJobId);

	sDestPath.Format(_T("%s%s\\%s.tif"),CCommonFun::GetDefaultPath(),PRT_CLIENT_JOB_DIR,CCommonFun::NewGuid());
	CCommonFun::ValidatePath(sDestPath);
	theLog.Write("tiff = %s,dest = %s",sPath,sDestPath);
	if(!PathFileExists(sPath))
	{
		theLog.Write("!!CPrinterJobHelper::GetTiffFile %s not exist",sPath);
	}

	theLog.Write("##CPrinterJobHelper::GetTiffFile,10,sPath=%s,dwCountOfCopies=%d,dwPagePerCopy=%d,bTxt=%d"
		, sPath, dwCountOfCopies, dwPagePerCopy, bTxt);

	if(CopyFile(sPath,sDestPath,FALSE) && PathFileExists(sDestPath))
	{
		//----------------------------
		//获取文档的大小
		CFileStatus st;
		CFile::GetStatus(sDestPath,st);
		nSize = st.m_size;

		//---------------------------
		//获取文档的页数
		CStringW wsPath(sPath);
		Image image(wsPath);
		UINT count = 0;
		count = image.GetFrameDimensionsCount();
		TRACE("The number of dimensions is %d.\n", count);
		GUID* pDimensionIDs = (GUID*)malloc(sizeof(GUID)*count);
		UINT frameCount = 0;
		// Get the list of frame dimensions from the Image object.
		image.GetFrameDimensionsList(pDimensionIDs, count);

		theLog.Write("##CPrinterJobHelper::GetTiffFile,20,count=%d", count);

#if 0 //delete by zfq,2013.01.04
		for (int n = 0 ; n < count; n ++)
		{
			// Display the GUID of the first (and only) frame dimension.
			WCHAR strGuid[39];
			StringFromGUID2(pDimensionIDs[n], strGuid, 39);
			TRACE("The first (and only) dimension ID is %s.\n", strGuid);

			// Get the number of frames in the first dimension.
			frameCount += image.GetFrameCount(&pDimensionIDs[n]);
		}
#else //add by zfq,2013.01.04
		if(1 == dwPagePerCopy && bTxt)
		{//当是TXT文档时,每份一页,总页数就直接等于份数
			frameCount = dwCountOfCopies;
		}
		else
		{
			for (int n = 0 ; n < count; n ++)
			{
				// Display the GUID of the first (and only) frame dimension.
				WCHAR strGuid[39];
				StringFromGUID2(pDimensionIDs[n], strGuid, 39);
				TRACE("The first (and only) dimension ID is %s.\n", strGuid);

				// Get the number of frames in the first dimension.
				frameCount += image.GetFrameCount(&pDimensionIDs[n]);
			}//for
		}//else
#endif
		nPageCount = frameCount;
		theLog.Write("##CPrinterJobHelper::GetTiffFile,23,nPageCount=%d,frameCount=%d,dwCountOfCopies=%d,dwPagePerCopy=%d", nPageCount, frameCount, dwCountOfCopies, dwPagePerCopy);
		//--------------------------------

		//压缩
		char* pPath = sDestPath.GetBuffer();
		file_compress(pPath,-1);
		sDestPath.ReleaseBuffer();
		sZipPath.Format("%s.gz",sDestPath);

		return TRUE;
	}
	return FALSE;
}

void CPrinterJobHelper::RemoveJob(int nJobId)
{
	JOB_INFO_2			*pJobStorage = NULL;
	PRINTER_INFO_2		*pPrinterInfo = NULL;

	DWORD	cByteNeeded, nReturned=0, cByteUsed, i;

	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	CString szPrtName = IST_PRINTER_NAME;
	if(OpenPrinter(szPrtName.GetBuffer(),&hPrint,&pd))
	{
		szPrtName.ReleaseBuffer();
		CString sPrinterInfo(""), sTask;;
		if (!GetPrinter(hPrint, 2, NULL, 0, &cByteNeeded))
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return ;
		}
		pPrinterInfo = (PRINTER_INFO_2 *)malloc(cByteNeeded);
		if (!(pPrinterInfo))
			return ;

		if (!GetPrinter(hPrint, 2, (LPBYTE)pPrinterInfo, cByteNeeded, &cByteUsed))
		{
			free(pPrinterInfo);
			pPrinterInfo = NULL;
			return ;
		} 
		if (!EnumJobs(hPrint, 
			0, 
			(pPrinterInfo)->cJobs, 
			2, 
			NULL, 
			0,
			(LPDWORD)&cByteNeeded,
			(LPDWORD)&nReturned))
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return ;
		}

		pJobStorage = (JOB_INFO_2 *)malloc(cByteNeeded);
		if (!pJobStorage)
			return ;

		ZeroMemory(pJobStorage, cByteNeeded);


		if (!EnumJobs(hPrint, 
			0, 
			(pPrinterInfo)->cJobs, 
			2, 
			(LPBYTE)pJobStorage, 
			cByteNeeded,
			(LPDWORD)&cByteUsed,
			(LPDWORD)&nReturned))
		{
			return ;
		}

		bool bAllPrinted = true;
		CString sPrinterName;
		theLog.Write("cJobs=%d,nReturned=%d", pPrinterInfo->cJobs, nReturned);
		for (i = 0; i < nReturned; i++)
		{
			int nTempJobID = pJobStorage[i].JobId;
			DWORD  dwStatus = pJobStorage[i].Status;

			theLog.Write("当前打印机［%s］作业ID：%d   状态：%d",pJobStorage[i].pPrinterName,nTempJobID,dwStatus);
			if(nJobId == nTempJobID)
			{
				if(SetJob(hPrint,nTempJobID,0,NULL,JOB_CONTROL_DELETE))
					theLog.Write("删除JOB:%d 成功",nTempJobID);
				else
					theLog.Write("删除JOB:%d 失败：%d",nTempJobID,GetLastError());
			}
		}
		ClosePrinter(hPrint);
	}	

	szPrtName.ReleaseBuffer();
	//-----------------------
	//删除文件
	CString sPath;
	CString sDir;
	CRegistry2 reg(2,TRUE);
	if(reg.Open(XAB_PRINTER_KEY_PATH))
	{
		reg.Read(_T("JobDir"),sDir);
	}
	if (sDir.IsEmpty())
	{
		sDir = _T("c:\\Xab");
	}
	sPath.Format(_T("%s\\job_%06d.tif"),sDir,nJobId);


	theLog.Write("tiff = %s",sPath);
	if(PathFileExists(sPath))
	{
		BOOL b = DeleteFile(sPath);
		theLog.Write("RemoveJob %s exist,delete %d",sPath,b);

	}
	/*
	CString sDestPath;
	sDestPath.Format(_T("%sPrintJob\\job_%06d.tif.gz"),CCommonFun::GetDefaultPath(),nJobId);
	if(PathFileExists(sDestPath))
	{
	BOOL b = DeleteFile(sDestPath);
	theLog.Write("%s exist,delete %d",sDestPath,b);
	}*/
}

CXabPrinterMonitor::CXabPrinterMonitor()
{
	m_pCallBack = 0;
	m_hThread = 0;
	m_dwThreadId = 0;
	m_hEventStopRequested= 0;
	m_pCallBack = 0;
	SetWorkDir();
}

CXabPrinterMonitor::~CXabPrinterMonitor()
{
	StopMonitor();
}
void CXabPrinterMonitor::EnumeratePrinters( void )
{
	DWORD dwNeeded, 
		dwReturned;
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, NULL, 0, &dwNeeded, &dwReturned);
	if (dwNeeded <=0)
	{
		return;
	}

	LPBYTE lpBuffer = new BYTE[dwNeeded];
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, lpBuffer, dwNeeded, &dwNeeded, &dwReturned);

	PPRINTER_INFO_1 p1 = (PPRINTER_INFO_1) lpBuffer;

	for (DWORD x = 0; x < dwReturned; x++)
	{
		m_aryPrinter.Add(p1->pName);
		p1++;
	}

	delete lpBuffer;
}

void CXabPrinterMonitor::StartMonitor()
{
	CString sPrinter = IST_PRINTER_NAME;
	if (m_hThread&& m_hEventStopRequested)
	{
		theLog.Write("!!CXabPrinterMonitor::StartMonitor,1,StartMonitor %s,m_hThread=0x%x,m_hEventStopRequested=0x%x"
			, sPrinter, m_hThread, m_hEventStopRequested);
		return ;
	}
	if(!m_oFileMap.Create(MEM_MAP_NAME,MEM_MAP_SIZE))
	{
		theLog.Write("Create MEM_MAP_NAME = %s,%d",MEM_MAP_NAME,::GetLastError());
	}

	m_aryPrinter.RemoveAll();
	m_aryPrinterHandle.RemoveAll();
	m_aryPrinter.Add(sPrinter);
	HANDLE hPrinter = NULL;
	if(OpenPrinter((LPTSTR) (LPCTSTR) sPrinter, &hPrinter, NULL) && hPrinter )
	{
		m_aryPrinterHandle.Add(hPrinter);
		theLog.Write("##CXabPrinterMonitor::StartMonitor,10,Oprinter OK,prt=%s", sPrinter);
	}
	else
	{
		theLog.Write("!!CXabPrinterMonitor::StartMonitor,11,Oprinter fail,prt=%s,err=%d", sPrinter, GetLastError());
	}
	if(m_aryPrinterHandle.GetCount() > 0)
	{
		Enable();
	}
}

void CXabPrinterMonitor::StopMonitor()
{
	Enable(FALSE);
	m_aryPrinter.RemoveAll();
	for(int n = 0; n < m_aryPrinterHandle.GetCount(); n++)
	{
		ClosePrinter(m_aryPrinterHandle.GetAt(n));
	}
	m_aryPrinterHandle.RemoveAll();
}

DWORD WINAPI CXabPrinterMonitor::ThreadFunc( void* pVoid)
{
	CXabPrinterMonitor* pThis = (CXabPrinterMonitor*)pVoid;

	PPRINTER_NOTIFY_INFO pNotification = NULL;
#if 0
	WORD JobFields[] = 
	{
		JOB_NOTIFY_FIELD_PRINTER_NAME,
		JOB_NOTIFY_FIELD_MACHINE_NAME,
		JOB_NOTIFY_FIELD_PORT_NAME,
		JOB_NOTIFY_FIELD_USER_NAME,
		JOB_NOTIFY_FIELD_NOTIFY_NAME,
		JOB_NOTIFY_FIELD_DATATYPE,
		JOB_NOTIFY_FIELD_PRINT_PROCESSOR,
		JOB_NOTIFY_FIELD_PARAMETERS,
		JOB_NOTIFY_FIELD_DRIVER_NAME,
		JOB_NOTIFY_FIELD_DEVMODE,
		JOB_NOTIFY_FIELD_STATUS,
		JOB_NOTIFY_FIELD_STATUS_STRING,
		JOB_NOTIFY_FIELD_DOCUMENT,
		JOB_NOTIFY_FIELD_PRIORITY,
		JOB_NOTIFY_FIELD_POSITION,
		JOB_NOTIFY_FIELD_SUBMITTED,
		JOB_NOTIFY_FIELD_START_TIME,
		JOB_NOTIFY_FIELD_UNTIL_TIME,
		JOB_NOTIFY_FIELD_TIME,
		JOB_NOTIFY_FIELD_TOTAL_PAGES,
		JOB_NOTIFY_FIELD_PAGES_PRINTED,
		JOB_NOTIFY_FIELD_TOTAL_BYTES,
		JOB_NOTIFY_FIELD_BYTES_PRINTED
	};
#else
	WORD JobFields[] = 
	{
		JOB_NOTIFY_FIELD_SUBMITTED,
		JOB_NOTIFY_FIELD_BYTES_PRINTED,
		JOB_NOTIFY_FIELD_STATUS
	};
#endif
	PRINTER_NOTIFY_OPTIONS_TYPE	Notifications[1] =					
	{
		{
			JOB_NOTIFY_TYPE,
				0,
				0,
				0,
				sizeof(JobFields) / sizeof(JobFields[0]),
				JobFields
		},
	};
	PRINTER_NOTIFY_OPTIONS NotificationOptions = 
	{
		2,
		0/*PRINTER_NOTIFY_OPTIONS_REFRESH*/,	//暂时不需要此参数，详细见msdn
		sizeof(Notifications) / sizeof(Notifications[0]),
		Notifications
	};

	DWORD dwChange = 0;
	DWORD dwCount = pThis->m_aryPrinterHandle.GetCount() + 1;
	HANDLE* pHandle = new HANDLE[dwCount];
	for (int n = 0 ; n < pThis->m_aryPrinterHandle.GetCount(); n ++)
	{
		pHandle[n] = FindFirstPrinterChangeNotification(pThis->m_aryPrinterHandle.GetAt(n),
			PRINTER_CHANGE_ALL,
			0, 
			&NotificationOptions);
		if(!pHandle[n])
		{
			ASSERT(0);
			theLog.Write("!!CXabPrinterMonitor::ThreadFunc,1,FindFirstPrinterChangeNotification err=%d,n=%d", GetLastError(), n);
		}
	}
	pHandle[dwCount - 1] = pThis->m_hEventStopRequested;

	while(1)
	{
		DWORD dwRet = WaitForMultipleObjects(dwCount, pHandle, FALSE, INFINITE);
		if(dwRet == (WAIT_OBJECT_0 + dwCount - 1))
		{
			theLog.Write("!!CXabPrinterMonitor::ThreadFunc,2,printer monitor stop");
			break;
		}
		else if(dwRet >= WAIT_OBJECT_0 && dwRet < (WAIT_OBJECT_0 + dwCount - 1))
		{
			FindNextPrinterChangeNotification(pHandle[dwRet], &dwChange, &NotificationOptions, (LPVOID *) &pNotification);
			theLog.Write("##CXabPrinterMonitor::ThreadFunc,3,dwChange %d,pNotification=0x%x",dwChange,pNotification);
			if (pNotification != NULL)
			{
				//------add by zfq,2013.01.10,test,begin
				if((dwChange & PRINTER_CHANGE_ADD_PORT) == PRINTER_CHANGE_ADD_PORT)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.111,PRINTER_CHANGE_ADD_JOB");
				}

				if((dwChange & PRINTER_CHANGE_CONFIGURE_PORT) == PRINTER_CHANGE_CONFIGURE_PORT)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.112,PRINTER_CHANGE_CONFIGURE_PORT");
				}

				if((dwChange & PRINTER_CHANGE_DELETE_PORT) == PRINTER_CHANGE_DELETE_PORT)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.113,PRINTER_CHANGE_DELETE_PORT");
				}

				if((dwChange & PRINTER_CHANGE_ADD_PRINT_PROCESSOR) == PRINTER_CHANGE_ADD_PRINT_PROCESSOR)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.121,PRINTER_CHANGE_ADD_PRINT_PROCESSOR");
				}

				if((dwChange & PRINTER_CHANGE_DELETE_PRINT_PROCESSOR) == PRINTER_CHANGE_DELETE_PRINT_PROCESSOR)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.122,PRINTER_CHANGE_DELETE_PRINT_PROCESSOR");
				}				

				if((dwChange & PRINTER_CHANGE_ADD_JOB) == PRINTER_CHANGE_ADD_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.131,PRINTER_CHANGE_ADD_JOB");
				}

				if((dwChange & PRINTER_CHANGE_SET_JOB) == PRINTER_CHANGE_SET_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.132,PRINTER_CHANGE_SET_JOB");
				}

				if((dwChange & PRINTER_CHANGE_DELETE_JOB) == PRINTER_CHANGE_DELETE_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.133,PRINTER_CHANGE_DELETE_JOB");
				}				

				if((dwChange & PRINTER_CHANGE_WRITE_JOB) == PRINTER_CHANGE_WRITE_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.134,PRINTER_CHANGE_WRITE_JOB");
#if 0
					DWORD dwJobID = pNotification->aData[x].Id;	//作业ID
					if(!pThis->PauseThisJob(dwRet, dwJobID))
					{
						theLog.Write("!!CXabPrinterMonitor::ThreadFunc,3.1341,PRINTER_CHANGE_WRITE_JOB,PauseThisJob fail,err=%d", GetLastError());
					}
#endif
/*
#define JOB_STATUS_PAUSED               0x00000001
#define JOB_STATUS_ERROR                0x00000002
#define JOB_STATUS_DELETING             0x00000004
#define JOB_STATUS_SPOOLING             0x00000008
#define JOB_STATUS_PRINTING             0x00000010
#define JOB_STATUS_OFFLINE              0x00000020
#define JOB_STATUS_PAPEROUT             0x00000040
#define JOB_STATUS_PRINTED              0x00000080
#define JOB_STATUS_DELETED              0x00000100
#define JOB_STATUS_BLOCKED_DEVQ         0x00000200
#define JOB_STATUS_USER_INTERVENTION    0x00000400
#define JOB_STATUS_RESTART              0x00000800
*/
				}				

				if((dwChange & PRINTER_CHANGE_ADD_FORM) == PRINTER_CHANGE_ADD_FORM)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.141,PRINTER_CHANGE_ADD_FORM");
				}				

				if((dwChange & PRINTER_CHANGE_SET_FORM) == PRINTER_CHANGE_SET_FORM)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.142,PRINTER_CHANGE_SET_FORM");
				}				
				if((dwChange & PRINTER_CHANGE_DELETE_FORM) == PRINTER_CHANGE_DELETE_FORM)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.143,PRINTER_CHANGE_DELETE_FORM");
				}				

				//------add by zfq,2013.01.10,test,end


				if((dwChange & PRINTER_CHANGE_ADD_JOB) == PRINTER_CHANGE_ADD_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.2,PRINTER_CHANGE_ADD_JOB");
//					JOB_CONTROL_PAUSE
				}
				else if((dwChange & PRINTER_CHANGE_SET_JOB) == PRINTER_CHANGE_SET_JOB)
				{
					theLog.Write("##CXabPrinterMonitor::ThreadFunc,3.3,PRINTER_CHANGE_SET_JOB");
					for (DWORD x = 0; x < pNotification->Count; x++)
					{
						if(pNotification->aData[x].Type == JOB_NOTIFY_TYPE)
						{
							DWORD dw = pNotification->aData[x].Id;	//作业ID
							//pThis->OnJobCreated(dwRet,dw);

							theLog.Write("##CXabPrinterMonitor::ThreadFunc,4.%d,A,printer job id = %d,%X"
								, x, dw, pNotification->aData[x].Field);
							if (pNotification->aData[x].Field == JOB_NOTIFY_FIELD_STATUS)
							{
								theLog.Write("##CXabPrinterMonitor::ThreadFunc,4.%d,B,jibid = %d,status %d"
									, x, dw, pNotification->aData[x].NotifyData.adwData[0]); 


								DWORD dwStatus = pNotification->aData[x].NotifyData.adwData[0];

/*
								if(JOB_STATUS_PRINTING & dwStatus)
								{
									theLog.Write("##CXabPrinterMonitor::ThreadFunc,4.%d,C,JOB_STATUS_PRINTING", x);
								}

								if(!(JOB_CONTROL_PAUSE & dwStatus))
								{
									theLog.Write("##CXabPrinterMonitor::ThreadFunc,4.%d,D,NOT JOB_STATUS_PRINTING", x);
								}

*/
								//--add by zfq,test,begin
								if((JOB_STATUS_PRINTING & dwStatus)
									&&
									!(JOB_CONTROL_PAUSE & dwStatus))
								{//正在打印并且没有被暂停的时候，暂停该作业
									CString sname = pThis->m_aryPrinter.GetAt(dwRet);
									CPrinterJobHelper hepler;
//									static BOOL bPauseIt = TRUE;
									CIniFile theIniFile;
									theIniFile.m_sPath.Format("%s%s",CCommonFun::GetDefaultPath(), SYS_CONFIG_FILE);
									BOOL bPauseIt = (0 == theIniFile.GetVal("PrintJob", "PauseIt", 1)) ? FALSE : TRUE;
 
									theLog.Write("##CXabPrinterMonitor::ThreadFunc,4.%d,F,bPauseIt=%d", x, bPauseIt);


									if(bPauseIt)
									{
										{
											theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.%d,PrintName=%s,Pause it OK", x, sname);
											CString szDestPath;
											szDestPath.Format("C:\\xlog\\%u.spl", dw);
											if(!hepler.CopySpoolFile(sname.GetBuffer(), dw, szDestPath))
											{
												theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.%d,0.2,PrintName=%s,CopySpoolFile fail,err=%d"
													, x, sname, GetLastError());
											}
											sname.ReleaseBuffer();
										}


										if(!hepler.PauseThisJob(sname.GetBuffer(), dw))
										{
											theLog.Write("!!CXabPrinterMonitor::ThreadFunc,10.%d,PrintName=%s", x, sname);
										}
										else
										{
#if 1
											theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.%d,PrintName=%s,Pause it OK", x, sname);
											CString szDestPath;
											szDestPath.Format("C:\\xlog\\%u.spl", dw);
											sname.ReleaseBuffer();
											if(!hepler.CopySpoolFile(sname.GetBuffer(), dw, szDestPath))
											{
												theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.%d,0.2,PrintName=%s,CopySpoolFile fail,err=%d"
													, x, sname, GetLastError());
											}
#endif
										}
										sname.ReleaseBuffer();
									}
									else
									{
										bPauseIt = TRUE;
										theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.%d,3,NextTime to Pause it", x);
									}
								}
								//--add by zfq,test,end


								if ((dwStatus & JOB_STATUS_COMPLETE) || (dwStatus & JOB_STATUS_PRINTED))
								{//工作到这里，不能在打印完成后再调用，应该在物理打印机打印之前就取，否则打印完成，该作业就消失了
									theLog.Write("printjob suc %d",dw);

/*
									{//测试
										CString sname = m_aryPrinter.GetAt(nPrtIdx);
										CPrinterJobHelper hepler;
										if(sname.CompareNoCase(IST_PRINTER_NAME) == 0 )
										{
											SubmitPrintJob log;
											theLog.Write("##CXabPrinterMonitor::ThreadFunc,10.1,%s,%d",sname,dwId);
											BOOL b = hepler.GetJobInfo(sname.GetBuffer(),dwId,log);
											if(b)
											{//停掉该作业

											}
										}

									}
*/

									pThis->OnJobCreated(dwRet,dw);
								}
							}

						}
					}
				}
				FreePrinterNotifyInfo(pNotification);
				pNotification = NULL;
			}

		}
	}

	for (int n = 0; n < dwCount - 1; n ++)
	{
		FindClosePrinterChangeNotification(pHandle[n]);
	}

	delete pHandle;

	return 0;
}


void CXabPrinterMonitor::Enable(BOOL bEnable /*= TRUE*/)
{
	if(bEnable)
	{
		if(!m_hEventStopRequested)
		{
			m_hEventStopRequested = CreateEvent(0,FALSE,FALSE,0);
			ASSERT(m_hEventStopRequested);
			m_hThread = ::CreateThread(0,0,ThreadFunc,this,0,&m_dwThreadId);
			ASSERT(m_hThread);
			if(!m_hThread)
			{
				CloseHandle(m_hEventStopRequested);
				m_hEventStopRequested = NULL;
			}
		}
	}
	else
	{
		if(m_hEventStopRequested)
		{
			SetEvent(m_hEventStopRequested);
			::WaitForSingleObject(m_hThread,INFINITE);
			CloseHandle(m_hEventStopRequested);
			CloseHandle(m_hThread);
		}
	}	
}

void CXabPrinterMonitor::OnJobCreated(int nPrtIdx,DWORD dwId)
{
	theLog.Write("##CXabPrinterMonitor::OnJobCreated,1");

	CString sname = m_aryPrinter.GetAt(nPrtIdx);
	CPrinterJobHelper hepler;
	if(sname.CompareNoCase(IST_PRINTER_NAME) == 0 )
	{
		SubmitPrintJob log;
		theLog.Write("OnJobCreated %s,%d",sname,dwId);
		BOOL b = hepler.GetJobInfo(sname.GetBuffer(),dwId,log);
		sname.ReleaseBuffer();
		if(m_pCallBack)
		{
			if(b)
			{
				m_pCallBack->OnPrinted(&log);
			}
			else
			{
				ASSERT(0);
			}
		}
	}
}
void RemoveUsers( ATL::CDacl &pDacl)
{
	for(UINT i = 0; i < pDacl.GetAceCount(); i++)
	{
		ATL::CSid pSid;
		ACCESS_MASK pMask = 0;
		BYTE pType = 0, pFlags = 0;
		/* Get the ith ACL */

		const_cast<ATL::CDacl &>(pDacl).GetAclEntry(i, &pSid, &pMask, &pType, &pFlags);

		CString str(pSid.AccountName());

		if (str.CompareNoCase("Users") == 0 )
		{
			pDacl.RemoveAce(i);
		}
	}
}
BOOL AddObjectAccess(CString strFileName, const CSid &rSid, ACCESS_MASK accessmask,SE_OBJECT_TYPE eType /*= SE_OBJECT_TYPE*/)
{
	ATL::CSecurityDesc OutSecDesc;
	ATL::AtlGetSecurityDescriptor(strFileName, eType, &OutSecDesc);
	OutSecDesc.MakeAbsolute();
	bool pbPresent = 0;

	ATL::CDacl pDacl;
	if (!OutSecDesc.GetDacl(&pDacl, &pbPresent))
		return FALSE;

	RemoveUsers(pDacl);

	if (!pDacl.AddAllowedAce(rSid, accessmask,  CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE))
	{
		//已经把已有的删掉了！
		return FALSE;
	}

	OutSecDesc.SetControl(SE_DACL_AUTO_INHERITED | SE_DACL_PROTECTED, SE_DACL_AUTO_INHERITED);
	ATL::AtlSetDacl(strFileName, eType, pDacl);
	ATL::CSacl pSacl;
	/* We've already set the Dacl. Now set the SACL. */
	OutSecDesc.GetSacl(&pSacl, &pbPresent);
	if(pbPresent)
	{
		ATL::AtlSetSacl(strFileName, eType, pSacl);
	}
	return TRUE;
}


void CXabPrinterMonitor::SetWorkDir()
{
	CRegistryEx reg(2,FALSE);
	if(reg.CreateKey(XAB_PRINTER_KEY_PATH))
	{
		TCHAR szDir[MAX_PATH + 1] = {0};
		GetWindowsDirectory(szDir,MAX_PATH + 1);
		strcat_s(szDir,sizeof(szDir),"\\temp");
		CreateDirectory(szDir,NULL) ;
		strcat_s(szDir,sizeof(szDir),"\\Xab");
		CreateDirectory(szDir,NULL) ;
		reg.Write(XAB_JOB_DIR,szDir);
		theLog.Write("jobDir = %s",szDir);
		AddObjectAccess(szDir,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
	}
	//设置注册表权限
	CString sPath;
	sPath.Format(_T("MACHINE\\%s"),XAB_PRINTER_KEY_PATH);
	AddObjectAccess(sPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_REGISTRY_KEY);

	TCHAR szDir[MAX_PATH + 1] = {0};
	::GetSystemDirectory(szDir,MAX_PATH + 1);
	sPath.Format(_T("%s\\%s"),szDir,IST_PRINTER_PORT);

	CFile f;
	CFileException e;
	TCHAR* pszFileName = _T("Open_File.dat");
	if(!f.Open(sPath, CFile::modeCreate | CFile::modeWrite, &e))
	{
		theLog.Write(_T("File could not be opened %d\n"), e.m_cause);
	}
	AddObjectAccess(sPath,ATL::Sids::Users(), GENERIC_ALL|STANDARD_RIGHTS_ALL,SE_FILE_OBJECT);
}
