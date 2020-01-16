#pragma once
#include "pspcljobchange.h"

class CPsPclJobChangeHP : public CPsPclJobChange
{
public:
	CPsPclJobChangeHP(void);
	~CPsPclJobChangeHP(void);

protected:
	//下列接口的返回值，表示内容是否有修改
	virtual BOOL ChangeJobStrToHB_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexH_PS(CStringA& szSource);

	virtual BOOL ChangeJobStrToHB_PCL(CStringA& szSource);
};
