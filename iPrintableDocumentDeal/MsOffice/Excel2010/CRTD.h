// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
// CRTD ��װ��

class CRTD : public COleDispatchDriver
{
public:
	CRTD(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CRTD(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CRTD(const CRTD& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// RTD ����
public:
	long get_ThrottleInterval()
	{
		long result;
		InvokeHelper(0x8c0, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ThrottleInterval(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8c0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void RefreshData()
	{
		InvokeHelper(0x8c1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void RestartServers()
	{
		InvokeHelper(0x8c2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// RTD ����
public:

};
