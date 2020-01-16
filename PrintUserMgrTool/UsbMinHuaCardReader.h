#pragma once

class CUsbMinHuaCardReader
{
public:
	CUsbMinHuaCardReader(void);
	virtual ~CUsbMinHuaCardReader(void);

	BOOL ReadUsbCard(CString &szCardNo);	//主动读USB卡

	void Beep();
protected:
	BOOL Init();
	void Release();

protected:
	BOOL m_bInit;
	long m_lBaund;	//波特率
	HANDLE m_hICDev;
	int m_nPort;	//端口
	int m_nOpMode;
};
