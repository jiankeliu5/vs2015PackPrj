#ifndef _KYOCERA_MIB_DEF_AA638EB3_5BEE_4D0E_B435_15BA8FF4DF31_H_
#define _KYOCERA_MIB_DEF_AA638EB3_5BEE_4D0E_B435_15BA8FF4DF31_H_

#include "Snmp_Def.h"

//���ɳ�������ڵ�
#define KyoceraA4PrintTotalPageOID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVRNA31FT0M=")		//".1.3.6.1.4.1.1347.42.3.1.1.1.1.1"
#define KyoceraA4PrintHBPageOID					TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0MdAA==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.1.1"
#define KyoceraA4PrintSingleColorPageOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0MdAw==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.1.2"
#define KyoceraA4PrintFullColorPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0MdAg==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.1.3"
#define KyoceraA4CopyTotalPageOID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVRNA31FT0A=")		//".1.3.6.1.4.1.1347.42.3.1.1.1.1.2"
#define KyoceraA4CopyHBPageOID					TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0AdAA==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.2.1"
#define KyoceraA4CopySingleColorPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0AdAw==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.2.2"
#define KyoceraA4CopyFullColorPageOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVdNA31FT0AdAg==")	//".1.3.6.1.4.1.1347.42.3.1.2.1.1.2.3"
#define KyoceraScanTotalPageOID					TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAB9YfVFNA31FT0M=")		//".1.3.6.1.4.1.1347.42.3.1.4.1.1.1"
#define KyoceraScanTotalPage2OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFdcAgFHYktSHGZaUg==")		//".1.3.6.1.4.1.1347.46.10.1.1.5.3"
#define KyoceraTotalPageInfo6OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAR9YfVRNA31CT0M=")		//".1.3.6.1.4.1.1347.42.2.1.1.1.6.1"
#define KyoceraTotalPageInfo7OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAR9YfVRNA31DT0M=")		//".1.3.6.1.4.1.1347.42.2.1.1.1.7.1"
#define KyoceraTotalPageInfo8OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAR9YfVRNA31MT0M=")		//".1.3.6.1.4.1.1347.42.2.1.1.1.8.1"
#define KyoceraTotalPageInfo9OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFNcAR9YfVRNA31NT0M=")		//".1.3.6.1.4.1.1347.42.2.1.1.1.9.1"

//���ɵ�ǰ��ҵ��Ϣ�ڵ�
#define KyoceraActiveJobInfoJobCountOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAh9YfVRNAH1F")	//".1.3.6.1.4.1.1347.47.1.1.1.2.1"		//��ǰ�����ӡ��ҵ����

#define KyoceraActiveJobInfoJobIDOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNAA==")	//".1.3.6.1.4.1.1347.47.2.1.1.2"		//��ҵID
#define KyoceraActiveJobInfo3OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNAQ==")	//".1.3.6.1.4.1.1347.47.2.1.1.3"		//���������֣�
#define KyoceraActiveJobInfoJobTypeOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNBg==")	//".1.3.6.1.4.1.1347.47.2.1.1.4"		//��ҵ���ͣ�1-��ӡ��2-��ӡ
#define KyoceraActiveJobInfoJobNameOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNBw==")	//".1.3.6.1.4.1.1347.47.2.1.1.5"		//��ҵ����(���ֽڴ洢)
#define KyoceraActiveJobInfoJobStateOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNBA==")	//".1.3.6.1.4.1.1347.47.2.1.1.6"		//��ҵ״̬��1-�����У�2-��ӡ�У�4-��ͣ��5-�ȴ���
#define KyoceraActiveJobInfoJobBeginTimeOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNBQ==")	//".1.3.6.1.4.1.1347.47.2.1.1.7"		//��ҵ��ʼʱ�䣬1970-01-01�뵱ǰʱ�������֮��
#define KyoceraActiveJobInfo8OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNCg==")	//".1.3.6.1.4.1.1347.47.2.1.1.8"		//���ַ�����
#define KyoceraActiveJobInfoPrintedCountOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNCw==")	//".1.3.6.1.4.1.1347.47.2.1.1.9"		//�Ѿ���ֽҳ��
#define KyoceraActiveJobInfoPrintedCopyOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2M=")	//".1.3.6.1.4.1.1347.47.2.1.1.10"		//�Ѿ���ֽ����
#define KyoceraActiveJobInfoTotalCopyOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2I=")	//".1.3.6.1.4.1.1347.47.2.1.1.11"		//�ܷ���
#define KyoceraActiveJobInfoUserNameOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2E=")	//".1.3.6.1.4.1.1347.47.2.1.1.12"		//�û���
#define KyoceraActiveJobInfo13OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2A=")	//".1.3.6.1.4.1.1347.47.2.1.1.13"		//(�����ڴ˽ڵ�)
#define KyoceraActiveJobInfoFromOID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2c=")	//".1.3.6.1.4.1.1347.47.2.1.1.14"		//������
#define KyoceraActiveJobInfoOrgDocPageCountOID	TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2Y=")	//".1.3.6.1.4.1.1347.47.2.1.1.15"		//ԭ��ҳ��
#define KyoceraActiveJobInfo16OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2U=")	//".1.3.6.1.4.1.1347.47.2.1.1.16"		//���������֣�
#define KyoceraActiveJobInfoColorTypeOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAR9YfVRNA2Q=")	//".1.3.6.1.4.1.1347.47.2.1.1.17"		//���ɫ�ʣ�0-�ڰף�1-ȫɫ��6-�Զ�ɫ�ʣ���ɫ���Ҷȣ���7-�Զ�ɫ�ʣ�8-���

//������ʷ��ҵ��Ϣ�ڵ�
#define KyoceraHistoryJobInfoJobCountOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcAh9YfVRNBH1F")	//".1.3.6.1.4.1.1347.47.1.1.1.6.1"		//��ʷ��ӡ��ҵ����־����

#define KyoceraHistoryJobInfoJobIDOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNAA==")	//".1.3.6.1.4.1.1347.47.6.1.1.2"		//��ҵID
#define KyoceraHistoryJobInfoJobTypeOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNAQ==")	//".1.3.6.1.4.1.1347.47.6.1.1.3"		//��ҵ���ͣ�1-��ӡ��2-��ӡ
#define KyoceraHistoryJobInfoJobNameOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNBg==")	//".1.3.6.1.4.1.1347.47.6.1.1.4"		//��ҵ����(���ֽڴ洢)
#define KyoceraHistoryJobInfoJobStateOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNBw==")	//".1.3.6.1.4.1.1347.47.6.1.1.5"		//��ҵ���״̬��0-��ɣ�1-ȡ����2-����
#define KyoceraHistoryJobInfoBeginTimeOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNBA==")	//".1.3.6.1.4.1.1347.47.6.1.1.6"		//��ҵ��ʼʱ�䣬1970-01-01�뵱ǰʱ�������֮��
#define KyoceraHistoryJobInfoEndTimeOID			TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNBQ==")	//".1.3.6.1.4.1.1347.47.6.1.1.7"		//��ҵ����ʱ�䣬1970-01-01�뵱ǰʱ�������֮��
#define KyoceraHistoryJobInfo8OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNCg==")	//".1.3.6.1.4.1.1347.47.6.1.1.8"		//���ַ�����
#define KyoceraHistoryJobInfoPrintedCountOID	TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNCw==")	//".1.3.6.1.4.1.1347.47.6.1.1.9"		//�Ѿ���ֽҳ��
#define KyoceraHistoryJobInfoPrintedCopyOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2M=")	//".1.3.6.1.4.1.1347.47.6.1.1.10"		//�Ѿ���ֽ����
#define KyoceraHistoryJobInfoTotalCopyOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2I=")	//".1.3.6.1.4.1.1347.47.6.1.1.11"		//�ܷ���
#define KyoceraHistoryJobInfoUserNameOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2E=")	//".1.3.6.1.4.1.1347.47.6.1.1.12"		//�û���
#define KyoceraHistoryJobInfo13OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2A=")	//".1.3.6.1.4.1.1347.47.6.1.1.13"		//���ַ�����
#define KyoceraHistoryJobInfo14OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2c=")	//".1.3.6.1.4.1.1347.47.6.1.1.14"		//���ַ�����
#define KyoceraHistoryJobInfoOrgDocPageCountOID	TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2Y=")	//".1.3.6.1.4.1.1347.47.6.1.1.15"		//ԭ��ҳ��
#define KyoceraHistoryJobInfo16OID				TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2U=")	//".1.3.6.1.4.1.1347.47.6.1.1.16"		//���������֣�
#define KyoceraHistoryJobInfoColorTypeOID		TEXT("H1h9Vk0EfUVPRh0AR2JWVwV9QFZcBR9YfVRNA2Q=")	//".1.3.6.1.4.1.1347.47.6.1.1.17"		//���ɫ�ʣ�0-�ڰף�6-�Զ�ɫ�ʣ���ɫ���Ҷȣ���1-ȫɫ��7-�Զ�ɫ�ʣ�8-���

#endif	//_KYOCERA_MIB_DEF_AA638EB3_5BEE_4D0E_B435_15BA8FF4DF31_H_
