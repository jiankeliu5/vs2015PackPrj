// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CTableStyle ��װ��

class CTableStyle : public COleDispatchDriver
{
public:
	CTableStyle(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CTableStyle(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTableStyle(const CTableStyle& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// TableStyle ����
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
	CString get__Default()
	{
		CString result;
		InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString get_Name()
	{
		CString result;
		InvokeHelper(0x6e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString get_NameLocal()
	{
		CString result;
		InvokeHelper(0x3a9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	BOOL get_BuiltIn()
	{
		BOOL result;
		InvokeHelper(0x229, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_TableStyleElements()
	{
		LPDISPATCH result;
		InvokeHelper(0xab1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	BOOL get_ShowAsAvailableTableStyle()
	{
		BOOL result;
		InvokeHelper(0xab2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ShowAsAvailableTableStyle(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xab2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ShowAsAvailablePivotTableStyle()
	{
		BOOL result;
		InvokeHelper(0xab3, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ShowAsAvailablePivotTableStyle(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xab3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Delete()
	{
		InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	LPDISPATCH Duplicate(VARIANT& NewTableStyleName)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x40f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &NewTableStyleName);
		return result;
	}
	BOOL get_ShowAsAvailableSlicerStyle()
	{
		BOOL result;
		InvokeHelper(0xb82, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ShowAsAvailableSlicerStyle(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xb82, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}

	// TableStyle ����
public:

};
