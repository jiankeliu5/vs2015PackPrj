// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CSetEffect ��װ��

class CSetEffect : public COleDispatchDriver
{
public:
	CSetEffect(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CSetEffect(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CSetEffect(const CSetEffect& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// SetEffect ����
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
	long get_Property()
	{
		long result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Property(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	VARIANT get_To()
	{
		VARIANT result;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
		return result;
	}
	void put_To(VARIANT& newValue)
	{
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, &newValue);
	}

	// SetEffect ����
public:

};
