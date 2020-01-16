// DBBackFun.h: interface for the CDBBackFun class.
#if !defined(AFX_DBBACKFUN_H__A2444F30_62B6_443E_94EE_7956639BD322__INCLUDED_)
#define AFX_DBBACKFUN_H__A2444F30_62B6_443E_94EE_7956639BD322__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum
{
	BACKTYPE_BYDAY = 0,
	BACKTYPE_BYMONTH = 1,
	BACKTYPE_BYWEEK = 2
}BACKTYPE;

class CDBBackFun  
{
public:
	CDBBackFun();
	virtual ~CDBBackFun();
	
public:
	// �õ���ǰ�ļ���·��
	void static GetLocalFilePath(CString& sLocalFilePath);

	/* 
	��ע����еõ�/�������ݿ��������Ϣ
	sServerName:���ݿ����������
	sBackDbName:�������ݿ�����
	sPort:���ݿ���ʶ˿�
	sAccount:��¼���ݿ���û���
	sAccountPsw:��¼���ݿ������
	*/
	void static GetDatebaseConfig(CString& sServerName,CString& sBackDbName,CString& sPort,CString& sInstanceName,CString& sAccount,CString& sAccountPsw);
	void static SetDatebaseConfig(CString sServerName,CString sBackDbName,CString sAccount,CString sAccountPsw);

	/*
	��ע����еõ�/�������ݿⱸ�ݵ���Ϣ
	sBackupDir: �������ݿⱣ��Ŀ¼
	sDesc:�������ݿ������
	bAutoBack:�Ƿ��Զ�����
	type:����ʱ������
	nInterVal:����ʱ���
	dTime:����ʱ��
	*/
	void static GetDBBackConfig(CString& sBackupDir,BOOL& bAutoBack,BACKTYPE& type,int& nInterVal,double& dTime);
	void static SetDBBackConfig(CString sBackupDir,BOOL bAutoBack,BACKTYPE type,int nInterVal,double dTime);

	/*
	��ע����еõ�/�������ݿ⻹ԭ��Ϣ
	sRestoreDir:��ԭ���ݿ�Ŀ¼
	*/
	void static GetDBRestoreConfig(CString& sRestoreDir,CString& sInitDir);
	void static SetDBRestoreConfig(CString sRestoreDir,CString sInitDir);
};
#endif // !defined(AFX_DBBACKFUN_H__A2444F30_62B6_443E_94EE_7956639BD322__INCLUDED_)
