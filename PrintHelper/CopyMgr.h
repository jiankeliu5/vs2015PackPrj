#pragma once

#include "SerialCommHelper.h"
#include <afxtempl.h>
#include "SimpleCry.h"

class CCopyInfo
{
public:
	CCopyInfo()
	{
		m_nUserID = 0;
		m_dt = 0;//ʱ��
		m_lCount = 0;
		m_fPrice = 0;
		m_eColor = WB_PAPER;
		m_ePaper = A4_PAPER;
		SYSTEMTIME st = {0};
		::GetLocalTime(&st);
		COleDateTime dt(st);
		m_dt = dt.m_dt;
		m_eFrom = CIF_HardWare;
		m_nJobid = 0;
		m_eCopyType = COPY_TYPE_ASYNCHRONOUS;
		m_eCopyStage = COPY_STAGE_IDLE;
		m_nPlex = DMDUP_SIMPLEX;
		m_szGuid = CCommonFun::NewGuid();
	}
	void CopyFrom(CCopyInfo& from)
	{
		m_dt = from.m_dt;
		m_lCount = from.m_lCount;
		m_fPrice = from.m_fPrice; 
		m_eColor = from.m_eColor;
		m_ePaper = from.m_ePaper;
		m_eFrom = from.m_eFrom;
		m_nJobid = from.m_nJobid;
		m_eCopyType = from.m_eCopyType;
		m_eCopyStage = from.m_eCopyStage;
		m_nPlex = from.m_nPlex;
	};

	int m_nUserID;
	double m_dt;
	long m_lCount;
	float m_fPrice;
	PAPER_COLOR m_eColor;
	PAPER_TYPE m_ePaper;
	COPY_INFO_FROM m_eFrom;
	int m_nJobid;
	COPY_TYPE m_eCopyType;//add by szy 20170520,�ο����Ͷ���˵��
	COPY_STAGE m_eCopyStage;//add by szy 20170522,�ο����Ͷ���˵��
	int m_nPlex;
	CString m_szGuid;
};

enum USER_STATE
{
	USER_ERR,
	USER_SUC
	
};

class _AA_DLL_EXPORT_ CCopyBakInfo
{
public:
	CCopyBakInfo()
	{
		m_nSize = 0;
		m_nColor = 1;
		m_nUnit = 1024;
		m_nPageCount = 0;
		m_fPrice = 0;
		m_bIsLastFile = FALSE;
		m_nJobID = 0;
		m_nUserID = 0;
	}
	CString m_sPath;

	CString m_sZipFilePath;
	CString m_sFileName;
	CString m_sDir;
	CString m_szGuid;

	int m_nSize;
	int m_nColor;
	int m_nPageCount;
	double m_dt;
	double m_fPrice;
	int m_nUnit;
	BOOL m_bIsLastFile;
	int m_nJobID;
	int m_nUserID;

	virtual BOOL Attach(CString& sPath);
protected:
	BOOL CompressTif();
};

class _AA_DLL_EXPORT_ CCopyStat : public ICopyCmdInterface
{
public:
	CCopyStat();
	~CCopyStat();
private:
	void Reset();
	CSerialCommHelper m_oHelper;
	//CList<CCopyInfo*,CCopyInfo*> m_oCopyList;
	CMap<UINT64,UINT64,CCopyInfo*,CCopyInfo*>	m_oCopyMap;
	long m_lUserLogonedSucTickCount;
	long m_lUserLogoned;
	long m_lInitSuc;
	long m_lActive;
	long m_lLogonTimeout;
	long m_lDelayTime;
	int m_nComIndex;
	long m_lTickCount;
	PAPER_TYPE m_eActiveType;
	PAPER_COLOR m_eActiveColor;
	long m_lBeginOnePaper;
	long m_lCmdResponse;
	long m_lCheckTickCount;
	long m_lLocked;
//	long m_lLastNotifyTickCount;
	DWORD m_dwLastNotifyTickCount;
	long m_nCopySpan;
	long m_lPowered;	//����״̬

	ICopyChargeInterface* m_pCallback;
	CRITICAL_SECTION m_csLock;

	inline void		InitLock()			{InitializeCriticalSection (&m_csLock );}
	inline void		DelLock()			{DeleteCriticalSection (&m_csLock );}
	
	HANDLE	m_hThreadTerm ;
	HANDLE	m_hThread;
	HANDLE	m_hThreadStarted;

	BYTE	m_szBrandCode[4];
	BYTE	m_szCheckCode[4];
	BYTE	m_szLockCode[4];
	BYTE	m_szUnLockCode[4];
	BYTE	m_szCheckHw[4];
	BYTE	m_szPowerOnCode[4];		//����
	BYTE	m_szPowerDownCode[4];	//ͣ��
	BRAND_TYPE m_eType;
protected:
	void OnInitSuc();
	void OnCheckStateSuc();
	void OnLockSuc(BOOL bLocked);
	void OnPowerSuc(BOOL bPowered);

	void OnPaperType(PAPER_TYPE e);
	void OnPaperColor(PAPER_COLOR e);
	void OnBeginOneCopy();
	void OnOneCopyCmplt();
	virtual void OnOneCopyCmplt(PAPER_TYPE,PAPER_COLOR);
	void OnErr();
	

	void CreateTh();
	
	void NowTickCount();
	BOOL IsIdleTimeOut();
	BOOL IsCmdResponseTimeOut();
	
	BOOL IsSameUser(void *pInfo,int nSize);
	BOOL IsCopying();
	
	void CheckState(BOOL& bSend);
	void InitHw();
	void LockHw(BOOL bLock);
	void OnIdleTimeout();
	float GetPrice(PAPER_TYPE eType,PAPER_COLOR eColor);
	static unsigned __stdcall ThreadFn(void*pvParam);

	
	void SendKey(BOOL& bSend);
	void SendData(BOOL& bSend);
	BYTE m_xor;
	BYTE m_add;
	//�ͷŸ�ӡ��¼
	void ReleaseList();
	void NotifyCopyData();
	BOOL IsCanNotify();
	//�ӳ�д�����ݵ���ӡ������
	void DelayWrite(DWORD dwMilliseconds = 500);
public:
	BOOL IsRunning();
	BOOL IsCopyBakRunning();
	BOOL IsInited();
	BOOL IsLocked();
	BOOL CheckHw();	//add by zxl,20140923,��⸴ӡ�������Ƿ�����
	//һ��ͨˢ���ӿڣ�ÿ�ζ����ô˽ӿڣ�
	//
	//void CheckUser(void *pInfo,int nSize);
	//��ʼ��ӡ
	void LogonUser();
	//��ӡ����
	void LogoffUser();
	//�û�����ʹ��
	BOOL IsUserLogoned();
	//�û���¼�ɹ�
	BOOL IsUserLogonedSuc();
	//�Ƿ񹩵�
	BOOL IsPowered();
	//�������
	void PowerHw(BOOL bPower);
	//����ϼƻ���
	void CalcAmount(float& fAmount);
	//
	void SetCallback(ICopyChargeInterface* pCall);
	void SetIdleDelay(long l);
	BOOL Init(long lIdle,int nComIndex);
	void Shutdown();
	void ShutdownCopyBak();

	//����Ʒ��
	void InitBrand(BRAND_TYPE eType);

	//���ÿ���ʱ��,������ڵ�ɨ��Ҳ��Ҫ�����Ĵ�ӡ��������
	void ReSetTimeCount();
	//��ӡ����
	BOOL StartCopyBak(CString sDir);
	void StopCopyBak();

private:
	HANDLE	m_hThreadTermCopyBak ;
	HANDLE	m_hThreadCopyBak;
	HANDLE	m_hThreadStartedCopyBak;
	long m_lCopyBakActive;
	static DWORD WINAPI ThreadFnCopyBak(void* pVoid);
	CString m_sDir;
	void ReadChanges2(CString sDir);
	void WriteToMemFile(FILE_NOTIFY_INFORMATION* pInfo);
};