#pragma once

class CUsbMinHuaCardReader
{
public:
	CUsbMinHuaCardReader(void);
	virtual ~CUsbMinHuaCardReader(void);

	BOOL ReadUsbCard(CString &szCardNo);	//������USB��

	void Beep();
protected:
	BOOL Init();
	void Release();

protected:
	BOOL m_bInit;
	long m_lBaund;	//������
	HANDLE m_hICDev;
	int m_nPort;	//�˿�
	int m_nOpMode;
};
