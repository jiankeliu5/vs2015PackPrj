// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CPublishObject ��װ��

class CPublishObject : public COleDispatchDriver
{
public:
	CPublishObject(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CPublishObject(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPublishObject(const CPublishObject& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// PublishObject ����
public:
	LPDISPATCH get_Application()
	{
		LPDISPATCH result;
		InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Parent()
	{
		LPDISPATCH result;
		InvokeHelper(0x7d2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	long get_HTMLVersion()
	{
		long result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_HTMLVersion(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_SourceType()
	{
		long result;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_SourceType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_RangeStart()
	{
		long result;
		InvokeHelper(0x7d5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RangeStart(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_RangeEnd()
	{
		long result;
		InvokeHelper(0x7d6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_RangeEnd(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_SlideShowName()
	{
		CString result;
		InvokeHelper(0x7d7, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_SlideShowName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_SpeakerNotes()
	{
		long result;
		InvokeHelper(0x7d8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_SpeakerNotes(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_FileName()
	{
		CString result;
		InvokeHelper(0x7d9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_FileName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Publish()
	{
		InvokeHelper(0x7da, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// PublishObject ����
public:

};
