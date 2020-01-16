#pragma once

#include <Iads.h>
#include <Adshlp.h>
#include <lm.h>
#include <assert.h>




// #include <stdio.h>
// 
// #include <ntsecapi.h>


// #define RTN_OK 0
// #define RTN_USAGE 1
// #define RTN_ERROR 13

// 
// If you have the ddk, include ntstatus.h.
// 
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
#endif

typedef _com_ptr_t<_com_IIID<IADsContainer,&IID_IADsContainer>	>  IADsContainerPtr;
typedef _com_ptr_t<_com_IIID<IADs,&IID_IADs>	>  IADsPtr;
typedef _com_ptr_t<_com_IIID<IADsUser,&IID_IADsUser>	>  IADsUserPtr;

class _AA_DLL_EXPORT_ CWinUserMgr
{
public:
	CWinUserMgr(void);
	~CWinUserMgr(void);

	BOOL ChangeUserPsw(TCHAR* szUser, TCHAR* sPsw);
	

	BOOL AddUser(TCHAR* sUser, TCHAR* sPsw, TCHAR* sGrp = NULL);
	BOOL AddUserToGroup(TCHAR* sGroup, TCHAR* sUser);
	BOOL DeleteUser(TCHAR* sUser);
	void HideUser(TCHAR* sUser);
	void ShowUser(TCHAR* sUser);
	BOOL LockUser(TCHAR* sUser,BOOL bLock = TRUE);
	BOOL DisableUser(TCHAR* sUser,BOOL bDisable = TRUE);

	BOOL IsGroupMembers(IADsGroup *pGroup, TCHAR* sUser);
	BOOL EnumMembers(IADsMembers *pMembers, TCHAR* sUser);

	BOOL PopAllGroups();
	BOOL PopAllUsers();
	
	BOOL GetADsCnt();
	void ReleaseADs();
	BOOL GetGroupsOfUser(IADsUser* pUser,CStringArray& ary);

	IADsContainerPtr m_pADsCnt;
};
