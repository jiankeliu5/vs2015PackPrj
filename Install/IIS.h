// IIS.h: interface for the CIIS class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_IIS_H__11B61858_60DD_4429_AFD2_29DE4C6BA243__INCLUDED_)
#define AFX_IIS_H__11B61858_60DD_4429_AFD2_29DE4C6BA243__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <lm.h>
#include "iads.H"
#include "Adshlp.h"
#pragma comment(lib,"Netapi32.lib")
#pragma comment(lib,"adsiid.lib")
#pragma comment(lib,"activeds.lib")

#pragma	pack (push,1)
// Site info
typedef struct S_SiteInfo
{
	int		nNumber;//ÐòºÅ
	TCHAR 	szPort[8];	//¶Ë¿Ú
	TCHAR	szIP[40]; //IP
	TCHAR	szComment[256]; // Comment
	TCHAR	szVPath[256];	//ÐéÄâÂ·¾¶
}	S_SITEINFO,*P_S_SITEINFO;

#pragma	pack (pop)

class  CIIS  
{
public:
	CIIS();
	virtual ~CIIS();
protected:
	BOOL IsGroupMembers(IADsGroup *pGroup,TCHAR* sUser);
	BOOL EnumMembers(IADsMembers *pMembers, TCHAR* sUser);

	BOOL AddUserToGroup(TCHAR* sGroup, TCHAR* sUser);
	BOOL AddUser(TCHAR* sUser, TCHAR* sPsw, TCHAR* sGrp);
	BOOL ChangeUserPsw(TCHAR* sUser, TCHAR* sPsw);
	//BOOL AddObjectAccess(CString strFileName, const CSid &rSid, ACCESS_MASK accessmask,SE_OBJECT_TYPE eType /*= SE_OBJECT_TYPE*/);
public:
	
	BOOL GrantRightToUser(CString szDir);
	BOOL AddAccessRights(LPCTSTR lpszFileName,LPCTSTR szUserName,DWORD dwAccessMask);
	bool GetAnonymousUserName(LPTSTR pUserName);
	BOOL InitIISAnonymousSA(LPSECURITY_ATTRIBUTES pSA);

	DWORD AddGroupMembers();
	DWORD AddMachineAccount();
	BOOL AddOperator();

	CString GetServerBinding(VARIANT FAR* vArray);

	void RemoveAllList(CPtrList &pList);

};

#endif // !defined(AFX_IIS_H__11B61858_60DD_4429_AFD2_29DE4C6BA243__INCLUDED_)
