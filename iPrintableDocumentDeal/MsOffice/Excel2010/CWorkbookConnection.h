// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CWorkbookConnection ��װ��

class CWorkbookConnection : public COleDispatchDriver
{
public:
	CWorkbookConnection(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CWorkbookConnection(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CWorkbookConnection(const CWorkbookConnection& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// WorkbookConnection ����
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
	CString get_Name()
	{
		CString result;
		InvokeHelper(0x6e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Name(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Description()
	{
		CString result;
		InvokeHelper(0xda, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Description(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xda, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get__Default()
	{
		CString result;
		InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put__Default(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Type()
	{
		long result;
		InvokeHelper(0x6c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_OLEDBConnection()
	{
		LPDISPATCH result;
		InvokeHelper(0xa89, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_ODBCConnection()
	{
		LPDISPATCH result;
		InvokeHelper(0xa8a, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Ranges()
	{
		LPDISPATCH result;
		InvokeHelper(0xa8b, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void Delete()
	{
		InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Refresh()
	{
		InvokeHelper(0x589, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// WorkbookConnection ����
public:

};
