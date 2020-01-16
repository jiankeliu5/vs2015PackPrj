#include "stdafx.h"
#include "MultiLanguage.h"
#include <Windows.h>

CMultiLanguage::CMultiLanguage()
{
	LoadLanguage();
}

CMultiLanguage::~CMultiLanguage()
{
}

CMultiLanguage& CMultiLanguage::GetInstance()
{
	static CMultiLanguage one;
	return one;
}

void CMultiLanguage::LoadLanguage()
{
	CCriticalSection2::Owner ow(m_cs4Language);

	//遍历language目录下的ini文件。
	TCHAR strModulePath[MAX_PATH] = { 0 };
	if (GetModuleFileName(0, strModulePath, MAX_PATH) == 0)
	{
		_tcscpy_s(strModulePath, _T("."));
	}
	else
	{
		TCHAR *pDest;
		pDest = _tcsrchr(strModulePath, _T('\\'));
		if (pDest)
			*pDest = 0;
	}

	TCHAR strPath[MAX_PATH] = { 0 };
	//此处好像不能过滤ini，暂时查找所有，然后判断是否为.ini文件。
	//_stprintf_s(strPath, _T("%s\\language\\*.ini"), strModulePath);
	_stprintf_s(strPath, _T("%s\\language\\*"), strModulePath);
	theLog.Write(_T("CMultiLanguage::LoadLanguage,1,strPath=%s"), strPath);

	WIN32_FIND_DATA oWin32FindData;
	memset(&oWin32FindData, 0x0, sizeof(WIN32_FIND_DATA));
	HANDLE hFindFile = FindFirstFile(strPath, &oWin32FindData);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		theLog.Write(_T("!!CMultiLanguage::LoadLanguage,FindFirstFile fail,error=%u"), GetLastError());
		return;
	}

	while (FindNextFile(hFindFile, &oWin32FindData))
	{
		if (!IsIniFile(oWin32FindData.cFileName))
		{
			continue;
		}

		TCHAR strLanguageFilePath[MAX_PATH] = { 0 };
		_stprintf_s(strLanguageFilePath, _T("%s\\language\\%s"), strModulePath, oWin32FindData.cFileName);
		AddOneLanguage(strLanguageFilePath, oWin32FindData.nFileSizeLow);
	}

	FindClose(hFindFile);
}

bool CMultiLanguage::IsIniFile(const TCHAR* strFilePath)
{
	stdtstring strTemp(strFilePath);
	if (strTemp.length() <= 4)
	{
		return false;
	}
	if (strTemp.compare(strTemp.length() - 4, 4, _T(".ini")) == 0)
	{
		return true;
	}
	return false;
}

void CMultiLanguage::AddOneLanguage(const TCHAR* strLanguageFilePath, DWORD dwFileSize)
{
	theLog.Write(_T("CMultiLanguage::AddOneLanguage,1,strLanguageFilePath=%s"), strLanguageFilePath);

	TCHAR* strBuffer = new TCHAR[dwFileSize];
	TCHAR* strBuffer2 = new TCHAR[dwFileSize];
	memset(strBuffer, 0x0, sizeof(TCHAR)*dwFileSize);
	UiLanguage oLang = (UiLanguage)GetPrivateProfileInt(_T("Language"), _T("Code"), 0, strLanguageFilePath);
	//theLog.Write(_T("CMultiLanguage::AddOneLanguage,2,Code=%d"), oLang);
	GetPrivateProfileString(_T("Language"), _T("Type"), _T(""), strBuffer, dwFileSize, strLanguageFilePath);
	//theLog.Write(_T("CMultiLanguage::AddOneLanguage,3,Type=%s"), strBuffer);
	GetPrivateProfileString(_T("Language"), _T("Name"), _T(""), strBuffer, dwFileSize, strLanguageFilePath);
	theLog.Write(_T("CMultiLanguage::AddOneLanguage,4,oLang=%d,Name=%s"), oLang, strBuffer);

	MultiLanguageMap::iterator it = m_oMultiLanguageMap.find(oLang);
	if (it == m_oMultiLanguageMap.end())
	{
		LanguageMap oLanguageMap;
		//首先获取所有Key
		memset(strBuffer, 0x0, sizeof(TCHAR)*dwFileSize);
		GetPrivateProfileString(_T("Text"), NULL, _T(""), strBuffer, dwFileSize, strLanguageFilePath);
		TCHAR* pStr = strBuffer;
		DWORD dwKeyLen = _tcslen(pStr);
		while (dwKeyLen > 0)
		{
			GetPrivateProfileString(_T("Text"), pStr, _T(""), strBuffer2, dwFileSize, strLanguageFilePath);
			oLanguageMap.insert(pair<stdtstring, stdtstring>(pStr, strBuffer2));
			pStr = pStr + (dwKeyLen + 1);
			dwKeyLen = _tcslen(pStr);
		}

		m_oMultiLanguageMap.insert(pair<UiLanguage, LanguageMap>(oLang, oLanguageMap));
	}

	delete[] strBuffer;
	delete[] strBuffer2;
}

#if Enable_MFC_CString_Support
CString CMultiLanguage::GetString(CString& strKey, const CString strDefault, UiLanguage language /*= UILANG_CHINESE*/)
{
	MultiLanguageMap::iterator it = m_oMultiLanguageMap.find(language);
	if (it == m_oMultiLanguageMap.end())
	{
		return strDefault;
	}
	else
	{
		LanguageMap& oLanguageMap = it->second;
		LanguageMap::iterator it2 = oLanguageMap.find(strKey.GetString());
		if (it2 == oLanguageMap.end())
		{
			return strDefault;
		}
		else
		{
			return it2->second.c_str();
		}
	}
}

CString CMultiLanguage::GetString(const TCHAR* strKey, const CString strDefault, UiLanguage language /*= UILANG_CHINESE*/)
{
	CString strKey2(strKey);
	return GetString(strKey2, strDefault, language);
}
#else
stdtstring CMultiLanguage::GetString(stdtstring& strKey, const stdtstring strDefault, UiLanguage language /*= UILANG_CHINESE*/)
{
	MultiLanguageMap::iterator it = m_oMultiLanguageMap.find(language);
	if (it == m_oMultiLanguageMap.end())
	{
		return strDefault;
	}
	else
	{
		LanguageMap& oLanguageMap = it->second;
		LanguageMap::iterator it2 = oLanguageMap.find(strKey);
		if (it2 == oLanguageMap.end())
		{
			return strDefault;
		}
		else
		{
			return it2->second;
		}
	}
}

stdtstring CMultiLanguage::GetString(const TCHAR* strKey, const stdtstring strDefault, UiLanguage language /*= UILANG_CHINESE*/)
{
	stdtstring strKey2(strKey);
	return GetString(strKey2, strDefault, language);
}
#endif
