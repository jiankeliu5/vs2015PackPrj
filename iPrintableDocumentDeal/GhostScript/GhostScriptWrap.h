#pragma once

/************************************************************************/
/* 
	CGhostScriptWrap���ǵ�����GhostScriptWrapLib.dll�Ľӿڣ�
	��GhostScriptWrapLib.dll��GhostScript�İ�װ��⡣
*/
/************************************************************************/

class _AA_DLL_EXPORT_ CGhostScriptWrap
{
public:
	CGhostScriptWrap(void);
	~CGhostScriptWrap(void);

	//���нӿڣ�PdfToTiff��PdfToTxt��PdfToPs��PsToPdf�Ĳ���nFirstPage��nLastPage��
	//��ʾת����Χ��ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��

	static BOOL PdfToTiff(CString szPdfFilePath, CString szTiffFilePath, BOOL bColor = TRUE, int nDpi = Image_Deal_Max_Dpi, int nFirstPage = 0, int nLastPage = 0);
	static BOOL PdfToTxt(CString szPdfFilePath, CString szTxtFilePath, int nFirstPage = 0, int nLastPage = 0);
	static BOOL PdfToPs(CString szPdfFilePath, CString szPsFilePath, int nFirstPage = 0, int nLastPage = 0);
	static BOOL PsToPdf(CString szPsFilePath, CString szPdfFilePath, int nFirstPage = 0, int nLastPage = 0);
};
