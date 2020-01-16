// EmbedWebSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "Install.h"
#include "EmbedWebSheet.h"


// CEmbedWebSheet

IMPLEMENT_DYNAMIC(CEmbedWebSheet, CPropertySheet)

CEmbedWebSheet::CEmbedWebSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

    AddPage(&page1);
    AddPage(&page2);
    m_bSuc = FALSE;

    m_psh.dwFlags &= ~(PSH_HASHELP);
    m_psh.dwFlags |= PSH_WIZARD;

    page1.m_psp.dwFlags &= ~(PSP_HASHELP);
    page2.m_psp.dwFlags &= ~(PSP_HASHELP);
}

CEmbedWebSheet::CEmbedWebSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(&page1);
    AddPage(&page2);
    m_bSuc = FALSE;

    m_psh.dwFlags &= ~(PSH_HASHELP);
    m_psh.dwFlags |= PSH_WIZARD;

    page1.m_psp.dwFlags &= ~(PSP_HASHELP);
    page2.m_psp.dwFlags &= ~(PSP_HASHELP);
}

CEmbedWebSheet::CEmbedWebSheet( UINT iSelectPage ,LPCTSTR pszCaption )
:CPropertySheet(pszCaption, NULL, 0)
{
    m_bSuc = FALSE;
    if (iSelectPage == 0)
    {
        AddPage(&page1);
        page1.m_psp.dwFlags &= ~(PSP_HASHELP);
    }
    else if (iSelectPage == 1)
    {
        AddPage(&page2);
        page2.m_psp.dwFlags &= ~(PSP_HASHELP);
    }
    else if (iSelectPage == 3)
    {
        AddPage(&page1);
        AddPage(&page2);
        page1.m_psp.dwFlags &= ~(PSP_HASHELP);
        page2.m_psp.dwFlags &= ~(PSP_HASHELP);
    }
    //AddPage(&page2);

    m_psh.dwFlags &= ~(PSH_HASHELP);
    m_psh.dwFlags |= PSH_WIZARD;
}
CEmbedWebSheet::~CEmbedWebSheet()
{
}

BEGIN_MESSAGE_MAP(CEmbedWebSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEmbedWebSheet 消息处理程序
