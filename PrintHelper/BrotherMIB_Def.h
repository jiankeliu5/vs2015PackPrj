#ifndef _BROTHER_MIB_DEF_02CEC903_48AC_4A56_B824_8AE1B254AA04_H_
#define _BROTHER_MIB_DEF_02CEC903_48AC_4A56_B824_8AE1B254AA04_H_

#include "Snmp_Def.h"

/************************************************************************/
/* 本文档定义的结构类型参考下列文件：[iSecPrint\Doc\Snmp\兄弟]
	1.Brother HL-L8250CDN series.log
	2.Brother MFC-8540DN.log
	3.snmp记录.txt
	4.DBJOB说明.docx
*/
/************************************************************************/

BEGIN_ENUM_STRUCT(BrotherFunctionType)
{
	PrintTotalPages = 1,
	FaxTotalPages = 2,
	CopyTotalPages = 3,
	CopyColorPages = 4,
	PrintColorPages = 5,
	FaxColorPages = 6,
	PrintBlackWhitePages = 7,
	FaxBlackWhitePages = 8,
	CopyBlackWhitePages = 9,
	Type10 = 10,
	PrintDuplxTotalPages = 11,
	CopyDuplxTotalPages = 12,
	FaxDuplxTotalPages = 13,
	Type12 = 14,
	Type24 = 24,
	Type25 = 25,
	Type26 = 26,
	Type45 = 45,
	Type80 = 80,
	Type81 = 81,
	Type90 = 90,
	OtherTotalPages = 91,
	OtherBlackWhitePages = 92,
	OtherColorPages = 93,
	OtherDuplxPages = 94
}
END_ENUM_STRUCT(BrotherFunctionType)

BEGIN_ENUM_STRUCT(BrotherPaperType)
{
	Executive = 1,
	Letter = 2,
	Legal = 3,
	A6 = 24,
	A5 = 25,
	A4 = 26,
	B6 = 99,
	B5 = 100,
	OthersMono = 702,
	OthersColor = 703,
	A4_Letter = 898,
	B5_Executive = 899,
	Envelope = 900,
	Folio = 909,	//LGL/FOLIO(LGL/A4LONG/FOLIO)
	Photograph_4x6 = 928,		//4x6	6寸照片
	A3 = 929,
	Others = 999,
	Auto = 1001
}
END_ENUM_STRUCT(BrotherPaperType)

//总出纸计数，根据纸型分类
#define BrotherCounterInfo1OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdAEdi")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.1.1"
typedef struct tag_BrotherCounterInfo1Entry	
{
	Integer32 Index;
	ENUM_STRUCT_TYPE(BrotherPaperType) Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo1Entry, *PBrotherCounterInfo1Entry;
typedef map<int, PBrotherCounterInfo1Entry> BrotherCounterInfo1EntryMap;
typedef list<PBrotherCounterInfo1Entry> BrotherCounterInfo1EntryList;

//单面出纸统计
#define BrotherCounterInfo2OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdA0di")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.2.1"
typedef struct tag_BrotherCounterInfo2Entry	
{
	Integer32 Index;
	ENUM_STRUCT_TYPE(BrotherPaperType) Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo2Entry, *PBrotherCounterInfo2Entry;
typedef map<int, PBrotherCounterInfo2Entry> BrotherCounterInfo2EntryMap;
typedef list<PBrotherCounterInfo2Entry> BrotherCounterInfo2EntryList;

//总出纸统计，根据色彩分类
#define BrotherCounterInfo3OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdAkdi")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.3.1"
typedef struct tag_BrotherCounterInfo3Entry	
{
	Integer32 Index;
	ENUM_STRUCT_TYPE(BrotherPaperType) Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo3Entry, *PBrotherCounterInfo3Entry;
typedef map<int, PBrotherCounterInfo3Entry> BrotherCounterInfo3EntryMap;
typedef list<PBrotherCounterInfo3Entry> BrotherCounterInfo3EntryList;

//双面出纸统计
#define BrotherCounterInfo4OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdBUdi")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.4.1"
typedef struct tag_BrotherCounterInfo4Entry	
{
	Integer32 Index;
	ENUM_STRUCT_TYPE(BrotherPaperType) Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo4Entry, *PBrotherCounterInfo4Entry;
typedef map<int, PBrotherCounterInfo4Entry> BrotherCounterInfo4EntryMap;
typedef list<PBrotherCounterInfo4Entry> BrotherCounterInfo4EntryList;

//总出纸统计,根据打印、复印、扫描、传真的功能和单双面分类，
//好像当只有打印功能的情况下没有有效的数据。
#define BrotherCounterInfo21OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdA1h9VA==") //".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.21.1"
typedef struct tag_BrotherCounterInfo21Entry	
{
	Integer32 Index;
	ENUM_STRUCT_TYPE(BrotherFunctionType) Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo21Entry, *PBrotherCounterInfo21Entry;
typedef map<int, PBrotherCounterInfo21Entry> BrotherCounterInfo21EntryMap;
typedef list<PBrotherCounterInfo21Entry> BrotherCounterInfo21EntryList;

//未知
#define BrotherCounterInfo22OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdA1t9VA==")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.22.1"
typedef struct tag_BrotherCounterInfo22Entry	
{
	Integer32 Index;
	Integer32 Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo22Entry, *PBrotherCounterInfo22Entry;
typedef map<int, PBrotherCounterInfo22Entry> BrotherCounterInfo22EntryMap;
typedef list<PBrotherCounterInfo22Entry> BrotherCounterInfo22EntryList;

//未知
#define BrotherCounterInfo31OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdAlh9VA==")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.31.1"
typedef struct tag_BrotherCounterInfo31Entry	
{
	Integer32 Index;
	Integer32 Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo31Entry, *PBrotherCounterInfo31Entry;
typedef map<int, PBrotherCounterInfo31Entry> BrotherCounterInfo31EntryMap;
typedef list<PBrotherCounterInfo31Entry> BrotherCounterInfo31EntryList;

//未知
#define BrotherCounterInfo41OID TEXT("HwAgCk1dIRNPFlxVRzoLF1chGgQGHUEbOhMCRjZaBBxHVBsjFwpBNgdPQAcCXH1XTQF9TU9GHQNHYktWHGZaVEAdBVh9VA==")	//".iso.org.dod.internet.private.enterprises.2435.2.3.9.4.2.1.5.5.52.41.1"
typedef struct tag_BrotherCounterInfo41Entry	
{
	Integer32 Index;
	Integer32 Type;	
	Integer32 PageNumbers;			
}BrotherCounterInfo41Entry, *PBrotherCounterInfo41Entry;
typedef map<int, PBrotherCounterInfo41Entry> BrotherCounterInfo41EntryMap;
typedef list<PBrotherCounterInfo41Entry> BrotherCounterInfo41EntryList;

#endif	//_BROTHER_MIB_DEF_02CEC903_48AC_4A56_B824_8AE1B254AA04_H_
