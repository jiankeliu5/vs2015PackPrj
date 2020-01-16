#pragma once

//�ο��ĵ�
//https://www.c-sharpcorner.com/UploadFile/8a67c0/curl-command-with-ftp-and-sftp-in-C-Sharp/
//https://www.cnblogs.com/lidabo/p/4583067.html
//https://wenku.baidu.com/view/e41480f8f705cc17552709c9.html

#include <string>
#include <sstream>
#include <vector>

#include <Windows.h>

//FTP����״̬
enum FtpTransferStatus
{
	FTS_Ok = 0,			//����
	FTS_Error = 1,		//����
	FTS_TimeOut = 2,	//��ʱ
	FTS_Cancel = 3		//ȡ��
};

//FTP��������
enum FtpTransferType
{
	FTT_Upload = 0,		//�ϴ�
	FTT_Download = 1,	//����
};

//FTP����ӿ�
class IFtpTransferInterface
{
public:
	virtual void OnFtpTransferBegin(const char* pFilePath, FtpTransferType nType) = NULL;
	virtual FtpTransferStatus OnFtpTransferData(const char* pFilePath, FtpTransferType nType, __int64 nFileTotalSize, __int64 nTransferSize) = NULL;
	virtual void OnFtpTransferEnd(const char* pFilePath, FtpTransferType nType, FtpTransferStatus nStatus) = NULL;
};

//Ftp����
enum FtpType
{
	FT_Ftp,
	FT_SFtp,
	FT_Ftps
};

struct FtpFileOperate
{
	char szRemotePath[MAX_PATH];//Զ��ftp·��,utf8����
	char szLocalPath[MAX_PATH];	//�����ļ�·��,utf8����
	FILE* pLocalFile;			//�򿪵ı����ļ�ָ��
	__int64 nFileSize;			//�ļ���С
	__int64 nFileTransferSize;	//�ļ��Ѿ�����Ĵ�С
	FtpTransferType oFtpTransferType;	//ftp��������
	FtpTransferStatus oFtpTransferStatus;	//ftp����״̬
	void* pData;				//�Զ�������
};

/************************************************************************/
/*  CFtpClient 
	ftp/ftps/sftp�����࣬�����ϴ������أ�ɾ���ļ���Ŀ¼������Ŀ¼��
	�������ļ���Ŀ¼��
	���нӿڳ��ֵ�Path������ȫ��ʹ��uft8����
*/
/************************************************************************/
class CFtpClient
{
public:
	CFtpClient(void);
	~CFtpClient(void);

	//�Ƿ�������־
	void SetDebug(bool bDebug);

	//����FTP��Ϣ
	void SetFtpInfo(FtpType oFtpType, const std::string & strServer, int nPort, 
		const std::string & strUser, const std::string & strPassword);

	//�ϴ��ļ�
	bool UploadFile(const std::string & strLocalPath, const std::string & strRemotePath, IFtpTransferInterface *pCallBack = NULL);

	//�����ļ�
	bool DownloadFile(const std::string & strRemotePath, const std::string & strLocalPath, IFtpTransferInterface *pCallBack = NULL);

	//ɾ���ļ�
	bool DeleteFile(const std::string & strRemotePath);

	//����Ŀ¼���𼶴���Ŀ¼�ṹ
	bool CreateDirectory(const std::string & strRemotePath);

	//ɾ��Ŀ¼����Ŀ¼������Ŀ¼���ļ�ʱ��ɾ��Ŀ¼ʧ��
	bool DeleteDirectory(const std::string & strRemotePath);

	//���������󣬿������ļ�����Ŀ¼
	bool Rename(const std::string & strOldRemotePath, const std::string & strNewRemotePath);

protected:
	//��ȡ������URL
	std::string GetServerUrl();

	//��ȡftp�ļ�������URL
	std::string GetRemoteUrl(const std::string & strRemotePath);

	//��ʽ��һ�����������1���������ַ���
	std::string GetCommand(const std::string & strCommand, const std::string & strRemotePath, bool bAddDoubleQuotationMarks = true);

	//��ʽ��һ�����������2���������ַ���
	std::string GetCommand2(const std::string & strCommand, const std::string & strRemotePath, const std::string & strRemotePath2, bool bAddDoubleQuotationMarks = true);

	//��ȡһ��URL�е����м�������Ŀ¼��ͨ�����飨oDirectoryVector������
	void GetDirectory(const std::string & strRemotePath, std::vector<std::string>& oDirectoryVector);

	//��ȡһ��·���е����һ��Ŀ¼��
	std::string GetLastDirectory(const std::string & strFullDirectory);

	//��URL��һЩ�����ַ�����ת�룬����URL���ܲ��ܱ���ȷ��ʶ��
	std::string TransCodeURL(const std::string & strURL);

	//��ȡԶ���ļ��Ĵ�С
	__int64 GetRemoteFileSize(const std::string & strRemoteUrl);

	//��ȡ�����ļ��Ĵ�С
	__int64 GetFileSize(const std::string & strFilePath);
	__int64 GetFileSize(const FILE* pFile);

	//UTF8ת���Unicode
	std::wstring UTF8ToUnicode(const char* pszSource);

	//Unicodeת���UTF8
	std::string UnicodeToUTF8(const wchar_t* pwszSource);

	//����SSL����
	void ConfigSSL(void* pContent);

	//�����������
	void ConfigNetwork(void* pContent);

private:
	bool m_bDebug;	//�Ƿ�������־
	FtpType m_oFtpType;			//ftp����
	std::string m_strServer;	//��������ַ
	int m_nPort;				//�������˿�
	std::string m_strUser;		//��������¼���û���
	std::string m_strPassword;	//��������¼������
};
