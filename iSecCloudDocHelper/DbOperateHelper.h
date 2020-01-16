#pragma once

class CDbOperateHelper
{
public:
	CDbOperateHelper(void);
	~CDbOperateHelper(void);

public:
	static BOOL InitDataBase();
	static void ReleaseDataBase();

	BOOL IsNeedDealCloudJob();

	BOOL GetNeedDealCloudJob(CListCloudJobInfo& list);
	BOOL UpdateCloudJob(PCloudJobInfo job);
	BOOL UpdateCloudJobState(PCloudJobInfo job, int nState);
	BOOL GetSystemSetting(CString& sName,CStringArray& ary);
	BOOL GetFileStorageList(FileStorageList& oList);	//��ȡ�ĵ��洢��Ϣ�б�
	CString GetTimeStr(double fTime);
	BOOL CheckDbConn();
	BOOL ReConnDb();

	int GetDeptByUserId(int nUserId);
	BOOL GetJobAuditProcess(int nUserId,JobAuditProcess& process);
	BOOL AddCheck(CPrintAlarmInfo* pInfo);//����˹��������
	BOOL AddCheckNotify(CPrintAlarmInfo* pInfo);

	//��ȡ��ӡϵͳ�������
	UiLanguage GetBackendLanguage();

	//��ȡ�û�����ҵ�Ƿ���Ҫ���
	BOOL GetUserJobAuditPolicy(int nUserId, int& nAuditTacticsId,int& nAuditType, BOOL& bAuditLog, CString& szAuditUsers, CString& szAuditAdmins);

protected:
	Cloud_Type m_CloudType;
	CAdo* m_pAdo;
	CCriticalSection2 m_cs;
};


class CPrintGlobalConfig
{
protected:
	CPrintGlobalConfig()
	{
		m_sScanDir.Empty();
		m_sScanHttpDir.Empty();
		m_sPrintDir.Empty();
		m_sPrintHttpDir.Empty();
		m_sBackupDir.Empty();

		m_nVersion = 1;
		m_bIsUseFtpStogeCloudDoc = FALSE;
		memset(&m_oFtpInfo, 0x0, sizeof(FtpInfo));
		m_bCloudDocDealAfterIsGz = FALSE;
		m_bCloudDocDealBeforeIsGz = FALSE;
		m_nOfficeDealFormatType = Cloud_Doc_Format_PDF;
		m_nOfficeDealFormatDpi = 300;
		m_nImageDealFormatType = Cloud_Doc_Format_TIFF;
		m_nImageDealFormatDpi = 300;
	}
public:
	CString m_sScanDir;
	CString m_sScanHttpDir;
	CString m_sPrintDir;
	CString m_sPrintHttpDir;
	CString m_sBackupDir;
	int m_nVersion;

	FtpInfo m_oFtpInfo;
	BOOL m_bIsUseFtpStogeCloudDoc;
	BOOL m_bCloudDocDealBeforeIsGz;	//���ĵ�����ǰ�Ƿ�ѹ��
	BOOL m_bCloudDocDealAfterIsGz;	//���ĵ�������Ƿ�ѹ��
	int m_nOfficeDealFormatType;	//��office�����ĵ�������ĸ�ʽ����ʱֻ֧�֡�pdf/tif�����֡�
	int m_nOfficeDealFormatDpi;		//��office�����ĵ�������Ϊ��.tif����ʽ��dpi,��.pdf����֧��dpi������
	int m_nImageDealFormatType;		//��ͼƬ�����ĵ�������ĸ�ʽ��֧�֡�jpg/png/bmp/tif/gif����
	int m_nImageDealFormatDpi;		//��ͼƬ�����ĵ�������Ϊ��setvalue5����ʽ��dpi��

	void Load()
	{
		CDbOperateHelper helper;
		if (!helper.CheckDbConn())
		{
			theLog.Write(_T("!!!!CPrintGlobalConfig::Load,CheckDbConn fail,break Load."));
			return;
		}
		CString sName = _T("ScanDir");
		CStringArray ary;
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 3)
		{
			m_sScanDir = ary.GetAt(1);
			m_sScanHttpDir = ary.GetAt(2);
		}
		sName = _T("PrintDir");
		ary.RemoveAll();
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 3)
		{
			m_sPrintDir = ary.GetAt(1);
			m_sPrintHttpDir = ary.GetAt(2);
		}
		sName = _T("wdbfml");
		ary.RemoveAll();
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 3)
		{
			m_sBackupDir = ary.GetAt(1);
		}
		sName = _T("Version");
		ary.RemoveAll();
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 2)
		{
			//0����ҵ�棬1��У԰��, 2:У԰�ͳɼ���3���ɼ�
			m_nVersion = _tstoi(ary.GetAt(1));
		}
		else
		{
			m_nVersion = 1;
		}

		sName = _T("cloud_doc_stoge_ftp_info");
		ary.RemoveAll();
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 11)
		{
			_tcscpy_s(m_oFtpInfo.szFtpIP, ary.GetAt(1));
			m_oFtpInfo.nFtpPort = _tstoi(ary.GetAt(2));
			_tcscpy_s(m_oFtpInfo.szFtpUser, ary.GetAt(3));
			_tcscpy_s(m_oFtpInfo.szFtpPwd, ary.GetAt(4));
			_tcscpy_s(m_oFtpInfo.szFtpDir, ary.GetAt(5));
			_tcscpy_s(m_oFtpInfo.szFtpDiskDir, ary.GetAt(6));
			m_bIsUseFtpStogeCloudDoc = _tstoi(ary.GetAt(10));
		}
		else
		{
			m_bIsUseFtpStogeCloudDoc = FALSE;
			memset(&m_oFtpInfo, 0x0, sizeof(FtpInfo));
		}
		theLog.Write(_T("CPrintGlobalConfig::Load,szFtpDiskDir=%s,m_bIsUseFtpStogeCloudDoc=%d")
			, m_oFtpInfo.szFtpDiskDir, m_bIsUseFtpStogeCloudDoc);

		sName = _T("cloud_doc_deal_policy");
		ary.RemoveAll();
		helper.GetSystemSetting(sName,ary);
		if (ary.GetCount() >= 11)
		{
			/*���ĵ�������ԣ�
			setvalue1�����ĵ�����ǰ�Ƿ�ѹ����0-��ѹ����1-ѹ����
			setvalue2�����ĵ�������Ƿ�ѹ����0-��ѹ����1-ѹ����
			setvalue3,��office�����ĵ�������ĸ�ʽ����ʱֻ֧�֡�pdf/tif�����֡�
			setvalue4,��office�����ĵ�������Ϊ��.tif����ʽ��dpi,��.pdf����֧��dpi������
			setvalue5,��ͼƬ�����ĵ�������ĸ�ʽ��֧�֡�jpg/png/bmp/tif/gif����
			setvalue6,��ͼƬ�����ĵ�������Ϊ��setvalue5����ʽ��dpi��*/
			m_bCloudDocDealBeforeIsGz = _tstoi(ary.GetAt(1));
			m_bCloudDocDealAfterIsGz = _tstoi(ary.GetAt(2));
			CString szOfficeDealFormatType = ary.GetAt(3);
			szOfficeDealFormatType.Trim(_T("."));
			szOfficeDealFormatType = _T(".") + szOfficeDealFormatType;
			m_nOfficeDealFormatType = (szOfficeDealFormatType.CompareNoCase(Cloud_Doc_Format_Suffix_PDF) == 0) ? Cloud_Doc_Format_PDF : Cloud_Doc_Format_TIFF;
			m_nOfficeDealFormatDpi = _tstoi(ary.GetAt(4));
			m_nImageDealFormatType = Cloud_Doc_Format_TIFF;	//��ʱֻ֧��tif
			m_nImageDealFormatDpi = _tstoi(ary.GetAt(6));
		}
		else
		{
			m_bCloudDocDealAfterIsGz = FALSE;
			m_bCloudDocDealBeforeIsGz = FALSE;
			m_nOfficeDealFormatType = Cloud_Doc_Format_PDF;
			m_nOfficeDealFormatDpi = 300;
			m_nImageDealFormatType = Cloud_Doc_Format_TIFF;
			m_nImageDealFormatDpi = 300;
		}
		theLog.Write(_T("CPrintGlobalConfig::Load")
			_T(",m_bCloudDocDealBeforeIsGz=%d,m_bCloudDocDealAfterIsGz=%d")
			_T(",m_nOfficeDealFormatType=%d,m_nOfficeDealFormatDpi=%d")
			_T(",m_nImageDealFormatType=%d,m_nImageDealFormatDpi=%d")
			, m_bCloudDocDealBeforeIsGz, m_bCloudDocDealAfterIsGz
			, m_nOfficeDealFormatType, m_nOfficeDealFormatDpi
			, m_nImageDealFormatType, m_nImageDealFormatDpi);
	}

	static CPrintGlobalConfig& GetInstance()
	{
		static CPrintGlobalConfig one;
		return one;
	}
};
