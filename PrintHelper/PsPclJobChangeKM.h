/*����Ŀǰ˫���ӡֻ������˫�泤��*/
#pragma once
#include "pspcljobchange.h"

class CPsPclJobChangeKM : public CPsPclJobChange
{
public:
	CPsPclJobChangeKM(void);
	~CPsPclJobChangeKM(void);

protected:
	//���нӿڵķ���ֵ����ʾ�����Ƿ����޸�
	virtual BOOL ChangeJobStrToHB_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexV_PS(CStringA& szSource);
	virtual BOOL ChangeJobStrToDuplexH_PS(CStringA& szSource);
};
