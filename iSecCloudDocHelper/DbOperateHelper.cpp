#include "StdAfx.h"
#include "DbOperateHelper.h"

CDbOperateHelper::CDbOperateHelper(void)
{
	m_pAdo = CAdoFactory::CreateAdoEx();
	CString szDbName(CAdoFactory::m_oAdoInfo.szDbName);
	if (szDbName.CompareNoCase(Db_Name_Normal) == 0)
	{
		m_CloudType = CPT_Normal;
	}
	else
	{
		m_CloudType = CPT_Cloud;
	}
}

CDbOperateHelper::~CDbOperateHelper(void)
{
}

BOOL CDbOperateHelper::InitDataBase()
{
	theLog.Write(_T("CDbOperateHelper::InitDataBase"));

	//打印数据库配置
	int nPrintDbType = theIniFile.GetVal(_T("DbConfig"),_T("DbType"), SQLSERVER);
	CString szPrintDbIP = theIniFile.GetVal(_T("DbConfig"),_T("DbSvr"), _T("localhost"));
	CString szPrintDbPort = theIniFile.GetVal(_T("DbConfig"),_T("DbPort"), _T("1433"));
	CString szPrintDbName = theIniFile.GetVal(_T("DbConfig"),_T("DbName"), _T("iSec_Print56"));
	CString szInstanceName = theIniFile.GetVal(_T("DbConfig"),_T("DbInstanceName"),_T(""));
	CString szPrintDbUser = theIniFile.GetVal(_T("DbConfig"),_T("DbUser"), _T("sa"));
	CString szPrintDbPwd = theIniFile.GetVal(_T("DbConfig"),_T("DbPwd"), _T(""));
#ifdef ENABLE_PWD_CRY
	char szPwd[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	CPWDCry cry;
#ifdef UNICODE
	CStringA szPrintDbPwdA = CCommonFun::UnicodeToMultiByte(szPrintDbPwd);
	cry.UnCryptPWD(szPrintDbPwdA.GetBuffer(), szPwd, nSize);
	szPrintDbPwdA.ReleaseBuffer();
	szPrintDbPwd = CCommonFun::MultiByteToUnicode(szPwd);
#else
	cry.UnCryptPWD(szPrintDbPwd.GetBuffer(),szPwd,nSize);
	szPrintDbPwd.ReleaseBuffer();
	szPrintDbPwd = szPwd;
#endif // UNICODE
#endif

	ADO_INFO ado;
	_tcscpy_s(ado.szIP, szPrintDbIP);
	_tcscpy_s(ado.szPort, szPrintDbPort);
	_tcscpy_s(ado.szDbName, szPrintDbName);
	_tcscpy_s(ado.szInstanceName, szInstanceName);
	_tcscpy_s(ado.szUser, szPrintDbUser);
	_tcscpy_s(ado.szPwd, szPrintDbPwd);

	CAdoFactory::SetDBType(nPrintDbType);
	CAdo* pAdo = CAdoFactory::CreateAdoEx(&ado);
	if (!pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::InitDataBase, CreateAdo fail"));
		return FALSE;
	}
	else
	{
		CAdoFactory::Release(pAdo);
		return TRUE;
	}
}

void CDbOperateHelper::ReleaseDataBase()
{
	theLog.Write(_T("CDbOperateHelper::ReleaseDataBase"));
	CAdoFactory::Release();
}

BOOL CDbOperateHelper::IsNeedDealCloudJob()
{
	return TRUE;
}

BOOL CDbOperateHelper::GetNeedDealCloudJob(CListCloudJobInfo& list)
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::GetNeedDealCloudJob,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}

	if (m_CloudType == CPT_Normal)
	{
		CStringArray ary;
		long lRow = 0;
		long lCol = 0;
		CString szSql;
		szSql.Format(_T(" select "
			" id, "				//0
			" UserId, "			//1
			" UserName, "		//2
			" DocName, "		//3
			" JobSize, "		//4
			" ZipJobSize, "		//5
			" PaperSize, "		//6
			" PageType, "		//7
			" Orientation, "	//8
			" Copies, "			//9
			" Color, "			//10
			" TotalPage, "		//11
			" DoublePrint, "	//12
			" JobPath, "		//13
			" PaperSource, "	//14
			" SubmitTime, "		//15
			" OriginalPath,"	//16
			" IP, "				//17
			" State "			//18
			" from t_log_SubmitJob where JobType=%d and State=%d order by id "),Print_Job_Type_Cloud, JOB_CLOUD_DEAL_WAIT);
		if (m_pAdo->GetRecordset(szSql, ary, lRow, lCol) && lRow>0)
		{
			for (long l=0; l<lRow; l++)
			{
				long lIndex = l * lCol;
				PCloudJobInfo job = new CloudJobInfo;
				memset(job, 0x0, sizeof(CloudJobInfo));
				job->nJobId = _tstoi(ary.GetAt(lIndex + 0));
				job->nUserId = _tstoi(ary.GetAt(lIndex + 1));
				_tcscpy_s(job->cAccount, ary.GetAt(lIndex + 2));
				_tcscpy_s(job->cDocName, ary.GetAt(lIndex + 3));
				job->nDocSize = _tstoi(ary.GetAt(lIndex + 4));
				job->nOrgJobSize = _tstoi(ary.GetAt(lIndex + 5));
				job->nPaperSize = _tstoi(ary.GetAt(lIndex + 6));
				_tcscpy_s(job->cPageType, ary.GetAt(lIndex + 7));
				job->nOrientation = _tstoi(ary.GetAt(lIndex + 8));
				job->nCopies = _tstoi(ary.GetAt(lIndex + 9));
				job->nColor = _tstoi(ary.GetAt(lIndex + 10));
				job->nTotalPage = _tstoi(ary.GetAt(lIndex + 11));
				job->nDoublePrint = _tstoi(ary.GetAt(lIndex + 12));
				job->nPaperSource = _tstoi(ary.GetAt(lIndex + 14));
				COleDateTime time;
				time.ParseDateTime(ary.GetAt(lIndex + 15));
				job->fSubmitTime = time.m_dt;
				CString szOriginalPath = ary.GetAt(lIndex + 16);
				_tcscpy_s(job->cJobDir, CCommonFun::GetFileDirByFullPath(szOriginalPath));
				_tcscpy_s(job->cOrgJobName, CCommonFun::GetFileNameByPathName(szOriginalPath));
				_tcscpy_s(job->cIP, ary.GetAt(lIndex + 17));
				job->nJobState = _tstoi(ary.GetAt(lIndex + 18));
				job->nOcrState = 0;
				list.AddTail(job);
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return (list.GetCount()>0 ? TRUE : FALSE);
}

BOOL CDbOperateHelper::UpdateCloudJob(PCloudJobInfo job)
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo || !job)
	{
		theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJob,m_pAdo=%p,job=%p"), m_pAdo, job);
		return FALSE;
	}

	CString szSql;
	if (m_CloudType == CPT_Normal)
	{
		BOOL bFileHaveDelete = FALSE;
		if (job->nJobState == JOB_CLOUD_DEAL_FAIL)
		{
			bFileHaveDelete = TRUE;
		}
		szSql.Format(
			_T(" update t_log_SubmitJob set "
			" UserId=%d, "			//1
			" UserName=N'%s', "		//2
			" DocName=N'%s', "		//3
			" JobSize=%d, "			//4
			" ZipJobSize=%d, "		//5
			" PaperSize=%d, "		//6
			" PageType=N'%s', "		//7
			" Orientation=%d, "		//8
			" Copies=%d, "			//9
			" Color=%d, "			//10
			" TotalPage=%d, "		//11
			" DoublePrint=%d, "		//12
			" JobPath=N'%s', "		//13
			" FileStorageId=%d, "	//14
			" PaperSource=%d, "		//15
			" SubmitTime=N'%s', "	//16
			" State=%d, "			//17
			" FileHaveDelete=%d, "  //18
			" OcrStatus=%d, "		//19
			" OriginalPath=NULL, "
			" AuditTacticsId=%d "	//20
			" where id=%d ")
			, job->nUserId
			, job->cAccount
			, job->cDocName
 			, job->nDocSize
 			, job->nNewJobSize
			, job->nPaperSize
			, job->cPageType
			, job->nOrientation
			, job->nCopies
			, job->nColor
			, job->nTotalPage
			, job->nDoublePrint
			, job->cNewJobName
			, job->nFileStorageId
			, job->nPaperSource
			, GetTimeStr(job->fSubmitTime)
			, job->nJobState
			, bFileHaveDelete
			, job->nOcrState
			, job->nAuditTacticsId
			, job->nJobId);
		if (!m_pAdo->ExecuteSQL(szSql))
		{
			theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJob,1,UpdateCloudJobState fail, nJobId=%d, nJobState=%d"), job->nJobId, job->nJobState);
			return FALSE;
		}	

		return TRUE;
	}
	else
	{
		return FALSE;
	}
	BOOL bRet = m_pAdo->ExecuteSQL(szSql);
	if (!bRet)
	{
		theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJob,4,szSql=%s"), szSql);
	}
	return bRet;
}

BOOL CDbOperateHelper::UpdateCloudJobState(PCloudJobInfo job, int nState)
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo || !job)
	{
		theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJobState,m_pAdo=%p,job=%p"), m_pAdo, job);
		return FALSE;
	}

	CString szSql;
	if (m_CloudType == CPT_Normal)
	{
		szSql.Format(_T(" update t_log_SubmitJob set state=%d where id=%d "), nState, job->nJobId);
	}
	else
	{
		return FALSE;
	}

	BOOL bRet = m_pAdo->ExecuteSQL(szSql);
	if (!bRet)
	{
		theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJobState,szSql=%s"), szSql);
	}
	return bRet;
}

BOOL CDbOperateHelper::GetSystemSetting(CString& sName,CStringArray& ary)
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::UpdateCloudJob,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}
	CString szSql;
	if (m_CloudType == CPT_Normal)
	{
		szSql.Format(_T(" select setname, setvalue1, setvalue2, setvalue3, setvalue4, setvalue5, "
			" setvalue6, setvalue7, setvalue8, setvalue9, setvalue10, IsShow, Description, IsUse "
			" from t_s_systemset where setname = N'%s' "), sName);
	}
	else
	{
		return FALSE;
	}
	return m_pAdo->GetAry(szSql,ary);
}

BOOL CDbOperateHelper::GetFileStorageList(FileStorageList& oList)
{
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::GetFileStorageList,1,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}

	BOOL bRet = FALSE;
	CString szSql;
	szSql.Format(_T("select id, "	//0
		"EquipId, "				//1
		"StorageType, "			//2
		"IP, "					//3
		"Port, "				//4
		"UserName, "			//5
		"Password, "			//6
		"VirtualDirectory, "	//7
		"AbsoluteDirectory, "	//8
		"ClientIpRange "		//9
		"from t_s_FileStorage"));

	long lRow = 0;
	long lCol = 0;
	CStringArray ary;
	if (m_pAdo->GetRecordset(szSql, ary, lRow, lCol))
	{
		for (int index=0; index<lRow; index++)
		{
			long lIndex = lCol * index;
			FileStorage* pInfo = oList.add_list();
			pInfo->set_id(_tstoi(ary.GetAt(lIndex + 0)));
			pInfo->set_equip_id(_tstoi(ary.GetAt(lIndex + 1)));
			pInfo->set_storage_type((ProtocolType)_tstoi(ary.GetAt(lIndex + 2)));
			pInfo->set_ip(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 3)));
			pInfo->set_port(_tstoi(ary.GetAt(lIndex + 4)));
			pInfo->set_username(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 5)));
			pInfo->set_password(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 6)));
			pInfo->set_virtual_directory(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 7)));
			pInfo->set_absolute_directory(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 8)));
			pInfo->set_client_ip_range(CCommonFun::UnicodeToUTF8(ary.GetAt(lIndex + 9)));
		}

		bRet = TRUE;
	}

	return bRet;
}

CString CDbOperateHelper::GetTimeStr(double fTime)
{
	COleDateTime time;
	time.m_dt = fTime;
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

BOOL CDbOperateHelper::CheckDbConn()
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::CheckDbConn,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}
	CString szSql;
	if (m_CloudType == CPT_Normal)
	{
		szSql.Format(_T(" select getdate() "));
	}
	else
	{
		return FALSE;
	}

	BOOL bRet = m_pAdo->ExecuteSQL(szSql);
	if (!bRet)
	{
		theLog.Write(_T("!!CDbOperateHelper::CheckDbConn,2,fail,m_pAdo=%p"), m_pAdo);
	}
	return bRet;
}

BOOL CDbOperateHelper::ReConnDb()
{
	CCriticalSection2::Owner ow(m_cs);

	if (!m_pAdo)
	{
		CAdoFactory::Release(m_pAdo);
	}

	m_pAdo = CAdoFactory::CreateAdoEx();

	return (m_pAdo != NULL);
}

int CDbOperateHelper::GetDeptByUserId( int nUserId )
{
	CCriticalSection2::Owner ow(m_cs);
	ASSERT(m_pAdo);
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::GetDeptByUserId,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}
	CString sSQL;
	CStringArray ary;
	sSQL.Format(_T("select depid from t_s_AccountInfo where userid=%d"),nUserId);
	if (m_pAdo->GetAry(sSQL,ary) && ary.GetCount() > 0)
	{
		return _tstoi(ary.GetAt(0));
	}
	return 0;
}

BOOL CDbOperateHelper::GetJobAuditProcess( int nUserId,JobAuditProcess& process )
{
	if(!m_pAdo)
	{
		return FALSE;
	}
	CString sSQL;
	CStringArray ary;
	sSQL.Format(_T(" select top 1 audittype,adminid,nextaction,filtertype,filtercontent,membertype,FileName,FileExt,UserIds,id from ("
		" select a.* from t_s_auditprocess a left join "
		" t_s_AccountInfo b on a.Membertype = 2 and a.memberid = b.UserTypeID where b.userId = %d and a.State = 1 "	//用户类型
		" union all"
		" select a.* from t_s_auditprocess a left join "
		" t_s_AccountInfo b on a.Membertype = 1 and a.memberid = b.depid where b.userid = %d and a.State = 1 and a.AuditType=3"	//部门内部人员审核，(如果有内部人员审核，则优先此策略)
		" union all"
		" select a.* from t_s_auditprocess a left join "
		" t_s_AccountInfo b on a.Membertype = 1 and a.memberid = b.depid where b.userid = %d and a.State = 1 and a.AuditType<>3" //非部门内部人员审核
		" union all"
		" select a.* from t_s_auditprocess a where a.Membertype = 0 and a.State = 1 ) "	//全局
		" as res order by membertype desc"),
		nUserId,nUserId,nUserId);

	if(m_pAdo->GetAry(sSQL,ary) && ary.GetCount() > 0)
	{
		process.set_check_by((JOb_CHECK_BY)_tstoi(ary.GetAt(0)));
		process.set_admin_id(_tstoi(ary.GetAt(1)));
		process.set_next_action((JOB_CHECK_FAILURE_ACTION_TYPE)_tstoi(ary.GetAt(2)));
		process.set_userids(CCommonFun::UnicodeToUTF8(ary.GetAt(8)));
		process.set_id(_tstoi(ary.GetAt(9)));

		JobFilterPolciy* flt = process.mutable_flt_policy();
		if (process.check_by() == JOB_CHECK_BY_SYS)
		{
			if (flt)
			{
				flt->set_type((JOB_FILTER_TYPE)_tstoi(ary.GetAt(3)));
				flt->set_key_word(CCommonFun::UnicodeToUTF8(ary.GetAt(4)));
			}
		}
		if (process.check_by() == JOB_CHECK_BY_DEPTUSERS)
		{
			if (flt)
			{
				flt->set_type((JOB_FILTER_TYPE)_tstoi(ary.GetAt(3)));
				CString szFileName = ary.GetAt(6);
				CString szFileExt= ary.GetAt(7);
				if (szFileName.GetLength() > 0)
				{
					flt->set_key_word_docname(CCommonFun::UnicodeToUTF8(szFileName));
				}
				if (szFileExt.GetLength() > 0)
				{
					flt->set_key_word_doctype(CCommonFun::UnicodeToUTF8(szFileExt));
				}
			}
		}

		//当采用文档内容关键字审核时，则获取OCR审核方式
		if (flt && (flt->type() == JOB_FLT_BY_DOC_CONTENT))
		{
			PRINT_JOB_OCR_TYPE nOcrType = PRINT_JOB_OCR_TYPE_SERVER;
			CString szSetName(_T("PrintJobOcrPolicy"));
			CStringArray ary;
			if (GetSystemSetting(szSetName, ary) && ary.GetCount()>0)
			{
				nOcrType = (PRINT_JOB_OCR_TYPE)_tstoi(ary.GetAt(0));
			}
			flt->set_ocr_type(nOcrType);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CDbOperateHelper::AddCheck( CPrintAlarmInfo* pInfo )
{
	if(!m_pAdo || !pInfo)
	{
		theLog.Write(_T("!!CDbOperateHelper::AddCheck,m_pAdo=%p,pInfo=%p"), m_pAdo, pInfo);
		return FALSE;
	}
	CString sSQL;
	//打印审核
	if (pInfo->m_sUserIds.GetLength() > 0 || pInfo->m_sAdminIds.GetLength() > 0)
	{
		sSQL.Format(_T(" insert into t_log_Check "
			" (adminid,subject,content,state,CreatedON,type,jobid,userid,UserIds,DepId) "
			" values(N'%s',N'%s',N'%s',0,getdate(),%d,%d,%d,N'%s',%d)")
			,pInfo->m_sAdminIds,pInfo->m_sSubject,pInfo->m_sContent,pInfo->m_bNeedAudit,pInfo->m_nJobId
			,pInfo->m_nUserId,pInfo->m_sUserIds,pInfo->m_nDeptId);

		//添加审核通知
		AddCheckNotify(pInfo);
	}
	else
	{	
// 		//违规预警
// 		sSQL.Format(_T("insert into t_log_alarm(subject,content,state,CreatedOn,UserId) "
// 			" values (N'%s',N'%s',1 ,getdate(),%d) "),pInfo->m_sSubject,pInfo->m_sContent,pInfo->m_nUserId);
	}

	return m_pAdo->ExecuteSQL(sSQL);
}

BOOL CDbOperateHelper::AddCheckNotify( CPrintAlarmInfo* pInfo )
{
	if(!m_pAdo || !pInfo)
	{
		theLog.Write(_T("!!CSqlPrintImp::AddCheckNotify,m_pAdo=%p,pInfo=%p"), m_pAdo, pInfo);
		return FALSE;
	}

	CStringArray ary;
	CString sSQL;
	//获取通知策略：系统设置表单：【t_s_Systemset】
	//SetName ：AuditPendingNotic ：待审核通知；AuditedNotic：已审通知；
	//SetValue1 ：0:不开启通知；1：开启通知
	//SetValue2 ：send_timetype --发送时间类型 1:即时消息  3:自定义时间
	//SetValue3 ：send_type --发送类型：1：邮箱 
	//SetValue4 ：sent_time--自定义时间（1分钟-1440分钟（1天 24小时））
	//SetValue5 ：max_retrycount  --最大重试次数（0-10）
	sSQL.Format(_T(" select SetName, SetValue1, SetValue2, SetValue3, SetValue4, SetValue5 ")	//0-5
		_T(" from t_s_Systemset as A where SetName=N'AuditPendingNotic' "));
	if (!m_pAdo->GetAry(sSQL, ary) || ary.GetCount()<=0)
	{
		return FALSE;
	}

	BOOL bState = _tstoi(ary.GetAt(1));
	if (!bState)
	{
		return FALSE;
	}

	Message_Send_Time_Type nSendTimeType = (Message_Send_Time_Type)_tstoi(ary.GetAt(2));
	Message_Send_Type nSendType = (Message_Send_Type)_tstoi(ary.GetAt(3));
	Message_Type nMessageType = Message_Type_Check;
	int nSendTime = _tstoi(ary.GetAt(4));
	int nMaxRetryCount = _tstoi(ary.GetAt(5));

	sSQL.Empty();
	if (pInfo->m_sUserIds.GetLength() > 0)
	{
		//查找部门审核人员的邮件信息和被审核人员信息
		sSQL.Format(_T(" select A.MailAddr,B.code,B.Name from t_s_AccountInfo as A ")	//0-2
			_T(" inner join t_s_userex as B on B.id=%d and A.UserId in (%s) ")
			, pInfo->m_nUserId, pInfo->m_sUserIds);
	}

	if (pInfo->m_sAdminIds.GetLength() > 0)
	{
		//查找部门审核人员的邮件信息和被审核人员信息
		if (!sSQL.IsEmpty())
		{
			sSQL.Append(_T(" union all "));
		}
		sSQL.Format(_T(" select A.MailAddr,B.code,B.Name from P_AdminInfo as A ")	//0-2
			_T(" inner join t_s_userex as B on B.id=%d and A.ID in (%s) ")
			, pInfo->m_nUserId, pInfo->m_sAdminIds);
	}

	long lRow = 0;
	long lCol = 0;
	ary.RemoveAll();
	if (m_pAdo->GetRecordset(sSQL, ary, lRow, lCol) && lRow > 0)
	{
		for(int row = 0; row < lRow; row++)
		{
			long lIndex = lCol * row;
			CString szMailAddr = ary.GetAt(lIndex + 0);
			szMailAddr.Trim();
			if (szMailAddr.GetLength() <= 0)
			{
				continue;
			}
			CString szMessageContent;	//预警实际内容
			szMessageContent.Format(_T("[%s(%s)]  %s"), ary.GetAt(2), ary.GetAt(1), pInfo->m_sContent);
			sSQL.Format(_T("insert into t_log_message_pool " 
				" ( " 
				" subscribe_id, " 			//消息订阅ID,t_s_subscribe表id，因为是非订阅消息，默认为0
				" message_type , " 			//消息类型，0：未知，1-预警消息，2-审核消息，3-报表消息
				" message_to, "				//消息接收者
				" message_subject, " 		//消息主题
				" message_content , " 		//消息内容
				" message_attachment , " 	//消息附件
				" send_timetype, " 			//发送时间类型 1:即时消息  3:自定义时间
				" send_time, " 				//自定义时间（1分钟-1440分钟（1天 24小时））
				" send_type, "				//发送类型：1：邮箱，2：短信
				" send_failcount, "			//发送失败次数
				" max_retrycount, "			//最大失败重试次数
				" state, "					//状态 -1:删除 0：未发送，1:已经发送，2：发送失败
				" createdate , "			//创建时间
				" updatedate , "			//最后更新时间，一般为发送时间
				" comment "					//备注
				" )values( " 
				" %d,%d,N'%s',N'%s',N'%s',N'',%d,%d,%d,0,%d,0,getdate(),NULL,N'' " 
				" )"), 0, nMessageType, szMailAddr, pInfo->m_sSubject, szMessageContent, nSendTimeType, nSendTime, nSendType, nMaxRetryCount);
			if(!m_pAdo->ExecuteSQL(sSQL))
			{
				theLog.Write(_T("!!CSqlPrintImp::AddCheckNotify,failed"));
			}
		}
	}

	return FALSE;
}

//获取打印系统后端语言
UiLanguage CDbOperateHelper::GetBackendLanguage()
{
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::GetBackendLanguage,1,m_pAdo=%p"), m_pAdo);
		return UILANG_CHINESE;
	}

	CString szSql;
	szSql.Format(_T(" select SetValue1 from t_s_Systemset where SetName=N'BackendLanguage' "));
	int nBackendLanguage = m_pAdo->GetOneInt(szSql);
	if (nBackendLanguage == INT_MAX)
	{
		nBackendLanguage = UILANG_CHINESE;
	}

	return (UiLanguage)nBackendLanguage;
}

static BOOL GetUserJobAuditPolicyByDept(CAdo* pAdo, int nDeptId, int& nAuditTacticsId, int& nAuditType, BOOL& bAuditLog, CString& szAuditUsers, CString& szAuditAdmins, int nMaxRecursion)
{
	if ((nMaxRecursion--) <= 0)
	{
		theLog.Write(_T("!!GetUserJobAuditPolicyByDept,超过最大递归次数"));
		return FALSE;
	}

	if (!pAdo)
	{
		theLog.Write(_T("!!GetUserJobAuditPolicyByDept,1,pAdo=%p"), pAdo);
		return FALSE;
	}

	CString szSql;
	szSql.Format(_T(" SELECT A.ID,A.AUDIT_TYPE,A.AUDIT_LOG_STATUS,A.AUDITER,A.AdminAudit,t_department.pid FROM t_department "
		" LEFT JOIN t_s_AuditTactics as A ON A.ID = t_department.AuditTacticsId "
		" WHERE t_department.ID = %d "), nDeptId);
	CStringArray ary;
	if (pAdo->GetAry(szSql, ary) && ary.GetCount() >= 6)
	{
		int nId = _tstoi(ary.GetAt(0));
		if (nId > 0)
		{
			nAuditTacticsId = _tstoi(ary.GetAt(0));
			nAuditType = _tstoi(ary.GetAt(1));
			bAuditLog = (_tstoi(ary.GetAt(2)) == 0) ? FALSE : TRUE;	//SQL的bit数据类型，ado得到的是-1表示True，0-False
			szAuditUsers = ary.GetAt(3);
			szAuditAdmins = ary.GetAt(4);
			return TRUE;
		}
		else
		{
			int nDeptIdTemp = _tstoi(ary.GetAt(4));
			if (nDeptIdTemp > 0)
			{
				return GetUserJobAuditPolicyByDept(pAdo, nDeptIdTemp, nAuditTacticsId, nAuditType, bAuditLog, szAuditUsers, szAuditAdmins, nMaxRecursion);
			}
		}
	}
	return FALSE;
}

//获取用户的作业是否需要审核
//nAuditType：审核类型，默认为0（0:不启动，1：手动审核，2：OCR审核)
//bAuditLog：审计日志是否开启，默认false（true： 开启，false：关闭）
BOOL CDbOperateHelper::GetUserJobAuditPolicy(int nUserId, int& nAuditTacticsId, int& nAuditType, BOOL& bAuditLog, CString& szAuditUsers, CString& szAuditAdmins)
{
	if (!m_pAdo)
	{
		theLog.Write(_T("!!CDbOperateHelper::GetUserJobAuditPolicy,1,m_pAdo=%p"), m_pAdo);
		return FALSE;
	}

	CString szSql;
	szSql.Format(_T(" SELECT t_s_AuditTactics.ID,AUDIT_TYPE, AUDIT_LOG_STATUS, AUDITER, AdminAudit FROM t_s_AuditTactics "
		" LEFT JOIN t_s_AccountType ON t_s_AuditTactics.ID = t_s_AccountType.AuditTacticsId "
		" inner join t_s_AccountInfo on t_s_AccountInfo.UserId = %d and t_s_AccountInfo.UserTypeID = t_s_AccountType.ID"), nUserId);
	CStringArray ary;
	if (m_pAdo->GetAry(szSql, ary) && ary.GetCount() >= 5)
	{
		nAuditTacticsId = _tstoi(ary.GetAt(0));
		nAuditType = _tstoi(ary.GetAt(1));
		bAuditLog = (_tstoi(ary.GetAt(2)) == 0) ? FALSE : TRUE;	//SQL的bit数据类型，ado得到的是-1表示True，0-False
		szAuditUsers = ary.GetAt(3);
		szAuditAdmins = ary.GetAt(4);
	}
	else
	{
		int nDeptId = GetDeptByUserId(nUserId);
		GetUserJobAuditPolicyByDept(m_pAdo, nDeptId, nAuditTacticsId, nAuditType, bAuditLog, szAuditUsers, szAuditAdmins, 20);	//默认最大20级部门
	}

	return TRUE;
}