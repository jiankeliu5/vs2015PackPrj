// �����Ϳ������á�����ࡱ�����ļ�������ɵ� IDispatch ��װ��

#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSPPT.OLB" no_namespace
// CSoundFormat ��װ��

class CSoundFormat : public COleDispatchDriver
{
public:
	CSoundFormat(){} // ���� COleDispatchDriver Ĭ�Ϲ��캯��
	CSoundFormat(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CSoundFormat(const CSoundFormat& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// ����
public:

	// ����
public:


	// SoundFormat ����
public:
	void Play()
	{
		InvokeHelper(0x7d0, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Import(LPCTSTR FileName)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, FileName);
	}
	long Export(LPCTSTR FileName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, FileName);
		return result;
	}
	long get_Type()
	{
		long result;
		InvokeHelper(0x7d3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_SourceFullName()
	{
		CString result;
		InvokeHelper(0x7d4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}

	// SoundFormat ����
public:

};
