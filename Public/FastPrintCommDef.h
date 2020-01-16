/************************************************************************/
/* 本文件最后修改日期：2016-09-20                                       */
/************************************************************************/

#ifndef _FastPrintCommDef_H_
#define _FastPrintCommDef_H_

//pipe
#define Pipe_iSecClientUi		_T("\\\\.\\pipe\\{47FF24E9-A370-4C00-941D-0FEDC4A7B111}")
#define Pipe_iSecClient			_T("\\\\.\\pipe\\{2945015C-710B-4F8C-8730-83EA936F6D6A}")
#define Pipe_iSecModuleMgr		_T("\\\\.\\pipe\\{8965B121-DB9D-40AD-A71B-356E90613D38}")
#define Pipe_iSecModule			_T("\\\\.\\pipe\\{E68B4300-2A92-4AE1-B9FF-3E5E87A3D4C5}")


#define DOWNLOAD_RETRY_NUMBER	3	//下载失败重试次数
#define MAX_PAY_WAIT_SECOND		20	//支付最长等待时间
#define UI_PARAM_FAILURE_TIME	5	//Ui调用c++接口参数失效间隔（秒）
#define REALTIME_DEDUCATION_TIME_SPAN	2//实时扣费间隔时间（秒）
#define READ_CARD_TIME_SPAN		500	//读卡频率（毫秒）

#define WM_FAST_BASE							(WM_USER + 2000)	//3024

//#define WM_MSG_QUE_OK							WM_FAST_BASE + 1

//Web服务端发送的MQ消息，cmd
#define WM_MfpUserLogin							WM_FAST_BASE + 1001 //用户登录打印机，扫码登录成功
#define WM_MfpUserLogOut						WM_FAST_BASE + 1002 //用户登出打印机

#define WM_UiSendToMainThread					WM_FAST_BASE + 2000 //UI向站点主线程发送的消息
//#define	WM_UiPrintJobs							WM_FAST_BASE + 2001 //打印作业
#define WM_ScanMockData							WM_FAST_BASE + 2101 //扫描模拟数据
//#define WM_UsbPrintMockData						WM_FAST_BASE + 2102 //USB打印模拟数据
#define WM_CheckPrinterMeter					WM_FAST_BASE + 2103 //抄表
#define WM_InitMQ								WM_FAST_BASE + 2222 //测试用

#define WM_InitCardReader						WM_FAST_BASE + 3000 //初始化读卡器
#define WM_ReadCardInfo							WM_FAST_BASE + 3001	//一卡通读卡信息
#define WM_ConsumeCardMoney						WM_FAST_BASE + 3002 //一卡通扣费
#define WM_RealtimeDeduction					WM_FAST_BASE + 3003 //一卡通实时扣费
#define WM_StopReadCard							WM_FAST_BASE + 3004 //停止一卡通操作

//pipe message
#define WM_Pip_AddOneMoudleTask					WM_FAST_BASE + 4001 //添加一个组件任务
#define WM_Pip_UpdateOneMoudleTask				WM_FAST_BASE + 4002 //修改一个组件任务
#define WM_Pip_EditOneMoudleTask				WM_FAST_BASE + 4003 //修改一个组件任务
#define WM_Pip_UpdateSyncTime					WM_FAST_BASE + 4004 //更新同步任务时间
#define WM_Pip_ShowModuleUi						WM_FAST_BASE + 4005 //显示管理器

#define WM_GetEnterpriseInfo					WM_FAST_BASE + 5003 //企业信息
#define WM_GetEquipmentInfo						WM_FAST_BASE + 5004 //设备信息

//连接层本地使用
#define WM_Link_BASE							(WM_FAST_BASE + 15000)

#pragma pack(push, 1)

#pragma pack(pop)

#define	 DEF_FASET_PRINT
#define	 ISEC_DOCCONVERT_EXE				TEXT("iSecDocConverter.exe")		//文件转换工具
#define	 ISEC_DOCCONVERTHELPER_EXE			TEXT("iSecDocConverterHelper.exe")	//文件转换帮助工具
#define  ISEC_PRINT_Client_Service				TEXT("iSecClientHost")	
#define	 ISEC_PRINT_CLIENTUI_EXE				TEXT("iSecClientUi.exe")			//打印客户端界面
#define	 ISEC_PRINT_CLIENTHOST_EXE			TEXT("iSecClientHost.exe")			//打印客户端服务
#define	 ISEC_MODULEHOST_EXE				TEXT("iSecModuleHost.exe")
#define	 ISEC_MODULEMGR_EXE					TEXT("iSecModuleMgr.exe")			//组件管理器
#define	 ISEC_SYSNC_EXE						TEXT("iSecSync.exe")				//用户同步工具
#define	 ISEC_DBBACUP_EXE					TEXT("iSecDbBackUp.exe")			//数据库备份工具
#define	 ISEC_MailSender_EXE				TEXT("iSecMailSend.exe")			//邮件发送工具
#define	 ISEC_TimingSwitcher_EXE			TEXT("iSecTimingSwitcher.exe")		//定时开关机

#define Update_Folder_Name					TEXT("UpdateFile")					//打包升级文件的文件夹名

#define Pack_Print_Client					TEXT("-PackPrintClient")		//打包打印客户端升级文件
#define Update_Print_Client					TEXT("-UpdatePrintClient")		//打印客户端升级参数
#define Copy_Print_Client					TEXT("-CopyPrintClient")		//打印客户端执行覆盖参数
#define Print_Update_Exe					TEXT("iSecUpdate.exe")			//打印升级进程

#define GLOBAL_MUTEX_DOCCONVERTER_NAME				_T("Global\\DocConvert")
#define GLOBAL_MUTEX_PRINTCLIENTHOST				_T("Global\\PrintClientHost")
#define GLOBAL_MUTEX_PRINTCLIENTUI					_T("Global\\PrintClientUi")
#define GLOBAL_MUTEX_MODULEMGR						_T("Global\\iSecModuleMgr")
#define ISEC_MODULE_SQLITE_MUTEX_NAME				_T("Sqlite_iSecModule")
#define ISEC_MODULE_DB_NAME							_T("isecmoduledb.dll")
#define ISEC_PRINTCLIENT_DB_NAME					_T("ISecPrintClient.dll")

#define ERROR_IP_ADDRESS						TEXT("0.0.0.0")
#define ERROR_MAC_ADDRESS						TEXT("00-00-00-00-00-00")

namespace FastErrCode
{//未完待续
	enum ISEC_ErrCode
	{
		ISEC_Web_UnKown	 = -1,					//未知错误信息
		ISEC_Web_Suc	 = 0,					//成功
		ISEC_Local_UnKownErr = 1,					//本地未知错误
		ISEC_Local_DataErr	 = 2,					//本地数据错误
		ISEC_Local_GetJobFail = 3,				//获取本地作业异常
		ISEC_Local_UserIdInValid = 4,				//用户id非法
		ISEC_Local_AddJobFail = 5,				//添加作业记录失败
		ISEC_Local_UploadFail = 6,				//上传作业失败
		ISEC_Local_NetErr = 6,					//网络错误
		ISEC_Web_Param		= 10001,			//基础类错误（参数验证类）
		ISEC_Web_NoUser =	10002,				//用户信息不存在
		ISEC_Web_NameOrPwd = 10003,				//用户名或者密码错误
		ISEC_Web_UserStateDisable = 10004,		//用户状态已禁用
		ISEC_Web_UserArrearage = 10005,			//用户已欠费
		ISEC_Web_UserActChangeFail = 10006,		//用户账户变更失败
		ISEC_Web_NoDept = 10007,				//部门不存在
		ISEC_Web_DeptDisable = 10008,			//部门不存在
		ISEC_Web_UserBindStationFail = 10009,	//部门已禁用
		ISEC_Web_UserLogoutFail = 10010,		//用户注销打印点失败
		ISEC_Web_NoEnterprise = 10050,			//企业不存在
		ISEC_Web_CardConsumeSaveFail = 10051,	//一卡通消费记录保存失败
		ISEC_Web_NoStation = 20001,				//打印点信息不存在
		ISEC_Web_LoginTimeOut = 20002,			//用户登录打印点超时
		ISEC_Web_LoginOtherStation = 20003,		//用户已在别的打印点登录
		ISEC_Web_OtherUserLoginStation = 20004,	//其他用户已登录此打印点
	};
}

#define QR_Code_Dir								TEXT("QR_Code")			//二维码保存路径
#define Print_Cache								TEXT("Print_Cache")		//打印保存总路径
#define Copy_Cache								TEXT("Copy_Cache")		//复印保存总路径
#define Scan_Cache								TEXT("Scan_Cache")		//扫描保存总路径
#define Update_Dir								TEXT("Update_Cache")	//升级保存目录
#endif //_CloudPrintRouterTmepDef_h_B9ED834F_A02F_4996_89E6_559841D7280D_