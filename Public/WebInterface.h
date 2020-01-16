#pragma once
#include <map>

#define MAX_MSG_LEN 128				//向ui传递消息的最大长度

#define  DEF_INTERFACE_WITH_PUBLIC 0

typedef enum tag_EN_MQCmd {
	enMQCmd_Begin = 10000,	//
	enMQCmd_UsbFileConvert = 10001,		//USB文件转换
	enMQCmd_UserLoginByQRCode = 10002,	//用户登录
	enMQCmd_StationInfoChange = 10003,	//web站点信息发生变更
	enMQCmd_StationPriceChange = 10004,	//web站点费率发生变更
	enMQCmd_StationPayFinish = 10005	//web站点支付成功
}EN_MQCmd;

typedef enum tag_EN_HttpCmd {
	enHttpCmd_GetToken	= 1,	//获取token
	enHttpCmd_UserLogin,		//用户登录
	enHttpCmd_StationUserLogin,	//站点用户登录
	enHttpCmd_UserLogout,		//登出
	enHttpCmd_GetUserInfo,		//获取用户信息
	enHttpCmd_GetPriceInfo,		//获取费率
	enHttpCmd_GetPrintJobList,	//获取未打印作业列表
	enHttpCmd_EditPrintJob,		//修改未打印作业
	enHttpCmd_DeletePrintJobs,	//删除未打印记录
	enHttpCmd_CreateOrder,		//创建订单
	enHttpCmd_PaymentNotice,	//支付通知
	enHttpCmd_PrintFinish,		//打印完成
	enHttpCmd_TaskCheck,		//校验退费
	enHttpCmd_AddScanJob,		//添加扫描记录
	enHttpCmd_ScanFinish,		//扫描完成	
	enHttpCmd_GetFtpInfo,		//获取ftp信息
	enHttpCmd_GetEntInfo,		//获取企业信息
	enHttpCmd_Register,			//注册工具
	enHttpCmd_GetNeedConvert,	//获取待转换的文件列表
	enHttpCmd_UpdateConvert,	//更新转换作业信息
	enHttpCmd_Recharge,			//充值
	enHttpCmd_AddCopy,			//添加复印记录并完成订单
	enHttpCmd_StationRegister,	//站点注册
	enHttpCmd_GetStationInfo,	//获取站点信息
	enHttpCmd_GetStationList,	//获取站点列表
	enHttpCmd_ReadMeter,		//抄表
	enHttpCmd_AddUsbFile,		//添加USB打印文件
	enHttpCmd_StationAlarm,		//站点预警(记录)
	enHttpCmd_StationAlarmStatus,//站点预警状态
	enHttpCmd_GetUserQRCode,	//获取用户登录的二维码（字符串）
	enHttpCmd_UserLoginConfirm,	//扫码登录确认
	enHtppCmd_AddSubmitJob,		//添加作业
	enHttpCmd_JobCheck,			//job级校验（退费）
	enHttpCmd_Upgrade,			//工具升级
	enHttpCmd_ToolConfig,      //获取配置信息
	enHttpCmd_ScanEmail,        //邮件推送
	enHttpCmd_GetRecentEmail,  //获取最近邮箱
	enHttpCmd_GetScanJob,	   //获取扫描作业
	enHttpCmd_UpdateScanJob,   //更新扫描作业
	enHttpCmd_UserIpass,   //一卡通消费记录
	enHttpCmd_StationEdit, //修改打印点信息
	enHttpCmd_End    
}EN_HttpCmd;

typedef std::map<int, CString> MAP_IntCString;
#if DEF_INTERFACE_WITH_PUBLIC
static MAP_IntCString g_httpInterface = {
	{ enHttpCmd_GetToken ,			_T("/public/index.php/api/Token/get") },
	{ enHttpCmd_StationUserLogin ,	_T("/public/index.php/api/v1.Station/userLogin") },
	{ enHttpCmd_GetUserInfo ,		_T("/public/index.php/api/v1.User/info") },
	{ enHttpCmd_GetPriceInfo ,		_T("/public/index.php/api/v1.Price/get") },
	{ enHttpCmd_GetPrintJobList ,	_T("/public/index.php/api/v1.Printing/get") },
	{ enHttpCmd_EditPrintJob ,		_T("/public/index.php/api/v1.Printing/edit") },
	{ enHttpCmd_DeletePrintJobs ,	_T("/public/index.php/api/v1.Printing/del") },
	{ enHttpCmd_CreateOrder ,		_T("/public/index.php/api/v1.Order/create") },
	{ enHttpCmd_UserLogout ,		_T("/public/index.php/api/v1.Station/logout") },
	{ enHttpCmd_PaymentNotice ,		_T("/public/index.php/api/v1.Order/payNotify") },
	{ enHttpCmd_PrintFinish ,		_T("/public/index.php/api/v1.Printing/finish") },
	{ enHttpCmd_TaskCheck ,			_T("/public/index.php/api/v1.Refund/taskCheck") },
	{ enHttpCmd_AddScanJob ,		_T("/public/index.php/api/v1.Scan/add") },
	{ enHttpCmd_ScanFinish , 		_T("/public/index.php/api/v1.Scan/finish") },
	{ enHttpCmd_GetFtpInfo , 		_T("/public/index.php/api/v1.Tools/ftp") },
	{ enHttpCmd_GetEntInfo ,		_T("/public/public/index.php/api/v1.Ent/info") },
	{ enHttpCmd_Register ,			_T("/public/public/index.php/api/v1.Tools/register") },
	{ enHttpCmd_GetNeedConvert ,	_T("/public/index.php/api/v1.Printing/beConvert") },
	{ enHttpCmd_UpdateConvert ,		_T("/public/index.php/api/v1.Printing/converted") },
	{ enHttpCmd_Recharge ,			_T("/public/index.php/api/v1.Order/recharge") },
	{ enHttpCmd_AddCopy ,			_T("/public/index.php/api/v1.Copy/add") },
	{ enHttpCmd_StationRegister,	_T("/public/index.php/api/v1.Station/register") },
	{ enHttpCmd_GetStationInfo ,	_T("/public/index.php/api/v1.Station/info") },
	{ enHttpCmd_ReadMeter,			_T("/public/index.php/api/v1.Station/meter") },
	{ enHttpCmd_AddUsbFile ,		_T("/public/index.php/api/v1.Printing/add") },
	{ enHttpCmd_UserLogin,			_T("/public/index.php/api/v1.User/login") },
	{ enHtppCmd_AddSubmitJob,		_T("/public/index.php/api/v1.Printing/add") },
	{ enHttpCmd_GetScanJob ,		_T("/index.php/api/v1.Push/email") },
	{ enHttpCmd_UpdateScanJob ,		_T("/index.php/api/v1.Push/response") },
	{ enHttpCmd_End ,				_T("") }
};
#else
static MAP_IntCString g_httpInterface = {
	{ enHttpCmd_GetToken ,			_T("/index.php/api/Token/get") },
	{ enHttpCmd_StationUserLogin ,	_T("/index.php/api/v1.Station/userLogin") },
	{ enHttpCmd_GetUserInfo ,		_T("/index.php/api/v1.User/info") },
	{ enHttpCmd_GetPriceInfo ,		_T("/index.php/api/v1.Price/get") },
	{ enHttpCmd_GetPrintJobList ,	_T("/index.php/api/v1.Printing/get") },
	{ enHttpCmd_EditPrintJob ,		_T("/index.php/api/v1.Printing/edit") },
	{ enHttpCmd_DeletePrintJobs ,	_T("/index.php/api/v1.Printing/del") },
	{ enHttpCmd_CreateOrder ,		_T("/index.php/api/v1.Order/create") },
	{ enHttpCmd_UserLogout ,		_T("/index.php/api/v1.Station/logout") },
	{ enHttpCmd_PaymentNotice ,		_T("/index.php/api/v1.Order/payNotify") },
	{ enHttpCmd_PrintFinish ,		_T("/index.php/api/v1.Printing/finish") },
	{ enHttpCmd_TaskCheck ,			_T("/index.php/api/v1.Refund/taskCheck") },
	{ enHttpCmd_AddScanJob ,		_T("/index.php/api/v1.Scan/add") },
	{ enHttpCmd_ScanFinish , 		_T("/index.php/api/v1.Scan/finish") },
	{ enHttpCmd_GetFtpInfo , 		_T("/index.php/api/v1.Tools/ftp") },
	{ enHttpCmd_GetEntInfo ,		_T("/index.php/api/v1.Ent/info") },
	{ enHttpCmd_Register ,			_T("/index.php/api/v1.Tools/register") },
	{ enHttpCmd_GetNeedConvert ,	_T("/index.php/api/v1.Printing/beConvert") },
	{ enHttpCmd_UpdateConvert ,		_T("/index.php/api/v1.Printing/converted") },
	{ enHttpCmd_Recharge ,			_T("/index.php/api/v1.Order/recharge") },
	{ enHttpCmd_AddCopy ,			_T("/index.php/api/v1.Copy/add") },
	{ enHttpCmd_StationRegister,	_T("/index.php/api/v1.Station/register") },
	{ enHttpCmd_GetStationInfo ,	_T("/index.php/api/v1.Station/info") },
	{ enHttpCmd_ReadMeter,			_T("/index.php/api/v1.Station/meter") },
	{ enHttpCmd_AddUsbFile ,		_T("/index.php/api/v1.Printing/add") },
	{ enHttpCmd_UserLogin,			_T("/index.php/api/v1.User/login") },
	{ enHtppCmd_AddSubmitJob,		_T("/index.php/api/v1.Printing/add") },
	{ enHttpCmd_GetStationList ,	_T("/index.php/api/v1.Station/stationList")},
	{ enHttpCmd_StationAlarm ,		_T("/index.php/api/v1.Station/alarm")},
	{ enHttpCmd_StationAlarmStatus ,_T("/index.php/api/v1.Station/alarmStatus")},
	{ enHttpCmd_GetUserQRCode ,		_T("/index.php/api/v1.User/qrcode")},
	{ enHttpCmd_UserLoginConfirm ,	_T("/index.php/api/v1.Station/userConfirm")},
	{ enHttpCmd_JobCheck ,			_T("/index.php/api/v1.Refund/jobCheck")},
	{ enHttpCmd_Upgrade ,			_T("/index.php/api/v1.Tools/upgrade")},
	{ enHttpCmd_ToolConfig ,		_T("/index.php/api/v1.Tools/config") },
	{ enHttpCmd_ScanEmail ,		    _T("/index.php/api/v1.Scan/email") },
	{ enHttpCmd_GetRecentEmail ,	_T("/index.php/api/v1.Scan/recent") },
	{ enHttpCmd_GetScanJob ,		_T("/index.php/api/v1.Push/email") },
	{ enHttpCmd_UpdateScanJob ,		_T("/index.php/api/v1.Push/response") },
	{ enHttpCmd_UserIpass ,		_T("/index.php/api/v1.User/ipass") },
	{ enHttpCmd_StationEdit,   _T("/index.php/api/v1.Station/edit")},
	{ enHttpCmd_End ,				_T("") }
};
#endif


inline CString GetMapValue(MAP_IntCString &map, int nKey)
{
	auto it = map.find(nKey);
	if (it != map.end())
	{
		return (*it).second;
	}
	return _T("");
}
typedef std::map<int, std::string> MAP_IntString;
inline std::string GetMapValue(MAP_IntString &map, int nKey)
{
	auto it = map.find(nKey);
	if (it != map.end())
	{
		return (*it).second;
	}
	return "";
}
inline int GetMapKey(MAP_IntString &map, std::string sKey)
{
	for (MAP_IntString::iterator it = map.begin(); it != map.end(); it++)
	{
		if (sKey.compare(it->second) == 0)
			return it->first;
	}
	return 0;
}
//ui程序和dll的接口
enum EN_StationFuncType {
	enStationFuncType_UserLogin = 1,	//登录
	enStationFuncType_UserLoginByCard,	//刷卡登录
	enStationFuncType_UserLogout,		//登出
	enStationFuncType_PrintJobs,		//打印
	enStationFuncType_GetUserInfo,		//获取用户信息
	enStationFuncType_GetPriceInfo,		//获取费率信息
	enStationFuncType_GetPrintJobList,	//获取待打印列表	
	enStationFuncType_EditPrintJob,		//修改待打印作业
	enStationFuncType_DeletePrintJobs,	//删除打印作业	
	enStationFuncType_Pay,				//支付（c++发起，调用js显示进行支付）
	enStationFuncType_ConfirmPayment,	//确认支付（js发起，通知c++确认进行支付（本地账户））
	enStationFuncType_PaymentCompleted,	//支付完成（c++发起，通知js支付完成）
	enStationFuncType_Delivery,			//投递到打印机
	enStationFuncType_StartScanning,	//开始扫描
	enStationFuncType_StopScanning,		//停止扫描（支付并发送）
	enStationFuncType_DeleteScanFiles,	//删除扫描文件
	enStationFuncType_ScanFileInfo,		//扫描文件信息
	enStationFuncType_Copy,				//开始复印
	enStationFuncType_CopyPay,			//复印付费
	enStationFuncType_CopyPayInfo,		//复印付费信息
	enStationFuncType_Recharge,			//充值
	enStationFuncType_CopyInfo,			//复印信息（单页）
	enStationFuncType_GetUsbFileList,	//获取USB文件列表
	enStationFuncType_FileConvert,		//文件转换
	enStationFuncType_StopReadCard,		//停止读卡
	enStationFuncType_GetStationInfo,	//获取站点信息
	enStationFuncType_Tips,				//提示信息
	enStationFuncType_Payback,			//欠费还款（登录时）
	enStationFuncType_DebugInfo,		//调试信息（调试用）
	enStationFuncType_ToolConfig,       //工具配置信息
	enStationFuncType_GetAcountBalance,  //获取账户余额
	enStationFuncType_GetRecentMail,   //最近邮箱
	enStationFuncType_CloseRecharge,  //取消充值
	enStationFuncType_PaybackPay,   //欠费返回付款
	enStationFuncType_RechargePay,   //充值返回付费
	enStationFuncType_PushAlarmInfo,  //推送告警信息
	enStationFuncType_Savestyle,   //设置扫描发送信息
	enStationFuncType_PushSavestyle,   //扫描发送信息结果返回
	enStationFuncType_GetStationQRCode, //获取站点二维码
	enStationFuncType_PushStationQRCode,  //推送站点二维码
	enStationFuncType_QuitStation,     //退出站点
	enStationFuncType_PushQuitStation,
	enStationFuncType_End
};
static MAP_IntString g_UiCallCppFunc = {
	{ enStationFuncType_UserLogin ,			"userLogin" },
	{ enStationFuncType_UserLoginByCard ,	"userLoginByCard"},
	{ enStationFuncType_UserLogout ,		"userLogout" },
	{ enStationFuncType_PrintJobs ,			"printJobs" },
	{ enStationFuncType_GetUserInfo ,		"getUserInfo"},
	{ enStationFuncType_GetPriceInfo ,		"getPriceInfo"},
	{ enStationFuncType_GetPrintJobList ,	"getPrintJobList"},
	{ enStationFuncType_EditPrintJob ,		"editPrintJob"},
	{ enStationFuncType_DeletePrintJobs ,	"deletePrintJobs"},
	{ enStationFuncType_ConfirmPayment ,	"confirmPayment"},
	{ enStationFuncType_StartScanning ,		"startScanning"},
	{ enStationFuncType_StopScanning ,		"stopScanning" },
	{ enStationFuncType_DeleteScanFiles ,	"deleteScanFiles"},
	{ enStationFuncType_Copy ,				"copy"},
	{ enStationFuncType_CopyPay ,			"copyPay"},		
	{ enStationFuncType_Recharge ,			"recharge"},
	{ enStationFuncType_GetUsbFileList ,	"getUsbFileList"},
	{ enStationFuncType_FileConvert ,		"fileConvert"},
	{ enStationFuncType_StopReadCard ,		"stopReadCard"},	
	{ enStationFuncType_GetStationInfo ,	"getStationInfo"},
	{ enStationFuncType_Payback ,			"payback"},	
	{ enStationFuncType_DebugInfo ,			"debugInfo"},
	{ enStationFuncType_ToolConfig,         "toolConfig"},
	{ enStationFuncType_GetAcountBalance , "getAcountBalance"},
	{ enStationFuncType_GetRecentMail,  "getRecentMail"},
	{ enStationFuncType_CloseRecharge , "closerecharge"},
	{ enStationFuncType_Savestyle,  "savestyle"},
	{ enStationFuncType_GetStationQRCode , "getStationQRCode"},
	{ enStationFuncType_QuitStation,      "quitStation"},
	{ enStationFuncType_End ,		"end"}
};
static MAP_IntString g_UiCallJsFunc = {
	{ enStationFuncType_UserLogin ,			"pushLoginResult" },
	{ enStationFuncType_UserLoginByCard ,	"pushUserLoginByCardResult" },
	{ enStationFuncType_UserLogout ,		"pushLogoutResult" },
	{ enStationFuncType_PrintJobs ,			"pushPrintJobsResult" },
	{ enStationFuncType_GetUserInfo ,		"pushUserInfo" },
	{ enStationFuncType_GetPriceInfo ,		"pushPriceInfo" },
	{ enStationFuncType_GetPrintJobList ,	"pushPrintJobList" },
	{ enStationFuncType_EditPrintJob ,		"pushEditPrintJobResult"},
	{ enStationFuncType_DeletePrintJobs ,	"pushDeletePrintJobsResult" },
	{ enStationFuncType_Delivery ,			"pushDeliveryResult"},
	{ enStationFuncType_Pay ,				"pay"},
	{ enStationFuncType_PaymentCompleted ,	"paymentCompleted"},
	{ enStationFuncType_StartScanning ,		"pushStartScanningResult" },
	{ enStationFuncType_StopScanning ,		"pushStopScanningResult" },
	{ enStationFuncType_DeleteScanFiles ,	"pushDeleteScanFilesResult" },
	{ enStationFuncType_ScanFileInfo ,		"pushScanFileInfo" },
	{ enStationFuncType_Copy ,				"pushCopyResult"},
	{ enStationFuncType_CopyPay ,			"pushCopyPayResult"},
	{ enStationFuncType_CopyPayInfo ,		"pushCopyPayInfo"},
	{ enStationFuncType_Recharge ,			"pushRechargeResult"},
	{ enStationFuncType_CopyInfo ,			"pushCopyInfo"},
	{ enStationFuncType_GetUsbFileList ,	"pushUsbFileList" },
	{ enStationFuncType_GetStationInfo,		"pushStationInfo" },
	{ enStationFuncType_DebugInfo ,			"pushDebugInfo" },
	{ enStationFuncType_FileConvert,		"pushFileCovertStatus"},
	{ enStationFuncType_Tips ,				"pushTips"},
	{ enStationFuncType_Payback ,			"pushPaybackResult"},
	{ enStationFuncType_ToolConfig,         "pushToolConfig"},
	{ enStationFuncType_GetAcountBalance ,  "pushAcountBalance"},
	{ enStationFuncType_GetRecentMail,      "pushRecentMail" },
	{ enStationFuncType_CloseRecharge ,     "pushCloseRecharge" },
	{ enStationFuncType_PaybackPay,         "pushPaybackPay" },
	{ enStationFuncType_RechargePay ,       "pushRechargePay" },
	{ enStationFuncType_PushAlarmInfo,      "pushAlarmInfo"},
	{ enStationFuncType_PushSavestyle,      "pushsavestyle"},
	{ enStationFuncType_PushStationQRCode,     "pushStationQRCode"},
	{ enStationFuncType_PushQuitStation,      "pushQuitStation" },
	{ enStationFuncType_End ,				"pushEnd"}
};
typedef BOOL(fn_UiCallBack)(int nFuncType, const char * szParam);

//异常处理的宏
#define Cloud_Http_Interface_Try_Catch_Exception_Log	\
	theLog.Write(_T("!!http interface catch exception：file：[%s]，function：[%s]，line：[%d]")\
	, __FILE__, __FUNCTION__, __LINE__);\
	return FALSE;

#define Cloud_Http_Interface_Try_Catch_Exception_Begin	try {
#define Cloud_Http_Interface_Try_Catch_Exception_End	}catch (...){ Cloud_Http_Interface_Try_Catch_Exception_Log }
