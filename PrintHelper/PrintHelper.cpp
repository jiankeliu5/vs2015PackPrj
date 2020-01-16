// PrintHelper.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "PrintHelper.h"

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

// CPrintHelperApp

BEGIN_MESSAGE_MAP(CPrintHelperApp, CWinApp)
END_MESSAGE_MAP()


// CPrintHelperApp ����

CPrintHelperApp::CPrintHelperApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


CFileLog theLog;
CIniFile g_oIniFile;	//add by zxl,20150825,����Ŀ����config.ini�ļ�ͳһʹ�ñ�����

// Ψһ��һ�� CPrintHelperApp ����
CPrintHelperApp theApp;		//theApp������λ��Ҫ��������ȫ�ֱ���������棬
							//��ΪtheApp�ڳ�ʼ��ʱ�����ܻ�ʹ�õ�����ȫ�ֱ���,
							//��ʱ��������ܱ���

// CPrintHelperApp ��ʼ��

BOOL CPrintHelperApp::InitInstance()
{
	CWinApp::InitInstance();

	CFileLog::EnableLog(CCommonFun::IsDebug());

	theLog.SetLogName(_T("prthelper"));
	g_oIniFile.m_sPath.Format(_T("%s%s"), CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);

	return TRUE;
}

extern "C" __declspec(dllexport) void WINAPI  SetPrintHelperBindPrinter(const TCHAR* pszBindPrinter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString szBindPrinter(pszBindPrinter);
	if (szBindPrinter.GetLength() > 0)
	{
		g_oIniFile.m_sPath.Format(_T("%s%s\\config_%s.ini"),CCommonFun::GetDefaultPath(),SYS_CONFIG_DIR,szBindPrinter);
		theLog.SetLogName(_T("prthelper_") + szBindPrinter);
	}
	else
	{
		theLog.SetLogName(_T("prthelper"));
		g_oIniFile.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
	}
}
