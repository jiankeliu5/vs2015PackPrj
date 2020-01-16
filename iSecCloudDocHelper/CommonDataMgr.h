#pragma once

#define GlobalData					CQtMgrGlobalDataConfig::GetInstance()			//��ȡȫ������ʵ���ĺ�

class CQtMgrGlobalDataConfig
{
protected:
	CQtMgrGlobalDataConfig();
public:
	static CQtMgrGlobalDataConfig& GetInstance();
	void LoadConfig();
	void CheckFileStorageList();

public:
	BOOL m_bCloudDocDealBeforeIsGz;	//���ĵ�����ǰ�Ƿ�ѹ��
	BOOL m_bCloudDocDealAfterIsGz;	//���ĵ�������Ƿ�ѹ��
	int m_nOfficeDealFormatType;	//��office�����ĵ�������ĸ�ʽ����ʱֻ֧�֡�pdf/tif�����֡�
	int m_nOfficeDealFormatDpi;		//��office�����ĵ�������Ϊ��.tif����ʽ��dpi,��.pdf����֧��dpi������
	int m_nImageDealFormatType;		//��ͼƬ�����ĵ�������ĸ�ʽ��֧�֡�jpg/png/bmp/tif/gif����
	int m_nImageDealFormatDpi;		//��ͼƬ�����ĵ�������Ϊ��setvalue5����ʽ��dpi��
	BOOL bImageUsePhysicalArea;		//��ͼƬ�����ĵ�����ʹͼƬ��СΪ��ӡ�豸����������,���Ǵ�ӡ����

};
