// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

// #import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CPresentations ��װ��

class CPresentations : public COleDispatchDriver
{
public:
	CPresentations(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CPresentations(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPresentations(const CPresentations& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Presentations ����
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
	LPDISPATCH Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPDISPATCH Add(long WithWindow)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d3, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, WithWindow);
		return result;
	}
	LPDISPATCH OpenOld(LPCTSTR FileName, long ReadOnly, long Untitled, long WithWindow)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, FileName, ReadOnly, Untitled, WithWindow);
		return result;
	}
	LPDISPATCH Open(LPCTSTR FileName, long ReadOnly, long Untitled, long WithWindow)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d5, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, FileName, ReadOnly, Untitled, WithWindow);
		return result;
	}
	void CheckOut(LPCTSTR FileName)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d6, DISPATCH_METHOD, VT_EMPTY, NULL, parms, FileName);
	}
	BOOL CanCheckOut(LPCTSTR FileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName);
		return result;
	}
	LPDISPATCH Open2007(LPCTSTR FileName, long ReadOnly, long Untitled, long WithWindow, long OpenAndRepair)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d8, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, FileName, ReadOnly, Untitled, WithWindow, OpenAndRepair);
		return result;
	}

	// Presentations ����
public:

};
