// PrintHelper.h : PrintHelper DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPrintHelperApp
// �йش���ʵ�ֵ���Ϣ������� PrintHelper.cpp
//

class CPrintHelperApp : public CWinApp
{
public:
	CPrintHelperApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
