// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CTableStyleElement ��װ��

class CTableStyleElement : public COleDispatchDriver
{
public:
	CTableStyleElement(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CTableStyleElement(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTableStyleElement(const CTableStyleElement& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// TableStyleElement ����
public:
	LPDISPATCH get_Application()
	{
		LPDISPATCH result;
		InvokeHelper(0x94, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	long get_Creator()
	{
		long result;
		InvokeHelper(0x95, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Parent()
	{
		LPDISPATCH result;
		InvokeHelper(0x96, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	BOOL get_HasFormat()
	{
		BOOL result;
		InvokeHelper(0xaaf, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Interior()
	{
		LPDISPATCH result;
		InvokeHelper(0x81, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Borders()
	{
		LPDISPATCH result;
		InvokeHelper(0x1b3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Font()
	{
		LPDISPATCH result;
		InvokeHelper(0x92, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	long get_StripeSize()
	{
		long result;
		InvokeHelper(0xab0, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_StripeSize(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xab0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Clear()
	{
		InvokeHelper(0x6f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// TableStyleElement ����
public:

};
