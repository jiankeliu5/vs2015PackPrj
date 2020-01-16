// Global.h: interface for the CRscObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GLOBAL_)
#define _GLOBAL_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#ifdef  _AA_DLL_
	#define _AA_DLL_EXPORT_		__declspec(dllexport)
	#define _AA_DLL_EXPORT_C_ 	extern "C" __declspec(dllexport)
#else
	#define _AA_DLL_EXPORT_		__declspec(dllimport)
	#define _AA_DLL_EXPORT_C_	extern "C" __declspec(dllimport)
#endif

#define PRODUCT_REGISTY_KEY				_T("SOFTWARE\\iSecStar")			//注册表键值
#define PROJECT_REGISTY_KEY				_T("SOFTWARE\\iSecStar")		//注册表键值
#define PROJECT_PATH					_T("\\iSecPrint")

#ifndef CP_GB2312
#define  CP_GB2312	936	//Code Page Identifiers ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
#endif

#ifdef UNICODE
#ifndef __Argv
#define __Argv __wargv
#endif // !__Argv
#else
#ifndef __Argv
#define __Argv __argv
#endif // !__Argv
#endif // UNICODE

#ifdef UNICODE
#ifndef stdtstring
#define stdtstring std::wstring
#endif // !stdtstring
#else
#ifndef stdtstring
#define stdtstring std::string
#endif // !stdtstring
#endif // UNICODE

#define REMOTE_START_PORT			7010	//远程开机端口
#define POLICY_TCP_PORT				5020
#define DBSVR_TCP_PORT				5021	//数据服务器


#ifndef __STR1__
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : "
#endif

#pragma warning(disable : 4996)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4200)


//#define XAB_TEST_MODE

//系统的子版本
#define ISEC_EDU	0x1
#define ISEC_STD	0x2
#define ISEC_ENT	0x4
#define ISEC_ADV	0x8
#define ISEC_ALL	0xF



//产品内部编号
#define ISEC_PRINT_CODE		_T("iSecPrint")
#define ISEC_INTRANET_CODE	_T("iSecIntranet")
#define SAFE_DELETE(x) if(x){delete x; x = NULL;}

//产品内部编码
#define ISEC_PRINT		0x100
#define ISEC_INTRANET	0x200
#define ISEC_CRPT		0x400

//打印系统
#define ISEC_PRINT_ALL		0x10f
#define ISEC_PRINT_EDU		0x101
#define ISEC_PRINT_STD		0x102
#define ISEC_PRINT_ENT		0x104
#define ISEC_PRINT_ADV		0x108

//内网管理
#define ISEC_INTRANET_ALL	0x207
#define ISEC_INTRANET_STD	0x201
#define ISEC_INTRANET_ENT	0x202
#define ISEC_INTRANET_ADV	0x204
//文档加密
#define ISEC_CRPT_ALL		0x407
#define ISEC_CRPT_STD		0x401
#define ISEC_CRPT_ENT		0x402
#define ISEC_CRPT_ADV		0x404


//终端类型
#define ISEC_PLT_CLT			1		//客户端
#define ISEC_PLT_MGR			2		//管理机
#define ISEC_PLT_SVR			3		//服务
#define ISEC_PLT_PRT			4		//打印站点
#define ISEC_PLT_TOOL			5		//工具
#define ISEC_PRT_CHARGE_TOOL	6		//充值工具
#define ISEC_PLT_WEB			7		//web服务
#define ISEC_PLT_ROUTER			8		//打印网关
#define ISEC_PLT_EMBEDWEB		9		//嵌入式web
#define ISEC_PLT_PrtCenterSvr	10		//打印集中管理端
#define ISEC_PLT_TPTOOL			11		//第三方登录服务组件
#define ISEC_PLT_SPLWORKER		12		//spl转换工具

//行为管理
#define ISEC_CLT_PRINT		MAKELONG(ISEC_PRINT,ISEC_PLT_CLT)
#define ISEC_CLT_INTRANET	MAKELONG(ISEC_INTRANET,ISEC_PLT_CLT)
#define ISEC_CLT_CRPT		MAKELONG(ISEC_CRPT,ISEC_PLT_CLT)

#define IsPrintProduct(dw)		(ISEC_PRINT&dw)
#define IsIntranetProduct(dw)	(ISEC_INTRANET&dw)

#ifndef _DEBUG
#define ISEC_ENABLE_ENCRYPT
//#define DEF_ISEC_VER_STD
#endif
#endif // !defined(_GLOBAL_)
