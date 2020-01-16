#include "StdAfx.h"
#include "UsbMinHuaCardReader.h"
#include "mwrf32.h"

#pragma comment(lib, "mwrf32.lib")

CUsbMinHuaCardReader::CUsbMinHuaCardReader(void)
{
	m_bInit = FALSE;
	m_lBaund = 115200;
	m_nPort = 0;
	m_nOpMode = 0;
	m_hICDev = NULL;
}

CUsbMinHuaCardReader::~CUsbMinHuaCardReader(void)
{
	Release();
}

BOOL CUsbMinHuaCardReader::Init()
{
	__int16 st = 0;
	unsigned char _Status[30];
	memset(_Status,0,30);

	if(m_hICDev)
	{
		::rf_exit(m_hICDev);
	}
	m_hICDev = ::rf_init(m_nPort, m_lBaund);
	st = rf_get_status(m_hICDev, _Status);
	if(0 > m_hICDev || st)
	{
		if(m_hICDev)
		{
			theLog.Write("!!CUsbMinHuaCardReader::Init,1,rf_get_status FAIL,ST=%d,m_nPort=%d,m_lBaund=%d"
				, st, m_nPort, m_lBaund);
			::rf_exit(m_hICDev);
			return FALSE;
		}

		return FALSE;
	}
	else
	{
		::rf_beep(m_hICDev, 10);
		theLog.Write("##CUsbMinHuaCardReader::Init,2,rf_get_status OK,ST=%d,m_nPort=%d,m_lBaund=%d"
			, st, m_nPort, m_lBaund);
		m_bInit = TRUE;
		return TRUE;
	}
}

void CUsbMinHuaCardReader::Release()
{
	__int16 st;
	if(NULL == m_hICDev || 0 > m_hICDev || INVALID_HANDLE_VALUE == m_hICDev)
	{
		theLog.Write("!#CUsbMinHuaCardReader::Release,1,m_hICDev=%p", m_hICDev);
		return;
	}

	st = ::rf_exit(m_hICDev);
}

void CUsbMinHuaCardReader::Beep()
{
	if(!m_bInit || !m_hICDev)
	{
		theLog.Write("!!CUsbMinHuaCardReader::Beep,1,m_bInit=%d,m_hICDev=%x", m_bInit, m_hICDev);
		return;
	}

	__int16 st;
	HANDLE icdev = m_hICDev;
	st = rf_reset(icdev,5);

	rf_beep(icdev,10);

	st = rf_halt(icdev);
	return;
}

//主动读USB卡
BOOL CUsbMinHuaCardReader::ReadUsbCard(CString &szCardNo)
{
	if(!m_bInit || !m_hICDev)
	{
		if(!Init())
		{
			theLog.Write("!!CUsbMinHuaCardReader::ReadUsbCard,1,m_bInit=%d,m_hICDev=%x", m_bInit, m_hICDev);
			return FALSE;
		}
	}
	BOOL BGet = FALSE;
//	unsigned char Sec = m_Sector;
	__int16 st;
	unsigned __int16 TagType;
	unsigned long Snr;
	unsigned char Size;
	unsigned char data[33];
	memset(data,0,33);

	HANDLE icdev = m_hICDev;

	st = rf_reset(icdev,5);

	st = rf_request(icdev,1,&TagType);
	if(st)
	{
//		m_Status.SetWindowText("寻卡失败!");
//		theLog.Write("!#CUsbMinHuaCardReader::ReadUsbCard,2,rf_request fail,寻卡失败");
		return FALSE;
	}
	st = rf_anticoll(icdev,0, &Snr);
	if(st)
	{
//		m_Status.SetWindowText("防冲突失败!");
		theLog.Write("!#CUsbMinHuaCardReader::ReadUsbCard,3,rf_anticoll fail,防冲突失败");
		return FALSE;
	}
	st = rf_select(icdev,Snr,&Size);
	if(st)
	{
//		m_Status.SetWindowText("选卡失败!");
		theLog.Write("!#CUsbMinHuaCardReader::ReadUsbCard,4,rf_select fail,选卡失败");
		return FALSE;
	}
	unsigned long sn = 0;
	__int16 a = rf_card(icdev,1,&sn);
	if (a == 0)
	{
/*
		CString s;
		s.Format(_T("%0.10X"),sn);
		AfxMessageBox(s);
*/
//		szCardNo.Format(_T("%0.10X"),sn);	//这样是10位十六进制
		szCardNo.Format(_T("%u"),sn);	//这样是十进制，与利盟读卡器读出一致
		BGet = TRUE;
		theLog.Write("##CUsbMinHuaCardReader::ReadUsbCard,10,读卡OK.oriNum=%d,szCardNo=%s", sn, szCardNo);
	}
#if 0
	unsigned char key[7];
	memset(key,0,7);
	a_hex(m_key.GetBuffer(12),key,12);
	m_key.ReleaseBuffer();
	unsigned char m_keymode;
	if(m_KeyModeCtrl.GetCurSel()==0)
		m_keymode = 0;
	else
		m_keymode = 4;
	st = rf_load_key(icdev,m_keymode,Sec,key);
	if(st)
	{
		m_Status.SetWindowText("装载密码失败!");
		return;
	}
	st = rf_authentication(icdev,m_keymode,Sec);
	if(st)
	{
		m_Status.SetWindowText("认证失败!");
		return;
	}
	if(m_OpMode == 0 )
	{
		unsigned char temp[33];
		memset(temp,0,33);


		if(m_OpMode == 0)
			st = rf_read(icdev,Sec*4,data);

		if(st)
		{
			m_Status.SetWindowText("读数据失败!");
			return;
		}
		hex_a(data,(char*)temp,16);
		m_Data.Format("%s",temp);


		m_Status.SetWindowText("读数据成功!");
	}
	else if(m_OpMode == 1)
	{
		if(m_Data == "" )
		{
			MessageBox("请确认是否有输入三个块的数据!","读写测试",MB_OK|MB_ICONWARNING);
			return;
		}
		unsigned char temp[33];
		memset(temp,0,33);
		memset(data,0,33);
		a_hex(m_Data.GetBuffer(32),temp,32);
		m_Data.ReleaseBuffer();
		memcpy(data,temp,16);

		if(m_OpMode == 1)
			st = rf_write(icdev,Sec*4,data);

		if(st)
		{
			m_Status.SetWindowText("写数据失败!");
			return;
		}

		m_Status.SetWindowText("写数据成功!");
	}
#endif
	rf_beep(icdev,10);

//	UpdateData(FALSE);
	st = rf_halt(icdev);
	return BGet;
}