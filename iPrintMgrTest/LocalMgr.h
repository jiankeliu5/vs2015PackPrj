#pragma once
class CNetThHelper;
class CNetThread;
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
class CLocalThreadMgr : public IModuleCallback,ISvcCallback
{
public:
	CLocalThreadMgr();
	static CLocalThreadMgr& GetInstance();
	BOOL InitTh();
	void EndTh();
	//通知网络线程
	virtual int NotifyNetTh(DWORD dwCmd,WPARAM w,LPARAM l);
	//通知网络辅助线程
	virtual int NotifyNetHelperTh(DWORD dwCmd,WPARAM w,LPARAM l);
	//通过辅助网络线程发送数据
	virtual int SendByNetHelper(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress = -1 );
	//通过主网络线程发送数据
	virtual int SendByNetTh(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress = -1 );
	//查询当前用户信息
	virtual void QueryUserInfo(int& nUserId,int& nAccountId,CString& sName);
	//上传日志信息反馈
	virtual void OnUploadLogResult(int type,BOOL bSuc,void* other = 0);
	//打印过滤结果
	BOOL OnJobFilterResult(SubmitPrintJob* job,JobFilterPolciy* policy);
	virtual void OnTipMsg(int nType,CString sMsg,DWORD dwParam,DWORD dwParam2);
	//网络超时
	virtual void OnSocketTimeOut();
	//运行当前session的界面进程
	virtual BOOL RunUiProcess();
	//请求提交打印任务
	virtual BOOL OnRequestSubmitPrintJob(int nJobId = 0);
	//日志上传
	virtual void OnUploadLog(int type,void* log);
	BOOL OnProduct(BYTE* pData,int nSize);
	BOOL OnUser(BYTE* pData,int nSize);
	BOOL OnPrintAccount(BYTE* pData,int nSize);
	BOOL OnPrinter(BYTE* pData,int nSize);
	BOOL OnPrinterPolicy(BYTE* pData,int nSize);

	//开始提交作业
	BOOL doSubmitPrintJob(int nJobId,int nPrtId);
	BOOL doSubmitPrintJob(BYTE* pData ,int nSize );
	
	BOOL IsNeedNotifyUi();
	BOOL IsPrintActive();
	BOOL IsNetInitSuccessed();
	BOOL IsEduPrint();
	//
	//用户登陆事件
	virtual void OnSessionChange(DWORD dwEventType,LPVOID lpEventData);

	CString GetLmPrintSvcHostPath();
public:
	CCriticalSection2 m_cs;
	CNetThHelper* m_pHelperTh;
	CNetThread*   m_pMainTh;

	long			m_nCnntState;

	ISvcCallback* m_pSvcCallback;

	ProductInfo	m_oProduct;
	PrintCltCheckUserResult m_oPrtAccount;
	CltCheckUserResult m_oUser;
	PrintCtlReqInfo m_oPrint;
	PrintCtrlPolicy m_oPrtPolicy;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
};


class CCmdMgr
{
public:
	CCmdMgr();
	void Clear();
	
	static CCmdMgr& GetInstance();
	void doReqCmd();
	void OnResponseCmd(NET_PACK_HEAD* pHead);
	void doCmd(const OnlineCmd& cmd);
	long m_lReqCount;
};
#if 0
class CXabVPrinterMgr :public CFilterMgr
{
public:
	virtual BOOL Init()
	{
		//打印监视
		m_pPrinterMon = new CPrinterMonitor();
		ASSERT(m_pPrinterMon);
		if(m_pPrinterMon)
		{
			m_pPrinterMon->SetCallBack(this);
			m_pPrinterMon->StartMonitor(IST_PRINTER_NAME);
		}					
	}
};
#endif