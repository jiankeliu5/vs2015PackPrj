#pragma once

// 密码加密与验证


class  CPWDCry
{
public:
	CPWDCry(void);
	~CPWDCry(void);
public:
	char*		CryptPWD(char* strPwd);
	int		UnCryptPWD(char* strSec,char* szPwd,int& nSize);
	char*		CryptPWD(int nID, char* strPwd);

	int		CheckPWD(char* strPwd, char* strCry);
	int		CheckPWD(int nID, char* strPwd, char* strCry);
private:
	char*		Crypt(char* szData, int nLength, int nID = -1);

	int		PutData(char* szData, int nSize, unsigned char* pInfo, unsigned char* pPos, int nInfoLen);
	int		GetData(char* szData, int nSize, unsigned char* pInfo, unsigned char* pPos, int nInfoLen);
	void		Data2String(char* szData, int nLength, char* str);
	char*		String2Data(char* szData, int& nLength);
};
