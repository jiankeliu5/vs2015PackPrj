#ifndef _PDFHELPER_517B7F85_B0C5_480E_B35B_199AC523DB5A_H_ 
#define _PDFHELPER_517B7F85_B0C5_480E_B35B_199AC523DB5A_H_

#ifdef  _AA_DLL_
#define _AA_DLL_EXPORT_		__declspec(dllexport)
#define _AA_DLL_EXPORT_C_ 	extern "C" __declspec(dllexport)
#else
#define _AA_DLL_EXPORT_		__declspec(dllimport)
#define _AA_DLL_EXPORT_C_	extern "C" __declspec(dllimport)
#endif

#include <windows.h>
#include <shlwapi.h>
#include <cstdio>

#include "PrintProcessDef.h"

#define PDF_DATA_DIR	L"PdfData"		//PDF字体cmap的数据目录名

//参考SplashColorMode
enum ImageColorMode {
	imageModeMono1,		// 1 bit per component, 8 pixels per byte,
	//   MSbit is on the left
	imageModeMono8,		// 1 byte per component, 1 byte per pixel
	imageModeRGB8,		// 1 byte per component, 3 bytes per pixel:
	//   RGBRGB...
	imageModeBGR8,		// 1 byte per component, 3 bytes per pixel:
	//   BGRBGR...
	imageModeXBGR8		// 1 byte per component, 4 bytes per pixel:
	//   XBGRXBGR...
};

typedef struct  
{
	const char* pszHeader;	//页眉内容，UTF8编码
	const char* pszFooter;	//水印内容，UTF8编码
	const char* pszWater;	//页脚内容，UTF8编码
}WaterInfo;

enum PageBoxType
{
	MediaBox,
	CropBox,
	BleedBox,
	TrimBox,
	ArtBox
};

typedef struct
{
	double X1;
	double Y1;
	double X2;
	double Y2;
}PageBox;

/* 关于CPdfHelper类的重要说明：
1.关于路径的描述使用UTF8编码，如：cPdfFilePath和GetPdfFilePath的返回值。
2.水印内容也是UTF8编码。
3.打印机名，用户名，文档名使用UTF8编码。
*/
class _AA_DLL_EXPORT_ CPdfHelper
{
public:
	//构造函数
	CPdfHelper();

	/**
	* @author zengxl
	* @param cPdfFilePath: pdf文件路径，UTF8编码
	* @param cOwnerPassword: 密码
	* @param cUserPassword: 密码
	*/
	//构造函数
	CPdfHelper(const char *cPdfFilePath, const char *cOwnerPassword = NULL, const char *cUserPassword = NULL);


	//析构函数
	~CPdfHelper();

	/**
	* @author zengxl
	* @param cPdfFilePath: pdf文件路径，UTF8编码
	* @param cOwnerPassword: 密码
	* @param cUserPassword: 密码
	* @return BOOL: TURE-打开成功，FALSE-打开失败
	*/
	//打开PDF文件
	BOOL OpenFile(const char *cPdfFilePath, const char *cOwnerPassword = NULL, const char *cUserPassword = NULL);

	/**
	* @author zengxl
	* @return void:
	*/
	//关闭被打开的pdf文件
	void CloseFile();

	/**
	* @author zengxl
	* @return BOOL: TURE-打开成功，FALSE-打开失败
	*/
	//判断PDF是否被正确的打开
	BOOL IsOK();

	/**
	* @author zengxl
	* @return int: PDF文件页数
	*/
	//获取打开的PDF文件页数
	int GetPageCount();

	/**
	* @author zengxl
	* @return const char*: PDF文件路径，UTF8编码
	*/
	//获取打开的PDF文件路径
	const char* GetPdfFilePath();

	/**
	* @author zengxl
	* @param nPageIndex: pdf页码，页码索引从0开始
	* @param hDPI: 获取的位图的横向dpi
	* @param vDPI: 获取的位图的竖向dpi
	* @param color: 获取的位图的色彩模式
	* @param nWidth: 获取的位图的宽度像素值，作为传出参数
	* @param nHeight: 获取的位图的高度像素值，作为传出参数
	* @param nStride: 获取的位图的Stride值，作为传出参数
	* @param scan0: 获取的位图的数据指针，作为传出参数
	* @return BOOL: TURE-获取成功，FALSE-获取失败
	*/
	//获取指定PDF页码的位图数据，当scan0在使用完后,必须调用FreeBitmapData释放内存
	BOOL GetBitmap(int nPageIndex, double hDPI, double vDPI, ImageColorMode color,
		int& nWidth, int& nHeight, int& nStride, unsigned char **scan0);

	/**
	* @author zengxl
	* @param scan0: 位图的数据指针
	* @return void:
	*/
	//释放GetBitmap接口返回的位图内存数据
	void FreeBitmapData(unsigned char *scan0);

	/**
	* @author zengxl
	* @param cPrinterName: 打印机名，UTF8编码
	* @param cDocName: 打印文档机名，UTF8编码
	* @param nBeginPage: 打印PDF的起始页，页码索引从0开始；如果是-1,表示从PDF第一页开始打印
	* @param nEndPage: 打印PDF的结束页，页码索引从0开始；如果是-1,表示打印至PDF最后一页
	* @param nPagesPerPaper: 多合一打印参数，表示多少页PDF内容打印至一张纸上面，只支持1,2,4,6,8,9,16
	* @param wColor: 打印PDF的输出色彩模式
	* @param wPaperSize: 打印PDF的输出纸张的纸型
	* @param wOrientation: 打印PDF的输出内容的方向，一般PDF内容有方向，所以，此参数暂时无效
	* @param wCopies: 打印PDF的输出份数，大于等于1
	* @param wCollate: 在打印多份文档时，是否启用逐份打印
	* @param wDuplex: 打印PDF的输出双面参数，包括单页，双面长边和双面短边
	* @param wPaperSource: 打印PDF的输出纸盒选择
	* @param wPaperWidth: 忽略参数wPaperSize，自定义打印PDF的输出纸张大小的宽度，单位是0.1毫米，要求大于0
	* @param wPaperHeight: 忽略参数wPaperSize，自定义打印PDF的输出纸张大小的高度，单位是0.1毫米，要求大于0
	* @param bUseCropBox: 打印PDF的CropBox范围内容，一般为FALSE
	* @param bUseFullPage: 打印PDF的内容到打印机的物理区域，而非打印区域，物理区域大于打印区域，一般使用物理区域
	* @param bUsePDFPageSize: 打印PDF的内容PDF的自身内容匹配的纸型，wPaperSize此时无效
	* @param bForceGray: 打印PDF的内容强制转换成灰度，一般是在wColor参数对打印机cPrinterName无效是才使用，但是一般可默认使用
	* @param pWaterInfo: 打印PDF时在页面绘制水印的内容，包括页眉，页脚和中间水印
	* @param callBack: 打印PDF的回调函数指针，包括打印的进度等信息
	* @param pCallBackData: 打印PDF的回调接口自带数据指针，在回调时会原样传回
	* @return BOOL: TURE-打印成功，FALSE-打印失败
	*/
	//使用指定的参数打印PDF文件到指定打印机
	BOOL PrintToPrinter(const char *cPrinterName, const char *cDocName = NULL,
		int nBeginPage = -1, int nEndPage = -1, int nPagesPerPaper = 1,
		WORD wColor = DMCOLOR_MONOCHROME, WORD wPaperSize = DMPAPER_A4,
		WORD wOrientation = DMORIENT_PORTRAIT, WORD wCopies = 1,
		WORD wCollate = DMCOLLATE_TRUE,
		WORD wDuplex = DMDUP_SIMPLEX, WORD wPaperSource = DMBIN_AUTO,
		WORD wPaperWidth = 0, WORD wPaperHeight = 0,
		BOOL bUseCropBox = FALSE, BOOL bUseFullPage = TRUE,
		BOOL bUsePDFPageSize = TRUE, BOOL bForceGray = FALSE,
		WaterInfo* pWaterInfo = NULL,
		FPrintProcessCallBack callBack = NULL,
		PrintProcessCallBackData* pCallBackData = NULL,
		BOOL bPauseJob = FALSE, int* pJobId = NULL);

	/**
	* @author zengxl
	* @param nPageIndex: PDF页码，页码索引从0开始
	* @param box: 获取的PDF的PageBox信息，作为传出参数
	* @param type: 获取的PDF的PageBox的类型
	* @return BOOL: TURE-获取成功，FALSE-获取失败
	*/
	//获取PDF中指定页码页面大小
	BOOL GetPageBox(int nPageIndex, PageBox& box, PageBoxType type = MediaBox);

	/**
	* @author zengxl
	* @param nPageIndex: PDF页码，页码索引从0开始
	* @return int: PDF页面旋转的角度，角度制，如0,90,270,360
	*/
	//获取PDF中指定页码页面旋转的角度
	int GetPageRotate(int nPageIndex);
	
	/**
	* @author zengxl
	* @param nPageIndex: PDF页码，页码索引从0开始
	* @return BOOL: TURE-表示空白页，FALSE-表示非空白页
	*/
	//判断指定的页面是否为空白页
	BOOL IsBlankPage(int nPageIndex);

	/**
	* @author zengxl
	* @param cJobUser: 打印作业作业所属用户，UTF8编码
	* @return void:
	*/
	//设置打印作业作业所属用户
	void SetJobUser(const char *cJobUser);

protected:
	void InitEnvironment();
	void SetPagesPerPaper(int nPagesPerPaper);
	BOOL IsBlankBitmap(ImageColorMode color, int nWidth, int nHeight, int nStride, unsigned char *scan0);

public:
	void *m_doc;			//PDFDoc
	void *m_surface;		//cairo_surface_t
	void *m_fileName;		//GooString
	void *m_outputName;		//GooString
	void *m_outputFileName;	//GooString
	void *m_imageFileName;	//GooString
	void *m_ownerPW;		//GooString
	void *m_userPW;			//GooString
	void *m_printer;		//GooString

	int m_firstPage;
	int m_lastPage;
	int m_pagesPerPaper;
	int m_currentPage;

	//
	bool m_png;
	bool m_jpeg;
	bool m_ps;
	bool m_eps;
	bool m_pdf;
	bool m_printToWin32;
	bool m_onePaperPrintCompleted;	//一张纸内容打印完成
	bool m_printdlg;
	bool m_svg;
	bool m_tiff;

	bool m_printOnlyOdd;
	bool m_printOnlyEven;
	bool m_singleFile;
	double m_max_resolution;
	double m_x_resolution;
	double m_y_resolution;
	int m_scaleTo;
	int m_x_scaleTo;
	int m_y_scaleTo;
	int m_crop_x;
	int m_crop_y;
	int m_crop_w;
	int m_crop_h;
	bool m_useCropBox;
	bool m_mono;
	bool m_gray;
	bool m_transp;

	bool m_level2;
	bool m_level3;
	bool m_origPageSizes;
	char m_paperSize[15];
	double m_paperWidth;
	double m_paperHeight;
	bool m_noCrop;
	bool m_expand;
	bool m_noShrink;
	bool m_noCenter;
	bool m_duplex;
	char m_tiffCompressionStr[16];

	bool m_setupdlg;

	bool m_useWin32PageSize;	//强制使用DEVMODE中的dmPaperSize，而不使用dmPaperWidth和dmPaperLength
	bool m_printing;
	bool m_printingAbort;		//打印被中止
	bool m_printingOutOfMemory;	//打印内存不足
	FILE *m_output_file;
	bool m_usePDFPageSize;
	bool m_useFullPage;
	bool m_forceGray;	//强制灰度(黑白色)打印,
						//某些打印机(已知:施乐)不能控制黑白和彩色打印,
						//在提交作业时,强制将作业渲染成灰度图.

	//
	HDC m_hdc;
	HGLOBAL m_hDevmode;
	HGLOBAL m_hDevnames;
	DEVMODEW *m_devmode;
	char* m_printerName;	//打印机名,UTF8编码
	char* m_jobUser;		//打印作业的所属用户,UTF8编码

	bool m_useWater;
	WaterInfo* m_waterInfo;
};

#endif	//_PDFHELPER_517B7F85_B0C5_480E_B35B_199AC523DB5A_H_