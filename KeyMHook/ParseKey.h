#pragma once

typedef void (WINAPI *PFN_OnReadCardByKeyHook)(char *pCardNum, void *pContext);

class CKeyHkUdpSock;

//����������
class CParseKey
{
public:
	CParseKey(void);
	virtual ~CParseKey(void);

	void SaveKey(WPARAM wp);

//	void OnReadCardByKeyBoard(CString &szCardNum);
	void OnReadCardByKeyBoard(BYTE *pszCardNum);

	void SetCallBack(PFN_OnReadCardByKeyHook pfn, void *pContext);
protected:
	BOOL IsThisKeyValid(WPARAM wp);
	BOOL IsEndFlag(WPARAM wp);
protected:
//	CString		m_szKeyCard;
	BYTE		*m_pKeyCard;
	DWORD		m_dwTickCountLastInput;
	DWORD		m_dwMaxSpan;	//ˢ����ģ��������룬��������������������
	DWORD		m_dwMaxCount;	//���ŵ����λ��
	BOOL		m_bFirst;		//�Ƿ��ǵ�һ��

	PFN_OnReadCardByKeyHook m_pfnOnReadCard;
	void *		m_pContext;
	CKeyHkUdpSock *m_pUdpSock;
};
