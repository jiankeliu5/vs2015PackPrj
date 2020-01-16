// ScanMonitor.cpp: implementation of the CScanMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanMonitor.h"
#include "../Public/PdfHelperDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL CScanInfo::Attach(CString& sPath)
{
	m_sPath = sPath;
	m_sOrgFileName = PathFindFileName(sPath);
	COleDateTime dt = COleDateTime::GetCurrentTime();
	m_dt = dt.m_dt;
	theLog.Write(_T("CScanInfo::Attach,1,sPath=%s"), sPath);
	
	CFileStatus st;
	if(!CFile::GetStatus(sPath,st))
	{
		theLog.Write(_T("�ļ������� = %s"),sPath);
		return FALSE;
	}
	m_nSize = st.m_size;
	do 
	{
		if (sPath.GetLength() > 4 && sPath.Right(4).CompareNoCase(_T(".pdf")) == 0)
		{
			m_nColor = 2;
			m_nPageCount = 1;
			CPdfHelper pdf;
#ifdef UNICODE
			if (pdf.OpenFile(CCommonFun::UnicodeToUTF8(m_sPath).GetString()))
#else
			if (pdf.OpenFile(CCommonFun::MultiByteToUTF8(m_sPath).GetString()))
#endif
			{
				m_nPageCount = pdf.GetPageCount();
			}
			m_sZipFilePath = m_sPath;
			theLog.Write(_T("CScanInfo::Attach,2,PDF,m_nPageCount=%d"), m_nPageCount);
			return TRUE;
		}
		if (sPath.GetLength() > 4 && sPath.Right(4).CompareNoCase(_T(".xps")) == 0)
		{
			m_nColor = 2;
			m_nPageCount = 1;
			theLog.Write(_T("CScanInfo::Attach,3,XPS,m_nPageCount=%d"), m_nPageCount);
			break;
		}
		CStringW wsPath(sPath);
		Image image(wsPath);
		Status st = image.GetLastStatus();
		if (st != Gdiplus::Ok)
		{
			theLog.Write(_T("GetLastStatus = %s,%d"),sPath,st);
			return FALSE;
		}
		UINT count = 0;
		count = image.GetFrameDimensionsCount();
		PixelFormat pf = image.GetPixelFormat();
		if (pf == PixelFormat1bppIndexed || pf == PixelFormat4bppIndexed ||  pf == PixelFormat8bppIndexed)
		{
			m_nColor = 1;
		}
		else
		{
			m_nColor = 2;
		}
		theLog.Write(_T("color = %d,%d"),m_nColor,pf);
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
			TRACE("The first (and only) dimension ID is %s.", strGuid);

			// Get the number of frames in the first dimension.
			frameCount += image.GetFrameCount(&pDimensionIDs[n]);
		}
		free(pDimensionIDs);
		m_nPageCount = frameCount;
		theLog.Write(_T("CScanInfo::Attach,4,image,m_nPageCount=%d"), m_nPageCount);
	} while (0);
	m_sZipFilePath = m_sPath;
	/*
	if (CompressTif())
	{
		m_sFileName = PathFindFileName(m_sZipFilePath);
		return TRUE;
	}*/
	return TRUE;
}
#define SCAN_JOB_DIR _T("scan_job")
BOOL CScanInfo::CompressTif()
{
	theLog.Write(_T("CScanInfo::CompressTif,1"));
	CString sDestPath;
	m_sDir.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SCAN_JOB_DIR);
	sDestPath.Format(_T("%s%s\\%s%s"),CCommonFun::GetDefaultPath(),SCAN_JOB_DIR,CCommonFun::NewGuid(),PathFindExtension(m_sPath));
	CCommonFun::ValidatePath(sDestPath);
	theLog.Write(_T("tiff = %s,dest = %s"),m_sPath,sDestPath);
	if(!PathFileExists(m_sPath))
	{
		theLog.Write(_T("GetTiffFile %s not exist"),m_sPath);
	}
	if(MoveFile(m_sPath,sDestPath) && PathFileExists(sDestPath))
	{
		//ѹ��
		CFileHelper::CompressGzFile(sDestPath, m_sZipFilePath, TRUE);
		return TRUE;
	}
	theLog.Write(_T("!!CScanInfo::CompressTif,2,fail,err=%u"), GetLastError());
	return FALSE;
}
CScanMonitor::CScanMonitor()
{
	InterlockedExchange(&m_lActive,0);
	m_pCallback = 0;
	m_hShutdown = 0;
	m_hThread = 0;
	m_hThreadStarted = 0;
	m_aryData.Add(_T(".jpg"));
	m_aryData.Add(_T(".tif"));
	m_aryData.Add(_T(".tiff"));
	m_aryData.Add(_T(".pdf"));
}

CScanMonitor::~CScanMonitor()
{
	Stop();
	Shutdown();
}

void CScanMonitor::SetScanData(CString& s)
{
	m_aryData.RemoveAll();
	CCommonFun::StringSplit(s,&m_aryData,_T(","));
}

BOOL CScanMonitor::IsTiff(CString& sPath)
{
	CFileStatus st;
	if(!CFile::GetStatus(sPath,st))
		return FALSE;
	//
	for (int n = 0; n < m_aryData.GetSize(); n ++)
	{
		CString s = m_aryData.GetAt(n);
		if (sPath.GetLength() > s.GetLength())
		{
			if(sPath.Right(s.GetLength()).CompareNoCase(s) == 0)
			{
				return TRUE;
			}
		}
	}
	/*
	if (sPath.GetLength() > 4)
	{
		if(sPath.Right(4).CompareNoCase(_T(".tif")) == 0)
		{
			return TRUE;
		}
	}
	if (sPath.GetLength() > 5)
	{
		if(sPath.Right(5).CompareNoCase(_T(".tiff")) == 0)
		{
			return TRUE;
		}
	}*/

	return FALSE;
}

BOOL CScanMonitor::IsActive()
{
	long lVal = 0;
	InterlockedExchange(&lVal,m_lActive);
	return lVal;
}

void CScanMonitor::ReadChanges2(CString sDir)
{
	theLog.Write(_T("CScanMonitor::ReadChanges2,��ȡ�ı䣬Ŀ¼��(%s)��"), sDir);
	if(sDir.IsEmpty())
		return;

	OVERLAPPED ol = {0};
	ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	

	HANDLE hDir = CreateFile(sDir, 
		FILE_LIST_DIRECTORY, 
		FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
		NULL
		);

	//theLog.Writeld(_T("\t��Ŀ¼���:0x%x\n"), hDir);
	if(!hDir)
		return;

	DWORD cbBuf=1024, dwRet;
	BYTE* pBuf = (BYTE*)malloc(cbBuf);
	BOOL bLoop = TRUE;
	while(bLoop)
	{
		memset(pBuf, 0xFF, 1024);
		BOOL bRet = ReadDirectoryChangesW(hDir, 
			pBuf, 
			cbBuf, 
			TRUE, 
			FILE_NOTIFY_CHANGE_FILE_NAME|	//modify by zxl,20151228,��һЩ��ӡ��ɨ��Ĳ����У�����ɨ���ļ���a.bcd����Ȼ��������Ϊ(a.pdf).
			/*FILE_NOTIFY_CHANGE_DIR_NAME|*/
			FILE_NOTIFY_CHANGE_LAST_WRITE//|
			/*FILE_NOTIFY_CHANGE_LAST_ACCESS|*/
			/*FILE_NOTIFY_CHANGE_SIZE*/, 
			&dwRet, 
			&ol, 
			NULL);
		if(!bRet)
		{
		//	theLog.Writelw(_T("��ȡĿ¼�ı����, b:%d, err:%d\n"), bRet, GetLastError());
			break;
		}

		HANDLE h[2];
		h[0] = ol.hEvent;
		h[1] = m_hShutdown;
		DWORD dw = WaitForMultipleObjects(2, h, FALSE, INFINITE);
		switch(dw)
		{
		case WAIT_OBJECT_0:
			{
				BOOL bSuc = GetOverlappedResult(hDir, &ol, &dwRet, FALSE);
				if(bSuc)
				{
				//	theLog.Writeld(_T("��ȡĿ¼�ı�ɹ���ret:%d, bytes:%d"), bSuc, dwRet);

					if(dwRet == 0)	//������������
					{
						if(cbBuf < 1024*1024)
						{
							cbBuf*=2;
							pBuf = (BYTE*)realloc(pBuf, cbBuf);
							if(!pBuf)
								break;
						}
				//		theLog.Writelw(_T("��ȡ��СΪ0�����ڻ�������СΪ:%d��\n"), cbBuf);
						continue;
					}

					FILE_NOTIFY_INFORMATION* pInfo = (FILE_NOTIFY_INFORMATION*)pBuf;
					//DumpFNIBuffer(pInfo);
					WriteToMemFile(pInfo);
				}
				else
				{
					ASSERT(FALSE);
				//	theLog.Writelw(_T("��ȡĿ¼�ı�ʧ�ܡ�ret:%d, bytes:%d, err:%d"), bSuc, dwRet, GetLastError());
					bLoop = FALSE;
					break;
				}
			}
			break;
		case WAIT_OBJECT_0+1:
		//	theLog.Writeld(_T("���յ��ر��¼���"));
			bLoop = FALSE;
			break;
		default:
			ASSERT(FALSE);
			//theLog.Writelw(_T("�ȴ�ʧ�ܡ�dw:%d err:%d"), dw, GetLastError());
			break;
		}
		long lVal = 0;
		InterlockedExchange(&lVal,m_lActive);
		bLoop = lVal;
	}

	if(pBuf)
		free(pBuf);

	CloseHandle(ol.hEvent);
	CloseHandle(hDir);
	CloseHandle(m_hShutdown);
	m_hShutdown = NULL;
}

void CScanMonitor::DumpFNIBuffer(FILE_NOTIFY_INFORMATION* pInfo)
{
	ASSERT(pInfo);
	if(!pInfo)
		return;

	static PTSTR szAction[] = {
		_T(""), 
		_T("FILE_ACTION_ADDED"), 
		_T("FILE_ACTION_REMOVED"), 
		_T("FILE_ACTION_MODIFIED"), 
		_T("FILE_ACTION_RENAMED_OLD_NAME"), 
		_T("FILE_ACTION_RENAMED_NEW_NAME")};

	do 
	{
//		theLog.Writeld(_T("\tNext:%d"), pInfo->NextEntryOffset);
//		theLog.Writeld(_T("\tAction:%s"), szAction[pInfo->Action]);
//		theLog.Writeld(_T("\tNameLen:%d"), pInfo->FileNameLength);
//		CString s(pInfo->FileName, pInfo->FileNameLength/2);
//		theLog.Writeld(_T("\tName:%s"), s);

		if(pInfo->NextEntryOffset > 0)
			pInfo = (FILE_NOTIFY_INFORMATION*)(((BYTE*)pInfo)+pInfo->NextEntryOffset);
		else
			pInfo = NULL;
	} while(pInfo != NULL);
}

void CScanMonitor::WriteToMemFile(FILE_NOTIFY_INFORMATION* pInfo)
{
	ASSERT(pInfo);
	if(!pInfo)
		return;

//	CCriticalSection2::Owner lock(theCrit);
	BOOL bLoop = TRUE;
	do
	{
		if(pInfo->NextEntryOffset == 0)	//���һ����¼
		{
			bLoop = FALSE;
			pInfo->NextEntryOffset = sizeof(FILE_NOTIFY_INFORMATION)-4+pInfo->FileNameLength;
		}

		if(pInfo->Action == FILE_ACTION_MODIFIED || pInfo->Action == FILE_ACTION_RENAMED_NEW_NAME)	
		{
			CString sPath (pInfo->FileName, pInfo->FileNameLength/2);
			CString sFullPath;
			sFullPath.Format(_T("%s\\%s"),m_sDir,sPath);

			theLog.Write(_T("Add File %s"),CString(pInfo->FileName, pInfo->FileNameLength/2));
			if (m_pCallback && IsTiff(sFullPath))
			{
				if(IsActive())
				{
					Sleep(10);
					CScanInfo scan;
					if(scan.Attach(sFullPath))
					{
						m_pCallback->OnOneScanCmplt(&scan);
					}
					else
					{
						theLog.Write(_T("get scan file err %s"),sFullPath);
					}
				}
				else
				{
					theLog.Write(_T("OnOneScanCmplt is not active"));
				}
			}
		}
		if (pInfo->Action == FILE_ACTION_ADDED)
		{
			
		}
		else 
		{
			
//			theFile.SeekToEnd();
	//		theFile.Write(pInfo, pInfo->NextEntryOffset);
		}

		CString sPath (pInfo->FileName, pInfo->FileNameLength/2);
		CString sFullPath;
		sFullPath.Format(_T("%s\\%s"),m_sDir,sPath);
		theLog.Write(_T("ad %d,%s "),pInfo->Action,sFullPath);
		if(bLoop)
			pInfo = (FILE_NOTIFY_INFORMATION*)(((BYTE*)pInfo)+pInfo->NextEntryOffset);
	}while(bLoop);
}

DWORD CScanMonitor::ThreadFn(void* pVoid)
{
	CScanMonitor* pThis = (CScanMonitor*)pVoid;
	if (pThis->m_hThreadStarted)
	{
		SetEvent(pThis->m_hThreadStarted);
	}
	

	pThis->ReadChanges2(pThis->m_sDir);

	return 0;
}

BOOL CScanMonitor::Start(CString sDir)
{
	InterlockedExchange(&m_lActive,1);
	if(!m_hShutdown)
	{
		m_sDir = sDir;
		m_hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hThreadStarted = CreateEvent(0,0,0,0);

		DWORD dw = 0;
		m_hThread = ::CreateThread(0,0,ThreadFn,this,0,&dw);

		DWORD dwWait = WaitForSingleObject ( m_hThreadStarted , INFINITE );
	
		ASSERT ( dwWait ==	WAIT_OBJECT_0 );
		CloseHandle(m_hThreadStarted);
	}
	return TRUE;
}
void CScanMonitor::Shutdown()
{
	InterlockedExchange(&m_lActive,0);
	if (m_hShutdown && m_hThread)
	{
		SetEvent(m_hShutdown);
		DWORD dwWait = WaitForSingleObject ( m_hThread , INFINITE );
		CloseHandle(m_hThread);
		CloseHandle(m_hShutdown);
		m_hThread = 0;
		m_hShutdown = 0;
	}
}
void CScanMonitor::Stop()
{
	InterlockedExchange(&m_lActive,0);
	//�������ļ����������ļ�
	//theLog.Write(_T("del dir %s "),m_sDir);
	//CCommonFun::DeleteDir(m_sDir,TRUE);
}

void CScanMonitor::SetCallback(IScanChargeInterface* pCall)
{
	ASSERT(pCall);
	m_pCallback = pCall;
}
