// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

// #import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CSlides ��װ��

class CSlides : public COleDispatchDriver
{
public:
	CSlides(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CSlides(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CSlides(const CSlides& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Slides ����
public:
	LPUNKNOWN get__NewEnum()
	{
		LPUNKNOWN result;
		InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	VARIANT _Index(long Index)
	{
		VARIANT result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, Index);
		return result;
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
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
	LPDISPATCH Item(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPDISPATCH FindBySlideID(long SlideID)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d3, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, SlideID);
		return result;
	}
	LPDISPATCH Add(long Index, long Layout)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d4, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Index, Layout);
		return result;
	}
	long InsertFromFile(LPCTSTR FileName, long Index, long SlideStart, long SlideEnd)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x7d5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, FileName, Index, SlideStart, SlideEnd);
		return result;
	}
	LPDISPATCH Range(VARIANT& Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x7d6, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, &Index);
		return result;
	}
	LPDISPATCH Paste(long Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7d7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Index);
		return result;
	}
	LPDISPATCH AddSlide(long Index, LPDISPATCH pCustomLayout)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I4 VTS_DISPATCH ;
		InvokeHelper(0x7d8, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Index, pCustomLayout);
		return result;
	}

	// Slides ����
public:

};
