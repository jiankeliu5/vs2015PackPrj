// iPrintableDocumentDeal.h : iPrintableDocumentDeal DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CiPrintableDocumentDealApp
// �йش���ʵ�ֵ���Ϣ������� iPrintableDocumentDeal.cpp
//

class CiPrintableDocumentDealApp : public CWinApp
{
public:
	CiPrintableDocumentDealApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance(); // return app exit code

	DECLARE_MESSAGE_MAP()
};
