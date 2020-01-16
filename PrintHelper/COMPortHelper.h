// COMPortHelper.h : interface for the CCOMPortHelper class.
// ICOMPortInterface������ҵ����COM���ڲ����Ļ����࣬
// ����Ĺ�������CInfraredInduction�����԰���һ��ICOMPortInterface��
// ��Ա����������дCOM���ںͽ������ݻص�
//////////////////////////////////////////////////////////////////////

#pragma once

class _AA_DLL_EXPORT_ ICOMPortInterface
{
public:
	virtual void OnByte(BYTE byte) = NULL;
	virtual void OnErr(DWORD dwError) = NULL;
};

class _AA_DLL_EXPORT_ CCOMPortHelper
{
public:
	CCOMPortHelper();
	virtual ~CCOMPortHelper();

	BOOL Open(int nComIndex);	//��Com�ӿ�
	void Close();				//�ر�Com�ӿ�
	void SetCallBack(ICOMPortInterface* pCall);
	void OnRead();
	BOOL Write(BYTE* pData, int nDataSize, DWORD dwTimeOut = 2000);

	static DWORD WINAPI ThreadFn(LPVOID lpParameter);

protected:
	void OnByte(BYTE byte);
	void OnErr(DWORD dwError);

private:
	DWORD m_dwLastError;
	HANDLE	m_hComPort;
	HANDLE m_hComEvent;
	HANDLE	m_hThreadStarted;
	HANDLE	m_hThreadTerm ;
	HANDLE	m_hThread;
	DWORD	m_dwThreadID;
	ICOMPortInterface* m_pCallback;
	CCriticalSection2 m_cs;
};

