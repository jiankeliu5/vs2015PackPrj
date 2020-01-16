
// Install.h : PROJECT_NAME 应用程序的主头文件

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CInstallApp:
// 有关此类的实现，请参阅 Install.cpp
//

class CInstallApp : public CWinAppEx
{
public:
	CInstallApp();

	// 创建key
	void CreateServerKey(LPCTSTR pszData, LPCTSTR pszData2);
	void DeleteServerKey(); 

	int ParseParam(void);
	int m_nExitCode;
	BOOL m_bQueit;	//静默安装
	void SetErrCode(int nCode,CString sErr);

private:
	BOOL m_bDataBaseInstalled;
	BOOL m_bCoreServerInstalled;
	BOOL m_bWebServerInstalled;
	BOOL m_bSharePrintServerInstalled;
	BOOL m_bPrintStationInstalled;
	BOOL m_bKMEmbedPrintInstalled;
	BOOL m_bHPEmbedPrintInstalled;
	CString m_szThisBackupDir;
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

protected:
	void IntallDogDrv();
	BOOL EncryptCheck();
	BOOL CopyEnCryptFiles();
	void DelEncrypFile();

	//获取第三方数据库配置信息
	void GetAdoInfo();

	//安装IIS,要求win7和win server2008
	BOOL InstallIIS();

	//安装IIS,要求win8和win server2012
	BOOL InstallIIS2012();

	//安装.NET3.5
	BOOL InstallDotNet35();

	//获取已经安装.NET4.x的版本号
	CString GetDotNet4xVersion();

	//安装.NET4.x,如4.0，4.6等
	BOOL InstallDotNet4x();

	//检测安装.NET4.x,如4.0，4.6等的系统环境是否符合要求
	BOOL CheckDotNet4xInstallEventment();

	//安装.NET4.x在安装时要求的证书
	BOOL InstallDotNet4xCert();

	//安装SqlExpress
	BOOL InstallSqlExpress(CString szInstanceName, CString szSaPassword);
	//卸载SqlExpress
	BOOL UnInstallSqlExpress(CString szInstanceName);

	//安装数据库
	BOOL InstallDataBase();
	//卸载数据库
	BOOL UnInstallDataBase();
	BOOL UpdateDataBaseBegin();
	BOOL UpdateDataBaseEnd();

	//安装核心服务
	BOOL InstallCoreServer();
	//卸载核心服务
	BOOL UnInstallCoreServer();
	BOOL UpdateCoreServerBegin();
	BOOL UpdateCoreServerEnd();

	//安装WEB服务
	BOOL InstallWebServer();
	//卸载WEB服务
	BOOL UnInstallWebServer();
	BOOL UpdateWebServerBegin();
	BOOL UpdateWebServerEnd();

	//安装打印站点
	BOOL InstallPrintStation();
	//卸载打印站点
	BOOL UnInstallPrintStation();
	BOOL UpdatePrintStationBegin();
	BOOL UpdatePrintStationEnd();

	//安装共享打印服务
	BOOL InstallSharePrintServer();
	//卸载共享打印服务
	BOOL UnInstallSharePrintServer();
	BOOL UpdateSharePrintServerBegin();
	BOOL UpdateSharePrintServerEnd();

	//安装嵌入式打印WEB服务
	BOOL InstallEmbedPrintWeb();
	//卸载嵌入式打印WEB服务
	BOOL UnInstallEmbedPrintWeb();

	//安装嵌入式打印WEB(柯美)服务
	BOOL InstallEmbedPrintWebKM();
	//卸载嵌入式打印WEB(柯美)服务
	BOOL UnInstallEmbedPrintWebKM();
	BOOL UpdateEmbedPrintWebKMBegin();
	BOOL UpdateEmbedPrintWebKMEnd();

	//安装嵌入式打印WEB(惠普)服务
	BOOL InstallEmbedPrintWebHP();
	//卸载嵌入式打印WEB(惠普)服务
	BOOL UnInstallEmbedPrintWebHP();
	BOOL UpdateEmbedPrintWebHPBegin();
	BOOL UpdateEmbedPrintWebHPEnd();

	//安装Apache和PHP运行环境
	BOOL InstallApacheAndPHP();
	//卸载Apache和PHP运行环境
	BOOL UnInstallApacheAndPHP();

	//安装嵌入式打印WEB(佳能)服务
	BOOL InstallEmbedPrintWebCanon();
	//卸载嵌入式打印WEB(佳能)服务
	BOOL UnInstallEmbedPrintWebCanon();
	
	//安装vc2015
	BOOL Install_VC_Redist_x86_2015();

};

class CDrvInstallHelper
{
private:
	HMODULE m_hModule;
	TCHAR m_szPath[MAX_PATH + 1];
public:
	CDrvInstallHelper()
	{
		m_hModule = NULL;
		int nLen = GetModuleFileName(NULL, m_szPath, MAX_PATH);
		if (nLen == 0)
		{
			ASSERT(0);
		}
		TCHAR* pszDest = _tcsrchr(m_szPath,'\\');
		if (pszDest) 
			*pszDest = '\0';
	}
	~CDrvInstallHelper()
	{
		if(m_hModule)
		{
			::FreeLibrary(m_hModule);
		}
	}

	bool CallFun(char* szFun);

private:
	BOOL Load();

};

#define PRODUCT_CODE	0
class CProductInfo
{
public:

	static CString GetEduPrintProductName();
	static CString GetPrintProductName();

	//add by zfq,从程序目录中的到产品目录,即含有“打印复印管理系统”的目录
	static BOOL GetProductDirNameInProgramDir(CString &szProgramDir, CString &szProductDir);	
};


class CConfigSettingHelper
{
public:
	CConfigSettingHelper()
	{
		SetCurrentConfig(_T(""));
	}
	void LoadConnStr(CString& sDbSvr,CString& sDbName,CString& sDbUser,CString& sDbPwd,CString& szDbPort,int& nData )
	{
		sDbSvr = theIni.GetVal(_T("DBConfig"),_T("DbSvr"),_T(""));
		sDbName = theIni.GetVal(_T("DBConfig"),_T("DbName"),_T(""));
		sDbUser = theIni.GetVal(_T("DBConfig"),_T("DbUser"),_T(""));
		szDbPort = theIni.GetVal(_T("DBConfig"),_T("DbPort"),_T("1433"));
		nData = theIni.GetVal(_T("DBConfig"),_T("DbType"),SQLSERVER);

		sDbPwd = theIni.GetVal(_T("DBConfig"),_T("DbPwd"),_T(""));
#ifdef ENABLE_PWD_CRY
		char cPwd[MAX_PATH] = {0};
		int nSize = MAX_PATH;
		CPWDCry cry;
#ifdef UNICODE
		CStringA sDbPwdA = CCommonFun::UnicodeToMultiByte(sDbPwd);
		cry.UnCryptPWD(sDbPwdA.GetBuffer(), cPwd, nSize);
		sDbPwdA.ReleaseBuffer();
#else
		cry.UnCryptPWD(sDbPwd.GetBuffer(),cPwd,nSize);
		sDbPwd.ReleaseBuffer();
#endif // UNICODE
		sDbPwd = cPwd;
#endif
	}


	void SaveConnStr(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort =  _T("1433"),int nData = SQLSERVER)
	{
		theIni.SetVal(_T("DBConfig"),_T("DbSvr"),sDbSvr);
		theIni.SetVal(_T("DBConfig"),_T("DbName"),sDbName);
		theIni.SetVal(_T("DBConfig"),_T("DbUser"),sDbUser);
		
		theIni.SetVal(_T("DBConfig"),_T("DbPort"),szDbPort);
		theIni.SetVal(_T("DBConfig"),_T("DbType"),/*nData*/SQLSERVER);
#pragma message("暂时默认为数据库SQLSERVER，后续扩展为其它数据库时，要求修改配置界面")

#ifdef ENABLE_PWD_CRY
		CPWDCry cry;
#ifdef UNICODE
		CStringA sDbPwdA = CCommonFun::UnicodeToMultiByte(sDbPwd);
		char* pPsw = cry.CryptPWD(sDbPwdA.GetBuffer());
		sDbPwdA.ReleaseBuffer();
		CString sPwd(pPsw);
#else
		char* pPsw  = cry.CryptPWD(sDbPwd.GetBuffer());
		sDbPwd.ReleaseBuffer();
		CString sPwd(pPsw);
#endif // UNICODE
		delete[] pPsw;
		theIni.SetVal(_T("DBConfig"),_T("DbPwd"),sPwd);
#else
		theIni.SetVal(_T("DBConfig"),_T("DbPwd"),sDbPwd);
#endif
	}

	void SaveConnStr2(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szInstanceName)
	{
		theIni.SetVal(_T("DBConfig"),_T("DbSvr"),sDbSvr);
		theIni.SetVal(_T("DBConfig"),_T("DbName"),sDbName);
		theIni.SetVal(_T("DBConfig"),_T("DbUser"),sDbUser);
		
		theIni.SetVal(_T("DBConfig"),_T("DbPort"),0);
		theIni.SetVal(_T("DBConfig"),_T("DbInstanceName"),szInstanceName);
		theIni.SetVal(_T("DBConfig"),_T("DbType"),/*nData*/SQLSERVER);
#pragma message("暂时默认为数据库SQLSERVER，后续扩展为其它数据库时，要求修改配置界面")

#ifdef ENABLE_PWD_CRY
		CPWDCry cry;
#ifdef UNICODE
		CStringA sDbPwdA = CCommonFun::UnicodeToMultiByte(sDbPwd);
		char* pPsw = cry.CryptPWD(sDbPwdA.GetBuffer());
		sDbPwdA.ReleaseBuffer();
		CString sPwd(pPsw);
#else
		char* pPsw = cry.CryptPWD(sDbPwd.GetBuffer());
		sDbPwd.ReleaseBuffer();
		CString sPwd(pPsw);
#endif // UNICODE
		delete[] pPsw;
		theIni.SetVal(_T("DBConfig"),_T("DbPwd"),sPwd);
#else
		theIni.SetVal(_T("DBConfig"),_T("DbPwd"),sDbPwd);
#endif
	}
	
	void SaveSyxConnStr(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort =  _T("1433"))
	{
		theIni.SetVal(_T("SyxDBConfig"),_T("DbSvr"),sDbSvr);
		theIni.SetVal(_T("SyxDBConfig"),_T("DbName"),sDbName);
		theIni.SetVal(_T("SyxDBConfig"),_T("DbUser"),sDbUser);

		theIni.SetVal(_T("SyxDBConfig"),_T("DbPort"),szDbPort);
#ifdef ENABLE_PWD_CRY
		CPWDCry cry;
#ifdef UNICODE
		CStringA sDbPwdA = CCommonFun::UnicodeToMultiByte(sDbPwd);
		char* pPsw = cry.CryptPWD(sDbPwdA.GetBuffer());
		sDbPwdA.ReleaseBuffer();
		CString sPwd(pPsw);
#else
		char* pPsw = cry.CryptPWD(sDbPwd.GetBuffer());
		sDbPwd.ReleaseBuffer();
		CString sPwd(pPsw);
#endif // UNICODE
		delete[] pPsw;
		theIni.SetVal(_T("SyxDBConfig"),_T("DbPwd"),sPwd);
#else
		theIni.SetVal(_T("SyxDBConfig"),_T("DbPwd"),sDbPwd);
#endif
	}

	void SaveClientConfig(CString sIP,CString sPort = _T(""),DWORD dwValue = 0)
	{
		theIni.SetVal(_T("ClientConfig"),_T("SvrIP"),sIP);
		if (!sPort.IsEmpty())
			theIni.SetVal(_T("ClientConfig"),_T("SvrPort"),sPort);
		if (dwValue != 0)
			theIni.SetVal(_T("ClientConfig"),_T("iSec"),dwValue);
	}


	void SaveUpdaterServerConfig(CString sPath,UINT uPort,int nChecked = 1)
	{
		theIni.SetVal(_T("Updater"),_T("PatchDir"),sPath);
		theIni.SetVal(_T("Updater"),_T("CheckKB"),nChecked);
		theIni.SetVal(_T("Updater"),_T("HttpPort"),uPort);

		{
			//保存更新配置信息到注册表中
			CRegistryEx pReg;		
			pReg.SetParam(false);
			CString	szPath;
			szPath.Format(__T("%s%s"),PRODUCT_REGISTY_KEY,_T("Updater"));
			pReg.SetParam(false);	
			if(!pReg.Open((LPCTSTR)szPath) )
			{
				pReg.SetParam(false);
				if (!pReg.CreateKey(szPath) ) 
				{
					return;					
				}  	
			}
			pReg.Write(_T("PatchDir"), sPath);
			pReg.Write(_T("CheckKB"), nChecked);
			pReg.Write(_T("HttpPort"), (int)uPort);
			pReg.Close();
		}
	}

	void SaveUpdaterClientConfig(CString sPath,CString sAddr,UINT uPort)
	{
		theIni.SetVal(_T("UpdaterClt"),_T("LocalAddr"),sPath);
		theIni.SetVal(_T("UpdaterClt"),_T("SvrAddr"),sAddr);
		theIni.SetVal(_T("UpdaterClt"),_T("SvrPort"),uPort);
	}

	void SavePrtStationConfig(CString sIP,CString sPort = _T(""))
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}

		theIni.SetVal(sKey,_T("SvrIP"),sIP);
		if (!sPort.IsEmpty())
		{
			theIni.SetVal(sKey,_T("SvrPort"),sPort);
		}
		
	}
	void SaveScanDir(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("ScanDir"),sDir);
	}
	void SaveCopyDir(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("CopyDir"),sDir);
	}
	void SaveScanData(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("ScanDataType"),sDir);
	}
	void LoadScanDir(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		sDir = theIni.GetVal(sKey,_T("ScanDir"),_T(""));
	}
	void LoadCopyDir(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		sDir = theIni.GetVal(sKey,_T("CopyDir"),_T(""));
	}
	void LoadScanData(CString& sDir)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		sDir = theIni.GetVal(sKey,_T("ScanDataType"),_T(""));
	}
	void LoadBindIP(CString& sIP)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		sIP = theIni.GetVal(sKey,_T("BindIP"),_T(""));
	}

	void SavePrtYTJ(BOOL bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("EnableYTJ"),bEnable);
	}
	void SavePrtCopyBak(BOOL bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("EnableCopyBak"),bEnable);
	}
	void LoadPrtYTJ(BOOL& b)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		b = theIni.GetVal(sKey,_T("EnableYTJ"),0);
	}
	void LoadPrtCopyBak(BOOL& b)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		b = theIni.GetVal(sKey,_T("EnableCopyBak"),0);
	}
	void SavePrtComIndex(int nIndex)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("ComIndex"),nIndex);
	}
	void SavePrtBindIP(CString& s)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("BindIP"),s);
	}
	void SaveBrand(int n)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("Brand"),n);
	}
	void LoadBrand(int& b)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		b = theIni.GetVal(sKey,_T("Brand"),0);
	}

    void SaveSignalCtrlSet(BOOL boo)
    {
        CString sKey = _T("PrtStationConfig");
        if(IsWithOutPrintStation())
        {
            sKey = _T("PrtCtrlConfig");
        }
        theIni.SetVal(sKey,_T("SignalCtrl"), boo ? 1 : 0);
    }
    void LoadSignalCtrlSet(BOOL &boo)
    {
        CString sKey = _T("PrtStationConfig");
        if(IsWithOutPrintStation())
        {
            sKey = _T("PrtCtrlConfig");
        }
        boo = theIni.GetVal(sKey,_T("SignalCtrl"), 0);
    }
	void SaveSNMPPrintMon(BOOL bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("SnmpPrintMonitor"), bEnable ? 1 : 0);
	}
	void LoadSNMPPrintMon(BOOL &bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		bEnable = theIni.GetVal(sKey,_T("SnmpPrintMonitor"), 0);
	}
	void SaveSNMPPrinterMeter(BOOL bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("SnmpPrinterMeterMonitor"), bEnable ? 1 : 0);
	}
	void LoadSNMPPrinterMeter(BOOL &bEnable)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		bEnable = theIni.GetVal(sKey,_T("SnmpPrinterMeterMonitor"), TRUE);
	}
	void SetPrtPaperConfig(CString& s)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("PaperConfig"),s);
	}
	void LoadPrtPaperConfig(CString& s)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		s = theIni.GetVal(sKey,_T("PaperConfig"),_T(""));
	}
	void SetPrtAddr(CString& s)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey,_T("PrinterAddr"),s);
	}
	void LoadPrtAddr(CString& s)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		s = theIni.GetVal(sKey,_T("PrinterAddr"),_T(""));
	}
	void LoadPrtComIndex(int& nIndex)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		nIndex = theIni.GetVal(sKey,_T("ComIndex"),1);
	}
	void SaveMgrConfig(CString sIP,CString sPort = _T(""))
	{
		theIni.SetVal(_T("MgrConfig"),_T("SvrIP"),sIP);
		if (!sPort.IsEmpty())
		{
			theIni.SetVal(_T("MgrConfig"),_T("SvrPort"),sPort);
		}
	}

	void LoadPrtStationConfig(CString &sIP,CString& sPort)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		sIP = theIni.GetVal(sKey,_T("SvrIP"),_T(""));
		sPort = theIni.GetVal(sKey,_T("SvrPort"),_T(""));
	}
	BOOL IsWithOutPrintStation()
	{
		CString sTmpPath;
		sTmpPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),_T("LmPrinterSvcHost.exe"));
		if (PathFileExists(sTmpPath))
		{
			return TRUE;
		}
		return FALSE;
	}

	//add by zxl, 20130618
	BOOL IsPrintStationCloud()
	{
		if (!IsWithOutPrintStation())
		{
			CString sTmpPath;
			sTmpPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),_T("PrintStationCloud.exe"));
			if (PathFileExists(sTmpPath))
			{
				return TRUE;
			}
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	//===================================================add by zfq,2013.02.16,打印集中管理端,begin
	void LoadPrtCenterSvrConfig(CString &szSvrIP, CString &szSvrPort, CString &szLocalIP)
	{
		CString sKey = _T("PrtCenterSvr");
		szSvrIP = theIni.GetVal(sKey, _T("SvrIP"), _T(""));
		szSvrPort = theIni.GetVal(sKey, _T("SvrPort"), _T(""));
		szLocalIP = theIni.GetVal(sKey, _T("BindIP"), _T(""));
		return;
	}

	void SavePrtCenterSvrConfig(CString &szSvrIP, CString &szSvrPort, CString &szLocalIP)
	{
		CString sKey = _T("PrtCenterSvr");
		if(!szSvrIP.IsEmpty())
		{
			theIni.SetVal(sKey, _T("SvrIP"), szSvrIP);
		}
		if(!szSvrPort.IsEmpty())
		{
			theIni.SetVal(sKey, _T("SvrPort"), szSvrPort);
		}
		if(!szLocalIP.IsEmpty())
		{
			theIni.SetVal(sKey, _T("BindIP"), szLocalIP);
		}
		return;
	}
	//===================================================add by zfq,2013.02.16,打印集中管理端,end
	
	//add by szy 20150421 begin
	void SaveEmbedWebCfg(CString &szTitle,CString &szColor,CString &szSize)
	{
        CString sKey = _T("PrinterEmbedWeb");
        if(!szTitle.IsEmpty())
        {
            theIni.SetVal(sKey, _T("Title"), szTitle);
        }
        if(!szColor.IsEmpty())
        {
            theIni.SetVal(sKey, _T("Color"), szColor);
        }
        if(!szSize.IsEmpty())
        {
            theIni.SetVal(sKey, _T("Size"), szSize);
        }
	}
	void LoadEmbedWebCfg(CString &szTitle,CString &szColor,CString &szSize)
	{
	    CString sKey = _T("PrinterEmbedWeb");
	    szTitle = theIni.GetVal(sKey, _T("Title"), _T(""));
	    szColor = theIni.GetVal(sKey, _T("Color"), _T("#000000"));
	    szSize = theIni.GetVal(sKey, _T("Size"), _T("40px"));
	}
	
	void SaveEmbedDBCfg(int &PrtStationID,int & jump)
	{
        CString sKey = _T("PrinterEmbedWeb");
        if(PrtStationID > 0)
        {
            theIni.SetVal(sKey, _T("PrtStationID"), PrtStationID);
        }
        if(jump > 0)
        {
            theIni.SetVal(sKey, _T("Jump"), jump);
        }
	}
	void LoadEmbedDBCfg(int &PrtStationID,int & jump)
	{
        CString sKey = _T("PrinterEmbedWeb");
        PrtStationID = theIni.GetVal(sKey, _T("PrtStationID"), 1);
        jump = theIni.GetVal(sKey, _T("Jump"), 3000);
	}
	//add by szy 20150421 end

	//--add by zfq,2015-07-22 begin
	void LoadIsEmbed(BOOL &bEmbed)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		
		int nRet = theIni.GetVal(sKey, _T("IsEmbed"), 0);
		bEmbed = (0 == nRet) ? FALSE : TRUE;
		return;
	}

	void SaveIsEmbed(BOOL bEmbed)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}

		theIni.SetVal(sKey, _T("IsEmbed"), bEmbed ? 1 : 0);
		return;
	}
	//--add by zfq,2015-07-22 end

	//add by szy 20161216 增加选择嵌入式连接类型 begin
	void LoadEmbedConnectionType(int &nType)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		nType = theIni.GetVal(sKey, _T("EmbedConnectionType"), 3);	//默认为：3-数据库
	}
	void SaveEmbedConnectionType(int nType)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}

		theIni.SetVal(sKey, _T("EmbedConnectionType"), nType);
		return;
	}
	//add by szy 20161216 增加选择嵌入式连接类型 end

	void LoadEmbedListenPort(int &nPort){
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		nPort = theIni.GetVal(sKey, _T("ListenPort"), 5025);
	}
	void SaveEmbedListenPort(int nPort)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey, _T("ListenPort"), nPort);
		return;
	}
	void SaveHPIsPro(BOOL isPro)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}

		theIni.SetVal(sKey, _T("IsHPPro"), isPro);
		return;
	}
	BOOL IsHpPro()
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		BOOL ret = theIni.GetVal(sKey, _T("IsHPPro"), FALSE);
		return ret;
	}
	void SaveHpAtvAccTime(DWORD value)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey, _T("HPRegAccessoryTime"), value);
		return ;
	}
	void LoadHpAtvAccTime(DWORD& value)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		value = theIni.GetVal(sKey, _T("HPRegAccessoryTime"), 10);
		return ;
	}
	void SaveHpAtvAccUrl(CString& url)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey, _T("HPRegAccessoryUrl"), url);
		return ;
	}

	void LoadHpAtvAccUrl(CString& url)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		url = theIni.GetVal(sKey, _T("HPRegAccessoryUrl"), _T(""));
		return ;
	}
	void SaveEnableHPRegAccessory(BOOL value)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		theIni.SetVal(sKey, _T("EnableHPRegAccessory"), value);
		return ;
	}

	void LoadEnableHPRegAccessory(BOOL& value)
	{
		CString sKey = _T("PrtStationConfig");
		if(IsWithOutPrintStation())
		{
			sKey = _T("PrtCtrlConfig");
		}
		value = theIni.GetVal(sKey, _T("EnableHPRegAccessory"), TRUE);
		return ;
	}

	//设置当前配置文件
	void SetCurrentConfig(CString szBindPrinter /*= _T("")*/)
	{
		if (szBindPrinter.GetLength()>0)
		{
			theIni.m_sPath.Format(_T("%s\\%s\\config_%s.ini"),g_sDefaultPath,SYS_CONFIG_DIR,szBindPrinter);
		}
		else
		{
			theIni.m_sPath.Format(_T("%s\\%s"),g_sDefaultPath,SYS_CONFIG_FILE);
		}
		g_oIniFileCurretnConfig.m_sPath = theIni.m_sPath;
	}

protected:
	CIniFile theIni;

};
extern CInstallApp theApp;
extern CConfigSettingHelper theConfig;