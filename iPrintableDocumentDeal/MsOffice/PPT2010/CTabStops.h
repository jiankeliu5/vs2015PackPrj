// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CTabStops ��װ��

class CTabStops : public COleDispatchDriver
{
public:
	CTabStops(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CTabStops(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTabStops(const CTabStops& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// TabStops ����
public:
	LPUNKNOWN get__NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	VARIANT _Index(long Index)
	{
		VARIANT result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, Index);
		return result;
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
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
	LPDISPATCH Item(long Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Index);
		return result;
	}
	float get_DefaultSpacing()
	{
		float result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_DefaultSpacing(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH Add(long Type, float Position)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_R4 ;
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, Position);
		return result;
	}

	// TabStops ����
public:

};
