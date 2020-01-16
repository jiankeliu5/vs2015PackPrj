#include "StdAfx.h"
#include "SnmpJobMonitorJC.h"
#include "KyoceraMIB_Def.h"

CSnmpJobMonitorJC::CSnmpJobMonitorJC()
{
	InitOID();
}

CSnmpJobMonitorJC::~CSnmpJobMonitorJC(void)
{
}

void CSnmpJobMonitorJC::InitOID()
{
	m_szKyoceraActiveJobInfoJobCountOID = DecryptOID(KyoceraActiveJobInfoJobCountOID);

	m_szKyoceraActiveJobInfoJobIDOID = DecryptOID(KyoceraActiveJobInfoJobIDOID);
	m_szKyoceraActiveJobInfo3OID = DecryptOID(KyoceraActiveJobInfo3OID);
	m_szKyoceraActiveJobInfoJobTypeOID = DecryptOID(KyoceraActiveJobInfoJobTypeOID);
	m_szKyoceraActiveJobInfoJobNameOID = DecryptOID(KyoceraActiveJobInfoJobNameOID);
	m_szKyoceraActiveJobInfoJobStateOID = DecryptOID(KyoceraActiveJobInfoJobStateOID);
	m_szKyoceraActiveJobInfoJobBeginTimeOID = DecryptOID(KyoceraActiveJobInfoJobBeginTimeOID);
	m_szKyoceraActiveJobInfo8OID = DecryptOID(KyoceraActiveJobInfo8OID);
	m_szKyoceraActiveJobInfoPrintedCountOID = DecryptOID(KyoceraActiveJobInfoPrintedCountOID);
	m_szKyoceraActiveJobInfoPrintedCopyOID = DecryptOID(KyoceraActiveJobInfoPrintedCopyOID);
	m_szKyoceraActiveJobInfoTotalCopyOID = DecryptOID(KyoceraActiveJobInfoTotalCopyOID);
	m_szKyoceraActiveJobInfoUserNameOID = DecryptOID(KyoceraActiveJobInfoUserNameOID);
	m_szKyoceraActiveJobInfo13OID = DecryptOID(KyoceraActiveJobInfo13OID);
	m_szKyoceraActiveJobInfoFromOID = DecryptOID(KyoceraActiveJobInfoFromOID);
	m_szKyoceraActiveJobInfoOrgDocPageCountOID = DecryptOID(KyoceraActiveJobInfoOrgDocPageCountOID);
	m_szKyoceraActiveJobInfo16OID = DecryptOID(KyoceraActiveJobInfo16OID);
	m_szKyoceraActiveJobInfoColorTypeOID = DecryptOID(KyoceraActiveJobInfoColorTypeOID);

	m_szKyoceraHistoryJobInfoJobCountOID = DecryptOID(KyoceraHistoryJobInfoJobCountOID);

	m_szKyoceraHistoryJobInfoJobIDOID = DecryptOID(KyoceraHistoryJobInfoJobIDOID);
	m_szKyoceraHistoryJobInfoJobTypeOID = DecryptOID(KyoceraHistoryJobInfoJobTypeOID);
	m_szKyoceraHistoryJobInfoJobNameOID = DecryptOID(KyoceraHistoryJobInfoJobNameOID);
	m_szKyoceraHistoryJobInfoJobStateOID = DecryptOID(KyoceraHistoryJobInfoJobStateOID);
	m_szKyoceraHistoryJobInfoBeginTimeOID = DecryptOID(KyoceraHistoryJobInfoBeginTimeOID);
	m_szKyoceraHistoryJobInfoEndTimeOID = DecryptOID(KyoceraHistoryJobInfoEndTimeOID);
	m_szKyoceraHistoryJobInfo8OID = DecryptOID(KyoceraHistoryJobInfo8OID);
	m_szKyoceraHistoryJobInfoPrintedCountOID = DecryptOID(KyoceraHistoryJobInfoPrintedCountOID);
	m_szKyoceraHistoryJobInfoPrintedCopyOID = DecryptOID(KyoceraHistoryJobInfoPrintedCopyOID);
	m_szKyoceraHistoryJobInfoTotalCopyOID = DecryptOID(KyoceraHistoryJobInfoTotalCopyOID);
	m_szKyoceraHistoryJobInfoUserNameOID = DecryptOID(KyoceraHistoryJobInfoUserNameOID);
	m_szKyoceraHistoryJobInfo13OID = DecryptOID(KyoceraHistoryJobInfo13OID);
	m_szKyoceraHistoryJobInfo14OID = DecryptOID(KyoceraHistoryJobInfo14OID);
	m_szKyoceraHistoryJobInfoOrgDocPageCountOID = DecryptOID(KyoceraHistoryJobInfoOrgDocPageCountOID);
	m_szKyoceraHistoryJobInfo16OID = DecryptOID(KyoceraHistoryJobInfo16OID);
	m_szKyoceraHistoryJobInfoColorTypeOID = DecryptOID(KyoceraHistoryJobInfoColorTypeOID);
}

/************************************************************************/
/* ��ҵ����˵��
1.������ҵ��Ϊ����ǰ������ҵ���͡��Ѿ������ҵ���������ࡣ
2.����ӡ��ҵʱ���Ƚ������ϴλ�ȡ���ġ���ǰ������ҵ��ȫ������״̬Ϊ����ɡ���
3.�ٽ���⡾��ǰ������ҵ������MAP��û�У�����ӣ������������Ϣ��ϢΪ����ӡ�С���
4.�ٴμ��MAP�������ҵ״̬Ϊ����ɡ�������ʵ�ʵġ���ǰ������ҵ���б���ҵû���ҵ���
����Ϊ�Ѿ���ɡ�
5.�ڡ��Ѿ������ҵ���л�ȡָ����ɵ���ҵID����ҵ�����Ϣ��
*/
/************************************************************************/
void CSnmpJobMonitorJC::CheckJob()
{
	if (!CheckIPActive())
	{
		return;
	}

	POSITION pos = m_oJobMap.GetStartPosition();
	while(pos)
	{
		int nJobId = 0;
		PSCP_JobInfo pJobInfo = NULL;
		m_oJobMap.GetNextAssoc(pos, nJobId, pJobInfo);
		if (pJobInfo)
		{
			pJobInfo->JobStatus = SCP_JOB_STATUS_COMPLETED;
		}
		else
		{
			theLog.Write(_T("##CSnmpJobMonitorJC::CheckJob,1,RemoveKey �쳣��ҵ,nJobId = %d"), nJobId);
			m_oJobMap.RemoveKey(nJobId);
		}
	}

	char cOidStr[128] = {0};
	char cValue[256] = {0};

	int nCurrentJobCount = 0;
	if(GetRequest(CStringToChar(m_szKyoceraActiveJobInfoJobCountOID), nCurrentJobCount))
	{
		if (nCurrentJobCount > 0)
		{
			//theLog.Write(_T("CSnmpJobMonitorJC::CheckJob,1,nCurrentJobCount = %d"), nCurrentJobCount);
			for (int nJobIndex=1; nJobIndex<=nCurrentJobCount; nJobIndex++)
			{
				do 
				{
					//��ӡ��ҵID
					int nJobId = 0;
					sprintf(cOidStr, "%s.%d", m_szKyoceraActiveJobInfoJobIDOID, nJobIndex);
					if (!GetRequest(cOidStr, nJobId))
					{
						//theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,2,GetRequest fail, cOidStr = %s"), cOidStr);
						break;
					}

					
					//��ҵ���ͣ�1-��ӡ��2-��ӡ
					int nJobType = 0;
					sprintf(cOidStr, "%s.%d", m_szKyoceraActiveJobInfoJobTypeOID, nJobIndex);
					if (!GetRequest(cOidStr, nJobId))
					{
						//theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,3,GetRequest fail, cOidStr = %s"), cOidStr);
						break;
					}

					if (nJobType != 1)
					{
						//theLog.Write(_T("##CSnmpJobMonitorJC::CheckJob,4,nJobType = %d, Not Print Job(1)"), nJobType);
						continue;
					}

					//��ҵ״̬��1-�����У�2-��ӡ�У�4-��ͣ��5-�ȴ���
					int nJobInfoState = 0;
					sprintf(cOidStr, "%s.%d", m_szKyoceraActiveJobInfoJobStateOID, nJobIndex);
					if (!GetRequest(cOidStr, nJobInfoState))
					{
						//theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,5,GetRequest fail, cOidStr = %s"), cOidStr);
						break;
					}

					//��ӡ��ҵ��ӡҳ��
					int nJobInfoPage = 0;
					sprintf(cOidStr, "%s.%d", m_szKyoceraActiveJobInfoPrintedCountOID, nJobIndex);
					if (!GetRequest(cOidStr, nJobInfoPage))
					{
						//theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,6,GetRequest fail, cOidStr = %s"), cOidStr);
						break;
					}

					PSCP_JobInfo pJobInfo = NULL;
					if (!m_oJobMap.Lookup(nJobId, pJobInfo))
					{
						//�����ҵ
						pJobInfo = new SCP_JobInfo;
						memset(pJobInfo, 0x0, sizeof(SCP_JobInfo));
						pJobInfo->nJobId = nJobId;
						pJobInfo->nPrintedPage = nJobInfoPage;
						pJobInfo->JobType = SCP_JOB_TYPE_PRINT; 
						pJobInfo->JobStatus = SCP_JOB_STATUS_START;
						m_oJobMap.SetAt(nJobId, pJobInfo);
					}	
					else
					{
						if (pJobInfo)
						{	
							//������ҵ��Ϣ
							pJobInfo->nPrintedPage = nJobInfoPage;
							pJobInfo->JobStatus = SCP_JOB_STATUS_PRINTING;
						}
						else
						{
							//ɾ���쳣��ҵ
							m_oJobMap.RemoveKey(nJobId);
							theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,7,RemoveKey �쳣��ҵ,nJobId = %d"), nJobId);
						}
					}
				} while (FALSE);
			}
		}
	}
	else
	{
		//theLog.Write(_T("!!CSnmpJobMonitorJC::CheckJob,7,fail,KyoceraActiveJobInfoJobCountOID=%s"), m_szKyoceraActiveJobInfoJobCountOID);
		return;
	}

	pos = m_oJobMap.GetStartPosition();
	while(pos)
	{
		int nJobId = 0;
		PSCP_JobInfo pJobInfo = NULL;
		m_oJobMap.GetNextAssoc(pos, nJobId, pJobInfo);
		if (pJobInfo)
		{
			if (m_pCallBack)
			{
				if (pJobInfo->JobStatus == SCP_JOB_STATUS_COMPLETED)
				{
					int nPrintedCount = GetPrintedCountByJobId(pJobInfo->nJobId);
					if (pJobInfo->nPrintedPage < nPrintedCount )
					{
						pJobInfo->nPrintedPage = nPrintedCount;
					}
					else
					{
						theLog.Write(_T("##CSnmpJobMonitorJC::CheckJob,8,nJobId=%d,nPrintedPage=%d,nPrintedCount=%d")
							, pJobInfo->nJobId, pJobInfo->nPrintedPage, nPrintedCount);
					}
				}
				m_pCallBack->OnJobInfo(pJobInfo);
			}

			if (pJobInfo->JobStatus == SCP_JOB_STATUS_COMPLETED)
			{
				m_oJobMap.RemoveKey(nJobId);
				delete pJobInfo;
			}
		}
	}
}

int CSnmpJobMonitorJC::CheckLastPrintCount()
{
	//CheckLastPrintCount_Color��CheckLastPrintCount_WBһ��Ҫ����CheckLastPrintCount����
	CheckLastPrintCount_Color();
	CheckLastPrintCount_WB();
	m_nLastPrintCount = m_nLastPrintCount_Color + m_nLastPrintCount_WB;
	return m_nLastPrintCount;
}

int CSnmpJobMonitorJC::CheckLastPrintCount_Color()
{
	m_nLastPrintCount_Color = 0;
	return m_nLastPrintCount_Color;
}

int CSnmpJobMonitorJC::CheckLastPrintCount_WB()
{
	m_nLastPrintCount_WB = 0;
	return m_nLastPrintCount_WB;
}

int CSnmpJobMonitorJC::CheckLastCopyCount()
{
	//CheckLastCopyCount_Color��CheckLastCopyCount_WBһ��Ҫ����CheckLastCopyCount����
	CheckLastCopyCount_Color();
	CheckLastCopyCount_WB();
	m_nLastCopyCount = m_nLastCopyCount_Color + m_nLastCopyCount_WB;
	return m_nLastCopyCount;
}

int CSnmpJobMonitorJC::CheckLastCopyCount_Color()
{
	m_nLastCopyCount_Color = 0;
	return m_nLastCopyCount_Color;
}

int CSnmpJobMonitorJC::CheckLastCopyCount_WB()
{
	m_nLastCopyCount_WB = 0;
	return m_nLastCopyCount_WB;
}

int CSnmpJobMonitorJC::CheckLastScanCount()
{
	m_nLastScanCount = 0;
	return m_nLastScanCount;
}

int CSnmpJobMonitorJC::CheckLastScanCount_Color()
{
	m_nLastScanCount_Color = 0;
	return m_nLastScanCount_Color;
}

int CSnmpJobMonitorJC::CheckLastScanCount_WB()
{
	m_nLastScanCount_WB = 0;
	return m_nLastScanCount_WB;
}

int CSnmpJobMonitorJC::CheckLastFaxCount()
{
	CheckLastFaxCount_Color();
	CheckLastFaxCount_WB();
	m_nLastFaxCount = m_nLastFaxCount_Color + m_nLastFaxCount_WB;
	return m_nLastFaxCount;
}

int CSnmpJobMonitorJC::CheckLastFaxCount_Color()
{
	m_nLastFaxCount_Color = 0;
	return m_nLastFaxCount_Color;
}

int CSnmpJobMonitorJC::CheckLastFaxCount_WB()
{
	m_nLastFaxCount_WB = 0;
	return m_nLastFaxCount_WB;
}


int CSnmpJobMonitorJC::CheckLastPageCount_A3()
{
	m_nA3PageCount = 0;
	return m_nA3PageCount;
}

int CSnmpJobMonitorJC::CheckLastPageCount_A4()
{
	m_nA4PageCount = 0;
	return m_nA4PageCount;
}

//��������ȡ��ӡ����������
int CSnmpJobMonitorJC::GetPrintedCountByJobId(int nJobId)
{
	//theLog.Write(_T("CSnmpJobMonitorJC::GetPrintedCountByJobId,1,nJobId=%d"), nJobId);

	int nPrintedCount = 0;
	//��ҵ�Ѿ���ֽҳ��
	CStringA szPrintedCount;
	szPrintedCount.Format("%s.%d", m_szKyoceraHistoryJobInfoPrintedCountOID, nJobId);
	if (!GetRequest(CStringToChar(szPrintedCount), nPrintedCount))
	{
		theLog.Write(_T("!!CSnmpJobMonitorJC::GetPrintedCountByJobId,2,GetRequest fail"));
	}

	return nPrintedCount;
}
