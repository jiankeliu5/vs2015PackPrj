// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CAutoCorrect ��װ��

class CAutoCorrect : public COleDispatchDriver
{
public:
	CAutoCorrect(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CAutoCorrect(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CAutoCorrect(const CAutoCorrect& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// AutoCorrect ����
public:
	BOOL get_DisplayAutoCorrectOptions()
	{
		BOOL result;
		InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DisplayAutoCorrectOptions(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x7d1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_DisplayAutoLayoutOptions()
	{
		BOOL result;
		InvokeHelper(0x7d2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DisplayAutoLayoutOptions(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x7d2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}

	// AutoCorrect ����
public:

};
