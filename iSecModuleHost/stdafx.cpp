// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// iSecClientHost.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

HANDLE g_hMutexInstance = nullptr;
BOOL HasRun(CString szGlobalMutexName)
{
	g_hMutexInstance = CreateMutex(NULL, FALSE, szGlobalMutexName);  //��������
	if (NULL == g_hMutexInstance)
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(g_hMutexInstance);
		return TRUE;
	}
	return FALSE;
}

