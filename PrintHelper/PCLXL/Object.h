#pragma once

#include "PCL_Def.h"

namespace ParsePCL
{//命名空间ParsePCL开始

//对象类型
enum ObjectType
{
	Null,
	Data,
	Operator,
	AttributeIdByte,
	AttributeIdUInt16,
	EmbeddedData,
	EmbeddedDataByte
};

//数据标签（类型）
enum DataTags
{
	UnKnow = 0x0,				//UnKnow
	UByteData = 0xc0,		//Unsigned 8-bit value
	UInt16Data = 0xc1,		//Unsigned 16-bit value
	UInt32Data = 0xc2,		//Unsigned 32-bit value
	SInt16Data = 0xc3,		//Signed 16-bit value
	SInt32Data = 0xc4,		//Signed 32-bit value
	Real32Data = 0xc5,		//Real number value
	/* 0xc6     */   
	/* 0xc7     */  
	UByteArray = 0xc8,		//Array of Unsigned 8-bit values
	UInt16Array = 0xc9,		//Array of Unsigned 16-bit values
	UInt32Array = 0xca,		//Array of Unsigned 32-bit values
	SInt16Array = 0xcb,		//Array of Signed 16-bit values
	SInt32Array = 0xcc,		//Array of Signed 32-bit values
	Real32Array = 0xcd,		//Array of Real number values
	/* 0xce     */  
	/* 0xcf     */  
	UByteXy = 0xd0,			//Two Unsigned 8-bit values
	UInt16Xy = 0xd1,		//Two Unsigned 16-bit values
	UInt32Xy = 0xd2,		//Two Unsigned 32-bit values
	SInt16Xy = 0xd3,		//Two Signed 16-bit values
	SInt32Xy = 0xd4,		//Two Signed 32-bit values
	Real32Xy = 0xd5,		//Two Real number values

	UByteBox = 0xe0,		//Four Unsigned 8-bit values
	UInt16Box = 0xe1,		//Four Unsigned 16-bit values
	UInt32Box = 0xe2,		//Four Unsigned 32-bit values
	SInt16Box = 0xe3,		//Four Signed 16-bit values
	SInt32Box = 0xe4,		//Four Signed 32-bit values
	Real32Box = 0xe5		//Four Real number values
};

enum DataPostion
{
	First,
	Second,
	Third,
	Fourth
};

union ObjectData
{
	UByte UByteData;
	UInt16 UInt16Data;
	UInt32 UInt32Data;
	SInt16 SInt16Data;
	SInt32 SInt32Data;
	Real32 Real32Data;
};

struct ArrayData
{
	DataTags oDataTags;		//数据的数据标签（类型）
	UInt32 nArrayLength;	//数组数据包含元素的个数
	UInt32 nDataLength;		//数组数据长度
	void* pArrayData;		//数组数据，可存储PCL_UByteArray，PCL_UInt16Array，PCL_UInt32Array，PCL_SInt16Array，PCL_SInt32Array，PCL_Real32Array六类数据

	ArrayData()
	{
		oDataTags = UnKnow;
		nArrayLength = 0;
		nDataLength = 0;
		pArrayData = NULL;
	}

	//拷贝构造函数(深度拷贝pArrayData数据)
	ArrayData(const ArrayData& obj)
	{
		this->oDataTags = obj.oDataTags;
		this->nArrayLength = obj.nArrayLength;
		this->nDataLength = obj.nDataLength;
		this->pArrayData = NULL;
		if (obj.pArrayData && (obj.nDataLength > 0))
		{
			this->pArrayData = malloc(obj.nDataLength);
			memcpy(this->pArrayData, obj.pArrayData, obj.nDataLength);
		}
	}

	//赋值构造函数(深度拷贝pArrayData数据)
	ArrayData& operator=(const ArrayData& obj)
	{
		if (this == &obj)
		{
			return *this;
		}

		this->oDataTags = obj.oDataTags;
		this->nArrayLength = obj.nArrayLength;
		this->nDataLength = obj.nDataLength;
		if (this->pArrayData)
		{
			free(this->pArrayData);
			this->pArrayData = NULL;
		}
		if (obj.pArrayData && (obj.nDataLength > 0))
		{
			this->pArrayData = malloc(obj.nDataLength);
			memcpy(this->pArrayData, obj.pArrayData, obj.nDataLength);
		}
		return *this;
	}

	~ArrayData()
	{
		if (pArrayData)
		{
			free(pArrayData);
		}
	}
};

class Object
{
public:
	//默认构造函数
	Object(void);

	//拷贝构造函数
	Object(const Object& obj);

	//赋值构造函数
	Object& operator=(const Object& obj);

	~Object(void);

public:
	//对象类型操作
	void SetObjectType(ObjectType oObjectType);
	ObjectType GetObjectType();

	//数据标签（类型）操作
	void SetDataTags(DataTags oDataTags);
	DataTags GetDataTags();

	//单个数据操作
	void SetData(UByte UByteData, DataPostion pos = First);
	void GetData(UByte& UByteData, DataPostion pos = First);
	void SetData(UInt16 UInt16Data, DataPostion pos = First);
	void GetData(UInt16& UInt16Data, DataPostion pos = First);
	void SetData(UInt32 UInt32Data, DataPostion pos = First);
	void GetData(UInt32& UInt32Data, DataPostion pos = First);
	void SetData(SInt16 SInt16Data, DataPostion pos = First);
	void GetData(SInt16& SInt16Data, DataPostion pos = First);
	void SetData(SInt32 SInt32Data, DataPostion pos = First);
	void GetData(SInt32& SInt32Data, DataPostion pos = First);
	void SetData(Real32 Real32Data, DataPostion pos = First);
	void GetData(Real32& Real32Data, DataPostion pos = First);

	//数组数据操作
	void SetArrayData(const ArrayData& oArrayData);
	void GetArrayData(ArrayData& oArrayData);

	//文件信息操作
	void SetFileInfo(FILE* pFile, __int64 nFilePos);
	void GetFileInfo(FILE** ppFile, __int64& nFilePos);

	//判断是否为数组数据对象
	bool IsArray();

	//判断是否为数据对象
	bool IsData();

	//判断是否为操作对象
	bool IsOperator();

	//判断是否为属性对象
	bool IsAttributeId();

	//判断是否为嵌入式数据对象
	bool IsEmbeddedData();

protected:
	ObjectType m_oObjectType;	//对象类型
	DataTags m_oDataTags;		//数据标签（类型）
	ObjectData m_oData;			//存储单数据，Xy数据的x，Box数据的第一个
	ObjectData m_oData2;		//Xy数据的y，Box数据的第二个
	ObjectData m_oData3;		//Box数据的第三个
	ObjectData m_oData4;		//Box数据的第四个
	ArrayData m_oArrayData;		//数据数据

	//对象的文件描述
	FILE* m_pFile;				//文件指针
	__int64 m_nFilePos;			//文件位置
};

}//命名空间ParsePCL结束
