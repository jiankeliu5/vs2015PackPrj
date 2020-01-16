#pragma once

#include "PCL_Def.h"

namespace ParsePCL
{//�����ռ�ParsePCL��ʼ

//��������
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

//���ݱ�ǩ�����ͣ�
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
	DataTags oDataTags;		//���ݵ����ݱ�ǩ�����ͣ�
	UInt32 nArrayLength;	//�������ݰ���Ԫ�صĸ���
	UInt32 nDataLength;		//�������ݳ���
	void* pArrayData;		//�������ݣ��ɴ洢PCL_UByteArray��PCL_UInt16Array��PCL_UInt32Array��PCL_SInt16Array��PCL_SInt32Array��PCL_Real32Array��������

	ArrayData()
	{
		oDataTags = UnKnow;
		nArrayLength = 0;
		nDataLength = 0;
		pArrayData = NULL;
	}

	//�������캯��(��ȿ���pArrayData����)
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

	//��ֵ���캯��(��ȿ���pArrayData����)
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
	//Ĭ�Ϲ��캯��
	Object(void);

	//�������캯��
	Object(const Object& obj);

	//��ֵ���캯��
	Object& operator=(const Object& obj);

	~Object(void);

public:
	//�������Ͳ���
	void SetObjectType(ObjectType oObjectType);
	ObjectType GetObjectType();

	//���ݱ�ǩ�����ͣ�����
	void SetDataTags(DataTags oDataTags);
	DataTags GetDataTags();

	//�������ݲ���
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

	//�������ݲ���
	void SetArrayData(const ArrayData& oArrayData);
	void GetArrayData(ArrayData& oArrayData);

	//�ļ���Ϣ����
	void SetFileInfo(FILE* pFile, __int64 nFilePos);
	void GetFileInfo(FILE** ppFile, __int64& nFilePos);

	//�ж��Ƿ�Ϊ�������ݶ���
	bool IsArray();

	//�ж��Ƿ�Ϊ���ݶ���
	bool IsData();

	//�ж��Ƿ�Ϊ��������
	bool IsOperator();

	//�ж��Ƿ�Ϊ���Զ���
	bool IsAttributeId();

	//�ж��Ƿ�ΪǶ��ʽ���ݶ���
	bool IsEmbeddedData();

protected:
	ObjectType m_oObjectType;	//��������
	DataTags m_oDataTags;		//���ݱ�ǩ�����ͣ�
	ObjectData m_oData;			//�洢�����ݣ�Xy���ݵ�x��Box���ݵĵ�һ��
	ObjectData m_oData2;		//Xy���ݵ�y��Box���ݵĵڶ���
	ObjectData m_oData3;		//Box���ݵĵ�����
	ObjectData m_oData4;		//Box���ݵĵ��ĸ�
	ArrayData m_oArrayData;		//��������

	//������ļ�����
	FILE* m_pFile;				//�ļ�ָ��
	__int64 m_nFilePos;			//�ļ�λ��
};

}//�����ռ�ParsePCL����
