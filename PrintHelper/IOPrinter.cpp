#include "StdAfx.h"
#include "IOPrinter.h"
#include "PrinterUtils.h"

#define File_Read_Length_Unit	(1024*1024)		//文件读取长度单元，1M大小

ColorMatrix colorMatrix = {
	0.299f, 0.299f, 0.299f, 0, 0,
	0.587f, 0.587f, 0.587f, 0, 0,
	0.114f, 0.114f, 0.114f, 0, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 0, 1
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

#include <AtlBase.h>

class CPrintWithWaterMarked
{
public:
	CPrintWithWaterMarked();
	~CPrintWithWaterMarked();

	Gdiplus::Bitmap* GetMarkedImg();
	BOOL Init(Image* img,JobSecNode* pSec);
	void DrawImg(Image* img);
private:
	Gdiplus::Font* InitFont(WCHAR* pVal, float fFontSize, int nFontStyle, SizeF& oSize);
	void OnDrawHeader(Graphics* grp);
	void OnDrawFooter(Graphics* grp);
	void OnDrawWatermark(Graphics* grp);
private:
	Bitmap* m_pWaterBmp;
	Graphics* m_pGrp;
	//WCHAR* Copyright;
	Gdiplus::Font* m_pFontHeader;
	Gdiplus::Font* m_pFontFooter;
	Gdiplus::Font* m_pFontWater;
	SizeF m_oHeaderSize;
	SizeF m_oFooterSize;
	SizeF m_oWaterSize;

	int m_nXOff;
	int m_nYOff;
	int m_nWidth;
	int m_nHeight;
	float m_fDpi;
	JobSecNode* m_pSecNode;
	BOOL m_bHasRotate;
};

CPrintWithWaterMarked::CPrintWithWaterMarked()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_pWaterBmp = 0;
	m_pGrp = 0;
	m_pFontWater = 0;
	m_pFontHeader = 0;
	m_pFontFooter = 0;
	m_pSecNode = 0;
	//Copyright = L"Copyright - 2011 信安宝";
	m_bHasRotate = FALSE;
}
CPrintWithWaterMarked::~CPrintWithWaterMarked()
{
	if (m_pWaterBmp)
	{
		delete m_pWaterBmp;
		m_pWaterBmp = 0;
	}
	if (m_pGrp)
	{
		delete m_pGrp;
		m_pGrp = 0;
	}
	
	if (m_pFontWater)
	{
		delete m_pFontWater;
		m_pFontWater = 0;
	}
	if (m_pFontHeader)
	{
		delete m_pFontHeader;
		m_pFontHeader = 0;
	}
	if (m_pFontFooter)
	{
		delete m_pFontFooter;
		m_pFontFooter = 0;
	}
}
Bitmap* CPrintWithWaterMarked::GetMarkedImg()
{
	return m_pWaterBmp;
}
BOOL CPrintWithWaterMarked::Init(Image* img,JobSecNode* pSec)
{
	ASSERT(img);
	ASSERT(pSec);
	m_pSecNode = pSec;

	//暂时默认所有的图片是非物理区域大小，所以原点的偏移为（0,0)。
	//如果将来要支持图片是物理区域大小，则要获取打印机的dc的原点偏移。
	m_nXOff = 0;
	m_nYOff = 0;
	m_nWidth = img->GetWidth() - m_nXOff * 2;
	m_nHeight = img->GetHeight() - m_nYOff * 2;
	m_fDpi = img->GetHorizontalResolution();

	//create a Bitmap the Size of the original photograph
	m_pWaterBmp = new Bitmap(m_nWidth, m_nHeight, PixelFormat32bppARGB);	//PixelFormat24bppRGB在emf文件的情况下，背景全部变成黑色了。
	ASSERT(m_pWaterBmp);
	if (!m_pWaterBmp)
	{
		return FALSE;
	}
	m_pWaterBmp->SetResolution(img->GetHorizontalResolution(), img->GetVerticalResolution());
	//load the Bitmap into a Graphics object 
	m_pGrp = Graphics::FromImage(m_pWaterBmp);
	m_pGrp->SetSmoothingMode(SmoothingModeAntiAlias);
	ASSERT(m_pGrp);
	if (!m_pGrp)
	{
		return FALSE;
	}
	return TRUE;
}

void CPrintWithWaterMarked::DrawImg(Image* img)
{
	ASSERT(img);
	Graphics* grp = m_pGrp;
	RectF rf(0, 0, m_nWidth, m_nHeight);

	//Draws the photo Image object at original size to the graphics object.
	grp->DrawImage(
		img,                               // Photo Image object
		rf, // Rectangle structure
		0,                                      // x-coordinate of the portion of the source image to draw. 
		0,                                      // y-coordinate of the portion of the source image to draw. 
		m_nWidth,                                // Width of the portion of the source image to draw. 
		m_nHeight,                               // Height of the portion of the source image to draw. 
		UnitPixel );							// Units of measure 

	//页眉
	OnDrawHeader(grp);
	//页脚
	OnDrawFooter(grp);
	//水印
	OnDrawWatermark(grp);
}

Gdiplus::Font* CPrintWithWaterMarked::InitFont(WCHAR* pVal, float fFontSize, int nFontStyle, SizeF& oSize)
{
	if (nFontStyle != FontStyleBoldItalic)	//非水印字体，才调整字体大小
	{
		fFontSize = fFontSize * 96.0 / m_fDpi;
	}

	Gdiplus::Font* crFont = new Gdiplus::Font(L"SimSun", fFontSize, nFontStyle);
	if (crFont)
	{
		PointF origin(0.0f, 0.0f);
		RectF boundRect;
		m_pGrp->MeasureString(pVal,-1, crFont,origin,&boundRect);
		oSize.Height = boundRect.Height;
		oSize.Width = boundRect.Width;
		return crFont;
	}
	return NULL;
}

void CPrintWithWaterMarked::OnDrawHeader(Graphics* grp)
{
	CStringW szHeaderW = L"";
	if (m_pSecNode && m_pSecNode->header.GetLength() > 0)
	{
#ifdef UNICODE
		szHeaderW = m_pSecNode->header;
#else
		USES_CONVERSION;
		szHeaderW = A2W(m_pSecNode->header);
#endif
	}
	if (!m_pFontHeader)
	{
		m_pFontHeader = InitFont(szHeaderW.GetBuffer(), 32.0, FontStyleRegular, m_oHeaderSize);
	}
	if (!m_pFontHeader)
	{
		return ;
	}
	theLog.Write(_T("printer the header,"));

	float xCenterOfImg = m_nXOff + m_nWidth * 0.5;
	float yPosFromBottom = m_nYOff + m_nHeight * 0.01;

	//Define the text layout by setting the text alignment to centered
	StringFormat StrFormat;
	StrFormat.SetAlignment(StringAlignmentCenter);

	//define a Brush which is semi trasparent black (Alpha set to 220)
	Gdiplus::SolidBrush semiTransBrush(Gdiplus::Color(220, 0, 0, 0));

	PointF pf(xCenterOfImg,yPosFromBottom);
	//Draw the Copyright string
	grp->DrawString(szHeaderW.GetBuffer(), -1,                //string of text
		m_pFontHeader,                                   //font
		pf,  //Position
		&StrFormat,
		&semiTransBrush);

	//绘制页眉文字下方的横线
	float fXPos2 = m_nXOff + m_nWidth * 0.1;
	float fYPos2 = yPosFromBottom + m_oHeaderSize.Height;	//文字下方
	float fXPos3 = m_nXOff + m_nWidth * 0.9;
	float fYPos3 = fYPos2;

	Pen pen(Color(220,0,0,0));
	grp->DrawLine(&pen, fXPos2, fYPos2, fXPos3, fYPos3);
}

void CPrintWithWaterMarked::OnDrawFooter(Graphics* grp)
{
	CStringW szFooterW = L"";
	if (m_pSecNode && m_pSecNode->footer.GetLength() > 0)
	{
#ifdef UNICODE
		szFooterW = m_pSecNode->footer;
#else
		USES_CONVERSION;
		szFooterW = A2W(m_pSecNode->footer);
#endif
	}
	if (!m_pFontFooter)
	{
		m_pFontFooter = InitFont(szFooterW.GetBuffer(), 32.0, FontStyleRegular, m_oFooterSize);
	}
	if (!m_pFontFooter)
	{
		return ;
	}
	theLog.Write(_T("printer the footer,"));

	float xCenterOfImg = m_nXOff + m_nWidth * 0.5;
	float yPosFromBottom = m_nYOff + m_nHeight * 0.99 - m_oFooterSize.Height;

	//Define the text layout by setting the text alignment to centered
	StringFormat StrFormat;
	StrFormat.SetAlignment(StringAlignmentCenter);

	//define a Brush which is semi trasparent black (Alpha set to 220)
	Gdiplus::SolidBrush semiTransBrush(Gdiplus::Color(220, 0, 0, 0));

	PointF pf(xCenterOfImg,yPosFromBottom);
	//Draw the Copyright string
	grp->DrawString(szFooterW.GetBuffer(), -1,                //string of text
		m_pFontFooter,                                   //font
		pf,  //Position
		&StrFormat,
		&semiTransBrush);
}

void CPrintWithWaterMarked::OnDrawWatermark(Graphics* grp)
{
	CStringW szWatermarkW = L"";
	if (m_pSecNode && m_pSecNode->water.GetLength() > 0)
	{
#ifdef UNICODE
		szWatermarkW = m_pSecNode->water;
#else
		USES_CONVERSION;
		szWatermarkW = A2W(m_pSecNode->water);
#endif
	}

	float fFontSize = 320.0;	//默认文字大小
	if (!m_pFontWater)
	{
		//获取测试字体大小的文本实际大小范围
		m_pFontWater = InitFont(szWatermarkW.GetBuffer(), fFontSize, FontStyleBoldItalic, m_oWaterSize);
	}

	if (!m_pFontWater)
	{
		return ;
	}

#if 0
	double fTextLength = (m_nWidth + m_nHeight) * 0.5;	//目标长度为宽高之和的平均值
#else
	double fTextLength = (m_nHeight > m_nWidth) ? m_nWidth : m_nHeight;	//先选择短边的长度
#endif

	fFontSize = fFontSize * fTextLength / m_oWaterSize.Width;
	//获取字体大小的文本实际大小范围
	delete m_pFontWater;	//删除测试的字体
	m_pFontWater = InitFont(szWatermarkW.GetBuffer(), fFontSize, FontStyleBoldItalic, m_oWaterSize);
	if (!m_pFontWater)
	{
		return ;
	}

	theLog.Write(_T("printer the water,"));

	float xCenterOfImg = m_nXOff + m_nWidth * 0.5;
	float yPosFromBottom = m_nXOff + (m_nHeight) * 0.5;

	//将纸张的中心作为坐标系统原点
	grp->TranslateTransform(xCenterOfImg, yPosFromBottom);
	//旋转打印内容
	grp->RotateTransform(-atan(1.0 * m_nHeight / m_nWidth) / 3.14159265358979323846 * 180);
	//将文字内容的向上进行偏移0.5个文字高度，以使文字中心与坐标系统原点重合
	xCenterOfImg = 0.0;
	yPosFromBottom = -m_oWaterSize.Height * 0.5;

	//Define the text layout by setting the text alignment to centered
	StringFormat StrFormat;
	StrFormat.SetAlignment(StringAlignmentCenter);

	//define a Brush which is semi trasparent black (Alpha set to 63)
	Gdiplus::SolidBrush semiTransBrush(Gdiplus::Color(63, 0, 0, 0));

	PointF pf(xCenterOfImg,yPosFromBottom);
	//Draw the Copyright string
	LPCTSTR water = szWatermarkW.GetString();
	grp->DrawString(water, -1,							//string of text
		m_pFontWater,                                   //font
		pf,  //Position
		&StrFormat,
		&semiTransBrush);
}


//-------------------------------------------
CString GetDefaultPath()
{
	TCHAR path[MAX_PATH];
	CString sDefaultPath;
	if(GetModuleFileName(0,path,MAX_PATH)==0) 
		return _T("");
	TCHAR *pdest;
	pdest=_tcsrchr(path,_T('\\'));
	if(pdest) 
		*pdest=0;
	sDefaultPath=path;
	sDefaultPath+=_T("\\");
	return sDefaultPath;
}

//打印机纸型信息
struct CPrinterPaperInfo
{
	CPrinterPaperInfo()
	{
		m_szPrinterName.clear();
		m_pwdPapers = NULL;
		m_pptPaperSize = NULL;
		m_pcPaperName = NULL;
		m_nPaperCount = 0;
		m_wdDefaultPaperSize = DMPAPER_A4;
	}
	~CPrinterPaperInfo()
	{
		m_szPrinterName.clear();
		if (m_pwdPapers)
		{
			delete[] m_pwdPapers;
			m_pwdPapers = NULL;
		}

		if (m_pptPaperSize)
		{
			delete[] m_pptPaperSize;
			m_pptPaperSize = NULL;
		}

		if (m_pcPaperName)
		{
			delete[] m_pcPaperName;
			m_pcPaperName = NULL;
		}

		m_nPaperCount = 0;
		m_wdDefaultPaperSize = DMPAPER_A4;
	}

	stdtstring m_szPrinterName;
	WORD* m_pwdPapers;
	POINT* m_pptPaperSize;
	TCHAR* m_pcPaperName;
	int m_nPaperCount;
	WORD m_wdDefaultPaperSize;
};

typedef map<stdtstring, CPrinterPaperInfo*> CPrinterPaperInfoMap;

//获取纸型信息工具类
#define CPrinterPaperSize_Support_PaperName	
class CPrinterPaperSize
{
protected:
	CPrinterPaperSize()
	{
		Release();
	}

public:
	static CPrinterPaperSize& GetInstance()
	{	
		static CPrinterPaperSize one;	
		return one;	
	}

	~CPrinterPaperSize()
	{
		Release();
	}

	void Release()
	{
		CCriticalSection2::Owner lock(m_cs4PrinterPaperInfoMap);

		CPrinterPaperInfoMap::iterator it;
		for (it=m_oPrinterPaperInfoMap.begin(); it!=m_oPrinterPaperInfoMap.end(); it++)
		{
			CPrinterPaperInfo* pPrinterPaperInfo = it->second;
			if (pPrinterPaperInfo)
			{
				delete pPrinterPaperInfo;
			}
		}
		m_oPrinterPaperInfoMap.clear();
	}

	void SetPrinterName(stdtstring szPrinterName)
	{
		CCriticalSection2::Owner lock(m_cs4PrinterPaperInfoMap);

		CPrinterPaperInfo* pPrinterPaperInfo = GetPrinterPaperInfo(szPrinterName);
		if (!pPrinterPaperInfo)
		{
			pPrinterPaperInfo = new CPrinterPaperInfo;
			pPrinterPaperInfo->m_szPrinterName = szPrinterName;
			pPrinterPaperInfo->m_wdDefaultPaperSize = GetPrinterDefaultPaperSize(szPrinterName);
			GetPrinterPaper(szPrinterName, pPrinterPaperInfo);
			m_oPrinterPaperInfoMap.insert(pair<stdtstring,CPrinterPaperInfo*>(szPrinterName, pPrinterPaperInfo));
		}
	}

	BOOL GetPaperInfoByPageSize(stdtstring szPrinterName, WORD wWidthMMT, WORD wHeightMMT, WORD& wdPaperSize, WORD& wdOrientation)
	{
		CPrinterPaperInfo* pPrinterPaperInfo = GetPrinterPaperInfo(szPrinterName);
		if (!pPrinterPaperInfo)
		{
			theLog.Write(_T("!!GetPaperInfoByPageSize,1,fail,szPrinterName=%s,pPrinterPaperInfo=%p")
				, szPrinterName.c_str(), pPrinterPaperInfo);
			return FALSE;
		}

		WORD* pwdPapers = pPrinterPaperInfo->m_pwdPapers;
		POINT* pptPaperSize = pPrinterPaperInfo->m_pptPaperSize;
		TCHAR* pcPaperName = pPrinterPaperInfo->m_pcPaperName;
		int nPaperCount = pPrinterPaperInfo->m_nPaperCount;
		WORD wdDefaultPaperSize = pPrinterPaperInfo->m_wdDefaultPaperSize;

		if (nPaperCount<=0 || !pptPaperSize)
		{
			theLog.Write(_T("!!GetPaperInfoByPageSize,2,fail,szPrinterName=%s,nPaperCount=%d,pptPaperSize=%p")
				, szPrinterName.c_str(), nPaperCount, pptPaperSize);
			return FALSE;
		}

		wdOrientation = (wWidthMMT > wHeightMMT) + 1;	//打印方向

		int nWidthMMT = wWidthMMT/* + 100*/;
		int nHeightMMT = wHeightMMT/* + 100*/;
		if (nWidthMMT > nHeightMMT)
		{
			int nTemp = nWidthMMT;
			nWidthMMT = nHeightMMT;
			nHeightMMT = nTemp;
		}

		int nDefaultPaperSizeIndex = -1;	//默认纸型的名称索引
		int nPaperSizeIndex = -1;			//匹配纸型的名称索引
		int nPaperDiffFactor = 0x7FFFFFFF;	//纸型误差因子

		for (int index=0; index<nPaperCount; index++)
		{
			if (pwdPapers[index] == wdDefaultPaperSize)
			{
				nDefaultPaperSizeIndex = index;
			}
			int nWidthDif = pptPaperSize[index].x-nWidthMMT;
			int nHeigthDif = pptPaperSize[index].y-nHeightMMT;
			int nTemp = (nWidthDif * nWidthDif) + (nHeigthDif * nHeigthDif);	
			if (nTemp < nPaperDiffFactor)//取差平方最小的匹配纸型
			{
				nPaperDiffFactor = nTemp;
				nPaperSizeIndex = index;
			}
		}

		if (nPaperSizeIndex >= 0)
		{
			wdPaperSize = pwdPapers[nPaperSizeIndex];	//打印纸型
		}

		if (nPaperDiffFactor > 10000)
		{
			theLog.Write(_T("########CPrinterPaperSize, nPaperDiffFactor(%d), Use DefaultPaperSize(%d)"), nPaperDiffFactor, wdDefaultPaperSize);
			wdPaperSize = wdDefaultPaperSize;	//匹配误差太大,使用默认打印纸型
			nPaperSizeIndex = nDefaultPaperSizeIndex;
		}

#ifdef CPrinterPaperSize_Support_PaperName
		TCHAR *pPaperName = pcPaperName + (nPaperSizeIndex*64);
		theLog.Write(_T("CPrinterPaperSize,wdPaperSize=%d,wdOrientation=%d,PaperName=[%s]"), wdPaperSize, wdOrientation, pPaperName);
#endif
		return TRUE;
	}

	BOOL GetPaperInfoByEmfHeader(stdtstring szPrinterName, PENHMETAHEADER pEmfHeader, WORD& wdPaperSize, WORD& wdOrientation)
	{
		if (!pEmfHeader)
		{
			theLog.Write(_T("!!GetPaperSizeByEmfHeader,1,fail,pEmfHeader=%p"), pEmfHeader);
			return FALSE;
		}

		int nEmfWidthMM = pEmfHeader->szlMillimeters.cx;
		int nEmfHeightMM = pEmfHeader->szlMillimeters.cy;
// 		int nEmfWidthMMT = 2 * (5 * nEmfWidthMM + 50);	//原始算法
// 		int nEmfHeightMMT = 2 * (5 * nEmfHeightMM + 50);
		int nEmfWidthMMT = nEmfWidthMM * 10 + 100;
		int nEmfHeightMMT = nEmfHeightMM * 10 + 100;
		return GetPaperInfoByPageSize(szPrinterName, nEmfWidthMMT, nEmfHeightMMT, wdPaperSize, wdOrientation);
	}

	BOOL GetPaperInfoByImage(stdtstring szPrinterName, Image& image, WORD& wdPaperSize, WORD& wdOrientation)
	{
		int nImageWidthMM = 0;
		int nImageHeightMM = 0;
		GetImageDeviceSize(image, nImageWidthMM, nImageHeightMM);
// 		int nImageWidthMMT = 2 * (5 * nImageWidthMM + 50);	//原始算法
// 		int nImageHeightMMT = 2 * (5 * nImageHeightMM + 50);
		int nImageWidthMMT = nImageWidthMM * 10 + 100;
		int nImageHeightMMT = nImageHeightMM * 10 + 100;
		return GetPaperInfoByPageSize(szPrinterName, nImageWidthMMT, nImageHeightMMT, wdPaperSize, wdOrientation);
	}

protected:
	int GetPrinterPaper(stdtstring szPrinterName, CPrinterPaperInfo* pPrinterPaperInfo)
	{
		if (!pPrinterPaperInfo)
		{
			return 0;
		}

		pPrinterPaperInfo->m_nPaperCount = 0;
		do 
		{
			if (szPrinterName.empty())
			{
				theLog.Write(_T("!!GetPrinterPaper,1,GetPrinterPaperSize fail,szPrinterName.IsEmpty()"));
				break;
			}

			stdtstring szPort = GetPrinterPort(szPrinterName);
			if (szPort.empty())
			{
				theLog.Write(_T("!!GetPrinterPaper,2,GetPrinterPaperSize fail,szPort.IsEmpty()"));
				break;
			}

			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERS, NULL, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,3,GetPrinterPaperSize DeviceCapabilities DC_PAPERS fail,m_nPaperCount=%d,err=%u"), pPrinterPaperInfo->m_nPaperCount, GetLastError());
				break;
			}
			theLog.Write(_T("GetPrinterPaper,3.2,GetPrinterPaperSize DeviceCapabilities DC_PAPERS succ,m_nPaperCount=%d"), pPrinterPaperInfo->m_nPaperCount);

			pPrinterPaperInfo->m_pwdPapers = new WORD[pPrinterPaperInfo->m_nPaperCount];	//array of WORD
			memset(pPrinterPaperInfo->m_pwdPapers, 0x0, sizeof(WORD)*pPrinterPaperInfo->m_nPaperCount);
			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERS, (LPTSTR)pPrinterPaperInfo->m_pwdPapers, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,4,GetPrinterPaperSize DeviceCapabilities DC_PAPERS fail,m_nPaperCount=%d,err=%u"),pPrinterPaperInfo-> m_nPaperCount, GetLastError());
				break;
			}

			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERSIZE, NULL, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,5,GetPrinterPaperSize DeviceCapabilities DC_PAPERSIZE fail,m_nPaperCount=%d,err=%u"), pPrinterPaperInfo->m_nPaperCount, GetLastError());
				break;
			}
			theLog.Write(_T("GetPrinterPaper,5.2,GetPrinterPaperSize DeviceCapabilities DC_PAPERSIZE succ,m_nPaperCount=%d"), pPrinterPaperInfo->m_nPaperCount);

			pPrinterPaperInfo->m_pptPaperSize = new POINT[pPrinterPaperInfo->m_nPaperCount];	//array of POINT structures
			memset(pPrinterPaperInfo->m_pptPaperSize, 0x0, sizeof(POINT)*pPrinterPaperInfo->m_nPaperCount);
			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERSIZE, (LPTSTR)pPrinterPaperInfo->m_pptPaperSize, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,6,GetPrinterPaperSize DeviceCapabilities DC_PAPERSIZE fail,m_nPaperCount=%d,err=%u"), pPrinterPaperInfo->m_nPaperCount, GetLastError());
				break;
			}

#ifdef CPrinterPaperSize_Support_PaperName
			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERNAMES, NULL, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,7,GetPrinterPaperSize DeviceCapabilities DC_PAPERNAMES fail,m_nPaperCount=%d,err=%u"), pPrinterPaperInfo->m_nPaperCount, GetLastError());
				break;
			}
			theLog.Write(_T("GetPrinterPaper,7.2,GetPrinterPaperSize DeviceCapabilities DC_PAPERNAMES succ,m_nPaperCount=%d"), pPrinterPaperInfo->m_nPaperCount);

			pPrinterPaperInfo->m_pcPaperName = new TCHAR[pPrinterPaperInfo->m_nPaperCount*64];	//Each string buffer is 64 characters long and contains the name of a paper form.
			memset(pPrinterPaperInfo->m_pcPaperName, 0x0, pPrinterPaperInfo->m_nPaperCount*64);
			pPrinterPaperInfo->m_nPaperCount = DeviceCapabilities(szPrinterName.c_str(),szPort.c_str(), DC_PAPERNAMES, (LPTSTR)pPrinterPaperInfo->m_pcPaperName, NULL);
			if (pPrinterPaperInfo->m_nPaperCount<=0)
			{
				theLog.Write(_T("!!GetPrinterPaper,8,GetPrinterPaperSize DeviceCapabilities DC_PAPERNAMES fail,m_nPaperCount=%d,err=%u"), pPrinterPaperInfo->m_nPaperCount, GetLastError());
				break;
			}

			for (int i=0; i<pPrinterPaperInfo->m_nPaperCount; i++)
			{
				theLog.Write(_T("PaperSize,i=%d\t%d\t%s\t(%d, %d)"), i+1, pPrinterPaperInfo->m_pwdPapers[i], pPrinterPaperInfo->m_pcPaperName+(i*64), pPrinterPaperInfo->m_pptPaperSize[i].x, pPrinterPaperInfo->m_pptPaperSize[i].y);
			}

#endif
		} while (FALSE);

		theLog.Write(_T("GetPrinterPaper,9,m_nPaperCount=%d"), pPrinterPaperInfo->m_nPaperCount);
		return pPrinterPaperInfo->m_nPaperCount;
	}

	stdtstring GetPrinterPort(stdtstring szPrnterName)
	{
		stdtstring szPort;
		HANDLE h = 0;
		PRINTER_DEFAULTS pd;
		pd.pDatatype     = NULL;
		pd.pDevMode      = NULL;
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		TCHAR cPrinter[MAX_PATH] = {0};
		_tcscpy_s(cPrinter, szPrnterName.c_str());
		if(OpenPrinter(cPrinter,&h,&pd) && h)
		{
			DWORD dwSize = 0;
			GetPrinter(h,2,0,0,&dwSize);
			//theLog.Write(_T("GetPrinterStatus %d"),dwSize);
			if (dwSize > 0)
			{
				BYTE* pData = new BYTE[dwSize];
				if(GetPrinter(h,2,pData,dwSize,&dwSize))
				{
					PRINTER_STATUS_BUSY;
					PRINTER_INFO_2* pInfo = (PRINTER_INFO_2*)pData;
					szPort = pInfo->pPortName;
				}
				else
				{
					theLog.Write(_T("!!GetPrinterPort,GetPrinter,3, err %d"),::GetLastError());				
				}
				delete[] pData;
			}
			else
			{
				theLog.Write(_T("!!GetPrinterPort,GetPrinter,2, err %d"),::GetLastError());				
			}
			ClosePrinter(h);
		}
		else
		{
			theLog.Write(_T("!!GetPrinterPort,OpenPrinter,1, err %d"),::GetLastError());				
		}

		return szPort;
	}

	WORD GetPrinterDefaultPaperSize(stdtstring szPrnterName)
	{
		WORD wdPaperSize = DMPAPER_A4;
		if (!szPrnterName.empty())
		{
			TCHAR cPrinterName[MAX_PATH] = {0};
			_tcscpy_s(cPrinterName, szPrnterName.c_str());
			HANDLE hPrinter = NULL;
			if (OpenPrinter(cPrinterName, &hPrinter, NULL))
			{
				LONG lRet = DocumentProperties(NULL, hPrinter, cPrinterName, NULL, NULL, 0);
				if (lRet > 0)
				{
					BYTE* pData = new BYTE[lRet];
					memset(pData, 0x0, lRet);
					lRet = DocumentProperties(NULL, hPrinter, cPrinterName, (PDEVMODE)pData, NULL, DM_OUT_BUFFER);
					if (lRet == IDOK)
					{
						DEVMODE* pDev = (DEVMODE*)pData;
						if (pDev->dmFields & DM_PAPERSIZE)
						{
							wdPaperSize = pDev->dmPaperSize;
						}
						else
						{
							theLog.Write(_T("!!GetPrinterDefaultPaperSize,Not Support DM_PAPERSIZE,dmFields=%d"), pDev->dmFields);
						}
					}
					else
					{
						theLog.Write(_T("!!GetPrinterDefaultPaperSize,DocumentProperties,BB fail,err=%u,lRet=%d"), GetLastError(), lRet);
					}
					delete[] pData;
				}
				else
				{
					theLog.Write(_T("!!GetPrinterDefaultPaperSize,DocumentProperties,AA fail,err=%u,lRet=%d"), GetLastError(), lRet);
				}
				ClosePrinter(hPrinter);
			}
			else
			{
				theLog.Write(_T("!!GetPrinterDefaultPaperSize,OpenPrinter fail,err=%u"), GetLastError());
			}
		}
		else
		{
			theLog.Write(_T("!!GetPrinterDefaultPaperSize,szPrnterName.IsEmpty()"));
		}
		theLog.Write(_T("GetPrinterDefaultPaperSize,wdPaperSize=%d"), wdPaperSize);
		return wdPaperSize;
	}

	//通过图片的dpi(600/300)和大小(pixel)来获取打印图片的设备大小(mm)
	void GetImageDeviceSize(Image& image, int& nDeviceWidthMM, int& nDeviceHeightMM)
	{
		int nImageWidth = image.GetWidth();
		int nImageHeight = image.GetHeight();
		float fImageWidthDpi = image.GetHorizontalResolution();
		float fImageHeightDpi = image.GetVerticalResolution();
		int nDeviceWidthMMT = nImageWidth / fImageWidthDpi * 25.4 * 10;	//1 in = 25.4 mm,这里是毫米的10倍
		int nDeviceHeightMMT = nImageHeight / fImageHeightDpi * 25.4 * 10;	
		nDeviceWidthMM = MulDiv(nDeviceWidthMMT, 1, 10);	//四舍五入，如MulDiv(2564, 1, 10) == 256
		nDeviceHeightMM = MulDiv(nDeviceHeightMMT, 1, 10);	//四舍五入，如MulDiv(2566, 1, 10) == 257
		theLog.Write(_T("GetImageDeviceSize,nDeviceWidthMM=%d,nDeviceHeightMM=%d"), nDeviceWidthMM, nDeviceHeightMM);
	}

	CPrinterPaperInfo* GetPrinterPaperInfo(stdtstring szPrnterName)
	{
		CCriticalSection2::Owner lock(m_cs4PrinterPaperInfoMap);

		CPrinterPaperInfo* pPrinterPaperInfo = NULL;
		CPrinterPaperInfoMap::iterator it = m_oPrinterPaperInfoMap.find(szPrnterName);
		if (it != m_oPrinterPaperInfoMap.end())
		{
			pPrinterPaperInfo = it->second;
		}
		return pPrinterPaperInfo;
	}

private:
	CPrinterPaperInfoMap m_oPrinterPaperInfoMap;
	CCriticalSection2 m_cs4PrinterPaperInfoMap;
};

CIOPrinter::CIOPrinter(void)
{
	m_szDocName = _T("DefaultPrintDocName");
	m_dwColor = DMCOLOR_MONOCHROME;
	m_dwDuplex = DMDUP_SIMPLEX;
	m_dwOrientation = DMORIENT_PORTRAIT;
	m_dwPaperSize = DMPAPER_A4;
	m_dwPaperLength = 0;
	m_dwPaperWidth = 0;
	m_dwScale = 100;
	m_dwCopies = 1;
	m_dwCollate = DMCOLLATE_TRUE;
	m_dwDefaultSource = DMBIN_AUTO;
	m_dwPrintQuality = DMRES_HIGH;
	m_dwPagesPerPaper = 1;
	m_bAutoPaperSize = TRUE;
	m_nPrinted = 0;
	m_eType = BT_THIN_CLT;
	m_nCurrentPage = 0;
	EnableDefaultSource(FALSE);

	m_bUseA4Print = g_oIniFile.GetVal(_T("PrtCtrlConfig"), _T("UseA4Print"), FALSE);//add by zxl,20150113,非A3/A4的打印任务，转换成A4纸型打印
	m_bForceMonochromePrint = g_oIniFile.GetVal(_T("PrtCtrlConfig"), _T("ForceHbPrint"), TRUE);	//add by zxl,20150728,在一些打印机驱动中无法在打印时设置色彩模式，所以强制使用黑白转换
	m_nNetPacketSize = g_oIniFile.GetVal(_T("PrtCtrlConfig"), _T("NetPacketSize"), 1024*64);		//网络包大小
	if (m_nNetPacketSize < 10240)
	{
		m_nNetPacketSize = 1024 * 64;
	}
	m_nNetPacketSleepMS = g_oIniFile.GetVal(_T("PrtCtrlConfig"), _T("NetPacketSleepMS"), 100);	//每个网络包等待的时间（毫秒）
	if (m_nNetPacketSleepMS < 100)
	{
		m_nNetPacketSleepMS = 100;
	}
	m_nAllNetPacketMinSleepMS = g_oIniFile.GetVal(_T("PrtCtrlConfig"), _T("AllNetPacketMinSleepMS"), 1000);	//所有网络包最小等待的时间（毫秒）
	theLog.Write(_T("CIOPrinter::CIOPrinter,m_bUseA4Print=%d,m_bForceMonochromePrint=%dm_nNetPacketSize=%d,m_nNetPacketSleepMS=%d,m_nAllNetPacketMinSleepMS=%d")
		, m_bUseA4Print, m_bForceMonochromePrint, m_nNetPacketSize, m_nNetPacketSleepMS, m_nAllNetPacketMinSleepMS);
}

CIOPrinter::~CIOPrinter(void)
{
}

static CString GetTempName(CString strPath,CString sName)
{
	const int nMin = 0;
	const int nMax = 999;
	//const CString strBase(_T("TestPrint"));
	static int nLastKnown = nMin;
	strPath.TrimRight(_T("\\"));

	sName.Replace('*',' ');
	sName.Replace('%',' ');
	bool bFound = false;
	CString strPathName;
	while (!bFound) {
		if (nLastKnown > nMax)
			break;
		strPathName = strPath;
		strPathName.AppendFormat(_T("\\%s_%03.3d.xps"), sName,nLastKnown++);
		HANDLE hFile = ::CreateFile(strPathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(hFile);
			bFound = TRUE;
		}
	}
	if (!bFound)
		strPathName.Empty();
	return(strPathName);
}
//获取默认打印机
CString CIOPrinter::GetDefault()
{
	CString sDefault;
	TCHAR szBuf[1024] = {0};
	DWORD dwSize = 1024;
	GetDefaultPrinter(szBuf,&dwSize);
	sDefault = szBuf;
	return sDefault;
}

void CIOPrinter::SetDocName(CString sDocName)
{
	m_szDocName = sDocName;
	VerifyDocName(m_szDocName);
	theLog.Write(_T("CIOPrinter::SetDocName,sDocName=%s,m_szDocName=%s"), sDocName, m_szDocName);
}

void CIOPrinter::SetJobUser(CString sJobUser)
{
	m_szJobUser = sJobUser;
	theLog.Write(_T("CIOPrinter::SetJobUser,m_szJobUser=%s"), m_szJobUser);
}

void CIOPrinter::EnableColor(BOOL b)
{
	if (b)
	{
		m_dwColor = DMCOLOR_COLOR;
	}
	else
	{
		m_dwColor = DMCOLOR_MONOCHROME;
	}
	theLog.Write(_T("CIOPrinter::EnableColor,m_dwColor=%d"), m_dwColor);
}

void CIOPrinter::SetDuplex(WORD dw)
{
	DMDUP_SIMPLEX;
	DMDUP_VERTICAL;
	DMDUP_HORIZONTAL;
	m_dwDuplex = dw;
	theLog.Write(_T("CIOPrinter::SetDuplex,m_dwDuplex=%d"), m_dwDuplex);
}

void CIOPrinter::SetOrientation(WORD dw)
{
	DMORIENT_PORTRAIT;
	m_dwOrientation = dw;
	theLog.Write(_T("CIOPrinter::SetOrientation,m_dwOrientation=%d"), m_dwOrientation);
}

void CIOPrinter::SetPaperLength(WORD dw)
{
	if (dw>=10)	//不能太小，否则打印会失败
	{
		m_dwPaperLength = dw;
	}
	theLog.Write(_T("CIOPrinter::SetPaperLength,m_dwPaperLength=%d,dw=%d"), m_dwPaperLength, dw);
}

void CIOPrinter::SetPaperWidth(WORD dw)
{
	if (dw>=10)	//不能太小，否则打印会失败
	{
		m_dwPaperWidth = dw;
	}
	theLog.Write(_T("CIOPrinter::SetPaperWidth,m_dwPaperWidth=%d,dw=%d"), m_dwPaperWidth, dw);
}

void CIOPrinter::SetScale(WORD dw)
{
	m_dwScale = dw;
	theLog.Write(_T("CIOPrinter::SetScale,m_dwScale=%d"), m_dwScale);
}

void CIOPrinter::SetCopies(WORD dw)
{
	if (dw<1 || dw>9999)
	{
		m_dwCopies = 1;
	}
	else
	{
		m_dwCopies = dw;
	}
	theLog.Write(_T("CIOPrinter::SetCopies,m_dwCopies=%d"), m_dwCopies);
}

void CIOPrinter::EnableCollate(BOOL b)
{
	if (b)
	{
		m_dwCollate = DMCOLLATE_TRUE;
	}
	else
	{
		m_dwCollate = DMCOLLATE_FALSE;
	}
	theLog.Write(_T("CIOPrinter::EnableCollate,m_dwCollate=%d"), m_dwCollate);
}

void CIOPrinter::EnableDefaultSource(BOOL bEnable)
{
	m_bEnableDefaultSource = bEnable;
	theLog.Write(_T("CIOPrinter::EnableDefaultSource,m_bEnableDefaultSource=%d"), m_bEnableDefaultSource);
}

void CIOPrinter::EnableAutoPaperSize(BOOL bEnable)
{
	m_bAutoPaperSize = bEnable;
	theLog.Write(_T("CIOPrinter::EnableAutoPaperSize,m_bAutoPaperSize=%d"), m_bAutoPaperSize);
}

BOOL CIOPrinter::IsEnableDefaultSource()
{
	return m_bEnableDefaultSource;
}


//设置打印机纸张来源
/* bin selections */
// #define DMBIN_FIRST         DMBIN_UPPER		//纸盒一
// #define DMBIN_UPPER         1				//纸盒一
// #define DMBIN_ONLYONE       1				//纸盒一
// #define DMBIN_LOWER         2				//纸盒二
// #define DMBIN_MIDDLE        3				//纸盒三
// #define DMBIN_MANUAL        4				//手动送纸器
// #define DMBIN_ENVELOPE      5				//信封进纸器
// #define DMBIN_ENVMANUAL     6				//手动信封
// #define DMBIN_AUTO          7				//打印机设置/自动
// #define DMBIN_TRACTOR       8
// #define DMBIN_SMALLFMT      9
// #define DMBIN_LARGEFMT      10
// #define DMBIN_LARGECAPACITY 11				//大容量纸盘
// #define DMBIN_CASSETTE      14				//多功能进纸器
// #define DMBIN_FORMSOURCE    15				//自动
// #define DMBIN_LAST          DMBIN_FORMSOURCE
// #define DMBIN_USER          256     /* device specific bins start here */


/*施乐纸选择盒参数
15	自动
7	自动选择
1	纸盘1
3	纸盘2
2	纸盘3
257	纸盘4
261	**纸盘3[大容量] 
262	**纸盘4[大容量] 
4	纸盘5(手送纸盘)
*/
void CIOPrinter::SetSLPrinterSource(Paper_Source PaperSource)	//施乐
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(1);
		break;
	case Paper_Source_2:
		SetDefaultSource(3);
		break;
	case Paper_Source_3:
		SetDefaultSource(2);
		break;
	case Paper_Source_4:
		SetDefaultSource(257);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(261);
		break;
	case Paper_Source_LARGEFMT:
		SetDefaultSource(262);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetSLPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*东芝纸选择盒参数
7	自动
1	纸盒 1
3	纸盒 2
2	纸盒 3
257	纸盒 4
258	大容量供纸器
11	扩展大容量供纸器
4	旁路托盘
259	插页器单元
*/
void CIOPrinter::SetDZPrinterSource(Paper_Source PaperSource)	//东芝
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(1);
		break;
	case Paper_Source_2:
		SetDefaultSource(3);
		break;
	case Paper_Source_3:
		SetDefaultSource(2);
		break;
	case Paper_Source_4:
		SetDefaultSource(257);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(11);
		break;
	case Paper_Source_LARGEFMT:
		SetDefaultSource(258);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetDZPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*科美纸选择盒参数
7	自动
257	纸盘1
258	纸盘2
259	纸盘3
260	纸盘4
1000	LCT[大容量纸盒]
4	手送纸盘
*/
void CIOPrinter::SetKMPrinterSource(Paper_Source PaperSource)	//柯美
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(257);
		break;
	case Paper_Source_2:
		SetDefaultSource(258);
		break;
	case Paper_Source_3:
		SetDefaultSource(259);
		break;
	case Paper_Source_4:
		SetDefaultSource(260);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(1000);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetKMPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*利盟纸选择盒参数
15	自动选择
7	使用打印机设置
1	进纸匣1
2	进纸匣2
3	进纸匣3
257	进纸匣4
258	进纸匣5
14	多功能进纸器
4	手动纸张
6	手动信封
5	信封进纸器
*/
void CIOPrinter::SetLMPrinterSource(Paper_Source PaperSource)	//利盟
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(15);
		break;
	case Paper_Source_1:
		SetDefaultSource(1);
		break;
	case Paper_Source_2:
		SetDefaultSource(2);
		break;
	case Paper_Source_3:
		SetDefaultSource(3);
		break;
	case Paper_Source_4:
		SetDefaultSource(257);
		break;
	case Paper_Source_5:
		SetDefaultSource(258);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_ENVMANUAL:
		SetDefaultSource(6);
		break;
	case Paper_Source_ENVELOPE:
		SetDefaultSource(5);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetLMPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*理光纸选择盒参数
7	自动选择纸盘
1	纸盘1
3	纸盘2
2	纸盘3
4	手送台
11	大容量纸盘
*/
void CIOPrinter::SetLGPrinterSource(Paper_Source PaperSource)	//理光
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(1);
		break;
	case Paper_Source_2:
		SetDefaultSource(3);
		break;
	case Paper_Source_3:
		SetDefaultSource(2);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(11);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetLGPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*京瓷纸选择盒参数
7	自动来源选择
14	纸盒 1
4	多用途托盘
*/
void CIOPrinter::SetJCPrinterSource(Paper_Source PaperSource)	//京瓷
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(14);
		break;
//信息不全,暂时只知道第一个纸盒
// 	case Paper_Source_2:
// 		SetDefaultSource(3);
// 		break;
// 	case Paper_Source_3:
// 		SetDefaultSource(2);
// 		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
// 	case Paper_Source_LARGECAPACITY:
// 		SetDefaultSource(11);
// 		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetJCPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*震旦纸选择盒参数
7	自动
257	纸盘1
258	纸盘2
259	纸盘3
260	纸盘4
1000	LCT[大容量纸盒]
4	手送纸盘
*/
void CIOPrinter::SetZDPrinterSource(Paper_Source PaperSource)	//震旦	- 与柯美是一样的
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(257);
		break;
	case Paper_Source_2:
		SetDefaultSource(258);
		break;
	case Paper_Source_3:
		SetDefaultSource(259);
		break;
	case Paper_Source_4:
		SetDefaultSource(260);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(4);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(1000);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetZDPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*夏普纸选择盒参数
7	自动选择
261	手送纸盒
257	纸盒1
258	纸盒2
259	纸盒3
260	纸盒4
263	纸盒5[大容量纸盒]
*/
void CIOPrinter::SetXPPrinterSource(Paper_Source PaperSource)	//夏普
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(257);
		break;
	case Paper_Source_2:
		SetDefaultSource(258);
		break;
	case Paper_Source_3:
		SetDefaultSource(259);
		break;
	case Paper_Source_4:
		SetDefaultSource(260);
		break;
// 	case Paper_Source_5:
// 		SetDefaultSource(263);
// 		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(261);
		break;
	case Paper_Source_LARGECAPACITY:
		SetDefaultSource(263);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetXPPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*三星纸选择盒参数
7	自动选择
6	多用途纸盘
1	纸盘 1
2	纸盘 2
257	纸盘 3
258	纸盘 4
*/
void CIOPrinter::SetSXPrinterSource(Paper_Source PaperSource)	//三星
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(7);
		break;
	case Paper_Source_1:
		SetDefaultSource(1);
		break;
	case Paper_Source_2:
		SetDefaultSource(2);
		break;
	case Paper_Source_3:
		SetDefaultSource(257);
		break;
	case Paper_Source_4:
		SetDefaultSource(258);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(6);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetSXPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

/*惠普纸选择盒参数
15	 自动选择
257	 打印机自动选择
258	 手动进纸
259	 纸盒 1
260	 纸盒 2
261	 纸盒 3
262	 纸盒 4
512	 信封输送器
*/
void CIOPrinter::SetHPPrinterSource(Paper_Source PaperSource)	//惠普
{
	switch (PaperSource)
	{
	case Paper_Source_AUTO:
		SetDefaultSource(15);
		break;
	case Paper_Source_1:
		SetDefaultSource(259);
		break;
	case Paper_Source_2:
		SetDefaultSource(260);
		break;
	case Paper_Source_3:
		SetDefaultSource(261);
		break;
	case Paper_Source_4:
		SetDefaultSource(262);
		break;
	case Paper_Source_MANUAL:
		SetDefaultSource(258);
		break;
	case Paper_Source_ENVELOPE:
		SetDefaultSource(512);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetHPPrinterSource, UnKnow Soruce=%d"), PaperSource);
		break;
	}
}

void CIOPrinter::SetDefaultSourceByBrand(BRAND_TYPE eType, Paper_Source PaperSource)
{
	m_eType = eType;
	switch (m_eType)
	{
	case BT_SL:
	case BT_SL2:
		SetSLPrinterSource(PaperSource);
		break;
	case BT_DZ:
		SetDZPrinterSource(PaperSource);
		break;
	case BT_KM:
		SetKMPrinterSource(PaperSource);
		break;
	case BT_LM:
		SetLMPrinterSource(PaperSource);
		break;
	case BT_LG:
		SetLGPrinterSource(PaperSource);
		break;
	case BT_JC:
		SetJCPrinterSource(PaperSource);
		break;
	case BT_ZD:
		SetZDPrinterSource(PaperSource);
		break;
	case BT_XP:
		SetXPPrinterSource(PaperSource);
		break;
	case BT_SX:
		SetSXPrinterSource(PaperSource);
		break;
	case BT_HP:
		SetHPPrinterSource(PaperSource);
		break;
	default:
		theLog.Write(_T("!!CIOPrinter::SetDefaultSourceByBrand, UnKnow BRAND_TYPE=%d, PaperSource=%d"), eType, PaperSource);
		break;
	}
}

void CIOPrinter::SetDefaultSource(WORD dw)
{
	EnableDefaultSource(TRUE);
	DMBIN_AUTO;
	m_dwDefaultSource = dw;
	theLog.Write(_T("CIOPrinter::SetDefaultSource,m_dwDefaultSource=%d"), m_dwDefaultSource);
}

void CIOPrinter::SetPrintQuality(WORD dw)
{
	m_dwPrintQuality = dw;
	theLog.Write(_T("CIOPrinter::SetPrintQuality,m_dwPrintQuality=%d"), m_dwPrintQuality);
}

void CIOPrinter::SetPagesPerPaper(WORD dw)
{
	if (dw == 1 || dw == 2 || dw == 4 || dw == 6 || dw == 8	|| dw == 9 || dw == 16)
	{
		m_dwPagesPerPaper = dw;
	}
	else
	{
		m_dwPagesPerPaper = 1;
		theLog.Write(_T("##CIOPrinter::SetPagesPerPaper,not support dw=%d,change to 1"), dw);
	}
	theLog.Write(_T("CIOPrinter::SetPagesPerPaper,m_dwPagesPerPaper=%d"), m_dwPagesPerPaper);
}

void CIOPrinter::GetAllPageInOneParam(Rect& printRect, Image& image, AllPageInOneParam& param)
{	
	int nPagesPerPaper = m_dwPagesPerPaper;
	int nCurrentPage = m_nCurrentPage;
	if (nPagesPerPaper == 1)
	{
		//不做任何事情
		param.x_offset = 0.0;
		param.y_offset = 0.0;
		param.rotate_radians = 0.0;
	}
	else if (nCurrentPage >= 0)
	{
		WORD paper_orientation = (printRect.Width < printRect.Height) ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;	//打印纸方向
		WORD page_orientation = (image.GetWidth() < image.GetHeight()) ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;		//打印内容方向
		int nPageNum = nCurrentPage % nPagesPerPaper;	//nPageNum和nCurrentPage是0为第1页
		switch (nPagesPerPaper)
		{
		case 2:	//二合一
			{
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 2;
					printRect.Height = temp;
					param.x_offset = 0.0;
					param.y_offset = (2 - nPageNum) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					//printRect.Width = printRect.Width;
					printRect.Height = printRect.Height / 2;
					param.x_offset = printRect.Width;
					param.y_offset = (2 - nPageNum) * (printRect.Height);
					param.rotate_radians = 180.05;	//旋转180度,目前发现如果取180.0时，emf打印在绘图时，
													//会出现质量下降非常严重的情况，原因未知，
													//暂时取一个最接近的并且不会下降绘图质量的值180.05。
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					printRect.Width = printRect.Width / 2;
					//printRect.Height = printRect.Height;
					param.x_offset = nPageNum * (printRect.Width);
					param.y_offset = 0.0;
					param.rotate_radians = 0.0;	//旋转0度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					int temp = printRect.Height;
					printRect.Height = printRect.Width / 2;
					printRect.Width = temp;
					param.x_offset = nPageNum * (printRect.Height);
					param.y_offset = printRect.Width;
					param.rotate_radians = 270.0;	//旋转270度
				}
			}
			break;
		case 4:	//四合一
			{
				int nRow = (nPageNum < 2) ? 0 : 1;
				int nCol = nPageNum - (nRow * 2);
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					printRect.Width = printRect.Width / 2;
					printRect.Height = printRect.Height / 2;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 2;
					printRect.Height = temp / 2;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 2;
					printRect.Height = temp / 2;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					printRect.Width = printRect.Width / 2;
					printRect.Height = printRect.Height / 2;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
			}
			break;
		case 6:	//六合一
			{
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					int nRow = 3 - (nPageNum % 3) - 1;
					int nCol = (nPageNum < 3) ? 0 : 1;
					int temp = printRect.Height;
					printRect.Height = printRect.Width / 2;
					printRect.Width = temp / 3;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					int nRow = 3 - (nPageNum % 3) - 1;
					int nCol = (nPageNum < 3) ? 0 : 1;
					printRect.Width = printRect.Width / 2;
					printRect.Height = printRect.Height / 3;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					int nRow = (nPageNum < 3) ? 0 : 1;
					int nCol = nPageNum % 3;
					printRect.Width = printRect.Width / 3;
					printRect.Height = printRect.Height / 2;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					int nRow = (nPageNum < 3) ? 0 : 1;
					int nCol = nPageNum % 3;
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 2;
					printRect.Height = temp / 3;
					param.x_offset = (nCol + 1) * (printRect.Height);
					param.y_offset = nRow * (printRect.Width);
					param.rotate_radians = 90.0;	//旋转90度
				}
			}
			break;
		case 8:	//八合一
			{
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					int nRow = 4 - (nPageNum % 4) - 1;
					int nCol = (nPageNum < 4) ? 0 : 1;
					int temp = printRect.Height;
					printRect.Height = printRect.Width / 2;
					printRect.Width = temp / 4;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					int nRow = 4 - (nPageNum % 4) - 1;
					int nCol = (nPageNum < 4) ? 0 : 1;
					printRect.Width = printRect.Width / 2;
					printRect.Height = printRect.Height / 4;
					param.x_offset = (nCol + 1) * (printRect.Width);
					param.y_offset = (nRow + 1) * (printRect.Height);
					param.rotate_radians = 180.05;	//旋转180度,目前发现如果取180.0时，emf打印在绘图时，
													//会出现质量下降非常严重的情况，原因未知，
													//暂时取一个最接近的并且不会下降绘图质量的值180.05。
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					int nRow = (nPageNum < 4) ? 0 : 1;
					int nCol = nPageNum % 4;
					printRect.Width = printRect.Width / 4;
					printRect.Height = printRect.Height / 2;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					int nRow = (nPageNum < 4) ? 0 : 1;
					int nCol = nPageNum % 4;
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 2;
					printRect.Height = temp / 4;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
			}
			break;
		case 9:	//九合一
			{
				int nRow = (nPageNum < 3) ? 0 : ((nPageNum < 6) ? 1 : 2);
				int nCol = nPageNum - (nRow * 3);
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					printRect.Width = printRect.Width / 3;
					printRect.Height = printRect.Height / 3;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 3;
					printRect.Height = temp / 3;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 3;
					printRect.Height = temp / 3;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					printRect.Width = printRect.Width / 3;
					printRect.Height = printRect.Height / 3;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
			}
			break;
		case 16:	//十六合一
			{
				int nRow = (nPageNum < 4) ? 0 : ((nPageNum < 8) ? 1 : ((nPageNum < 12) ? 2 : 3));
				int nCol = nPageNum - (nRow * 4);
				if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_PORTRAIT)
				{
					printRect.Width = printRect.Width / 4;
					printRect.Height = printRect.Height / 4;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
				else if (paper_orientation==DMORIENT_PORTRAIT && page_orientation==DMORIENT_LANDSCAPE)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 4;
					printRect.Height = temp / 4;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_PORTRAIT)
				{
					int temp = printRect.Width;
					printRect.Width = printRect.Height / 4;
					printRect.Height = temp / 4;
					param.x_offset = nCol * (printRect.Height);
					param.y_offset = (nRow + 1) * (printRect.Width);
					param.rotate_radians = 270.0;	//旋转270度
				}
				else /*if (paper_orientation==DMORIENT_LANDSCAPE && page_orientation==DMORIENT_LANDSCAPE)*/
				{
					printRect.Width = printRect.Width / 4;
					printRect.Height = printRect.Height / 4;
					param.x_offset = nCol * (printRect.Width);
					param.y_offset = nRow * (printRect.Height);
					param.rotate_radians = 0.0;	//旋转0度
				}
			}
			break;
		default:
			{
				theLog.Write(_T("!!CIOPrinter::GetAllPageInOneParam,not support AllPageInOneParam,nPagesPerPaper=%d"), nPagesPerPaper);
			}
			break;
		}	
	}
}

void CIOPrinter::SetPrinted(int nPrinted)
{
	m_nPrinted = nPrinted;
	theLog.Write(_T("CIOPrinter::SetPrinted,m_nPrinted=%d"), m_nPrinted);
}

void CIOPrinter::SetPaperSize(WORD dw)
{
	DMPAPER_A4;
	if (DMPAPER_A4 == dw)
	{
		m_dwPaperSize = DMPAPER_A4;
	}
	else if (DMPAPER_A3 == dw)
	{
		m_dwPaperSize = DMPAPER_A3;
	} 
	else
	{
		if (m_bUseA4Print)
		{
			theLog.Write(_T("!!CIOPrinter::SetPaperSize,PapeSize [%d] is not support,changeto DMPAPER_A4."), dw);
			m_dwPaperSize = DMPAPER_A4;
		}
		else
		{
			m_dwPaperSize = dw;
		}
	}
	theLog.Write(_T("CIOPrinter::SetPaperSize,m_dwPaperSize=%d"), m_dwPaperSize);
}

//GetPrinterStatus不支持Windows 95/98/Me
DWORD CIOPrinter::GetPrinterStatus(CString sName)
{
	DWORD dwStatus = 0;
	HANDLE h = 0;
	PRINTER_DEFAULTS pd;
	pd.pDatatype     = NULL;
	pd.pDevMode      = NULL;
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(sName.GetBuffer(),&h,&pd) && h)
	{
		DWORD dwSize = 0;
		GetPrinter(h,6,0,0,&dwSize);
		//theLog.Write(_T("GetPrinterStatus %d"),dwSize);
		if (dwSize > 0)
		{
			BYTE* pData = new BYTE[dwSize];
			if(GetPrinter(h,6,pData,dwSize,&dwSize))
			{
				PRINTER_STATUS_BUSY;
				PRINTER_INFO_6* pInfo = (PRINTER_INFO_6*)pData;
				dwStatus = pInfo->dwStatus;
			}
			else
			{
				theLog.Write(_T("getprinter err %d"),::GetLastError());				
			}
			delete[] pData;
		}
		ClosePrinter(h);
	}
	else
	{
		theLog.Write(_T("!!CIOPrinter::GetPrinterStatus,OpenPrinter fail,err=%u,sName=%s"), GetLastError(), sName);
	}
	
	sName.ReleaseBuffer();
	return dwStatus;
}
void CIOPrinter::SetSelPrinter(CString sName)
{
	m_sPrinter = sName;
	if (m_sPrinter.GetLength() <= 0)
	{
		return;
	}
	theLog.Write(_T("CIOPrinter::SetSelPrinter [%s]"),m_sPrinter);

	CPrinterPaperSize::GetInstance().SetPrinterName(m_sPrinter.GetString());	//add by zxl,20150706,获取打印机支持的纸型

	HANDLE h = 0;
	PRINTER_DEFAULTS pd;
	pd.pDatatype     = NULL;
	pd.pDevMode      = NULL;
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(OpenPrinter(sName.GetBuffer(),&h,0) && h)
	{
		/*
		DWORD dwSize = 0;
		GetPrinter(h,2,0,0,&dwSize);
		theLog.Write(_T("SetSelPrinter %d"),dwSize);
		if (dwSize > 0)
		{
			BYTE* pData = new BYTE[dwSize];
			if(GetPrinter(h,2,pData,dwSize,&dwSize))
			{
				PRINTER_STATUS_BUSY;
				DWORD dwNew = PRINTER_STATUS_IO_ACTIVE;
				PRINTER_INFO_2* pInfo = (PRINTER_INFO_2*)pData;
				dwNew = 0;
				BOOL b = 0;//SetPrinter(h,0,(BYTE*)&dwNew,PRINTER_CONTROL_SET_STATUS);
				theLog.Write(_T("print(%s) (%d)(%d),%d"),sName,pInfo->Status,b,::GetLastError());
			}
			else
			{
				theLog.Write(_T("SetSelPrinter err %d"),::GetLastError());
			}
			delete[] pData;
		}*/
		ClosePrinter(h);
	}
	else
	{
		theLog.Write(_T("!!CIOPrinter::SetSelPrinter,OpenPrinter [%s],faile ,%d"),sName,::GetLastError());
		m_sPrinter.Empty();
	}
	sName.ReleaseBuffer();
}

BOOL CIOPrinter::PrintTiffFile(CString sPath,IPrtStationPrintCallback* pChargeCall,int nFrom,int nTo, BOOL bUsePhysicalArea /*= FALSE*/)
{
	if (m_nPrinted > 0)
	{
		return PrintTiffFileBreakPoint(sPath, pChargeCall, nFrom, nTo, bUsePhysicalArea);
	}
	theLog.Write(_T("CIOPrinter::PrintTiffFile,sPath=%s,nFrom=%d,nTo=%d,bUsePhysicalArea=%d"),sPath,nFrom,nTo,bUsePhysicalArea);
	int nErrRet = SP_NOTREPORTED;
	int nPrinted = 0;
	HDC hdcPrinter = 0;
	float dpiRatio = 1.0;
	ImageAttributes *pImgAtt = NULL;
	Graphics* pGraphics = NULL;
	GUID* pDimensionIDs = NULL;

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gisplusStartUpinput;
	GdiplusStartup(&gdiplusToken, &gisplusStartUpinput, NULL);
	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintTiffFile %s"),m_sPrinter);

	DEVNAMES* pDevName = 0;
	BYTE* pDev = 0;
	if(!PrinterUtils::GetPrinterDevMode(m_sPrinter,pDevName,pDev))
	{
		theLog.Write(_T("GetPrinterDevMode err"));
		goto ERR_PRINT;
	}
	if (!pDev)
	{
		theLog.Write(_T("GetPrinterDevMode pDev = null err"));
		goto ERR_PRINT;
	}
	PDEVMODE pDevmodeWork = (PDEVMODE)pDev;
		
	//改变方向
	if(pDevmodeWork->dmFields & DM_ORIENTATION)  
	{
		if (pDevmodeWork->dmOrientation == m_dwOrientation)
		{
			theLog.Write(_T("the printer no change orientation %d"),m_dwOrientation);
		}
		else
		{
			theLog.Write(_T("change the orientation %d"),m_dwOrientation);
			pDevmodeWork->dmOrientation = m_dwOrientation;
		}
	}
	//改变颜色
	DMCOLOR_COLOR;
	if (pDevmodeWork->dmFields & DM_COLOR)
	{
		if (pDevmodeWork->dmColor == m_dwColor)
		{
			theLog.Write(_T("the printer no change color %d"),m_dwColor);
		}
		else
		{
			theLog.Write(_T("change the color %d"),m_dwColor);
			pDevmodeWork->dmColor = m_dwColor;
		}
	}
	else
	{
		theLog.Write(_T("no support color Field, ForceMonochromePrint"));
		m_bForceMonochromePrint = TRUE;
	}

	if (m_bForceMonochromePrint)
	{
		//一般情况下不需要将彩色文件转换成灰度图片,
		//但是,如果打印设备不支持彩色控制的时候,可能使用,
		if (m_dwColor == DMCOLOR_MONOCHROME)
		{
			theLog.Write(_T("change the ImageAttributes to DMCOLOR_MONOCHROME"));
			pImgAtt = new ImageAttributes();
			ASSERT(pImgAtt);
			if(pImgAtt)
				pImgAtt->SetColorMatrix(&colorMatrix, ColorMatrixFlagsSkipGrays, ColorAdjustTypeDefault);
		}
	}

	//改变纸张大小
	DM_PAPERSIZE;
	if (pDevmodeWork->dmFields & DM_PAPERSIZE)
	{
		if (pDevmodeWork->dmPaperSize == m_dwPaperSize)
		{
			theLog.Write(_T("the printer no change papersize %d"),m_dwPaperSize);
		}
		else
		{
			theLog.Write(_T("change the papersize %d"),m_dwPaperSize);
			pDevmodeWork->dmPaperSize = m_dwPaperSize;
		}
	}

	//指定自定义纸张大小打印文档
	if ((m_dwPaperSize>=DMPAPER_USER) && (m_dwPaperWidth>=10) && (m_dwPaperLength>=10))
	{
		theLog.Write(_T("自定义纸张打印 m_dwPaperSize=%d,m_dwPaperWidth=%d,m_dwPaperLength=%d"),m_dwPaperSize,m_dwPaperWidth, m_dwPaperLength);
		if (m_dwPaperWidth > m_dwPaperLength)
		{
			pDevmodeWork->dmOrientation = DMORIENT_LANDSCAPE;
			pDevmodeWork->dmPaperWidth = m_dwPaperLength;
			pDevmodeWork->dmPaperLength = m_dwPaperWidth;
		}
		else
		{
			pDevmodeWork->dmOrientation = DMORIENT_PORTRAIT;
			pDevmodeWork->dmPaperWidth = m_dwPaperWidth;
			pDevmodeWork->dmPaperLength = m_dwPaperLength;
		}
		//dmPaperSize 
		//For printer devices only, selects the size of the paper to print on. 
		//This member can be set to zero if the length and width of the paper are both set by the dmPaperLength and dmPaperWidth members. 
		//Otherwise, the dmPaperSize member can be set to a device specific value greater than or equal to DMPAPER_USER or to one of the following predefined values. 
		pDevmodeWork->dmPaperSize = DMPAPER_USER;
		pDevmodeWork->dmFields |= DM_ORIENTATION | DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	}

	//双面打印
	theLog.Write(_T("dup = %d"),pDevmodeWork->dmDuplex);
	if (DM_DUPLEX & pDevmodeWork->dmFields)
	{
		DMDUP_SIMPLEX;
		DMDUP_VERTICAL;
		DMDUP_HORIZONTAL;
		if (pDevmodeWork->dmDuplex == m_dwDuplex)
		{
			theLog.Write(_T("the printer no change duplex %d"),m_dwDuplex);
		}
		else
		{
			theLog.Write(_T("change the duplex %d"),m_dwDuplex);
			pDevmodeWork->dmDuplex = m_dwDuplex;
		}
	}

	//改变打印份数
	DM_COPIES;
	if (pDevmodeWork->dmFields & DM_COPIES)
	{
		if (pDevmodeWork->dmCopies == m_dwCopies)
		{
			theLog.Write(_T("the printer no change copies %d"),m_dwCopies);
		}
		else
		{
			theLog.Write(_T("change the copies %d"),m_dwCopies);
			pDevmodeWork->dmCopies = m_dwCopies;
		}
	}

	//逐份打印
	DM_COLLATE;
	if (pDevmodeWork->dmFields & DM_COLLATE)
	{
		if (pDevmodeWork->dmCollate == m_dwCollate)
		{
			theLog.Write(_T("the printer no change collate %d"), m_dwCollate);
		}
		else
		{
			theLog.Write(_T("change the collate %d"), m_dwCollate);
			pDevmodeWork->dmCollate = m_dwCollate;
		}
	}

	//纸张来源
	if (IsEnableDefaultSource() && (DM_DEFAULTSOURCE & pDevmodeWork->dmFields))
	{
		if (pDevmodeWork->dmDefaultSource == m_dwDefaultSource)
		{
			theLog.Write(_T("the printer no change DefaultSource %d"),m_dwDefaultSource);
		}
		else
		{
			theLog.Write(_T("change the DefaultSource %d"),m_dwDefaultSource);
			pDevmodeWork->dmDefaultSource = m_dwDefaultSource;
		}
	}

	hdcPrinter = ::CreateDC((LPCTSTR)pDevName + pDevName->wDriverOffset,
		(LPCTSTR)pDevName + pDevName->wDeviceOffset,
		(LPCTSTR)pDevName + pDevName->wOutputOffset,
		(DEVMODE*)pDev);
	if (hdcPrinter == NULL)
	{
		theLog.Write(_T("!!CIOPrinter::PrintTiffFile,11,hdcPrinter = null,error=%u"), GetLastError());
		goto ERR_PRINT;
	}
	else
	{		
		// call StartDoc() to begin printing
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDatatype = _T("RAW");
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = m_szDocName.GetString();

		// if it fails, complain and exit gracefully
		nErrRet = StartDoc(hdcPrinter, &docinfo);
		if (nErrRet <= 0)
		{
			SP_ERROR;	
			theLog.Write(_T("!!CIOPrinter::PrintTiffFile,12,StartDoc failed, nErrRet=%d, err=%d, hdcPrinter=%x, lpszDocName=%s"), nErrRet, GetLastError(), hdcPrinter, docinfo.lpszDocName);
			goto ERR_PRINT;
		}
		
		CStringW wsPath(sPath);
		Image image(wsPath);
		UINT count = 0;
		count = image.GetFrameDimensionsCount();
		theLog.Write(_T("The number of dimensions is %d"), count);
		if (count <= 0)
		{
			SP_ERROR;	
			theLog.Write(_T("!!CIOPrinter::PrintTiffFile,13,GetFrameDimensionsCount failed"));
			goto ERR_PRINT;
		}

		pDimensionIDs = (GUID*)malloc(sizeof(GUID)*count);

		// Get the list of frame dimensions from the Image object.
		image.GetFrameDimensionsList(pDimensionIDs, count);
		for (int n = 0 ; n < count; n ++)
		{
			// Display the GUID of the first (and only) frame dimension.
			WCHAR strGuid[39];
			StringFromGUID2(pDimensionIDs[n], strGuid, 39);
			theLog.Write(_T("The first (and only) dimension ID is %s."), strGuid);

			// Get the number of frames in the first dimension.
			UINT frameCount = image.GetFrameCount(&pDimensionIDs[n]);
			theLog.Write(_T("The number of frames in that dimension is %d."), frameCount);

			if(nFrom >= 0 && nTo >= nFrom && nTo < frameCount)
			{
				theLog.Write(_T("print tiff from = %d to = %d ,copy = %d"),nFrom,nTo,m_dwCopies);
				m_nCurrentPage = 0;
				BOOL bAllInOne = (m_dwPagesPerPaper == 1) ? FALSE : TRUE;	//是否多合一打印
				for (int m = nFrom; m <= nTo; m ++)
				{
					BOOL bBeginPage = (!bAllInOne || ((m_nCurrentPage % m_dwPagesPerPaper) == 0)) ? TRUE : FALSE;	//是否一张纸起始内容页
					BOOL bEndPage = (!bAllInOne || (m == nTo) || (((m_nCurrentPage+1) % m_dwPagesPerPaper) == 0)) ? TRUE : FALSE;	//是否一张纸结束内容页
					theLog.Write(_T("print tiff nFrom = %d, m = %d, bBeginPage=%d, bEndPage=%d"),nFrom,m,bBeginPage,bEndPage);

					image.SelectActiveFrame(&FrameDimensionPage,m);

					if (!bAllInOne)
					{
						ResetPrinterHDC(hdcPrinter, pDevmodeWork, image); //重置HDC
					}

					if (bBeginPage)
					{
						nErrRet = StartPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							theLog.Write(_T("StartPage failed"));
							goto ERR_PRINT;
						}

						pGraphics = Graphics::FromHDC(hdcPrinter);
						pGraphics->SetSmoothingMode(SmoothingModeHighQuality);
						pGraphics->SetInterpolationMode(InterpolationModeHighQuality); 
						pGraphics->SetPageUnit(UnitPixel);
						pGraphics->SetPageScale(1.0);
					}

					GraphicsState state = pGraphics->Save();

					Rect rc;
					GetPrintDeviceArea(hdcPrinter, image, rc, *pGraphics, bUsePhysicalArea);	//获取打印区域
					int nPictureWidth = image.GetWidth();
					int nPictureHeight = image.GetHeight();
					theLog.Write(_T("print tiff rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
						,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

					if (IsWater())
					{
						CPrintWithWaterMarked water;
						water.Init(&image,&m_oSec);
						water.DrawImg(&image);
						pGraphics->DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}
					else
					{
						pGraphics->DrawImage(&image, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}
					
					pGraphics->Restore(state);

					if (bEndPage)
					{
						pGraphics->ReleaseHDC(hdcPrinter);
						delete pGraphics;
						pGraphics = NULL;

						nErrRet = EndPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							theLog.Write(_T("EndPage failed"));
							goto ERR_PRINT;
						}
						nPrinted++;
						//回调通知更新界面，通知服务器扣费
						if(pChargeCall)
						{
							pChargeCall->OnNewPagePrinted(m);
						}
					}
					m_nCurrentPage++;
				}
			}
		}

		nErrRet = EndDoc(hdcPrinter);
		if (nErrRet <= 0)
		{
		    theLog.Write(_T("EndDoc failed"));
			goto ERR_PRINT;
		}

		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nPrinted*m_dwCopies);
		}
	}
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pGraphics)
	{
		delete pGraphics;
	}
	if (pDimensionIDs)
	{
		free(pDimensionIDs);
	}
	GdiplusShutdown(gdiplusToken);

	return TRUE;

ERR_PRINT:
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pGraphics)
	{
		delete pGraphics;
	}
	if (pDimensionIDs)
	{
		free(pDimensionIDs);
	}

	theLog.Write(_T("print tiff err:%d,prined:%d"),nErrRet,nPrinted);
	if (pChargeCall)
	{
		pChargeCall->OnPrintedErr(nPrinted*m_dwCopies,nErrRet);
	}
	GdiplusShutdown(gdiplusToken);
	return FALSE;
}

BOOL CIOPrinter::PrintTiffFileBreakPoint(CString sPath,IPrtStationPrintCallback* pChargeCall,int nFrom,int nTo, BOOL bUsePhysicalArea /*= FALSE*/)
{
	theLog.Write(_T("CIOPrinter::PrintTiffFileBreakPoint,sPath=%s,nFrom=%d,nTo=%d,nType=%d,bUsePhysicalArea=%d"),sPath,nFrom,nTo,bUsePhysicalArea);
	int nErrRet = SP_NOTREPORTED;
	int nPrinted = 0;
	HDC hdcPrinter = 0;
	float dpiRatio = 1.0;
	ImageAttributes *pImgAtt = NULL;
	GUID* pDimensionIDs = NULL;

	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintTiffFileBreakPoint %s"),m_sPrinter);

	DEVNAMES* pDevName = 0;
	BYTE* pDev = 0;
	if(!PrinterUtils::GetPrinterDevMode(m_sPrinter,pDevName,pDev))
	{
		theLog.Write(_T("GetPrinterDevMode err"));
		goto ERR_PRINT;
	}
	if (!pDev)
	{
		theLog.Write(_T("GetPrinterDevMode pDev = null err"));
		goto ERR_PRINT;
	}
	PDEVMODE pDevmodeWork = (PDEVMODE)pDev;

	dpiRatio=300.0f/pDevmodeWork->dmPrintQuality;
	theLog.Write(_T(" dpiRatio = %0.2f"),dpiRatio);
	//改变方向
	if(pDevmodeWork->dmFields & DM_ORIENTATION)  
	{
		if (pDevmodeWork->dmOrientation == m_dwOrientation)
		{
			theLog.Write(_T("the printer no change orientation %d"),m_dwOrientation);
		}
		else
		{
			theLog.Write(_T("change the orientation %d"),m_dwOrientation);
			pDevmodeWork->dmOrientation = m_dwOrientation;
		}
	}
	//改变颜色
	DMCOLOR_COLOR;
	if (pDevmodeWork->dmFields & DM_COLOR)
	{
		if (pDevmodeWork->dmColor == m_dwColor)
		{
			theLog.Write(_T("the printer no change color %d"),m_dwColor);
		}
		else
		{
			theLog.Write(_T("change the color %d"),m_dwColor);
			pDevmodeWork->dmColor = m_dwColor;
		}
	}
	else
	{
		theLog.Write(_T("no support color Field, ForceMonochromePrint"));
		m_bForceMonochromePrint = TRUE;
	}

	if (m_bForceMonochromePrint)
	{
		//一般情况下不需要将彩色文件转换成灰度图片,
		//但是,如果打印设备不支持彩色控制的时候,可能使用,
		if (m_dwColor == DMCOLOR_MONOCHROME)
		{
			theLog.Write(_T("change the ImageAttributes to DMCOLOR_MONOCHROME"));
			pImgAtt = new ImageAttributes();
			ASSERT(pImgAtt);
			pImgAtt->SetColorMatrix(&colorMatrix, ColorMatrixFlagsSkipGrays, ColorAdjustTypeDefault);
		}
	}

	//改变纸张大小
	DM_PAPERSIZE;
	if (pDevmodeWork->dmFields & DM_PAPERSIZE)
	{
		if (pDevmodeWork->dmPaperSize  == m_dwPaperSize)
		{
			theLog.Write(_T("the printer no change papersize %d"),m_dwPaperSize);
		}
		else
		{
			theLog.Write(_T("change the papersize %d"),m_dwPaperSize);
			pDevmodeWork->dmPaperSize  = m_dwPaperSize;
		}
	}

	//指定自定义纸张大小打印文档
	if ((m_dwPaperSize>=DMPAPER_USER) && (m_dwPaperWidth>=10) && (m_dwPaperLength>=10))
	{
		theLog.Write(_T("自定义纸张打印 m_dwPaperSize=%d,m_dwPaperWidth=%d,m_dwPaperLength=%d"),m_dwPaperSize,m_dwPaperWidth, m_dwPaperLength);
		if (m_dwPaperWidth > m_dwPaperLength)
		{
			pDevmodeWork->dmOrientation = DMORIENT_LANDSCAPE;
			pDevmodeWork->dmPaperWidth = m_dwPaperLength;
			pDevmodeWork->dmPaperLength = m_dwPaperWidth;
		}
		else
		{
			pDevmodeWork->dmOrientation = DMORIENT_PORTRAIT;
			pDevmodeWork->dmPaperWidth = m_dwPaperWidth;
			pDevmodeWork->dmPaperLength = m_dwPaperLength;
		}
		pDevmodeWork->dmPaperSize = DMPAPER_USER;
		pDevmodeWork->dmFields |= DM_ORIENTATION | DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	}

	//双面打印
	theLog.Write(_T("dup = %d"),pDevmodeWork->dmDuplex);
	if (DM_DUPLEX & pDevmodeWork->dmFields)
	{
		DMDUP_SIMPLEX;
		DMDUP_VERTICAL;
		DMDUP_HORIZONTAL;
		if (pDevmodeWork->dmDuplex == m_dwDuplex)
		{
			theLog.Write(_T("the printer no change duplex %d"),m_dwDuplex);
		}
		else
		{
			theLog.Write(_T("change the duplex %d"),m_dwDuplex);
			pDevmodeWork->dmDuplex = m_dwDuplex;
		}
	}

#if 0
	//改变打印份数
	DM_COPIES;
	if (pDevmodeWork->dmFields & DM_COPIES)
	{
		if (pDevmodeWork->dmCopies == m_dwCopies)
		{
			theLog.Write(_T("the printer no change copies %d"),m_dwCopies);
		}
		else
		{
			theLog.Write(_T("change the copies %d"),m_dwCopies);
			pDevmodeWork->dmCopies = m_dwCopies;
		}
	}
#endif

	//逐份打印
	DM_COLLATE;
	if (pDevmodeWork->dmFields & DM_COLLATE)
	{
		if (pDevmodeWork->dmCollate == m_dwCollate)
		{
			theLog.Write(_T("the printer no change collate %d"), m_dwCollate);
		}
		else
		{
			theLog.Write(_T("change the collate %d"), m_dwCollate);
			pDevmodeWork->dmCollate = m_dwCollate;
		}
	}

	//纸张来源
	if (IsEnableDefaultSource() && (DM_DEFAULTSOURCE & pDevmodeWork->dmFields))
	{
		if (pDevmodeWork->dmDefaultSource == m_dwDefaultSource)
		{
			theLog.Write(_T("the printer no change DefaultSource %d"),m_dwDefaultSource);
		}
		else
		{
			theLog.Write(_T("change the DefaultSource %d"),m_dwDefaultSource);
			pDevmodeWork->dmDefaultSource = m_dwDefaultSource;
		}
	}

	hdcPrinter = ::CreateDC((LPCTSTR)pDevName + pDevName->wDriverOffset,
		(LPCTSTR)pDevName + pDevName->wDeviceOffset,
		(LPCTSTR)pDevName + pDevName->wOutputOffset,
		(DEVMODE*)pDev);
	if (hdcPrinter == NULL)
	{
		theLog.Write(_T("hdcPrinter = null,err=%u"),GetLastError());
		goto ERR_PRINT;
	}
	else
	{		
		// call StartDoc() to begin printing
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDatatype = _T("RAW");
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = m_szDocName.GetString();

		// if it fails, complain and exit gracefully
		nErrRet = StartDoc(hdcPrinter, &docinfo);
		if (nErrRet <= 0)
		{
			SP_ERROR;			
			goto ERR_PRINT;
		}

		CStringW wsPath(sPath);
		Image image(wsPath);
		UINT count = 0;
		count = image.GetFrameDimensionsCount();
		theLog.Write(_T("The number of dimensions is %d"), count);
		if (count <= 0)
		{
			SP_ERROR;	
			theLog.Write(_T("!!print tiff,GetFrameDimensionsCount failed"));
			goto ERR_PRINT;
		}

		pDimensionIDs = (GUID*)malloc(sizeof(GUID)*count);

		// Get the list of frame dimensions from the Image object.
		image.GetFrameDimensionsList(pDimensionIDs, count);
		for (int n = 0 ; n < count; n ++)
		{
			// Display the GUID of the first (and only) frame dimension.
			WCHAR strGuid[39];
			StringFromGUID2(pDimensionIDs[n], strGuid, 39);
			theLog.Write(_T("The first (and only) dimension ID is %s."), strGuid);

			// Get the number of frames in the first dimension.
			UINT frameCount = image.GetFrameCount(&pDimensionIDs[n]);
			theLog.Write(_T("The number of frames in that dimension is %d."), frameCount);

			if((nFrom == -1 && nTo == -1))
			{
				nFrom = 0;
				nTo = frameCount - 1;
			}

			int nFrom2 = 0;
			int nCopy = m_dwCopies;
			if (nCopy == 1)
			{
				nCopy = (m_nPrinted > 0) ? 0 : 1;
				nFrom2 = m_nPrinted;
			}
			else
			{
				int nPagePerCopy = nTo - nFrom + 1;
				int nTotalPage = nCopy * nPagePerCopy;
				int nLeftPage = nTotalPage - m_nPrinted;
				nCopy = nLeftPage / nPagePerCopy;
				int nLeftPage2 = nLeftPage - (nCopy * nPagePerCopy);
				nFrom2 = nPagePerCopy - nLeftPage2;
			}

			if(nFrom >= 0 && nTo >= nFrom && nTo < frameCount)
			{
				theLog.Write(_T("print tiff from = %d to = %d ,copy = %d, nFrom2=%d"),nFrom,nTo,nCopy, nFrom2);
				for (int i=nFrom2; i <= nTo; i++)
				{
					theLog.Write(_T("print tiff nFrom2 = %d i = %d"),nFrom2,i);

					image.SelectActiveFrame(&FrameDimensionPage,i);

					ResetPrinterHDC(hdcPrinter, pDevmodeWork, image); //重置HDC

					nErrRet = StartPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}

					Graphics graphics(hdcPrinter);
					graphics.SetSmoothingMode(SmoothingModeHighQuality);
					graphics.SetInterpolationMode(InterpolationModeHighQuality); 
					graphics.SetPageUnit(UnitPixel);
					graphics.SetPageScale(1.0);

					Rect rc;
					GetPrintDeviceArea(hdcPrinter, image, rc, graphics, bUsePhysicalArea);	//获取打印区域

					int nPictureWidth = image.GetWidth();
					int nPictureHeight = image.GetHeight();
					theLog.Write(_T("print tiff AA rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
						,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

					//添加水印
					if (IsWater())
					{
						CPrintWithWaterMarked water;
						water.Init(&image,&m_oSec);
						water.DrawImg(&image);
						graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}
					else
					{
						graphics.DrawImage(&image, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}

					graphics.ReleaseHDC(hdcPrinter);

					nErrRet = EndPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}
					nPrinted++;
					//回调通知更新界面，通知服务器扣费
					if(pChargeCall)
					{
						pChargeCall->OnNewPagePrinted(nFrom2);
					}
				}		

				for (int x = 0; x < nCopy; x++ )
				{
					theLog.Write(_T("print tiff TotalCopy=%d, CurrentCopy=%d"),nCopy, x);
					for (int m = nFrom; m <= nTo; m ++)
					{
						theLog.Write(_T("print tiff nFrom = %d, m = %d"),nFrom,m);

						image.SelectActiveFrame(&FrameDimensionPage,m);

						ResetPrinterHDC(hdcPrinter, pDevmodeWork, image); //重置HDC

						nErrRet = StartPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}

						Graphics graphics(hdcPrinter);
						graphics.SetSmoothingMode(SmoothingModeHighQuality);
						graphics.SetInterpolationMode(InterpolationModeHighQuality); 
						graphics.SetPageUnit(UnitPixel);
						graphics.SetPageScale(1.0);

						Rect rc;
						GetPrintDeviceArea(hdcPrinter, image, rc, graphics, bUsePhysicalArea);	//获取打印区域

						int nPictureWidth = image.GetWidth();
						int nPictureHeight = image.GetHeight();
						theLog.Write(_T("print tiff BB rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
							,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

						//添加水印
						if (IsWater())
						{
							CPrintWithWaterMarked water;
							water.Init(&image,&m_oSec);
							water.DrawImg(&image);
							graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
						}
						else
						{
							graphics.DrawImage(&image, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
						}
						
						graphics.ReleaseHDC(hdcPrinter);

						nErrRet = EndPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}
						nPrinted++;
						//回调通知更新界面，通知服务器扣费
						if(pChargeCall)
						{
							pChargeCall->OnNewPagePrinted(m);
						}
					}
				}
			
			}
			else
			{
				theLog.Write(_T("!!print tiff,nFrom=%d, nTo=%d,frameCount=%d"), nFrom, nTo, frameCount);
				ASSERT(0);
			}
		}


		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nPrinted);
		}
		nErrRet = EndDoc(hdcPrinter);
		if (nErrRet <= 0)
		{
			goto ERR_PRINT;
		}

	}
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pDimensionIDs)
	{
		free(pDimensionIDs);
	}

	return TRUE;

ERR_PRINT:
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pDimensionIDs)
	{
		free(pDimensionIDs);
	}

	theLog.Write(_T("print tiff err:%d,prined:%d"),nErrRet,nPrinted);
	if (pChargeCall)
	{
		pChargeCall->OnPrintedErr(nPrinted,nErrRet);
	}
	return FALSE;
}

BOOL CIOPrinter::PrintEmfFile(CString sPath,IPrtStationPrintCallback* pChargeCall,int nFrom/* = -1*/,int nTo/* = -1*/)
{
	if (m_nPrinted > 0)
	{
		return PrintEmfFileBreakPoint(sPath, pChargeCall, nFrom, nTo);
	}
	theLog.Write(_T("CIOPrinter::PrintEmfFile,1,sPath=%s,nFrom=%d,nTo=%d"),sPath,nFrom,nTo);
	int nErrRet = SP_NOTREPORTED;
	int nPrinted = 0;
	HDC hdcPrinter = 0;
	ImageAttributes *pImgAtt = NULL;
	Graphics* pGraphics = NULL;

	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintEmfFile,2,Printer=[%s]"),m_sPrinter);

	DEVNAMES* pDevName = 0;
	BYTE* pDev = 0;
	if(!PrinterUtils::GetPrinterDevMode(m_sPrinter,pDevName,pDev))
	{
		theLog.Write(_T("GetPrinterDevMode err"));
		goto ERR_PRINT;
	}
	if (!pDev)
	{
		theLog.Write(_T("GetPrinterDevMode pDev = null err"));
		goto ERR_PRINT;
	}
	PDEVMODE pDevmodeWork = (PDEVMODE)pDev;

	//改变方向
	if(pDevmodeWork->dmFields & DM_ORIENTATION)  
	{
		if (pDevmodeWork->dmOrientation == m_dwOrientation)
		{
			theLog.Write(_T("the printer no change orientation %d"),m_dwOrientation);
		}
		else
		{
			theLog.Write(_T("change the orientation"));
			pDevmodeWork->dmOrientation = m_dwOrientation;
		}
	}

	//改变颜色
	DMCOLOR_COLOR;
	if (pDevmodeWork->dmFields & DM_COLOR)
	{
		if (pDevmodeWork->dmColor == m_dwColor)
		{
			theLog.Write(_T("the printer no change color %d"),m_dwColor);
		}
		else
		{
			theLog.Write(_T("change the color %d"),m_dwColor);
			pDevmodeWork->dmColor = m_dwColor;
		}
	}
	else
	{
		theLog.Write(_T("no support color Field, ForceMonochromePrint"));
		m_bForceMonochromePrint = TRUE;
	}

	if (m_bForceMonochromePrint)
	{
		//一般情况下不需要将彩色文件转换成灰度图片,
		//但是,如果打印设备不支持彩色控制的时候,可能使用,
		if (m_dwColor == DMCOLOR_MONOCHROME)
		{
			theLog.Write(_T("change the ImageAttributes to DMCOLOR_MONOCHROME"));
			pImgAtt = new ImageAttributes();
			ASSERT(pImgAtt);
			pImgAtt->SetColorMatrix(&colorMatrix, ColorMatrixFlagsSkipGrays, ColorAdjustTypeDefault);
		}
	}

	//改变纸张大小
	DM_PAPERSIZE;
	if (pDevmodeWork->dmFields & DM_PAPERSIZE)
	{
		if (pDevmodeWork->dmPaperSize  == m_dwPaperSize)
		{
			theLog.Write(_T("the printer no change papersize %d"),m_dwPaperSize);
		}
		else
		{
			theLog.Write(_T("change the papersize %d"),m_dwPaperSize);
			pDevmodeWork->dmPaperSize  = m_dwPaperSize;
		}
	}

	//指定自定义纸张大小打印文档
	if ((m_dwPaperSize>=DMPAPER_USER) && (m_dwPaperWidth>=10) && (m_dwPaperLength>=10))
	{
		theLog.Write(_T("自定义纸张打印 m_dwPaperSize=%d,m_dwPaperWidth=%d,m_dwPaperLength=%d"),m_dwPaperSize,m_dwPaperWidth, m_dwPaperLength);
		if (m_dwPaperWidth > m_dwPaperLength)
		{
			pDevmodeWork->dmOrientation = DMORIENT_LANDSCAPE;
			pDevmodeWork->dmPaperWidth = m_dwPaperLength;
			pDevmodeWork->dmPaperLength = m_dwPaperWidth;
		}
		else
		{
			pDevmodeWork->dmOrientation = DMORIENT_PORTRAIT;
			pDevmodeWork->dmPaperWidth = m_dwPaperWidth;
			pDevmodeWork->dmPaperLength = m_dwPaperLength;
		}
		pDevmodeWork->dmPaperSize = DMPAPER_USER;
		pDevmodeWork->dmFields |= DM_ORIENTATION | DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	}

	//双面打印
	theLog.Write(_T("dup = %d"),pDevmodeWork->dmDuplex);
	if (DM_DUPLEX & pDevmodeWork->dmFields)
	{
		DMDUP_SIMPLEX;
		if (pDevmodeWork->dmDuplex == m_dwDuplex)
		{
			theLog.Write(_T("the printer no change duplex %d"),m_dwDuplex);
		}
		else
		{
			theLog.Write(_T("change the duplex %d"),m_dwDuplex);
			pDevmodeWork->dmDuplex = m_dwDuplex;
		}
	}

	//改变打印份数
	DM_COPIES;
	if (pDevmodeWork->dmFields & DM_COPIES)
	{
		if (pDevmodeWork->dmCopies == m_dwCopies)
		{
			theLog.Write(_T("the printer no change copies %d"),m_dwCopies);
		}
		else
		{
			theLog.Write(_T("change the copies %d"),m_dwCopies);
			pDevmodeWork->dmCopies = m_dwCopies;
		}
	}

	//逐份打印
	DM_COLLATE;
	if (pDevmodeWork->dmFields & DM_COLLATE)
	{
		if (pDevmodeWork->dmCollate == m_dwCollate)
		{
			theLog.Write(_T("the printer no change collate %d"), m_dwCollate);
		}
		else
		{
			theLog.Write(_T("change the collate %d"), m_dwCollate);
			pDevmodeWork->dmCollate = m_dwCollate;
		}
	}

	//选择打印纸张来源
	DM_DEFAULTSOURCE;
	if (IsEnableDefaultSource() && (pDevmodeWork->dmFields & DM_DEFAULTSOURCE))
	{
		if (pDevmodeWork->dmDefaultSource == m_dwDefaultSource)
		{
			theLog.Write(_T("the printer no change paper source %d"),m_dwDefaultSource);
		}
		else
		{
			theLog.Write(_T("change the paper source %d"),m_dwDefaultSource);
			pDevmodeWork->dmDefaultSource = m_dwDefaultSource;
		}
	}

	hdcPrinter = ::CreateDC((LPCTSTR)pDevName + pDevName->wDriverOffset,
		(LPCTSTR)pDevName + pDevName->wDeviceOffset,
		(LPCTSTR)pDevName + pDevName->wOutputOffset,
		(DEVMODE*)pDev);
	if (hdcPrinter == NULL)
	{
		theLog.Write(_T("hdcPrinter = null"));
		goto ERR_PRINT;
	}
	else
	{		
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDatatype = _T("RAW");
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = m_szDocName.GetString();

		// call StartDoc() to begin printing
		nErrRet = StartDoc(hdcPrinter, &docinfo);
		if (nErrRet <= 0)
		{
			SP_ERROR;			
			goto ERR_PRINT;
		}

		CParseSpl spl;
		if (spl.InitParse(sPath, TRUE))
		{
			DWORD dwPagePerCopy = spl.GetPageCountPerCopy();
			if(nFrom >= 0 && nTo >= nFrom && nTo < dwPagePerCopy)
			{
				theLog.Write(_T("print emf from = %d to = %d ,copy = %d"),nFrom,nTo,m_dwCopies);
				m_nCurrentPage = 0;
				BOOL bAllInOne = (m_dwPagesPerPaper == 1) ? FALSE : TRUE;	//是否多合一打印
				for (int m = nFrom; m <= nTo; m ++)
				{
					BOOL bBeginPage = (!bAllInOne || ((m_nCurrentPage % m_dwPagesPerPaper) == 0)) ? TRUE : FALSE;	//是否一张纸起始内容页
					BOOL bEndPage = (!bAllInOne || (m == nTo) || (((m_nCurrentPage+1) % m_dwPagesPerPaper) == 0)) ? TRUE : FALSE;	//是否一张纸结束内容页
					theLog.Write(_T("print emf nFrom = %d, m = %d, bBeginPage=%d, bEndPage=%d"),nFrom,m,bBeginPage,bEndPage);
					HENHMETAFILE hEmf = GetEmf(spl, m);
					if (hEmf == NULL)
					{
						theLog.Write(_T("!!hEmf == NULL, pageindex=%d"), m);
						goto ERR_PRINT;
					}
					ENHMETAHEADER emfheader;
					GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &emfheader);
					Metafile mf(hEmf, TRUE);
					if (!bAllInOne)
					{
						ResetPrinterHDC(hdcPrinter, pDevmodeWork, &emfheader); //重置HDC
					}

					if (bBeginPage)
					{
						nErrRet = StartPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}

						pGraphics = Graphics::FromHDC(hdcPrinter);
						pGraphics->SetSmoothingMode(SmoothingModeHighQuality);
						pGraphics->SetInterpolationMode(InterpolationModeHighQuality); 
						pGraphics->SetPageUnit(UnitPixel);
						pGraphics->SetPageScale(1.0);
					}

					GraphicsState state = pGraphics->Save();

					Rect rc;
					GetPrintDeviceArea(hdcPrinter, mf, rc, *pGraphics);	//获取打印区域
					int nPictureWidth = mf.GetWidth();
					int nPictureHeight = mf.GetHeight();
					theLog.Write(_T("print emf rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
						,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

					if (IsWater())
					{
						CPrintWithWaterMarked water;
						water.Init(&mf,&m_oSec);
						water.DrawImg(&mf);
						pGraphics->DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}
					else
					{
						pGraphics->DrawImage(&mf, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}

					pGraphics->Restore(state);

					if (bEndPage)
					{
						pGraphics->ReleaseHDC(hdcPrinter);
						delete pGraphics;
						pGraphics = NULL;

						nErrRet = EndPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							theLog.Write(_T("EndPage failed"));
							goto ERR_PRINT;
						}

						nPrinted++;
						//回调通知更新界面，通知服务器扣费
						if(pChargeCall)
						{
							pChargeCall->OnNewPagePrinted(m);
						}
					}
					m_nCurrentPage++;
				}
			}
			else
			{
				theLog.Write(_T("!!nFrom=%d,nTo=%d,dwPagePerCopy=%d"), nFrom, nTo, dwPagePerCopy);
				ASSERT(0);
			}
		}
		else
		{
			theLog.Write(_T("!!spl InitParse fail.sPath=[%s]"), sPath);	
		}

		nErrRet = EndDoc(hdcPrinter);
		if (nErrRet <= 0)
		{
			goto ERR_PRINT;
		}

		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nPrinted * m_dwCopies);
		}
	}
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pGraphics)
	{
		delete pGraphics;
	}

	return TRUE;

ERR_PRINT:
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	if (pGraphics)
	{
		delete pGraphics;
	}

	theLog.Write(_T("print emf err:%d,prined:%d"),nErrRet,nPrinted);
	if (pChargeCall)
	{
		pChargeCall->OnPrintedErr(nPrinted * m_dwCopies,nErrRet);
	}
	return FALSE;
}

BOOL CIOPrinter::PrintEmfFileBreakPoint(CString sPath,IPrtStationPrintCallback* pChargeCall,int nFrom/* = -1*/,int nTo/* = -1*/)
{
	theLog.Write(_T("CIOPrinter::PrintEmfFileBreakPoint,1,sPath=%s,nFrom=%d,nTo=%d"),sPath,nFrom,nTo);
	int nErrRet = SP_NOTREPORTED;
	int nPrinted = 0;
	HDC hdcPrinter = 0;
	ImageAttributes *pImgAtt = NULL;

	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintEmfFileBreakPoint,2,Printer=[%s]"),m_sPrinter);

	DEVNAMES* pDevName = 0;
	BYTE* pDev = 0;
	if(!PrinterUtils::GetPrinterDevMode(m_sPrinter,pDevName,pDev))
	{
		theLog.Write(_T("GetPrinterDevMode err"));
		goto ERR_PRINT;
	}
	if (!pDev)
	{
		theLog.Write(_T("GetPrinterDevMode pDev = null err"));
		goto ERR_PRINT;
	}
	PDEVMODE pDevmodeWork = (PDEVMODE)pDev;

	//改变方向
	if(pDevmodeWork->dmFields & DM_ORIENTATION)  
	{
		if (pDevmodeWork->dmOrientation == m_dwOrientation)
		{
			theLog.Write(_T("the printer no change orientation %d"),m_dwOrientation);
		}
		else
		{
			theLog.Write(_T("change the orientation"));
			pDevmodeWork->dmOrientation = m_dwOrientation;
		}
	}

	//改变颜色
	DMCOLOR_COLOR;
	if (pDevmodeWork->dmFields & DM_COLOR)
	{
		if (pDevmodeWork->dmColor == m_dwColor)
		{
			theLog.Write(_T("the printer no change color %d"),m_dwColor);
		}
		else
		{
			theLog.Write(_T("change the color %d"),m_dwColor);
			pDevmodeWork->dmColor = m_dwColor;
		}
	}
	else
	{
		theLog.Write(_T("no support color Field, ForceMonochromePrint"));
		m_bForceMonochromePrint = TRUE;
	}

	if (m_bForceMonochromePrint)
	{
		//一般情况下不需要将彩色文件转换成灰度图片,
		//但是,如果打印设备不支持彩色控制的时候,可能使用,
		if (m_dwColor == DMCOLOR_MONOCHROME)
		{
			theLog.Write(_T("change the ImageAttributes to DMCOLOR_MONOCHROME"));
			pImgAtt = new ImageAttributes();
			ASSERT(pImgAtt);
			pImgAtt->SetColorMatrix(&colorMatrix, ColorMatrixFlagsSkipGrays, ColorAdjustTypeDefault);
		}
	}

	//改变纸张大小
	DM_PAPERSIZE;
	if (pDevmodeWork->dmFields & DM_PAPERSIZE)
	{
		if (pDevmodeWork->dmPaperSize  == m_dwPaperSize)
		{
			theLog.Write(_T("the printer no change papersize %d"),m_dwPaperSize);
		}
		else
		{
			theLog.Write(_T("change the papersize %d"),m_dwPaperSize);
			pDevmodeWork->dmPaperSize  = m_dwPaperSize;
		}
	}

	//指定自定义纸张大小打印文档
	if ((m_dwPaperSize>=DMPAPER_USER) && (m_dwPaperWidth>=10) && (m_dwPaperLength>=10))
	{
		theLog.Write(_T("自定义纸张打印 m_dwPaperSize=%d,m_dwPaperWidth=%d,m_dwPaperLength=%d"),m_dwPaperSize,m_dwPaperWidth, m_dwPaperLength);
		if (m_dwPaperWidth > m_dwPaperLength)
		{
			pDevmodeWork->dmOrientation = DMORIENT_LANDSCAPE;
			pDevmodeWork->dmPaperWidth = m_dwPaperLength;
			pDevmodeWork->dmPaperLength = m_dwPaperWidth;
		}
		else
		{
			pDevmodeWork->dmOrientation = DMORIENT_PORTRAIT;
			pDevmodeWork->dmPaperWidth = m_dwPaperWidth;
			pDevmodeWork->dmPaperLength = m_dwPaperLength;
		}
		pDevmodeWork->dmPaperSize = DMPAPER_USER;
		pDevmodeWork->dmFields |= DM_ORIENTATION | DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	}

	//双面打印
	theLog.Write(_T("dup = %d"),pDevmodeWork->dmDuplex);
	if (DM_DUPLEX & pDevmodeWork->dmFields)
	{
		DMDUP_SIMPLEX;
		if (pDevmodeWork->dmDuplex == m_dwDuplex)
		{
			theLog.Write(_T("the printer no change duplex %d"),m_dwDuplex);
		}
		else
		{
			theLog.Write(_T("change the duplex %d"),m_dwDuplex);
			pDevmodeWork->dmDuplex = m_dwDuplex;
		}
	}

#if 0
	//改变打印份数
	DM_COPIES;
	if (pDevmodeWork->dmFields & DM_COPIES)
	{
		if (pDevmodeWork->dmCopies == m_dwCopies)
		{
			theLog.Write(_T("the printer no change copies %d"),m_dwCopies);
		}
		else
		{
			theLog.Write(_T("change the copies %d"),m_dwCopies);
			pDevmodeWork->dmCopies = m_dwCopies;
		}
	}
#endif

	//逐份打印
	DM_COLLATE;
	if (pDevmodeWork->dmFields & DM_COLLATE)
	{
		if (pDevmodeWork->dmCollate == m_dwCollate)
		{
			theLog.Write(_T("the printer no change collate %d"), m_dwCollate);
		}
		else
		{
			theLog.Write(_T("change the collate %d"), m_dwCollate);
			pDevmodeWork->dmCollate = m_dwCollate;
		}
	}

	//选择打印纸张来源
	DM_DEFAULTSOURCE;
	if (IsEnableDefaultSource() && (pDevmodeWork->dmFields & DM_DEFAULTSOURCE))
	{
		if (pDevmodeWork->dmDefaultSource == m_dwDefaultSource)
		{
			theLog.Write(_T("the printer no change paper source %d"),m_dwDefaultSource);
		}
		else
		{
			theLog.Write(_T("change the paper source %d"),m_dwDefaultSource);
			pDevmodeWork->dmDefaultSource = m_dwDefaultSource;
		}
	}

	hdcPrinter = ::CreateDC((LPCTSTR)pDevName + pDevName->wDriverOffset,
		(LPCTSTR)pDevName + pDevName->wDeviceOffset,
		(LPCTSTR)pDevName + pDevName->wOutputOffset,
		(DEVMODE*)pDev);
	if (hdcPrinter == NULL)
	{
		theLog.Write(_T("hdcPrinter = null"));
		goto ERR_PRINT;
	}
	else
	{		
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDatatype = _T("RAW");
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = m_szDocName.GetString();

		// call StartDoc() to begin printing
		nErrRet = StartDoc(hdcPrinter, &docinfo);
		if (nErrRet <= 0)
		{
			SP_ERROR;			
			goto ERR_PRINT;
		}

		CParseSpl spl;
		if (spl.InitParse(sPath, TRUE))
		{
			int nCopy = m_dwCopies;
			int nFrom2 = 0;
			if (nCopy == 1)
			{
				nCopy = (m_nPrinted > 0) ? 0 : 1;
				nFrom2 = m_nPrinted;
			}
			else
			{
				int nPagePerCopy = nTo - nFrom + 1;
				int nTotalPage = nCopy * nPagePerCopy;
				int nLeftPage = nTotalPage - m_nPrinted;
				nCopy = nLeftPage / nPagePerCopy;
				int nLeftPage2 = nLeftPage - (nCopy * nPagePerCopy);
				nFrom2 = nPagePerCopy - nLeftPage2;
			}

			DWORD dwPagePerCopy = spl.GetPageCountPerCopy();
			if(nFrom >= 0 && nTo >= nFrom && nTo < dwPagePerCopy)
			{
				theLog.Write(_T("print emf from = %d to = %d ,copy = %d,nFrom2=%d"),nFrom,nTo,nCopy, nFrom2);
				for (int i=nFrom2; i <= nTo; i++)
				{
					theLog.Write(_T("print emf nFrom2 = %d, i = %d"), nFrom2, i);
					HENHMETAFILE hEmf = GetEmf(spl, i);
					if (hEmf == NULL)
					{
						theLog.Write(_T("!!hEmf == NULL, pageindex=%d"), i);
						goto ERR_PRINT;
					}
					ENHMETAHEADER emfheader;
					GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &emfheader);
					Metafile mf(hEmf, TRUE);
					ResetPrinterHDC(hdcPrinter, pDevmodeWork, &emfheader); //重置HDC

					nErrRet = StartPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}

					Graphics graphics(hdcPrinter);
					graphics.SetSmoothingMode(SmoothingModeHighQuality);
					graphics.SetInterpolationMode(InterpolationModeHighQuality); 
					graphics.SetPageUnit(UnitPixel);
					graphics.SetPageScale(1.0);

					Rect rc;
					GetPrintDeviceArea(hdcPrinter, mf, rc, graphics);	//获取打印区域
					int nPictureWidth = mf.GetWidth();
					int nPictureHeight = mf.GetHeight();
					theLog.Write(_T("print emf AA rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
						,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

					if (IsWater())
					{
						CPrintWithWaterMarked water;
						water.Init(&mf,&m_oSec);
						water.DrawImg(&mf);
						graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}
					else
					{
						graphics.DrawImage(&mf, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
					}

					graphics.ReleaseHDC(hdcPrinter);

					nErrRet = EndPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}
					nPrinted++;
					//回调通知更新界面，通知服务器扣费
					if(pChargeCall)
					{
						pChargeCall->OnNewPagePrinted(nFrom2);
					}
				}			
				for (int x = 0; x < nCopy; x++ )
				{
					theLog.Write(_T("print emf from = %d to = %d ,x = %d"),nFrom,nTo,x);
					for (int m = nFrom; m <= nTo; m ++)
					{
						theLog.Write(_T("print emf nFrom = %d, m = %d"), nFrom, m);
						HENHMETAFILE hEmf = GetEmf(spl, m);
						if (hEmf == NULL)
						{
							theLog.Write(_T("!!hEmf == NULL, pageindex=%d"), m);
							goto ERR_PRINT;
						}
						ENHMETAHEADER emfheader;
						GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &emfheader);
						Metafile mf(hEmf, TRUE);
						ResetPrinterHDC(hdcPrinter, pDevmodeWork, &emfheader); //重置HDC

						nErrRet = StartPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}

						Graphics graphics(hdcPrinter);
						graphics.SetSmoothingMode(SmoothingModeHighQuality);
						graphics.SetInterpolationMode(InterpolationModeHighQuality); 
						graphics.SetPageUnit(UnitPixel);
						graphics.SetPageScale(1.0);

						Rect rc;
						GetPrintDeviceArea(hdcPrinter, mf, rc, graphics);	//获取打印区域
						int nPictureWidth = mf.GetWidth();
						int nPictureHeight = mf.GetHeight();
						theLog.Write(_T("print emf BB rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
							,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

						if (IsWater())
						{
							CPrintWithWaterMarked water;
							water.Init(&mf,&m_oSec);
							water.DrawImg(&mf);
							graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
						}
						else
						{
							graphics.DrawImage(&mf, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel, pImgAtt);
						}

						graphics.ReleaseHDC(hdcPrinter);

						nErrRet = EndPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}
						nPrinted++;
						//回调通知更新界面，通知服务器扣费
						if(pChargeCall)
						{
							pChargeCall->OnNewPagePrinted(m);
						}
					}
				}
			}
			else
			{
				theLog.Write(_T("!!nFrom=%d,nTo=%d,dwPagePerCopy=%d"), nFrom, nTo, dwPagePerCopy);
				ASSERT(0);
			}				
		}
		else
		{
			theLog.Write(_T("!!spl InitParse fail.sPath=[%s]"), sPath);	
		}

		nErrRet = EndDoc(hdcPrinter);
		if (nErrRet <= 0)
		{
			goto ERR_PRINT;
		}

		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nPrinted);
		}
	}
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}
	return TRUE;

ERR_PRINT:
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImgAtt)
	{
		delete pImgAtt;
	}

	theLog.Write(_T("print emf err:%d,prined:%d"),nErrRet,nPrinted);
	if (pChargeCall)
	{
		pChargeCall->OnPrintedErr(nPrinted,nErrRet);
	}
	return FALSE;
}


HENHMETAFILE CIOPrinter::GetEmf(CParseSpl& spl, DWORD dwPageIndex)
{
#ifdef ENABLE_MEMORY_MAP_PARSE_SPL
	HENHMETAFILE hEMF = NULL;
	PEMF_DATA emf = spl.GetEmfDataByPageIndex(dwPageIndex);
	if (emf && emf->pData)
	{
		hEMF = SetEnhMetaFileBits(emf->dwSize, emf->pData);
	}
	else
	{
		theLog.Write(_T("!!CIOPrinter::GetEmf,1,fail,emf=%p, pData=%p"), emf, (emf!=NULL)?emf->pData:NULL);
	}
	return hEMF;
#else
	return spl.GetEmfDataByPageIndex(dwPageIndex);
#endif
}

//获取指定索引的PDF图片数据
Image* CIOPrinter::GetPdf(CPdfHelper& pdf, DWORD dwPageIndex, BOOL bColor)
{
	Image* pImage = NULL;
	IStream* pIStream = NULL;
	HRESULT hr = S_OK;
	Status stat = Ok;
	double hDPI = 300.0;
	double vDPI = 300.0;
	ImageColorMode colorMode = bColor ? imageModeBGR8 : imageModeMono8;
	int nWidth = 0;
	int nHeight = 0;
	int nStride = 0;
	unsigned char *scan0 = NULL;
	if (pdf.GetBitmap(dwPageIndex, hDPI, vDPI, colorMode, nWidth, nHeight, nStride, &scan0))
	{
		int nStep = 1;
		WORD biBitCount = 32;
		if (colorMode == imageModeMono1)
		{
			biBitCount = 1;
			nStep = 255;
		}
		else if (colorMode == imageModeMono8)
		{
			biBitCount = 8;
			nStep = 1;
		}
		else if ((colorMode == imageModeRGB8) || (colorMode == imageModeBGR8))
		{
			biBitCount = 24;
		}
		else if (colorMode == imageModeXBGR8)
		{
			biBitCount = 32;
		}

		BITMAPINFO *pBi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));
		memset(pBi, 0x0, sizeof(BITMAPINFO));
		pBi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBi->bmiHeader.biWidth = nWidth;
		pBi->bmiHeader.biHeight = (nStride>0) ? -nHeight : nHeight;	// If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. 
		//If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. 
		pBi->bmiHeader.biSizeImage = 0;
		pBi->bmiHeader.biXPelsPerMeter = hDPI / 0.0254;
		pBi->bmiHeader.biYPelsPerMeter = vDPI / 0.0254;
		pBi->bmiHeader.biPlanes = 1;
		pBi->bmiHeader.biBitCount = biBitCount;
		pBi->bmiHeader.biCompression = BI_RGB;
		pBi->bmiHeader.biClrUsed = 0;
		pBi->bmiHeader.biClrImportant = 0;
		if (biBitCount <= 8)
		{
			int nColorCount = 1 << biBitCount;
			unsigned char* pPalette = (unsigned char*)pBi + sizeof(BITMAPINFOHEADER);
			for (int i=0; i<nColorCount; i++)
			{
				int nIndex = i * sizeof(RGBQUAD);
				int nColor = i * nStep;
				//nColor = (nColor>255) ? 255 : nColor;
				pPalette[nIndex] = nColor;
				pPalette[nIndex+1] = nColor;
				pPalette[nIndex+2] = nColor;
				pPalette[nIndex+3] = 0;
			}
		}
		Bitmap bitmap(pBi, scan0);
		free(pBi);
		CLSID bmpClsid;
		GetEncoderClsid(L"image/bmp", &bmpClsid);
		hr = CreateStreamOnHGlobal(NULL, TRUE, &pIStream);
		if(FAILED(hr))
		{
			theLog.Write(_T("!!CIOPrinter::GetPdf,1,fail,err=%u"), GetLastError());
			goto Exit;
		}
		stat = bitmap.Save(pIStream, &bmpClsid);
		if (stat != Ok)
		{
			theLog.Write(_T("!!CIOPrinter::GetPdf,2,fail,stat=%d"), stat);
			goto Exit;
		}
		pImage = new Image(pIStream);
	}
	else
	{
		theLog.Write(_T("!!CIOPrinter::GetPdf,3,fail"));
		goto Exit;
	}

Exit:
	if (scan0)
		free(scan0);
	if(pIStream)
		pIStream->Release();

	return pImage;
}

void CIOPrinter::GetPrintDeviceArea(HDC& hdcPrinter, Image& image, Rect& printRect, Graphics& graphics, BOOL bUsePhysicalArea /*= FALSE*/)
{
	int nImageWidth = image.GetWidth();
	int nImageHeight = image.GetHeight();
	int nDeviceHORZRES = GetDeviceCaps(hdcPrinter, HORZRES);			//可打印页面宽度(单位:pixels)
	int nDeviceVERTRES = GetDeviceCaps(hdcPrinter, VERTRES);			//可打印页面高度(单位:pixels)
	int nDeviceWidth = GetDeviceCaps(hdcPrinter, PHYSICALWIDTH);		//物理宽度(设备单位)
	int nDeviceHeight = GetDeviceCaps(hdcPrinter, PHYSICALHEIGHT);		//物理高度(设备单位)
	int nDeviceOffSetX = GetDeviceCaps(hdcPrinter, PHYSICALOFFSETX);	//左上角x偏移(设备单位)
	int nDeviceOffSetY = GetDeviceCaps(hdcPrinter, PHYSICALOFFSETY);	//左上角y偏移(设备单位)
	int nScalingFactorX = GetDeviceCaps(hdcPrinter, SCALINGFACTORX);	//Scaling factor x
	int nScalingFactorY = GetDeviceCaps(hdcPrinter, SCALINGFACTORY);	//Scaling factor y
	theLog.Write(_T("CIOPrinter::GetPrintDeviceArea,nDeviceHORZRES=%d,nDeviceVERTRES=%d,nDeviceWidth=%d,nDeviceHeight=%d,")
					_T("nDeviceOffSetX=%d,nDeviceOffSetY=%d,nScalingFactorX=%d, nScalingFactorY=%d, nImageWidth=%d, nImageHeight=%d")
					, nDeviceHORZRES, nDeviceVERTRES, nDeviceWidth, nDeviceHeight
					, nDeviceOffSetX, nDeviceOffSetY, nScalingFactorX, nScalingFactorY
					, nImageWidth, nImageHeight);

	if (bUsePhysicalArea)
	{
		printRect.X = -nDeviceOffSetX;
		printRect.Y = -nDeviceOffSetY;
		printRect.Width = nDeviceWidth;
		printRect.Height = nDeviceHeight;
	}
	else
	{
		printRect.X = 0;
		printRect.Y = 0;
		printRect.Width = nDeviceHORZRES;
		printRect.Height = nDeviceVERTRES;
	}

	Matrix matrix;
	matrix.Translate(printRect.X, printRect.Y);

	//获取多合一打印参数
	AllPageInOneParam param;
	memset(&param, 0x0, sizeof(AllPageInOneParam));
	GetAllPageInOneParam(printRect, image, param);

	matrix.Translate(param.x_offset, param.y_offset);
	matrix.Rotate(param.rotate_radians);

	double fScaleWidth = 1.0 * printRect.Width / nImageWidth;
	double fScaleHeight = 1.0 * printRect.Height / nImageHeight;
	double fScale = (fScaleWidth < fScaleHeight) ? fScaleWidth : fScaleHeight;

	matrix.Translate((printRect.Width - nImageWidth*fScale) / 2, (printRect.Height - nImageHeight*fScale) / 2);
	matrix.Scale(fScale, fScale);
	graphics.SetTransform(&matrix);

	printRect.X = 0;	//将画图的原点定为（0,0），即世界坐标原点。
	printRect.Y = 0;
	printRect.Width = nImageWidth;	//打印范围为图片的宽高
	printRect.Height = nImageHeight;

	theLog.Write(_T("CIOPrinter::GetPrintDeviceArea,2,OffsetX=%0.2f, OffsetY=%0.2f,")
		_T("param.x_offset=%0.2f, param.y_offset=%0.2f, param.rotate_radians=%0.2f")
		, matrix.OffsetX(), matrix.OffsetY()
		, param.x_offset, param.y_offset, param.rotate_radians);
}

//重置HDC
void CIOPrinter::ResetPrinterHDC(HDC& hdcPrinter, PDEVMODE pDevMode, PENHMETAHEADER pEmfHeader)
{
	if (!pDevMode && !pEmfHeader)
	{
		theLog.Write(_T("!!CIOPrinter::ResetPrinterHDC,1,pDevMode=%p,pEmfHeader=%p"), pDevMode, pEmfHeader);
		return;
	}

	WORD wdPaperSize = DMPAPER_A4;
	WORD wdOrientation = DMORIENT_PORTRAIT;
	if (m_bAutoPaperSize)
	{
		stdtstring sPrinter = m_sPrinter.GetString();
		CPrinterPaperSize::GetInstance().GetPaperInfoByEmfHeader(sPrinter, pEmfHeader, wdPaperSize, wdOrientation);
		if ((DMPAPER_A4 != wdPaperSize) && (DMPAPER_A3 != wdPaperSize) && m_bUseA4Print)
		{
			theLog.Write(_T("##CIOPrinter::ResetPrinterHDC,wdPaperSize [%d] is not support,changeto DMPAPER_A4."), wdPaperSize);
			wdPaperSize = DMPAPER_A4;
		}
	}
	else
	{
		wdPaperSize = pDevMode->dmPaperSize;	//不更改打印纸型
		wdOrientation = (pEmfHeader->szlMillimeters.cx > pEmfHeader->szlMillimeters.cy) ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;	//打印方向
	}

	if (pDevMode->dmOrientation != wdOrientation || pDevMode->dmPaperSize != wdPaperSize)
	{
		pDevMode->dmOrientation = wdOrientation;
		if (pDevMode->dmPaperSize == 0 || pDevMode->dmPaperSize >= DMPAPER_USER)	
		{
			//dmPaperSize为0或者大于等于DMPAPER_USER(256)，表示是自定义纸型大小，此处不要更改dmPaperSize,保持原始值
		}
		else
		{
			pDevMode->dmPaperSize = wdPaperSize;
		}
		hdcPrinter = ResetDC(hdcPrinter, pDevMode);
	}
}

//重置HDC
void CIOPrinter::ResetPrinterHDC(HDC& hdcPrinter, PDEVMODE pDevMode, Image& image)
{
	if (!pDevMode)
	{
		theLog.Write(_T("!!CIOPrinter::ResetPrinterHDC,1,pDevMode=%p"), pDevMode);
		return;
	}

	WORD wdPaperSize = DMPAPER_A4;
	WORD wdOrientation = DMORIENT_PORTRAIT;
	if (m_bAutoPaperSize)
	{
		stdtstring sPrinter = m_sPrinter.GetString();
		CPrinterPaperSize::GetInstance().GetPaperInfoByImage(sPrinter, image, wdPaperSize, wdOrientation);
		if ((DMPAPER_A4 != wdPaperSize) && (DMPAPER_A3 != wdPaperSize) && m_bUseA4Print)
		{
			theLog.Write(_T("##CIOPrinter::ResetPrinterHDC,wdPaperSize [%d] is not support,changeto DMPAPER_A4."), wdPaperSize);
			wdPaperSize = DMPAPER_A4;
		}
	}
	else
	{
		wdPaperSize = pDevMode->dmPaperSize;	//不更改打印纸型
		wdOrientation = (image.GetWidth() > image.GetHeight()) ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;	//打印方向
	}

	if (pDevMode->dmOrientation != wdOrientation || pDevMode->dmPaperSize != wdPaperSize)
	{
		pDevMode->dmOrientation = wdOrientation;
		if (pDevMode->dmPaperSize == 0 || pDevMode->dmPaperSize >= DMPAPER_USER)	
		{
			//dmPaperSize为0或者大于等于DMPAPER_USER(256)，表示是自定义纸型大小，此处不要更改dmPaperSize,保持原始值
		}
		else
		{
			pDevMode->dmPaperSize = wdPaperSize;
		}
		hdcPrinter = ResetDC(hdcPrinter, pDevMode);
	}
}

//判断打印机是否存在
BOOL CIOPrinter::IsPrinterExist(CString szPrinterName)
{
	HANDLE hPrinter = NULL;
	if(OpenPrinter(szPrinterName.GetBuffer(),&hPrinter,NULL))
	{
		ClosePrinter(hPrinter);
		return TRUE;
	}
	else
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_INVALID_PRINTER_NAME)
		{
			theLog.Write(_T("!!CIOPrinter::IsPrinterExist,OpenPrinter fail,dwError=%u,szPrinterName=%s")
				, dwError, szPrinterName);
		}
		return FALSE;
	}
}

void CIOPrinter::VerifyDocName(CString& szDocName)		//验证文档名合法性
{
	szDocName.Trim();
	int nDocNameLen = szDocName.GetLength();
	if (nDocNameLen >= MAX_PATH)
	{
		int nLen = (MAX_PATH / 2) - 1;
		szDocName.Format(_T("%s~%s"), szDocName.Left(nLen), szDocName.Right(nLen));
	}
}

void WINAPI CIOPrinter::PrintProcessCallBack(PrintProcessCallBackData* pCallBackData)
{
	if (pCallBackData)
	{
		int nPrintedPage = pCallBackData->nPrintedPage;	//此处是以1为开始索引
		BOOL bPrintComplete = pCallBackData->bPrintComplete;
		BOOL bError = pCallBackData->bError;
		IPrtStationPrintCallback* pCall = (IPrtStationPrintCallback*)pCallBackData->pData;
		if (pCall)
		{
			if (bError && bPrintComplete)
			{
				pCall->OnPrintedErr(nPrintedPage,0);
			}
			else if (!bError && bPrintComplete)
			{
				pCall->OnPrintedCmplt(nPrintedPage);
			}
			else
			{
				pCall->OnNewPagePrinted(nPrintedPage-1);//此处是以0为开始索引
			}
		}
	}
	else
	{
		theLog.Write(_T("##PrintProcessCallBack,pCallBackData=%p"), pCallBackData);
	}
}
BOOL SendData(SOCKET hSocket, const char* pData, int nDataSize)
{
	int nSend = 0;
	int nRet = 0;
	while (nSend < nDataSize)
	{
		nRet = send(hSocket, pData+nSend, nDataSize-nSend, 0);
		if (nRet == SOCKET_ERROR)
		{
			theLog.Write(_T("!!SendData, send fail errno=%d"), WSAGetLastError());
			break;
		}
		nSend += nRet;
	}
	return (nSend == nDataSize) ? TRUE : FALSE;
}

//add in 2017.10.24
BOOL CIOPrinter::PrintSplFileToNetPrinter(CString szPrinterIP, int nPrinterPort, CString szFilePath
										  , IPrtStationPrintCallback* pCall, int nPageCount, BRAND_TYPE nType/* = BT_THIN_CLT*/)
{
	theLog.Write(_T("CIOPrinter::PrintSPLFileToNetPrinter,szPrinterIP=%s,nPrinterPort=%d,szFilePath=%s,nPageCount=%d,nType=%d")
		, szPrinterIP, nPrinterPort, szFilePath, nPageCount, nType);
	BOOL bRet = FALSE;
	int nPageIndex = 0;
	int nPrintedPage = 0;
	int nSendGuid = 1;
	int nNotSleepPackNum = 0;
	SOCKET hSocket = INVALID_SOCKET;
	BYTE *pData = NULL;
	FILE *hFile = NULL;
	int iResult = NO_ERROR;

	ChangeJob(szFilePath, m_dwColor, m_dwDuplex, nType);
	do 
	{
		//初始化网络环境
		WSADATA wsaData;
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(iResult != NO_ERROR)
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdfFileToNetPrinter,1, WSAStartup fail. errno=%d"), WSAGetLastError());
			break;
		}
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (hSocket == INVALID_SOCKET) 
		{
			theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,2, create socket fail. errno=%d"), WSAGetLastError());
			break;
		}

		SOCKADDR_IN sockaddr; 
		sockaddr.sin_family = AF_INET;
		CStringA szPrinterIPA;
#ifdef UNICODE
		szPrinterIPA = CCommonFun::UnicodeToMultiByte(szPrinterIP);
#else
		szPrinterIPA = szPrinterIP;
#endif
		
		sockaddr.sin_addr.S_un.S_addr = inet_addr(szPrinterIPA.GetString());
		sockaddr.sin_port = htons(nPrinterPort);

		if (connect(hSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) 
		{
			theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,3, connect fail. errno=%d"), WSAGetLastError());
			break;
		}
		//发送二进制作业文件
		hFile = ::_tfopen(szFilePath.GetString(),_T("rb"));
		if (!hFile)
		{
			theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,5.2,Open fail,err=%u,szFilePath=%s"), GetLastError(), szFilePath);
			break;
		}

		DWORD dwFileLen = CCommonFun::GetFileSizeByte(szFilePath);
		int nRead = 0;
		int nSend = 0;
		int nPacketCount = 1;
		int nAllNetPacketMinSleepMS = m_nAllNetPacketMinSleepMS;
		if (dwFileLen > m_nNetPacketSize)
		{
			nPacketCount = dwFileLen/m_nNetPacketSize + 1;
			nAllNetPacketMinSleepMS = m_nNetPacketSleepMS * nPacketCount;
			if (nAllNetPacketMinSleepMS < m_nAllNetPacketMinSleepMS)
			{
				nAllNetPacketMinSleepMS = m_nAllNetPacketMinSleepMS;
			}
		}
		if (nPageCount <= 0)
		{
			nPageCount = 1;
		}
		int nPageAverageSize = dwFileLen / nPageCount;	//平均每页大小
		if (nPageAverageSize <= 0)
		{
			nPageAverageSize = 1;
		}
		theLog.Write(_T("CIOPrinter::PrintSPLFileToNetPrinter,5.3,dwFileLen=%d,nPageAverageSize=%d,nPageCount=%d,nPacketCount=%d,nAllNetPacketMinSleepMS=%d")
			, dwFileLen, nPageAverageSize, nPageCount, nPacketCount, nAllNetPacketMinSleepMS);
		pData = new BYTE[m_nNetPacketSize]; 
		for (int i = 0;i <nPacketCount; i++)
		{
			memset(pData, 0x0, m_nNetPacketSize);
			nRead = ::fread(pData, 1, m_nNetPacketSize, hFile); //Read(pData, nBlock);
			if (nRead <= 0)
			{
				break;
			}
			if (i != (nPacketCount - 1))	//不是最后一个包
			{
				if (!SendData(hSocket, (const char*)pData, nRead))
				{
					theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,6,SendData fail"));
					break;
				}
				//Sleep(m_nNetPacketSleepMS);
			}
			else
			{
				CStringA szLastData = (char*)pData;
				int nIndex = szLastData.Find("%-12345X");
				int nLeftSize = dwFileLen - nSend;
				if (nIndex >= 0)
				{
					theLog.Write(_T("CIOPrinter::PrintSPLFileToNetPrinter,5"));
					if (!SendData(hSocket, (const char*)pData, nIndex))
					{
						theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,6,SendData fail"));
						break;
					}

					//等待一会，让打印机有处理时间
					Sleep(nAllNetPacketMinSleepMS);

					if (!SendData(hSocket, (const char*)pData + nIndex, nLeftSize - nIndex))
					{
						theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,6,SendData fail"));
						break;
					}

					//最后再等待一个网络包的时间
					Sleep(m_nNetPacketSleepMS);
				}
				else
				{
					if (!SendData(hSocket, (const char*)pData, nRead))
					{
						theLog.Write(_T("!!CIOPrinter::PrintSPLFileToNetPrinter,6,SendData fail"));
						break;
					}

					//等待一会，让打印机有处理时间
					Sleep(nAllNetPacketMinSleepMS);
				}

			}
			nSend += nRead;
			nPrintedPage = nSend / nPageAverageSize;	//以发送大小来模拟打印页数
			if (nPrintedPage > nPageIndex)
			{
				if (pCall)
				{
					pCall->OnNewPagePrinted(nPageIndex);//此处是以0为开始索引
				}
				nPageIndex++;
			}
		}

		theLog.Write(_T("CIOPrinter::PrintSPLFileToNetPrinter,8,Succ"));
		bRet = TRUE;
	} while (FALSE);

	//关闭文件
	if (hFile)
	{
		fclose(hFile);
		hFile = NULL;
	}

	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}

	if (hSocket != INVALID_SOCKET)
	{
		closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}

	//清理网络环境
	if (iResult == NO_ERROR)
	{
		iResult = WSACleanup();
		if(iResult != NO_ERROR)
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdfFileToNetPrinter,9,WSACleanup fail. errno=%d"), WSAGetLastError());
		}
	}

	if (pCall)
	{
		if (bRet)
		{
			pCall->OnPrintedCmplt(nPageCount * m_dwCopies);
		}
		else
		{
			pCall->OnPrintedErr(0,0);
		}
	}
	theLog.Write(_T("CIOPrinter::PrintSPLFileToNetPrinter,10,end. bRet=%d"), bRet);
	return bRet;
}

//add in 2017.09.08
//参考文档：【C:\WorkSpace\iSecPrint\Doc\PJL_trm.pdf】
BOOL CIOPrinter::PrintFileToNetPrinter(CString szPrinterIP, int nPrinterPort, CString szFilePath, CStringA szBeginPJL, CStringA szEndPJL
										  , IPrtStationPrintCallback* pCall, int nPageCount)
{
	theLog.Write(_T("CIOPrinter::PrintFileToNetPrinter,szPrinterIP=%s,nPrinterPort=%d,szFilePath=%s,pCall=%p,nPageCount=%d")
		, szPrinterIP, nPrinterPort, szFilePath, pCall, nPageCount);
	BOOL bRet = FALSE;
	int nPageIndex = 0;
	int nPrintedPage = 0;
	SOCKET hSocket = INVALID_SOCKET;
	BYTE *pData = NULL;
	int iResult = NO_ERROR;
	do 
	{
		//初始化网络环境
		WSADATA wsaData;
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(iResult != NO_ERROR)
		{
			theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,1, WSAStartup fail. errno=%d"), WSAGetLastError());
			break;
		}

		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (hSocket == INVALID_SOCKET) 
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdfFileToNetPrinter,2, create socket fail. errno=%d"), WSAGetLastError());
			break;
		}
		CStringA szPrinterIPA = NULL;
		SOCKADDR_IN sockaddr; 
		sockaddr.sin_family = AF_INET;
#ifdef UNICODE
		szPrinterIPA = CCommonFun::UnicodeToMultiByte(szPrinterIP);
#else
		szPrinterIPA = szPrinterIP;
#endif
		sockaddr.sin_addr.S_un.S_addr = inet_addr(szPrinterIPA.GetString());
		sockaddr.sin_port = htons(nPrinterPort);

		if (connect(hSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) 
		{
			theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,3, connect fail. errno=%d"), WSAGetLastError());
			break;
		}

		theLog.Write(_T("CIOPrinter::PrintFileToNetPrinter,4,connect Succ"));

		//发送PJL开始命令
		if (szBeginPJL.GetLength() > 0)
		{
			if (!SendData(hSocket, szBeginPJL.GetString(), szBeginPJL.GetLength()))
			{
				theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,5,SendData fail"));
				break;
			}
		}

		//发送二进制作业文件，一般支持PCL,PS,PDF,jpg
		CFile file;
		if (!file.Open(szFilePath, CFile::modeRead))
		{
			theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,5.2,Open fail,err=%u,szFilePath=%s"), GetLastError(), szFilePath);
			break;
		}
		int nFileLen = file.GetLength();
		int nRead = 0;
		int nSend = 0;
		int nPacketCount = 1;
		int nAllNetPacketMinSleepMS = m_nAllNetPacketMinSleepMS;
		if (nFileLen > m_nNetPacketSize)
		{
			nPacketCount = nFileLen/m_nNetPacketSize + 1;
			nAllNetPacketMinSleepMS = m_nNetPacketSleepMS * nPacketCount;
			if (nAllNetPacketMinSleepMS < m_nAllNetPacketMinSleepMS)
			{
				nAllNetPacketMinSleepMS = m_nAllNetPacketMinSleepMS;
			}
		}
		if (nPageCount <= 0)
		{
			nPageCount = 1;
		}
		int nPageAverageSize = nFileLen / nPageCount;	//平均每页大小
		if (nPageAverageSize <= 0)
		{
			nPageAverageSize = 1;
		}
		theLog.Write(_T("CIOPrinter::PrintFileToNetPrinter,5.3,nFileLen=%d,nPageAverageSize=%d,nPageCount=%d,nPacketCount=%d,nAllNetPacketMinSleepMS=%d")
			, nFileLen, nPageAverageSize, nPageCount, nPacketCount, nAllNetPacketMinSleepMS);
		pData = new BYTE[m_nNetPacketSize];
		while(TRUE)
		{
			nRead = file.Read(pData, m_nNetPacketSize);
			if (nRead <= 0)
			{
				break;
			}
			if (!SendData(hSocket, (const char*)pData, nRead))
			{
				theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,6,SendData fail"));
				break;
			}
			nSend += nRead;
			nPrintedPage = nSend / nPageAverageSize;	//以发送大小来模拟打印页数
			if (nPrintedPage > nPageIndex)
			{
				if (pCall)
				{
					pCall->OnNewPagePrinted(nPageIndex);//此处是以0为开始索引
				}
				nPageIndex++;
			}
		}

		file.Close();	//关闭文件

		//等待一会，让打印机有处理时间
		Sleep(nAllNetPacketMinSleepMS);

		//发送PJL结束命令
		if (szEndPJL.GetLength() > 0)
		{
			if (!SendData(hSocket, szEndPJL.GetString(), szEndPJL.GetLength()))
			{
				theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,7,SendData fail"));
				break;
			}
		}

		//最后再等待一个网络包的时间
		Sleep(m_nNetPacketSleepMS);

		theLog.Write(_T("CIOPrinter::PrintFileToNetPrinter,8,Succ"));

		bRet = TRUE;

	} while (FALSE);

	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}

	if (hSocket != INVALID_SOCKET)
	{
		closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}

	//清理网络环境
	if(iResult == NO_ERROR)
	{
		iResult = WSACleanup();
		if(iResult != NO_ERROR)
		{
			theLog.Write(_T("!!CIOPrinter::PrintFileToNetPrinter,9,WSACleanup fail. errno=%d"), WSAGetLastError());
		}	
	}	

	if (pCall)
	{
		if (bRet)
		{
			pCall->OnPrintedCmplt(nPageCount * m_dwCopies);
		}
		else
		{
			pCall->OnPrintedErr(0,0);
		}
	}

	theLog.Write(_T("CIOPrinter::PrintFileToNetPrinter,10,end. bRet=%d"), bRet);

	return bRet;
}

BOOL CIOPrinter::PrintPdfFileToNetPrinter(CString szPrinterIP, int nPrinterPort, CString szPdfPath, IPrtStationPrintCallback* pCall, int nPageCount)
{
	theLog.Write(_T("CIOPrinter::PrintPdfFileToNetPrinter,szPrinterIP=%s,nPrinterPort=%d,szPdfPath=%s,pCall=%p,nPageCount=%d")
		, szPrinterIP, nPrinterPort, szPdfPath, pCall, nPageCount);
	CStringA szBeginPJL;
	szBeginPJL.Format("\x1b%%-12345X");
	szBeginPJL.AppendFormat("@PJL SET QTY = %d \n", m_dwCopies);	//份数,逐份打印
//	szBeginPJL.AppendFormat("@PJL SET COPIES = %d \n", m_dwCopies);	//份数,逐张多份打印
	if (m_dwDuplex == DMDUP_SIMPLEX)
	{
		szBeginPJL.AppendFormat("@PJL SET DUPLEX = OFF \n");	//单面打印
	}
	else
	{
		szBeginPJL.AppendFormat("@PJL SET DUPLEX = ON \n");	//双面打印
		if (m_dwDuplex == DMDUP_VERTICAL)
		{
			szBeginPJL.AppendFormat("@PJL SET BINDING = LONGEDGE \n");	//长边翻转
		}
		else
		{
			szBeginPJL.AppendFormat("@PJL SET BINDING = SHORTEDGE \n");	//短边翻转
		}
	}
	if (m_dwColor == DMCOLOR_MONOCHROME)
	{
		//黑白打印
		szBeginPJL.AppendFormat("@PJL SET PLANESINUSE = 1 \n");
		szBeginPJL.AppendFormat("@PJL SET RENDERMODE = GRAYSCALE \n");
	}
	else
	{
		//彩色打印
		szBeginPJL.AppendFormat("@PJL SET PLANESINUSE = 3 \n");
		szBeginPJL.AppendFormat("@PJL SET RENDERMODE = COLOR \n");
	}
// 	szBeginPJL.AppendFormat("@PJL SET FINISH = NONE \n");
// 	szBeginPJL.AppendFormat("@PJL SET OUTBIN = AUTO \n");
// 	szBeginPJL.AppendFormat("@PJL SET ORIENTATION = PORTRAIT \n");
// 	szBeginPJL.AppendFormat("@PJL SET PAPER = A4 \n");
// 	//116929*82677
// 	szBeginPJL.AppendFormat("@PJL SET PAPERLENGTH = 11906 \n");	//纸张长度，720分之一英寸
// 	szBeginPJL.AppendFormat("@PJL SET PAPERWIDTH = 8419 \n");	//纸张宽度，720分之一英寸
// 	szBeginPJL.AppendFormat("@PJL SET USERNAME = \"PrintUser\" \n");	//用户名
//  szBeginPJL.AppendFormat("@PJL JOB NAME = \"%s\" START = %d END = %d\n", sDocName, nFrom+1, nTo+1);	//文档名,起始页和结束页,页码从1开始。
	szBeginPJL.AppendFormat("@PJL SET JOBNAME = \"%s\" \n", m_szDocName);	//文档名
//	szBeginPJL.AppendFormat("@PJL ENTER LANGUAGE = AUTO \n");	//作业格式，设置为自动，在一些打印中，如果加了这个指令，作业不打印，暂时不加。

	CStringA szEndPJL;
	szEndPJL.AppendFormat("\n");
	//szEnd.AppendFormat("@PJL EOJ NAME = \"%s\" ", sDocName);	//EOJ结束作业，要与JOB要匹配使用，不能单个出现
	szEndPJL.AppendFormat("\x1b%%-12345X\n");

	return PrintFileToNetPrinter(szPrinterIP, nPrinterPort, szPdfPath, szBeginPJL, szEndPJL, pCall, nPageCount);
}

//add in 2016.04.18
BOOL CIOPrinter::PrintPdfFile(CString szPdfPath,IPrtStationPrintCallback* pChargeCall,int nFrom /*= -1*/,int nTo /*= -1*/)
{
	if (m_nPrinted > 0)
	{
		return PrintPdfFileBreakPoint(szPdfPath, pChargeCall, nFrom, nTo);
	}
	theLog.Write(_T("CIOPrinter::PrintPdfFile,1,szPdfPath=%s,nFrom=%d,nTo=%d"),szPdfPath,nFrom,nTo);
	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintPdfFile,2,Printer=[%s]"),m_sPrinter);

	BOOL bForceGray = FALSE;
#if 0	//
	if ((m_eType == BT_SL) || (m_eType == BT_SL2))
	{
		if (m_dwColor == DMCOLOR_MONOCHROME)
		{
			bForceGray = TRUE;	//目前，只有施乐打印机无法控制黑白和彩色，此处强制使用黑白打印
		}
	}
#else
	if (m_dwColor == DMCOLOR_MONOCHROME)
	{
		bForceGray = TRUE;
	}
#endif

	BOOL bRet = FALSE;
	CPdfHelper helper;
#ifdef UNICODE
	if (helper.OpenFile(CCommonFun::UnicodeToUTF8(szPdfPath).GetString()))
#else
	if (pdf.OpenFile(CCommonFun::MultiByteToUTF8(szPdfPath).GetString()))
#endif
	{
		PrintProcessCallBackData* pCallBackData = new PrintProcessCallBackData;
		memset(pCallBackData, 0x0, sizeof(PrintProcessCallBackData));
		pCallBackData->pData = pChargeCall;
		BOOL bUseCropBox = FALSE;
		//bUseFullPage为TRUE,则表示将pdf整个页面大小与输出纸的物理区域进行匹配，
		//一般能保持内容大小不变，但是可能一些在pdf极边缘处的内容无法打印出来；
		//如果为FALSE,则表示将pdf整个页面大小与输出纸的打印区域进行匹配，
		//所以pdf页面内容能全部打印出来。一般打印区域比物理区域小一圈，
		//所以打印出来的内容会比pdf实际内容整体缩小一些。
		BOOL bUseFullPage = FALSE;
		BOOL bUsePDFPageSize = m_bAutoPaperSize;
		WaterInfo* pWater = NULL;
		CStringA szHeaderA, szFooterA, szWaterA;
		if (IsWater())
		{
			pWater = new WaterInfo;
#ifdef UNICODE
			szHeaderA = CCommonFun::UnicodeToUTF8(m_oSec.header);
			szFooterA = CCommonFun::UnicodeToUTF8(m_oSec.footer);
			szWaterA = CCommonFun::UnicodeToUTF8(m_oSec.water);
#else
			szHeaderA = CCommonFun::MultiByteToUTF8(m_oSec.header);
			szFooterA = CCommonFun::MultiByteToUTF8(m_oSec.footer);
			szWaterA = CCommonFun::MultiByteToUTF8(m_oSec.water);
#endif
			pWater->pszHeader = szHeaderA.GetString();
			pWater->pszFooter = szFooterA.GetString();
			pWater->pszWater = szWaterA.GetString();
		}

#ifdef UNICODE
		CStringA szPrinterA = CCommonFun::UnicodeToUTF8(m_sPrinter);
		CStringA szDocNameA = CCommonFun::UnicodeToUTF8(m_szDocName);
#else
		CStringA szPrinterA = CCommonFun::MultiByteToUTF8(m_sPrinter);
		CStringA szDocNameA = CCommonFun::MultiByteToUTF8(m_szDocName);
#endif

		//设置作业属性
		if (m_szJobUser.GetLength() > 0)
		{
#ifdef UNICODE
			helper.SetJobUser(CCommonFun::UnicodeToUTF8(m_szJobUser).GetString());
#else
			helper.SetJobUser(CCommonFun::MultiByteToUTF8(m_szJobUser).GetString());
#endif // UNICODE
		}

		if ((helper.GetPageCount() > 0) && (m_dwDuplex > DMDUP_SIMPLEX))
		{
			//通过获取pdf的第1页内容来判断文档的方向，因为如果文档方向参数不正确，
			//在往一些打印机驱动（已知的柯美打印驱动）提交时，可能双面长边和双面短边参数会起相反作用。
			int nPageRotate = helper.GetPageRotate(0);
			PageBox mediaBox;
			if (helper.GetPageBox(0, mediaBox))
			{
				int nWidth = mediaBox.X2 - mediaBox.X1;
				int nHeight = mediaBox.Y2 - mediaBox.Y1;
				if ((nPageRotate == 90) || (nPageRotate == 270))
				{
					int nTemp = nWidth;
					nWidth = nHeight;
					nHeight = nWidth;
				}
				if (nWidth < nHeight)
				{
					m_dwOrientation = DMORIENT_PORTRAIT;
				}
				else
				{
					m_dwOrientation = DMORIENT_LANDSCAPE;
				}
			}
		}

		bRet = helper.PrintToPrinter(szPrinterA.GetString(), szDocNameA.GetString(), nFrom, nTo
			, m_dwPagesPerPaper, m_dwColor, m_dwPaperSize, m_dwOrientation, m_dwCopies, m_dwCollate, m_dwDuplex
			, m_dwDefaultSource, m_dwPaperWidth, m_dwPaperLength, bUseCropBox, bUseFullPage
			, bUsePDFPageSize, bForceGray, pWater, PrintProcessCallBack, pCallBackData);
		delete pCallBackData;
		if (pWater)
		{
			delete pWater;
		}
		if (!bRet)
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdfFile,3,PrintToPrinter fail"));
		}
	}
	else
	{
		theLog.Write(_T("!!CIOPrinter::PrintPdfFile,4,OpenFile fail"));
	}
	helper.CloseFile();

	theLog.Write(_T("CIOPrinter::PrintPdfFile,5,end"));

	return bRet;
}

BOOL CIOPrinter::PrintPdfFileBreakPoint(CString szPdfPath,IPrtStationPrintCallback* pChargeCall,int nFrom /*= -1*/,int nTo /*= -1*/)
{
	theLog.Write(_T("CIOPrinter::PrintPdfFileBreakPoint,1,szPdfPath=%s,nFrom=%d,nTo=%d"),szPdfPath,nFrom,nTo);
	int nErrRet = SP_NOTREPORTED;
	int nPrinted = 0;
	HDC hdcPrinter = 0;

	if (m_sPrinter.IsEmpty())
	{
		m_sPrinter = GetDefault();
	}
	theLog.Write(_T("CIOPrinter::PrintPdfFileBreakPoint,2,Printer=[%s]"),m_sPrinter);

	Image* pImage = NULL;
	DEVNAMES* pDevName = 0;
	BYTE* pDev = 0;
	if(!PrinterUtils::GetPrinterDevMode(m_sPrinter,pDevName,pDev))
	{
		theLog.Write(_T("CIOPrinter::PrintPdfFileBreakPoint,GetPrinterDevMode err"));
		goto ERR_PRINT;
	}
	if (!pDev)
	{
		theLog.Write(_T("CIOPrinter::PrintPdfFileBreakPoint,GetPrinterDevMode pDev = null err"));
		goto ERR_PRINT;
	}
	PDEVMODE pDevmodeWork = (PDEVMODE)pDev;

	//改变方向
	if(pDevmodeWork->dmFields & DM_ORIENTATION)  
	{
		if (pDevmodeWork->dmOrientation == m_dwOrientation)
		{
			theLog.Write(_T("the printer no change orientation %d"),m_dwOrientation);
		}
		else
		{
			theLog.Write(_T("change the orientation"));
			pDevmodeWork->dmOrientation = m_dwOrientation;
		}
	}

	//改变颜色
	DMCOLOR_COLOR;
	if (pDevmodeWork->dmFields & DM_COLOR)
	{
		if (pDevmodeWork->dmColor == m_dwColor)
		{
			theLog.Write(_T("the printer no change color %d"),m_dwColor);
		}
		else
		{
			theLog.Write(_T("change the color %d"),m_dwColor);
			pDevmodeWork->dmColor = m_dwColor;
		}
	}
	else
	{
		theLog.Write(_T("no support color Field"));
	}

	//改变纸张大小
	DM_PAPERSIZE;
	if (pDevmodeWork->dmFields & DM_PAPERSIZE)
	{
		if (pDevmodeWork->dmPaperSize  == m_dwPaperSize)
		{
			theLog.Write(_T("the printer no change papersize %d"),m_dwPaperSize);
		}
		else
		{
			theLog.Write(_T("change the papersize %d"),m_dwPaperSize);
			pDevmodeWork->dmPaperSize  = m_dwPaperSize;
		}
	}

	//指定自定义纸张大小打印文档
	if ((m_dwPaperSize>=DMPAPER_USER) && (m_dwPaperWidth>=10) && (m_dwPaperLength>=10))
	{
		theLog.Write(_T("自定义纸张打印 m_dwPaperSize=%d,m_dwPaperWidth=%d,m_dwPaperLength=%d"),m_dwPaperSize,m_dwPaperWidth, m_dwPaperLength);
		if (m_dwPaperWidth > m_dwPaperLength)
		{
			pDevmodeWork->dmOrientation = DMORIENT_LANDSCAPE;
			pDevmodeWork->dmPaperWidth = m_dwPaperLength;
			pDevmodeWork->dmPaperLength = m_dwPaperWidth;
		}
		else
		{
			pDevmodeWork->dmOrientation = DMORIENT_PORTRAIT;
			pDevmodeWork->dmPaperWidth = m_dwPaperWidth;
			pDevmodeWork->dmPaperLength = m_dwPaperLength;
		}
		pDevmodeWork->dmPaperSize = DMPAPER_USER;
		pDevmodeWork->dmFields |= DM_ORIENTATION | DM_PAPERSIZE | DM_PAPERWIDTH | DM_PAPERLENGTH;
	}

	//双面打印
	theLog.Write(_T("dup = %d"),pDevmodeWork->dmDuplex);
	if (DM_DUPLEX & pDevmodeWork->dmFields)
	{
		DMDUP_SIMPLEX;
		if (pDevmodeWork->dmDuplex == m_dwDuplex)
		{
			theLog.Write(_T("the printer no change duplex %d"),m_dwDuplex);
		}
		else
		{
			theLog.Write(_T("change the duplex %d"),m_dwDuplex);
			pDevmodeWork->dmDuplex = m_dwDuplex;
		}
	}

#if 0
	//改变打印份数
	DM_COPIES;
	if (pDevmodeWork->dmFields & DM_COPIES)
	{
		if (pDevmodeWork->dmCopies == m_dwCopies)
		{
			theLog.Write(_T("the printer no change copies %d"),m_dwCopies);
		}
		else
		{
			theLog.Write(_T("change the copies %d"),m_dwCopies);
			pDevmodeWork->dmCopies = m_dwCopies;
		}
	}
#endif

	//逐份打印
	DM_COLLATE;
	if (pDevmodeWork->dmFields & DM_COLLATE)
	{
		if (pDevmodeWork->dmCollate == m_dwCollate)
		{
			theLog.Write(_T("the printer no change collate %d"), m_dwCollate);
		}
		else
		{
			theLog.Write(_T("change the collate %d"), m_dwCollate);
			pDevmodeWork->dmCollate = m_dwCollate;
		}
	}

	//选择打印纸张来源
	DM_DEFAULTSOURCE;
	if (IsEnableDefaultSource() && (pDevmodeWork->dmFields & DM_DEFAULTSOURCE))
	{
		if (pDevmodeWork->dmDefaultSource == m_dwDefaultSource)
		{
			theLog.Write(_T("the printer no change paper source %d"),m_dwDefaultSource);
		}
		else
		{
			theLog.Write(_T("change the paper source %d"),m_dwDefaultSource);
			pDevmodeWork->dmDefaultSource = m_dwDefaultSource;
		}
	}

	hdcPrinter = ::CreateDC((LPCTSTR)pDevName + pDevName->wDriverOffset,
		(LPCTSTR)pDevName + pDevName->wDeviceOffset,
		(LPCTSTR)pDevName + pDevName->wOutputOffset,
		(DEVMODE*)pDev);
	if (hdcPrinter == NULL)
	{
		theLog.Write(_T("hdcPrinter = null"));
		goto ERR_PRINT;
	}
	else
	{		
		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszOutput = NULL;
		docinfo.lpszDocName = m_szDocName.GetString();

		// call StartDoc() to begin printing
		nErrRet = StartDoc(hdcPrinter, &docinfo);
		if (nErrRet <= 0)
		{
			SP_ERROR;			
			goto ERR_PRINT;
		}

		CPdfHelper pdf;
#ifdef UNICODE
		if (pdf.OpenFile(CCommonFun::UnicodeToUTF8(szPdfPath).GetString()))
#else
		if (pdf.OpenFile(CCommonFun::MultiByteToUTF8(szPdfPath).GetString()))
#endif
		{
			int nCopy = m_dwCopies;
			int nFrom2 = 0;
			if (nCopy == 1)
			{
				nCopy = (m_nPrinted > 0) ? 0 : 1;
				nFrom2 = m_nPrinted;
			}
			else
			{
				int nPagePerCopy = nTo - nFrom + 1;
				int nTotalPage = nCopy * nPagePerCopy;
				int nLeftPage = nTotalPage - m_nPrinted;
				nCopy = nLeftPage / nPagePerCopy;
				int nLeftPage2 = nLeftPage - (nCopy * nPagePerCopy);
				nFrom2 = nPagePerCopy - nLeftPage2;
			}

			BOOL bColor = (m_dwColor == DMCOLOR_COLOR) ? TRUE : FALSE;
			DWORD dwPagePerCopy = pdf.GetPageCount();
			if(nFrom >= 0 && nTo >= nFrom && nTo < dwPagePerCopy)
			{
				theLog.Write(_T("print pdf from = %d to = %d ,copy = %d,nFrom2=%d"),nFrom,nTo,nCopy, nFrom2);
				for (int i=nFrom2; i <= nTo; i++)
				{
					theLog.Write(_T("print pdf nFrom2 = %d, i = %d"), nFrom2, i);
					pImage = GetPdf(pdf, i, bColor);
					if (pImage == NULL)
					{
						theLog.Write(_T("!!pImage == NULL, pageindex=%d"), i);
						goto ERR_PRINT;
					}
					ResetPrinterHDC(hdcPrinter, pDevmodeWork, *pImage); //重置HDC

					nErrRet = StartPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}

					Graphics graphics(hdcPrinter);
					graphics.SetSmoothingMode(SmoothingModeHighQuality);
					graphics.SetInterpolationMode(InterpolationModeHighQuality); 
					graphics.SetPageUnit(UnitPixel);
					graphics.SetPageScale(1.0);

					Rect rc;
					GetPrintDeviceArea(hdcPrinter, *pImage, rc, graphics, TRUE);	//获取打印区域,PDF包含了物理区域
					int nPictureWidth = pImage->GetWidth();
					int nPictureHeight = pImage->GetHeight();
					theLog.Write(_T("print pdf AA rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
						,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

					if (IsWater())
					{
						CPrintWithWaterMarked water;
						water.Init(pImage,&m_oSec);
						water.DrawImg(pImage);
						graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel);
					}
					else
					{
						graphics.DrawImage(pImage, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel);
					}

					delete pImage;
					pImage = NULL;

					graphics.ReleaseHDC(hdcPrinter);

					nErrRet = EndPage(hdcPrinter);
					if (nErrRet <= 0)
					{
						goto ERR_PRINT;
					}
					nPrinted++;
					//回调通知更新界面，通知服务器扣费
					if(pChargeCall)
					{
						pChargeCall->OnNewPagePrinted(nFrom2);
					}
				}			
				for (int x = 0; x < nCopy; x++ )
				{
					theLog.Write(_T("print pdf from = %d to = %d ,x = %d"),nFrom,nTo,x);
					for (int m = nFrom; m <= nTo; m ++)
					{
						theLog.Write(_T("print pdf nFrom = %d, m = %d"), nFrom, m);
						pImage = GetPdf(pdf, m, bColor);
						if (pImage == NULL)
						{
							theLog.Write(_T("!!pImage == NULL, pageindex=%d"), m);
							goto ERR_PRINT;
						}
						
						ResetPrinterHDC(hdcPrinter, pDevmodeWork, *pImage); //重置HDC

						nErrRet = StartPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}

						Graphics graphics(hdcPrinter);
						graphics.SetSmoothingMode(SmoothingModeHighQuality);
						graphics.SetInterpolationMode(InterpolationModeHighQuality); 
						graphics.SetPageUnit(UnitPixel);
						graphics.SetPageScale(1.0);

						Rect rc;
						GetPrintDeviceArea(hdcPrinter, *pImage, rc, graphics, TRUE);	//获取打印区域,PDF包含了物理区域
						int nPictureWidth = pImage->GetWidth();
						int nPictureHeight = pImage->GetHeight();
						theLog.Write(_T("print pdf BB rc.X=%d, rc.Y=%d, rc.Width=%d, rc.Height=%d, nPictureWidth=%d, nPictureHeight=%d")
							,rc.X, rc.Y, rc.Width, rc.Height, nPictureWidth, nPictureHeight);

						if (IsWater())
						{
							CPrintWithWaterMarked water;
							water.Init(pImage,&m_oSec);
							water.DrawImg(pImage);
							graphics.DrawImage(water.GetMarkedImg(), rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel);
						}
						else
						{
							graphics.DrawImage(pImage, rc, 0, 0, nPictureWidth, nPictureHeight, UnitPixel);
						}

						delete pImage;
						pImage = NULL;

						graphics.ReleaseHDC(hdcPrinter);

						nErrRet = EndPage(hdcPrinter);
						if (nErrRet <= 0)
						{
							goto ERR_PRINT;
						}
						nPrinted++;
						//回调通知更新界面，通知服务器扣费
						if(pChargeCall)
						{
							pChargeCall->OnNewPagePrinted(m);
						}
					}
				}
			}
			else
			{
				theLog.Write(_T("!!nFrom=%d,nTo=%d,dwPagePerCopy=%d"), nFrom, nTo, dwPagePerCopy);
				ASSERT(0);
			}				
		}
		else
		{
			theLog.Write(_T("!!pdf OpenFile fail.szPdfPath=[%s]"), szPdfPath);	
		}

		nErrRet = EndDoc(hdcPrinter);
		if (nErrRet <= 0)
		{
			goto ERR_PRINT;
		}

		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nPrinted);
		}
	}
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImage)
	{
		delete pImage;
	}
	
	return TRUE;

ERR_PRINT:
	if(hdcPrinter)
	{
		::DeleteDC(hdcPrinter);
		hdcPrinter = 0;
	}
	if (pDev)
	{
		delete[] pDev;
	}
	if (pDevName)
	{
		delete pDevName;
	}
	if (pImage)
	{
		delete pImage;
	}

	theLog.Write(_T("print pdf err:%d,prined:%d"),nErrRet,nPrinted);
	if (pChargeCall)
	{
		pChargeCall->OnPrintedErr(nPrinted,nErrRet);
	}
	return FALSE;
}

//add in 2013.06.13
BOOL CIOPrinter::PrintSplFile(CString szSplPath, IPrtStationPrintCallback* pChargeCall, int nTotalPage, BRAND_TYPE nType, BOOL bSaveSplFile/* = FALSE*/)
{
	theLog.Write(_T("CIOPrinter::PrintSplFile,1,szSplPath=%s,pChargeCall=%p,nTotalPage=%d,bSaveSplFile=%d"), szSplPath, pChargeCall, nTotalPage, bSaveSplFile);
	if(!PathFileExists(szSplPath) || m_sPrinter.IsEmpty())
	{
		theLog.Write(_T("!!CIOPrinter::PrintSplFile,2,szSplPath=[%s],m_sPrinter=[%s]"), szSplPath, m_sPrinter);
		int nPrinted = 0;
		if(pChargeCall)
		{
			pChargeCall->OnPrintedErr(nPrinted, -1);
		}
		return FALSE;
	}

	if(PrintSpl(m_sPrinter, szSplPath, nType, bSaveSplFile))
	{//打印成功
		theLog.Write(_T("CIOPrinter::PrintSplFile,3,PrintSpl succ,m_sPrinter=[%s]"),m_sPrinter);
		if(pChargeCall)
		{
			pChargeCall->OnPrintedCmplt(nTotalPage);
		}
		return TRUE;
	}
	else
	{//打印失败
		theLog.Write(_T("!!CIOPrinter::PrintSplFile,4,m_sPrinter=[%s],szSplPath=[%s]"), m_sPrinter, szSplPath);
		int nPrinted = 0;
		if(pChargeCall)
		{
			pChargeCall->OnPrintedErr(nPrinted, -1);
		}
		return FALSE;
	}
}

BOOL CopySplFile(CString szSource, CString szDest)
{
	if(szSource.IsEmpty() || szDest.IsEmpty())
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,1,szSource=%s,szDest=%s"), szSource, szDest);
		return FALSE;
	}
	if(!::PathFileExists(szSource))
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,2,NOT EXISTS,szSource=%s,"), szSource);
		return FALSE;
	}

	return CopyFile(szSource, szDest, FALSE);	//add by zxl,20131029

	CFile oFileSource;
	CFile oFileDest;
	DWORD dwFileLen = 0;
	DWORD dwBufLen = 40960;
	DWORD dwCount = 0;
	DWORD i = 0;
	BYTE *pBuf = NULL;
	DWORD dwHasReadLen = 0;
	DWORD dwNeedReadLen = 0;
	DWORD dwRet = 0;
	BOOL BFail = FALSE;
	if(!oFileSource.Open(szSource, CFile::modeRead))
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,3,Open fail,err=%d,szSource=%s,szDest=%s"), GetLastError(), szSource, szDest);
		goto ERR1;
	}
	dwFileLen = oFileSource.GetLength();

	if(!oFileDest.Open(szDest, CFile::modeCreate|CFile::modeWrite))
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,4,Open fail,err=%d,szSource=%s,szDest=%s"), GetLastError(), szSource, szDest);
		goto ERR2;
	}

	pBuf = new BYTE[dwBufLen];
	if(!pBuf)
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,5,new fail,dwBufLen=%d,err=%d,szSource=%s,szDest=%s")
			, dwBufLen, GetLastError(), szSource, szDest);
		goto ERR3;
	}

	if(dwFileLen <= dwBufLen)
	{
		try
		{
			while(dwHasReadLen < dwFileLen)
			{
				dwNeedReadLen = dwFileLen - dwHasReadLen;
				dwRet = oFileSource.Read(pBuf + dwHasReadLen, dwNeedReadLen);
				if(0 >= dwRet)
				{
					theLog.Write(_T("!!CIOPrinter::CopySplFile,6,read fail,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s]"), GetLastError(), dwHasReadLen, dwFileLen, szSource);
					BFail = TRUE;
					break;
				}

				dwHasReadLen += dwRet;
			}
		}
		catch(...)
		{
			theLog.Write(_T("!!CIOPrinter::CopySplFile,7,read catch sth,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s]"), GetLastError(), dwHasReadLen, dwFileLen, szSource);
			goto ERR4;
		}
		if(BFail)
		{
			theLog.Write(_T("!!CIOPrinter::CopySplFile,8,read fail,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s]"), GetLastError(), dwHasReadLen, dwFileLen,  szSource);
			goto ERR4;
		}

		try
		{
			oFileDest.Write(pBuf, dwHasReadLen);
		}
		catch(...)
		{
			theLog.Write(_T("!!CIOPrinter::CopySplFile,9,write fail,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s],szDest=%s"), GetLastError(), dwHasReadLen, dwFileLen, szSource, szDest);
			goto ERR4;
		}
	}//if(dwFileLen <= dwBufLen)
	else
	{
		dwCount = dwFileLen/dwBufLen;
		if(0 != (dwFileLen % dwBufLen))
		{
			dwCount++;
		}
		DWORD dwThisLoopNeedRdLen = dwBufLen;
		for(i = 0; i < dwCount; i++)
		{
			if(dwCount - 1 == i)
			{
				dwThisLoopNeedRdLen = dwFileLen - (dwBufLen * i);
			}
			try
			{
				dwHasReadLen = 0;
				while(dwHasReadLen < dwThisLoopNeedRdLen)
				{
					dwNeedReadLen = dwThisLoopNeedRdLen - dwHasReadLen;
					dwRet = oFileSource.Read(pBuf + dwHasReadLen, dwNeedReadLen);
					if(0 >= dwRet)
					{
						theLog.Write(_T("!!CIOPrinter::CopySplFile,16,read fail,i=%d,err=%d,dwHasReadLen=%d,dwThisLoopNeedRdLen=%d,szSource=[%s]")
							, i, GetLastError(), dwHasReadLen, dwThisLoopNeedRdLen, szSource);
						BFail = TRUE;
						break;
					}

					dwHasReadLen += dwRet;
				}
			}
			catch(...)
			{
				theLog.Write(_T("!!CIOPrinter::CopySplFile,17,read catch sth,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s]"), GetLastError(), dwHasReadLen, dwFileLen, szSource);
				break;			
			}
			if(BFail)
			{
				theLog.Write(_T("!!CIOPrinter::CopySplFile,18,read fail,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s]"), GetLastError(), dwHasReadLen, dwFileLen,  szSource);
				goto ERR4;
			}

			try
			{
				oFileDest.Write(pBuf, dwThisLoopNeedRdLen);
			}
			catch(...)
			{
				theLog.Write(_T("!!CIOPrinter::CopySplFile,19,write fail,err=%d,dwHasReadLen=%d,nFileLen=%d,szSource=[%s],szDest=%s"), GetLastError(), dwHasReadLen, dwFileLen, szSource, szDest);
				goto ERR4;
			}
		}//for(i = 0; i < dwCount; i++)
	}//else
	

	delete []pBuf;
	oFileDest.Flush();
	oFileDest.Close();
	oFileSource.Close();	

	return TRUE;
ERR4:
	delete []pBuf;
ERR3:
	oFileDest.Close();
ERR2:
	oFileSource.Close();
ERR1:
	return FALSE;
}


BOOL ChangeJobColorIfNeededEx2(CString szSplPath, CString &szNewSplPath, DWORD dwColor, DWORD dwDuplex)
{
	if(szSplPath.IsEmpty() || !::PathFileExists(szSplPath))
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx2,1,szSplPath=%s"), szSplPath);
		return FALSE;
	}

	DWORD dwOriPathLen = szSplPath.GetLength();
	int nDotPos = szSplPath.ReverseFind('.');
	szNewSplPath.Format(_T("%s_2.spl"), szSplPath.Left(nDotPos));

	theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx2,1.2,szNewSplPath=[%s]"), szNewSplPath);

	CFile oFileSource;
	CFile oFileDest;
	DWORD dwRet = 0;
	BOOL BFirst = TRUE;
	DWORD dwBufLen = 40961;	//40K + 1
	BYTE *pBuf = new BYTE[dwBufLen];
	if(!pBuf)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx2,2,new BYTE[%d] fail,err=%d,szSplPath=%s")
			, dwBufLen, GetLastError(), szSplPath);
		goto ERR1;
	}

	if(!oFileSource.Open(szSplPath, CFile::modeRead))
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,3,Open fail,err=%d,szSplPath=%s,szNewSplPath=%s"), GetLastError(), szSplPath, szNewSplPath);
		goto ERR2;
	}	

	if(!oFileDest.Open(szNewSplPath, CFile::modeCreate|CFile::modeWrite))
	{
		theLog.Write(_T("!!CIOPrinter::CopySplFile,4,Open fail,err=%d,szSplPath=%s,szNewSplPath=%s"), GetLastError(), szSplPath, szNewSplPath);
		goto ERR3;
	}

	do 
	{
		dwRet = oFileSource.Read(pBuf, dwBufLen - 1);
		if(0 < dwRet)
		{
			if(BFirst)
			{//改造字符串
				BFirst = FALSE;

				int nRet = 0;
				BYTE *pPos = NULL;
				CStringA szTmp ;
				pBuf[dwBufLen - 1] = 0x0;	//最后一个,保证可以形成字符串
				szTmp.Format("%s", pBuf);
				DWORD dwOriLen = szTmp.GetLength();
				pPos = pBuf + dwOriLen;	//字符串后面的有效缓存
				DWORD dwLeftLen = dwRet - dwOriLen;	//字符串后面的有效缓存长度
				CStringA szEnableDuplex = "@PJL SET DUPLEX = ON";
				CStringA szDisableDuplex = "@PJL SET DUPLEX = OFF";

#if 0
				if(0 == dwDuplex)	//临时换成单面,后面要换过来
				{//需要单面					
					nRet = szTmp.Replace(szEnableDuplex, szDisableDuplex);
					if(0 != nRet)
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,20,找到双面，替换成单面"));
					}
					else
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,20,没有找到双面"));
					}
				}
				else
				{//需要双面
					szTmp.Replace(szDisableDuplex, szEnableDuplex);
				}
#endif

				CStringA szEnableColor = "@PJL SET RENDERMODE = COLOR";	//启用彩色
				CStringA szDisableColor = "@PJL SET RENDERMODE = GRAYSCALE";	//启用黑白
				if(DMCOLOR_COLOR == dwColor)
				{//启用彩色
					nRet = szTmp.Replace(szDisableColor, szEnableColor);
					if(0 != nRet)
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,30,找到黑白，替换成彩色"));
					}
					else
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,30,没有找到黑白"));
					}
				}
				else
				{//启用黑白
					nRet = szTmp.Replace(szEnableColor, szDisableColor);
					if(0 != nRet)
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,40,找到彩色，替换成黑白"));
					}
					else
					{
						theLog.Write(_T("==CIOPrinter::CopySplFile,41,没有找到彩色"));
					}
				}

				DWORD dwNewTmpLen = szTmp.GetLength();

				oFileDest.Write(szTmp.GetBuffer(), dwNewTmpLen);
				szTmp.ReleaseBuffer();
				if(0 < dwLeftLen)
				{
					oFileDest.Write(pPos, dwLeftLen);
				}
			}//if(BFirst)
			else
			{
				oFileDest.Write(pBuf, dwRet);
			}
		}//if(0 < dwRet)
		else
		{
			break;
		}
	} while (1);

	oFileDest.Close();
	oFileSource.Close();

	if(!::DeleteFile(szSplPath))
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx2,50,DeleteFile fail,err=%d,szSplPath=[%s]"), GetLastError(), szSplPath);
	}
	if(!::CopyFile(szNewSplPath, szSplPath, FALSE))
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx2,51,CopyFile fail,err=%d,szSplPath=[%s]"), GetLastError(), szSplPath);
	}

#pragma message("================CIOPrinter::ChangeJobColorIfNeededEx2,100,测试，暂时没有删除处理过的spl临时文件")
	::DeleteFile(szNewSplPath);

	delete []pBuf;
	return TRUE;
ERR4:
	oFileDest.Close();
ERR3:
	oFileSource.Close();
ERR2:
	delete []pBuf;
ERR1:
	return FALSE;
}

BOOL ChangeJobColorIfNeededEx(CString szPrintName, DWORD dwColor, DWORD dwPaperSize)
{
	HANDLE hPrinter = NULL;
	if ( !::OpenPrinter(szPrintName.GetBuffer(), &hPrinter, NULL) )
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,1,OpenPrinter fail,err=%d,szPrintName=%s"), GetLastError(), szPrintName);
		return FALSE;
	}

	BOOL bNeedSet = FALSE;
	PDEVMODE DevMode = NULL;
	DWORD dwBytes       = DocumentProperties(NULL, hPrinter, szPrintName.GetBuffer(), NULL, NULL, 0);
	szPrintName.ReleaseBuffer();
	HGLOBAL lhDevMode   = GlobalAlloc(GHND, dwBytes);
	if(!lhDevMode)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,2,GlobalAlloc fail,err=%d,dwBytes=%d")
			, GetLastError(), dwBytes);
		goto ERR1;
	}
	DevMode = (PDEVMODE)GlobalLock(lhDevMode);                  //获取PDevMode结构
	if(!DevMode)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,3,GlobalLock fail,err=%d,dwBytes=%d,lhDevMode=0x%x")
			, GetLastError(), dwBytes, lhDevMode);
		goto ERR2;
	}
	DocumentProperties(NULL, hPrinter, szPrintName.GetBuffer(), DevMode, NULL, DM_OUT_BUFFER);
	if (!DevMode)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,4,DocumentProperties fail,err=%d,dwBytes=%d,lhDevMode=0x%x")
			, GetLastError(), dwBytes, lhDevMode);
		goto ERR2;
	}
#if 0
	DevMode->dmFields = DevMode->dmFields|DM_PAPERSIZE;   
	DevMode->dmFields = DevMode->dmFields|DM_PAPERLENGTH;
	DevMode->dmFields = DevMode->dmFields|DM_PAPERWIDTH;
	DevMode->dmPaperSize    = DMPAPER_USER;//设为自定义纸张
	//            DevMode->dmPaperLength  = m_paperHeight;
	//            DevMode->dmPaperWidth   = m_paperWidth;
#endif

	theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,6,dmColor=%d,dmPaperSize=%d,dmDuplex=0x%x,dmFields=0x%x")
		, DevMode->dmColor, DevMode->dmPaperSize, DevMode->dmDuplex, DevMode->dmFields);

	//改变颜色
	if (DevMode->dmFields & DM_COLOR)
	{
		if (DevMode->dmColor == dwColor)
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,11,the printer no change color %d"),dwColor);
		}
		else
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,12,change the color %d"),dwColor);
			DevMode->dmColor = dwColor;
			bNeedSet = TRUE;

			DevMode->dmFields |= DevMode->dmFields & DM_COLOR;
			if(IDOK != DocumentProperties(NULL, hPrinter, szPrintName.GetBuffer(), DevMode, DevMode, DM_IN_BUFFER|DM_OUT_BUFFER))
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,12.2,change the color %d,fail,err=%d"),dwColor, GetLastError());
			}
			szPrintName.ReleaseBuffer();
		}
	}
	else
	{
		theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13,no color"));
	}

	//测试单双面
	{
#if 0
		//双面打印
		if (DM_DUPLEX & DevMode->dmFields)
		{
			DMDUP_SIMPLEX;
			DMDUP_VERTICAL;
			DMDUP_HORIZONTAL;
//			if (DevMode->dmDuplex == m_dwDuplex)	//临时测试
			if (DevMode->dmDuplex == DMDUP_HORIZONTAL)	//临时测试
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.2,the printer no change duplex %d"), DMDUP_HORIZONTAL);
			}
			else
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.3,change the duplex %d"), DMDUP_HORIZONTAL);
				DevMode->dmDuplex = DMDUP_SIMPLEX;
			}
		}
#else
			DevMode->dmFields |= DevMode->dmFields & DM_DUPLEX;
			DevMode->dmDuplex = DMDUP_SIMPLEX;

			if(IDOK != DocumentProperties(NULL, hPrinter, szPrintName.GetBuffer(), DevMode, DevMode, DM_IN_BUFFER|DM_OUT_BUFFER))
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,13.211,change the duplex %d,fail,err=%d"), DevMode->dmPaperSize, GetLastError());
			}
			else
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.211,change the duplex OK!"));
			}
			szPrintName.ReleaseBuffer();
#endif
	}

	//测试a4转a3
#if 0
	{
		//改变纸张大小
		DM_PAPERSIZE;
		if (DevMode->dmFields & DM_PAPERSIZE)
		{
			if (DevMode->dmPaperSize  == dwPaperSize)
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.2,the printer no change papersize %d"),dwPaperSize);
			}
			else
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.4,change the papersize %d"), dwPaperSize);
				DevMode->dmPaperSize  = dwPaperSize;
				bNeedSet = TRUE;

				DevMode->dmFields |= pSrcDevMode->dmFields & DM_PAPERSIZE;
				if(IDOK != DocumentProperties(NULL, hPrinter, NULL, DevMode, DevMode, DM_IN_BUFFER|DM_OUT_BUFFER))
				{
					theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,12.2,change the color %d,fail,err=%d"),dwColor, GetLastError());
				}

			}
		}
		else
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,13.5,No PaperSize"));
		}
	}
#else
	DevMode->dmPaperSize = DMPAPER_A3;
	bNeedSet = TRUE;
	DevMode->dmFields |= DevMode->dmFields & DM_PAPERSIZE;
	if(IDOK != DocumentProperties(NULL, hPrinter, szPrintName.GetBuffer(), DevMode, DevMode, DM_IN_BUFFER|DM_OUT_BUFFER))
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,14.2,change the size %d,fail,err=%d"), DevMode->dmPaperSize, GetLastError());
	}
	szPrintName.ReleaseBuffer();

	if(!PrinterUtils::SetPrinterSetting( szPrintName, *DevMode))
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeededEx,15,SetPrinterSetting fail,err=%d"), GetLastError());
	}

#endif

	theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeededEx,17,dmColor=%d,dmPaperSize=%d")
		, DevMode->dmColor, DevMode->dmPaperSize);

#if 0
	if(bNeedSet)
	{
		dwBytes = DocumentProperties(NULL, hPrinter, NULL, DevMode, DevMode, DM_IN_BUFFER|DM_OUT_BUFFER);
	}
#endif


	GlobalUnlock(lhDevMode);
	GlobalFree(lhDevMode);
	::ClosePrinter(hPrinter);
	return TRUE;
ERR3:
	GlobalUnlock(lhDevMode);
ERR2:
	GlobalFree(lhDevMode);
ERR1:
	::ClosePrinter(hPrinter);
	return FALSE;
}

BOOL ChangeJobColorIfNeeded(HANDLE hPrinter, int nJobID, DWORD dwColor, DWORD dwPaperSize)
{
	if(!hPrinter || INVALID_HANDLE_VALUE == hPrinter)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,1,hPrinter=0x%x"), hPrinter);
		return FALSE;
	}

	DWORD cbNeed = 0;
	BYTE *pJobData = NULL;
	JOB_INFO_2* pJobInfo = NULL;
	BOOL bRet = FALSE;
	BOOL bNeedSet = FALSE;

	if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,2,GetJob1 error err：%d"),GetLastError());
			goto ERR1;
		}
	}	
	pJobData = new BYTE[cbNeed];
	if(!pJobData)
	{
		theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,3,new BYTE[%d] error err：%d"), cbNeed, GetLastError());
		goto ERR1;
	}
	memset(pJobData,0,cbNeed);
	DWORD dwState = 0;

	if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
	{
		theLog.Write(_T("!!CPrinterJobHelper::ChangeJobColorIfNeeded,4,GetJob2 error: %d"),GetLastError());
		goto ERR2;
	}	
	pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

	theLog.Write(_T("##CIOPrinter::ChangeJobColorIfNeeded,10,pPrinterName:%s,pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  ")
		_T(" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d ")
		_T(" dmColor:[%d],dmDuplex:%d,orig = %d,dmFields=[0x%x],dmPaperSize=[0x%x]"),
		pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
		pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
		pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
		pJobInfo->pDevMode->dmColor, pJobInfo->pDevMode->dmDuplex, pJobInfo->pDevMode->dmOrientation,
		pJobInfo->pDevMode->dmFields, pJobInfo->pDevMode->dmPaperSize);

	//改变颜色
	DMCOLOR_COLOR;
	if (pJobInfo->pDevMode->dmFields & DM_COLOR)
	{
		if (pJobInfo->pDevMode->dmColor == dwColor)
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,11,the printer no change color %d"),dwColor);
		}
		else
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,12,change the color %d"),dwColor);
			pJobInfo->pDevMode->dmColor = dwColor;
			bNeedSet = TRUE;
		}
	}
	else
	{
		theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,13,no color"));
	}

#pragma message("=============================CIOPrinter::ChangeJobColorIfNeeded,测试，先强制设置作业属性")

	//测试a4转a3
#if 0
	{
		//改变纸张大小
		DM_PAPERSIZE;
		if (pJobInfo->pDevMode->dmFields & DM_PAPERSIZE)
		{
			if (pJobInfo->pDevMode->dmPaperSize  == dwPaperSize)
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,13.2,the printer no change papersize %d"),dwPaperSize);
			}
			else
			{
				theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,13.4,change the papersize %d"), dwPaperSize);
				pJobInfo->pDevMode->dmPaperSize  = dwPaperSize;
				bNeedSet = TRUE;
			}
		}
		else
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,13.5,No PaperSize"));
		}
	}
#else
	pJobInfo->pDevMode->dmPaperSize = DMPAPER_A3;
	bNeedSet = TRUE;
#endif

	theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,14,bNeedSet=%d"), bNeedSet);

#if 0
	//设置
	if(bNeedSet)
	{
		if(!SetJob(hPrinter, nJobID, 2, pJobData, 0))
		{
			theLog.Write(_T("CIOPrinter::ChangeJobColorIfNeeded,15,SetJob fail,nJobID=%d,dwColor=0x%x")
				, nJobID, dwColor);
			goto ERR2;
		}
#if 0
		do
		{
			PRINTER_INFO_9 *pInfo9 = NULL;
			DWORD dwTmpBufLen = 0;
			BYTE *pTmpBuf = NULL;
			LPDEVMODE pDevMd = NULL;
			if(!GetPrinter(hPrinter, 9, NULL, 0, &dwTmpBufLen))
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,15.2,GetPrinter fail,nJobID=%d,err=%d")
					, nJobID, GetLastError());
				break;
			}

			pTmpBuf = new BYTE[dwTmpBufLen];
			if(!pTmpBuf)
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,15.3,new BYTE[%d] fail,nJobID=%d,err=%d")
					, dwTmpBufLen, nJobID, GetLastError());
				break;
			}

			if(!GetPrinter(hPrinter, 9, pTmpBuf, dwTmpBufLen, &dwTmpBufLen))
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,15.4,GetPrinter fail,nJobID=%d,err=%d")
					, nJobID, GetLastError());
				delete []pTmpBuf;
				break;
			}

			pInfo9 = (PRINTER_INFO_9 *)pTmpBuf;
			pDevMd = pInfo9->pDevMode;
			if(!pDevMd)
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,15.5,pDevMode=0x%x,nJobID=%d,err=%d")
					, pDevMd, nJobID, GetLastError());
				delete []pTmpBuf;
				break;
			}
			theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,16,dmColor=[0x%x],dmPaperSize=[0x%x]")
				, pDevMd->dmColor, pDevMd->dmPaperSize);

			delete []pTmpBuf;
		}
		while(0);
#endif
	}
#endif

	delete []pJobData;

#if 0
	{//临时测试
		if(!GetJob(hPrinter,nJobID,2,NULL,0,&cbNeed))
		{
			if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,22,GetJob1 error err：%d"),GetLastError());
				goto ERR1;
			}
		}	
		pJobData = new BYTE[cbNeed];
		if(!pJobData)
		{
			theLog.Write(_T("!!CIOPrinter::ChangeJobColorIfNeeded,23,new BYTE[%d] error err：%d"), cbNeed, GetLastError());
			goto ERR1;
		}
		memset(pJobData,0,cbNeed);
		DWORD dwState = 0;

		if(!GetJob(hPrinter,nJobID,2,pJobData,cbNeed,&cbNeed))//JOB_STATUS_SPOOLING
		{
			theLog.Write(_T("!!CPrinterJobHelper::ChangeJobColorIfNeeded,24,GetJob2 error: %d"),GetLastError());
			goto ERR2;
		}	
		pJobInfo = (JOB_INFO_2 *)pJobData;//DMDUP_SIMPLEX

		theLog.Write(_T("##CIOPrinter::ChangeJobColorIfNeeded,30,pPrinterName:%s,pMachineName:%s pUserName:%s pDocument:%s  pDatatype:%s  pPrintProcessor:%s  ")
			_T(" pDriverName:%s  pStatus:%s  Status:%d   Position:%d  TotalPages:%d  PagesPrinted:%d  Copys:%d ")
			_T(" dmColor:[%d],dmDuplex:%d,orig = %d,dmFields=[0x%x],dmPaperSize=[0x%x]"),
			pJobInfo->pPrinterName,pJobInfo->pMachineName,pJobInfo->pUserName,pJobInfo->pDocument,pJobInfo->pDatatype,
			pJobInfo->pPrintProcessor,pJobInfo->pDriverName,pJobInfo->pStatus,pJobInfo->Status,pJobInfo->Position,
			pJobInfo->TotalPages,pJobInfo->PagesPrinted,pJobInfo->pDevMode->dmCopies,
			pJobInfo->pDevMode->dmColor, pJobInfo->pDevMode->dmDuplex, pJobInfo->pDevMode->dmOrientation, 
			pJobInfo->pDevMode->dmFields, pJobInfo->pDevMode->dmPaperSize);

		delete []pJobData;
	}
#endif
	return TRUE;
ERR2:
	delete []pJobData;
ERR1:
	return FALSE;
}

//add in 2016.04.18
BOOL CIOPrinter::PrintPdf(CString szPrinter, CString szDocName, CString szPdfPath)
{
	theLog.Write(_T("CIOPrinter::PrintPdf,1,szPrinter=%s, szDocName=%s, szPdfPath=%s"), szPrinter, szDocName, szPdfPath);

	BOOL bOperateSucc = FALSE;
	BOOL bWriteSucc = TRUE;
	BYTE* pData = NULL;
	HANDLE hPrinter = NULL;
	CFile file;

	do 
	{
		TCHAR* pJobType = _T("RAW");
		PRINTER_DEFAULTS pd;
		ZeroMemory(&pd, sizeof(PRINTER_DEFAULTS));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		pd.pDatatype = pJobType;

		TCHAR cPrinterName[MAX_PATH] = {0};
		_tcscpy(cPrinterName, szPrinter);
		if(!OpenPrinter(cPrinterName,&hPrinter,&pd))
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,2,OpenPrinter fail, err=%u"), GetLastError());
			break;
		}

		if(!file.Open(szPdfPath,CFile::modeRead))
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,3,Open file fail, err=%u"), GetLastError());
			break;
		}

		theLog.Write(_T("CIOPrinter::PrintPdf,4,Open file succ, len=%u"), file.GetLength());

		pData = new BYTE[File_Read_Length_Unit];
		if(!pData)
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,4,pData=%p, err=%u"), pData, GetLastError());
			break;
		}

		DOC_INFO_1 docinfo;
		VerifyDocName(szDocName);
		docinfo.pDocName = szDocName.GetBuffer();
		docinfo.pOutputFile = NULL;
		docinfo.pDatatype = pJobType;
		if (!StartDocPrinter(hPrinter,1,(LPBYTE)&docinfo))
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,5,StartDocPrinter fail,err=%u"), GetLastError());
			break;
		}

		if (!StartPagePrinter(hPrinter))
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,6,StartPagePrinter fail,err=%u"), GetLastError());
			break;
		}

		while (TRUE)
		{
			memset(pData, 0x0, File_Read_Length_Unit);
			DWORD dwRead = file.Read(pData, File_Read_Length_Unit);
			if (dwRead > 0)
			{
				DWORD dwWrite = 0;
				if (!WritePrinter(hPrinter,pData,dwRead,&dwWrite))
				{
					bWriteSucc = FALSE;
					theLog.Write(_T("!!CIOPrinter::PrintPdf,7,WritePrinter fail,dwRead=%d, dwWrite=%d,err=%u")
						, dwRead, dwWrite, GetLastError());
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (!EndDocPrinter(hPrinter))
		{
			theLog.Write(_T("!!CIOPrinter::PrintPdf,8,EndDocPrinter fail,err=%u"), GetLastError());
		}

		bOperateSucc = TRUE;

	} while (FALSE);
	
	file.Close();

	if (hPrinter)
	{
		ClosePrinter(hPrinter);
	}
	if(pData)
	{
		delete[] pData;	
	}
	theLog.Write(_T("CIOPrinter::PrintPdf,9,bOperateSucc=%d,bWriteSucc=%d"), bOperateSucc, bWriteSucc);
	return (bOperateSucc && bWriteSucc);
}

//add in 2013.06.13,for 超大分辨率文件8900x6800,600DPI
BOOL CIOPrinter::PrintSpl(CString szPrinter, CString szSplPath, BRAND_TYPE nType, BOOL bSaveSplFile/* = FALSE*/)
{
	theLog.Write(_T("CIOPrinter::PrintSpl,1,szSplPath=%s,nType=%d,bSaveSplFile=%d"), szSplPath, nType, bSaveSplFile);
	TCHAR* pJobType = _T("RAW");
	HANDLE            hPrinter;
	BOOL BRet = FALSE;
	PRINTER_DEFAULTS   pd;
	ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = pJobType;

	do 
	{
		//ChangeJob(szSplPath, m_dwColor, m_dwDuplex, nType);
		if(!OpenPrinter(szPrinter.GetBuffer(),&hPrinter,&pd))
		{
			szPrinter.ReleaseBuffer();
			theLog.Write(_T("!!CIOPrinter::PrintSpl,2,OpenPrinter fail,err=%d,szPrinter=%s"), GetLastError(), szPrinter);
			break;
		}

		szPrinter.ReleaseBuffer();

		BYTE pData[1024] = {0};
		DWORD dwNeed = 0;
		BOOL bRet = AddJob(hPrinter,1,pData,1024,&dwNeed);
		if(!bRet)
		{
			theLog.Write(_T("!!CIOPrinter::PrintSpl,2.2,AddJob fail,err=%d,dwNeed=%d,szPrinter=%s"), GetLastError(), dwNeed, szPrinter);
			break;
		}
		ADDJOB_INFO_1* pInfo = (ADDJOB_INFO_1*)pData;
		CString szFile = pInfo->Path;
		int nJobID = pInfo->JobId;

		{//设置作业属性
			BYTE pJobData[1024] = {0};
			DWORD dwNeed = 0;
			if (GetJob(hPrinter, nJobID, 1, pJobData, 1024, &dwNeed))
			{
				JOB_INFO_1* pJobInfo = (JOB_INFO_1*)pJobData;
				//pJobInfo->pUserName = "abcd";	 //更改用户名
				pJobInfo->pDocument = m_szDocName.GetBuffer();	//更改文档名
				if (!SetJob(hPrinter, nJobID, 1, pJobData, 0))
				{
					theLog.Write(_T("!!CIOPrinter::PrintSpl,2.2.1,SetJob fail,err=%d,szPrinter=[%s],szSplPath=[%s],szFile=[%s]")
						, GetLastError(), szPrinter, szSplPath, szFile);
				}
			}
			else
			{
				theLog.Write(_T("!!CIOPrinter::PrintSpl,2.2.2,GetJob fail,err=%d,szPrinter=[%s],szSplPath=[%s],szFile=[%s]")
					, GetLastError(), szPrinter, szSplPath, szFile);
			}
		}

		if (bSaveSplFile)
		{
			if(!CopyFile(szSplPath, szFile, FALSE))
			{
				theLog.Write(_T("!!CIOPrinter::PrintSpl,2.3,CopyFile fail,err=%d,szPrinter=[%s],szSplPath=[%s],szFile=[%s]")
					, GetLastError(), szPrinter, szSplPath, szFile);
				break;
			}
		}
		else
		{
			if(!MoveFile(szSplPath, szFile))
			{
				theLog.Write(_T("!!CIOPrinter::PrintSpl,2.3,MoveFile fail,err=%d,szPrinter=[%s],szSplPath=[%s],szFile=[%s]")
					, GetLastError(), szPrinter, szSplPath, szFile);
				break;
			}
		}

		bRet = ScheduleJob(hPrinter,nJobID);
		if(bRet)
		{
			int nError = GetLastError();   
			theLog.Write(_T("CIOPrinter::PrintSpl,9,ScheduleJob OK,err=%d,nJobID=%d,szFile=[%s],szSplPath=[%s]"), GetLastError(), nJobID,  szFile, szSplPath);
		}
		else
		{
			theLog.Write(_T("!!CIOPrinter::PrintSpl,10,ScheduleJob fail,err=%d,nJobID=%d,szFile=[%s],szSplPath=[%s]"), GetLastError(), nJobID,  szFile, szSplPath);
			break;
		}

		BRet = TRUE;
	} while (0);

	if(hPrinter)
	{
		ClosePrinter(hPrinter);
	}

	return BRet;
}

BOOL CIOPrinter::IsWater()
{
	if (m_oSec.header.GetLength() > 0 ||
		m_oSec.footer.GetLength() > 0 ||
		m_oSec.water.GetLength() > 0)
	{
		return TRUE;
	}
	return FALSE;
}
void CIOPrinter::SetFooter(CString sVal)
{
	theLog.Write(_T("footer = %s"),sVal);
	m_oSec.footer = sVal;
}
void CIOPrinter::SetHeader(CString sVal)
{
	theLog.Write(_T("header = %s"),sVal);
	m_oSec.header = sVal;
}
void CIOPrinter::SetWatermark(CString sVal)
{
	theLog.Write(_T("watermark = %s"),sVal);
	m_oSec.water = sVal;
}

void CIOPrinter::KeepJob(HANDLE hPrint,BOOL bEnable ,BOOL& bKeep)
{
	PRINTER_INFO_2       *pPrinterInfo = NULL;

	DWORD  cByteNeeded, nReturned=0, cByteUsed;

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

void CIOPrinter::ChangeJob(CString szSplPath, DWORD dwColor, DWORD dwDuplex, BRAND_TYPE eType)
{
	theLog.Write(_T("CIOPrinter::ChangeJob,1,szSplPath=%s,m_dwColor=%d,m_dwDuplex=%d,eType=%d"), szSplPath, dwColor, dwDuplex, eType);

	SplType nSplType = GetSplType(szSplPath);
	theLog.Write(_T("CIOPrinter::ChangeJob,2,nSplType=%d"),nSplType);

	CPsPclJobChange *pPsPclJobChange = NULL;
	switch (eType)
	{
	case BT_HP:
		pPsPclJobChange = new CPsPclJobChangeHP;
		break;
	case BT_KM:
		pPsPclJobChange = new CPsPclJobChangeKM;
		break;
	default:
		pPsPclJobChange = new CPsPclJobChange;
		break;
	}

	if (pPsPclJobChange)
	{
		if (!pPsPclJobChange->ChangeJob(szSplPath,dwColor,dwDuplex,nSplType, m_szDocName))
		{
			theLog.Write(_T("!!CIOPrinter::ChangeJob,3,fail"));
		}
		delete pPsPclJobChange;
		pPsPclJobChange = NULL;
	}
	else
	{
		theLog.Write(_T("##CIOPrinter::ChangeJob,4,Not Support eType=%d"), eType);
	}
}

SplType CIOPrinter::GetSplType(CString szSplFilePath)
{
	SplType dwType = ST_UnKnow;
	if (szSplFilePath.IsEmpty() || !PathFileExists(szSplFilePath))
	{
		return dwType;
	}
	CStdioFile file;
	if (file.Open(szSplFilePath,CFile::modeRead))
	{
		//@PJL ENTER LANGUAGE = PCLXL
		//@PJL ENTER LANGUAGE=PCLXL
		//@PJL ENTER LANGUAGE = PCL
		//@PJL ENTER LANGUAGE=PCL
		//@PJL ENTER LANGUAGE = POSTSCRIPT
		//@PJL ENTER LANGUAGE=POSTSCRIPT
		//%!PS-Adobe-3.0
		//%!PS-Adobe-2.0

		DWORD dwReadLine = 0;
		CString szPjl = _T("");
		while(file.ReadString(szPjl) && (dwReadLine<200))
		{
			dwReadLine++;
			if (szPjl.Find(_T("LANGUAGE")) > 0)
			{
				if (szPjl.Find(_T("PCLXL")) > 0)//先查PCL6
				{
					dwType = ST_PCL;	//ST_PCL6
				}
				else if (szPjl.Find(_T("PCL")) > 0)
				{
					dwType = ST_PCL;	//ST_PCL5
				}
				else if (szPjl.Find(_T("POSTSCRIPT")) > 0)
				{
					dwType = ST_PS;
				}
			}
			else if ((szPjl.Find(_T("%!PS-Adobe-3.0")) == 0)
				|| (szPjl.Find(_T("%!PS-Adobe-2.0")) == 0))
			{
				dwType = ST_PS;
			}

			if (dwType > ST_UnKnow)
			{
				break;
			}
		}
		file.Close();
	}
	else
	{
		theLog.Write(_T("!!CPSPCLDlg::GetSplType,open fail,err=%d,szSplFilePath=%s"),GetLastError(),szSplFilePath);
	}
	return dwType;
}
