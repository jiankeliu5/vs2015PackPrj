#pragma once

#include "../Public/Global.h"
#include "../Public/Include/MultiLanguageDef.h"
#include <map>
using namespace std;

#define MultiLanguageString	CMultiLanguage::GetInstance().GetString

#define Enable_MFC_CString_Support 1

class _AA_DLL_EXPORT_ CMultiLanguage
{
protected:
	CMultiLanguage();
public:
	~CMultiLanguage();
	static CMultiLanguage& GetInstance();
#if Enable_MFC_CString_Support
	CString GetString(CString& strKey, const CString strDefault, UiLanguage language = UILANG_CHINESE);
	CString GetString(const TCHAR* strKey, const CString strDefault, UiLanguage language = UILANG_CHINESE);
#else
	stdtstring GetString(stdtstring& strKey, const stdtstring strDefault, UiLanguage language = UILANG_CHINESE);
	stdtstring GetString(const TCHAR* strKey, const stdtstring strDefault, UiLanguage language = UILANG_CHINESE);
#endif

protected:
	void LoadLanguage();
	bool IsIniFile(const TCHAR* strFilePath);
	void AddOneLanguage(const TCHAR* strLanguageFilePath, DWORD dwFileSize);

private:
	typedef map<stdtstring, stdtstring> LanguageMap;	//<key,string>
	typedef map<UiLanguage, LanguageMap> MultiLanguageMap;
	MultiLanguageMap m_oMultiLanguageMap;
	CCriticalSection2 m_cs4Language;
};
