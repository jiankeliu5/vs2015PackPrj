// HookProc.h : HookProc DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
class CHookProcApp : public CWinApp
{
public:
	CHookProcApp();

// ��д
public:
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL InitInstance();
};

class CLockCS
{
public:
	CLockCS(CRITICAL_SECTION& cs)
	{
		m_cs= cs;
		EnterCriticalSection(&m_cs);
	}
	~CLockCS()
	{
		LeaveCriticalSection(&m_cs);
	}
	CRITICAL_SECTION m_cs;
};
#define HOOKTHREAD_COUNT  10

//////////////////////////////////////////////////////////////////////////
// ��������

BOOL __declspec(dllexport)StartHook(char*);									// ��������������
BOOL __declspec(dllexport)StopHook();									// �رշ���������
