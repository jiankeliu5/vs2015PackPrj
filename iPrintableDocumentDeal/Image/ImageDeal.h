#ifndef _Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_
#define _Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_

enum PicturePostion
{
	PicturePostion_Cetter	= 0,	//居中
	PicturePostion_Left		= 1,	//居左
	PicturePostion_Rigth	= 2,	//居右
	PicturePostion_Top		= 3,	//剧上
	PicturePostion_Bottom	= 4		//居下
};

enum PictureSize
{
	PictureSize_Original	= 0,	//原始大小，假如画面比图片小，则等比例缩放到可显示的最大的图片
	PictureSize_Full		= 1		//等比例缩放到可显示的最大的图片
};

/* <CloudDocDef.h>
#define Cloud_Doc_Format_UnKnow		0		//未知可打印文档
#define Cloud_Doc_Format_Word		1		//office word
#define Cloud_Doc_Format_PowerPoint	2		//office PowerPoint
#define Cloud_Doc_Format_Excel		3		//office Excel
#define Cloud_Doc_Format_PDF		4		//Adobe Portable Document Format (PDF)
#define Cloud_Doc_Format_XPS		5		//Microsoft XML Paper Specification (XPS)
#define Cloud_Doc_Format_WMF		6		//Windows Metafile Format (WMF)
#define Cloud_Doc_Format_EMF		7		//Windows Enhanced MetaFile format (EMF)
#define Cloud_Doc_Format_PS			8		//Adobe PostScript (PS)
#define Cloud_Doc_Format_PCL		9		//Printer Control Language (PCL)
#define Cloud_Doc_Format_SPL		10		//Microsoft? Windows Spool File Format (SPL) (可能是pcl/ps/emf/xps等格式)
#define Cloud_Doc_Format_JPG		11		//Joint Photographic Experts Group（联合图像专家小组/JPEG）
#define Cloud_Doc_Format_TIF		12		//Tagged Image File Format（标签图像文件格式/TIFF）
#define Cloud_Doc_Format_PNG		13		//Portable Network Graphic Format (可移植网络图形格式/PNG)
#define Cloud_Doc_Format_BMP		14		//Bitmap (位图)
#define Cloud_Doc_Format_GIF		15		//Graphics Interchange Format (图像互换格式/GIF)
*/
enum PrintFileFormat
{
	PrintFileFormat_UnKnow,		//0		//未知可打印文档
	PrintFileFormat_Word,		//1		//office word
	PrintFileFormat_PowerPoint,	//2		//office PowerPoint
	PrintFileFormat_Excel,		//3		//office Excel
	PrintFileFormat_PDF,		//4		//Adobe Portable Document Format (PDF)
	PrintFileFormat_XPS,		//5		//Microsoft XML Paper Specification (XPS)
	PrintFileFormat_WMF,		//6		//Windows Metafile Format (WMF)
	PrintFileFormat_EMF,		//7		//Windows Enhanced MetaFile format (EMF)
	PrintFileFormat_PS,			//8		//Adobe PostScript (PS)
	PrintFileFormat_PCL,		//9		//Printer Control Language (PCL)
	PrintFileFormat_SPL,		//10	//Microsoft? Windows Spool File Format (SPL) (可能是pcl/ps/emf/xps等格式)
	PrintFileFormat_JPEG,		//11	//Joint Photographic Experts Group（联合图像专家小组/JPEG）
	PrintFileFormat_TIFF,		//12	//Tagged Image File Format（标签图像文件格式/TIFF）
	PrintFileFormat_PNG,		//13	//Portable Network Graphic Format (可移植网络图形格式/PNG)
	PrintFileFormat_BMP,		//14	//Bitmap (位图)
	PrintFileFormat_GIF			//15	//Graphics Interchange Format (图像互换格式/GIF)
};

#define Image_Deal_Max_Dpi	300		//图片处理的最大dpi



#endif //_Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_

