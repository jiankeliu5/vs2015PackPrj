#pragma once

//参考文档
//https://www.c-sharpcorner.com/UploadFile/8a67c0/curl-command-with-ftp-and-sftp-in-C-Sharp/
//https://www.cnblogs.com/lidabo/p/4583067.html
//https://wenku.baidu.com/view/e41480f8f705cc17552709c9.html

#include <string>
#include <sstream>
#include <vector>

#include <Windows.h>

//FTP传输状态
enum FtpTransferStatus
{
	FTS_Ok = 0,			//正常
	FTS_Error = 1,		//出错
	FTS_TimeOut = 2,	//超时
	FTS_Cancel = 3		//取消
};

//FTP传输类型
enum FtpTransferType
{
	FTT_Upload = 0,		//上传
	FTT_Download = 1,	//下载
};

//FTP传输接口
class IFtpTransferInterface
{
public:
	virtual void OnFtpTransferBegin(const char* pFilePath, FtpTransferType nType) = NULL;
	virtual FtpTransferStatus OnFtpTransferData(const char* pFilePath, FtpTransferType nType, __int64 nFileTotalSize, __int64 nTransferSize) = NULL;
	virtual void OnFtpTransferEnd(const char* pFilePath, FtpTransferType nType, FtpTransferStatus nStatus) = NULL;
};

//Ftp类型
enum FtpType
{
	FT_Ftp,
	FT_SFtp,
	FT_Ftps
};

struct FtpFileOperate
{
	char szRemotePath[MAX_PATH];//远程ftp路径,utf8编码
	char szLocalPath[MAX_PATH];	//本地文件路径,utf8编码
	FILE* pLocalFile;			//打开的本地文件指针
	__int64 nFileSize;			//文件大小
	__int64 nFileTransferSize;	//文件已经传输的大小
	FtpTransferType oFtpTransferType;	//ftp传输类型
	FtpTransferStatus oFtpTransferStatus;	//ftp传输状态
	void* pData;				//自定义数据
};

/************************************************************************/
/*  CFtpClient 
	ftp/ftps/sftp操作类，包含上传，下载，删除文件和目录，创建目录和
	重命名文件和目录。
	其中接口出现的Path参数，全部使用uft8编码
*/
/************************************************************************/
class CFtpClient
{
public:
	CFtpClient(void);
	~CFtpClient(void);

	//是否启用日志
	void SetDebug(bool bDebug);

	//设置FTP信息
	void SetFtpInfo(FtpType oFtpType, const std::string & strServer, int nPort, 
		const std::string & strUser, const std::string & strPassword);

	//上传文件
	bool UploadFile(const std::string & strLocalPath, const std::string & strRemotePath, IFtpTransferInterface *pCallBack = NULL);

	//下载文件
	bool DownloadFile(const std::string & strRemotePath, const std::string & strLocalPath, IFtpTransferInterface *pCallBack = NULL);

	//删除文件
	bool DeleteFile(const std::string & strRemotePath);

	//创建目录，逐级创建目录结构
	bool CreateDirectory(const std::string & strRemotePath);

	//删除目录，当目录下有子目录和文件时，删除目录失败
	bool DeleteDirectory(const std::string & strRemotePath);

	//重命名对象，可以是文件或者目录
	bool Rename(const std::string & strOldRemotePath, const std::string & strNewRemotePath);

protected:
	//获取服务器URL
	std::string GetServerUrl();

	//获取ftp文件的完整URL
	std::string GetRemoteUrl(const std::string & strRemotePath);

	//格式化一个操作命令（带1个参数）字符串
	std::string GetCommand(const std::string & strCommand, const std::string & strRemotePath, bool bAddDoubleQuotationMarks = true);

	//格式化一个操作命令（带2个参数）字符串
	std::string GetCommand2(const std::string & strCommand, const std::string & strRemotePath, const std::string & strRemotePath2, bool bAddDoubleQuotationMarks = true);

	//获取一个URL中的所有级的完整目录，通过数组（oDirectoryVector）传出
	void GetDirectory(const std::string & strRemotePath, std::vector<std::string>& oDirectoryVector);

	//获取一个路径中的最后一级目录名
	std::string GetLastDirectory(const std::string & strFullDirectory);

	//将URL的一些特殊字符进行转码，否则URL可能不能被正确的识别
	std::string TransCodeURL(const std::string & strURL);

	//获取远程文件的大小
	__int64 GetRemoteFileSize(const std::string & strRemoteUrl);

	//获取本地文件的大小
	__int64 GetFileSize(const std::string & strFilePath);
	__int64 GetFileSize(const FILE* pFile);

	//UTF8转码成Unicode
	std::wstring UTF8ToUnicode(const char* pszSource);

	//Unicode转码成UTF8
	std::string UnicodeToUTF8(const wchar_t* pwszSource);

	//配置SSL参数
	void ConfigSSL(void* pContent);

	//配置网络参数
	void ConfigNetwork(void* pContent);

private:
	bool m_bDebug;	//是否启用日志
	FtpType m_oFtpType;			//ftp类型
	std::string m_strServer;	//服务器地址
	int m_nPort;				//服务器端口
	std::string m_strUser;		//服务器登录的用户名
	std::string m_strPassword;	//服务器登录的密码
};
