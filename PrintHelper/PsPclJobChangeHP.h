#pragma once
#include "pspcljobchange.h"

class CPsPclJobChangeHP : public CPsPclJobChange
{
public:
	CPsPclJobChangeHP(void);
	~CPsPclJobChangeHP(void);

protected:
	//���нӿڵķ���ֵ����ʾ�����Ƿ����޸�
	virtual BOOL ChangeJobStrToHB_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexH_PS(CStringA& szSource);

	virtual BOOL ChangeJobStrToHB_PCL(CStringA& szSource);
};
