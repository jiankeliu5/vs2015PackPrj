// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CSparklineGroups ��װ��

class CSparklineGroups : public COleDispatchDriver
{
public:
	CSparklineGroups(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CSparklineGroups(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CSparklineGroups(const CSparklineGroups& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// SparklineGroups ����
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
	LPDISPATCH Add(long Type, LPCTSTR SourceData)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0xb5, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Type, SourceData);
		return result;
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0xaa, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPUNKNOWN get__NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get__Default(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	void Clear()
	{
		InvokeHelper(0x6f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ClearGroups()
	{
		InvokeHelper(0xb83, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Group(LPDISPATCH Location)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(0x2e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Location);
	}
	void Ungroup()
	{
		InvokeHelper(0xf4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// SparklineGroups ����
public:

};
