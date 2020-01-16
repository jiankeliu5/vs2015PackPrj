// Ado.cpp: implementation of the CAdo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ado.h"
//

#define CONECTTIMOUT         600

#define COMMITTIMOUT         600

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//                                                       //
//      PrintProviderError Function                      //
//                                                       //
///////////////////////////////////////////////////////////

void CAdo::PrintProviderError(_ConnectionPtr pConnection)
{
    // Print Provider Errors from Connection object.
    // pErr is a record object in the Connection's Error collection.
	if( pConnection == NULL )
		return;
    ErrorPtr    pErr  = NULL;
	CString sError=_T("");
    if( (pConnection->Errors->Count) > 0)
    {
        long nCount = pConnection->Errors->Count;
        // Collection ranges from 0 to nCount -1.
        for(long i = 0;i < nCount;i++)
        {
            pErr = pConnection->Errors->GetItem(i);
			CString sTmp;
			//sTmp.Format(_T("\r\n\t Error number: %x\t%s"), pErr->Number,
            //    pErr->Description);
			//sError += sTmp;
        }
    }

   //WriteFileLog(sError);
}

///////////////////////////////////////////////////////////
//                                                       //
//      PrintComError Function                           //
//                                                       //
///////////////////////////////////////////////////////////

void CAdo::WriteFileLog(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	CString sTmp;
	sTmp.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteFileLog(sTmp);
}
//extern BOOL g_bIsWriteLog;
void CAdo::WriteFileLog(CString sLog)
{
	if( sLog.GetLength() > 250 )
		TRACE(sLog.Left(250));
	else
		TRACE(sLog);
	TRACE("\n");

	if (!CFileLog::IsDebug())
	{
		return ;
	}
//	if( !g_bIsWriteLog )
//		return;

	CString sSysDir = CCommonFun::GetLogDir();

	CFileFind fd;

	if( !fd.FindFile(sSysDir) )
	{
		if( !::CreateDirectory(sSysDir,NULL) )
		return;
	}

	CTime time = CTime::GetCurrentTime();
	CString sDate = time.Format(_T("%y%m%d"));
	
	CString sFileName;
	sFileName.Format(_T("%s\\ADO_%s.Log"),sSysDir,sDate);
	
	
	try
	{
		CFile fileLog;
		if(!fileLog.Open(sFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
			return;
		
		CString sTime = time.Format( _T("%Y-%m-%d %H:%M:%S") );
		CString sTmp;
		
		sTmp.Format(_T("%s,	 %s"),sTime,sLog);
		sTmp += _T("\r\n");
		fileLog.SeekToEnd();
		fileLog.Write(sTmp,sTmp.GetLength());
		fileLog.Close();
	}
	catch (...) 
	{
		;
	}

}

void CAdo::PrintComError(_com_error &e)
{
   _bstr_t bstrSource(e.Source());
   _bstr_t bstrDescription(e.Description());

   CString sError;
    // Print COM errors. 
   sError.Format(_T("\r\nError\r\n,\tCode = %08lx\r\n,\tCode meaning = %s\r\n,")
			_T("\tSource = %s\r\n,\tDescription = %s\r\n,\tErrorSql = %s\r\n"),
			e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription, m_sErrorSql);


   WriteFileLog(sError);
}
//--------------------------------
//���ַ���ת���BSTR,�˺����Ƚ�Σ�գ���ʱͣ��
//--------------------------------
/*
BSTR CAdo::SToB(const char* pSrc)
{
	return _com_util::ConvertStringToBSTR(pSrc);
}
*/
// ����bInit=False,����Ҫ�ֶ��������ݿ�
CAdo::CAdo()
{
	try
	{
		m_pConn = NULL;
		m_pRs = NULL;
		m_sErrorSql = _T("");		
	}
	catch(_com_error &e)
	{
		PrintProviderError(m_pConn);
        PrintComError(e);
	}
}

CAdo::~CAdo()
{
	ReleaseConn();

}

BOOL CAdo::Connect(int nTimeOut/*=0*/)
{
	ReleaseConn();
	HRESULT hResult;

	try
    {		

		hResult = m_pConn.CreateInstance(__uuidof(Connection));

		if(nTimeOut)
			m_pConn->ConnectionTimeout = nTimeOut;	// Ĭ����15��

		hResult = m_pConn->Open(m_bConnStr,"","",adConnectUnspecified);
		//----modify 9.30---
		m_pConn->CursorLocation = adUseClient;
		//------------------
		
		m_pConn->CommandTimeout = CONECTTIMOUT;
	}
	catch(_com_error &e)
	{
        PrintComError(e);

		return FALSE;
    }
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAdo::GetRecordset(CString sSqlStr,_Recordset **pRs,int nOpenType)
{
	BSTR bsSql = sSqlStr.AllocSysString();
	BOOL b = GetRecordset(bsSql,pRs,nOpenType);
	SysFreeString(bsSql);
	return b;
}

CString CAdo::GetGoToPageSql(const CString &szTabName		//����
					   , const CString &szGetFields			//��Ҫ���ص���
					   , const CString &szWhere				//��ѯ����,(ע��:��Ҫ�ӵ���"where",�ڲ��Ѿ�����)
					   , const CString &szSortName			//������ֶ�
					   , bool bSortDesc						//��������,�Ƿ�Ϊ����
					   , int nPageSize						//ҳ�ߴ�,ÿҳ��������¼
					   , int nPageIndex						//ҳ�룬����Ҫ��ѯ�ڶ���ҳ������
					   , const CString &szSortName2In			//�ڶ��������ֶ�,����Ϊ��
					   , bool bSortDesc2					//�ڶ��������ֶ��Ƿ�Ϊ����
					   )	//add by zfq,�õ���ĳ�ֶν������򣬲�ѯĳ���е�ĳҳ��SQL���
{
	CString szSql, szTmp, szOrder;
	if(szTabName.IsEmpty() || szGetFields.IsEmpty() || szSortName.IsEmpty())
	{
		ASSERT(0);
		return szSql;
	}

	CString szSortName2;
	if(szSortName.CompareNoCase(_T("ID")) && szSortName2In.IsEmpty()) //�����ֶβ���ID,�ҵڶ��������ֶ�Ϊ��
	{//��ֹAccess��ѯselect top N *���ֲ��е�һ�����
		szSortName2 = _T("ID");
	}
	else
	{
		szSortName2 = szSortName2In;
	}

	CString szTmp2;
	if(FALSE == szSortName2.IsEmpty())
	{//�еڶ��������ֶ�
		szTmp2.Format(_T(", %s %s")
			, szSortName2
			, bSortDesc2 ? _T("desc") : _T("asc"));
	}

	if(bSortDesc)
	{//����
		szTmp = _T("< ( select min");
		szOrder = _T(" order by ") + szSortName + _T(" desc") + szTmp2;
	}
	else
	{//����
		szTmp = _T("> ( select max");
		szOrder = _T(" order by ") + szSortName + _T(" asc") + szTmp2;
	}//else ����

	if(1 == nPageIndex)
	{//����ǵ�һҳ��ִ�����������ӿ�ִ���ٶ�
		if(szWhere.IsEmpty())
		{//������������Ϊ��
			szSql.Format(_T("select top %d %s from %s %s"), nPageSize, szGetFields, szTabName, szOrder);
		}
		else
		{
			szSql.Format(_T("select top %d %s from %s where %s %s "), nPageSize, szGetFields, szTabName, szWhere, szOrder);
		}
	}
	else
	{//���ǵ�һҳ
		if(szWhere.IsEmpty())
		{
			szSql.Format(_T("select top %d %s from %s where %s %s (%s) from ( select top %u %s from %s %s ) as tabTmp) %s ")
				, nPageSize
				, szGetFields
				, szTabName
				, szSortName
				, szTmp
				, szSortName
				, (nPageIndex - 1) * nPageSize
				, szSortName
				, szTabName
				, szOrder
				, szOrder);
			/*
			set @strSQL = 'select top ' + str(@PageSize) +' '+@strGetFields+ '  from '
			+ @tblName + ' where ' + @fldName + '' + @strTmp + '('+ @fldName + ') from (select top ' + str((@PageIndex-1)*@PageSize) + ' '+ @fldName + ' from ' + @tblName + '' + @strOrder + ') as tblTmp)'+ @strOrder
			*/
		}//if(szWhere.IsEmpty())
		else
		{//szWhere
			szSql.Format(_T("select top %d %s from %s where %s %s (%s) from (select top %u %s from %s where %s %s) as TabTmp) and %s %s ")
				, nPageSize
				, szGetFields
				, szTabName
				, szSortName
				, szTmp
				, szSortName
				, (nPageIndex - 1) * nPageSize
				, szSortName
				, szTabName
				, szWhere
				, szOrder
				, szWhere
				, szOrder);
			/*
			set @strSQL = 'select top ' + str(@PageSize) +' '+@strGetFields+ '  from '
			+ @tblName + ' where ' + @fldName + '' + @strTmp + '('
			+ @fldName + ') from (select top ' + str((@PageIndex-1)*@PageSize) + ' '
			+ @fldName + ' from ' + @tblName + ' where ' + @strWhere + ' '
			+ @strOrder + ') as tblTmp) and ' + @strWhere + ' ' + @strOrder
			*/
		}//else FALSE == szWhere.IsEmpty()
	}// else 1 != nPageIndex

	return szSql;
}


BOOL CAdo::GetRecordset(BSTR sSqlStr,_Recordset **pRs,int nOpenType)
{

	m_sErrorSql = CString(sSqlStr);
	WriteFileLog(m_sErrorSql);
	if( CString(sSqlStr).IsEmpty() ) 
		return FALSE;

	_RecordsetPtr pTempRs = NULL;
	BOOL bRet = FALSE;
	
	try
	{		
		if( m_pConn == NULL )
		{
			return FALSE;
		}

		if( m_pConn->State != adStateOpen )
		{
			Reconnect();
			bRet = FALSE;
		}
		else
		{
			pTempRs.CreateInstance( __uuidof(Recordset) );
			switch( nOpenType )
			{
			case ADO_OPEN_READONLY:
				pTempRs->Open((_bstr_t)sSqlStr,
							(IDispatch *)m_pConn,
							adOpenStatic,
							adLockReadOnly,
							adCmdText);			
				break;
			case ADO_OPEN_ADDNEW:
				pTempRs->Open((_bstr_t)sSqlStr,
							(IDispatch *)m_pConn,
							adOpenKeyset,
							adLockOptimistic,
							adCmdTable);
				break;
			case ADO_OPEN_UPDATE:
				pTempRs->Open((_bstr_t)sSqlStr,
							(IDispatch *)m_pConn,
							adOpenKeyset,
							adLockOptimistic,
							adCmdText);
				break;

			}
#if 0
			_StreamPtr	pStream ; //declare one first
			pStream.CreateInstance( __uuidof(Stream) ); //create it after
			pTempRs->Save( pStream.GetInterfacePtr(), adPersistADTG );    //old trick, call Save 
			int n = pStream->GetSize();
			//pStream->SaveToFile("c:\\a.xml",adSaveCreateNotExist);
			CComVariant vt = pStream->Read(n);
			vt.
			pStream->Close();
#endif		

			if( pRs != NULL )
			{
				*pRs = pTempRs.Detach();
			}
			else
			{
				pTempRs->Close();
			}
			bRet = TRUE;
		}

	}
	catch(_com_error  &e)
	{
		PrintProviderError(m_pConn);
        PrintComError(e);
		HRESULT hr= e.Error();
		DWORD dwErr = e.HRESULTToWCode(hr);
		bRet = FALSE;
		Reconnect();
	}
	catch(...)
	{
		bRet = FALSE;
	}

	return bRet;
}

void CAdo::SetConnStr(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort)
{
    CString sConnString=_T("");
	sConnString+=_T("Provider=sqloledb;");
	//bsConn+=_T("driver={SQL SERVER};");
	sConnString+=_T("Data Source=");
	//bsConn+=_T("server=");
	sConnString+=sDbSvr;
	//sConnString += _T(";Port=");
	sConnString += _T(",");
	sConnString += szDbPort;
	sConnString+=_T(";Initial Catalog=");
    //bsConn+=_T(";database=");
	sConnString+=sDbName;
	sConnString+=_T(";User Id=");
	sConnString+=sDbUser;
	sConnString+=_T(";Password=");
	sConnString+=sDbPwd;
	sConnString+=_T(";");


	m_bConnStr=sConnString;
// 	WriteFileLog(_T("connectstr = %s"),sConnString);
}

void CAdo::SetConnStr2(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort,int nDbType)
{
	CString sConnString=_T("");
	if (SQLSERVER == nDbType)
	{//MicroSoft Sql Server���ݿ�
		sConnString+=_T("Provider=sqloledb;");
		//bsConn+=_T("driver={SQL SERVER};");
		sConnString+=_T("Data Source=");
		//bsConn+=_T("server=");
		sConnString+=sDbSvr;
		//sConnString += _T(";Port=");
		sConnString += _T(",");
		sConnString += szDbPort;
		sConnString+=_T(";Initial Catalog=");
		//bsConn+=_T(";database=");
		sConnString+=sDbName;
		sConnString+=_T(";User Id=");
		sConnString+=sDbUser;
		sConnString+=_T(";Password=");
		sConnString+=sDbPwd;
		sConnString+=_T(";");
	}
	else if (ORACLE == nDbType)
	{//Oracle���ݿ�
		sConnString.Format(_T("Provider=OraOLEDB.Oracle;Password=%s;User ID=%s;")
			_T("Data Source=(DESCRIPTION =(ADDRESS = (PROTOCOL = TCP)(HOST = %s)(PORT = %s))")
			_T("(CONNECT_DATA =(SERVER = DEDICATED)(SERVICE_NAME = %s)));Persist Security Info=True")
			,sDbPwd,sDbUser,sDbSvr,szDbPort,sDbName);
	}
	else if (MYSQL == nDbType)
	{//MySql���ݿ�
		sConnString += _T("Driver={MySQL ODBC 5.1 Driver};Server=");
		sConnString += sDbSvr;
		sConnString += _T(";Database=");
		sConnString += sDbName;
		sConnString += _T(";User=");
		sConnString += sDbUser;
		sConnString += _T(";Port=");
		sConnString += szDbPort;
		sConnString += _T(";Password=");
		sConnString += sDbPwd;
		//�����ִ��
		sConnString += _T(";option=67108864");
		//����gb2312��ʽ
		sConnString += _T(";Charset=gb2312");
	}
	else if (ACCESS2003 == nDbType)
	{//Access(*.mdb)���ݿ� [office2003]
		//str = _T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source= D:\myweb\data\minspace.accdb")
		//str=_T("DBQ= D:\myweb\data\minspace.accdb;DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}")
		//2003:Provider=Microsoft.Jet.OLEDB.4.0;Data Source=XXX.mdb
		//2007:Provider=Microsoft.ACE.OLEDB.12.0;Data Source=XXX.accdb;
		sConnString += _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=\"");
		sConnString += sDbName + _T("\"");
		sConnString += _T(";Jet OLEDB:Database Password='");
		sConnString += sDbPwd;
		sConnString += _T("'");
	}
	else if (ACCESS2007 == nDbType)
	{//Access(*.accdb)���ݿ� [office2007���Ժ�]
		sConnString += _T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=\"");
		sConnString += sDbName + _T("\"");
		sConnString += _T(";Jet OLEDB:Database Password='");
		sConnString += sDbPwd;
		sConnString += _T("'");
	}
	else
	{
		WriteFileLog(_T("!!CAdo::SetConnStr2,nDbType=%d"),nDbType);
	}


	m_bConnStr=sConnString;
// 	WriteFileLog(_T("connectstr = %s"),sConnString);
}

void CAdo::SetConnStr3(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szInstanceName)
{
	CString sConnString=_T("");
	sConnString+=_T("Provider=sqloledb;");
	//bsConn+=_T("driver={SQL SERVER};");
	sConnString+=_T("Data Source=");
	//bsConn+=_T("server=");
	sConnString+=sDbSvr;
	//sConnString += _T(";Instance=");
	sConnString += _T("\\");
	sConnString += szInstanceName;
	sConnString+=_T(";Initial Catalog=");
	//bsConn+=_T(";database=");
	sConnString+=sDbName;
	sConnString+=_T(";User Id=");
	sConnString+=sDbUser;
	sConnString+=_T(";Password=");
	sConnString+=sDbPwd;
	sConnString+=_T(";");

	m_bConnStr=sConnString;
	// 	WriteFileLog(_T("CAdo::SetConnStr3 connectstr = %s"),sConnString);
}

void CAdo::SetConnStr(CString sConnStr)
{
   m_bConnStr=sConnStr;
}
void CAdo::SetAccessConnStr(CString sDBPath)
{	
	CString sConn;
	sConn = _T("DRIVER={Microsoft Access Driver (*.mdb)};");
	sConn += _T("DBQ=");
	sConn += sDBPath;	
	sConn += _T(";");	
	m_bConnStr=sConn;
}
void CAdo::SetTextConnStr(CString sPath)
{
	CString sConn = _T("Driver={Microsoft Text Driver (*.txt; *.csv)};");
    sConn += _T("DBQ=");
	sConn += sPath;
	sConn += _T(";");
    sConn += _T("DefaultDir=");
	sConn += sPath;
	sConn += _T(";") ;

   m_bConnStr=sConn;
}

BOOL CAdo::ExecuteSQL(CString sSQL)
{
	BSTR bsSql = sSQL.AllocSysString();
	BOOL b =  ExecuteSQL(bsSql);
	SysFreeString(bsSql);
	return b;
}
BOOL CAdo::ExecuteSQL(BSTR sSQL)
{
	if( CString(sSQL).IsEmpty() ) 
		return FALSE;
	m_sErrorSql = CString(sSQL);
	WriteFileLog(m_sErrorSql);

 	_CommandPtr comm;


	BOOL bRet = FALSE;
	try
    {
		if( m_pConn == NULL  )
		{
			return FALSE;
		}
		if( m_pConn->State != adStateOpen )
		{
			Reconnect();
			return FALSE;
		}
		HRESULT hResult=comm.CreateInstance(__uuidof(Command));
		if(SUCCEEDED(hResult))
		{  
			comm->ActiveConnection=m_pConn;
			comm->CommandTimeout=COMMITTIMOUT;
			comm->CommandText=sSQL;	
			
			comm->Execute(NULL,NULL,adCmdText);
			comm.Release();
			bRet = TRUE;
		}
		
	}
	catch(_com_error  &e)
	{
		PrintProviderError(m_pConn);
        PrintComError(e);
		bRet = FALSE;
		Reconnect();
	}
	catch(...)
	{
		bRet = FALSE;
	}
	
	return bRet;
}

BOOL CAdo::GetOneData(CString sSql, _variant_t &varData)
{
	BSTR bsSql = sSql.AllocSysString();
	BOOL b = GetOneData(bsSql,varData);
	SysFreeString(bsSql);
	return b;
}

BOOL CAdo::GetOneData(BSTR sSql, _variant_t &varData)
{
	varData.Clear();
	m_sErrorSql = CString(sSql);
	WriteFileLog(m_sErrorSql);

	BOOL bRet = FALSE;
	try
	{
		_RecordsetPtr pRs=NULL;
		if( !GetRecordset(sSql,&pRs) )
		{
			return FALSE;
		}
		if( !pRs->EndOfFile )
		{
			varData = pRs->Fields->GetItem(CComVariant(0))->Value;
			bRet = TRUE;
		}

		pRs->Close();
		pRs.Release();
	}
	catch(_com_error &e)
	{
		PrintProviderError(m_pConn);
        PrintComError(e);
		return FALSE;
	}
	return bRet;
}

BOOL CAdo::GetRecordset(CString sSql, BOOL bGetArray /* = FALSE */, int nOpenType /* = ADO_OPEN_READONLY */)
{
	BSTR bs = sSql.AllocSysString();
	BOOL b = GetRecordset(bs, bGetArray, nOpenType);
	SysFreeString(bs);
	return b;
}

BOOL CAdo::GetRecordset(BSTR sSql, BOOL bGetArray /* = FALSE */, int nOpenType /* = ADO_OPEN_READONLY */)
{
	m_sErrorSql = CString(sSql);

	try
	{
		if( m_pRs != NULL )
		{
			if( m_pRs->GetState() == adStateOpen )
			{
				m_pRs->Close();
			}
		}

		if( !GetRecordset(sSql,&m_pRs, nOpenType) )
		{
			return FALSE;
		}
		if (!bGetArray)
			return TRUE;

		int index = 0;
		m_nRow = m_pRs->RecordCount;
		_variant_t vt;
		while(!m_pRs->EndOfFile)
		{
			m_nColumn = m_pRs->Fields->GetCount();
			for(int i = 0; i < m_nColumn; i++)
			{
				vt.Clear();
				vt = m_pRs->Fields->GetItem(CComVariant(i))->Value;
				CString sTmp = GetVariantStr(vt);
				m_aRows.SetAtGrow(index++,sTmp);
			}
			m_pRs->MoveNext();
		}
	}
	catch(_com_error &e)
	{
		PrintProviderError(m_pConn);
        PrintComError(e);
		return FALSE;
	}
	
	return TRUE;
}


void CAdo::ReleaseConn()
{
	try
	{
		if( m_pConn != NULL )
		{
			try
			{
				if( m_pConn->State == adStateOpen )
				{
					m_pConn->Close();
					m_pConn = NULL;
					//m_pConn.Release();
				}
			}
			catch(_com_error &e)
			{
				PrintProviderError(m_pConn);
				PrintComError(e);
			}
		}
	}
	catch(...)
	{
		CString szMsg;
		szMsg.Format(_T("%d"),GetLastError());

	}

}

BOOL CAdo::Reconnect()
{
	ReleaseConn();
	return Connect();
}


void CAdo::SetExcelConnstr(CString sPathName)
{
	/*CString sConnStr = _T("Provider=Microsoft.Jet.OLEDB.4.0;");
	sConnStr += _T("Data Source=");
	sConnStr += sPathName;	
	sConnStr += _T(";Extended Properties=Excel 8.0;");
	*/
	CString sConnStr = _T("Provider=MSDASQL; Driver={Microsoft Excel Driver (*.xls)};DBQ=");
	sConnStr += sPathName;
	sConnStr += _T(";ReadOnly=False;");

	m_bConnStr=sConnStr;
}

CString CAdo::GetOneData(CString szSql)
{
	_variant_t varData;
	BSTR bsSql = szSql.AllocSysString();
	GetOneData(bsSql,varData);
	::SysFreeString(bsSql);
	varData.ChangeType(VT_BSTR);
	return CString(varData.bstrVal);
}

BOOL CAdo::GetRecordset(CString sSQL,CStringArray& RsAry,long &lRow,long &lCol)
{
	lRow = -1;
	lCol = -1;
	RsAry.RemoveAll();

	_RecordsetPtr pRs = NULL;
	try
	{
		BOOL bRet = GetRecordset(sSQL,&pRs);
		if(!bRet || pRs == NULL)
			return false;
		
		lCol = pRs->Fields->GetCount();
		lRow = pRs->RecordCount;
		
		if(lRow>0)
		{
			_variant_t tempvariant = pRs->GetRows(lRow);////��һά����
			
			RsAry.SetSize(lRow * lCol);
			int nIdx = 0;
			VARIANT result;
			long rgIndices[2];
			for(int i=0;i<lRow;i++)
			{
				for(int j=0;j<lCol;j++)
				{
					rgIndices[0] = j; 
					rgIndices[1] = i;
					HRESULT hr1= SafeArrayGetElement(tempvariant.parray, 
						rgIndices, &result);
					if(result.vt == VT_NULL || result.vt == VT_EMPTY)
						RsAry.SetAt(nIdx++,_T(""));
					else
					{
						VariantChangeType (&result, &result, 0, VT_BSTR);
						RsAry.SetAt(nIdx++,CString(result.bstrVal));
					}
					VariantClear(&result);
				}
			}	
		}
		else
		{
			lRow = lCol = 0;//�ն�ά����
		}
	}
	catch (_com_error &e)
	{
		PrintComError(e);
	}
	if(pRs)
	{
		pRs->Close();
		pRs = NULL;
	}
	return true;
}

BOOL CAdo::GetAry(CString sSQL, CStringArray &ary)
{
	long nRow = 0;
	long nCol = 0;
	if(GetRecordset(sSQL,ary,nRow,nCol))
	{
		return ary.GetSize() > 0 ? TRUE : FALSE;
	}
	return FALSE;
}
BOOL CAdo::GetRow(CStringArray& RowValues, int row /* = 0 */)
{
	CString str;

	try
	{
		//RowValues.SetSize(m_nColumn);
		int i = row * m_nColumn;
		if (row >= m_nRow || row < 0)
			return FALSE;
		for (int j = 0; j < m_nColumn; j++)
		{
			str = m_aRows.GetAt(i + j);
			//RowValues.Add(str);
			RowValues.SetAtGrow(j, str);
		}
	}
	catch(_com_error &e)
	{
		PrintProviderError(m_pConn);
		PrintComError(e);
	}
	catch (...)
	{
		TRACE("ERRor\n");
	}
	return TRUE;
}
int CAdo::GetRowCount()const
{
	return m_nRow;
}
int CAdo::GetColumnCount()const
{
	return m_nColumn;
}

//�õ���ǰʱ��
CString CAdo::GetDate()
{
	CString str;
	_RecordsetPtr pTempRs = NULL;
	try
	{		
		if( m_pConn == NULL )
		{
			return  _T("");
		}

		if( m_pConn->State != adStateOpen )
		{
			Reconnect();
			return _T("");
		}
		else
		{
			pTempRs.CreateInstance( __uuidof(Recordset) );

			pTempRs->Open(/*(_bstr_t)sSqlStr*/"select getdate()",
				(IDispatch *)m_pConn,
				adOpenStatic,
				adLockReadOnly,
				adCmdText);			

			_variant_t vt;
			if (!pTempRs->EndOfFile)
			{				
				vt.Clear();
				vt = pTempRs->Fields->GetItem(CComVariant(0))->Value;
				str = GetVariantStr(vt);
				//TRACE(str);
				//TRACE("\n");
			}
			pTempRs->Close();

		}

	}
	catch(_com_error  &e)
	{
		PrintProviderError(m_pConn);
		PrintComError(e);
		HRESULT hr= e.Error();
		DWORD dwErr = e.HRESULTToWCode(hr);
		str = _T("");
		Reconnect();
	}
	catch(...)
	{
		str = _T("");
	}

	return str;
}

BOOL CAdo::GetLinearSA(CString sSql, CStringArray& LinearValues)
{
	try
	{
		GetRecordset(sSql, TRUE);
		//LinearValues.SetSize(10, 10);
		for (int j = 0; j < m_nRow * m_nColumn; j++)
		{

			//RowValues.Add(str);
			LinearValues.SetAtGrow(j, m_aRows.GetAt(j));
			//TRACE(LinearValues.GetAt(j));
			//	TRACE("   ");F
		}
	}
	catch(_com_error &e)
	{
		PrintProviderError(m_pConn);
		PrintComError(e);
	}
	catch (...)
	{
		TRACE("ERRor\n");
	}
	return TRUE;
}

CString CAdo::GetVariantStr(_variant_t varValue)
{
	if( varValue.vt == VT_NULL || varValue.vt == VT_EMPTY )
		varValue.SetString("");

	varValue.ChangeType(VT_BSTR);
	return CString(varValue.bstrVal);
}

int  CAdo::GetVariantInt(_variant_t varValue)
{
	return _tstoi(GetVariantStr(varValue));
}
CString CAdo::GetOneString(CString sSql)
{
	_variant_t vt;
	GetOneData(sSql, vt);
	return GetVariantStr(vt);
}
int CAdo::GetOneInt(CString sSql)
{
	_variant_t vt;
	if (GetOneData(sSql, vt))
		return GetVariantInt(vt);
	return INT_MAX;
}

CString CAdo::GetGoToPageSql2(int  nClsID,CString sClsName,int nStuID,int nPageSize,int nPageIndex)	
{	
	CString szSql;
	CString szTmp = _T("< ( select min");
	CString	szOrder = _T(" order by ResName desc");

	if(1 == nPageIndex)
	{
		//����ǵ�һҳ��ִ�����������ӿ�ִ���ٶ�
		szSql.Format(_T("select top %d a.* from t_ShareRes a left join t_ResControl b on a.id = b.ResId where a.ClsID = %d and a.ClsName = '%s' and b.StuID = %d %s"), nPageSize,nClsID,sClsName,nStuID,szOrder);
	}
	else
	{
		szSql.Format(_T("select top %d a.* from t_ShareRes a left join t_ResControl b on a.id = b.ResId where ResName %s (ResName) from (select top %u a.ResName from t_ShareRes a left join t_ResControl b on a.id = b.ResId where ClsID = %d and a.ClsName = '%s' and b.StuID = %d %s) as TabTmp) and a.ClsID = %d and a.ClsName = '%s' and b.StuID = %d %s"),
			nPageSize,szTmp,(nPageIndex-1)*nPageSize,nClsID,sClsName,nStuID,szOrder,nClsID,sClsName,nStuID,szOrder);
	}
	return szSql;
}

BOOL CAdo::GetXml()
{
// 	CString sSqlStr;
// 	sSqlStr.Format(_T("select * from t_c_EquipMent for xml auto"));
// 
// 	m_sErrorSql = CString(sSqlStr);
// 	WriteFileLog(m_sErrorSql);
// 	if( CString(sSqlStr).IsEmpty() ) 
// 		return FALSE;
// 
// 	_CommandPtr pTempCmd = NULL;
// 	BOOL bRet = FALSE;
// 
// 	try
// 	{		
// 		if( m_pConn == NULL )
// 		{
// 			return FALSE;
// 		}
// 
// 		if( m_pConn->State != adStateOpen )
// 		{
// 			Reconnect();
// 			bRet = FALSE;
// 		}
// 		else
// 		{
// 			
// 			_variant_t varOptional(DISP_E_PARAMNOTFOUND,VT_ERROR);  
// 
// 			_StreamPtr stream; 
// 			stream.CreateInstance("ADODB.Stream"); 
// 			stream->raw_Open(varOptional, adModeUnknown, adOpenStreamUnspecified,NULL,NULL); 
// 			stream->put_Type(adTypeText); 
// 			_CommandPtr    Cmd1; 
// 			Cmd1.CreateInstance( __uuidof( Command ) ); 
// 			Cmd1->ActiveConnection = m_pConn; 
// 			Cmd1->CommandText      = _bstr_t("Select * from t_c_EquipMent for XML Auto"); 
// 			PropertiesPtr props; 
// 			props=Cmd1->GetProperties(); 
// 			PropertyPtr prop; 
// 			prop = props->Item[0]; 
// 			prop->PutValue(_variant_t("root")); 
// 			prop =props->Item; 
// 			prop->PutValue(_variant_t((IDispatch *) stream, true)); 
// 			Cmd1->Execute(&varOptional,&varOptional,adExecuteStream); 
// 			stream->SaveToFile("test.xml",adSaveCreateOverWrite); 
// 			printf("save stream to file finished\n"); 
// 			stream->Close(); 
// 			stream->raw_Write()
// 			
// 			bRet = TRUE;
// 		}
// 
// 	}
// 	catch(_com_error  &e)
// 	{
// 		PrintProviderError(m_pConn);
// 		PrintComError(e);
// 		HRESULT hr= e.Error();
// 		DWORD dwErr = e.HRESULTToWCode(hr);
// 		bRet = FALSE;
// 		Reconnect();
// 	}
// 	catch(...)
// 	{
// 		bRet = FALSE;
// 	}

	return 1;
}

BOOL CAdo::IsConnected()
{
	BOOL bRet = FALSE;
	try
	{
		if( m_pConn != NULL )
		{
			try
			{
				if( m_pConn->State == adStateOpen )
				{
					bRet = TRUE;
				}
			}
			catch(_com_error &e)
			{
				PrintProviderError(m_pConn);
				PrintComError(e);
			}
		}
	}
	catch(...)
	{
		CString szMsg;
		szMsg.Format(_T("%d"),GetLastError());

	}
	return bRet;
}

//ִ�в�ѯ���
//insert into t_test values ('xx') select ? = SCOPE_IDENTITY()
BOOL CAdo::ExecuteSQL(CString sSQL,int& nID)
{
	_CommandPtr comm;
	BOOL bRet = FALSE;
	try
	{
		if( m_pConn == NULL  )
		{
			return FALSE;
		}
		if( m_pConn->State != adStateOpen )
		{
			Reconnect();
			return FALSE;
		}
		HRESULT hResult=comm.CreateInstance(__uuidof(Command));
		if(SUCCEEDED(hResult))
		{  
			
			CComVariant vxxx;
			vxxx.vt = VT_INT;
			vxxx.intVal = nID;

			_ParameterPtr pParam1;
			pParam1 = comm->CreateParameter(_bstr_t ("pParam1"), 
				adInteger,adParamOutput, sizeof(int),( VARIANT ) vxxx);
			comm->Parameters->Append(pParam1);
			comm->ActiveConnection=m_pConn;
			comm->CommandTimeout=COMMITTIMOUT;
			comm->CommandText=_bstr_t(sSQL);	

			comm->Execute(NULL,NULL,adCmdText);
			
			vxxx = comm->Parameters->GetItem(CComVariant(0))->GetValue();
			nID = vxxx.intVal;

			comm.Release();
			bRet = TRUE;
			
		}

	}
	catch(_com_error  &e)
	{
		ASSERT(0);
		PrintProviderError(m_pConn);
		PrintComError(e);
		bRet = FALSE;
		Reconnect();
	}
	catch(...)
	{
		bRet = FALSE;
	}

	return bRet;
}