#include "StdAfx.h"
#include "GhostScriptWrap.h"
#include "GhostScriptDef.h"

CGhostScriptWrap::CGhostScriptWrap(void)
{
}

CGhostScriptWrap::~CGhostScriptWrap(void)
{
}

BOOL CGhostScriptWrap::PdfToTiff(CString szPdfFilePath, CString szTiffFilePath, BOOL bColor /*= TRUE*/, int nDpi /*= Image_Deal_Max_Dpi*/, int nFirstPage /*= 0*/, int nLastPage /*= 0*/)
{
#ifdef UNICODE
	CStringA szPdfFilePathTemp = UnicodeToUTF8(szPdfFilePath);
	CStringA szTiffFilePathTemp = UnicodeToUTF8(szTiffFilePath);
#else
	CString szPdfFilePathTemp = MultiByteToUTF8(szPdfFilePath);
	CString szTiffFilePathTemp = MultiByteToUTF8(szTiffFilePath);
#endif // UNICODE

 	CGhostScript gs;
 	return gs.ExtractToTiff(szPdfFilePathTemp.GetBuffer(), szTiffFilePathTemp.GetBuffer(), bColor, nDpi, nFirstPage, nLastPage);
}

BOOL CGhostScriptWrap::PdfToTxt(CString szPdfFilePath, CString szTxtFilePath, int nFirstPage /*= 0*/, int nLastPage /*= 0*/)
{
#ifdef UNICODE
	CStringA szPdfFilePathTemp = UnicodeToUTF8(szPdfFilePath);
	CStringA szTxtFilePathTemp = UnicodeToUTF8(szTxtFilePath);
#else
	CString szPdfFilePathTemp = MultiByteToUTF8(szPdfFilePath);
	CString szTxtFilePathTemp = MultiByteToUTF8(szTxtFilePath);
#endif // UNICODE

 	CGhostScript gs;
 	return gs.ExtractToTxt(szPdfFilePathTemp.GetBuffer(), szTxtFilePathTemp.GetBuffer(), nFirstPage, nLastPage);
}

BOOL CGhostScriptWrap::PdfToPs(CString szPdfFilePath, CString szPsFilePath, int nFirstPage /*= 0*/, int nLastPage /*= 0*/)
{
#ifdef UNICODE
	CStringA szPdfFilePathTemp = UnicodeToUTF8(szPdfFilePath);
	CStringA szPsFilePathTemp = UnicodeToUTF8(szPsFilePath);
#else
	CString szPdfFilePathTemp = MultiByteToUTF8(szPdfFilePath);
	CString szPsFilePathTemp = MultiByteToUTF8(szPsFilePath);
#endif // UNICODE

 	CGhostScript gs;
 	return gs.ExtractToPs(szPdfFilePathTemp.GetBuffer(), szPsFilePathTemp.GetBuffer(), nFirstPage, nLastPage);
}

BOOL CGhostScriptWrap::PsToPdf(CString szPsFilePath, CString szPdfFilePath, int nFirstPage /*= 0*/, int nLastPage /*= 0*/)
{
#ifdef UNICODE
	CStringA szPsFilePathTemp = UnicodeToUTF8(szPsFilePath);
	CStringA szPdfFilePathTemp = UnicodeToUTF8(szPdfFilePath);
#else
	CString szPsFilePathTemp = MultiByteToUTF8(szPsFilePath);
	CString szPdfFilePathTemp = MultiByteToUTF8(szPdfFilePath);
#endif // UNICODE

 	CGhostScript gs;
 	return gs.ExtractToPdf(szPsFilePathTemp.GetBuffer(), szPdfFilePathTemp.GetBuffer(), nFirstPage, nLastPage);
}
