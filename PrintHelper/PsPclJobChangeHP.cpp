#include "StdAfx.h"
#include "PsPclJobChangeHP.h"

CPsPclJobChangeHP::CPsPclJobChangeHP(void)
{
}

CPsPclJobChangeHP::~CPsPclJobChangeHP(void)
{
}

BOOL CPsPclJobChangeHP::ChangeJobStrToHB_PS(CStringA& szSource)
{
	if (szSource.Find("BLACKONLY") >= 0 || szSource.Find("COMPOSITE") >= 0)
	{
		return FALSE;
	}

	int nRet = szSource.Replace("@PJL SET GRAYSCALE=OFF", "@PJL SET GRAYSCALE=BLACKONLY");
	CStringA szTag = "@PJL SET PLANESINUSE=";
	int nIndex = szSource.Find(szTag);
	if (nIndex >= 0)
	{
		szTag = szSource.Mid(nIndex,szTag.GetLength() + 1);
	}
	theLog.Write(_T("CPsPclJobChangeHP::ChangeJobStrToHB_PS,szTag=%s"),szTag);
	szSource.Replace(szTag, "@PJL SET PLANESINUSE=1");//不管黑白还是彩色一律替换
	if (nRet != 0)
	{
		theLog.Write(_T("ChangJobToHB_PS,2,找到彩色，替换成黑白"));
	}
	else
	{
		theLog.Write(_T("ChangJobToHB_PS,3,没找到彩色或本身无色彩参数，追加黑白"));
		int nPos = szSource.Find("@PJL ENTER LANGUAGE=POSTSCRIPT");
		if (nPos >= 0)
		{
			szSource.Insert(nPos,"@PJL SET PLANESINUSE=1\n");
			szSource.Insert(nPos,"@PJL SET GRAYSCALE=BLACKONLY\n");//顺序不能变
		}
	}
	return TRUE;
}

BOOL CPsPclJobChangeHP::ChangeJobStrToDuplexV_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexNoTumble");
	int nRet2 = szSource.Replace("<</Duplex false /Tumble false>> setpagedevice","<</Duplex true /Tumble false>> setpagedevice");
	if (nRet != 0 || nRet2 != 0)
	{
		theLog.Write(_T("ChangeJobToDuplexV_PS,找到单面，替换成双面长边"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("ChangeJobToDuplexV_PS,没找到单面"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeHP::ChangeJobStrToDuplexH_PS(CStringA& szSource)
{
	int nRet = szSource.Replace("%%BeginFeature: *Duplex None","%%BeginFeature: *Duplex DuplexTumble");
	int nRet2 = szSource.Replace("<</Duplex false /Tumble false>> setpagedevice","<</Duplex true /Tumble true>> setpagedevice");
	if (nRet != 0 || nRet2 !=0)
	{
		theLog.Write(_T("ChangeJobToDuplexH_PS,2,找到单面，替换成双面短边"));
		return TRUE;
	}
	else
	{
		//theLog.Write(_T("ChangeJobToDuplexH_PS,3,没找到单面"));
		return FALSE;
	}
}

BOOL CPsPclJobChangeHP::ChangeJobStrToHB_PCL(CStringA& szSource)
{
	if (szSource.Find("BLACKONLY") >= 0 || szSource.Find("COMPOSITE") >= 0)
	{
		return FALSE;
	}

	int nRet = szSource.Replace("@PJL SET GRAYSCALE=OFF", "@PJL SET GRAYSCALE=BLACKONLY");
	CStringA szTag = "@PJL SET PLANESINUSE=";
	int nIndex = szSource.Find(szTag);
	if (nIndex >= 0)
	{
		szTag = szSource.Mid(nIndex, szTag.GetLength() + 1);
	}
	theLog.Write(_T("CPsPclJobChangeHP::ChangeJobStrToHB_PCL,szTag=%s"), szTag);
	szSource.Replace(szTag, "@PJL SET PLANESINUSE=1");//不管黑白还是彩色一律替换
	if (nRet != 0)
	{
		theLog.Write(_T("ChangeJobStrToHB_PCL,2,找到彩色，替换成黑白"));
	}
	else
	{
		theLog.Write(_T("ChangeJobStrToHB_PCL,3,没找到彩色或本身无色彩参数，追加黑白"));
		//@PJL ENTER LANGUAGE=PCLXL
		//@PJL ENTER LANGUAGE = PCLXL
		int nPos = szSource.Find("@PJL ENTER LANGUAGE");
		if (nPos >= 0)
		{
			szSource.Insert(nPos, "@PJL SET PLANESINUSE=1\n");
			szSource.Insert(nPos, "@PJL SET GRAYSCALE=BLACKONLY\n");//顺序不能变
		}
	}
	return TRUE;
}
