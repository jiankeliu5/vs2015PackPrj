#ifndef _PCL_DEF_BADA4D64_6A20_40EE_9CB0_44B6BE1A23A9_H_
#define _PCL_DEF_BADA4D64_6A20_40EE_9CB0_44B6BE1A23A9_H_

#include <assert.h>
#include <Windows.h>
// #include <WinGDI.h>
#include <string>
#include <stack>
#include <list>
// using namespace std;

namespace ParsePCL
{//命名空间ParsePCL开始

//add by zxl,20181128
/************************************************************************/
/* 本文件用于定义惠普公司开发的打印机语言（PCLXL）的数据结构和数据类型,
	支持协议PCLXL 2.0,2.1,3.0

参考文档：
	1.pcl_xl_2_0_technical_reference_rev2_2.pdf
	2.xl_ref21.pdf
	3.xl_ref30.pdf
	4.PCL分析日志.txt													*/
/************************************************************************/

/* 一些特殊字符的定义 */
#define PCL_ESC					0x1b	//Esc
#define PCL_At					0x40	//@
#define PCL_RigthBracket		0x29	//)
#define PCL_Space				0x20	//Space

typedef unsigned char			UByte;	//PCL_UByteData
typedef unsigned short			UInt16;	//PCL_UInt16Data
typedef unsigned int			UInt32;	//PCL_UInt32Data
typedef short					SInt16;	//PCL_SInt16Data
typedef int						SInt32;	//PCL_SInt32Data
typedef float					Real32;	//PCL_Real32Data

#define PCL_MaxNumberOfOperators		256	//操作类型的最大个数
#define PCL_MaxNumberOfAttributeIds		256 //属性值的最大个数

struct OperatorInfo 
{
	UByte ubOperator;
	char szOperator[64];
};

extern OperatorInfo gOperatorInfoTable[];

struct AttributeInfo 
{
	UByte ubAttributeId;
	char szAttribute[64];
};
extern AttributeInfo gAttributeInfoTable[];


/* 操作类型(Operator)宏定义开始 */
#define PCL_BeginSession		0x41 
#define PCL_EndSession			0x42
#define PCL_BeginPage			0x43 
#define PCL_EndPage				0x44
#define PCL_Comment				0x47
#define PCL_OpenDataSource		0x48 
#define PCL_CloseDataSource		0x49

#define PCL_BeginFontHeader		0x4f
#define PCL_ReadFontHeader		0x50
#define PCL_EndFontHeader		0x51
#define PCL_BeginChar			0x52 
#define PCL_ReadChar			0x53
#define PCL_EndChar				0x54  
#define PCL_RemoveFont			0x55
#define PCL_SetCharAttributes   0x56

#define PCL_SetDefaultGS		0x57
#define PCL_SetColorTreatment	0x58

#define PCL_BeginStream			0x5b
#define PCL_ReadStream			0x5c
#define PCL_EndStream			0x5d
#define PCL_ExecStream			0x5e
#define PCL_RemoveStream		0x5f

#define PCL_PopGS               0x60
#define PCL_PushGS              0x61
#define PCL_SetClipReplace      0x62
#define PCL_SetBrushSource      0x63
#define PCL_SetCharAngle        0x64 
#define PCL_SetCharScale        0x65
#define PCL_SetCharShear        0x66
#define PCL_SetClipIntersect    0x67 
#define PCL_SetClipRectangle    0x68
#define PCL_SetClipToPage       0x69 
#define PCL_SetColorSpace		0x6a
#define PCL_SetCursor			0x6b 
#define PCL_SetCursorRel		0x6c 
#define PCL_SetHalftoneMethod	0x6d
#define PCL_SetFillMode			0x6e
#define PCL_SetFont				0x6f

#define PCL_SetLineDash			0x70  
#define PCL_SetLineCap			0x71 
#define PCL_SetLineJoin			0x72
#define PCL_SetMiterLimit		0x73 
#define PCL_SetPageDefaultCTM	0x74
#define PCL_SetPageOrigin		0x75
#define PCL_SetPageRotation		0x76 
#define PCL_SetPageScale		0x77     
#define PCL_SetPaintTxMode		0x78
#define PCL_SetPenSource		0x79
#define PCL_SetPenWidth			0x7a 
#define PCL_SetROP				0x7b
#define PCL_SetSourceTxMode		0x7c
#define PCL_SetCharBoldValue	0x7d
#define PCL_SetNeutralAxis		0x7e
#define PCL_SetClipMode			0x7f
#define PCL_SetPathToClip		0x80
#define PCL_SetCharSubMode		0x81
#define PCL_BeginUserDefinedLineCaps	0x82
#define PCL_EndUserDefinedLineCaps		0x83
#define PCL_CloseSubPath		0x84   
#define PCL_NewPath				0x85     
#define PCL_PaintPath			0x86   

#define PCL_ArcPath				0x91
#define PCL_SetColorTrapping    0x92
#define PCL_BezierPath			0x93
#define PCL_SetAdaptiveHalftoning		0x94
#define PCL_BezierRelPath		0x95
#define PCL_Chord				0x96
#define PCL_ChordPath			0x97
#define PCL_Ellipse				0x98    
#define PCL_EllipsePath			0x99    

#define PCL_LinePath			0x9b

#define PCL_LineRelPath			0x9d
#define PCL_Pie					0x9e   
#define PCL_PiePath				0x9f

#define PCL_Rectangle			0xa0
#define PCL_RectanglePath		0xa1
#define PCL_RoundRectangle		0xa2
#define PCL_RoundRectanglePath  0xa3

#define PCL_Text				0xa8
#define PCL_TextPath			0xa9     

#define PCL_BeginImage          0xb0
#define PCL_ReadImage           0xb1 
#define PCL_EndImage            0xb2  
#define PCL_BeginRastPattern    0xb3 
#define PCL_ReadRastPattern     0xb4  
#define PCL_EndRastPattern      0xb5   
#define PCL_BeginScan           0xb6   
#define PCL_EndScan             0xb8  
#define PCL_ScanLineRel         0xb9  

#define PCL_PassThrough         0xbf		//XL 3.0
/* 操作类型(Operator)宏定义结束 */

/* 数据标签(data tags)宏定义开始 */
#define  PCL_UByteData			0xc0		//Unsigned 8-bit value
#define  PCL_UInt16Data			0xc1		//Unsigned 16-bit value
#define  PCL_UInt32Data			0xc2		//Unsigned 32-bit value
#define  PCL_SInt16Data			0xc3		//Signed 16-bit value
#define  PCL_SInt32Data			0xc4		//Signed 32-bit value
#define  PCL_Real32Data			0xc5		//Real number value
//Note: Real numbers are represented in standard 32-bit IEEE single-precision floating point format.
/* 0xc6     */   
/* 0xc7     */  
#define  PCL_UByteArray			0xc8		//Array of Unsigned 8-bit values
#define  PCL_UInt16Array		0xc9		//Array of Unsigned 16-bit values
#define  PCL_UInt32Array		0xca		//Array of Unsigned 32-bit values
#define  PCL_SInt16Array		0xcb		//Array of Signed 16-bit values
#define  PCL_SInt32Array		0xcc		//Array of Signed 32-bit values
#define  PCL_Real32Array		0xcd		//Array of Real number values
/* 0xce     */  
/* 0xcf     */  
#define  PCL_UByteXy			0xd0		//Two Unsigned 8-bit values
#define  PCL_UInt16Xy			0xd1		//Two Unsigned 16-bit values
#define  PCL_UInt32Xy			0xd2		//Two Unsigned 32-bit values
#define  PCL_SInt16Xy			0xd3		//Two Signed 16-bit values
#define  PCL_SInt32Xy			0xd4		//Two Signed 32-bit values
#define  PCL_Real32Xy			0xd5		//Two Real number values

#define  PCL_UByteBox			0xe0		//Four Unsigned 8-bit values
#define  PCL_UInt16Box			0xe1		//Four Unsigned 16-bit values
#define  PCL_UInt32Box			0xe2		//Four Unsigned 32-bit values
#define  PCL_SInt16Box			0xe3		//Four Signed 16-bit values
#define  PCL_SInt32Box			0xe4		//Four Signed 32-bit values
#define  PCL_Real32Box			0xe5		//Four Real number values
/* 数据标签(data tags)宏定义结束 */


/* 属性标识(AttributeID)宏定义开始 */
#define  PCL_AttributeIdByte	0xf8		//Unsigned, 8-bit Attribute
#define  PCL_AttributeIdUInt16	0xf9		//Unsigned, 16-bit Attribute
/* 属性标识(AttributeID)宏定义结束 */

/* 嵌入数据(Embed Data)宏定义开始 */
#define  PCL_EmbeddedData		0xfa		//Embedded Data Follows (0-0xffffffff bytes)
#define  PCL_EmbeddedDataByte	0xfb		//Embedded Data Follows (0-255 bytes)
/* 嵌入数据(Embed Data)宏定义结束 */


/* 属性标识和属性名的对应表 开始 */
//#define PCL_CMYColor			1
#define PCL_PaletteDepth		2
#define PCL_ColorSpace			3
//#define PCL_DDColorSpace		PCL_ColorSpace
#define PCL_NullBrush			4
#define PCL_NullPen				5
#define PCL_PaletteData			6
#define PCL_PatternSelectID		8
#define PCL_GrayLevel			9
#define PCL_RGBColor			11
#define PCL_PatternOrigin		12
#define PCL_NewDestinationSize	13
#define PCL_PrimaryArray		14
#define PCL_PrimaryDepth		15

#define PCL_AllObjectTypes		29
#define PCL_TextObjects			30
#define PCL_VectorObjects		31
#define PCL_RasterObjects		32
#define PCL_DeviceMatrix		33
#define PCL_DitherMatrixDataType	34
#define PCL_DitherOrigin		35
#define PCL_MediaDestination	36
#define PCL_MediaSize			37
#define PCL_MediaSource			38
#define PCL_MediaType			39
#define PCL_Orientation			40
#define PCL_PageAngle			41
#define PCL_PageOrigin			42
#define PCL_PageScale			43
#define PCL_ROP3				44
#define PCL_TxMode				45
#define PCL_CustomMediaSize		47
#define PCL_CustomMediaSizeUnits  48
#define PCL_PageCopies			49
#define PCL_DitherMatrixSize	50
#define PCL_DitherMatrixDepth	51
#define PCL_SimplexPageMode		52
#define PCL_DuplexPageMode		53
#define PCL_DuplexPageSide		54

#define PCL_ArcDirection		65
#define PCL_BoundingBox			66
#define PCL_DashOffset			67
#define PCL_EllipseDimension	68
#define PCL_EndPoint			69
#define PCL_FillMode			70
#define PCL_LineCapStyle		71
#define PCL_LineJoinStyle		72
#define PCL_MiterLength			73
#define PCL_LineDashStyle		74
#define PCL_PenWidth			75
#define PCL_Point				76
#define PCL_NumberOfPoints		77
#define PCL_SolidLine			78
#define PCL_StartPoint			79
#define PCL_PointType			80
#define PCL_ControlPoint1		81
#define PCL_ControlPoint2		82
#define PCL_ClipRegion			83
#define PCL_ClipMode			84


//#define PCL_ColorDepthArray	97
#define PCL_ColorDepth			98
//#define PCL_PixelDepth		PCL_ColorDepth
#define PCL_BlockHeight			99
#define PCL_ColorMapping		100
//#define PCL_PixelEncoding		PCL_ColorMapping
#define PCL_CompressMode		101
#define PCL_DestinationBox		102
#define PCL_DestinationSize		103
#define PCL_PatternPersistence	104
#define PCL_PatternDefineID		105
#define PCL_SourceHeight		107
#define PCL_SourceWidth			108
#define PCL_StartLine			109
#define PCL_XPairType			110
#define PCL_NumberOfXPairs		111
//#define PCL_YStart			112
//#define PCL_XStart			113
//#define PCL_XEnd				114
#define PCL_NumberOfScanLines	115

#define PCL_ColorTreatment		120
#define PCL_CommentData			129
#define PCL_DataOrg				130
#define PCL_Measure				134
#define PCL_SourceType			136
#define PCL_UnitsPerMeasure		137
//#define PCL_QueryKey			138
#define PCL_StreamName			139
#define PCL_StreamDataLength	140

#define PCL_ErrorReport			143
//#define PCL_IOReadTimeOut		144

//#define PCL_PassThroughCommand	158
//#define PCL_PassThroughArray	159
//#define PCL_Diagnostics		160
#define PCL_CharAngle			161
#define PCL_CharCode			162
#define PCL_CharDataSize		163
#define PCL_CharScale			164
#define PCL_CharShear			165
#define PCL_CharSize			166
#define PCL_FontHeaderLength	167
#define PCL_FontName			168
#define PCL_FontFormat			169
#define PCL_SymbolSet			170
#define PCL_TextData			171
#define PCL_CharSubModeArray	172
#define PCL_PCLSelectFont		173

#define PCL_XSpacingData		175
#define PCL_YSpacingData		176
#define PCL_CharBoldValue		177
/* 属性标识和属性名的对应表 结束 */

/* enable Enumeration ******************************************************/
#define PCL_eOn                      0
#define PCL_eOff                     1

/* boolean Enumeration *****************************************************/
#define PCL_eFalse                   0
#define PCL_eTrue                    1

/* unitOfMeasure Enumeration *******************************************/
#define PCL_eInch                    0
#define PCL_eMillimeter              1
#define PCL_eTenthsOfAMillimeter     2

/* errorReportingEnumeration *********************************************/
#define PCL_eNoReporting             0  
#define PCL_eBackChannel             1  
#define PCL_eErrorPage               2    
#define PCL_eBackChAndErrPage        3
#define PCL_eBackChanAndErrPage      3 /* for backward compatibilty only */
#define PCL_eNWBackChannel			 4	//2.0
#define PCL_eNWErrorPage			 5	//2.0
#define PCL_eNWBackChAndErrPage		 6	//2.0

/* dataOrganizationEnumeration ********************************************/
#define PCL_eBinaryHighByteFirst     0
#define PCL_eBinaryLowByteFirst      1 

/* duplexPageModeEnumeration **********************************************/
#define PCL_eDuplexHorizontalBinding 0
#define PCL_eDuplexVerticalBinding   1 

/* duplexPageSideEnumeration **********************************************/
#define PCL_eFrontMediaSide          0
#define PCL_eBackMediaSide           1

/* simplexPageModeEnumeration *********************************************/
#define PCL_eSimplexFrontSide        0

/* orientationEnumeration *************************************************/
#define PCL_ePortraitOrientation     0  
#define PCL_eLandscapeOrientation    1 
#define PCL_eReversePortrait         2   
#define PCL_eReverseLandscape        3 
#define PCL_eDefaultOrientation      4 

/* mediaSize Enumeration ***************************************************/
#define PCL_eLetterPaper             0
#define PCL_eLegalPaper              1
#define PCL_eA4Paper                 2
#define PCL_eExecPaper               3
#define PCL_eLedgerPaper             4
#define PCL_eA3Paper                 5
#define PCL_eCOM10Envelope           6
#define PCL_eMonarchEnvelope         7
#define PCL_eC5Envelope              8
#define PCL_eDLEnvelope              9
#define PCL_eJB4Paper                10
#define PCL_eJB5Paper                11
#define PCL_eB5Envelope              12
#define PCL_eB5Paper	             13	//2.1
#define PCL_eJPostcard               14
#define PCL_eJDoublePostcard         15
#define PCL_eA5Paper                 16
#define PCL_eA6Paper                 17	//2.0
#define PCL_eJB6Paper                18	//2.0
#define PCL_JIS8K					 19	//2.1
#define PCL_JIS16K					 20	//2.1
#define PCL_JISExec					 21	//2.1
#define PCL_eDefaultPaperSize		 96	//2.1

/* mediaSource Enumeration *************************************************/
#define PCL_eDefaultSource          0
#define PCL_eAutoSelect             1    
#define PCL_eManualFeed             2
#define PCL_eMultiPurposeTray       3
#define PCL_eUpperCassette          4
#define PCL_eLowerCassette          5
#define PCL_eEnvelopeTray           6
#define PCL_eThirdCassette          7	//2.0
//External Trays 1-248				8-255	//2.0 扩展纸盒，1-248

/* mediaDestinationEnumeration ********************************************/
#define PCL_eDefaultDestination     0
#define PCL_eFaceDownBin            1
#define PCL_eFaceUpBin              2
#define PCL_eJobOffsetBin           3
//External Bins 1-251				5-255 //扩展输出，1-251

/* compressionEnumeration *************************************************/
#define PCL_eNoCompression          0 
#define PCL_eRLECompression         1
#define PCL_eJPEGCompression        2	//2.0
#define PCL_eDeltaRowCompression    3	//2.0

/* arcDirectionEnumeration ************************************************/
#define PCL_eClockWise              0
#define PCL_eCounterClockWise       1 

/* fillModeEnumeration ****************************************************/
#define PCL_eNonZeroWinding         0
#define PCL_eEvenOdd                1

/* lineEndEnumeration *****************************************************/
#define PCL_eButtCap                0              
#define PCL_eRoundCap               1
#define PCL_eSquareCap              2
#define PCL_eTriangleCap            3 

#define PCL_eButtEnd                0  /* xxxEnd for backward compatibility */             
#define PCL_eRoundEnd               1  /* xxxEnd for backward compatibility */
#define PCL_eSquareEnd              2  /* xxxEnd for backward compatibility */
#define PCL_eTriangleEnd            3  /* xxxEnd for backward compatibility */

/* charSubModeEnumeration *************************************************/
#define PCL_eNoSubstitution         0
#define PCL_eVerticalSubstitution   1

/* lineJoinEnumeration ****************************************************/
#define PCL_eMiterJoin              0
#define PCL_eRoundJoin              1
#define PCL_eBevelJoin              2
#define PCL_eNoJoin                 3

/* ditherMatrixEnumeration ************************************************/
#define PCL_eDeviceBest             0
#define PCL_eDeviceIndependent      1

/* dataSourceEnumeration **************************************************/
#define PCL_eDefaultDataSource      0 

/* colorSpaceEnumeration **************************************************/
#define PCL_eGray                   1   
#define PCL_eRGB                    2  

/* colorDepthEnumeration **************************************************/
#define PCL_e1Bit                   0  
#define PCL_e4Bit                   1 
#define PCL_e8Bit                   2  

/* colorMappingEumeration *************************************************/
#define PCL_eDirectPixel            0 
#define PCL_eIndexedPixel           1 

/* colorTreatmentEumeration *************************************************/
#define PCL_eNoTreatment            0 
#define PCL_eScreenMatch            1 
#define PCL_eVivid		            2 

/* diagnosticEumeration ***************************************************/
#define PCL_eNoDiag                 0      
#define PCL_eFilterDiag             1  
#define PCL_eCommandsDiag           2  
#define PCL_ePersonalityDiag        3  
#define PCL_ePageDiag               4    

/* clipmodeEnumeration ***************************************************/
#define PCL_eInterior               0
#define PCL_eExterior               1

/* dataTypeEnumeration ***************************************************/
#define PCL_eUByte                  0
#define PCL_eSByte                  1
#define PCL_eUInt16                 2
#define PCL_eSInt16                 3
#define PCL_eReal32                 4

/* patternPersistenceEnumeration *****************************************/
#define PCL_eTempPattern            0
#define PCL_ePagePattern            1
#define PCL_eSessionPattern         2

/* transparancyEnumeration ************************************************/
#define PCL_eOpaque                 0
#define PCL_eTransparent            1

/* charSubModeEnumeration      ***********************************************/
#define PCL_eNoSubstitution         0
#define PCL_eVerticalSubstitution   1 

}//命名空间ParsePCL结束

#endif	//_PCL_DEF_BADA4D64_6A20_40EE_9CB0_44B6BE1A23A9_H_