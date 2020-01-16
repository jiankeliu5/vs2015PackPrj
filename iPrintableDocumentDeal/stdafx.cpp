// stdafx.cpp : 只包括标准包含文件的源文件
// iPrintableDocumentDeal.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CStringA MultiByteToUTF8(const char* puszSource)
{
	CStringA szRetA = "";
	if (puszSource)
	{
		int len = MultiByteToWideChar(CP_GB2312, 0, (LPCSTR)puszSource, -1, NULL, 0);
		wchar_t* wszText = new wchar_t[len + 1];
		memset(wszText, 0, len * 2 + 2);
		MultiByteToWideChar(CP_GB2312, 0, (LPCSTR)puszSource, -1, wszText, len);

		len = WideCharToMultiByte(CP_UTF8, 0, wszText, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wszText, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] wszText;
		delete[] szText;
	}
	return szRetA;
}

CStringA UnicodeToUTF8(const wchar_t* pwszSource)
{
	CStringA szRetA = "";
	if (pwszSource)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, pwszSource, -1, NULL, 0, NULL, NULL);
		char *szText = new char[len + 1];
		memset(szText, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, pwszSource, -1, szText, len, NULL, NULL);

		szRetA = szText;
		delete[] szText;
	}
	return szRetA;
}
