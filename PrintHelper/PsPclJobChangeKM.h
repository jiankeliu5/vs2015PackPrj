/*科美目前双面打印只看到过双面长边*/
#pragma once
#include "pspcljobchange.h"

class CPsPclJobChangeKM : public CPsPclJobChange
{
public:
	CPsPclJobChangeKM(void);
	~CPsPclJobChangeKM(void);

protected:
	//下列接口的返回值，表示内容是否有修改
	virtual BOOL ChangeJobStrToHB_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexH_PS(CStringA& szSource);
};
