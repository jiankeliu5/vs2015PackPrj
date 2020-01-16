#ifndef _UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_ 
#define _UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_

#include <string>
using namespace std;

typedef int int32;

//设备状态
enum Equip_Status
{
	Equip_Status_Disable = 0,	//禁用
	Equip_Status_Enable = 1	//启用
};

//升级类型
enum Update_Type
{
	Update_Beta = 0,	//测试版
	Update_Offical = 1	//正式版
};

//云打印设备更新信息
typedef struct tag_Equip_Update_Info
{
	Equip_Status Status;//设备状态
	int EquipId;					//设备id
	bool isUpdate;					//是否需要升级
	string Version;					//升级版本号
	string UpdateUrl;				//升级包url
	string UpdateFilePath;			//升级文件本地路径
	string UpdateLog;				//升级日志
}Equip_Update_Info, *PEquip_Update_Info;


#endif //_UPDATEDEF_369E6394_A0B3_4F47_8CB6_B597F3934303_H_