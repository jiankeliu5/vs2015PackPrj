#include "StdAfx.h"
#include "PWDCry.h"
#include "Twofish.h"
/*
密码加密原理：
1. 把密码长度扩充到16的整数倍，增加的值为0。
2. 获取占用8字节的用户信息，其中实际使用5字节，1字节表示长度，4个字节表示用户id。
3. 获取位置信息，占用字节数与实际使用长度与用户信息相同。其中的值是随机的，范围是0-密码长度。
4. 组合数据，其中用户信息是从后往前排列的：
----------------------------------------
|密码		|位置信息		|	用户信息|
----------------------------------------
5. 根据位置信息交换密码与用户信息数据。
6. 将数据加密（twofish）。
7. 将加密后的数据转换成显示字符。转换方法：每个字节的高4位与低4位各自加上‘A’，这样一个字节变成2个‘A’-‘P’之间的字母。
8. 增加‘Q’-‘Z’之间的字母。方法：在8个固定位置中，每次插入2个随机字母（范围‘A’-‘Z’）。
9. 加密完成。
10.用户信息长度由INFO_LENGTH控制，如果要增加额外信息，需要修改INFO_LENGTH和INFO_SPAN宏。
验证原理：
1. 验证方式是将加密后的密码解密后得到用户密码原文，然后与传入的用户密码比较。
2. 将显示字符转换成数据，忽略其中8个固定位置。
3. 解密
4. 根据位置信息还原密码与用户信息
5. 分别比较密码和用户信息是否相符。
6. 验证完成。

注意：小写字母比大写字母大0x20，BYTE类型的大小写字母左移一位后的值相同，所以显示字符是不区分大小写的。

*/
TwofishInstance instance;
CPWDCry::CPWDCry(void)
{
	//u4byte key[] = {1, 2, 4};
	u4byte key[128] = {
		0xB5, 0xC7, 0xC2, 0xBC, 0xB5, 0xC7, 0xC2, 0xBC, 0xC3, 0xFB, 0xC3, 0xDC, 0xC2, 0xEB, 0x20, 0xD1, 
		0xA1, 0xD4, 0xF1, 0xC8, 0xA5, 0xCF, 0xF2, 0x20, 0xC3, 0xE2, 0xB7, 0xD1, 0xD3, 0xCA, 0xCF, 0xE4, 
		0x20, 0x56, 0x49, 0x50, 0xD3, 0xCA, 0xCF, 0xE4, 0x20, 0x32, 0x30, 0x30, 0x38, 0xD3, 0xCA, 0xCF, 
		0xE4, 0x20, 0xCD, 0xAC, 0xC3, 0xFB, 0xD3, 0xCA, 0xCF, 0xE4, 0x20, 0x43, 0x4E, 0xD3, 0xCA, 0xCF, 
		0xE4, 0x20, 0xCE, 0xA2, 0xB2, 0xA9, 0x20, 0xB2, 0xA9, 0xBF, 0xCD, 0x20, 0xCA, 0xD3, 0xC6, 0xB5, 
		0x20, 0xC2, 0xDB, 0xCC, 0xB3, 0x20, 0xBB, 0xE1, 0xD4, 0xB1, 0xD6, 0xD0, 0xD0, 0xC4, 0x20, 0xC4, 
		0xFA, 0xBA, 0xC3, 0xA3, 0xAC, 0x73, 0x32, 0x30, 0x31, 0x31, 0x31, 0x30, 0x32, 0x36, 0xD1, 0xA1, 
		0xD4, 0xF1, 0xC8, 0xA5, 0xCF, 0xF2, 0x20, 0xC3, 0xE2, 0xB7, 0xD1, 0xD3, 0xCA, 0xCF, 0xE4, 0x20
	};
	ZeroMemory(&instance, sizeof(instance));
	twofish_set_key(&instance, key, sizeof(key) / sizeof(u4byte));
}

CPWDCry::~CPWDCry(void)
{
}

char* CPWDCry::CryptPWD(char* strPwd)
{
	return CryptPWD(-1, strPwd);
}

// 每个info块的长度，一共2块。
#define INFO_SPAN	(8)
#define INFO_LENGTH (5)

#define MIX_BYTE_COUNT	16
unsigned char nMixIndex[MIX_BYTE_COUNT/2] =	{1, 5, 10, 15, 20, 25, 26, 30};

char* CPWDCry::CryptPWD(int nID, char* strPwd)
{
	const int nSpan = INFO_SPAN * 2;

	ASSERT (nSpan % 16 == 0);

	srand(GetTickCount());

	//CTime t = CTime::GetCurrentTime();
	__int64 now = 0.0;//t.GetTime();

	unsigned char nInfoLength = 1/*infolen*/ + 
		//sizeof(__int64)/*time*/ +
		4/*id*/ ;
	
	//nInfoLength *= 2; /*position*/

	ASSERT((nInfoLength * 2) <= nSpan);
	ASSERT(nInfoLength == INFO_LENGTH);

	// 记录每个info位的位置
	unsigned char* pPosition = new unsigned char[nInfoLength ];
	unsigned char* pInfo = new unsigned char[nInfoLength];
	
	ZeroMemory(pPosition, nInfoLength);
	ZeroMemory(pInfo, nInfoLength);

	int nCopy = sizeof(nInfoLength);
	unsigned char* pTemp = pInfo;
	memcpy(pTemp, &nInfoLength, nCopy);
	pTemp += nCopy;
	
	nCopy = sizeof(nID);
	memcpy(pTemp, &nID, nCopy);
	pTemp += nCopy;
#if 0
	nCopy = sizeof(now);
	memcpy(pTemp, &now, nCopy);
	pTemp += nCopy;
#endif

	// 按16个字节对齐
	int nFix = 0;
	int nLength = strlen(strPwd);
	if (nLength < 16)
	{
		nFix = 16 - nLength;
	}
	else if (nLength % 16 != 0)
	{
		nFix = 16 - nLength % 16;
	}
	nLength += nFix;
	
	// 随机出position
	unsigned char* pPos = pPosition;
	for (int i = 0; i < nInfoLength; i++, pPos++)
	{
		BOOL bRepeat = FALSE;
		do 
		{
			(*pPos) = rand() % nLength;

			bRepeat = FALSE;
			for (int j = 0; j < i; j++)
			{
				if (*(pPosition + j) == *pPos)
				{
					bRepeat = TRUE;
					break;
				}
			}
		} while (bRepeat);
	}
	nLength += nSpan;

	char* szData = new CHAR[nLength];
	ZeroMemory(szData, nLength);
	memcpy(szData, strPwd, strlen(strPwd));

	PutData(szData, nLength, pInfo, pPosition, nInfoLength);
	
	for (int i = 0; i < nLength; i += 16)
	{
		twofish_encrypt(&instance, (const unsigned int *)(szData + i), (unsigned int *)(szData + i));
	}
	int nOutLength = nLength * 2 + MIX_BYTE_COUNT + 1;
	char* out = new char[nOutLength];
	
	ZeroMemory(out, nOutLength);

	// 形成字符串
	Data2String(szData, nLength, out);
	delete szData;
	delete pInfo;
	delete pPosition;
	return out;
}

void CPWDCry::Data2String(char* szData, int nLength,  char* out)
{
	ASSERT(nLength >= 16 + INFO_SPAN * 2);
	if (nLength < 16 + INFO_SPAN * 2)
		return;

	char* pCur = out;
	for (int i = 0; i < nLength; i++)
	{
		for (int j= 0; j < 8; j++)
		{
			if (i == nMixIndex[j])
			{
#if 0
				str.AppendChar(rand() % 10 + 'Q');
				str.AppendChar(rand() % 10 + 'Q');
#else
				*pCur = rand() % 26 + 'A';
				pCur++;
				*pCur = rand() % 26 + 'A';
				pCur++;
				//str.AppendChar(rand() % 26 + 'A');
				//str.AppendChar(rand() % 26 + 'A');
#endif
			}
		}
		int a = szData[i] & 0x0F;
		ASSERT(a <= 25);
	//	str.AppendChar('A' + a);
		*pCur = a + 'A';
		pCur++;
		a = (szData[i] & 0xF0) >> 4;
		ASSERT(a <= 25);
		//str.AppendChar('A' + a);
		*pCur = a + 'A';
		pCur++;
	}
}
char* CPWDCry::String2Data(char* szData, int& nOutLength)
{
	nOutLength = 0;
	int nLen = strlen(szData);
	if (nLen == 0)
	{
		ASSERT(0);
		return NULL;
	}
	if (nLen % 2 == 1)
	{
		ASSERT(0);
		return NULL;
	}
	nOutLength = (nLen - MIX_BYTE_COUNT) / 2;
	if (nOutLength < 1)
	{
		ASSERT(0);
		return NULL;
	}
	char* pData = new char[nOutLength];
	memset(pData,0,nOutLength);
	unsigned char nJmp = 0;
	for (int i = 0; i < nOutLength; i++)
	{
		BOOL bJmp = FALSE;
		for (int j= 0; j < 8; j++)
		{
			if (i == nMixIndex[j])
			{
				bJmp = TRUE;
				break;
			}
		}
		if (bJmp)
		{
			nJmp += 2;
		}
		unsigned char a = szData[i * 2 + nJmp];
		a -= 'A';
		a &= 0xF;
		unsigned char b = (szData[i * 2 + 1 + nJmp]);
		b -= 'A';
		b <<= 4;
		b &= 0xF0;
		pData[i] = a +  b;
	}
	return pData;
}
int CPWDCry::CheckPWD(char* strPwd, char* strCry)
{
	return CheckPWD(-1, strPwd, strCry);
}

int CPWDCry::CheckPWD(int nID, char* strPwd, char* strCry)
{
#if 0
	// 对原密码进行计算
	CString strResult;
	strResult.Format("%d--%s", nID, strPwd);

	// 比较计算结果
	if (strResult.Compare(strCry) == 0)
		return TRUE;
#else
	// 对密码进行解密
	int nLength;
	char* szData = String2Data(strCry, nLength);
	if ((szData == NULL) || (nLength == 0))
		return FALSE;
	if (nLength < 32)
		return FALSE;

	for (int i = 0; i < nLength; i += 16)
	{
		twofish_decrypt(&instance, (const unsigned int *)(szData + i), (unsigned int *)(szData + i));
	}

	unsigned char nInfoLen = INFO_LENGTH;
	if ((nInfoLen > nLength) || (nLength == 0))
		return FALSE;
	unsigned char* pInfo = new unsigned char[nInfoLen];
	unsigned char* pPos = new unsigned char[nInfoLen];

	ZeroMemory(pInfo, nInfoLen);
	ZeroMemory(pPos, nInfoLen);

	BOOL bEqual = TRUE;
	if (!GetData(szData, nLength, pInfo, pPos, nInfoLen))
		bEqual = FALSE;
	else
	{
		int nSpanTailStart = nLength - INFO_SPAN * 2;
		// 前面有判断nLength必定大于INFO_SPAN * 2
		// 当密码长度刚好是16倍数时，没有结束符。
		// 赋值会破坏位置信息，但是不影响用户信息。
		szData[nSpanTailStart] = '\0';
		
		int* pNiD = (int*)(pInfo + 1);
		if (*pNiD != nID)
			bEqual = FALSE;
		if (strcmp(szData, strPwd))
			bEqual = FALSE;
	}
	delete pInfo;
	delete pPos;
	delete szData;
	return bEqual;
#endif
	return FALSE;
}

int CPWDCry::UnCryptPWD(char* strSec,char* szPwd,int& nSize)
{
	int nID = -1;
	int nLength;
	char* szData = String2Data(strSec, nLength);
	if ((szData == NULL) || (nLength == 0))
		return FALSE;
	if (nLength < 32)
		return FALSE;

	for (int i = 0; i < nLength; i += 16)
	{
		twofish_decrypt(&instance, (const unsigned int *)(szData + i), (unsigned int *)(szData + i));
	}

	unsigned char nInfoLen = INFO_LENGTH;
	if ((nInfoLen > nLength) || (nLength == 0))
		return FALSE;
	unsigned char* pInfo = new unsigned char[nInfoLen];
	unsigned char* pPos = new unsigned char[nInfoLen];

	ZeroMemory(pInfo, nInfoLen);
	ZeroMemory(pPos, nInfoLen);

	BOOL bEqual = TRUE;
	if (!GetData(szData, nLength, pInfo, pPos, nInfoLen))
		bEqual = FALSE;
	else
	{
		int* pNiD = (int*)(pInfo + 1);
		if (*pNiD != nID)
			bEqual = FALSE;
		
		int nSpanTailStart = nLength - INFO_SPAN * 2;
		// 前面有判断nLength必定大于INFO_SPAN * 2
		// 当密码长度刚好是16倍数时，没有结束符。
		// 赋值会破坏位置信息，但是不影响用户信息。
		szData[nSpanTailStart] = '\0';

		strcpy_s(szPwd,nSize,szData);
	}
	delete pInfo;
	delete pPos;
	delete szData;
	return bEqual;
	return FALSE;
}
// 把info放在pos处，原先的数据移到最后一节的-i处
int CPWDCry::PutData(char* szData, int nSize, unsigned char* pInfo, unsigned char* pPos, int nInfoLen)
{
	memcpy(szData + nSize - INFO_SPAN * 2, pPos, nInfoLen);
	for (int i = 0; i < nInfoLen; i++, pPos++, pInfo++)
	{
		unsigned char pos = *pPos;
		ASSERT(pos < nSize);
		if (pos  >= nSize)
			return FALSE;
		unsigned char temp = szData[pos];
		szData[pos] = *pInfo;
		szData[nSize - 1 - i] = temp;
	}
	return TRUE;
}

BOOL CPWDCry::GetData(char* szData, int nSize, unsigned char* pInfo, unsigned char* pPos, int nInfoLen)
{
	// 获取pos
	memcpy(pPos, szData + nSize - INFO_SPAN * 2, nInfoLen);

	for (int i = 0; i < nInfoLen; i++, pPos++, pInfo++)
	{
		unsigned char pos = *pPos;
		ASSERT(pos < nSize);
		if (pos  >= nSize)
			return FALSE;
		unsigned char temp = szData[pos];
		szData[pos] = szData[nSize - 1 - i];
		*pInfo = temp;
	}
	return TRUE;
}