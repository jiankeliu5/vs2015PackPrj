#include "StdAfx.h"
#include "PsPclJobChangeKM.h"

CPsPclJobChangeKM::CPsPclJobChangeKM(void)
{
}

CPsPclJobChangeKM::~CPsPclJobChangeKM(void)
{
}

BOOL CPsPclJobChangeKM::ChangeJobStrToHB_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("<</ProcessColorModel /DeviceCMYK>>setpagedevice", "<</ProcessColorModel /DeviceGray>>setpagedevice");
	if (nRet != 0)
	{
		theLog.Write(_T("CPsPclJobChangeKM::ChangJobToHB_PS,2,�ҵ���ɫ���滻�ɺڰ�"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangJobToHB_PS,3,û�ҵ���ɫ"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeKM::ChangeJobStrToDuplexV_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("<</Duplex false>>setpagedevice","<</Duplex true>>setpagedevice");
	int nRet2 = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexNoTumble");
	if (nRet != 0 || nRet2 != 0)
	{
		theLog.Write(_T("CPsPclJobChangeKM::ChangeJobToDuplexV_PS,2,�ҵ����棬�滻��˫�泤��"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangeJobToDuplexV_PS,3,û�ҵ�����"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("<</Duplex false>>setpagedevice","<</Duplex true>>setpagedevice");
	int nRet2 = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexTumble");
	if (nRet != 0 || nRet2 !=0)
	{
		theLog.Write(_T("CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS,2,�ҵ����棬�滻��˫��̱�"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS,3,û�ҵ�����"));
		return FALSE;
	}
}
