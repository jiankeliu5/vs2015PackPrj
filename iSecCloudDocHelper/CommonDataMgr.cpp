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
		/*���ĵ�������ԣ�
		setvalue1�����ĵ�����ǰ�Ƿ�ѹ����0-��ѹ����1-ѹ����
		setvalue2�����ĵ�������Ƿ�ѹ����0-��ѹ����1-ѹ����
		setvalue3,��office�����ĵ�������ĸ�ʽ����ʱֻ֧�֡�pdf/tif�����֡�
		setvalue4,��office�����ĵ�������Ϊ��.tif����ʽ��dpi,��.pdf����֧��dpi������
		setvalue5,��ͼƬ�����ĵ�������ĸ�ʽ��֧�֡�jpg/png/bmp/tif/gif����
		setvalue6,��ͼƬ�����ĵ�������Ϊ��setvalue5����ʽ��dpi��
		setvalue7,��ͼƬ�����ĵ�����ʹͼƬ��СΪ��ӡ�豸����������,���Ǵ�ӡ����*/
		m_bCloudDocDealBeforeIsGz = _tstoi(ary.GetAt(1));
		m_bCloudDocDealAfterIsGz = _tstoi(ary.GetAt(2));
		CString szOfficeDealFormatType = ary.GetAt(3);
		szOfficeDealFormatType.Trim(_T("."));
		szOfficeDealFormatType = _T(".") + szOfficeDealFormatType;
		m_nOfficeDealFormatType = (szOfficeDealFormatType.CompareNoCase(Cloud_Doc_Format_Suffix_PDF) == 0) ? Cloud_Doc_Format_PDF : Cloud_Doc_Format_TIFF;
		m_nOfficeDealFormatDpi = _tstoi(ary.GetAt(4));
		m_nImageDealFormatType = Cloud_Doc_Format_TIFF;	//��ʱֻ֧��tif
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

