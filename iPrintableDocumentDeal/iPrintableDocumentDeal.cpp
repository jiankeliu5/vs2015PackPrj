// iPrintableDocumentDeal.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "iPrintableDocumentDeal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CiPrintableDocumentDealApp

BEGIN_MESSAGE_MAP(CiPrintableDocumentDealApp, CWinApp)
END_MESSAGE_MAP()


// CiPrintableDocumentDealApp ����

CiPrintableDocumentDealApp::CiPrintableDocumentDealApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

BOOL IsDebug()
{
	LPCTSTR strPath = PROJECT_REGISTY_KEY;
	LPCTSTR strKey=_T("DebugLog");
	DWORD dwRet, dwSize=sizeof(DWORD), dwType=REG_DWORD;
	HKEY hKey;

	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		(LPCTSTR)strPath,
		0,
		KEY_READ,
		&hKey)!= ERROR_SUCCESS)
		return 0;

	if(::RegQueryValueEx(hKey,
		(LPCTSTR)strKey,
		NULL,
		&dwType,
		(LPBYTE)&dwRet,
		&dwSize) != ERROR_SUCCESS)
		dwRet = 0;

	RegCloseKey(hKey);

	return dwRet;
}

CFileLog theLog;

// Ψһ��һ�� CiPrintableDocumentDealApp ����
CiPrintableDocumentDealApp theApp;	//theApp������λ��Ҫ��������ȫ�ֱ���������棬
									//��ΪtheApp�ڳ�ʼ��ʱ�����ܻ�ʹ�õ�����ȫ�ֱ���,
									//��ʱ��������ܱ���

// CiPrintableDocumentDealApp ��ʼ��

BOOL CiPrintableDocumentDealApp::InitInstance()
{
	CWinApp::InitInstance();

	theLog.EnableLog(IsDebug());
	theLog.SetLogName(_T("iPrintableDocumentDeal"));

	CoInitialize(NULL);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	return TRUE;
}

int CiPrintableDocumentDealApp::ExitInstance()
{
	CoUninitialize();

	GdiplusShutdown(gdiplusToken);

	return CWinApp::InitInstance();
}
