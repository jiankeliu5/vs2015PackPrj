// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CMediaBookmark ��װ��

class CMediaBookmark : public COleDispatchDriver
{
public:
	CMediaBookmark(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CMediaBookmark(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CMediaBookmark(const CMediaBookmark& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// MediaBookmark ����
public:
	long get_Index()
	{
		long result;
		InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_Name()
	{
		CString result;
		InvokeHelper(0x7d2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long get_Position()
	{
		long result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void Delete()
	{
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// MediaBookmark ����
public:

};
