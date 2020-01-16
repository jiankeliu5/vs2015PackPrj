#pragma once

class CXabPrinterMonitor;
class CPrintJobMgr : public ICltLogCallback
{
public:
	CPrintJobMgr(void);
	~CPrintJobMgr(void);

public:
	static CPrintJobMgr& GetInstance();

	virtual void Release();
	void StartPrinter();
public:
	static double GetNowDateTime();
	BYTE* GetBufPtr();

public:	
	//���ûص�����
	void SetCallback(IModuleCallback* pCall);
	//�����ϴ�
	BOOL IsUploadActOnce();
	// ��־�ϴ�
	virtual int OnLogUpload(int nId,int type,const void* pdata,int len,void* context);

protected:
	//������ʾ
	virtual void OnTipMsg(int,CString,DWORD,DWORD);
	//��ӡ��־
	virtual void OnPrinted(void*);
private:
	void CheckBufSize(int nBufSize);
	BOOL OnUpload(int nCmd,int nSub,BYTE* pData,int nDataLen,int nCompress = -1);
	BOOL UploadPrintLog(SubmitPrintJob& log);
private:
	IModuleCallback*	m_pCallback;
	CCriticalSection2 m_cs;
	BYTE*				m_pBuf;
	int					m_nDatalen;
	CXabPrinterMonitor* m_pPrinterMon;
};
