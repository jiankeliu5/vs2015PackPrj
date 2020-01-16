// IISWeb.cpp: implementation of the CIISWeb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IISWeb.h"
#include <iiisext.h>
#include <iisext_i.c>
#include <intshcut.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIISWeb::CIISWeb()
{
	m_sAppPoolName = _T("DefaultAppPool");	//设置默认的应用程序池
}

CIISWeb::~CIISWeb()
{

}
//////////////////////////////////////////
//得到WEB站点的列表
//
//////////////////////////////////////////
void CIISWeb::GetWebSiteList(CPtrList *pList)
{
	RemoveAllList(*pList);
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
	
	// 首先创建一个服务实例
#if 0 //delete by zfq,2012.12.11
	if( ADsGetObject(L"IIS://localhost/w3svc",IID_IADsContainer,(void**)&iContainer) == S_OK ) 
#else
	HRESULT hRet = ADsGetObject(L"IIS://localhost/w3svc",IID_IADsContainer,(void**)&iContainer);
	if(S_OK != hRet)
	{
		theLog.Write(_T("!!CIISWeb::GetWebSiteList,1,ADsGetObject fail,err=%d,hRet=%x"), GetLastError(), hRet);
	}
	else
#endif
	{
		IEnumVARIANT *pEnum;
		LPUNKNOWN     pUnk;
		VARIANT       var;
		IDispatch    *pDisp;
		BSTR          bstrName;
		BSTR          bstrPath;
		unsigned long    lFetch;
		IADs         *pADs; 
		
		
		hRet = iContainer->get__NewEnum(&pUnk);
		if(FAILED(hRet))	//add by zfq,2012.12.11,log it when fail
		{
			theLog.Write(_T("!!CIISWeb::GetWebSiteList,2,get__NewEnum fail,err=%d,hRet=%x"), GetLastError(), hRet);
		}
		
		hRet = pUnk->QueryInterface(IID_IEnumVARIANT, (void**) &pEnum);
		if(FAILED(hRet) || !pEnum)	//add by zfq,2012.12.11,log it when fail
		{
			theLog.Write(_T("!!CIISWeb::GetWebSiteList,3,QueryInterface fail,err=%d,hRet=%x,pEnum=%p"), GetLastError(), hRet, pEnum);
		}

		pUnk->Release();
		
		// Now Enumerate 
		HRESULT hr = pEnum->Next(1, &var, &lFetch);
		if(S_OK != hr)
		{
			theLog.Write(_T("!!CIISWeb::GetWebSiteList,4,Next fail,err=%d,hRet=%x,pEnum=%p"), GetLastError(), hRet, pEnum);
		}

		while(hr == S_OK)
		{
			if (lFetch == 1)
			{
				pDisp = V_DISPATCH(&var);
				pDisp->QueryInterface(IID_IADs, (void**)&pADs); 
				pDisp->Release();
				pADs->get_Name(&bstrName);
				CString sName(bstrName);		
				pADs->get_ADsPath(&bstrPath);
				CString sPath(bstrPath);
				
				_variant_t varComment;
				pADs->Get(_bstr_t("ServerComment"),&varComment);	
				CString sComment = m_fun.GetVariantStr(varComment);
				sComment.TrimLeft(_T(" "));
				sComment.TrimRight(_T(" "));
				if( !sComment.IsEmpty()  
					&& sComment.CompareNoCase(_T("管理 Web 站点")) !=0
					&& sComment.CompareNoCase(_T("Microsoft SharePoint 管理")) !=0
					&& sComment.CompareNoCase(_T("Windows Media 管理站点")) !=0
					&& sComment.CompareNoCase(_T("Administration")) !=0
					)
					
				{
					CString sVirPath;
					GetWebSitePath(sPath,sVirPath);
					VARIANT varBindings;
					pADs->Get(_bstr_t("ServerBindings"),&varBindings);
					CString sBindings = GetServerBinding( &varBindings );
					int nPos = sBindings.Find(_T(":"));
					int nPos2 = sBindings.ReverseFind( ':' );
					CString sServerIP = sBindings.Left( nPos );
					CString sServerPort = sBindings.Mid(nPos+1, nPos2 - nPos -1 );
					
					//------------------------------------------
					// Save comment
					P_S_SITEINFO pInfo = new S_SITEINFO;
					ZeroMemory( pInfo, sizeof(S_SITEINFO));
					
					pInfo->nNumber = m_fun.SToN(sName);
					_tcscpy(pInfo->szPort,sServerPort);
					_tcscpy(pInfo->szIP,sServerIP);
					_tcscpy(pInfo->szComment, sComment);
					_tcscpy(pInfo->szVPath,sVirPath);
					
					pList->AddTail( pInfo );
				}
				
				pADs->Release();
			}
			hr = pEnum->Next(1, &var, &lFetch);
		}			
		
		pEnum->Release();		
	}
	if( iContainer )
	{			
		iContainer->Release();
		iContainer = NULL;
	}
}

///////////////////////////////////////////////////////////
//得到WEB站点的path
//nWebNum为web站点号
///////////////////////////////////////////////////////////
void CIISWeb::GetWebSitePath(int nWebNum,CString &sVPath)
{
	CString sWeb;
	sWeb.Format(_T("IIS://LocalHost/w3svc/%d"),nWebNum);	
	GetWebSitePath(sWeb,sVPath);
}

///////////////////////////////////////////////////////////
//得到WEB站点的path
//sIISPath为web站点的IIS路径如:IIS://LocalHost/w3svc/1
////////////////////////////////////////////////////////////
void CIISWeb::GetWebSitePath(CString sIISPath,CString &sVPath)
{
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;

	HRESULT     hr;
	BSTR bsTmp = sIISPath.AllocSysString();
	hr = ADsGetObject(bsTmp,IID_IADsContainer,(void**)&iContainer);
	::SysFreeString(bsTmp);
 	// 首先创建一个服务实例
	if( hr == S_OK ) 
	{
		_bstr_t bsRoot("Root");
		hr = iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), bsRoot,(IDispatch**)&iAds);
		if( hr == S_OK )
		{
			_variant_t varComment;
			iAds->Get(_bstr_t("Path"),&varComment);
			
			sVPath = varComment.bstrVal;
			
			iAds->Release();
		}
		else //add by zfq,2012.12.11,log it when fail
		{
			theLog.Write(_T("!!CIISWeb::GetWebSitePath,10,GetObject FAIL,err=%d,hr=%x"), GetLastError(), hr);
		}

		iContainer->Release();
	}
	else //add by zfq,2012.12.11,log it when fail
	{
		theLog.Write(_T("!!CIISWeb::GetWebSitePath,11,ADsGetObject FAIL,err=%d,hr=%x"), GetLastError(), hr);
	}
}

//创建指定版本和类型的应用程序池
CString CIISWeb::CreateAppPool(CString szVersion /*= _T("v4.0")*/, BOOL bClassic /*= FALSE*/)
{
	return _T("");
}

//删除应用程序池
BOOL CIISWeb::DeleteAppPool(CString szAppPoolName)
{
	return FALSE;
}

//获取应用程序池。如果为""=默认应用程序池，如果为"v4.0"="ASP.NET v4.0"
//DefaultAppPool,			IIS://localhost/W3SVC/AppPools/DefaultAppPool
//Classic .NET AppPool,		IIS://localhost/W3SVC/AppPools/Classic .NET AppPool
//ASP.NET v4.0,				IIS://localhost/W3SVC/AppPools/ASP.NET v4.0
//ASP.NET v4.0 Classic,		IIS://localhost/W3SVC/AppPools/ASP.NET v4.0 Classic
CString CIISWeb::GetAppPool(CString szVersion /*= _T("")*/, BOOL bClassic /*= FALSE*/)
{
	theLog.Write(_T("CIISWeb::GetAppPool,szVersion=%s,bClassic=%d"), szVersion, bClassic);

	CString szAppPool = _T("DefaultAppPool");
	if (bClassic)
	{
		szAppPool = _T("Classic .NET AppPool");
	}
	if (szVersion.GetLength() <= 0)
	{
		return szAppPool;
	}

	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
	HRESULT hRet = ADsGetObject(L"IIS://localhost/W3SVC/AppPools",IID_IADsContainer,(void**)&iContainer);
	if(SUCCEEDED(hRet)) 
	{
		IEnumVARIANT *pEnum;
		LPUNKNOWN     pUnk;
		VARIANT       var;
		IDispatch    *pDisp;
		BSTR          bstrName;
		unsigned long    lFetch;
		IADs         *pADs; 

		hRet = iContainer->get__NewEnum(&pUnk);
		if(FAILED(hRet))
		{
			theLog.Write(_T("!!CIISWeb::GetAppPool,2,get__NewEnum fail,err=%d,hRet=%x"), GetLastError(), hRet);
		}

		hRet = pUnk->QueryInterface(IID_IEnumVARIANT, (void**) &pEnum);
		if(FAILED(hRet) || !pEnum)
		{
			theLog.Write(_T("!!CIISWeb::GetAppPool,3,QueryInterface fail,err=%d,hRet=%x,pEnum=%p"), GetLastError(), hRet, pEnum);
		}

		pUnk->Release();

		// Now Enumerate 
		HRESULT hr = pEnum->Next(1, &var, &lFetch);
		if(S_OK != hr)
		{
			theLog.Write(_T("!!CIISWeb::GetAppPool,4,Next fail,err=%d,hRet=%x,pEnum=%p"), GetLastError(), hRet, pEnum);
		}

		while(hr == S_OK)
		{
			if (lFetch == 1)
			{
				pDisp = V_DISPATCH(&var);
				pDisp->QueryInterface(IID_IADs, (void**)&pADs); 
				pDisp->Release();
				pADs->get_Name(&bstrName);
				CString sName(bstrName);
				BSTR bstrPath;
				pADs->get_ADsPath(&bstrPath);
				CString sPath(bstrPath);
				theLog.Write(_T("sName=%s,sPath=%s"), sName, sPath);
				pADs->Release();
				if (sName.Find(szVersion) >= 0)
				{
					if (bClassic)
					{
						if (sName.Find(_T("Classic")) >= 0)
						{
							szAppPool = sName;
							break;
						}
					}
					else
					{
						if (sName.Find(_T("Classic")) < 0)
						{
							szAppPool = sName;
							break;
						}
					}
				}
			}
			hr = pEnum->Next(1, &var, &lFetch);
		}

		pEnum->Release();		
		iContainer->Release();
	}
	else
	{
		theLog.Write(_T("!!CIISWeb::GetAppPool,5,ADsGetObject fail"));
	}

	theLog.Write(_T("CIISWeb::GetAppPool,szAppPool=%s"), szAppPool);

	return szAppPool;
}

void CIISWeb::SetAppPool(CString szVersion /*= _T("")*/, BOOL bClassic /*= FALSE*/)
{
	m_sAppPoolName = GetAppPool(szVersion, bClassic);
}

void  CIISWeb::GetVirtualWebDirPath(int nWebNum,CString sVDir,CString &sVPath)
{
	CString sIISPath;
	sIISPath.Format(_T("IIS://localhost/w3svc/%d/ROOT"),nWebNum);	

	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;

	HRESULT     hr;
	BSTR bsTmp = sIISPath.AllocSysString();
	hr = ADsGetObject(bsTmp,IID_IADsContainer,(void**)&iContainer);
	::SysFreeString(bsTmp);
 	// 首先创建一个服务实例
	if( hr == S_OK ) 
	{
		_bstr_t bsRoot(sVDir);
		hr = iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), bsRoot,(IDispatch**)&iAds);
		if( hr == S_OK )
		{
			_variant_t varComment;
			iAds->Get(_bstr_t("Path"),&varComment);
			
			sVPath = varComment.bstrVal;
			
			iAds->Release();
		}
		else //add by zfq,2012.12.11,log it when fail
		{
			theLog.Write(_T("!!CIISWeb::GetVirtualWebDirPath,10,GetObject FAIL,err=%d,hr=%p"), GetLastError(),hr);
		}

		iContainer->Release();
	}
	else //add by zfq,2012.12.11,log it when fail
	{
		theLog.Write(_T("!!CIISWeb::GetVirtualWebDirPath,11,ADsGetObject FAIL,err=%d,hr=%p"), GetLastError(),hr);
	}
}

///////////////////////////////////////////////////////////
//创建web站点
//LPCTSTR sComment 名称描述
//LPCTSTR sPath  路径
//int nPort  端口
////////////////////////////////////////////////////////////
BOOL CIISWeb::CreateWebSite(LPCTSTR sComment, LPCTSTR sVirName, LPCTSTR sPath,int nPort,BOOL bStart)
{
	IADsContainer* iContainer;
	IADs* iAds;

	CString sIISPath;
	sIISPath.Format(_T("IIS://localhost/w3svc"));
 	/* 获得WebSever */ 
	if(ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer) != S_OK)
	{
		theLog.Write(_T("!!CIISWeb::CreateWebSite,1,ADsGetObject fail,err=%d"), GetLastError());
		return FALSE;
	}

	//建立虚拟目录 
	int nIndex = GetUnusedWebNum();	
	if( nIndex == 0 )
		nIndex = 10;
	if(HRESULT hr =iContainer->Create(_bstr_t("IIsWebServer"), _bstr_t(m_fun.NToS(nIndex)),(IDispatch**)&iAds)!=S_OK)
	{
		iContainer->Release();
		theLog.Write(_T("!!CIISWeb::CreateWebSite,2,Create fail,err=%d, hr=%p"), GetLastError(), hr);
		return FALSE;
	}
	CString sBinding;
	sBinding.Format(_T(":%d:"),nPort);
	iAds->Put(_bstr_t("ServerSize"),_variant_t("1"));
	iAds->Put(_bstr_t("ServerComment"),_variant_t(sComment));
	iAds->Put(_bstr_t("ServerBindings"),_variant_t(sBinding));
	iAds->Put(_bstr_t("ServerAutoStart"),_variant_t("1"));
	if( bStart )
	{
		IADsServiceOperations *pSrvOp = NULL;
		long status = 0;
		
		HRESULT hr = iAds->QueryInterface(IID_IADsServiceOperations,(void**)&pSrvOp);
		
		
		if(pSrvOp) pSrvOp->Start();
		if(pSrvOp) pSrvOp->Release();		
	}
	iAds->SetInfo();
	BSTR bsIISPath;
	iAds->get_ADsPath(&bsIISPath);
	iAds->Release();
	iContainer->Release();

#if 1
	sIISPath = CString(bsIISPath);
	theLog.Write(_T("CIISWeb::CreateWebSite,sIISPath=[%s]"), sIISPath);
	//创建一个默认路径
	CString szDefaultPath(_T("c:\\inetpub\\wwwroot"));
	if (!PathFileExists(szDefaultPath))
	{
		CCommonFun::CreateDir(szDefaultPath);
	}
	if( !CreateVirtualWebDir(sIISPath,szDefaultPath,_T("ROOT")) )
	{
		theLog.Write(_T("!!CIISWeb::CreateWebSite,15,CreateVirtualWebDir fail,err=%d"), GetLastError());
		return FALSE;
	}

	CString szVirName(sVirName);
	if (!szVirName.IsEmpty())
	{
		sIISPath += CString(_T("/Root"));
		theLog.Write(_T("CIISWeb::CreateWebSite,sIISPath=[%s]"), sIISPath);
		if( !CreateVirtualWebDir(sIISPath,sPath,sVirName) )
		{
			theLog.Write(_T("!!CIISWeb::CreateWebSite,16,CreateVirtualWebDir fail,err=%d"), GetLastError());
			return FALSE;
		}
	}

	sIISPath = CString(bsIISPath);
	if (!SetWebSiteDefaultDoc(sIISPath,_T("Default.aspx")))
	{
		theLog.Write(_T("!!CIISWeb::CreateWebSite,17,SetWebSiteDefaultDoc fail,err=%d"), GetLastError());
		return FALSE;
	}
#endif

	return TRUE;
}
///////////////////////////////////////////////////////////
//创建web站点虚拟目录
// LPCTSTR sDiskPath 硬盘路径
//LPCTSTR sVirName 虚拟目录名称
//nSvIndex WEB站点号
////////////////////////////////////////////////////////////
BOOL CIISWeb::CreateVirtualWebDir(int nSvIndex,LPCTSTR sDiskPath,LPCTSTR sVirName,LPCTSTR sUserName,LPCTSTR sPwd)
{
	CString sPath;
	sPath.Format(_T("IIS://localhost/w3svc/%d/Root"),nSvIndex);
	return CreateVirtualWebDir(sPath,sDiskPath,sVirName,sUserName,sPwd);
}

///////////////////////////////////////////////////////////
//创建web站点虚拟目录
//LPCTSTR sIISPath WEB站点的IIS路径如:IIS://LocalHost/w3svc/1
// LPCTSTR sDiskPath 硬盘路径
//LPCTSTR sVirName 虚拟目录名称
////////////////////////////////////////////////////////////
BOOL CIISWeb::CreateVirtualWebDir(LPCTSTR sIISPath,LPCTSTR sDiskPath,LPCTSTR sVirName,LPCTSTR sUserName,LPCTSTR sPwd)
{
	IADsContainer* iContainer;
	IADs* iAds;

	CString lpszDiskPath = sDiskPath;
	CString lpszVirtualDirName = sVirName;

 	/* 获得WebSever */ 
	HRESULT hr=ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer);
	if( hr != S_OK)
	{
		m_fun.WriteFileLog(_T("ADsGetObject(%s) Error=%d hr=0x%x"),sIISPath,GetLastError(),hr);
		return FALSE;
	}

	//先检测了虚拟目录 是否存在
	if( this->IsVirtualDirExist(sIISPath,sVirName) )
	{
		m_fun.WriteFileLog(_T(" %s,%s already exist!"),sIISPath,sVirName);
		hr=iContainer->Delete(_bstr_t("IIsWebVirtualDir"), _bstr_t(lpszVirtualDirName));
		if( hr != S_OK)
		{
			m_fun.WriteFileLog(_T("Delete(IIsWebVirtualDir %s) Error=%d hr=0x%x"),lpszVirtualDirName,GetLastError(),hr);
		}

		/*hr=iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), _bstr_t(lpszVirtualDirName),(IDispatch**)&iAds);
		if( hr != S_OK)
		{
			m_fun.WriteFileLog(_T("GetObject(IIsWebVirtualDir) Error=%d hr=0x%x"),GetLastError(),hr);
			iContainer->Release();
			return FALSE;
		}
		iContainer->Release();*/
	}
	//else
//	{		
		//建立虚拟目录 
		if(hr=iContainer->Create(_bstr_t("IIsWebVirtualDir"), _bstr_t(lpszVirtualDirName),(IDispatch**)&iAds)!=S_OK)
		{
			m_fun.WriteFileLog(_T("Create(%s) Error=%d hr=0x%x"),lpszVirtualDirName,GetLastError(),hr);
			iContainer->Release();
			return FALSE;
		}
		iContainer->Release();
//	}

	//设置虚拟目录的属性 
	iAds->Put(_bstr_t("AccessRead"),_variant_t("True"));//注意跟VB中的设置属性比较
	iAds->Put(_bstr_t("AccessWrite"),_variant_t("False"));
	iAds->Put(_bstr_t("AccessNoRemoteWrite"),_variant_t("False"));
	iAds->Put(_bstr_t("EnableDirBrowsing"),_variant_t("FALSE"));
	iAds->Put(_bstr_t("EnableDefaultDoc"),_variant_t("true"));
	iAds->Put(_bstr_t("AccessFlags"),_variant_t(long(513)));

	iAds->Put(_bstr_t("DontLog"),_variant_t("TRUE"));
	iAds->Put(_bstr_t("Path"),_variant_t(lpszDiskPath));
	iAds->Put(_bstr_t("AspEnableParentPaths"),_variant_t(true));
	
	if( sUserName != NULL && !CString(sUserName).IsEmpty() )
	{
		iAds->Put(_bstr_t("UNCUserName"),_variant_t(sUserName));
		iAds->Put(_bstr_t("UNCPassword"),_variant_t(sPwd));
	}

	//------------------------------------------------
	IISApp *pApp = NULL;
	IISApp2 *pApp2 = NULL;
	IISApp3 *pApp3 = NULL;	

	hr = iAds->QueryInterface( IID_IISApp3, (void **)&pApp3 );
	if ( FAILED( hr ) )
	{
		hr = iAds->QueryInterface( IID_IISApp2, (void **)&pApp2 );
		if ( FAILED( hr ) )
		{
			hr = iAds->QueryInterface( IID_IISApp, (void **)&pApp );
			if ( FAILED( hr ) )
			{
				TRACE( "QI for IIsApp3 failed. Error 0x%0x\n", hr );
				goto error;
			}
		}
	}

	theLog.Write(_T("CreateVirtualWebDir,pApp=%p,pApp2=%p,pApp3=%p"), pApp, pApp2, pApp3);

	VARIANT varPool;
	VariantInit( &varPool );
	
	varPool.vt = VT_BSTR;
	//varPool.bstrVal = SysAllocString( L"DefaultAppPool" );
	varPool.bstrVal = m_sAppPoolName.AllocSysString();
	
	VARIANT varCreatePool;
	VariantInit( &varCreatePool );	
	varCreatePool.vt = VT_BOOL;
	varCreatePool.boolVal = VARIANT_TRUE;
	
	if(pApp)
		hr = pApp->AppCreate(FALSE);
	else if(pApp2)
		hr = pApp2->AppCreate2(1);
	else if(pApp3)
		hr = pApp3->AppCreate3( 2, varPool, varCreatePool );
	if ( FAILED( hr ) )
	{
		TRACE( "AppCreateX() call failed. Error 0x%0x\n", hr );
		goto error;
	}
	
	TRACE( "Call to AppCreate3() succeeded!\n" );

	iAds->Put(_bstr_t("AppFriendlyName"),_variant_t(sVirName));

error:
	if ( pApp)
		pApp->Release();
	if ( pApp3 )
		pApp3->Release();
	
	if ( pApp2 )
		pApp2->Release();

	//-------------------------------------------------

	iAds->SetInfo();
	iAds->Release();

	return TRUE;
}
///////////////////////////////////////////////////////////
//得到WebBinding
//nWebNum为web站点号
//////////////////////////////////////////////////////////
CString CIISWeb::GetWebBinding(int nWebNum)
{
	CString sBindings;
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
 	
	CString sWebNum;
	sWebNum.Format(_T("%d"),nWebNum);
	if( ADsGetObject(L"IIS://localhost/w3svc",IID_IADsContainer,(void**)&iContainer) == S_OK ) 
	{
		if( iContainer->GetObject(_bstr_t("IIsWebServer"), _bstr_t( sWebNum.operator LPCTSTR() ),(IDispatch**)&iAds) == S_OK )
		{
			VARIANT varBindings;
			int nRet;
			nRet = iAds->Get(_bstr_t("ServerBindings"),&varBindings);
			sBindings = GetServerBinding( &varBindings );
			iAds->Release();
		}

		iContainer->Release();
	}

	return sBindings;
}
///////////////////////////////////////////////////////////
//删除站点
//nIndex为web站点号
//////////////////////////////////////////////////////////
BOOL CIISWeb::DeleteWebSite(int nIndex)
{	
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
	CString sPath;
	sPath.Format(_T("IIS://localhost/w3svc"));	
	// 首先创建一个服务实例
	HRESULT hr =  ADsGetObject(_bstr_t(sPath),IID_IADsContainer,(void**)&iContainer);
	if(hr == S_OK ) 
	{
		BSTR bs = _com_util::ConvertStringToBSTR(m_fun.NToS(nIndex));

		hr = iContainer->Delete(_bstr_t("IIsWebServer"),bs);
		SysFreeString(bs);

		iContainer->Release();
	}

	return TRUE;
}
///////////////////////////////////////////////////////////
//得到最小的一个未用的站点号
//////////////////////////////////////////////////////////
int CIISWeb::GetUnusedWebNum()
{
	int nIndex = 0;
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
	
	// 首先创建一个服务实例
	if( ADsGetObject(L"IIS://localhost/w3svc",IID_IADsContainer,(void**)&iContainer) == S_OK ) 
	{
		for(int i=1; i<100; i++)
		{
			CString sNumber;
			sNumber.Format(_T("%d"), i);
			
			if( iContainer->GetObject(_bstr_t("IIsWebServer"), _bstr_t( sNumber.operator LPCTSTR() ),(IDispatch**)&iAds) == S_OK )
			{				
				iAds->Release();
			}
			else
			{
				nIndex = i;
				break;
			}
			
		}
		
		iContainer->Release();
	}
	return nIndex;
}

///////////////////////////////////////////////////////////
//检查站点的虚拟目录是否存在
//nSvIndex为web站点号
//////////////////////////////////////////////////////////
BOOL CIISWeb::IsVirtualDirExist(LPCTSTR sVirDir,int nSvIndex)
{
	IADsContainer* iContainer;

	CString sW3Path;
	sW3Path.Format(_T("IIS://localhost/w3svc/%d/Root/%s"),nSvIndex,sVirDir);
 	/* 获得WebSever */ 
	HRESULT hr1 = ADsGetObject(_bstr_t(sW3Path),IID_IADsContainer,(void**)&iContainer);
	if( hr1 == S_OK)
	{	
		iContainer->Release();
		return TRUE;
	}

	return FALSE;
}
///////////////////////////////////////////////////////////
//检查站点的虚拟目录是否存在
//nSvIndex为web站点号
//////////////////////////////////////////////////////////
BOOL CIISWeb::IsVirtualDirExist(LPCTSTR sIISPath,LPCTSTR sVirDir)
{
	IADsContainer* iContainer;

	CString sW3Path;
	sW3Path.Format(_T("%s/%s"),sIISPath,sVirDir);
 	/* 获得WebSever */ 
	HRESULT hr1 = ADsGetObject(_bstr_t(sW3Path),IID_IADsContainer,(void**)&iContainer);
	if( hr1 == S_OK)
	{	
		iContainer->Release();
		return TRUE;
	}

	return FALSE;
}

BOOL CIISWeb::SetWebSitePath(int nIndex, CString sPath)
{
	IADsContainer* iContainer;
	IADs* iAds;

	CString sIISPath;
	sIISPath.Format(_T("IIS://localhost/w3svc/%d"),nIndex);
 	/* 获得FtpSever */ 
	HRESULT hr;
	if(hr=ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer) != S_OK)
	{
		m_fun.WriteFileLog(_T("Path=%s hr=0x%x"),sIISPath,hr);
		return FALSE;
	}

	if( hr=iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), _bstr_t("ROOT"),(IDispatch**)&iAds) != S_OK )
	{
		m_fun.WriteFileLog(_T("Path=%s hr=0x%x"),sIISPath,hr);
		return FALSE;
	}

	

	//设置虚拟目录的属性 	
	_variant_t var;
	var.vt = VT_BOOL;
	var.boolVal = true;
	iAds->Put(_bstr_t("AccessRead"),var);
	iAds->Put(_bstr_t("AccessWrite"),var);
	var.boolVal = false;
	iAds->Put(_bstr_t("DontLog"),var);
	iAds->Put(_bstr_t("Path"),_variant_t(sPath));
	iAds->SetInfo();

	iAds->Release();
	iContainer->Release();

	return TRUE;
}
BOOL CIISWeb::SetWebSiteDefaultDoc(int nIndex, CString sDoc)
{
	IADsContainer* iContainer;
	IADs* iAds;

	CString sIISPath;
	sIISPath.Format(_T("IIS://localhost/w3svc/%d"),nIndex);
 	/* 获得FtpSever */ 
	if(ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer) != S_OK)
		return FALSE;

	if( !iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), _bstr_t("ROOT"),(IDispatch**)&iAds) == S_OK )
	{
		return FALSE;
	}

	

	//设置虚拟目录的属性 	

	iAds->Put(_bstr_t("DefaultDoc"),_variant_t(sDoc));
	iAds->Put(_bstr_t("AspEnableParentPaths"),_variant_t(true));

	iAds->SetInfo();

	iAds->Release();
	iContainer->Release();

	return TRUE;
}

BOOL CIISWeb::SetWebSiteDefaultDoc(CString sIISPath, CString sDoc)
{
	theLog.Write(_T("CIISWeb::SetWebSiteDefaultDoc,sIISPath=[%s]"), sIISPath);
	IADsContainer* iContainer;
	IADs* iAds;

// 	CString sIISPath;
// 	sIISPath.Format(_T("IIS://localhost/w3svc/%d"),nIndex);
	/* 获得FtpSever */ 
	if(ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer) != S_OK)
		return FALSE;

	if( !iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), _bstr_t("ROOT"),(IDispatch**)&iAds) == S_OK )
	{
		return FALSE;
	}



	//设置虚拟目录的属性 	

	iAds->Put(_bstr_t("DefaultDoc"),_variant_t(sDoc));
	iAds->Put(_bstr_t("AspEnableParentPaths"),_variant_t(true));

	iAds->SetInfo();

	iAds->Release();
	iContainer->Release();

	return TRUE;
}


BOOL CIISWeb::SetWebDirWriteAccess(int nIndex, CString sVName, CString sDir)
{
	IADsContainer* iContainer;
	IADs* iAds;

	CString sIISPath;
	HRESULT hr;
	sIISPath.Format(_T("IIS://localhost/w3svc/%d/ROOT/%s"),nIndex,sVName);
 	/* 获得FtpSever */ 
	hr = ADsGetObject(_bstr_t(sIISPath),IID_IADsContainer,(void**)&iContainer);
	if( hr != S_OK)
		return FALSE;

	hr = iContainer->GetObject(_bstr_t("IIsWebVirtualDir"), _bstr_t(sDir),(IDispatch**)&iAds);

	if( FAILED(hr) )
	{
		m_fun.WriteFileLog(_T("GetObject(IIsWebDirectory) Error %d,hr=0x%x "),GetLastError(),hr);
		return FALSE;
	}

	
	//设置虚拟目录的属性 	
	_variant_t var;
	var.vt = VT_BOOL;
	var.boolVal = true;
	iAds->Put(_bstr_t("AccessRead"),var);
	iAds->Put(_bstr_t("AccessWrite"),var);
	iAds->SetInfo();

	iAds->Release();
	iContainer->Release();

	return TRUE;
}

BOOL CIISWeb::DeleteVirtualDir(LPCTSTR sVirDir, int nSvIndex)
{
	IADsContainer* iContainer;

	CString sW3Path;
	sW3Path.Format(_T("IIS://localhost/w3svc/%d/Root"),nSvIndex);
 	/* 获得WebSever */ 
	HRESULT hr1 = ADsGetObject(_bstr_t(sW3Path),IID_IADsContainer,(void**)&iContainer);
	if( hr1 == S_OK)
	{	
		HRESULT hr=iContainer->Delete(_bstr_t("IIsWebVirtualDir"), _bstr_t(sVirDir));
		iContainer->Release();
		if( hr == S_OK )
		{
			return TRUE;
		}
		else
		{
			m_fun.WriteFileLog(_T("Delete(IIsWebVirtualDir %s) Error=%d hr=0x%x"),sVirDir,GetLastError(),hr);

		}
	}

	return FALSE;
}

void CIISWeb::StartWebSite(int nIndex)
{
	/*
	CString sBindings;
	IADsContainer* iContainer = NULL;
	IADs* iAds = NULL;
 	
	CString sWebNum;
	sWebNum.Format(_T("%d"),nIndex);
	if( ADsGetObject(L"IIS://localhost/w3svc",IID_IADsContainer,(void**)&iContainer) == S_OK ) 
	{
		if( iContainer->GetObject(_bstr_t("IIsWebServer"), _bstr_t( sWebNum.operator LPCTSTR() ),(IDispatch**)&iAds) == S_OK )
		{
			iAds->Start();	
			iAds->Release();
		}

		iContainer->Release();
	}
	*/
}

BOOL CIISWeb::DisableHttp(LPCTSTR sComment, LPCTSTR sSerIp /*= _T("*")*/, int nPort /*= 80*/)
{
	//添加站点HTTP绑定
	CString szExe;
	CString szParam;
	//C:\Windows\System32\inetsrv\appcmd.exe
	TCHAR tcSystemWindowsDirectory[MAX_PATH] = { 0 };
	GetSystemWindowsDirectory(tcSystemWindowsDirectory, MAX_PATH);
	szExe.Format(_T("%s\\System32\\inetsrv\\appcmd.exe"), tcSystemWindowsDirectory);
	szParam.Format(_T(" set site \"%s\" /-bindings.[protocol='http',bindingInformation='%s:%d:'] "), sComment, sSerIp, nPort);
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	return TRUE;
}

BOOL CIISWeb::EnableHttp(LPCTSTR sComment, LPCTSTR sSerIp /*= _T("*")*/, int nPort /*= 80*/)
{
	//删除站点HTTP绑定
	CString szExe;
	CString szParam;
	//C:\Windows\System32\inetsrv\appcmd.exe
	TCHAR tcSystemWindowsDirectory[MAX_PATH] = { 0 };
	GetSystemWindowsDirectory(tcSystemWindowsDirectory, MAX_PATH);
	szExe.Format(_T("%s\\System32\\inetsrv\\appcmd.exe"), tcSystemWindowsDirectory);
	szParam.Format(_T(" set site \"%s\" /+bindings.[protocol='http',bindingInformation='%s:%d:'] "), sComment, sSerIp, nPort);
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	return TRUE;
}

BOOL CIISWeb::DisableHttps(LPCTSTR sComment, LPCTSTR sSerIp /*= _T("*")*/, int nPort /*= 443*/)
{
	//删除站点HTTPS绑定
	CString szExe;
	CString szParam;
	//C:\Windows\System32\inetsrv\appcmd.exe
	TCHAR tcSystemWindowsDirectory[MAX_PATH] = { 0 };
	GetSystemWindowsDirectory(tcSystemWindowsDirectory, MAX_PATH);
	szExe.Format(_T("%s\\System32\\inetsrv\\appcmd.exe"), tcSystemWindowsDirectory);
	szParam.Format(_T(" set site \"%s\" /-bindings.[protocol='https',bindingInformation='%s:%d:'] "), sComment, sSerIp, nPort);
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	return TRUE;
}

/*
:: AppCmd.exe is located in the  directory.
:: Because it is not path of the PATH automatically, you need to use the full
:: path to the executable when executing commands like in %systemroot%\system32\inetsrv\AppCmd.exe list sites.
:: Alternatively, you can manually add the inetsrv directory to the path on your machine so that you can access AppCmd.exe directly from any location.
cd C:\Windows\System32\inetsrv
:: OR
cd %systemroot%\system32\inetsrv\

certutil -f -p "password" -importpfx "C:\temp\mycert.pfx"

:: The thumbprint is gained by installing the certificate, going to cert manager > personal, clicking on it, then getting the Thumbprint.
:: Be careful copying the thumbprint. It can add hidden characters, esp at the front.
:: appid can be any valid guid
netsh http add sslcert ipport=0.0.0.0:443 certhash=5de934dc39cme0234098234098dd111111111115 appid={75B2A5EC-5FD8-4B89-A29F-E5D038D5E289}

:: bind to all ip's with no domain. There are plenty of examples with domain binding on the web
appcmd set site "Default Web Site" /+bindings.[protocol='https',bindingInformation='*:443:']
*/
BOOL CIISWeb::EnableHttps(LPCTSTR sComment, LPCTSTR sCertPath, LPCTSTR sCertPassword, LPCTSTR sCertHash, LPCTSTR sSerIp /*= _T("*")*/, int nPort /*= 443*/)
{
	//1.导入证书
	CString szExe = _T("certutil");
	CString szParam;
	szParam.Format(_T(" -f -p \"%s\" -importpfx \"%s\" "), sCertPassword, sCertPath);
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	//2.配置ssl证书到IIS
	szExe = _T("netsh");
	szParam.Format(_T(" http add sslcert ipport=0.0.0.0:%d certhash=%s appid=%s "), nPort, sCertHash, CCommonFun::NewGuid());
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	//3.添加站点HTTPS绑定
	//C:\Windows\System32\inetsrv\appcmd.exe
	TCHAR tcSystemWindowsDirectory[MAX_PATH] = { 0 };
	GetSystemWindowsDirectory(tcSystemWindowsDirectory, MAX_PATH);
	szExe.Format(_T("%s\\System32\\inetsrv\\appcmd.exe"), tcSystemWindowsDirectory);
	szParam.Format(_T(" set site \"%s\" /+bindings.[protocol='https',bindingInformation='%s:%d:'] "), sComment, sSerIp, nPort);
	CCommonFun::RunProcess(szExe, szParam, INFINITE, SW_HIDE);

	return TRUE;
}

void CIISWeb::CreateAdminLnk(CString szSerIp/* = _T("")*/, CString szSerPort/* = _T("80")*/)
{
	CString szURL;
	szURL.Format(_T("http://localhost:%s/%s/Login.aspx"),szSerPort,VIRTUALNAME);
	theLog.Write(_T("CIISWeb::CreateAdminLnk,szSerIp=[%s],szSerPort=[%s],szURL=[%s]"), szSerIp, szSerPort, szURL);
	TCHAR   path[255];
	SHGetSpecialFolderPath(0,path,CSIDL_DESKTOPDIRECTORY,0); 
	CString fpath;
	fpath.Format(_T("%s\\打印系统查询.url"), path);
	CString iconPath;
	iconPath.Format(_T("%s\\bin\\app.ico"), g_sDefaultPath);

	CCommonFun::CreateURLLink(szURL, fpath, iconPath);
}

void CIISWeb::RemoveAdminLnk()
{
	TCHAR   path[255];
	SHGetSpecialFolderPath(0,path,CSIDL_DESKTOPDIRECTORY,0); 
	CString fpath;
	fpath.Format(_T("%s\\打印系统查询.url"), path);
	m_fun.WriteFileLog(_T("删除web快捷方式：%s"), fpath);

	if(PathFileExists(fpath))
		CFile::Remove(fpath);
	m_fun.WriteFileLog(_T("RemoveAdminLnk over"));
}

