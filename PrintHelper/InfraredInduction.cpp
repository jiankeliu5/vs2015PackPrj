#include "StdAfx.h"
#include "InfraredInduction.h"

CInfraredInduction::CInfraredInduction(void)
{
	m_pHelper = NULL;
	m_pCallBack = NULL;
	memset(m_oDataBuffer, 0x0, DataBufferLen);
	m_nReadDataBufferLen = 0;
}

CInfraredInduction::~CInfraredInduction(void)
{
	Exit();
}

BOOL CInfraredInduction::Init(int nComIndex)
{
	Exit();

	m_pHelper = new CCOMPortHelper;
	m_pHelper->SetCallBack(this);
	return m_pHelper->Open(nComIndex);
}

void CInfraredInduction::Exit()
{
	if (m_pHelper)
	{
		m_pHelper->Close();
		delete m_pHelper;
		m_pHelper = NULL;
	}
	m_pCallBack = NULL;
	memset(m_oDataBuffer, 0x0, DataBufferLen);
	m_nReadDataBufferLen = 0;
}

void CInfraredInduction::SetCallBack(IInfraredInductionInterface* pCallBack)
{
	m_pCallBack = pCallBack;
}

void CInfraredInduction::OnByte(BYTE byte)
{
	// 	theLog.Write(_T("CInfraredInduction::OnByte,1,Data0=%02X,Data1=%02X,Data2=%02X,Data3=%02X")

	// 		, m_oDataBuffer[0], m_oDataBuffer[1], m_oDataBuffer[2], m_oDataBuffer[3]);
	m_oDataBuffer[m_nReadDataBufferLen%DataBufferLen] = byte;
	m_nReadDataBufferLen++;
	if (m_oDataBuffer[0] > 0 || m_oDataBuffer[1] > 0 || m_oDataBuffer[2] > 0 || m_oDataBuffer[3] > 0)
	{
		m_pCallBack->OnInfraredInduction(IIT_SIGNAL, m_oDataBuffer[1]);
		m_nReadDataBufferLen = 0;
	}
#if 0
	if (m_nReadDataBufferLen >= DataBufferLen)
	{
		//30 31 20 0A	//01	//̽ͷһ������λ���ݡ�'0','1',�ո�,���С�
		//30 32 20 0A	//02	//̽ͷ��������λ���ݡ�'0','2',�ո�,���С�
		if (m_oDataBuffer[0] == 0x30 && m_oDataBuffer[2] == 0x20 && m_oDataBuffer[3] == 0x0A)
		{
			if (m_oDataBuffer[1] ==	0x31 || m_oDataBuffer[1] ==	0x32)
			{
				if (m_pCallBack)
				{
					m_pCallBack->OnInfraredInduction(IIT_SIGNAL, m_oDataBuffer[1]);
				}
			}
			else
			{
				theLog.Write(_T("!!CInfraredInduction::OnByte,2,Data0=%02X,Data1=%02X,Data2=%02X,Data3=%02X")

					, m_oDataBuffer[0], m_oDataBuffer[1], m_oDataBuffer[2], m_oDataBuffer[3]);
			}
		}
		else
		{
			theLog.Write(_T("!!CInfraredInduction::OnByte,3,Data0=%02X,Data1=%02X,Data2=%02X,Data3=%02X")

				, m_oDataBuffer[0], m_oDataBuffer[1], m_oDataBuffer[2], m_oDataBuffer[3]);
		}
		m_nReadDataBufferLen = 0;
	}
#endif
}

void CInfraredInduction::OnErr(DWORD dwError)
{
	if (m_pCallBack)
	{
		m_pCallBack->OnInfraredInduction(IIT_ERROR, dwError);
	}
}
