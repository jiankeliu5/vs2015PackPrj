// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CSelection0 ��װ��

class CSelection0 : public COleDispatchDriver
{
public:
	CSelection0(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CSelection0(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CSelection0(const CSelection0& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Selection ����
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
	void Cut()
	{
		InvokeHelper(0x7d3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Copy()
	{
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Delete()
	{
		InvokeHelper(0x7d5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Unselect()
	{
		InvokeHelper(0x7d6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long get_Type()
	{
		long result;
		InvokeHelper(0x7d7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_SlideRange()
	{
		LPDISPATCH result;
		InvokeHelper(0x7d8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_ShapeRange()
	{
		LPDISPATCH result;
		InvokeHelper(0x7d9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_TextRange()
	{
		LPDISPATCH result;
		InvokeHelper(0x7da, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_ChildShapeRange()
	{
		LPDISPATCH result;
		InvokeHelper(0x7db, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	BOOL get_HasChildShapeRange()
	{
		BOOL result;
		InvokeHelper(0x7dc, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_TextRange2()
	{
		LPDISPATCH result;
		InvokeHelper(0x7dd, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}

	// Selection ����
public:

};
