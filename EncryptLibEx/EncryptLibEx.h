#pragma once

typedef enum
{
	EN_ENCRYPT_MD5	= 1		,
	EN_ENCRYPT_BASE64		,
	EN_ENCRYPT_DES			,
	EN_ENCRYPT_TWOFILSH, 	//该加密每次加密会取时间戳，结果不一样
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

/*根据入参机器ID和加密算法，进行加密
  将加密后的内容与指定文件内的内容进行比对
  比对成功返回true，比对失败时，根据参数bExitIfFail做不同的动作
*/
bool CheckValidVerion(char * pMachineID	//机器ID字符串
					  , char *pFilePath	//认证文件绝对路径
					  , int nEncryptType = EN_ENCRYPT_MD5
					  , bool bExitIfFail = false	//比对失败后是否直接退出进程
					  );