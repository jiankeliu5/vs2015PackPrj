#pragma once

//�ļ�����״̬
enum FileOperateStatus
{
	FOS_Ok = 0,			//����
	FOS_Error = 1,		//����
	FOS_TimeOut = 2,	//��ʱ
	FOS_Cancel = 3		//ȡ��
};

//�ļ���������
enum FileOperateType
{
	FOT_Upload = 0,		//�ϴ�
	FOT_Download = 1,	//����
	FOT_Compress = 2,	//ѹ��
	FOT_UnCompress = 3	//��ѹ
};

//�ļ������ӿ�
class IFileOperateInterface
{
public:
	virtual void OnFileOperateBegin(LPCTSTR pFilePath, FileOperateType nType) = NULL;
	virtual FileOperateStatus OnFileOperateData(LPCTSTR pFilePath, FileOperateType nType, DWORD dwFileTotalSize, DWORD dwOperatedSize) = NULL;
	virtual void OnFileOperateEnd(LPCTSTR pFilePath, FileOperateType nType, FileOperateStatus nStatus) = NULL;
};

class _AA_DLL_EXPORT_ CFileHelper
{
public:
	CFileHelper(void);
	~CFileHelper(void);

	static BOOL CreateFtpDir(CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd);

	//FTP�ļ�����
	static BOOL UploadFileToFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd, CString szFtpFileName, CString szLocalFilePath, IFileOperateInterface *pCallBack = NULL);
	static BOOL DownloadFileFromFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd, CString szFtpFileName, CString szLocalFilePath, IFileOperateInterface *pCallBack = NULL);
	static BOOL DeleteFileFromFtpServer(CString szFtpServer, int nFtpPort, CString szFtpDir, CString szUserName, CString szPwd, CString szFtpFileName);

	//HTTP�ļ�����
	static BOOL DownloadFileFromHttpServer(CString szHttpServer, int nHttpPort, CString szVirtualDirectory, CString szHttpFilePath, CString szUserName, CString szPwd, CString szLocalFilePath, IFileOperateInterface *pCallBack = NULL);
	static BOOL DownloadFileFromHttpServer(CString szHttpUrl, CString szLocalFilePath, IFileOperateInterface *pCallBack = NULL);

	//��ѹ��ѹ���ļ�
	static BOOL UnCompressGzFile(IN CString szFileGzPath, OUT CString& szFileOrgPath, BOOL bDelGzFile, IFileOperateInterface *pCallBack = NULL);
	static BOOL CompressGzFile(IN CString szFileOrg, OUT CString& szFileGzPath, BOOL bDelOrgFile, IFileOperateInterface *pCallBack = NULL);
	static BOOL CompressGzFile2(IN CString szFileOrg, IN CString& szFileGzPath, BOOL bDelOrgFile, IFileOperateInterface *pCallBack = NULL);
	static BOOL UnCompressGzFile2(IN CString szFileGzPath, IN CString& szFileOrgPath, BOOL bDelGzFile, IFileOperateInterface *pCallBack = NULL);
	static BOOL UnCompressZipFile(IN CString szZipFilePath, IN CString szOrgFilePath, BOOL bDelZipFile);
	static BOOL CompressZipFile(IN CString szOrgFilePath, IN CString szZipFilePath, BOOL bDelOrgFile);

	//�ļ���������base64����ת��
	static BOOL FileToBase64(IN CString szFilePath, OUT CString& szBase64);
	static BOOL Base64ToFile(IN CString& szBase64, IN CString szFilePath);
};
