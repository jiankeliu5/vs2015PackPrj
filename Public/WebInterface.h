#pragma once
#include <map>

#define MAX_MSG_LEN 128				//��ui������Ϣ����󳤶�

#define  DEF_INTERFACE_WITH_PUBLIC 0

typedef enum tag_EN_MQCmd {
	enMQCmd_Begin = 10000,	//
	enMQCmd_UsbFileConvert = 10001,		//USB�ļ�ת��
	enMQCmd_UserLoginByQRCode = 10002,	//�û���¼
	enMQCmd_StationInfoChange = 10003,	//webվ����Ϣ�������
	enMQCmd_StationPriceChange = 10004,	//webվ����ʷ������
	enMQCmd_StationPayFinish = 10005	//webվ��֧���ɹ�
}EN_MQCmd;

typedef enum tag_EN_HttpCmd {
	enHttpCmd_GetToken	= 1,	//��ȡtoken
	enHttpCmd_UserLogin,		//�û���¼
	enHttpCmd_StationUserLogin,	//վ���û���¼
	enHttpCmd_UserLogout,		//�ǳ�
	enHttpCmd_GetUserInfo,		//��ȡ�û���Ϣ
	enHttpCmd_GetPriceInfo,		//��ȡ����
	enHttpCmd_GetPrintJobList,	//��ȡδ��ӡ��ҵ�б�
	enHttpCmd_EditPrintJob,		//�޸�δ��ӡ��ҵ
	enHttpCmd_DeletePrintJobs,	//ɾ��δ��ӡ��¼
	enHttpCmd_CreateOrder,		//��������
	enHttpCmd_PaymentNotice,	//֧��֪ͨ
	enHttpCmd_PrintFinish,		//��ӡ���
	enHttpCmd_TaskCheck,		//У���˷�
	enHttpCmd_AddScanJob,		//���ɨ���¼
	enHttpCmd_ScanFinish,		//ɨ�����	
	enHttpCmd_GetFtpInfo,		//��ȡftp��Ϣ
	enHttpCmd_GetEntInfo,		//��ȡ��ҵ��Ϣ
	enHttpCmd_Register,			//ע�Ṥ��
	enHttpCmd_GetNeedConvert,	//��ȡ��ת�����ļ��б�
	enHttpCmd_UpdateConvert,	//����ת����ҵ��Ϣ
	enHttpCmd_Recharge,			//��ֵ
	enHttpCmd_AddCopy,			//��Ӹ�ӡ��¼����ɶ���
	enHttpCmd_StationRegister,	//վ��ע��
	enHttpCmd_GetStationInfo,	//��ȡվ����Ϣ
	enHttpCmd_GetStationList,	//��ȡվ���б�
	enHttpCmd_ReadMeter,		//����
	enHttpCmd_AddUsbFile,		//���USB��ӡ�ļ�
	enHttpCmd_StationAlarm,		//վ��Ԥ��(��¼)
	enHttpCmd_StationAlarmStatus,//վ��Ԥ��״̬
	enHttpCmd_GetUserQRCode,	//��ȡ�û���¼�Ķ�ά�루�ַ�����
	enHttpCmd_UserLoginConfirm,	//ɨ���¼ȷ��
	enHtppCmd_AddSubmitJob,		//�����ҵ
	enHttpCmd_JobCheck,			//job��У�飨�˷ѣ�
	enHttpCmd_Upgrade,			//��������
	enHttpCmd_ToolConfig,      //��ȡ������Ϣ
	enHttpCmd_ScanEmail,        //�ʼ�����
	enHttpCmd_GetRecentEmail,  //��ȡ�������
	enHttpCmd_GetScanJob,	   //��ȡɨ����ҵ
	enHttpCmd_UpdateScanJob,   //����ɨ����ҵ
	enHttpCmd_UserIpass,   //һ��ͨ���Ѽ�¼
	enHttpCmd_StationEdit, //�޸Ĵ�ӡ����Ϣ
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
//ui�����dll�Ľӿ�
enum EN_StationFuncType {
	enStationFuncType_UserLogin = 1,	//��¼
	enStationFuncType_UserLoginByCard,	//ˢ����¼
	enStationFuncType_UserLogout,		//�ǳ�
	enStationFuncType_PrintJobs,		//��ӡ
	enStationFuncType_GetUserInfo,		//��ȡ�û���Ϣ
	enStationFuncType_GetPriceInfo,		//��ȡ������Ϣ
	enStationFuncType_GetPrintJobList,	//��ȡ����ӡ�б�	
	enStationFuncType_EditPrintJob,		//�޸Ĵ���ӡ��ҵ
	enStationFuncType_DeletePrintJobs,	//ɾ����ӡ��ҵ	
	enStationFuncType_Pay,				//֧����c++���𣬵���js��ʾ����֧����
	enStationFuncType_ConfirmPayment,	//ȷ��֧����js����֪ͨc++ȷ�Ͻ���֧���������˻�����
	enStationFuncType_PaymentCompleted,	//֧����ɣ�c++����֪ͨjs֧����ɣ�
	enStationFuncType_Delivery,			//Ͷ�ݵ���ӡ��
	enStationFuncType_StartScanning,	//��ʼɨ��
	enStationFuncType_StopScanning,		//ֹͣɨ�裨֧�������ͣ�
	enStationFuncType_DeleteScanFiles,	//ɾ��ɨ���ļ�
	enStationFuncType_ScanFileInfo,		//ɨ���ļ���Ϣ
	enStationFuncType_Copy,				//��ʼ��ӡ
	enStationFuncType_CopyPay,			//��ӡ����
	enStationFuncType_CopyPayInfo,		//��ӡ������Ϣ
	enStationFuncType_Recharge,			//��ֵ
	enStationFuncType_CopyInfo,			//��ӡ��Ϣ����ҳ��
	enStationFuncType_GetUsbFileList,	//��ȡUSB�ļ��б�
	enStationFuncType_FileConvert,		//�ļ�ת��
	enStationFuncType_StopReadCard,		//ֹͣ����
	enStationFuncType_GetStationInfo,	//��ȡվ����Ϣ
	enStationFuncType_Tips,				//��ʾ��Ϣ
	enStationFuncType_Payback,			//Ƿ�ѻ����¼ʱ��
	enStationFuncType_DebugInfo,		//������Ϣ�������ã�
	enStationFuncType_ToolConfig,       //����������Ϣ
	enStationFuncType_GetAcountBalance,  //��ȡ�˻����
	enStationFuncType_GetRecentMail,   //�������
	enStationFuncType_CloseRecharge,  //ȡ����ֵ
	enStationFuncType_PaybackPay,   //Ƿ�ѷ��ظ���
	enStationFuncType_RechargePay,   //��ֵ���ظ���
	enStationFuncType_PushAlarmInfo,  //���͸澯��Ϣ
	enStationFuncType_Savestyle,   //����ɨ�跢����Ϣ
	enStationFuncType_PushSavestyle,   //ɨ�跢����Ϣ�������
	enStationFuncType_GetStationQRCode, //��ȡվ���ά��
	enStationFuncType_PushStationQRCode,  //����վ���ά��
	enStationFuncType_QuitStation,     //�˳�վ��
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

//�쳣����ĺ�
#define Cloud_Http_Interface_Try_Catch_Exception_Log	\
	theLog.Write(_T("!!http interface catch exception��file��[%s]��function��[%s]��line��[%d]")\
	, __FILE__, __FUNCTION__, __LINE__);\
	return FALSE;

#define Cloud_Http_Interface_Try_Catch_Exception_Begin	try {
#define Cloud_Http_Interface_Try_Catch_Exception_End	}catch (...){ Cloud_Http_Interface_Try_Catch_Exception_Log }
