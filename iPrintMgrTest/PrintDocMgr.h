#pragma once
#include "../Public/Include/PrintDocEvent.h"

typedef BOOL (*PFINSHOOK)(char*); 
typedef BOOL (*PFSTOPHOOK)(); 
class CPrintDocMgr :public IPipeCallback
{
public:
	CPrintDocMgr();
	~CPrintDocMgr();
	void Init(IModuleCallback* pCall);
	void Shutdown();
	void SetDocType(const char* szType);
	void Start();
	void Stop();
	static CPrintDocMgr& GetInstance();
protected:
	virtual void OnPipeRcv(void*,BYTE* pData,int nSize);
	virtual int OnNewPipe(void* );
	virtual void OnPipeClose(void*);

	BOOL UploadPrintDocLog(PrintDocumentInfo& log,BYTE* pData,int nDataLen);

	BOOL GetDocument(CString sPath,CString& sZipPath,int& nSize);
	void OnDocumentPrinted(CDocumentLogNode* node);
	void CheckBufSize(int nBufSize);
	BOOL OnUpload(int nCmd,int nSub,BYTE* pData,int nDataLen,int nCompress = -1);
	CPipeSvrThread* m_pPipeSvr;
	BYTE*				m_pBuf;
	int					m_nDatalen;
	IModuleCallback*	m_pCallback;
	HMODULE				m_hModule;
	PFINSHOOK			m_pfInstall;
	PFSTOPHOOK			m_pfStop;
	char				m_szDocType[1024];

};