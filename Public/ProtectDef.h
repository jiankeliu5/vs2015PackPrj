#pragma once
//#pragma message(__LOC__"��ʱ�ļ����Ƶ�gl��")

//#define	ONE_INSTANCE			1	//ÿ̨����ֻ������һ��
#define PROTECT_ELAPSE			1000*10//���������ͨ�ż��������
#define DESKTOPSWTICH_ELAPSE	3000//�������Ƿ��winlogonת��default
#define MARSUSERLOGIN_ELAPSE	10000//�Զ�ʹ��ר���˻���¼

#define MAX_LOST				10//��������Ӧ������
#define SVC_CHECK_ELAPSE		3000//���̼��������еļ��

//#define MAX_SESSION_PROTECT		1000//��ౣ����session��,sessionֵ����С�ڴ�ֵ


#define MARS_CLT_PROTECTER_START 10000//MARS_STU_IPC��DestID��MARS_CLT_PROTECTER_START���϶����������̱���
//#define MARS_CLT_PROTECTER_END	 (MARS_CLT_PROTECTER_START + MAX_SESSION_PROTECT)

#define IPC_STU_PROTECT_MAIN	1
#define IPC_STU_PROTECT_NET		2
#define IPC_STU_EXIT_WINDOWS	3//�˳�windows������Ҫ��������
#define IPC_STU_LOGON_CONFIG	4//��������
#define IPC_STU_LOGON_CMD		5//�˵�����
#define IPC_STU_LOCK			6
#define IPC_STU_PROTECT_ENABLE  7//ѧ����֪ͨ�����Ƿ񱣻�ѧ��������
#define IPC_STU_HELP_EXIT_WIN	8//ѧ���������æ�ػ�
#define IPC_STU_HELP_VIDEO_PLAY	9//ѧ����Ҫ����ӰԺ��
#define IPC_SVC_KILLYOURSELF	10//����֪ͨ�����������ɱ���Լ���ѧ������ɱ

#define MAP_NAME_USER_PWD		_T("Global\\MaREDFGSDMMUserPWD")
#define MAP_NAME_NO_PROTECT		_T("Global\\mamASXZAFANoProtect")
#define WIN_USER_NAME_LEN		128//��¼��windows���û�������
#define WIN_USER_PWD_LEN		128//��¼��windows�����볤��
#define WIN_USER_DOM_LEN		128//��¼��windows����������

#define	SERVICE_CONTROL_MY_STOP	 0x000000EC
#define SERVICE_CTL_START_DEVICE 0x000000EE
#define SERVICE_CTL_STOP_DEVICE	 0x000000EF
//#define ASMARS_USER_NAME		"AsiaStarMars"
#define SYSTEMUSER_EXE_NAME		_T("winlogon.exe")
#define NORMALUSER_EXE_NAME		_T("explorer.exe")