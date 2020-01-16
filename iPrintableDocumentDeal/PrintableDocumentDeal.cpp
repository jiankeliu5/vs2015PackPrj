#include "StdAfx.h"
#include "PrintableDocumentDeal.h"

CPrintableDocumentDeal::CPrintableDocumentDeal(void)
{
}

CPrintableDocumentDeal::~CPrintableDocumentDeal(void)
{
}

//获取PDF文档页数,成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::GetPdfPageCount(CString szPdfFilePath, DWORD &dwPageCount)
{
	dwPageCount = CPdfWrap::GetPdfPageCount(szPdfFilePath);
	theLog.Write(_T("CPrintableDocumentDeal::GetPdfPageCount,dwPageCount=%d,szPdfFilePath=%s"), dwPageCount, szPdfFilePath);
	return (dwPageCount>0) ? TRUE : FALSE;
}

//将pdf文件转换成tif文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::PdfToTiff(CString szPdfFilePath, CString szTiffFilePath, BOOL bColor /*= FALSE*/, int nDpi /*= Image_Deal_Max_Dpi*/, BOOL bDelPdf /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::PdfToTiff,szPdfFilePath=%s,szTiffFilePath=%s,bColor=%d,nDpi=%d,bDelPdf=%d")
		, szPdfFilePath, szTiffFilePath, bColor, nDpi, bDelPdf);

	BOOL bRet = CPdfWrap::PDFToTiff(szPdfFilePath, szTiffFilePath, bColor, nDpi);

	if (bDelPdf)
	{
		if (!DeleteFile(szPdfFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PDFToTiff,DeleteFile fail,szPdfFilePath=%s,err=%u"), szPdfFilePath, GetLastError());
		}
	}

	return bRet;
}

//将PostScript文件转换成pdf文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::PsToPdf(CString szPsFilePath, CString szPdfFilePath, BOOL bDelPs /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::PsToPdf,szPsFilePath=%s,szPdfFilePath=%s,bDelPs=%d")
		, szPsFilePath, szPdfFilePath, bDelPs);

	BOOL bRet = CGhostScriptWrap::PsToPdf(szPsFilePath, szPdfFilePath);

	if (bDelPs)
	{
		if (!DeleteFile(szPsFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PsToPdf,DeleteFile fail,szPsFilePath=%s,err=%u"), szPsFilePath, GetLastError());
		}
	}

	return bRet;
}

//将Word文件转换成PDF文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::WordToPdf(CString szWordFilePath, CString szPdfFilePath, BOOL bDelWord /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::WordToPdf,szWordFilePath=%s,szPdfFilePath=%s,bDelWord=%d")
		, szWordFilePath, szPdfFilePath, bDelWord);

	BOOL bRet = FALSE;
	do 
	{
		CWordOperate wo;
		wo.SetVisible(FALSE);
		if (!wo.Open(szWordFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::WordToPdf,1,Open szWordFilePath fail.[%s]"),szWordFilePath);
			break;
		}

		if (!wo.ExportDocumentAs(szPdfFilePath, wdExportFormatPDF))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::WordToPdf,2,ExportDocumentAs szPdfFilePath fail.[%s]"),szPdfFilePath);
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (bDelWord)
	{
		if (!DeleteFile(szWordFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::WordToPdf,3,DeleteFile fail,szWordFilePath=%s,err=%u"), szWordFilePath, GetLastError());
		}
	}

	return bRet;
}

//将Word文件转换成tif文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::WordToTiff(CString szWordFilePath, CString szTiffFilePath, BOOL bColor /*= FALSE*/, int nDpi /*= Image_Deal_Max_Dpi*/, BOOL bDelWord /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::WordToTiff,szWordFilePath=%s,szTiffFilePath=%s,bColor=%d,nDpi=%d,bDelWord=%d")
		, szWordFilePath, szTiffFilePath, bColor, nDpi, bDelWord);

	CString szPdfFilePath = szTiffFilePath + _T(".pdf");
	if (!WordToPdf(szWordFilePath, szPdfFilePath, bDelWord))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::WordToTiff,1,szWordFilePath=%s, szPdfFilePath=%s"), szWordFilePath, szPdfFilePath);
		return FALSE;
	}

	if (!PdfToTiff(szPdfFilePath, szTiffFilePath, bColor, nDpi, TRUE))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::WordToTiff,2,szPdfFilePath=%s, szTiffFilePath=%s, bColor=%d"), szPdfFilePath, szTiffFilePath, bColor);
		return FALSE;
	}

	return TRUE;
}

//获取Word文档页数,成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::GetWordPageCount(CString szWordFilePath, DWORD &dwPageCount)
{
	theLog.Write(_T("CPrintableDocumentDeal::GetWordPageCount,szWordFilePath=%s"), szWordFilePath);
	dwPageCount = 0;
	return FALSE;
}

//将Excel文件转换成PDF文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::ExcelToPdf(CString szExcelFilePath, CString szPdfFilePath, BOOL bDelExcel /*= FALSE*/
										, WORD dwPaperSize /*= DMPAPER_A4*/, WORD dwPageOrientation /*= DMORIENT_PORTRAIT*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::ExcelToPdf,szExcelFilePath=%s,szPdfFilePath=%s,bDelExcel=%d,dwPaperSize=%d,dwPageOrientation=%d")
		, szExcelFilePath, szPdfFilePath, bDelExcel, dwPaperSize, dwPageOrientation);

	BOOL bRet = FALSE;
	do 
	{
		CExcelOperate eo;
		eo.SetVisible(FALSE);
		if (!eo.Open(szExcelFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::ExcelToPdf,1,Open szExcelFilePath fail.[%s]"),szExcelFilePath);
			break;
		}

		//设置页面参数
		eo.SetPageParam((XlPaperSize)dwPaperSize, (XlPageOrientation)dwPageOrientation);

		if (!eo.ExportDocumentAs(szPdfFilePath, xlTypePDF))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::ExcelToPdf,2,ExportDocumentAs szPdfFilePath fail.[%s]"),szPdfFilePath);
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (bDelExcel)
	{
		if (!DeleteFile(szExcelFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::ExcelToPdf,3,DeleteFile fail,szExcelFilePath=%s,err=%u"), szExcelFilePath, GetLastError());
		}
	}

	return bRet;
}

//将Excel文件转换成tif文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::ExcelToTiff(CString szExcelFilePath, CString szTiffFilePath, BOOL bColor /*= FALSE*/, int nDpi /*= Image_Deal_Max_Dpi*/, BOOL bDelExcel /*= FALSE*/
										 , WORD dwPaperSize /*= DMPAPER_A4*/, WORD dwPageOrientation /*= DMORIENT_PORTRAIT*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::ExcelToTiff,szExcelFilePath=%s,szTiffFilePath=%s,bColor=%d,nDpi=%d,bDelExcel=%d")
		, szExcelFilePath, szTiffFilePath, bColor, nDpi, bDelExcel);

	CString szPdfFilePath = szTiffFilePath + _T(".pdf");
	if (!ExcelToPdf(szExcelFilePath, szPdfFilePath, bDelExcel, dwPaperSize, dwPageOrientation))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::ExcelToTiff,1,szWordFilePath=%s, szPdfFilePath=%s"), szExcelFilePath, szPdfFilePath);
		return FALSE;
	}

	if (!PdfToTiff(szPdfFilePath, szTiffFilePath, bColor, nDpi, TRUE))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::ExcelToTiff,2,szPdfFilePath=%s, szTiffFilePath=%s, bColor=%d"), szPdfFilePath, szTiffFilePath, bColor);
		return FALSE;
	}

	return TRUE;
}

//获取Excel文档页数,成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::GetExcelPageCount(CString szExcelFilePath, DWORD &dwPageCount)
{
	theLog.Write(_T("CPrintableDocumentDeal::GetExcelPageCount,szExcelFilePath=%s"), szExcelFilePath);
	dwPageCount = 0;
	return FALSE;
}

//将PPT文件转换成PDF文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::PPTToPdf(CString szPPTFilePath, CString szPdfFilePath, BOOL bDelPpt /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::PPTToPdf,szPPTFilePath=%s,szPdfFilePath=%s,bDelPpt=%d")
		, szPPTFilePath, szPdfFilePath, bDelPpt);

	BOOL bRet = FALSE;
	do 
	{
		CPPTOperate po;
		po.SetVisible(FALSE);
		if (!po.Open(szPPTFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PPTToPdf,1,Open szExcelFilePath fail.[%s]"),szPPTFilePath);
			break;
		}

		if (!po.ExportDocumentAs(szPdfFilePath, ppFixedFormatTypePDF))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PPTToPdf,2,ExportDocumentAs szPdfFilePath fail.[%s]"),szPdfFilePath);
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (bDelPpt)
	{
		if (!DeleteFile(szPPTFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PPTToPdf,3,DeleteFile fail,szPPTFilePath=%s,err=%u"), szPPTFilePath, GetLastError());
		}
	}

	return bRet;
}

//将PPT文件转换成tif文件,转换成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::PPTToTiff(CString szPPTFilePath, CString szTiffFilePath, BOOL bColor /*= FALSE*/, int nDpi /*= Image_Deal_Max_Dpi*/, BOOL bDelPpt /*= FALSE*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::PPTToTiff,szPPTFilePath=%s,szTiffFilePath=%s,bColor=%d,nDpi=%d,bDelPpt=%d")
		, szPPTFilePath, szTiffFilePath, bColor, nDpi, bDelPpt);

	CString szPdfFilePath = szTiffFilePath + _T(".pdf");
	if (!PPTToPdf(szPPTFilePath, szPdfFilePath, bDelPpt))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::PPTToTiff,1,szWordFilePath=%s, szPdfFilePath=%s"), szPPTFilePath, szPdfFilePath);
		return FALSE;
	}

	if (!PdfToTiff(szPdfFilePath, szTiffFilePath, bColor, nDpi, TRUE))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::PPTToTiff,2,szPdfFilePath=%s, szTiffFilePath=%s, bColor=%d"), szPdfFilePath, szTiffFilePath, bColor);
		return FALSE;
	}

	return TRUE;
}

//获取PPT文档页数,成功返回TRUE,否则返回FALSE
BOOL CPrintableDocumentDeal::GetPPTPageCount(CString szPPTFilePath, DWORD &dwPageCount)
{
	theLog.Write(_T("CPrintableDocumentDeal::GetPPTPageCount,szPPTFilePath=%s"), szPPTFilePath);
	dwPageCount = 0;
	return FALSE;
}

//获取Tif文档页数
BOOL CPrintableDocumentDeal::GetImagePageCount(CString szImageFilePath, DWORD &dwPageCount)
{
	theLog.Write(_T("CPrintableDocumentDeal::GetImagePageCount,1,szImageFilePath=[%s]"), szImageFilePath);
	CStringW wszImageFilePath(szImageFilePath);
	Image image(wszImageFilePath);
	GUID guid;
	Status s = image.GetFrameDimensionsList(&guid, 1);
	if (s == Ok)
	{
		dwPageCount = image.GetFrameCount(&guid);  // 取得图片页数
		return TRUE;
	}
	else
	{
		dwPageCount = 0;
		theLog.Write(_T("!!CPrintableDocumentDeal::GetImagePageCount,2,GetFrameDimensionsList fail,s=%d"), s);
		return FALSE;
	}
}

//预处理普通的图片文件,使其转换成正常打印的指定图片格式的文件
BOOL CPrintableDocumentDeal::PictureToPrintFile(CString szPicFilePath, CString szPrintFilePath, int nPapeType, int nPapeOrientation
							, BOOL bColor /*= FALSE*/, PrintFileFormat pff /*= PrintFileFormat_TIFF*/, int nMaxDpi /*= Image_Deal_Max_Dpi*/
							, BOOL bImageUsePhysicalArea /*= TRUE*/, BOOL bDelPic/* = FALSE*/
							, PicturePostion pp/* = PicturePostion_Cetter*/, PictureSize ps/* = PictureSize_Full*/)
{
	theLog.Write(_T("CPrintableDocumentDeal::PictureToPrintFile,1,szPicFilePath=[%s],szPrintFilePath=[%s],nPapeType=%d,nPapeOrientation=%d")
		_T(",bColor=%d,nMaxDpi=%d,bImageUsePhysicalArea=%d,bDelPic=%d,PrintFileFormat=%d,PicturePostion=%d,PictureSize=%d")
		, szPicFilePath, szPrintFilePath, nPapeType, nPapeOrientation, bColor, nMaxDpi, bImageUsePhysicalArea, bDelPic, pff, pp, ps);

	if (!PathFileExists(szPicFilePath))
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::PictureToPrintFile,2,PathFileExists is not exist.[%s]"),szPicFilePath);
		return FALSE;
	}

#ifdef _DEBUG
	DWORD dwTick1 = GetTickCount();
#endif

	CStringW wszPicFilePath(szPicFilePath);
	Image* image = new Image(wszPicFilePath);
#if 0	//add by zxl,20161121,忽略输入的方向参数nPapeOrientation，根据图片的的大小，自动识别打印方向
	nPapeOrientation = (image->GetWidth() > image->GetHeight()) ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;
#endif
	if (nPapeOrientation == DMORIENT_LANDSCAPE)
	{
		//CMYK的图片如果RotateFlip后，在下面的DrawImage会失败，所有先将CMYK转换成RGB
		PixelFormat format = image->GetPixelFormat();
		if ((format & PixelFormat32bppCMYK) == PixelFormat32bppCMYK)
		{
			Image* pTemp = ConvertCmykToRgb(image);
			if (pTemp)
			{
				delete image;
				image = pTemp;
			}
		}
		//横打图片，旋转90度
 		image->RotateFlip(Rotate90FlipNone);
	}
	CStringW wszPrintFilePath(szPrintFilePath);
	Bitmap *bmp = NULL;
	Graphics *graphics = NULL;
	ImageAttributes *pImgAtt = NULL;
	if (!bColor)
	{
		pImgAtt = new ImageAttributes();
		pImgAtt->SetColorMatrix(&grayMatrix, ColorMatrixFlagsSkipGrays, ColorAdjustTypeDefault);
	}
	Size CanvasSize;	//画布大小，图片的最终大小
	Size PhysicalSize;	//打印机物理区域大小，一般略大于可打印区域大小
	Size PrintSize;		//可打印区域大小
	Size ImageSize;	//图片实际大小
	Rect ImageDestRect;
	float fXDpi = nMaxDpi;
	float fYDpi = nMaxDpi;
	//根据图片和打印参数，获取图片的最佳打印参数
	GetImagePrintParam(*image, nPapeType, fXDpi, fYDpi, PhysicalSize, PrintSize, ImageSize, ImageDestRect, bImageUsePhysicalArea, pp, ps);
	CanvasSize = bImageUsePhysicalArea ? PhysicalSize : PrintSize;
	bmp = new Bitmap(CanvasSize.Width, CanvasSize.Height, PixelFormat24bppRGB);	//CanvasSize为图片大小,24位深
	bmp->SetResolution(fXDpi, fYDpi);
	graphics = new Graphics(bmp);
	SolidBrush WhiteBrush(Color::White);
 	graphics->FillRectangle(&WhiteBrush, 0, 0, CanvasSize.Width, CanvasSize.Height);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	BOOL bRet = TRUE;
 	Status s = graphics->DrawImage(image, ImageDestRect, 0, 0, ImageSize.Width, ImageSize.Height, UnitPixel, pImgAtt);
	if (s != Ok)
	{
		bRet = FALSE;
		theLog.Write(_T("!!CPrintableDocumentDeal::PictureToPrintFile,3,graphics->DrawImage fail.s=%d,err=%u,szPrintFilePath=%s")
			, s, GetLastError(), szPrintFilePath);
	}
	else
	{
		CLSID imageClsid;
		GetEncoderClsid(pff, &imageClsid);
		if (nPapeOrientation == DMORIENT_LANDSCAPE)
		{
			//横打图片，旋转270度
			bmp->RotateFlip(Rotate270FlipNone);
		}
		s = bmp->Save(wszPrintFilePath, &imageClsid);
		if (s != Ok)
		{
			bRet = FALSE;
			theLog.Write(_T("!!CPrintableDocumentDeal::PictureToPrintFile,4,bmp->Save fail.s=%d,err=%u,szPrintFilePath=%s")
				, s, GetLastError(), szPrintFilePath);
		}
	}
	delete bmp;
	delete graphics;
	delete image;
	if (pImgAtt)
	{
		delete pImgAtt;
	}

#ifdef _DEBUG
	DWORD dwTick2 = GetTickCount();
	theLog.Write(_T("#########CPrintableDocumentDeal::PictureToPrintFile,Time Ms = %u"), dwTick2-dwTick1);
#endif

	if (bDelPic)
	{
		if (!DeleteFile(szPicFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PictureToPrintFile,5,DeleteFile fail,szPicFilePath=%s,err=%u"), szPicFilePath, GetLastError());
		}
	}

	if (!bRet)
	{
		if (!DeleteFile(szPrintFilePath))
		{
			theLog.Write(_T("!!CPrintableDocumentDeal::PictureToPrintFile,6,DeleteFile fail,szPrintFilePath=%s,err=%u"), szPrintFilePath, GetLastError());
		}
	}

	return bRet;
}

//调整图片的打印位置
void CPrintableDocumentDeal::AdjustImagePostion(Size PhysicalSize, Size PrintSize, Size ImageSize, Rect& ImageDestRect
												  , BOOL bImageUsePhysicalArea, PicturePostion pp, PictureSize ps)
{
// 	theLog.Write(_T("CPrintableDocumentDeal::AdjustImagePostion,PhysicalSize.Width=%d,PhysicalSize.Height=%d,")
// 		_T("PrintSize.Width=%d,PrintSize.Height=%d,ImageSize.Width=%d,ImageSize.Height=%d,bImageUsePhysicalArea=%d,PicturePostion=%d,PictureSize=%d.")
// 		, PhysicalSize.Width, PhysicalSize.Height, PrintSize.Width, PrintSize.Height, ImageSize.Width, ImageSize.Height, bImageUsePhysicalArea, pp, ps);

#pragma message("!!!!!!!!!!!!!!CPrintableDocumentDeal::AdjustImagePostion,暂时只处理PicturePostion_Cetter和PictureSize_Full的情况，后续要完善")

	//以更大的Rate为等比标准缩放轴
	double fRateX = 1.0 * ImageSize.Width / PrintSize.Width;
	double fRateY = 1.0 * ImageSize.Height / PrintSize.Height;

	if (fRateX > fRateY)
	{
		ImageDestRect.Width = PrintSize.Width;
		ImageDestRect.Height = ImageSize.Height / fRateX;
	}
	else if (fRateX < fRateY)
	{
		ImageDestRect.Width = ImageSize.Width / fRateY;
		ImageDestRect.Height = PrintSize.Height;
	}
	else
	{
		ImageDestRect.Width = PrintSize.Width;
		ImageDestRect.Height = PrintSize.Height;
	}

	Size CanvasSize = bImageUsePhysicalArea ? PhysicalSize : PrintSize;	//画布大小，图片的最终大小
	ImageDestRect.X = (CanvasSize.Width - ImageDestRect.Width) / 2;
	ImageDestRect.Y = (CanvasSize.Height - ImageDestRect.Height) / 2;
}

//根据打印纸型和打印dpi，获取物理区域大小和打印区域图片大小
void CPrintableDocumentDeal::GetPageSize(int nPapeType, float fXDpi, float fYDpi, Size& PhysicalSize, Size& PrintSize)
{
// 	theLog.Write(_T("CPrintableDocumentDeal::GetPageSize, nPapeType=%d, fXDpi=%f, fYDpi=%f"), nPapeType, fXDpi, fYDpi);

	//参考【XabVprinter.gpd】文件里面的参数
	//默认为A4纸型，下面是以1200dpi为标准的图片大小
	int nPhysicalWidth = 9922;
	int nPhysicalHeight = 14032;
	int nPrintWidth = 9534;
	int nPrintHeight = 13644;
	
	switch (nPapeType)
	{
	case DMPAPER_LETTER:
	case DMPAPER_LETTERSMALL:
		nPhysicalWidth = 10200;
		nPhysicalHeight = 13200;
		nPrintWidth = 9812;
		nPrintHeight = 12812;
		break;
	case DMPAPER_TABLOID:
		nPhysicalWidth = 13200;
		nPhysicalHeight = 20400;
		nPrintWidth = 12812;
		nPrintHeight = 20012;
		break;
	case DMPAPER_LEDGER:
		nPhysicalWidth = 20400;
		nPhysicalHeight = 13200;
		nPrintWidth = 20012;
		nPrintHeight = 12812;
		break;
	case DMPAPER_LEGAL:
		nPhysicalWidth = 10200;
		nPhysicalHeight = 16800;
		nPrintWidth = 9812;
		nPrintHeight = 16412;
		break;
	case DMPAPER_STATEMENT:
		nPhysicalWidth = 6600;
		nPhysicalHeight = 10200;
		nPrintWidth = 6212;
		nPrintHeight = 9812;
		break;
	case DMPAPER_EXECUTIVE:
		nPhysicalWidth = 8703;
		nPhysicalHeight = 12600;
		nPrintWidth = 8315;
		nPrintHeight = 12212;
		break;
	case DMPAPER_A3:
		nPhysicalWidth = 14032;
		nPhysicalHeight = 19843;
		nPrintWidth = 13644;
		nPrintHeight = 19455;
		break;
	case DMPAPER_A4:
	case DMPAPER_A4SMALL:
		nPhysicalWidth = 9922;
		nPhysicalHeight = 14032;
		nPrintWidth = 9534;
		nPrintHeight = 13644;
		break;
	case DMPAPER_A5:
		nPhysicalWidth = 6993;
		nPhysicalHeight = 9922;
		nPrintWidth = 6605;
		nPrintHeight = 9534;
		break;
	case DMPAPER_B4:
		nPhysicalWidth = 12142;
		nPhysicalHeight = 17197;
		nPrintWidth = 11754;
		nPrintHeight = 16809;
		break;
	case DMPAPER_B5:
		nPhysicalWidth = 8599;
		nPhysicalHeight = 12142;
		nPrintWidth = 8211;
		nPrintHeight = 11754;
		break;
	case DMPAPER_FOLIO:
		nPhysicalWidth = 10200;
		nPhysicalHeight = 15600;
		nPrintWidth = 9812;
		nPrintHeight = 15212;
		break;
	case DMPAPER_QUARTO:
		nPhysicalWidth = 10157;
		nPhysicalHeight = 12992;
		nPrintWidth = 9769;
		nPrintHeight = 12604;
		break;
	case DMPAPER_10X14:
		nPhysicalWidth = 10157;
		nPhysicalHeight = 12992;
		nPrintWidth = 9769;
		nPrintHeight = 12604;
		break;
	case DMPAPER_11X17:
		nPhysicalWidth = 13200;
		nPhysicalHeight = 20400;
		nPrintWidth = 12812;
		nPrintHeight = 20012;
		break;
	case DMPAPER_NOTE:
		nPhysicalWidth = 10200;
		nPhysicalHeight = 13200;
		nPrintWidth = 9812;
		nPrintHeight = 12812;
		break;
	default:
		break;
	}

	float fXRatio = fXDpi / 1200.0;
	float fYRatio = fYDpi / 1200.0;
	PhysicalSize.Width = nPhysicalWidth * fXRatio;
	PhysicalSize.Height = nPhysicalHeight * fYRatio;
	PrintSize.Width = nPrintWidth * fXRatio;
	PrintSize.Height = nPrintHeight * fYRatio;
}

//根据图片和最大可打印区域，获取最佳的打印dpi
void CPrintableDocumentDeal::GetImagePrintDpi(Image& image, Size& MaxPrintSize, float& fXMaxDpi, float& fYMaxDpi)
{
	int fMaxDpi = fXMaxDpi;	//默认X和Y的dpi是相同的,这里保存图片最大的可能dpi。
	fXMaxDpi = image.GetHorizontalResolution();
	fYMaxDpi = image.GetVerticalResolution();
// 	theLog.Write(_T("CPrintableDocumentDeal::GetImagePrintDpi,1,fXMaxDpi=%f,fYMaxDpi=%f,Image_Deal_Max_Dpi=%f,MaxPrintSize.Width=%d,MaxPrintSize.Height=%d")
// 		, fXMaxDpi, fYMaxDpi, Image_Deal_Max_Dpi, MaxPrintSize.Width, MaxPrintSize.Height);

	float fXYMaxDpi = max(fXMaxDpi, fYMaxDpi);
	if (fXYMaxDpi > fMaxDpi)
	{
		if (fXMaxDpi > fYMaxDpi)
		{
			fYMaxDpi = fYMaxDpi / fXMaxDpi * fMaxDpi;
			fXMaxDpi = fMaxDpi;
		}
		else
		{
			fXMaxDpi = fXMaxDpi / fYMaxDpi * fMaxDpi;
			fYMaxDpi = fMaxDpi;
		}
	}
	else
	{
		//尽可能最大限度保留图片的原有像素信息。
		float fRateWidth = 1.0 * MaxPrintSize.Width / image.GetWidth();
		float fRateHeight = 1.0 * MaxPrintSize.Height / image.GetHeight();
		float fRateMin = min(fRateWidth, fRateHeight);
		if (fRateMin < 1.0)
		{//原始图片大小大于最佳可打印区域大小，则将图片的dpi调整为合适dpi，以缩小图片的大小来适应可打印区域，图片的dpi可能变大
			fXMaxDpi = fMaxDpi * fRateMin;
			fYMaxDpi = fMaxDpi * fRateMin; 
		}
		else
		{//原始图片大小小于最佳可打印区域大小，则将图片的dpi调整为合适dpi，以放大图片的大小来适应可打印区域, 图片的dpi可能变大
			fXMaxDpi = fMaxDpi / fRateMin;
			fYMaxDpi = fMaxDpi / fRateMin;

// 			if (fXMaxDpi < image.GetHorizontalResolution())
// 			{
// 				fXMaxDpi = image.GetHorizontalResolution();
// 			}
// 
// 			if (fYMaxDpi < image.GetVerticalResolution())
// 			{
// 				fYMaxDpi = image.GetVerticalResolution();
// 			}
		}
	}
// 	theLog.Write(_T("CPrintableDocumentDeal::GetImagePrintDpi,2,fXMaxDpi=%f,fYMaxDpi=%f,Image_Deal_Max_Dpi=%f"), fXMaxDpi, fYMaxDpi, Image_Deal_Max_Dpi);
}

//根据图片和打印参数，获取图片的最佳打印参数
void CPrintableDocumentDeal::GetImagePrintParam(Image& image, int nPapeType, float& fXMaxDpi, float& fYMaxDpi
												, Size& PhysicalSize, Size& PrintSize, Size& ImageSize, Rect& ImageDestRect
												, BOOL bImageUsePhysicalArea, PicturePostion pp, PictureSize ps)
{
	//获取图片大小
	ImageSize.Width = image.GetWidth();
	ImageSize.Height = image.GetHeight();

	//根据纸型和最大可打印dpi来确定最大可打印区域
	GetPageSize(nPapeType, fXMaxDpi, fYMaxDpi, PhysicalSize, PrintSize);
	//根据图片和最大可打印区域来确定实际最大打印DPI
	GetImagePrintDpi(image, PrintSize, fXMaxDpi, fYMaxDpi);
	//根据纸型和实际最大可打印dpi来确定实际最大可打印区域
	GetPageSize(nPapeType, fXMaxDpi, fYMaxDpi, PhysicalSize, PrintSize);
	//调整图片打印区域
	AdjustImagePostion(PhysicalSize, PrintSize, ImageSize, ImageDestRect, bImageUsePhysicalArea, pp, ps);

	theLog.Write(_T("CPrintableDocumentDeal::GetImagePrintParam,PhysicalSize.Width=%d,PhysicalSize.Height=%d,")
		_T("PrintSize.Width=%d,PrintSize.Height=%d,ImageSize.Width=%d,ImageSize.Height=%d,PicturePostion=%d,PictureSize=%d,")
		_T("fXMaxDpi=%f,fYMaxDpi=%f,ImageDestRect.X=%d,ImageDestRect.Y=%d,ImageDestRect.Width=%d,ImageDestRect.Height=%d,")
		_T("bImageUsePhysicalArea=%d,PicturePostion=%d,PictureSize=%d")
		, PhysicalSize.Width, PhysicalSize.Height, PrintSize.Width, PrintSize.Height, ImageSize.Width, ImageSize.Height, pp, ps
		, fXMaxDpi, fYMaxDpi, ImageDestRect.X, ImageDestRect.Y, ImageDestRect.Width, ImageDestRect.Height
		, bImageUsePhysicalArea, pp, ps);
}

Image* CPrintableDocumentDeal::ConvertCmykToRgb(Image* cmykImage)
{
	if (!cmykImage)
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::ConvertCmykToRgb,cmykImage=%p"), cmykImage);
		return NULL;
	}

	int nImageWidth = cmykImage->GetWidth();
	int nImageHeight = cmykImage->GetHeight();
	Bitmap* tmpBmp = new Bitmap(nImageWidth, nImageHeight, PixelFormat24bppRGB);
	Graphics graphics(tmpBmp);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	Rect rc(0, 0, nImageWidth, nImageHeight);
	//将CMYK图片重绘一遍,此时GDI+自动将CMYK格式转换为RGB了
	Status s = graphics.DrawImage(cmykImage, rc);
	if (s != Ok)
	{
		theLog.Write(_T("!!CPrintableDocumentDeal::ConvertCmykToRgb,DrawImage fail,s=%d,err=%d"), s, GetLastError());
		delete tmpBmp;
		tmpBmp = NULL;
	}

	return tmpBmp;  
}
