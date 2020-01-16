// CommonFun.h: interface for the CCommonFun class.
//
//////////////////////////////////////////////////////////////////////


#pragma once


typedef enum tagPLATFORM
{
	WIN_9X,WIN_NT,WIN_2K,WIN_XP,WIN_2003,UNKNOWN
} PLATFORM;

#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
#include <Winspool.h>
#include <WinSvc.h>
#include <comutil.h>
class  CCommonFun  
{
public:
	static int CmpDate(CTime& tm1,CTime& tm2);
	static CString GetVariantStr(_variant_t varValue);
	static BOOL CCommonFun::Is256ColorSupported();
	static BOOL StrToTime(CString &sStr,CTime& tm);
	static CString GetDirFromShell(BOOL bNewStyle, HWND hOwner = NULL);
	static DWORD GetSystemPageSize();
	static DWORD GetProcessSessionID(DWORD dwPID);
	static BOOL IsProcAlive(DWORD dwProID);	//add by zfq,�ж�ĳ�������Ƿ����

	//version
	static BOOL IsWin9X();
	static BOOL IsWinNT();
	static BOOL IsWin2003();
	static PLATFORM GetPlatform();
	static BOOL IsWinVistaOrLater();
#define EX_WIN_LOGOFF	1
#define	EX_WIN_REBOOT	2
#define EX_WIN_SHUTDOWN	3
	static BOOL ExitWin(int nMode);

	static CString GetDefaultPath();
	static CString GetLocalIP(void);
	static BOOL IsLocalIP(CString sIP);
	static int GetLocalIPList(CStringArray& ary);
	static CString GetLoaclHost(void);

	static CString GetLastErrorInfo();
	CString ChangePathToShort(CString sPath);
	
	static BOOL IsWow64();
	static BOOL CreateDir(CString sPath);
	static void ClearArrayElement(CObArray *pAry);
	static BOOL SHGetSpecialPath(int nFolder,CString &sPath);
	
	static void WriteFileLog(LPCTSTR lpszFormat, ...);
	static void WriteFileLog(CString sLog);
	static void StringSplit(const CString str, CStringArray *psArray , char chDelimiter=',', int nCount=-1);
	static void StringSplit(const CString sSource, CStringArray *parArray , CString sSplitter,BOOL bTriNUll = FALSE);


	static BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable);
	static BOOL IsAdmin();

	static DWORD StartSvc(CString strSvcName);
	static DWORD StopSvc(CString strSvcName);

	CCommonFun();
	virtual ~CCommonFun();

	static BOOL ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable);

	static void DeleteDir(CString sDir,BOOL bOnlyDelFile  = FALSE );
	static void ModifyFileAttribute(CString strPath, BYTE bToRemove, BYTE bToAdd);//�޸��ļ����ļ�������
	static BOOL DeleteFileOrDir(CString szPath);	//add by zfq, ɾ��һ���ļ���Ŀ¼

	static CString GetProgramDir();
	static CString GetDesktopDir();
	static CString GetCommonDesktopDir();
	static DWORD WTSGetActiveConsoleSessionIdEx();	

	static BOOL CreateProcessAndWait(LPTSTR cmd);

	//add by szy 20150514 copy from aCmmlib
	static BOOL	AddNetShare(CString sShareName,CString sShareDesc,CString sDirPath);// ��ӹ���Ŀ¼
	static BOOL CheckNetShare(CString sDirPath);						// �жϹ���Ŀ¼�Ƿ����
	static BOOL DelNetShare(CString sShareName);						// ɾ������Ŀ¼

	//add by szy 20150515
	static BOOL AddRights(TCHAR* lpFileName,TCHAR* lpAccountName,DWORD dw);

	//����û�
	static BOOL AddUser(CString szUserName, CString szPassword, CString szComment = _T(""));
	//ɾ���û�
	static BOOL DeleteUser(CString szUserName);
	//����û�������
	static BOOL AddUserToGroup(CString szUserName, CString szGroupName);
	//������ɾ���û�
	static BOOL DeleteUserFromGroup(CString szUserName, CString szGroupName);

	static BOOL RunProcess(CString sPath,CString sParam,DWORD dwWaitMilliseconds = INFINITE,WORD dwShowCmd = SW_SHOW);
	//RunProcessByGUISessionId���ṩ��������̵��ã��������̿���ʧ�ܡ�
	static BOOL RunProcessByGUISessionId(CString sPath,CString sParam, DWORD dwSessionId = 0xFFFFFFFF, DWORD dwWaitMilliseconds = INFINITE,WORD dwShowCmd = SW_SHOW);

	//��ȡָ���ļ����ڵ�Ŀ¼,"C:\test\test.doc" => ""C:\test""
	static CString GetFileDirByFullPath(CString szFullPath);
	//��ȡָ���ļ���,"C:\test\test.doc" => "test.doc"
	static CString GetFileNameByFullPath(CString szFullPath);

	static BOOL RegisterSessionNotification(HWND hWnd);
	static void UnRegisterSessionNotification(HWND hWnd);

	//��ȡ����·����һ������·������������������ܹ���������
	static BOOL GetSharePath(CString szLocalPath, CString szShareName, CString& szShareDir, CString& szSharePath);

	//add by zxl,20150930,Unicode/UTF-8����ֽ��໥ת��
	static CStringA WideCharToMultiByte2(const wchar_t* pwszSource);
	static CStringW MultiByteToWideChar2(const char* pszSource);
	static CStringA UnicodeToMultiByte(const wchar_t* pwszSource);
	static CStringW MultiByteToUnicode(const char* pszSource);
	static CStringA UTF8ToMultiByte(const char* puszSource);
	static CStringA MultiByteToUTF8(const char* puszSource);
	static CStringW UTF8ToUnicode(const char* puszSource);
	static CStringA UnicodeToUTF8(const wchar_t* pwszSource);

	//��ȡ��־Ŀ¼
	static CString& GetLogDir();
};
