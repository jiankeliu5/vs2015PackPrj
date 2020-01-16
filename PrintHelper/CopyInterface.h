#pragma once

enum PAPER_TYPE
{
	A3_PAPER,
		A4_PAPER
};

enum PAPER_COLOR
{
	NULL_PAPER,
	WB_PAPER,
	COLOR_PAPER
};

enum COPY_INFO_FROM
{
	CIF_UnKnow,
	CIF_HardWare,
	CFI_Snmp
};

enum COPY_OP_TYPE
{
	COPY_START,
	COPY_STOP
};

enum COPY_TYPE
{
	COPY_TYPE_ASYNCHRONOUS,  //异步的， 即边扫描边打印，大部分机器原生复印都如此
	COPY_TYPE_SYNCHRONIZED //同步的， 即先扫描后打印，如复印拆分或预复印
};

/*复印阶段*/
enum COPY_STAGE
{
	COPY_STAGE_IDLE,
	COPY_STAGE_BEGIN,
	COPY_STAGE_SCANCMLT,
	COPY_STAGE_PROCESSING,
	COPY_STAGE_END
};

enum BRAND_TYPE
{
	BT_THIN_CLT=1,
	BT_SL,
	BT_DZ,
	BT_KM,
	BT_LM,
	BT_LG,		//add by zxl,2013.03.08,理光打印机
	BT_SL2,		//add by zxl,2013.07.01,施乐的新电路板
	BT_JC,		//add by zxl,2013.09.16,京瓷打印机
	BT_ZD,		//add by zxl,2013.09.17,震旦打印机
	BT_XP,		//add by zxl,2013.08.09,夏普打印机
	BT_SX,		//add by zxl,2015.04.27,三星打印机
	BT_HP,		//add by zxl,2015.07.16,惠普打印机
	BT_EPSON,	//add by szy 2016.04.20,爱普生打印机
	BT_CANON,	//add by szy 2016.04.20,佳能打印机
	BT_BROTHER, //add by zfq 2016.05.04,兄弟打印机
	BT_JT,		//add by zfq 2016.05.04,京图打印机
	BT_OKI,		//add by zfq 2016.05.04,OKI打印机
	BT_SEIKO,	//add by zxl 2016.05.18,精工打印机
	BT_PANTUM,		//add by zxl 2016.05.24,奔图
	BT_JOLIMARK,	//add by zxl 2016.05.24,映美
	BT_GPRINTER,	//add by zxl 2016.05.24,佳博
	BT_PANASONIC,	//add by zxl 2016.05.24,松下
	BT_LENOVO,		//add by zxl 2016.05.24,联想
	BT_START,		//add by zxl 2016.05.24,实达
	BT_XPRINTER,	//add by zxl 2016.05.24,芯烨
	BT_SONY,		//add by zxl 2016.05.24,索尼
	BT_DELL			//add by zxl 2016.05.24,戴尔
};
class ICopyCmdInterface
{
public:
	virtual void OnInitSuc(){ASSERT(0);}
	virtual void OnCheckStateSuc(){ASSERT(0);}
	virtual void OnLockSuc(BOOL bLocked){ASSERT(0);}
	virtual void OnPowerSuc(BOOL bPowered){ASSERT(0);}
	
	virtual void OnPaperType(PAPER_TYPE){ASSERT(0);}
	virtual void OnPaperColor(PAPER_COLOR){ASSERT(0);}
	virtual void OnBeginOneCopy(){ASSERT(0);}
	virtual void OnOneCopyCmplt(){ASSERT(0);}
	virtual void OnOneCopyCmplt(PAPER_TYPE,PAPER_COLOR){ASSERT(0);}
	virtual void OnErr(int){ASSERT(0);}
};


class ICopyChargeInterface
{
public:
	virtual void OnOneCopyCmplt(void*){ASSERT(0);}
	virtual void OnReqCopyResponse(void *,int){ASSERT(0);}//add by szy 20170524
	virtual void OnAllCopyCmplt(void*){ASSERT(0);}
	virtual void OnErr(int){ASSERT(0);}
	virtual void OnIdle(int ){ASSERT(0);}
	virtual void OnTimeout(){ASSERT(0);}
	virtual void OnOneCopyBakSendCmplt(void*){ASSERT(0);} //add by lzj 20170329		
};