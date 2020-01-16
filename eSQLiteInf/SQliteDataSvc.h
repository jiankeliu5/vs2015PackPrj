
#pragma once
#include "../Public/Include/sqlite3.h"
#include "../Public/include/SystemDef.h"
// 此类是从 SQliteDataSvc.dll 导出的
// typedef int (*hardware_callback)(int,double,const TCHAR*,const void*,int);
// typedef int (*software_callback)(double ,const TCHAR*,const void*,int);
// typedef int (*log_callback)(int,const void*,int);

class _AA_DLL_EXPORT_ CSQliteDataSvc 
{
protected:
	CSQliteDataSvc(void);
public:
	~CSQliteDataSvc();
	static CSQliteDataSvc& GetInstance();
	BOOL ExecuteSQL(TCHAR* szSQL);
	BOOL Insert(TCHAR* szSQL);
	BOOL GetRecordStream(TCHAR* szSQL,isafetec::datastream& ary);
	BOOL GetRecordStream(TCHAR* szSQL,BYTE*& pBuf,int &nBufSize);
	
	void InitConnt(TCHAR* szDbFile /*= NULL*/,TCHAR* szPwd = NULL,DWORD dwTimeout = 10 * 1000);
	BOOL Connect();
	void Release();
	BOOL CreateDB(TCHAR* szDb);
	
public:
	BOOL GetSimplePolicy(POLICY_TYPE type,SimplePolicyList& list);
	BOOL CheckSimplePolicy(SimplePolicyList* list,SimplePolicyList& rs,SimplePolicyList& rs2);
	
	//更新策略
	BOOL UpdatePolicy(int nId,int nType, double time,BYTE* pData,int nSize);
	BOOL GetPolicy(int nId,BYTE*& pData,int& nSize);
	
	// 资产信息的处理
	BOOL UpdateAssets(SYSTEM_INFO_TYPE eType,double time,BYTE* pData,int nSize,int nUploadSuc = 1);
	BOOL GetAssets(SYSTEM_INFO_TYPE eType,BYTE*& pData,int &nSize,BOOL& bUpSucced);
	BOOL CleanAssets();

	// 日志的处理
	BOOL InsertLog(LOG_TYPE eType,BYTE* pData,int nLen);
	BOOL InsertLog(LOG_TYPE eType,BYTE* pData,int nLen,int &jobId);
	
	//获取数据库中没有上传的日志
	BOOL PopLog(ICltLogCallback* pCall,LOG_TYPE eType = NULL_LOG,void* context = NULL);
	BOOL PopOneLog(ICltLogCallback* pCall,LOG_TYPE eType = NULL_LOG,void* context = NULL);
	BOOL PopOnePrintLog(ICltLogCallback* pCall,int nid,void* context = NULL);
	BOOL QueryLog(ICltLogCallback* pCall,LOG_TYPE eType= NULL_LOG);
	BOOL GetOnePrintLog(SubmitPrintJob* job,int &nId);
	BOOL DelLog(LOG_TYPE eType,int nId = 0);
	int GetLogCount(LOG_TYPE eType = NULL_LOG);

	BOOL SaveConfig(CString sName,CString sVal,BYTE* pData,int nSize);
	BOOL GetConfigVal(CString sName,CString& sVal);
	BOOL GetConfigData(CString sName,BYTE*& pData,int& nSize);
	BOOL GetConfigData(CString sName,CBuffer* pBuf);
	BOOL GetProductInfo(CString sName, ProductInfo* pInfo);
	BOOL GetEquipInfo(EQUIP_INFO* pInfo);
private:
	sqlite3*	m_hDB;
	HANDLE		m_hMutex;
	DWORD		m_dwTimeout;
	TCHAR		m_szDbFile[MAX_PATH];
	TCHAR		m_szPwd[MAX_PATH];
	BOOL InitMutex();

};


// class _AA_DLL_EXPORT_ CSQliteBase
// {
// public:
// 	CSQliteBase();
// 	~CSQliteBase();
// 	virtual BOOL ExecuteSQL(TCHAR* szSQL);
// 	//	BOOL Insert(TCHAR* szSQL);
// 	// 	BOOL GetRecordStream(TCHAR* szSQL,isafetec::datastream& ary);
// 	// 	BOOL GetRecordStream(TCHAR* szSQL,BYTE*& pBuf,int &nBufSize);
// 
// 	//void InitConnt(TCHAR* szDbFile /*= NULL*/,TCHAR* szPwd = NULL,DWORD dwTimeout = 10 * 1000);
// 	virtual BOOL Connect();
// 	virtual void Release();
// 	virtual BOOL CreateDB(TCHAR* szDb);
// };

class _AA_DLL_EXPORT_ CSQliteMemDb //:public CSQliteBase
{
public:
	CSQliteMemDb();
	~CSQliteMemDb();
	BOOL Connect();
	void Release();
	
	BOOL ExecuteSQL(TCHAR* szSQL);
	
	void TestInsert();
	void TestSelect();
	BOOL GetOneData(TCHAR* szSQL,int& nRet);
	void InitDB(TCHAR* pBuf);
private:
	BOOL CreateDB(TCHAR* szSql);
	sqlite3*	m_hDB;
	HANDLE		m_hMutex;
	DWORD		m_dwTimeout;
	TCHAR		m_szDbFile[MAX_PATH];
	TCHAR		m_szPwd[MAX_PATH];
	TCHAR*		m_pInitDb;
	BOOL InitMutex();
};