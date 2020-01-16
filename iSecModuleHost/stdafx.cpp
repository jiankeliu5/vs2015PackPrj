// stdafx.cpp : 只包括标准包含文件的源文件
// iSecClientHost.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

HANDLE g_hMutexInstance = nullptr;
BOOL HasRun(CString szGlobalMutexName)
{
	g_hMutexInstance = CreateMutex(NULL, FALSE, szGlobalMutexName);  //创建互斥
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

