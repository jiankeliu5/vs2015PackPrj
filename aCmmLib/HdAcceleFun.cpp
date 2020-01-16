// HdAcceleFun.cpp: implementation of the CHdAcceleFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HdAcceleFun.h"
#include <winsvc.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHdAcceleFun::CHdAcceleFun()
{

}

CHdAcceleFun::~CHdAcceleFun()
{

}

BOOL CHdAcceleFun::CloseAccele()
{
	return SetHdAccele(5);
}

BOOL CHdAcceleFun::OpenAccele()
{
	return SetHdAccele(0);
}

BOOL CHdAcceleFun::GetHdAccelleLevel(DWORD& dwLevel)
{
	TCHAR Video0Name[200] = {0};
	DISPLAY_DEVICE dd;
	ZeroMemory(&dd,sizeof(dd));
	dd.cb = sizeof(dd);
	
	for(int i = 0; EnumDisplayDevices(NULL,i,&dd,0); i++)
	{
		//获取主显示设备
		if((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0)
		{
			_tcscpy(Video0Name,dd.DeviceKey);
			TRACE("%s\n",Video0Name);
			break;
		}
	}
	
	HKEY hk = NULL;
	// manual search
	if(Video0Name[0] == 0)
	{
		TRACE("自己遍历\n");
		RegOpenKeyA(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\VIDEO",&hk);
		if(hk != 0)
		{
			DWORD pdn = sizeof(Video0Name);
			for(int i = 0; i < 10; i++)
			{
				DWORD pt;
				TCHAR buf[128] = {0};
				pdn = sizeof(Video0Name);
				_stprintf(buf, _T("\\Device\\Video%d"),i);
				LONG lRet = RegQueryValueEx(hk,buf,0, &pt,(LPBYTE)Video0Name,&pdn);
				/* Make sure that it isn't a pseudo-device, e.g. smsmdd */
				if(_tcsstr(Video0Name, _T("\\Device")) == 0 && _tcsstr(Video0Name, _T("VgaSave")) == 0 &&
					startswith(Video0Name,_T("\\Registry\\Machine\\")))
					break;
				else
					Video0Name[0] = 0;
			}
			
			if(Video0Name[0] == 0)
				return FALSE;
			else
				TRACE("Selected video %s\n",Video0Name);
		}
		RegCloseKey(hk);
	}
	//设置
	hk = NULL;
	//默认值为0
	dwLevel = 0;
	RegOpenKey(HKEY_LOCAL_MACHINE,Video0Name+_tcslen(_T("\\Registry\\Machine\\")),&hk);
	if(hk != 0)
	{
		DWORD pdn = sizeof(DWORD);
		DWORD pt;
		RegQueryValueExA(hk,"Acceleration.Level",0,&pt,(LPBYTE)&dwLevel,&pdn);
		return TRUE;
	}
	return FALSE;
}
bool CHdAcceleFun::startswith(LPCTSTR src, LPCTSTR prefix)
{
    return _tcsncmp(src, prefix, _tcslen(prefix)) == 0;
}

bool CHdAcceleFun::startsiwith(LPCTSTR src, LPCTSTR prefix)
{
    return _tcsncmp(src, prefix, _tcslen(prefix)) == 0;
}

BOOL CHdAcceleFun::SetHdAccele(DWORD dwLevel)
{
	ASSERT(dwLevel >= 0 && dwLevel <= 5);
	TCHAR Video0Name[200] = {0};
	DISPLAY_DEVICE dd;
	ZeroMemory(&dd,sizeof(dd));
	dd.cb = sizeof(dd);

	for(int i = 0; EnumDisplayDevices(NULL,i,&dd,0); i++)
	{
		//获取主显示设备
		if((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0)
		{
			_tcscpy(Video0Name,dd.DeviceKey);
			TRACE("%s\n",Video0Name);
			break;
		}
	}
	
	HKEY hk = NULL;
	// manual search
	if(Video0Name[0] == 0)
	{

		TRACE("自己遍历\n");
		RegOpenKey(HKEY_LOCAL_MACHINE,_T("HARDWARE\\DEVICEMAP\\VIDEO"),&hk);
		if(hk != 0)
		{
			DWORD pdn = sizeof(Video0Name);
			for(int i = 0; i < 10; i++)
			{
				DWORD pt;
				char buf[128] = {0};
				pdn = sizeof(Video0Name);
				sprintf(buf,"\\Device\\Video%d",i);
				LONG lRet = RegQueryValueExA(hk,buf,0, &pt,(LPBYTE)Video0Name,&pdn);
				/* Make sure that it isn't a pseudo-device, e.g. smsmdd */
				if(_tcsstr(Video0Name, _T("\\Device")) == 0 && _tcsstr(Video0Name, _T("VgaSave")) == 0 &&
					startswith(Video0Name,_T("\\Registry\\Machine\\")))
					break;
				else
					Video0Name[0] = 0;
			}
			
			if(Video0Name[0] == 0)
				return FALSE;
		}
		RegCloseKey(hk);
	}
	//设置
	hk = NULL;
    DWORD level = 0;
	BOOL bNeedChange = FALSE;
	TRACE(_T("Video0 Device is %s\n"),Video0Name);
	RegOpenKey(HKEY_LOCAL_MACHINE,Video0Name+_tcslen(_T("\\Registry\\Machine\\")),&hk);
	if(hk != 0)
	{
		DWORD pdn = sizeof(DWORD);
		DWORD pt;
		if(RegQueryValueEx(hk,_T("Acceleration.Level"),0,&pt,(LPBYTE)&level,&pdn) != ERROR_SUCCESS)
			pt = REG_DWORD;
		if(level != dwLevel)
		{
			bNeedChange = TRUE;
			RegSetValueEx(hk,_T("Acceleration.Level"),0,pt, (LPBYTE)&dwLevel,sizeof(dwLevel));
		}
		RegCloseKey(hk);
	}
	else
		return FALSE;
	
	//通知
	if (bNeedChange)
	{
		ChangeDisplaySettings(NULL,0);
	}
	return TRUE;
}

BOOL CHdAcceleFun::IsOpenedAccele()
{
	DWORD dwLevel = 0;
	GetHdAccelleLevel(dwLevel);
	return dwLevel == 5 ? FALSE : TRUE;
}