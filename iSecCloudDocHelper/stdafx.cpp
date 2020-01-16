#include "stdafx.h"

HANDLE g_hMutexInstance = NULL;
BOOL RunOnce(CString szGlobalMutexName)
{
	g_hMutexInstance = CreateMutex(NULL, FALSE, szGlobalMutexName);  //��������
	if(NULL == g_hMutexInstance)
	{
		return FALSE;
	}

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(g_hMutexInstance);
		return FALSE;
	}
	return TRUE;
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
