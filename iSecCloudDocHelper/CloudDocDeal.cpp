#include "StdAfx.h"
#include "CloudDocDeal.h"

CCloudDocDeal::CCloudDocDeal(void)
{
	m_pJob = NULL;
	m_szOrgJobPath.Empty();
	m_szJobPath.Empty();
	m_szDealSaveJobPath.Empty();
	m_szNewJobName.Empty();
	m_szNewJobPath.Empty();
	m_bNeedDelOrgFile = TRUE;
}

CCloudDocDeal::CCloudDocDeal(PCloudJobInfo job)
{
	SetJobInfo(job);
}

CCloudDocDeal::~CCloudDocDeal(void)
{
	Release();
}

void CCloudDocDeal::SetJobInfo(PCloudJobInfo job)
{
	m_pJob = job;
	if (m_pJob)
	{
		theLog.Write(_T("CCloudDocDeal::SetJobInfo,cDocName=%s, cJobDir=%s, cOrgJobName=%s")
			, m_pJob->cDocName, m_pJob->cJobDir, m_pJob->cOrgJobName);
		InitFilePath();
	}
	else
	{
		theLog.Write(_T("!!CCloudDocDeal::SetJobInfo, m_pJob=%p"), m_pJob);
	}
}

PCloudJobInfo CCloudDocDeal::GetJobInfo()
{
	return m_pJob;
}

void CCloudDocDeal::Release()
{
	if (m_pJob)
	{
		delete m_pJob;
		m_pJob = NULL;
	}
}

BOOL CCloudDocDeal::IsGzFile()
{
	BOOL bRet = FALSE;
	if (m_pJob)
	{
		CString szOrgJobName(m_pJob->cOrgJobName);
		if (szOrgJobName.Right(_tcsclen(Cloud_Doc_Format_Suffix_GZ)).CompareNoCase(Cloud_Doc_Format_Suffix_GZ) == 0)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CCloudDocDeal::IsImageFile()
{
	if ((m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_JPG)).CompareNoCase(Cloud_Doc_Format_Suffix_JPG) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_JPEG)).CompareNoCase(Cloud_Doc_Format_Suffix_JPEG) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_TIF)).CompareNoCase(Cloud_Doc_Format_Suffix_TIF) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_TIFF)).CompareNoCase(Cloud_Doc_Format_Suffix_TIFF) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_PNG)).CompareNoCase(Cloud_Doc_Format_Suffix_PNG) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_BMP)).CompareNoCase(Cloud_Doc_Format_Suffix_BMP) == 0)
		|| (m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_GIF)).CompareNoCase(Cloud_Doc_Format_Suffix_GIF) == 0)
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCloudDocDeal::IsPdfFile()
{
	if ((m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_PDF)).CompareNoCase(Cloud_Doc_Format_Suffix_PDF) == 0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCloudDocDeal::IsWordFile()
{
	if ((m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_Word)).CompareNoCase(Cloud_Doc_Format_Suffix_Word) == 0)
		||(m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_WordX)).CompareNoCase(Cloud_Doc_Format_Suffix_WordX) == 0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCloudDocDeal::IsPptFile()
{
	if ((m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_PowerPoint)).CompareNoCase(Cloud_Doc_Format_Suffix_PowerPoint) == 0)
		||(m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_PowerPointX)).CompareNoCase(Cloud_Doc_Format_Suffix_PowerPointX) == 0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCloudDocDeal::IsExcelFile()
{
	if ((m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_Excel)).CompareNoCase(Cloud_Doc_Format_Suffix_Excel) == 0)
		||(m_szJobPath.Right(_tcsclen(Cloud_Doc_Format_Suffix_ExcelX)).CompareNoCase(Cloud_Doc_Format_Suffix_ExcelX) == 0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCloudDocDeal::ConvertFile()
{
	BOOL bRet = FALSE;
	do 
	{
		if (!m_pJob)
		{
			theLog.Write(_T("!!CCloudDocDeal::ConvertFile,1,m_pJob=%p"), m_pJob);
			break;

		}
		if (!PathFileExists(m_szJobPath))
		{
			theLog.Write(_T("!!CCloudDocDeal::ConvertFile,2,PathFileExists fail,m_szJobPath=%s"), m_szJobPath);
			break;
		}

		DWORD dwCopies = m_pJob->nCopies;
		DWORD dwPaperSize = m_pJob->nPaperSize;
		DWORD dwOrientation = m_pJob->nOrientation;
		BOOL bColor = (m_pJob->nColor==DMCOLOR_COLOR) ? TRUE : FALSE;
		BOOL bImageUsePhysicalArea = GlobalData.bImageUsePhysicalArea;
		int nImageDealFormat = GlobalData.m_nImageDealFormatType;
		int nImageDealDpi = GlobalData.m_nImageDealFormatDpi;
		int nOfficeDealFormat = GlobalData.m_nOfficeDealFormatType;
		int nOfficeDealDpi = GlobalData.m_nOfficeDealFormatDpi;

		if (IsImageFile())
		{
			bRet = CPrintableDocumentDeal::PictureToPrintFile(m_szJobPath, m_szDealSaveJobPath, dwPaperSize, dwOrientation, bColor
				, (PrintFileFormat)nImageDealFormat, nImageDealDpi, bImageUsePhysicalArea, m_bNeedDelOrgFile);
			if (!bRet)
			{
				theLog.Write(_T("!!CCloudDocDeal::ConvertFile,3,PictureToPrintFile fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
				break;
			}
		}
		else if (IsWordFile())
		{
			if (nOfficeDealFormat == Cloud_Doc_Format_PDF)
			{
				bRet = CPrintableDocumentDeal::WordToPdf(m_szJobPath, m_szDealSaveJobPath, m_bNeedDelOrgFile);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,4,WordToPdf fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
			else
			{
				bRet = CPrintableDocumentDeal::WordToTiff(m_szJobPath, m_szDealSaveJobPath, bColor, nOfficeDealDpi, m_bNeedDelOrgFile);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,4,WordToTiff fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
		}
		else if (IsExcelFile())
		{
			if (nOfficeDealFormat == Cloud_Doc_Format_PDF)
			{
				bRet = CPrintableDocumentDeal::ExcelToPdf(m_szJobPath, m_szDealSaveJobPath, m_bNeedDelOrgFile, dwPaperSize, dwOrientation);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,5,ExcelToPdf fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
			else
			{
				bRet = CPrintableDocumentDeal::ExcelToTiff(m_szJobPath, m_szDealSaveJobPath, bColor, nOfficeDealDpi, m_bNeedDelOrgFile, dwPaperSize, dwOrientation);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,5,ExcelToTiff fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
		}
		else if (IsPptFile())
		{
			if (nOfficeDealFormat == Cloud_Doc_Format_PDF)
			{
				bRet = CPrintableDocumentDeal::PPTToPdf(m_szJobPath, m_szDealSaveJobPath, m_bNeedDelOrgFile);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,6,PPTToPdf fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
			else
			{
				bRet = CPrintableDocumentDeal::PPTToTiff(m_szJobPath, m_szDealSaveJobPath, bColor, nOfficeDealDpi, m_bNeedDelOrgFile);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,6,PPTToTiff fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
		}
		else if (IsPdfFile())
		{
			if (GlobalData.m_nOfficeDealFormatType == Cloud_Doc_Format_PDF)
			{
				CFile::Rename(m_szJobPath, m_szDealSaveJobPath);	//不转换pdf,将原来的pdf文件名重命名为转换后的文件名
				bRet = TRUE;
			}
			else
			{
				bRet = CPrintableDocumentDeal::PdfToTiff(m_szJobPath, m_szDealSaveJobPath, bColor, nOfficeDealDpi, m_bNeedDelOrgFile);
				if (!bRet)
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,7,PPTToTiff fail,m_szJobPath=%s,m_szDealSaveJobPath=%s"), m_szJobPath, m_szDealSaveJobPath);
					break;
				}
			}
		}	
		else
		{
			theLog.Write(_T("!!CCloudDocDeal::ConvertFile,8,fail,m_szJobPath=%s,m_bNeedDelOrgFile=%d"), m_szJobPath, m_bNeedDelOrgFile);
			if (m_bNeedDelOrgFile)
			{
				if (!DeleteFile(m_szJobPath))
				{
					theLog.Write(_T("!!CCloudDocDeal::ConvertFile,9,DeleteFile fail,m_szJobPath=%s,err=%u"), m_szJobPath, GetLastError());
				}
			}
			break;
		}

		DWORD dwDocPageCount = 0;
		if (IsImageFile())
		{
			CPrintableDocumentDeal::GetImagePageCount(m_szDealSaveJobPath, dwDocPageCount);
			theLog.Write(_T("CCloudDocDeal::ConvertFile,10.A,m_szDealSaveJobPath=%s,dwDocPageCount=%u,dwCopies=%u"), m_szDealSaveJobPath, dwDocPageCount, dwCopies);
		}
		else
		{
			if (nOfficeDealFormat == Cloud_Doc_Format_PDF)
			{
				CPrintableDocumentDeal::GetPdfPageCount(m_szDealSaveJobPath, dwDocPageCount);
				theLog.Write(_T("CCloudDocDeal::ConvertFile,10.B,m_szDealSaveJobPath=%s,dwDocPageCount=%u,dwCopies=%u"), m_szDealSaveJobPath, dwDocPageCount, dwCopies);
			}
			else
			{
				CPrintableDocumentDeal::GetImagePageCount(m_szDealSaveJobPath, dwDocPageCount);
				theLog.Write(_T("CCloudDocDeal::ConvertFile,10.C,m_szDealSaveJobPath=%s,dwDocPageCount=%u,dwCopies=%u"), m_szDealSaveJobPath, dwDocPageCount, dwCopies);
			}
		}
		m_pJob->nPageCount = dwDocPageCount;
		m_pJob->nTotalPage = dwCopies * dwDocPageCount;
		if (dwDocPageCount <= 0 || dwCopies <= 0)
		{
			theLog.Write(_T("!!CCloudDocDeal::ConvertFile,10.D,m_szDealSaveJobPath=%s,dwDocPageCount=%u,dwCopies=%u"), m_szDealSaveJobPath, dwDocPageCount, dwCopies);
			break;
		}

		if (GlobalData.m_bCloudDocDealAfterIsGz)
		{
			if (!CFileHelper::CompressGzFile2(m_szDealSaveJobPath, m_szNewJobPath, TRUE))
			{
				theLog.Write(_T("!!CCloudDocDeal::ConvertFile,11,fail,m_szJobPath=%s, m_szNewJobPath=%s")
					, m_szJobPath, m_szNewJobPath);
				break;
			}
		}

		//更新压缩包名称
		_tcscpy_s(m_pJob->cNewJobName, m_szNewJobName);
		m_pJob->nNewJobSize = CCommonFun::GetFileSizeByte(m_szNewJobPath);
		m_pJob->nNewJobIsGz = GlobalData.m_bCloudDocDealAfterIsGz;

		theLog.Write(_T("CCloudDocDeal::ConvertFile,12,succ,cOrgJobName=%s,nOrgJobSize=%d,cNewJobName=%s,nNewJobSize=%d,nNewJobIsGz=%d")
			, m_pJob->cOrgJobName, m_pJob->nOrgJobSize, m_pJob->cNewJobName, m_pJob->nNewJobSize, m_pJob->nNewJobIsGz);
		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

BOOL CCloudDocDeal::InitFilePath()
{
	BOOL bRet = FALSE;
	if (m_pJob)
	{
		CString szJobDir(m_pJob->cJobDir);
		szJobDir.Trim();
		szJobDir.Replace(Cloud_Doc_Path_Slash_Current_Reverse, Cloud_Doc_Path_Slash_Current);
		szJobDir.TrimRight(Cloud_Doc_Path_Slash_Current);
		CString szOrgJobName(m_pJob->cOrgJobName);
		szOrgJobName.Trim();
		if (IsGzFile())
		{
			m_bNeedDelOrgFile = TRUE;	//转换完成后，要删除解压后的原文档
			m_szOrgJobPath.Format(_T("%s%s%s"), szJobDir, Cloud_Doc_Path_Slash_Current, szOrgJobName);
			m_szJobPath = m_szOrgJobPath.Left(m_szOrgJobPath.GetLength()-_tcsclen(Cloud_Doc_Format_Suffix_GZ));
			if (!CFileHelper::UnCompressGzFile2(m_szOrgJobPath, m_szJobPath, FALSE))
			{
				theLog.Write(_T("!!CCloudDocDeal::InitFilePath,1,UnCompressGzFile fail,m_szOrgJobPath=%s, m_szJobPath=%s"), m_szOrgJobPath, m_szJobPath);
				m_szJobPath.Empty();
			}
		}
		else
		{
			m_bNeedDelOrgFile = TRUE;	//转换完成后，删除未压缩的原文档，否则可能会出现文档一直积压，浪费存储空间
			m_szJobPath.Format(_T("%s%s%s"), szJobDir, Cloud_Doc_Path_Slash_Current, szOrgJobName);
			m_szOrgJobPath = m_szJobPath;
		}

		if (m_szJobPath.GetLength()>0)
		{
			CString szOfficeDealFormatType = Cloud_Doc_Format_Suffix_TIF;	//默认是tif
			if (IsImageFile())
			{
				//szOfficeDealFormatType = Cloud_Doc_Format_Suffix_TIF;
			}
			else
			{
				if (GlobalData.m_nOfficeDealFormatType == Cloud_Doc_Format_PDF)
				{
					szOfficeDealFormatType =  Cloud_Doc_Format_Suffix_PDF;
				}
			}
			m_szNewJobName.Format(_T("%s%s%s"), GetFileNameByPathName(m_szJobPath), szOfficeDealFormatType
				, GlobalData.m_bCloudDocDealAfterIsGz ? Cloud_Doc_Format_Suffix_GZ : _T(""));
			m_szNewJobPath.Format(_T("%s%s%s"), szJobDir, Cloud_Doc_Path_Slash_Current, m_szNewJobName);
			m_szDealSaveJobPath = m_szJobPath + szOfficeDealFormatType;
			bRet = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CCloudDocDeal::InitFilePath,2,m_szJobPath=%s"), m_szJobPath);
			m_szNewJobName.Empty();
			m_szNewJobPath.Empty();
			m_szDealSaveJobPath.Empty();
			bRet = FALSE;
		}

		theLog.Write(_T("CCloudDocDeal::InitFilePath,3,m_szOrgJobPath=%s,m_szJobPath=%s,m_szDealSaveJobPath=%s,")
			_T("m_szNewJobName=%s,m_szNewJobPath=%s,m_bNeedDelOrgFile=%d")
			, m_szOrgJobPath, m_szJobPath, m_szDealSaveJobPath, m_szNewJobName, m_szNewJobPath, m_bNeedDelOrgFile);
	}
	else
	{
		theLog.Write(_T("!!CCloudDocDeal::InitFilePath,4,m_pJob=%p"), m_pJob);
	}

	return bRet;
}

CString CCloudDocDeal::GetFileNameByPathName(CString szPathName)
{
	if( szPathName.IsEmpty()) 
		return _T("");

	CStringArray sAry;
	CCommonFun::StringSplit(szPathName,&sAry,Cloud_Doc_Path_Slash_Current);
	if( sAry.GetSize() > 0 )
		return sAry.GetAt(sAry.GetSize()-1);

	return _T("");
}
