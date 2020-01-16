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
	//֪ͨ�����߳�
	virtual int NotifyNetTh(DWORD dwCmd,WPARAM w,LPARAM l);
	//֪ͨ���縨���߳�
	virtual int NotifyNetHelperTh(DWORD dwCmd,WPARAM w,LPARAM l);
	//ͨ�����������̷߳�������
	virtual int SendByNetHelper(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress = -1 );
	//ͨ���������̷߳�������
	virtual int SendByNetTh(int nCmd,int nSub,BYTE* pData,int nSize,int nCompress = -1 );
	//��ѯ��ǰ�û���Ϣ
	virtual void QueryUserInfo(int& nUserId,int& nAccountId,CString& sName);
	//�ϴ���־��Ϣ����
	virtual void OnUploadLogResult(int type,BOOL bSuc,void* other = 0);
	//��ӡ���˽��
	BOOL OnJobFilterResult(SubmitPrintJob* job,JobFilterPolciy* policy);
	virtual void OnTipMsg(int nType,CString sMsg,DWORD dwParam,DWORD dwParam2);
	//���糬ʱ
	virtual void OnSocketTimeOut();
	//���е�ǰsession�Ľ������
	virtual BOOL RunUiProcess();
	//�����ύ��ӡ����
	virtual BOOL OnRequestSubmitPrintJob(int nJobId = 0);
	//��־�ϴ�
	virtual void OnUploadLog(int type,void* log);
	BOOL OnProduct(BYTE* pData,int nSize);
	BOOL OnUser(BYTE* pData,int nSize);
	BOOL OnPrintAccount(BYTE* pData,int nSize);
	BOOL OnPrinter(BYTE* pData,int nSize);
	BOOL OnPrinterPolicy(BYTE* pData,int nSize);

	//��ʼ�ύ��ҵ
	BOOL doSubmitPrintJob(int nJobId,int nPrtId);
	BOOL doSubmitPrintJob(BYTE* pData ,int nSize );
	
	BOOL IsNeedNotifyUi();
	BOOL IsPrintActive();
	BOOL IsNetInitSuccessed();
	BOOL IsEduPrint();
	//
	//�û���½�¼�
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
		//��ӡ����
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