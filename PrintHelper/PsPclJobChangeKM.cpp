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
		theLog.Write(_T("CPsPclJobChangeKM::ChangJobToHB_PS,2,找到彩色，替换成黑白"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangJobToHB_PS,3,没找到彩色"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeKM::ChangeJobStrToDuplexV_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("<</Duplex false>>setpagedevice","<</Duplex true>>setpagedevice");
	int nRet2 = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexNoTumble");
	if (nRet != 0 || nRet2 != 0)
	{
		theLog.Write(_T("CPsPclJobChangeKM::ChangeJobToDuplexV_PS,2,找到单面，替换成双面长边"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangeJobToDuplexV_PS,3,没找到单面"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("<</Duplex false>>setpagedevice","<</Duplex true>>setpagedevice");
	int nRet2 = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexTumble");
	if (nRet != 0 || nRet2 !=0)
	{
		theLog.Write(_T("CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS,2,找到单面，替换成双面短边"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("CPsPclJobChangeKM::ChangeJobStrToDuplexH_PS,3,没找到单面"));
		return FALSE;
	}
}
