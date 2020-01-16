// Ado.h: interface for the CAdo class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

//��¼���Ĵ򿪷�ʽ
#define ADO_OPEN_READONLY				0									//��ֻ����ʽ�򿪼�¼��
#define ADO_OPEN_ADDNEW					1									//��ADDNew��ʽ�򿪼�¼��
#define ADO_OPEN_UPDATE					2									//��Update��ʽ�򿪼�¼��

typedef enum tag_DataBase_Type {SQLSERVER,ORACLE,MYSQL,ACCESS2003,ACCESS2007,REMOTE_AGENT} DB_TYPE;

class  CAdoInterface
{
public:
	virtual BOOL ExecuteSQL(CString sSQL) = 0;
	virtual BOOL GetRecordset(CString sSQL,CStringArray& ary,long &lRow,long &lCol) = 0;
	virtual CString GetOneData(CString sSQL) = 0;
};

class  CAdo : public CAdoInterface
{
public:
	_bstr_t m_bConnStr;
	CString m_sErrorSql;
	CAdo();
	virtual ~CAdo();
public:
	CString GetGoToPageSql(const CString &szTabName				//����
		, const CString &szGetFields = _T("*")	//��Ҫ���ص���
		, const CString &szWhere = _T("")		//��ѯ����,(ע��:��Ҫ�ӵ���"where",�ڲ��Ѿ�����)
		, const CString &szSortName = _T("")	//������ֶ�
		, bool bSortDesc = true					//��������,�Ƿ�Ϊ����
		, int nPageSize = 10					//ҳ�ߴ�,ÿҳ��������¼
		, int nPageIndex = 1					//ҳ�룬����Ҫ��ѯ�ڶ���ҳ������
		, const CString &szSortName2 = _T("")	//�ڶ��������ֶ�,����Ϊ��
		, bool bSortDesc2 = true				//�ڶ��������ֶ��Ƿ�Ϊ����
		);	//add by zfq,�õ���ĳ�ֶν������򣬲�ѯĳ���е�ĳҳ��SQL���
	CString GetGoToPageSql2(int  nClsID,CString sClsName,int nStuID,int nPageSize,int nPageIndex);
	void SetExcelConnstr(CString sPathName);
	BOOL Reconnect();
	void ReleaseConn();

	BOOL GetAry(CString sSQL,CStringArray &ary);
	BOOL GetRecordset(CString sSQL,CStringArray& ary,long &lRow,long &lCol);
	CString GetOneData(CString szSql);
	BOOL GetRecordset(CString sSql, BOOL bGetArray /*= FALSE*/, int nOpenType = ADO_OPEN_READONLY);
	BOOL GetRecordset(BSTR sSql, BOOL bGetArray /*= FALSE*/, int nOpenType = ADO_OPEN_READONLY);

	BOOL GetRecordset(CString sSqlStr,_Recordset **pRs,int nOpenType=ADO_OPEN_READONLY);
	BOOL GetRecordset(BSTR sSqlStr,_Recordset **pRs,int nOpenType=ADO_OPEN_READONLY);

	BOOL GetOneData(CString sSql,_variant_t &varData);
	BOOL GetOneData(BSTR sSql,_variant_t &varData);

	void PrintProviderError(_ConnectionPtr pConnection);
	void PrintComError(_com_error &e);
	static void WriteFileLog(LPCTSTR lpszFormat, ...);
	static void WriteFileLog(CString sLog);

	BOOL IsConnected();
	BOOL Connect(int nTimeOut=0);
	
	void SetConnStr(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort);
	void SetConnStr2(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szDbPort,int nDbType);
	void SetConnStr3(CString sDbSvr,CString sDbName,CString sDbUser,CString sDbPwd,CString szInstanceName);
	void SetConnStr(CString sConnStr);
	void SetTextConnStr(CString sPath);
	void SetAccessConnStr(CString sDBPath);

	BOOL ExecuteSQL(CString sSQL);
	BOOL ExecuteSQL(BSTR sSQL);
	BOOL ExecuteSQL(CString sSQL,int& nID);	

public:
	CString GetDate();

	BOOL GetXml();
	int  GetRowCount()const;
	int  GetColumnCount()const;

	BOOL GetRow(CStringArray& RowValues, int row = 0); 
	BOOL GetLinearSA(CString sSql, CStringArray& LinearValues);//m_pRs��ı䣡

	CString GetVariantStr(_variant_t varValue);
	int     GetVariantInt(_variant_t varValue);

	int GetOneInt(CString sSql);
	CString GetOneString(CString sSql);

private:
	CStringArray m_aRows;
	int    m_nColumn;
	int    m_nRow;

	_ConnectionPtr m_pConn;
	_RecordsetPtr m_pRs;
	_CommandPtr		m_pCmd;
};


