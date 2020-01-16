#pragma once

#include "resource.h"

class CSvcHost : public CService
{
public:
	CSvcHost(TCHAR* szName);
	~CSvcHost(void);
public:
	void OnStart();
	void OnStop();
	void OnShutdown();

	virtual BOOL OnServiceInstalled(SC_HANDLE svc);
	virtual BOOL OnServiceUninstalled(SC_HANDLE svc);

protected:

private:
	void Initial();
	void Release();
	static void CALLBACK TimerProc(HWND hwnd,
		UINT uMsg,
		UINT_PTR idEvent,
		DWORD dwTime
	);
private:
	HMODULE m_hModule = NULL;
	CAutoRunMgr m_oAutoMgr;
	ISvcCallback* m_pSvcCallback  = nullptr;

};
