#ifndef _UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_ 
#define _UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_

#include <string>
using namespace std;

typedef int int32;

//�豸״̬
enum Equip_Status
{
	Equip_Status_Disable = 0,	//����
	Equip_Status_Enable = 1	//����
};

//��������
enum Update_Type
{
	Update_Beta = 0,	//���԰�
	Update_Offical = 1	//��ʽ��
};

//�ƴ�ӡ�豸������Ϣ
typedef struct tag_Equip_Update_Info
{
	Equip_Status Status;//�豸״̬
	int EquipId;					//�豸id
	bool isUpdate;					//�Ƿ���Ҫ����
	string Version;					//�����汾��
	string UpdateUrl;				//������url
	string UpdateFilePath;			//�����ļ�����·��
	string UpdateLog;				//������־
}Equip_Update_Info, *PEquip_Update_Info;


#endif //_UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_