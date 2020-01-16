#pragma once
#include "..\Public\SplHelperDef.h"


class CPsPclJobChange
{
public:
	CPsPclJobChange(void);
	~CPsPclJobChange(void);
	virtual BOOL ChangeJob(CString szSplPath, DWORD dwColor, DWORD dwDuplex, SplType nSplType, CString szDocName=_T(""));

protected:
	virtual BOOL ChangeJob_PS(CString szSplPath, DWORD dwColor, DWORD dwDuplex, CString szDocName);
	virtual BOOL ChangeJob_PCL(CString szSplPath, DWORD dwColor, DWORD dwDuplex, CString szDocName);

	//下列接口的返回值，表示内容是否有修改
	virtual BOOL ChangeJobStrToHB_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToColor_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PS(CStringA& szSource);	//修改为双面长边
	virtual BOOL ChangeJobStrToDuplexH_PS(CStringA& szSource);	//修改为双面短边
	virtual BOOL ChangeJobDocName_PS(CStringA& szSource, CString szDocName);

	virtual BOOL ChangeJobStrToHB_PCL(CStringA& szSource);
	virtual BOOL ChangeJobStrToColor_PCL(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PCL(CStringA& szSource);	//修改为双面长边
	virtual BOOL ChangeJobStrToDuplexH_PCL(CStringA& szSource);	//修改为双面短边
	virtual BOOL ChangeJobDocName_PCL(CStringA& szSource, CString szDocName);

protected:
	BOOL ChangeJobDocName(CStringA& szSource, CString szDocName);

	DWORD ReadToBuf(FILE* pFile, BYTE* pBuf, DWORD dwLen);
	BOOL WriteFile(FILE* pSourceFile, __int64 nSourceFrom, __int64 nSourceTo, FILE* pDestFile, void* pBuffer, size_t nBufferLength);
};
