#pragma once

class CCloudDocDeal
{
public:
	CCloudDocDeal(void);
	CCloudDocDeal(PCloudJobInfo job);
	~CCloudDocDeal(void);

	void SetJobInfo(PCloudJobInfo job);
	PCloudJobInfo GetJobInfo();
	void Release();

	BOOL IsGzFile();
	BOOL IsImageFile();
	BOOL IsPdfFile();
	BOOL IsWordFile();
	BOOL IsPptFile();
	BOOL IsExcelFile();
	BOOL ConvertFile();

protected:
	BOOL InitFilePath();
	CString GetFileNameByPathName(CString szPathName);

protected:
	PCloudJobInfo m_pJob;
	CString m_szOrgJobPath;	//���ĵ�����ǰ���ĵ�����������gzѹ������������δѹ���ģ���test.doc.gz����test.doc
	CString m_szJobPath;	//���ĵ�����ǰ���ĵ�����һ����δѹ���ģ���test.doc
	CString m_szDealSaveJobPath;	//���ĵ��������ĵ�ȫ·��,ͼƬת����һ����tif,officeת����pdf��tif
	CString m_szNewJobName;//���ĵ��������ĵ�����������gzѹ������������δѹ���ģ���test.doc.tif.gz����test.doc.tif������ȡ�ļ���������ļ�Ϊ׼
	CString m_szNewJobPath;
	BOOL m_bNeedDelOrgFile;
};
