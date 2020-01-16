#include "StdAfx.h"
#include "Printermonitor.h"
// #include "spl_h.h"
#include <cassert>
#include <atlimage.h>
#include <math.h>

#ifdef DEF_UseThis	//========================================================================================
struct MY_STRUCT
{
	DWORD       dwType;
	const char *pName;
};

#define MACRO_DATA(nm) {nm, #nm}

static MY_STRUCT g_table[] = {
	MACRO_DATA(SRT_METAFILE),
	MACRO_DATA(SRT_ENGINE_FONT),//       0x00000002 /*  2 A TrueType font definition               */
	MACRO_DATA(SRT_DEVMODE),//           0x00000003 /*  3 DevMode                                  */
	MACRO_DATA(SRT_TYPE1_FONT),//        0x00000004 /*  4 A PostScript Type 1 font definition      */
	MACRO_DATA(SRT_PRESTARTPAGE),//      0x00000005 /*  5 The start page for EPS                   */
	MACRO_DATA(SRT_DESIGNVECTOR),//      0x00000006 /*  6 A font design vector (Multiple Master)   */
	MACRO_DATA(SRT_SUBSET_FONT),//       0x00000007 /*  7 A subset font definition                 */
	MACRO_DATA(SRT_DELTA_FONT),//        0x00000008 /*  8 A delta font definition                  */
	MACRO_DATA(SRT_FORM_METAFILE),//     0x00000009 /*  9 EMF                                      */
	MACRO_DATA(SRT_BW_METAFILE),//       0x0000000A /* 10 Monochrome EMF                           */ 
	MACRO_DATA(SRT_BW_FORM_METAFILE),//  0x0000000B /* 11 Monochrome EMF                           */
	MACRO_DATA(SRT_METAFILE_DATA),//     0x0000000C /* 12 Enhanced Meta File (EMF) Win2k           */ 
	MACRO_DATA(SRT_EXT_METAFILE),//      0x0000000D /* 13 An offset to document content            */
	MACRO_DATA(SRT_EXT_BW_METAFILE),//   0x0000000E /* 14 An offset to monochrome document content */
	MACRO_DATA(SRT_EXT_ENGINE_FONT),//   0x0000000F /* 15 An offset to a TrueType font definition  */
	MACRO_DATA(SRT_EXT_TYPE1_FONT),//    0x00000010 /* 16 An offset to a PS Type 1 font definition */
	MACRO_DATA(SRT_EXT_DESIGNVECTOR),//  0x00000011 /* 17 An offset to a font design vector        */
	MACRO_DATA(SRT_EXT_SUBSET_FONT),//   0x00000012 /* 18 An offset to a subset font definition    */
	MACRO_DATA(SRT_EXT_DELTA_FONT),//    0x00000013 /* 19 An offset to a delta font definition     */
	MACRO_DATA(SRT_EXT_PS_JOB_DATA)//   0x00000014 /* 20 Document-level PostScript data           */
};

//////////////////////////////////////////////////////////////////////////
//
CPrinterJobHelper::CPrinterJobHelper()
{
	
}

CPrinterJobHelper::~CPrinterJobHelper()
{

}
void CPrinterJobHelper::KeepJob(HANDLE hPrint,BOOL bEnable ,BOOL& bKeep)
{
	PRINTER_INFO_2		*pPrinterInfo = NULL;

	DWORD	cByteNeeded, nReturned=0, cByteUsed, i;

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
	if (pPrinterInfo->Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS)
	{
		bKeep = TRUE;
	}
	else
	{
		bKeep = FALSE;
	}
	if(bEnable)
		pPrinterInfo->Attributes |= PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS;
	else
		pPrinterInfo->Attributes |= ~PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS;

	SetPrinter(hPrint,2,(BYTE*)pPrinterInfo,0);

	if (pPrinterInfo)
	{
		free(pPrinterInfo);
		pPrinterInfo = NULL;
	}
}
//
BOOL CPrinterJobHelper::GetJobInfo(LPTSTR sPrinter,int nJobID,OutGoingLog& log)
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
			theLog.Write("OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
			break;
		}
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				ClosePrinter(hPrinter);
				theLog.Write("GetJob1 error err：%d %d",GetLastError(),cbNeed);
				break;
			}
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
			break;
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			ClosePrinter(hPrinter);
			theLog.Write("GetJob2 error: %d",GetLastError());
			break;
		}	
		pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

		//获取文件
		CString sZipPath ;
		int nJobSize = 0;
		int nPageCount = 0;
		if(GetTiffFile(sPrinter,nJobID,sZipPath,nJobSize,nPageCount))
		{
			theLog.Write("zipPath %s",sZipPath);
			CFileStatus st;
			CFile::GetStatus(sZipPath,st);
			
			
			SYSTEMTIME lst;
			SystemTimeToTzSpecificLocalTime(0,&pJobInfo->Submitted,&lst);
			COleDateTime dt(lst);
		//	log.set_submit_time(dt.m_dt);
			theLog.Write("submittime :(%d-%d-%d %d:%d:%d)",lst.wYear,lst.wMonth,lst.wDay,lst.wHour,lst.wMinute,lst.wSecond);
			log.set_sourcefilepath(sZipPath);
			log.set_sourcefilename(pJobInfo->pDocument);
			//删除作业
			RemoveSecJob(nJobID);

			bRet = TRUE;
		}

		theLog.Write("pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation );

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
			theLog.Write("OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
			break;
		}
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				ClosePrinter(hPrinter);
				theLog.Write("GetJob1 error err：%d",GetLastError());
				break;
			}
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
			break;
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			ClosePrinter(hPrinter);
			theLog.Write("GetJob2 error: %d",GetLastError());
			break;
		}	
		pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

		//获取文件
		CString sZipPath ;
		int nJobSize = 0;
		int nPageCount = 0;
		if(GetTiffFile(sPrinter,nJobID,sZipPath,nJobSize,nPageCount))
		{
			theLog.Write("zipPath %s",sZipPath);
			CFileStatus st;
			CFile::GetStatus(sZipPath,st);
			//SubmitPrintJob log;
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

			if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A3)
				log.set_page_type("A3");
			else if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A4)
				log.set_page_type("A4");
			else
				log.set_page_type("A4");

			if(pJobInfo->pDevMode->dmDuplex != DMDUP_SIMPLEX )
			{
				log.set_double_print(DMDUP_VERTICAL);
			}
			else
			{
				log.set_double_print(DMDUP_SIMPLEX);
			}
			log.set_orientation( pJobInfo->pDevMode->dmOrientation);
			//删除作业
			RemoveJob(nJobID);

			bRet = TRUE;
		}

		theLog.Write("pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation );

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

//获取打印信息
BOOL CPrinterJobHelper::GetJobInfo(LPTSTR sPrinter,int nJobID,PrintLog& log)
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
			theLog.Write("OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
			break;
		}
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				ClosePrinter(hPrinter);
				theLog.Write("GetJob1 error err：%d",GetLastError());
				break;
			}
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
			break;
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			ClosePrinter(hPrinter);
			theLog.Write("GetJob2 error: %d",GetLastError());
			break;
		}	
		pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

		//获取文件		
		{
			log.set_color(pJobInfo->pDevMode->dmColor);
			log.set_copies(pJobInfo->pDevMode->dmCopies);
			log.set_doc_name(pJobInfo->pDocument);
			log.set_winuser(pJobInfo->pUserName);
	#if 1
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
			log.set_time(dt.m_dt);
			theLog.Write("submittime :(%d-%d-%d %d:%d:%d)",lst.wYear,lst.wMonth,lst.wDay,lst.wHour,lst.wMinute,lst.wSecond);
			
			if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A3)
				log.set_page_type("A3");
			else if(pJobInfo->pDevMode->dmPaperSize == DMPAPER_A4)
				log.set_page_type("A4");
			else
				log.set_page_type("A4");

			if(pJobInfo->pDevMode->dmDuplex != DMDUP_SIMPLEX )
			{
				log.set_double_print(DMDUP_VERTICAL);
			}
			else
			{
				log.set_double_print(DMDUP_SIMPLEX);
			}

			bRet = TRUE;
		}

		theLog.Write("pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation );
		
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

BOOL CPrinterJobHelper::GetJobInfo(LPTSTR sPrinter,int nJobID,JOB_INFO_2*& pVal)
{
	DWORD cbNeed = 0;
	HANDLE hPrinter = NULL;
	BYTE *pJobData = NULL;
	BOOL bRet = FALSE;
	JOB_INFO_2* pJobInfo = 0;
	do 
	{
		if(!OpenPrinter(sPrinter,&hPrinter,NULL))
		{
			theLog.Write("OpenPrinter:[%s] Error:%d",sPrinter,GetLastError());
			break;
		}
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				ClosePrinter(hPrinter);
				theLog.Write("GetJob1 error err：%d,%d",GetLastError(),cbNeed);
				break;
			}
			theLog.Write("job size = %d",cbNeed);
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
			break;
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			ClosePrinter(hPrinter);
			theLog.Write("GetJob2 error: %d,%d",GetLastError(),cbNeed);
			break;
		}	

		bRet = TRUE;
		pJobInfo = (JOB_INFO_2*)pJobData;
		theLog.Write("pPrinterName:%s  pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  "
			" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d "
			" dmColor:%d  dmDuplex:%d orig = %d",
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor,
			pJobInfo->pDevMode->dmDuplex,
			pJobInfo->pDevMode->dmOrientation );

	} while (0);
	if(hPrinter)
	{
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
	if (bRet)
	{
		pVal = (JOB_INFO_2*)pJobData;
	}
	else
	{
		if (pJobData)
		{
			delete pJobData;
		}
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
BOOL CPrinterJobHelper::ZipSpoolFile(CString sPath)
{
	return TRUE;
}

BOOL CPrinterJobHelper::SpoolToEmf(const char* pFile,const char* pOut)
{
	ASSERT(pFile);
	ASSERT(pOut);
	if(!pFile || !pOut)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	FILE *fp = NULL;
	do 
	{
		fp = fopen ( pFile, "rb" );
		assert ( fp && "文件打开失败!" );
		if(!fp)
		{
			break;
		}
		BYTE buf[2048] = {0};
		int nSize = fread (buf, 1, 2048, fp);
		SPL_HEADER *pSpl = reinterpret_cast<SPL_HEADER*>(buf);
		if ( nSize < sizeof(SPL_HEADER) )
		{
			break;
		}

		//doc name
		WCHAR * pName = (WCHAR*)((BYTE*)pSpl+pSpl->offDocumentName);

		int nSeek = pSpl->nSize;
		int nFileTag = 0;
		for (;0 == fseek ( fp, nSeek ,SEEK_SET);)
		{
			int nRet = fread (buf, 1, sizeof(SMR), fp);
			if ( nRet < sizeof(SMR) ) 
			{
				break;
			}

			SMR *pSMR = reinterpret_cast<SMR *>(buf);
			if ( SRT_METAFILE_DATA == pSMR->iType )
			{
				int nReadSize = pSMR-> nSize;// - sizeof(SMR);
				BYTE *pRead = new BYTE[nReadSize];
				assert(pRead && "pread = null");
				if(pRead)
				{
					int nReaded = fread (pRead, 1, nReadSize, fp );
					if ( nReadSize == nReaded )
					{
						ENHMETAHEADER *pKK = reinterpret_cast<ENHMETAHEADER*>( pRead );
						assert ( nReadSize == nReaded );
						char szWriteName[MAX_PATH] = {0};
						sprintf_s (szWriteName,sizeof(szWriteName), "%s\\%02d.emf",pOut,nFileTag++);
						FILE *fpWrite = fopen (szWriteName, "wb+" );
						assert ( fpWrite && "文件写入失败!" );
						if(fpWrite)
						{
							DWORD dwW = pKK->nBytes;// + sizeof(ENHMETAHEADER);//  + sizeof(ENHMETAHEADER);
							if ( dwW <= pSMR->nSize )
							{
								nReaded = fwrite (pRead, 1, dwW , fpWrite );
								assert ( nReaded == dwW );
							}
							fclose (fpWrite);
						}
					}
				}
			}
			else
			{
				//other type;
			}
			
			nSeek += pSMR->nSize + sizeof(SMR);
		}
		
	} while (0);
	
	if (fp)
	{
		fclose (fp);
		fp = NULL;
	}
	return TRUE;
}

BOOL CPrinterJobHelper::EmfToBmp(CString sEmf,CString sBmp)
{
	//Metafile的矩形计算的变量声明
	RECT	rect = {0};
	float	PixelsX = 0, PixelsY = 0, MMX = 0, MMY = 0;
	float fAspectRatio = 0;
	long lWidth = 0, lHeight = 0;
	DWORD dwRetError = 0;

	ENHMETAHEADER emh = {0};
	HENHMETAFILE  hemf = NULL;
	HBITMAP 	  bitmap = NULL;
	HDC 		  memDC = NULL;
	HPALETTE pal = NULL;
	CImage        objImage;
	HBRUSH hbru = NULL;
	BOOL bRet = FALSE;
	HDC dc = ::GetDC(NULL);

	do 
	{
		if(!dc)
		{
			break;
		}
		hemf = ::GetEnhMetaFile(sEmf);            //得到EMF头
		if(!hemf)
		{
			break;
		}
		ZeroMemory(&emh, sizeof(ENHMETAHEADER));
		emh.nSize = sizeof(ENHMETAHEADER);
		if( GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &emh) == 0)
		{
			break;
		}

		//获得输出设备的特性。
		PixelsX = (float)GetDeviceCaps(dc, HORZRES);
		PixelsY = (float)GetDeviceCaps(dc, VERTRES);
		MMX = (float)GetDeviceCaps(dc, HORZSIZE);
		MMY = (float)GetDeviceCaps(dc, VERTSIZE);

		//计算EMF矩形图元在设备上的大小
		rect.top = (int)((float)(emh.rclFrame.top) * PixelsY / (MMY*100.0f));
		rect.left = (int)((float)(emh.rclFrame.left) * PixelsX / (MMX*100.0f));
		rect.right = (int)((float)(emh.rclFrame.right) * PixelsX / (MMX*100.0f));
		rect.bottom = (int)((float)(emh.rclFrame.bottom) * PixelsY / (MMY*100.0f));

		//计算宽度和高度
		lWidth = (long)((float)(abs(rect.left - rect.right)));
		lHeight =(long)((float)(abs(rect.top-rect.bottom )));

		//memDc填充矩形结构
		rect.left = 0;
		rect.top = 0;
		rect.right = lWidth;
		rect.bottom = lHeight;

		//创建一个内存DC兼容WindowDC
		memDC=::CreateCompatibleDC(dc); 
		if (!memDC)
		{
			break;
		}
		//创建一个与WindwosDc兼容的位图
		bitmap = ::CreateCompatibleBitmap(dc,lWidth,lHeight);
		if(!bitmap)
		{
			break;
		}
		dwRetError = GetLastError();

		//memDc选择位图对象
		::SelectObject(memDC, bitmap);

		//画出白色背景
		hbru = CreateSolidBrush(RGB(255, 255, 255));     //创建白色刷子,
		if(!hbru)
		{
			break;
		}
		FillRect(memDC, &rect, hbru);

		//将hemf图元按rect大小画上memDC，并忽略其返回值
		PlayEnhMetaFile(memDC, hemf, &rect);	
		dwRetError = GetLastError();

		//如果设备支持调色板，创建逻辑调色板
/*		if( GetDeviceCaps(dc,RASTERCAPS) & RC_PALETTE)
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
			LOGPALETTE *pLP = (LOGPALETTE*) new BYTE[nSize];
			pLP->palVersion = 0x300;

			pLP->palNumEntries = GetSystemPaletteEntries(dc, 0, 255, pLP->palPalEntry);

			pal = ::CreatePalette(pLP);   //该函数创建一个逻辑彩色调色板
			delete[] pLP;
		}

		HANDLE hDIB = DDBToDIB(bitmap, BI_RGB, pal);  //设备相关位图(DDB) 转 设备无关位图（DIB）

		if(hDIB == NULL)
		{
			DeleteEnhMetaFile(hemf);
			return FALSE;
		}
		*/
		bRet = TRUE;
	} while (0);
	
	if(bRet)
	{
		objImage.Attach(bitmap);
		bRet = objImage.Save(sBmp);
	}
	//::GlobalFree(hDIB);
	if(hemf)
	{
		::DeleteEnhMetaFile(hemf);
	}
	if(dc)
	{
		::ReleaseDC(NULL, dc);
	}
	return bRet;
}

BOOL CPrinterJobHelper::EmfToJpg(CString sPath)
{
	return TRUE;
}
BOOL CPrinterJobHelper::GetTiffFile(LPTSTR pPrint,int nJobId,CString& sZipPath,int& nSize,int& nPageCount)
{
	CString sPath;
	CString sDir;
	CRegistry2 reg(2,TRUE);
	BOOL bZip = TRUE;
	if(stricmp(pPrint,IST_PRINTER_NAME) == 0)
	{
		if(reg.Open(XAB_PRINTER_KEY_PATH))
		{
			reg.Read(_T("JobDir"),sDir);
		}
		
	}
	else if (stricmp(pPrint,SEC_PRINTER_NAME) == 0)
	{
		if(reg.Open(SEC_PRINTER_KEY_PATH))
		{
			reg.Read(_T("JobDir"),sDir);
		}
		bZip = FALSE;
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
		theLog.Write("GetTiffFile %s not exist",sPath);
	}
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
		for (int n = 0 ; n < count; n ++)
		{
			// Display the GUID of the first (and only) frame dimension.
			WCHAR strGuid[39];
			StringFromGUID2(pDimensionIDs[n], strGuid, 39);
			TRACE("The first (and only) dimension ID is %s.\n", strGuid);

			// Get the number of frames in the first dimension.
			frameCount += image.GetFrameCount(&pDimensionIDs[n]);
		}
		nPageCount = frameCount;
		//--------------------------------

		if(bZip)
		{
			//压缩
			char* pPath = sDestPath.GetBuffer();
			file_compress(pPath,-1);
			sDestPath.ReleaseBuffer();
			sZipPath.Format("%s.gz",sDestPath);
		}
		else 
		{
			CTFEncrypt tif;
			tif.EncryptFile(sDestPath);
			sZipPath =sDestPath;
		}
		
		return TRUE;
	}
	return FALSE;
}

BOOL CPrinterJobHelper::RemoveJob(CString sName,int nJobId)
{
	BOOL bRet = FALSE;
	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(sName.GetBuffer(),&hPrint,&pd))
	{
		if(bRet = SetJob(hPrint,nJobId,0,NULL,JOB_CONTROL_DELETE))
			theLog.Write("RemoveJob :%d 成功",nJobId);
		else
			theLog.Write("RemoveJob :%d 失败：%d",nJobId,GetLastError());

		ClosePrinter(hPrint);
	}	
	sName.ReleaseBuffer();
	return bRet;
}

BOOL CPrinterJobHelper::ResumeJob(CString sName,int nJobId)
{
	BOOL bRet = FALSE;
	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(sName.GetBuffer(),&hPrint,&pd))
	{
		if(bRet = SetJob(hPrint,nJobId,0,NULL,JOB_CONTROL_RESUME))
			theLog.Write("ResumeJob :%d 成功",nJobId);
		else
			theLog.Write("ResumeJob :%d 失败：%d",nJobId,GetLastError());

		ClosePrinter(hPrint);
	}	
	sName.ReleaseBuffer();
	return bRet;
}
BOOL CPrinterJobHelper::PauseJob(CString sName,int nJobId)
{
	BOOL bRet = FALSE;
	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(sName.GetBuffer(),&hPrint,&pd))
	{
		if(bRet = SetJob(hPrint,nJobId,0,NULL,JOB_CONTROL_PAUSE))
			theLog.Write("PauseJob :%d 成功",nJobId);
		else
			theLog.Write("PauseJob:%d 失败：%d",nJobId,GetLastError());

		ClosePrinter(hPrint);
	}	
	sName.ReleaseBuffer();
	return bRet;
}
void CPrinterJobHelper::RemoveSecJob(int nJobId)
{
	JOB_INFO_2			*pJobStorage = NULL;
	PRINTER_INFO_2		*pPrinterInfo = NULL;

	DWORD	cByteNeeded, nReturned=0, cByteUsed, i;

	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(SEC_PRINTER_NAME,&hPrint,&pd))
	{
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

	//-----------------------
	//删除文件
	CString sPath;
	CString sDir;
	CRegistry2 reg(2,TRUE);
	if(reg.Open(SEC_PRINTER_KEY_PATH))
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
void CPrinterJobHelper::RemoveJob(int nJobId)
{
	JOB_INFO_2			*pJobStorage = NULL;
	PRINTER_INFO_2		*pPrinterInfo = NULL;

	DWORD	cByteNeeded, nReturned=0, cByteUsed, i;

	HANDLE hPrint;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(IST_PRINTER_NAME,&hPrint,&pd))
	{
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
//////////////////////////////////////////////////////////////////////////
//=================================================================
//打印作业信息
CJobInfo::CJobInfo( const int nJobId )
{
	m_nJobId              = nJobId;       
	m_timeSubmitted.wHour = USHRT_MAX; // just some invalid value
	m_nTotalPages         = 0;
	m_nPagesPrinted       = 0;
	m_nTotalBytes         = 0;
	m_nBytesPrinted       = 0;

	// since m_mapJobStatus is used by all instances, we only want to populate it once
	if (m_mapJobStatus.IsEmpty())
	{
		m_mapJobStatus.SetAt(JOB_STATUS_BLOCKED_DEVQ, _T("The driver cannot print the job."));
		//        m_mapJobStatus.SetAt(JOB_STATUS_COMPLETE, _T("Job is sent to the printer, but the job may not be printed yet."));
		m_mapJobStatus.SetAt(JOB_STATUS_DELETED, _T("Job has been deleted."));
		m_mapJobStatus.SetAt(JOB_STATUS_DELETING, _T("Job is being deleted."));
		m_mapJobStatus.SetAt(JOB_STATUS_ERROR, _T("An error is associated with the job."));
		m_mapJobStatus.SetAt(JOB_STATUS_OFFLINE, _T("Printer is offline."));
		m_mapJobStatus.SetAt(JOB_STATUS_PAPEROUT, _T("Printer is out of paper."));
		m_mapJobStatus.SetAt(JOB_STATUS_PAUSED, _T("Job is paused."));
		m_mapJobStatus.SetAt(JOB_STATUS_PRINTED, _T("Job has printed."));
		m_mapJobStatus.SetAt(JOB_STATUS_PRINTING, _T("Job is printing."));
		m_mapJobStatus.SetAt(JOB_STATUS_RESTART, _T("Job has been restarted."));
		m_mapJobStatus.SetAt(JOB_STATUS_SPOOLING, _T("Job is spooling."));
		m_mapJobStatus.SetAt(JOB_STATUS_USER_INTERVENTION, _T("Printer has an error that requires the user to do something."));
	}
}

CMap<int, int, LPCTSTR, LPCTSTR> CJobInfo::m_mapJobStatus;    

//=================================================================

void CJobInfo::UpdateInfo( const PPRINTER_NOTIFY_INFO_DATA pNotifyData )
{
	theLog.Write("UpdateInfo %d",pNotifyData->Field);
	if (pNotifyData->Field == JOB_NOTIFY_FIELD_USER_NAME)
		m_strUser = (LPCTSTR) pNotifyData->NotifyData.Data.pBuf;
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_MACHINE_NAME)
		m_strMachine = (LPCTSTR) pNotifyData->NotifyData.Data.pBuf;
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_PORT_NAME)
		m_strPort = (LPCTSTR) pNotifyData->NotifyData.Data.pBuf;
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_DOCUMENT)
		m_strDocument = (LPCTSTR) pNotifyData->NotifyData.Data.pBuf;
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_SUBMITTED)
		m_timeSubmitted = *((SYSTEMTIME *) pNotifyData->NotifyData.Data.pBuf);
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_PAGES_PRINTED)
		m_nPagesPrinted = pNotifyData->NotifyData.adwData[0];
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_TOTAL_PAGES)
		m_nTotalPages = pNotifyData->NotifyData.adwData[0];
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_BYTES_PRINTED)
		m_nBytesPrinted = pNotifyData->NotifyData.adwData[0];
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_TOTAL_BYTES)
		m_nTotalBytes = pNotifyData->NotifyData.adwData[0];
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_STATUS)
		m_nStatus = pNotifyData->NotifyData.adwData[0]; 
	else if (pNotifyData->Field == JOB_NOTIFY_FIELD_STATUS_STRING)
		;
	else
		TRACE(_T("Unhandled job field: %x\n"), pNotifyData->Field);
}


int CJobInfo::GetJobId( void ) const
{
	return m_nJobId;
}

LPCTSTR CJobInfo::GetUser( void ) const
{
	return m_strUser;
}

LPCTSTR CJobInfo::GetMachine( void ) const
{
	return m_strMachine;
}

LPCTSTR CJobInfo::GetPort( void ) const
{
	return m_strPort;
}

LPCTSTR CJobInfo::GetDocument( void ) const
{
	return m_strDocument;
}

SYSTEMTIME CJobInfo::GetTimeSubmitted( void ) const
{
	return m_timeSubmitted;
}

int CJobInfo::GetTotalPages( void ) const
{
	return m_nTotalPages;
}

int CJobInfo::GetPagesPrinted( void ) const
{
	return m_nPagesPrinted;
}

int CJobInfo::GetTotalBytes( void ) const
{                
	return m_nTotalBytes;
}

int CJobInfo::GetBytesPrinted( void ) const
{
	return m_nBytesPrinted;
}

LPCTSTR CJobInfo::GetStatus( void ) const
{
	LPCTSTR lpszStatus = NULL;

	m_mapJobStatus.Lookup(m_nStatus, lpszStatus);

	return lpszStatus;
}

//////////////////////////////////////////////////////////////////////////

CPrinterMonitor::CPrinterMonitor()
{
	m_pCallBack = 0;
	m_hThread = 0;
	m_dwThreadId = 0;
	m_hEventStopRequested= 0;
	m_pCallBack = 0;
	m_bLogging = FALSE;
	m_bFilter = FALSE;
	m_bXabPrinterAcitve = FALSE;
	m_hPrintHookDll = NULL;
}
void CPrinterMonitor::Release()
{
	for (int n = 0; n < m_oFltAry.GetCount();n ++)
	{
		CPrinterFilterNode* p = m_oFltAry.GetAt(n);
		if (p)
		{
			delete p;
		}
	}
	m_oFltAry.RemoveAll();
}
CPrinterMonitor::~CPrinterMonitor()
{
	StopMonitor();
	Release();
}
void CPrinterMonitor::EnumeratePrinters( void )
{
	DWORD dwNeeded = 0, dwReturned = 0;
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, NULL, 0, &dwNeeded, &dwReturned);
	if (dwNeeded <= 0)
	{
		theLog.Write("dwNeeded is too small");
		return;
	}

	LPBYTE lpBuffer = new BYTE[dwNeeded];
	if (!lpBuffer)
	{
		theLog.Write("new BYTE error!");
		return;
	}

	if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, lpBuffer, dwNeeded, &dwNeeded, &dwReturned))
	{
		theLog.Write("EnumPrinters2 error!");
		delete lpBuffer;
		return;
	}

	PPRINTER_INFO_1 p1 = (PPRINTER_INFO_1) lpBuffer;
	for (DWORD x = 0; x < dwReturned; x++)
	{
		CString s = p1->pName;
		if(s.CompareNoCase(SEC_PRINTER_NAME) != 0)
		{
			m_aryPrinter.Add(p1->pName);
		}
		p1++;
	}
	delete lpBuffer;
}

void CPrinterMonitor::StartMonitor(CString sPrinter)
{
	// 开启文件后缀名显示
	BanCommand::SetFileExtShow(TRUE);

	// 开启打印监管机制
	if (m_hThread&& m_hEventStopRequested)
	{
		theLog.Write("StartMonitor %s",sPrinter);
		return ;
	}
	//监视所有
	if (sPrinter.IsEmpty())
	{
		if(m_aryPrinter.GetCount() < 1)
		{
			EnumeratePrinters();
		}
		if(m_aryPrinter.GetCount() > 0)
		{
			m_aryKeepJob.RemoveAll();
			for (int n = 0; n < m_aryPrinter.GetCount(); n++)
			{
				CString s = m_aryPrinter.GetAt(n);
				HANDLE hPrinter = NULL;
				if(OpenPrinter((LPTSTR) (LPCTSTR) s, &hPrinter, NULL) && hPrinter )
				{
					m_aryPrinterHandle.Add(hPrinter);
#if PRT_KEEP_JOB
					BOOL bKeep = FALSE;
					CPrinterJobHelper::KeepJob(hPrinter,TRUE,bKeep);
					m_aryKeepJob.Add(bKeep);
#endif
				}
			}

			if(m_aryPrinterHandle.GetCount() > 0)
			{
				Enable();
			}
		}
	}
	//只监视xabvprinter
	else
	{
		m_bXabPrinterAcitve = TRUE;
		m_aryPrinter.RemoveAll();
		m_aryPrinterHandle.RemoveAll();
		m_aryPrinter.Add(sPrinter);
		HANDLE hPrinter = NULL;
		if(OpenPrinter((LPTSTR) (LPCTSTR) sPrinter, &hPrinter, NULL) && hPrinter )
		{
			m_aryPrinterHandle.Add(hPrinter);
		}
		if(m_aryPrinterHandle.GetCount() > 0)
		{
			Enable();
		}
	}
	CJobInfoMap::GetInstance().Cleanup();
}

void CPrinterMonitor::StopMonitor()
{
	Enable(FALSE);
	m_aryPrinter.RemoveAll();
	for(int n = 0; n < m_aryPrinterHandle.GetCount(); n++)
	{
#if PRT_KEEP_JOB
		if (!m_aryKeepJob.GetAt(n))
		{
			BOOL b = 0;
			CPrinterJobHelper::KeepJob(m_aryPrinterHandle.GetAt(n),FALSE,b);
		}
#endif
		ClosePrinter(m_aryPrinterHandle.GetAt(n));
	}
	m_aryPrinterHandle.RemoveAll();
	m_aryKeepJob.RemoveAll();
	CJobInfoMap::GetInstance().Cleanup();
}

DWORD WINAPI CPrinterMonitor::ThreadFunc( void* pVoid)
{
	CPrinterMonitor* pThis = (CPrinterMonitor*)pVoid;

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
			theLog.Write("FindFirstPrinterChangeNotification err");
		}
	}
	pHandle[dwCount - 1] = pThis->m_hEventStopRequested;

	while(1)
	{
		DWORD dwRet = WaitForMultipleObjects(dwCount, pHandle, FALSE, INFINITE);
		if(dwRet == (WAIT_OBJECT_0 + dwCount - 1))
		{
			theLog.Write("printer monitor stop");
			break;
		}
		else if(dwRet >= WAIT_OBJECT_0 && dwRet < (WAIT_OBJECT_0 + dwCount - 1))
		{
			FindNextPrinterChangeNotification(pHandle[dwRet], &dwChange, &NotificationOptions, (LPVOID *) &pNotification);
			theLog.Write("dwChange %d,%X",dwChange,pNotification);
			if (pNotification != NULL)
			{
				if((dwChange & PRINTER_CHANGE_ADD_JOB) == PRINTER_CHANGE_ADD_JOB)
				{
					theLog.Write("PRINTER_CHANGE_ADD_JOB");
					CJobInfo *pJobInfo = NULL;
					DWORD nJobId = 0;
					for (DWORD x = 0; x < pNotification->Count; x++)
					{
						if(pNotification->aData[x].Type == JOB_NOTIFY_TYPE)
						{
							nJobId = pNotification->aData[x].Id;
														
#if 0
							if (! CJobInfoMap::GetInstance().Lookup(pNotification->aData[x].Id, pJobInfo))
							{
								pJobInfo = new CJobInfo(pNotification->aData[x].Id);

								CJobInfoMap::GetInstance().SetAt(pNotification->aData[x].Id, pJobInfo);
							}
							ASSERT(pJobInfo != NULL);
							pJobInfo->UpdateInfo(&pNotification->aData[x]);
#endif
							if(pNotification->aData[x].Field == JOB_NOTIFY_FIELD_STATUS)
							{
								DWORD dwStatus = pNotification->aData[x].NotifyData.adwData[0];
								theLog.Write("printer job id = %d,%x",nJobId,dwStatus);
								if (JOB_STATUS_PRINTING & dwStatus ||
									JOB_STATUS_SPOOLING & dwStatus ||
									dwStatus == 0)
								{
									pThis->OnJobCreating(dwRet,nJobId);
									break;
								}
							}
						}
					}
					//ASSERT(pJobInfo);
					//pThis->OnJobCreated(pJobInfo);
				}
				else if((dwChange & PRINTER_CHANGE_SET_JOB) == PRINTER_CHANGE_SET_JOB)
				{
					theLog.Write("PRINTER_CHANGE_SET_JOB");
					for (DWORD x = 0; x < pNotification->Count; x++)
					{
						if(pNotification->aData[x].Type == JOB_NOTIFY_TYPE)
						{
							DWORD dw = pNotification->aData[x].Id;
							//pThis->OnJobCreated(dwRet,dw);

							theLog.Write("printer job id = %d,%X",dw,pNotification->aData[x].Field);
							if (pNotification->aData[x].Field == JOB_NOTIFY_FIELD_STATUS)
							{
								theLog.Write("jibid = %d,status %d",dw, pNotification->aData[x].NotifyData.adwData[0]); 
								DWORD dwStatus = pNotification->aData[x].NotifyData.adwData[0];
								if ((dwStatus & JOB_STATUS_COMPLETE) || (dwStatus & JOB_STATUS_PRINTED))
								{
									theLog.Write("printjob suc %d",dw);
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

void CPrinterMonitor::Enable(BOOL bEnable /*= TRUE*/)
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
			m_hEventStopRequested = NULL;
			m_hThread = NULL;
		}
	}	
}

void CPrinterMonitor::OnJobCreated(CJobInfo* pJob)
{
	if(m_pCallBack)
	{
		//m_pCallBack->OnPrinted(pJob);
	}
}
void CPrinterMonitor::OnJobCreating(int nPrtIdx,DWORD dwId)
{
	CString sname = m_aryPrinter.GetAt(nPrtIdx);
	if(sname.CompareNoCase(IST_PRINTER_NAME) == 0 || sname.CompareNoCase(SEC_PRINTER_NAME) == 0 )
	{
		return ;
	}
	CPrinterJobHelper hepler;
	JOB_INFO_2* pJob = 0;
	int nMode = -1;
	CString sTips = "";
	BOOL bAllow = TRUE;
	if(hepler.PauseJob(sname,dwId))
	{
		::Sleep(10);
		if(hepler.GetJobInfo(sname.GetBuffer(),dwId,pJob) && pJob)
		{
			if(OnFilter(sname,pJob,nMode,sTips) && nMode == 1)
			{
				hepler.RemoveJob(sname,dwId);
				bAllow = FALSE;
			}
			else
				hepler.ResumeJob(sname,dwId);

			if (m_pCallBack)
			{
				m_pCallBack->OnPrintedEvent(pJob,bAllow);
				if (nMode == 0 || nMode == 1)
				{
					if (sTips.IsEmpty())
						sTips.Format("违反管理员设置的打印控制策略！");

					CString sMsg;
					if (nMode)
						sMsg.Format("%s 禁止打印机：%s 打印文档:%s",sTips,pJob->pPrinterName,pJob->pDocument);
					else
						sMsg.Format("%s 使用打印机：%s 打印文档:%s非法",sTips,pJob->pPrinterName,pJob->pDocument);
					m_pCallBack->OnTipMsg(TM_ONLIY_STR,sMsg,0,0);
				}
			}

			delete pJob;
		}
		else
		{
			hepler.ResumeJob(sname,dwId);
		}
		sname.ReleaseBuffer();
	}
}

void CPrinterMonitor::OnJobCreated(int nPrtIdx,DWORD dwId)
{
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
	else if (sname.CompareNoCase(SEC_PRINTER_NAME) == 0)
	{
		OutGoingLog log;
		theLog.Write("OnJobCreated %s,%d",sname,dwId);
		BOOL b = hepler.GetJobInfo(sname.GetBuffer(),dwId,log);
		sname.ReleaseBuffer();
		if(m_pCallBack)
		{
			theLog.Write("hepler.GetJobInfo %d",b);
			if(b)
			{
				m_pCallBack->OnSecDocPrinted(&log);
			}
			else
			{
				ASSERT(0);
			}
		}
	}
	else
	{
		return;
		JOB_INFO_2* pJob = 0;
		if(hepler.GetJobInfo(sname.GetBuffer(),dwId,pJob) && pJob)
		{
			if (m_pCallBack)
			{
				m_pCallBack->OnPrintedEvent(pJob,TRUE);
			}
			delete pJob;
		}
		sname.ReleaseBuffer();
	}
}

BOOL CPrinterMonitor::OnFilter(CString& sPrinter,JOB_INFO_2* pJob,int& nMode,CString& sTips)
{
	BOOL bFilter = FALSE;
	CString sDocName = pJob->pDocument;
	sDocName.MakeLower();
	sPrinter.MakeLower();

	for (int n =0 ; n < m_oFltAry.GetCount(); n++)
	{
		CPrinterFilterNode* node = m_oFltAry.GetAt(n);
		if (node)
		{
			CString sTemp;
			CStringArray aryPrinter;
			CCommonFun::StringSplit(node->m_sPrinter,&aryPrinter,",");
			if (aryPrinter.GetSize() == 0)
			{
				sTemp = "";
				aryPrinter.Add(sTemp);
			}

			for (int m = 0 ; m < aryPrinter.GetSize(); m++)
			{
				sTemp = aryPrinter.GetAt(m);
				if(sTemp.IsEmpty() || sPrinter.Find(sTemp) >= 0)
				{
					theLog.Write("printer filter by printername  %s,%s,%s",node->m_sPrinter,node->m_sKeyword,sDocName);
					CStringArray ary;
					CCommonFun::StringSplit(node->m_sKeyword,&ary,",");
					for (int i = 0; i < ary.GetCount(); i++)
					{
						CString sNode = ary.GetAt(i);
						sNode.TrimLeft();
						sNode.TrimRight();
						theLog.Write("printer filter node %s",sNode);
						if (sNode.IsEmpty())
						{
							continue;
						}
						if(sDocName.Find(sNode) >= 0)
						{
							bFilter=TRUE;
							nMode = node->m_nMode;
							sTips = node->m_sTipsMsg;
							theLog.Write("printer filter by keyword %s,%s,%s",node->m_sPrinter,node->m_sKeyword,sDocName);
							break;
						}
					}
				}
			}
			
		}
	}
#if 0
	for (int n = 0; n < m_nCount; n ++)
	{
		CltPrintControlPolicy* pVal = (CltPrintControlPolicy*)m_ppPolicy[n];
		for(int i =0 ;i < pVal->filterlist_size(); i++)
		{
			CltPrintControlPolicy_FilterNode* node = pVal->mutable_filterlist(i);
			if (node)
			{
				
				CString sName = node->printer().c_str();
				sName.MakeLower();
				sName.Replace("*","");
				
				CString sKey = node->key_word().c_str();
				sKey.MakeLower();
				sKey.Replace("*","");
				
				if(sPrinter.Find(sName) >= 0)
				{
					bFilter = TRUE;
					theLog.Write("printer filter by printername  %s,%s,%s",sName,sKey,sDocName);
				}
				else
				{
					CStringArray ary;
					CCommonFun::StringSplit(sKey,&ary,",");
					for (int i = 0; i < ary.GetCount(); i++)
					{
						if(sDocName.Find(ary.GetAt(i)) >= 0 )
						{
							bFilter=TRUE;
							theLog.Write("printer filter by keyword %s,%s,%s",sName,sKey,sDocName);
							break;
						}
					}
				}
			}
		}
	}
#endif
	return bFilter;
}

void CPrinterMonitor::StopLog()
{
	m_bLogging = FALSE;
	if (!m_bFilter)
	{
		StopMonitor();
	}
}

void CPrinterMonitor::StartLog()
{
	m_bLogging = TRUE;
	StartMonitor("");
}
void CPrinterMonitor::StartFilter()
{
	m_bFilter = TRUE;
	StartMonitor("");
}
void CPrinterMonitor::StopFilter()
{
	m_bFilter = FALSE;
	if (!m_bLogging)
	{
		StopMonitor();
	}
}
void CPrinterMonitor::SetPolicy(void** pAry,int nCount)
{	
	StopMonitor();
	Release();
	CltPrintControlPolicy* pVal = NULL;
	BOOL bEnabled = FALSE;
	if (!pAry)
	{
		typedef BOOL (*inshook)(PVOID,int); 
		inshook instkbhook = 0;
		theLog.Write("CPrinterMonitor SetPolicy From");
		if (m_hPrintHookDll == NULL)
		{
			
			m_hPrintHookDll = LoadLibrary((LPCTSTR)"PrintHookProc.dll");
			if (m_hPrintHookDll)
			{
				instkbhook = (inshook)GetProcAddress(m_hPrintHookDll, "SetPrintPolicy"); 
				if (instkbhook)
					instkbhook(0,0);	
			}
		}
		else
		{							
			instkbhook = (inshook)GetProcAddress(m_hPrintHookDll, "SetPrintDocType"); 
			if (instkbhook)
				instkbhook(0,0);
		}
		theLog.Write("CPrinterMonitor SetPolicy End");
	}
	else
	{
		for (int n = 0; n < nCount; n ++)
		{
			pVal = (CltPrintControlPolicy*)pAry[n];
			if (pVal && pVal->inuse() && CFilterMgr::GetInstance().CheckWithClaim(pVal->filter_type()))
			{				
				bEnabled = TRUE;
				int nSize = sizeof(CPrinterFilterNode)*pVal->filterlist_size();
				theLog.Write("nSize-%d,pValCount-%d",nSize,pVal->filterlist_size());
				PBYTE pDate = new BYTE[nSize];;
				CPrinterFilterNode* pNode = NULL;
				for (int i = 0;i <pVal->filterlist_size();i++)
				{
					pNode = (CPrinterFilterNode*)(pDate + sizeof(CPrinterFilterNode) * i);
					CltPrintControlPolicy_FilterNode* node = pVal->mutable_filterlist(i);
					if (node)
					{
						pNode->m_nType = node->type();
						strncpy(pNode->m_sKeyword,node->key_word().c_str(),MAX_PATH-1);
						strncpy(pNode->m_sPrinter,node->printer().c_str(),MAX_PATH-1);
						strncpy(pNode->m_sTipsMsg,pVal->hints().c_str(),MAX_PATH-1);
						CString(pNode->m_sPrinter).MakeLower();
						CString(pNode->m_sPrinter).Replace("*","");

						CString(pNode->m_sKeyword).MakeLower();
						CString(pNode->m_sKeyword).Replace("*","");

						pNode->m_nMode = pVal->filter_mode();
						m_oFltAry.Add(pNode);
						theLog.Write("print ctr %s,%s,%s",pNode->m_sPrinter,pNode->m_sKeyword,pNode->m_sTipsMsg);

						CFileSystemControl::GetInstance().HookSpoolsv(true);
					}

				}
				if (pDate)
				{
					typedef BOOL (*inshook)(PVOID,int); 
					inshook instkbhook;
					theLog.Write("CPrinterMonitor SetPolicy From");
					if (m_hPrintHookDll == NULL)
					{
						m_hPrintHookDll = LoadLibrary((LPCTSTR)"PrintHookProc.dll");
						if (m_hPrintHookDll)
							instkbhook = (inshook)GetProcAddress(m_hPrintHookDll, "SetPrintPolicy"); 
						if (instkbhook)
							instkbhook((PVOID)pDate,nSize);	
					}
					else
					{							
						instkbhook = (inshook)GetProcAddress(m_hPrintHookDll, "SetPrintDocType"); 
						if (instkbhook)
							instkbhook((PVOID)pDate,nSize);
					}
					SAFE_DELETE(pDate);
					theLog.Write("CPrinterMonitor SetPolicy End");
				}
				break;
			}
		}
	}
}
#endif //#if 0	//========================================================================================