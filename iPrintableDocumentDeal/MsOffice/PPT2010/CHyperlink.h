// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CHyperlink ��װ��

class CHyperlink : public COleDispatchDriver
{
public:
	CHyperlink(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CHyperlink(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CHyperlink(const CHyperlink& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// Hyperlink ����
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
	long get_Type()
	{
		long result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_Address()
	{
		CString result;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Address(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_SubAddress()
	{
		CString result;
		InvokeHelper(0x7d5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_SubAddress(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void AddToFavorites()
	{
		InvokeHelper(0x7d6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	CString get_EmailSubject()
	{
		CString result;
		InvokeHelper(0x7d7, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_EmailSubject(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ScreenTip()
	{
		CString result;
		InvokeHelper(0x7d8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ScreenTip(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_TextToDisplay()
	{
		CString result;
		InvokeHelper(0x7d9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_TextToDisplay(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ShowAndReturn()
	{
		long result;
		InvokeHelper(0x7da, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ShowAndReturn(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7da, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Follow()
	{
		InvokeHelper(0x7db, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void CreateNewDocument(LPCTSTR FileName, long EditNow, long Overwrite)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x7dc, DISPATCH_METHOD, VT_EMPTY, NULL, parms, FileName, EditNow, Overwrite);
	}
	void Delete()
	{
		InvokeHelper(0x7dd, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// Hyperlink ����
public:

};
