#include "StdAfx.h"
#include "Object.h"

// using namespace ParsePCL;
namespace ParsePCL
{//命名空间ParsePCL开始

Object::Object(void)
{
	m_oObjectType = Null;
	m_oDataTags = UnKnow;
	m_oData.UInt32Data = 0;
	m_oData2.UInt32Data = 0;
	m_oData3.UInt32Data = 0;
	m_oData4.UInt32Data = 0;
	m_pFile = NULL;
	m_nFilePos = 0;
}

Object::Object(const Object& obj)
{
	this->m_oObjectType = obj.m_oObjectType;
	this->m_oDataTags = obj.m_oDataTags;

	this->m_oData.UInt32Data = obj.m_oData.UInt32Data;
	this->m_oData2.UInt32Data = obj.m_oData2.UInt32Data;
	this->m_oData3.UInt32Data = obj.m_oData3.UInt32Data;
	this->m_oData4.UInt32Data = obj.m_oData4.UInt32Data;

	this->m_oArrayData = obj.m_oArrayData;

	this->m_pFile = obj.m_pFile;
	this->m_nFilePos = obj.m_nFilePos;
}

Object& Object::operator=(const Object& obj)
{
	if (this == &obj)
	{
		return *this;
	}

	this->m_oObjectType = obj.m_oObjectType;
	this->m_oDataTags = obj.m_oDataTags;

	this->m_oData.UInt32Data = obj.m_oData.UInt32Data;
	this->m_oData2.UInt32Data = obj.m_oData2.UInt32Data;
	this->m_oData3.UInt32Data = obj.m_oData3.UInt32Data;
	this->m_oData4.UInt32Data = obj.m_oData4.UInt32Data;

	this->m_oArrayData = obj.m_oArrayData;

	this->m_pFile = obj.m_pFile;
	this->m_nFilePos = obj.m_nFilePos;

	return *this;
}

Object::~Object(void)
{
}

void Object::SetObjectType(ObjectType oObjectType)
{
	m_oObjectType = oObjectType;
}

ObjectType Object::GetObjectType()
{
	return m_oObjectType;
}

void Object::SetDataTags(DataTags oDataTags)
{
	m_oDataTags = oDataTags;
}

DataTags Object::GetDataTags()
{
	return m_oDataTags;
}

//单个数据操作
void Object::SetData(UByte UByteData, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.UByteData = UByteData;
		break;
	case Second:
		m_oData2.UByteData = UByteData;
		break;
	case Third:
		m_oData3.UByteData = UByteData;
		break;
	case Fourth:
		m_oData4.UByteData = UByteData;
		break;
	default:
		assert(false);
		m_oData.UByteData = UByteData;
		break;
	}
}

void Object::GetData(UByte& UByteData, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		UByteData = m_oData.UByteData;
		break;
	case Second:
		UByteData = m_oData2.UByteData;
		break;
	case Third:
		UByteData = m_oData3.UByteData;
		break;
	case Fourth:
		UByteData = m_oData4.UByteData;
		break;
	default:
		assert(false);
		UByteData = m_oData.UByteData;
		break;
	}
}

void Object::SetData(UInt16 UInt16Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.UInt16Data = UInt16Data;
		break;
	case Second:
		m_oData2.UInt16Data = UInt16Data;
		break;
	case Third:
		m_oData3.UInt16Data = UInt16Data;
		break;
	case Fourth:
		m_oData4.UInt16Data = UInt16Data;
		break;
	default:
		assert(false);
		m_oData.UInt16Data = UInt16Data;
		break;
	}
}

void Object::GetData(UInt16& UInt16Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		UInt16Data = m_oData.UInt16Data;
		break;
	case Second:
		UInt16Data = m_oData2.UInt16Data;
		break;
	case Third:
		UInt16Data = m_oData3.UInt16Data;
		break;
	case Fourth:
		UInt16Data = m_oData4.UInt16Data;
		break;
	default:
		assert(false);
		UInt16Data = m_oData.UInt16Data;
		break;
	}
}

void Object::SetData(UInt32 UInt32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.UInt32Data = UInt32Data;
		break;
	case Second:
		m_oData2.UInt32Data = UInt32Data;
		break;
	case Third:
		m_oData3.UInt32Data = UInt32Data;
		break;
	case Fourth:
		m_oData4.UInt32Data = UInt32Data;
		break;
	default:
		assert(false);
		m_oData.UInt32Data = UInt32Data;
		break;
	}
}

void Object::GetData(UInt32& UInt32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		UInt32Data = m_oData.UInt32Data;
		break;
	case Second:
		UInt32Data = m_oData2.UInt32Data;
		break;
	case Third:
		UInt32Data = m_oData3.UInt32Data;
		break;
	case Fourth:
		UInt32Data = m_oData4.UInt32Data;
		break;
	default:
		assert(false);
		UInt32Data = m_oData.UInt32Data;
		break;
	}
}

void Object::SetData(SInt16 SInt16Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.SInt16Data = SInt16Data;
		break;
	case Second:
		m_oData2.SInt16Data = SInt16Data;
		break;
	case Third:
		m_oData3.SInt16Data = SInt16Data;
		break;
	case Fourth:
		m_oData4.SInt16Data = SInt16Data;
		break;
	default:
		assert(false);
		m_oData.SInt16Data = SInt16Data;
		break;
	}
}

void Object::GetData(SInt16& SInt16Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		SInt16Data = m_oData.SInt16Data;
		break;
	case Second:
		SInt16Data = m_oData2.SInt16Data;
		break;
	case Third:
		SInt16Data = m_oData3.SInt16Data;
		break;
	case Fourth:
		SInt16Data = m_oData4.SInt16Data;
		break;
	default:
		assert(false);
		SInt16Data = m_oData.SInt16Data;
		break;
	}
}

void Object::SetData(SInt32 SInt32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.SInt32Data = SInt32Data;
		break;
	case Second:
		m_oData2.SInt32Data = SInt32Data;
		break;
	case Third:
		m_oData3.SInt32Data = SInt32Data;
		break;
	case Fourth:
		m_oData4.SInt32Data = SInt32Data;
		break;
	default:
		assert(false);
		m_oData.SInt32Data = SInt32Data;
		break;
	}
}

void Object::GetData(SInt32& SInt32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		SInt32Data = m_oData.SInt32Data;
		break;
	case Second:
		SInt32Data = m_oData2.SInt32Data;
		break;
	case Third:
		SInt32Data = m_oData3.SInt32Data;
		break;
	case Fourth:
		SInt32Data = m_oData4.SInt32Data;
		break;
	default:
		assert(false);
		SInt32Data = m_oData.SInt32Data;
		break;
	}
}

void Object::SetData(Real32 Real32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		m_oData.Real32Data = Real32Data;
		break;
	case Second:
		m_oData2.Real32Data = Real32Data;
		break;
	case Third:
		m_oData3.Real32Data = Real32Data;
		break;
	case Fourth:
		m_oData4.Real32Data = Real32Data;
		break;
	default:
		assert(false);
		m_oData.Real32Data = Real32Data;
		break;
	}
}

void Object::GetData(Real32& Real32Data, DataPostion pos /*= First*/)
{
	switch (pos)
	{
	case First:
		Real32Data = m_oData.Real32Data;
		break;
	case Second:
		Real32Data = m_oData2.Real32Data;
		break;
	case Third:
		Real32Data = m_oData3.Real32Data;
		break;
	case Fourth:
		Real32Data = m_oData4.Real32Data;
		break;
	default:
		assert(false);
		Real32Data = m_oData.Real32Data;
		break;
	}
}

//数组数据操作
void Object::SetArrayData(const ArrayData& oArrayData)
{
	m_oArrayData = oArrayData;
}

void Object::GetArrayData(ArrayData& oArrayData)
{
	oArrayData = m_oArrayData;
}

//文件信息操作
void Object::SetFileInfo(FILE* pFile, __int64 nFilePos)
{
	m_pFile = pFile;
	m_nFilePos = nFilePos;
}

void Object::GetFileInfo(FILE** ppFile, __int64& nFilePos)
{
	if (ppFile)
	{
		*ppFile = m_pFile;
	}
	nFilePos = m_nFilePos;
}

//判断是否为数组数据对象
bool Object::IsArray()
{
	if ((m_oObjectType == Data) && (m_oDataTags >= UByteArray) && (m_oDataTags <= Real32Array))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判断是否为数据对象
bool Object::IsData()
{
	if (m_oObjectType == Data)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判断是否为操作对象
bool Object::IsOperator()
{
	if (m_oObjectType == Operator)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判断是否为属性对象
bool Object::IsAttributeId()
{
	if ((m_oObjectType == AttributeIdByte) || (m_oObjectType == AttributeIdUInt16))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判断是否为嵌入式数据对象
bool Object::IsEmbeddedData()
{
	if ((m_oObjectType == EmbeddedData) || (m_oObjectType == EmbeddedDataByte))
	{
		return true;
	}
	else
	{
		return false;
	}
}

}//命名空间ParsePCL结束
