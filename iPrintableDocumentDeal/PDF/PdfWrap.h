#pragma once

/************************************************************************/
/* 
	CPdfWrap���ǵ�����PdfHelper.dll�Ľӿڣ�
	��PdfHelper.dll��Popplert��Cairo�İ�װ��⡣
*/
/************************************************************************/

class _AA_DLL_EXPORT_ CPdfWrap
{
public:
	CPdfWrap(void);
	~CPdfWrap(void);

	static int GetPdfPageCount(CString csPdfFilePath);
	static BOOL PDFToTiff(CString csPdfFilePath, CString csTiffFilePath, BOOL bColor, int nDpi = Image_Deal_Max_Dpi);

protected:
};
