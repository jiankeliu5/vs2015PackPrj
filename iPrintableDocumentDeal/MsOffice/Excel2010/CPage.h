// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CPage ��װ��

class CPage : public COleDispatchDriver
{
public:
	CPage(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CPage(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPage(const CPage& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Page ����
public:
	LPDISPATCH get_LeftHeader()
	{
		LPDISPATCH result;
		InvokeHelper(0x3fa, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_CenterHeader()
	{
		LPDISPATCH result;
		InvokeHelper(0x3f3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_RightHeader()
	{
		LPDISPATCH result;
		InvokeHelper(0x402, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_LeftFooter()
	{
		LPDISPATCH result;
		InvokeHelper(0x3f9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_CenterFooter()
	{
		LPDISPATCH result;
		InvokeHelper(0x3f2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_RightFooter()
	{
		LPDISPATCH result;
		InvokeHelper(0x401, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}

	// Page ����
public:

};
