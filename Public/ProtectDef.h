#pragma once
//#pragma message(__LOC__"临时文件，移到gl中")

//#define	ONE_INSTANCE			1	//每台机器只能运行一个
#define PROTECT_ELAPSE			1000*10//服务与进程通信间隔，毫秒
#define DESKTOPSWTICH_ELAPSE	3000//检查进程是否从winlogon转入default
#define MARSUSERLOGIN_ELAPSE	10000//自动使用专有账户登录

#define MAX_LOST				10//进程无响应最大次数
#define SVC_CHECK_ELAPSE		3000//进程检查服务运行的间隔

//#define MAX_SESSION_PROTECT		1000//最多保护的session数,session值必须小于此值


#define MARS_CLT_PROTECTER_START 10000//MARS_STU_IPC的DestID从MARS_CLT_PROTECTER_START向上都用来做进程保护
//#define MARS_CLT_PROTECTER_END	 (MARS_CLT_PROTECTER_START + MAX_SESSION_PROTECT)

#define IPC_STU_PROTECT_MAIN	1
#define IPC_STU_PROTECT_NET		2
#define IPC_STU_EXIT_WINDOWS	3//退出windows，不需要重启进程
#define IPC_STU_LOGON_CONFIG	4//参数配置
#define IPC_STU_LOGON_CMD		5//菜单命令
#define IPC_STU_LOCK			6
#define IPC_STU_PROTECT_ENABLE  7//学生机通知服务是否保护学生机程序
#define IPC_STU_HELP_EXIT_WIN	8//学生机请求帮忙关机
#define IPC_STU_HELP_VIDEO_PLAY	9//学生机要网际影院了
#define IPC_SVC_KILLYOURSELF	10//服务通知其他服务进程杀死自己的学生并自杀

#define MAP_NAME_USER_PWD		_T("Global\\MaREDFGSDMMUserPWD")
#define MAP_NAME_NO_PROTECT		_T("Global\\mamASXZAFANoProtect")
#define WIN_USER_NAME_LEN		128//登录到windows的用户名长度
#define WIN_USER_PWD_LEN		128//登录到windows的密码长度
#define WIN_USER_DOM_LEN		128//登录到windows的域名长度

#define	SERVICE_CONTROL_MY_STOP	 0x000000EC
#define SERVICE_CTL_START_DEVICE 0x000000EE
#define SERVICE_CTL_STOP_DEVICE	 0x000000EF
//#define ASMARS_USER_NAME		"AsiaStarMars"
#define SYSTEMUSER_EXE_NAME		_T("winlogon.exe")
#define NORMALUSER_EXE_NAME		_T("explorer.exe")