// eSQLiteInf.h : eSQLiteInf DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CeSQLiteInfApp
// �йش���ʵ�ֵ���Ϣ������� eSQLiteInf.cpp
//

class CeSQLiteInfApp : public CWinApp
{
public:
	CeSQLiteInfApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
