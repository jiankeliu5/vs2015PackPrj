#pragma once

typedef enum
{
	EN_ENCRYPT_MD5	= 1		,
	EN_ENCRYPT_BASE64		,
	EN_ENCRYPT_DES			,
	EN_ENCRYPT_TWOFILSH, 	//�ü���ÿ�μ��ܻ�ȡʱ����������һ��
	EN_ENCRYPT_MD5_BY16
};

#ifndef IN
#	define IN
#endif

#ifndef OUT
#	define OUT
#endif

bool EncryptLibEx( IN const char*pBufIn
				  , IN int uBufInLen
				  , IN OUT char *pBufOut
				  , IN OUT int &uBufOutLen
				  , int nEncryptType = EN_ENCRYPT_MD5);

/*������λ���ID�ͼ����㷨�����м���
  �����ܺ��������ָ���ļ��ڵ����ݽ��бȶ�
  �ȶԳɹ�����true���ȶ�ʧ��ʱ�����ݲ���bExitIfFail����ͬ�Ķ���
*/
bool CheckValidVerion(char * pMachineID	//����ID�ַ���
					  , char *pFilePath	//��֤�ļ�����·��
					  , int nEncryptType = EN_ENCRYPT_MD5
					  , bool bExitIfFail = false	//�ȶ�ʧ�ܺ��Ƿ�ֱ���˳�����
					  );