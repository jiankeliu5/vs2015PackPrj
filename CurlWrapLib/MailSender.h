#pragma once

#include <string>
#include <list>
#include <fstream>

using namespace std;

class CMailSender
{
public:
	CMailSender(void);
	~CMailSender(void);

	//�����ʼ�����
	void SetCharset(const char* pCharset);

	//�����ʼ���������Ϣ
	void SetServer(const char* pcServer, int nPort, const char* pcUser, const char* pcPassword, bool bUseSSL, bool bUseTLS);

	//�����ʼ���������Ϣ��Ϣ
	void SetSender(const char* pcFrom, const char* pSenderName = NULL);

	//���һ���ռ���
	void AddRecipient(const char* pcTo);

	//����ռ���
	void ClearRecipient();

	//���һ��������
	void AddCarbonCopy(const char* pcCC);

	//��ճ�����
	void ClearCarbonCopy();

	//�����ʼ�����
	void SetSubject(const char* pcSubject);

	//�����ʼ�����
	void SetContent(const char* pcContent);

	//���һ������(�ļ�·��)��pcAttachmentʹ��UTF8����
	void AddAttachment(const char* pcAttachment);

	//��ո���
	void ClearAttachment();

	//ִ�з��Ͳ���
	bool Send(int& nError, char* pBuf, int nBufSize);

	//���������Ƿ��Ѿ�׼������
	bool IsReadyToSend();

	//�����Ƿ�������־
	void EnableDebug(bool bDebug);

protected:
	string m_strCharset;	//�ʼ�����
	string m_strServer;		//�ʼ���������ַ
	int m_nPort;			//�ʼ��������˿�
	bool m_bUseSSL;			//�Ƿ�ʹ��SSL����
	bool m_bUseTLS;			//�Ƿ�ʹ��STARTTLS����
	string m_strUser;		//�������û���
	string m_strPassword;	//����������

	string m_strSubject;	//�ʼ�����
	string m_strContent;	//�ʼ�����
	string m_strSenderName;	//�ʼ�����������
	string m_strSenderMail;		//�ʼ�����������
	list<string> m_oListRecipient;	//�ʼ��ռ����б�
	list<string> m_oListCarbonCopy;	//�ʼ��������б�
	list<string> m_oListAttachment;	//�ʼ������б�
	string m_strSendData;	//����MIMEЭ���ַ���

	static size_t payload_source(void *ptr, size_t size, size_t nmemb, void* userp);
	static int debug_callback(void* curl, int type, char * str, size_t size, void* uerp);

	void ComposeData();
	size_t m_nSentDataSize;
	size_t m_nTotalDataSize;
	fstream m_oLogFile;
	bool m_bDebug;

	string Base64Encode(const unsigned char * pucSource, unsigned int nlen);  
	string GetEncodedFileName(const string& strFileName);
	string GetFileName(const string& strFilePath);
	string GenerateMessageId();
};
