// log4zLib.h : log4zLib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Clog4zLibApp
// �йش���ʵ�ֵ���Ϣ������� log4zLib.cpp
//

class Clog4zLibApp : public CWinApp
{
public:
	Clog4zLibApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
