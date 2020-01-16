// SecurityTool.h: interface for the CSecurityTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECURITYTOOL_H__3293B54B_D7E3_4DDF_AEF2_08461B3AA3F5__INCLUDED_)
#define AFX_SECURITYTOOL_H__3293B54B_D7E3_4DDF_AEF2_08461B3AA3F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 
class CSecurityTool  
{
public:
	CSecurityTool();
	virtual ~CSecurityTool();

public:
	//�õ���ǰ�����û�
	static BOOL GetCurrProcessUser(CString& strName);
	static BOOL ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable);

	//��ȡXP��½�û�
	static BOOL GetLogUserXP(CString& strName);
	//��ȡwin2000��½�û�
	static BOOL GetLogUser2K(CString& strName);
	static DWORD CreateProcByCurUsr(TCHAR * pProc,TCHAR * pParam,int nShowType = SW_HIDE,DWORD dwSessionId = 0);
	static DWORD CreateProcUseProcToken(DWORD dwProcessID,TCHAR * pProcName,TCHAR * pParam,int nShowType = SW_HIDE,void* pEn = 0);

private:
	static BOOL GetProcessUser(DWORD dwProcessID, TCHAR *szUserName, DWORD nNameLen);	
};

#endif // !defined(AFX_SECURITYTOOL_H__3293B54B_D7E3_4DDF_AEF2_08461B3AA3F5__INCLUDED_)
