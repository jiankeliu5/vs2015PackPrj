// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CChartData ��װ��

class CChartData : public COleDispatchDriver
{
public:
	CChartData(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CChartData(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CChartData(const CChartData& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// ChartData ����
public:
	LPDISPATCH get_Workbook()
	{
		LPDISPATCH result;
		InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void Activate()
	{
		InvokeHelper(0x7d2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL get_IsLinked()
	{
		BOOL result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void BreakLink()
	{
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// ChartData ����
public:

};
