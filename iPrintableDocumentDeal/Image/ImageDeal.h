#ifndef _Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_
#define _Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_

enum PicturePostion
{
	PicturePostion_Cetter	= 0,	//����
	PicturePostion_Left		= 1,	//����
	PicturePostion_Rigth	= 2,	//����
	PicturePostion_Top		= 3,	//����
	PicturePostion_Bottom	= 4		//����
};

enum PictureSize
{
	PictureSize_Original	= 0,	//ԭʼ��С�����续���ͼƬС����ȱ������ŵ�����ʾ������ͼƬ
	PictureSize_Full		= 1		//�ȱ������ŵ�����ʾ������ͼƬ
};

/* <CloudDocDef.h>
#define Cloud_Doc_Format_UnKnow		0		//δ֪�ɴ�ӡ�ĵ�
#define Cloud_Doc_Format_Word		1		//office word
#define Cloud_Doc_Format_PowerPoint	2		//office PowerPoint
#define Cloud_Doc_Format_Excel		3		//office Excel
#define Cloud_Doc_Format_PDF		4		//Adobe Portable Document Format (PDF)
#define Cloud_Doc_Format_XPS		5		//Microsoft XML Paper Specification (XPS)
#define Cloud_Doc_Format_WMF		6		//Windows Metafile Format (WMF)
#define Cloud_Doc_Format_EMF		7		//Windows Enhanced MetaFile format (EMF)
#define Cloud_Doc_Format_PS			8		//Adobe PostScript (PS)
#define Cloud_Doc_Format_PCL		9		//Printer Control Language (PCL)
#define Cloud_Doc_Format_SPL		10		//Microsoft? Windows Spool File Format (SPL) (������pcl/ps/emf/xps�ȸ�ʽ)
#define Cloud_Doc_Format_JPG		11		//Joint Photographic Experts Group������ͼ��ר��С��/JPEG��
#define Cloud_Doc_Format_TIF		12		//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF��
#define Cloud_Doc_Format_PNG		13		//Portable Network Graphic Format (����ֲ����ͼ�θ�ʽ/PNG)
#define Cloud_Doc_Format_BMP		14		//Bitmap (λͼ)
#define Cloud_Doc_Format_GIF		15		//Graphics Interchange Format (ͼ�񻥻���ʽ/GIF)
*/
enum PrintFileFormat
{
	PrintFileFormat_UnKnow,		//0		//δ֪�ɴ�ӡ�ĵ�
	PrintFileFormat_Word,		//1		//office word
	PrintFileFormat_PowerPoint,	//2		//office PowerPoint
	PrintFileFormat_Excel,		//3		//office Excel
	PrintFileFormat_PDF,		//4		//Adobe Portable Document Format (PDF)
	PrintFileFormat_XPS,		//5		//Microsoft XML Paper Specification (XPS)
	PrintFileFormat_WMF,		//6		//Windows Metafile Format (WMF)
	PrintFileFormat_EMF,		//7		//Windows Enhanced MetaFile format (EMF)
	PrintFileFormat_PS,			//8		//Adobe PostScript (PS)
	PrintFileFormat_PCL,		//9		//Printer Control Language (PCL)
	PrintFileFormat_SPL,		//10	//Microsoft? Windows Spool File Format (SPL) (������pcl/ps/emf/xps�ȸ�ʽ)
	PrintFileFormat_JPEG,		//11	//Joint Photographic Experts Group������ͼ��ר��С��/JPEG��
	PrintFileFormat_TIFF,		//12	//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF��
	PrintFileFormat_PNG,		//13	//Portable Network Graphic Format (����ֲ����ͼ�θ�ʽ/PNG)
	PrintFileFormat_BMP,		//14	//Bitmap (λͼ)
	PrintFileFormat_GIF			//15	//Graphics Interchange Format (ͼ�񻥻���ʽ/GIF)
};

#define Image_Deal_Max_Dpi	300		//ͼƬ��������dpi



#endif //_Image_Deal_Def_H_39D7038C_1F92_4e76_8FD7_BEF986F65209_

