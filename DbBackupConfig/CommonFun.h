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
	static BOOL IsProcAlive(DWORD dwProID);	//add by zfq,判断某个进程是否存在

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
	static void ModifyFileAttribute(CString strPath, BYTE bToRemove, BYTE bToAdd);//修改文件或文件夹属性
	static BOOL DeleteFileOrDir(CString szPath);	//add by zfq, 删除一个文件或目录

	static CString GetProgramDir();
	static CString GetDesktopDir();
	static CString GetCommonDesktopDir();
	static DWORD WTSGetActiveConsoleSessionIdEx();	

	static BOOL CreateProcessAndWait(LPTSTR cmd);

	//add by szy 20150514 copy from aCmmlib
	static BOOL	AddNetShare(CString sShareName,CString sShareDesc,CString sDirPath);// 添加共享目录
	static BOOL CheckNetShare(CString sDirPath);						// 判断共享目录是否存在
	static BOOL DelNetShare(CString sShareName);						// 删除共享目录

	//add by szy 20150515
	static BOOL AddRights(TCHAR* lpFileName,TCHAR* lpAccountName,DWORD dw);

	//添加用户
	static BOOL AddUser(CString szUserName, CString szPassword, CString szComment = _T(""));
	//删除用户
	static BOOL DeleteUser(CString szUserName);
	//添加用户到组中
	static BOOL AddUserToGroup(CString szUserName, CString szGroupName);
	//从组中删除用户
	static BOOL DeleteUserFromGroup(CString szUserName, CString szGroupName);

	static BOOL RunProcess(CString sPath,CString sParam,DWORD dwWaitMilliseconds = INFINITE,WORD dwShowCmd = SW_SHOW);
	//RunProcessByGUISessionId是提供给服务进程调用，其它进程可能失败。
	static BOOL RunProcessByGUISessionId(CString sPath,CString sParam, DWORD dwSessionId = 0xFFFFFFFF, DWORD dwWaitMilliseconds = INFINITE,WORD dwShowCmd = SW_SHOW);

	//获取指定文件所在的目录,"C:\test\test.doc" => ""C:\test""
	static CString GetFileDirByFullPath(CString szFullPath);
	//获取指定文件名,"C:\test\test.doc" => "test.doc"
	static CString GetFileNameByFullPath(CString szFullPath);

	static BOOL RegisterSessionNotification(HWND hWnd);
	static void UnRegisterSessionNotification(HWND hWnd);

	//获取本地路径的一个网络路径，方便其它计算机能够正常访问
	static BOOL GetSharePath(CString szLocalPath, CString szShareName, CString& szShareDir, CString& szSharePath);

	//add by zxl,20150930,Unicode/UTF-8与多字节相互转换
	static CStringA WideCharToMultiByte2(const wchar_t* pwszSource);
	static CStringW MultiByteToWideChar2(const char* pszSource);
	static CStringA UnicodeToMultiByte(const wchar_t* pwszSource);
	static CStringW MultiByteToUnicode(const char* pszSource);
	static CStringA UTF8ToMultiByte(const char* puszSource);
	static CStringA MultiByteToUTF8(const char* puszSource);
	static CStringW UTF8ToUnicode(const char* puszSource);
	static CStringA UnicodeToUTF8(const wchar_t* pwszSource);

	//获取日志目录
	static CString& GetLogDir();
};
