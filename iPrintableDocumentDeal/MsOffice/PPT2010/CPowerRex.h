// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CPowerRex ��װ��

class CPowerRex : public COleDispatchDriver
{
public:
	CPowerRex(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CPowerRex(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPowerRex(const CPowerRex& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// _PowerRex ����
public:
	void OnAsfEncoderEvent(VARIANT& erorCode, VARIANT& bstrErrorDesc)
	{
		static BYTE parms[] = VTS_VARIANT VTS_VARIANT ;
		InvokeHelper(0x7d1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, &erorCode, &bstrErrorDesc);
	}

	// _PowerRex ����
public:

};
