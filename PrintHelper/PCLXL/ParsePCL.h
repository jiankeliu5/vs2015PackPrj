#pragma once

#include "PCL_Def.h"
#include "Object.h"

namespace ParsePCL
{//命名空间ParsePCL开始

class CParsePCL
{
public:
	CParsePCL(void);
	~CParsePCL(void);

	bool OpenFile(const TCHAR* pcszPclFilePath);
	void CloseFile();
	__int64 GetFileLength();	//获取文件大小
	__int64 GetPclPostion();	//获取PCL数据的起始位置
	std::string GetJobName();	//utf-8编码
	std::string GetUserName();	//utf-8编码
	bool GetPageCount(int& nPageCount, int& nA3PageCount, int& nA4PageCount, int& nOtherPageCount);
	bool GetCopies(int& nCopies);
	bool GetCollate(int& nCollate);
	bool GetDuplex(int& nDuplex);
	bool GetColor(int& nColor);
	bool GetPaper(int& nPaper);
	bool GetOrientation(int& nOrientation);

	bool GetPageDuplexInfo(std::list<Object>& oPageDuplexObjectList);

	bool Parse();

protected:
	/******* 解析PJL相关接口，begin *******/
	//解析PJL指令
	bool ParsePJL();
	
	//读取一行数据
	bool ReadLine();

	//获取PCLXL协议信息
	bool ReadProtocolInfo();

	//删除PJL字符串的前后多余的内容
	void TrimPJLString(std::string& strString);

	//获取作业名
	bool GetJobNameByPJL();

	//获取用户名
	bool GetUserNameByPJL();

	//获取打印份数
	int GetCopiesByPJL();

	//获取双面打印开关参数
	int GetDuplexByPJL();

	//获取BINDING参数
	int GetBindingByPJL();

	//获取打印内容方向参数
	int GetOrientationByPJL();

	//获取彩色打印参数
	int GetColorByPJL();
	/******* 解析PJL相关接口，end *******/


	/******* 解析PCL相关接口，begin *******/
	//解析PCL指令
	bool ParsePCL();

	//获取当前位置的指令
	bool GetCmd(UByte& ubCmd);

	//读取数值
	bool GetDataValue(UByte& nValue);
	bool GetDataValue(UInt16& nValue);
	bool GetDataValue(UInt32& nValue);
	bool GetDataValue(SInt16& nValue);
	bool GetDataValue(SInt32& nValue);
	bool GetDataValue(Real32& nValue);

	//将文件位置从当前位置跳过指定长度(nSkipLength)，即后移
	bool SkipData(__int64 nSkipLength);

	//读取文件从当前位置往后的指定长度(nDataLength)的数据
	bool ReadData(UByte* pData, size_t nDataLength);

	//命令的属性判断
	bool IsDataTag(UByte ubCmd);
	bool IsAttributeId(UByte ubCmd);
	bool IsOperator(UByte ubCmd);
	bool IsEmbeddedData(UByte ubCmd);

	bool GetData(UByte ubCmd);
	bool GetAttribute(UByte ubCmd);
	bool GetOperator(UByte ubCmd);
	bool GetEmbeddedData(UByte ubCmd);

	//获取数组数据的信息
	bool GetArrayDataInfo(UByte ubCmd, UByte& ubDataType, UInt32& nDataElements, UInt32& nDataLength);

	//判断系统的是否为大端字节序
	bool IsBigEndianSystem();

	//清空解析PCL时，保存的临时对象栈
	void ClearParseStack();

	//实现部分Operator
	void BeginSession();
	void EndSession();
	void BeginPage();
	void EndPage();
	void SetColorSpace();
	/******* 解析PCL相关接口，end *******/

protected:
	TCHAR m_szPclFilePath[300];
	__int64 m_nFileLength;
	__int64 m_nFilePos;
	__int64 m_nPclPos;
	FILE* m_pFile;
	char* m_pDataBufer;
	int m_nDataBuferLength;

	//PCLXL协议信息
	char m_szProtocol[32];
	int m_nMainProtocol;
	int m_nSubProtocol;

	//标识系统是否为大端字节序
	bool m_bIsBigEndianSystem;

	//作业信息
	bool m_bHaveGetJobName;
	std::string m_strJobName;	//作业名，utf-8编码
	bool m_bHaveGetUserName;
	std::string m_strUserName;	//用户名，utf-8编码
	bool m_bHaveGetPageCount;
	int m_nPageCount;	//内容页数
	int m_nA3PageCount;
	int m_nA4PageCount;
	int m_nOtherPageCount;
	bool m_bHaveGetCopies;
	int m_nCopies;		//打印份数
	bool m_bHaveGetCollate;
	int m_nCollate;		//逐份打印，0-DMCOLLATE_FALSE（非逐份打印），1-DMCOLLATE_TRUE（逐份打印）
	bool m_bHaveGetOrientation;
	enum ORIENTATION { PORTRAIT, LANDSCAPE };	//纸张内容方向，PORTRAIT-竖向，LANDSCAPE-横向
	int m_oOrientation;	//纸张内容的方向
	bool m_bHaveGetBinding;	//双面的长边或者短边
	enum BINDING { LONGEDGE, SHORTEDGE, LEFT, RIGHT, TOP };	//长边：LONGEDGE；短边：SHORTEDGE，LEFT和RIGHT表示竖边，TOP表示横边
	BINDING m_oBinding;	//装订选项
	bool m_bHaveGetDuplex;
	bool m_bDuplex;		//双面开关
	bool m_bHaveGetPageMode;
	enum PageMode { Simplex, DuplexVertical, DuplexHorizontal };	//Simplex-单面，DuplexVertical-双面竖边，DuplexHorizontal-双面横边
	PageMode m_oPageMode;	//页面打印模式
	bool m_bHaveGetColor;
	int m_nColor;		//彩色打印：1-DMCOLOR_MONOCHROME, 2-DMCOLOR_COLOR
	bool m_bHaveGetPaper;
	int m_nPaper;		//纸型：8-DMPAPER_A3，9-DMPAPER_A4


	//解析PCL时，保存的临时对象栈
	std::stack<Object> m_oParseStack;

	//保存每页的双面打印参数配置，特别说明：不是每页都有双面打印参数
	std::list<Object> m_oPageDuplexObjectList;
};

}//命名空间ParsePCL结束
