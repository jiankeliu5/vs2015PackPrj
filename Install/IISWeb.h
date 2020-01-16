// IISWeb.h: interface for the CIISWeb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IISWEB_H__6D9570B0_B2C1_474D_9DE0_F55791A0849D__INCLUDED_)
#define AFX_IISWEB_H__6D9570B0_B2C1_474D_9DE0_F55791A0849D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IIS.h"
#include "CommonFun.h"
class  CIISWeb : public CIIS  
{
public:
	CIISWeb();
	virtual ~CIISWeb();
public:
	void StartWebSite(int nIndex);
	BOOL DeleteVirtualDir(LPCTSTR sVirDir,int nSvIndex);
	BOOL SetWebDirWriteAccess(int nIndex,CString sVName,CString sDir);
	BOOL SetWebSiteDefaultDoc(int nIndex, CString sDoc);
	BOOL SetWebSiteDefaultDoc(CString sIISPath, CString sDoc);
	BOOL SetWebSitePath(int nIndex, CString sPath);
	void GetWebSiteList(CPtrList *pList);
	void GetWebSitePath(int nWebNum,CString &sVPath);
	void GetWebSitePath(CString sIISPath,CString &sVPath);
	CString GetWebBinding(int nWebNum);

	//创建指定版本和类型的应用程序池
	CString CreateAppPool(CString szVersion = _T("v4.0"), BOOL bClassic = FALSE);
	//删除应用程序池
	BOOL DeleteAppPool(CString szAppPoolName);
	//获取应用程序池类型
	CString GetAppPool(CString szVersion = _T(""), BOOL bClassic = FALSE);
	//设置应用程序池类型
	void SetAppPool(CString szVersion = _T(""), BOOL bClassic = FALSE);

	void GetVirtualWebDirPath(int nWebNum,CString sVDir,CString &sVPath);
	BOOL CreateVirtualWebDir(int nSvIndex,LPCTSTR sDiskPath,LPCTSTR sVirName,LPCTSTR sUserName=NULL,LPCTSTR sPwd=NULL);
	BOOL CreateVirtualWebDir(LPCTSTR sIISPath,LPCTSTR sDiskPath,LPCTSTR sVirName,LPCTSTR sUserName=NULL,LPCTSTR sPwd=NULL);

	BOOL CreateWebSite(LPCTSTR sComment,LPCTSTR sVirName,LPCTSTR sPath,int nPort,BOOL bStart=TRUE);

	BOOL IsVirtualDirExist(LPCTSTR sVirDir,int nSvIndex);
	BOOL IsVirtualDirExist(LPCTSTR sIISPath,LPCTSTR sVirDir);
	BOOL DeleteWebSite(int nIndex);

	BOOL DisableHttp(LPCTSTR sComment, LPCTSTR sSerIp = _T("*"), int nPort = 80);
	BOOL EnableHttp(LPCTSTR sComment, LPCTSTR sSerIp = _T("*"), int nPort = 80);
	BOOL DisableHttps(LPCTSTR sComment, LPCTSTR sSerIp = _T("*"), int nPort = 443);
	BOOL EnableHttps(LPCTSTR sComment, LPCTSTR sCertPath, LPCTSTR sCertPassword, LPCTSTR sCertHash, LPCTSTR sSerIp = _T("*"), int nPort = 443);

	void CreateAdminLnk(CString szSerIp = _T(""), CString szSerPort = _T("80"));
	void RemoveAdminLnk();
	
private:
	CCommonFun m_fun;
	int GetUnusedWebNum();
	CString m_sAppPoolName;	//应用程序池名称
};

#endif // !defined(AFX_IISWEB_H__6D9570B0_B2C1_474D_9DE0_F55791A0849D__INCLUDED_)
