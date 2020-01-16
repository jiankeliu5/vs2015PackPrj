#include "stdafx.h"
#include "CommonDataMgr.h"

CQtMgrGlobalDataConfig::CQtMgrGlobalDataConfig()
{
	m_bCloudDocDealAfterIsGz = FALSE;
	m_bCloudDocDealBeforeIsGz = FALSE;
	m_nOfficeDealFormatType = Cloud_Doc_Format_PDF;
	m_nOfficeDealFormatDpi = 300;
	m_nImageDealFormatType = Cloud_Doc_Format_TIFF;
	m_nImageDealFormatDpi = 300;
	bImageUsePhysicalArea = FALSE;
}

CQtMgrGlobalDataConfig& CQtMgrGlobalDataConfig::GetInstance()
{
	static CQtMgrGlobalDataConfig one ;
	return one;
}

void CQtMgrGlobalDataConfig::LoadConfig()
{
	CDbOperateHelper helper;
	if (!helper.CheckDbConn())
	{
		theLog.Write(_T("!!!!CPrintGlobalConfig::Load,CheckDbConn fail,break Load."));
		return;
	}

	CString sName = _T("cloud_doc_deal_policy");
	CStringArray ary;
	helper.GetSystemSetting(sName,ary);
	if (ary.GetCount() >= 8)
	{
		/*云文档处理策略：
		setvalue1，云文档处理前是否压缩，0-不压缩，1-压缩。
		setvalue2，云文档处理后是否压缩，0-不压缩，1-压缩。
		setvalue3,【office】云文档处理保存的格式，暂时只支持【pdf/tif】两种。
		setvalue4,【office】云文档处理保存为【.tif】格式的dpi,【.pdf】不支持dpi参数。
		setvalue5,【图片】云文档处理保存的格式，支持【jpg/png/bmp/tif/gif】。
		setvalue6,【图片】云文档处理保存为【setvalue5】格式的dpi。
		setvalue7,【图片】云文档处理使图片大小为打印设备的物理区域,不是打印区域。*/
		m_bCloudDocDealBeforeIsGz = _tstoi(ary.GetAt(1));
		m_bCloudDocDealAfterIsGz = _tstoi(ary.GetAt(2));
		CString szOfficeDealFormatType = ary.GetAt(3);
		szOfficeDealFormatType.Trim(_T("."));
		szOfficeDealFormatType = _T(".") + szOfficeDealFormatType;
		m_nOfficeDealFormatType = (szOfficeDealFormatType.CompareNoCase(Cloud_Doc_Format_Suffix_PDF) == 0) ? Cloud_Doc_Format_PDF : Cloud_Doc_Format_TIFF;
		m_nOfficeDealFormatDpi = _tstoi(ary.GetAt(4));
		m_nImageDealFormatType = Cloud_Doc_Format_TIFF;	//暂时只支持tif
		m_nImageDealFormatDpi = _tstoi(ary.GetAt(6));
		bImageUsePhysicalArea = _tstoi(ary.GetAt(7));
	}
	else
	{
		m_bCloudDocDealAfterIsGz = FALSE;
		m_bCloudDocDealBeforeIsGz = FALSE;
		m_nOfficeDealFormatType = Cloud_Doc_Format_PDF;
		m_nOfficeDealFormatDpi = 300;
		m_nImageDealFormatType = Cloud_Doc_Format_TIFF;
		m_nImageDealFormatDpi = 300;
		bImageUsePhysicalArea = FALSE;
	}
	theLog.Write(_T("CPrintGlobalConfig::LoadConfig")
		_T(",m_bCloudDocDealBeforeIsGz=%d,m_bCloudDocDealAfterIsGz=%d")
		_T(",m_nOfficeDealFormatType=%d,m_nOfficeDealFormatDpi=%d")
		_T(",m_nImageDealFormatType=%d,m_nImageDealFormatDpi=%d")
		_T(",bImageUsePhysicalArea=%d")
		, m_bCloudDocDealBeforeIsGz, m_bCloudDocDealAfterIsGz
		, m_nOfficeDealFormatType, m_nOfficeDealFormatDpi
		, m_nImageDealFormatType, m_nImageDealFormatDpi
		, bImageUsePhysicalArea);

	CheckFileStorageList();
}


void CQtMgrGlobalDataConfig::CheckFileStorageList()
{
	theLog.Write(_T("CQtMgrGlobalDataConfig::CheckFileStorageList"));

	FileStorageList oFileStorageList;
	CDbOperateHelper helper;
	if (helper.GetFileStorageList(oFileStorageList))
	{
		CFileStorageHelper::GetInstance().InitFileStorageList(oFileStorageList);
	}
	else
	{
		theLog.Write(_T("!!CQtMgrGlobalDataConfig::CheckFileStorageList,fail"));
	}
}

