#pragma once


#define DB_SEVERIP_SIZE		50
#define DB_NAME_SIZE		50
#define DB_USER_SIZE		50
#define DB_PWD_SIZE			50

/* ADO信息 */
typedef struct tag_ST_ADO_INFO
{
	char cIP[DB_SEVERIP_SIZE];
	char cPort[10];
	char cDbName[DB_NAME_SIZE];
	char cUser[DB_USER_SIZE];
	char cPwd[DB_PWD_SIZE];
} ADO_INFO, *PADO_INFO;
