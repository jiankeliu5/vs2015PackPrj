#include "StdAfx.h"
#include "SnmpJobMonitorDZ.h"
#include "ToshibaMIB_Def.h"

CSnmpJobMonitorDZ::CSnmpJobMonitorDZ()
{
	InitOID();
}

CSnmpJobMonitorDZ::~CSnmpJobMonitorDZ(void)
{
}

void CSnmpJobMonitorDZ::InitOID()
{
	m_szToshibaTotalPageCountOID = DecryptOID(ToshibaTotalPageCountOID);
}

void CSnmpJobMonitorDZ::CheckJob()
{
	if (!CheckIPActive())
	{
		return;
	}

	GetPrintJobList();
}

int CSnmpJobMonitorDZ::CheckLastPrintCount()
{
	//CheckLastPrintCount_Color��CheckLastPrintCount_WBһ��Ҫ����CheckLastPrintCount����
	CheckLastPrintCount_Color();
	CheckLastPrintCount_WB();
	m_nLastPrintCount = m_nLastPrintCount_Color + m_nLastPrintCount_WB;
	return m_nLastPrintCount;
}

int CSnmpJobMonitorDZ::CheckLastPrintCount_Color()
{
	m_nLastPrintCount_Color = 0;
	return m_nLastPrintCount_Color;
}

int CSnmpJobMonitorDZ::CheckLastPrintCount_WB()
{
	m_nLastPrintCount_WB = 0;
	return m_nLastPrintCount_WB;
}

int CSnmpJobMonitorDZ::CheckLastCopyCount()
{
	//CheckLastCopyCount_Color��CheckLastCopyCount_WBһ��Ҫ����CheckLastCopyCount����
	CheckLastCopyCount_Color();
	CheckLastCopyCount_WB();
	m_nLastCopyCount = m_nLastCopyCount_Color + m_nLastCopyCount_WB;
	return m_nLastCopyCount;
}

int CSnmpJobMonitorDZ::CheckLastCopyCount_Color()
{
	m_nLastCopyCount_Color = 0;
	return m_nLastCopyCount_Color;
}

int CSnmpJobMonitorDZ::CheckLastCopyCount_WB()
{
	m_nLastCopyCount_WB = 0;
	return m_nLastCopyCount_WB;
}

int CSnmpJobMonitorDZ::CheckLastScanCount()
{
	m_nLastScanCount = 0;
	return m_nLastScanCount;
}

int CSnmpJobMonitorDZ::CheckLastScanCount_Color()
{
	m_nLastScanCount_Color = 0;
	return m_nLastScanCount_Color;
}

int CSnmpJobMonitorDZ::CheckLastScanCount_WB()
{
	m_nLastScanCount_WB = 0;
	return m_nLastScanCount_WB;
}

int CSnmpJobMonitorDZ::CheckLastFaxCount()
{
	CheckLastFaxCount_Color();
	CheckLastFaxCount_WB();
	m_nLastFaxCount = m_nLastFaxCount_Color + m_nLastFaxCount_WB;
	return m_nLastFaxCount;
}

int CSnmpJobMonitorDZ::CheckLastFaxCount_Color()
{
	m_nLastFaxCount_Color = 0;
	return m_nLastFaxCount_Color;
}

int CSnmpJobMonitorDZ::CheckLastFaxCount_WB()
{
	m_nLastFaxCount_WB = 0;
	return m_nLastFaxCount_WB;
}

int CSnmpJobMonitorDZ::CheckLastPageCount_A3()
{
	m_nA3PageCount = 0;
	return m_nA3PageCount;
}

int CSnmpJobMonitorDZ::CheckLastPageCount_A4()
{
	m_nA4PageCount = 0;
	return m_nA4PageCount;
}

//������ֻ�ܻ�ȡ��ӡ��������������
int CSnmpJobMonitorDZ::GetPrintedCountByJobId(int nJobId)
{
	return 0;
}

int CSnmpJobMonitorDZ::CheckLastPrintCount2()
{
	int nAllPages = 0;

	//�ܼ�����ԭ������
	if (!GetRequest(CStringToChar(m_szToshibaTotalPageCountOID), nAllPages))	//AllPages=".1.3.6.1.4.1.1129.2.3.50.1.3.21.6.1.3.1.4"
	{
		theLog.Write(_T("!!CSnmpJobMonitorDZ::CheckLastPrintCount2,GetRequest fail"));
	}

	m_nLastPrintCount = nAllPages;
	return m_nLastPrintCount;
}

//��ȡ�Ѿ���ӡ�б�
void CSnmpJobMonitorDZ::GetPrintJobList()
{
	/* ͨ������һ���ⲿ��������ȡ��ӡ��־���ⲿ�����ǻ���.Net4.0���л����ġ�
	ʹ�������е��ã�����˳�򴫵�10��������
	ʾ����OperatePrinterLog.exe GetLog "192.168.4.180" "user" "password" 10 "SearchKey" "SearchValue" 1 100 "C:\printlog.xml"
	���У�
	����1��GetLog����ʶ�ǻ�ȡ��־
	����2����ӡ����IP
	����3����ӡ���ĵ�¼�û�,Ĭ��Ϊadmin
	����4����ӡ���ĵ�¼����,Ĭ��Ϊ123456
	����5����ȡ����־���ͣ�5 - ɨ�裬6 - ��ӡ��7 - ���棬8 - ������գ�9 - ���淢�ͣ�10 - ��ӡ��17 - Maintenance
	����6����־�������ͣ�< �� >��< UserName > ���� < DepartmentCode >
	����7����־��������
	����8����־����ʼ��¼
	����9����ȡ��־��������
	����10����־������·����һ����XML��ʽ
	*/

	theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,1,begin"));

	CString szPrinterIP = GetPrtIP();
	CString szUser = _T("admin");
	CString szPassword = _T("123456");
	CString szDefaultPath = CCommonFun::GetDefaultPath();
	CString szToolPath;
	szToolPath.Format(_T("%sToshiba\\OperatePrinterLog.exe"), szDefaultPath);
	CString szLogPath;
	COleDateTime dt = COleDateTime::GetCurrentTime();
	szLogPath.Format(_T("%sToshiba\\printlog_%s.xml"), szDefaultPath, szPrinterIP);

	//ɾ��ԭ������־�ļ�
	DeleteFile(szLogPath);

	CString szCmdLine;
	szCmdLine.Format(_T("\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %d \"%s\" \"%s\" %d %d \"%s\" ")
		, szToolPath, _T("GetLog"), szPrinterIP, szUser, szPassword, ToshibaLogType_Print, _T(""), _T(""), 1, 100, szLogPath);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(0, szCmdLine.GetBuffer(), 0, 0, FALSE, 0, 0, 0, &si, &pi))
	{
		theLog.Write(_T("!!CSnmpJobMonitorDZ::GetPrintJobList,2,CreateProcess fail,error=%u,szCmdLine=%s"), GetLastError(), szCmdLine);
		return;
	}

	BOOL bGetLogSucc = FALSE;
	theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,3,CreateProcess succ,dwProcessId=%u"), pi.dwProcessId);
	DWORD dwTimeoutSecond = 60 * 3;
	DWORD dwRet = WaitForSingleObject(pi.hProcess, dwTimeoutSecond * 1000);
	if (dwRet == WAIT_TIMEOUT)
	{
		theLog.Write(_T("!!CSnmpJobMonitorDZ::GetPrintJobList,4,WaitForSingleObject WAIT_TIMEOUT ��ҵ����ʱ(%d��)��ǿ�ƽ�����ӡ��ҵ,dwProcessId=%u")
			, dwTimeoutSecond, pi.dwProcessId);
		CCommonFun::TerminaPID(pi.dwProcessId);
	}
	else
	{
		theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,5,WaitForSingleObject succ,dwProcessId=%u"), pi.dwProcessId);
		DWORD dwExitCode = 0;	//STILL_ACTIVE,EXCEPTION_INT_DIVIDE_BY_ZERO
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		if (dwExitCode == 1)
		{
			bGetLogSucc = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CSnmpJobMonitorDZ::GetPrintJobList,6,fail,dwExitCode=%u(0x%X),dwProcessId=%u")
				, dwExitCode, dwExitCode, pi.dwProcessId);
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (bGetLogSucc)
	{
		SCP_JobInfo_List PrintedJobList;

		CXmlParse xml;
		if (xml.ParseXml(szLogPath))
		{
			//��������ӡ��־����Ϊ��ȡ��ӡ��־�Ľӿ��У�ʵ�ʻ�õ���ӡ�͸�ӡ������־��
			ClearCopyLog(xml);

			//Ȼ�����ӡ��־
			do
			{
				MSXML2::IXMLDOMDocument2Ptr doc = xml.GetXmlDocment();
				MSXML2::IXMLDOMNodePtr getLogResponse = xml.SelectSingleNode(doc, _T("getLogResponse"));
				MSXML2::IXMLDOMNodePtr Log = xml.SelectSingleNode(getLogResponse, _T("Log"));
				MSXML2::IXMLDOMNodePtr OperationLog = xml.SelectSingleNode(Log, _T("OperationLog"));
				MSXML2::IXMLDOMNodePtr JobLog = xml.SelectSingleNode(OperationLog, _T("JobLog"));
				MSXML2::IXMLDOMNodePtr PrintLog = xml.SelectSingleNode(JobLog, _T("PrintLog"));
				MSXML2::IXMLDOMNodePtr CopyLog = xml.SelectSingleNode(JobLog, _T("CopyLog"));
				MSXML2::IXMLDOMNodeListPtr PrintLogEntryList = xml.SelectNodes(PrintLog, _T("PrintLogEntry"));
				MSXML2::IXMLDOMNodeListPtr CopyLogEntryList = xml.SelectNodes(CopyLog, _T("CopyLogEntry"));
				if (PrintLogEntryList)
				{
					long nCount = PrintLogEntryList->Getlength();
					theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,7,PrintLog,nCount=%d"), nCount);
					for (long index = 0; index < nCount; index++)
					{
						CString szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages,
							szTotalPapers, szPrintedPages, szDocumentName;
						MSXML2::IXMLDOMNodePtr PrintLogEntry = PrintLogEntryList->Getitem(index);
						xml.GetNodeValue(PrintLogEntry, _T("Id"), szId);
						xml.GetNodeValue(PrintLogEntry, _T("JobType"), szJobType);
						xml.GetNodeValue(PrintLogEntry, _T("ColorMode"), szColorMode);
						xml.GetNodeValue(PrintLogEntry, _T("PaperSize"), szPaperSize);
						xml.GetNodeValue(PrintLogEntry, _T("Sets"), szSets);
						xml.GetNodeValue(PrintLogEntry, _T("OriginalPages"), szOriginalPages);
						xml.GetNodeValue(PrintLogEntry, _T("TotalPapers"), szTotalPapers);
						xml.GetNodeValue(PrintLogEntry, _T("PrintedPages"), szPrintedPages);
						xml.GetNodeValue(PrintLogEntry, _T("DocumentName"), szDocumentName);

						theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,Id=%s,szJobType=%s,szColorMode=%s"
							",szPaperSize=%s,szSets=%s, szOriginalPages=%s, szTotalPapers=%s, szPrintedPages=%s"
							", szDocumentName=%s")
							, szId, szJobType, szColorMode
							, szPaperSize, szSets, szOriginalPages, szTotalPapers, szPrintedPages
							, szDocumentName);

						//��ȡBasicInfo
						CString szStartTime, szFinishTime, szStatus, szStatusCode;
						MSXML2::IXMLDOMNodePtr BasicInfo = xml.SelectSingleNode(PrintLogEntry, _T("BasicInfo"));
						xml.GetNodeValue(BasicInfo, _T("StartTime"), szStartTime);
						xml.GetNodeValue(BasicInfo, _T("FinishTime"), szFinishTime);
						xml.GetNodeValue(BasicInfo, _T("Status"), szStatus);
						xml.GetNodeValue(BasicInfo, _T("StatusCode"), szStatusCode);

						/*
						theLog.Write("CSnmpJobMonitorDZ::GetPrintJobList,8,szId=%s,szJobType=%s,szColorMode=%s,"
						"szPaperSize=%s,szSets=%s,szOriginalPages=%s,szTotalPapers=%s,szPrintedPages=%s,"
						"szDocumentName=%s,szStartTime=%s,szFinishTime=%s,szStatus=%s,szStatusCode=%s"
						, szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages, szTotalPapers
						, szPrintedPages, szDocumentName, szStartTime, szFinishTime, szStatus, szStatusCode);
						*/

						SCP_JobInfo info;
						memset(&info, 0x0, sizeof(SCP_JobInfo));
						info.nJobId = _tstoi(szId);
						info.nPrintedPage = _tstoi(szPrintedPages);
						info.nTotalPage = _tstoi(szOriginalPages);
						int nJobType = _tstoi(szJobType);
						info.JobType = SCP_JOB_TYPE_PRINT;
						info.nCopies = _tstoi(szSets);
						//info.bColor = (szColorMode.CompareNoCase("Monochrome") == 0) ? false : true;
						//info.nMediaSize = (szColorMode.CompareNoCase("A4") == 0) ? 9 : 8;
						//COleDateTime time;
						//time.ParseDateTime(szStartTime);
						//info.fJobStartTime = time.m_dt;
						//time.ParseDateTime(szFinishTime);
						//info.fJobEndTime = time.m_dt;
						strcpy_s(info.cJobName, CCommonFun::UnicodeToUTF8(szDocumentName));
						int nStatusCode = _tstoi(szStatusCode);
						info.JobStatus = SCP_JOB_STATUS_COMPLETED;
						/*
						if (nStatusCode == 4000)
						{
						info.JobStatus = SCP_JOB_STATUS_COMPLETED;
						}
						else if (nStatusCode == 4011)
						{
						info.JobStatus = SCP_JOB_STATUS_CANCELED;
						}
						else
						{
						info.JobStatus = SCP_JOB_STATUS_PRINTING;
						}
						*/

						PrintedJobList.push_back(info);
					}
				}
				if (CopyLogEntryList)
				{
					long nCount = CopyLogEntryList->Getlength();
					theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,8,CopyLog��nCount=%d"), nCount);
					for (long index = 0; index < nCount; index++)
					{
						CString szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages,
							szTotalPapers, szPrintedPages, szDuplexMode;
						MSXML2::IXMLDOMNodePtr CopyLogEntry = CopyLogEntryList->Getitem(index);
						xml.GetNodeValue(CopyLogEntry, _T("Id"), szId);
						xml.GetNodeValue(CopyLogEntry, _T("ColorMode"), szColorMode);
						xml.GetNodeValue(CopyLogEntry, _T("PaperSize"), szPaperSize);
						xml.GetNodeValue(CopyLogEntry, _T("Sets"), szSets);
						xml.GetNodeValue(CopyLogEntry, _T("OriginalPages"), szOriginalPages);
						xml.GetNodeValue(CopyLogEntry, _T("TotalPapers"), szTotalPapers);
						xml.GetNodeValue(CopyLogEntry, _T("PrintedPages"), szPrintedPages);
						xml.GetNodeValue(CopyLogEntry, _T("DuplexMode"), szDuplexMode);

						theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,Id=%s,szJobType=%s,szColorMode=%s"
							",szPaperSize=%s,szSets=%s, szOriginalPages=%s, szTotalPapers=%s, szPrintedPages=%s"
							", szDocumentName=%s")
							, szId, szJobType, szColorMode
							, szPaperSize, szSets, szOriginalPages, szTotalPapers, szPrintedPages
							, szDuplexMode);

						//��ȡBasicInfo
						CString szStartTime, szFinishTime, szStatus, szStatusCode;
						MSXML2::IXMLDOMNodePtr BasicInfo = xml.SelectSingleNode(CopyLogEntry, _T("BasicInfo"));
						xml.GetNodeValue(BasicInfo, _T("StartTime"), szStartTime);
						xml.GetNodeValue(BasicInfo, _T("FinishTime"), szFinishTime);
						xml.GetNodeValue(BasicInfo, _T("Status"), szStatus);
						xml.GetNodeValue(BasicInfo, _T("StatusCode"), szStatusCode);

						/*
						theLog.Write("CSnmpJobMonitorDZ::GetPrintJobList,8,szId=%s,szJobType=%s,szColorMode=%s,"
						"szPaperSize=%s,szSets=%s,szOriginalPages=%s,szTotalPapers=%s,szPrintedPages=%s,"
						"szDocumentName=%s,szStartTime=%s,szFinishTime=%s,szStatus=%s,szStatusCode=%s"
						, szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages, szTotalPapers
						, szPrintedPages, szDocumentName, szStartTime, szFinishTime, szStatus, szStatusCode);
						*/

						SCP_JobInfo info;
						memset(&info, 0x0, sizeof(SCP_JobInfo));
						info.nJobId = _tstoi(szId);
						info.nPrintedPage = _tstoi(szPrintedPages);
						info.nTotalPage = _tstoi(szOriginalPages);
						int nJobType = _tstoi(szJobType);
						info.JobType = SCP_JOB_TYPE_COPY;
						info.nCopies = _tstoi(szSets);
						//info.bColor = (szColorMode.CompareNoCase("Monochrome") == 0) ? false : true;
						//info.nMediaSize = (szColorMode.CompareNoCase("A4") == 0) ? 9 : 8;
						//COleDateTime time;
						//time.ParseDateTime(szStartTime);
						//info.fJobStartTime = time.m_dt;
						//time.ParseDateTime(szFinishTime);
						//info.fJobEndTime = time.m_dt;
						int nStatusCode = _tstoi(szStatusCode);
						info.JobStatus = SCP_JOB_STATUS_COMPLETED;
						info.bColor = szColorMode.Find(_T("Color")) >= 0;
						info.nMediaSize = (szPaperSize.Find(_T("A3"))>=0 || szPaperSize.Find(_T("a3")) >= 0) ? A3_PAPER : A4_PAPER;
						/*
						if (nStatusCode == 4000)
						{
						info.JobStatus = SCP_JOB_STATUS_COMPLETED;
						}
						else if (nStatusCode == 4011)
						{
						info.JobStatus = SCP_JOB_STATUS_CANCELED;
						}
						else
						{
						info.JobStatus = SCP_JOB_STATUS_PRINTING;
						}
						*/

						PrintedJobList.push_back(info);
					}
				}
			} while (FALSE);
		}
		else
		{
			theLog.Write(_T("!!CSnmpJobMonitorDZ::GetPrintJobList,9,ParseXml fail"));
		}

		if (PrintedJobList.size() > 0 && m_pCallBack)
		{
			m_pCallBack->OnJobInfoList(&PrintedJobList);
			//�����ӡ��¼
			ClearPrintJobList(PrintedJobList);
		}
	}

	theLog.Write(_T("CSnmpJobMonitorDZ::GetPrintJobList,100,end"));
}

//�����ӡ���ϵĴ�ӡ��¼
void CSnmpJobMonitorDZ::ClearPrintJobList(SCP_JobInfo_List& PrintedJobList)
{
	SCP_JobInfo_List_I it;
	for (it = PrintedJobList.begin(); it != PrintedJobList.end(); it++)
	{
		//if (it->JobVerifyResult == SCP_JOB_VERIFY_RESULT_SUCC)
		{
			theLog.Write(_T("CSnmpJobMonitorDZ::ClearPrintJobList,nJobId=%d,cJobName=%S")
				, it->nJobId, it->cJobName);
			ClearJobLog(ToshibaLogType_Print, it->nJobId);
		}
	}
}

//����ӡ��¼��־
void CSnmpJobMonitorDZ::ClearCopyLog(CXmlParse& xml)
{
	MSXML2::IXMLDOMDocument2Ptr doc = xml.GetXmlDocment();
	MSXML2::IXMLDOMNodePtr getLogResponse = xml.SelectSingleNode(doc, _T("getLogResponse"));
	MSXML2::IXMLDOMNodePtr Log = xml.SelectSingleNode(getLogResponse, _T("Log"));
	MSXML2::IXMLDOMNodePtr OperationLog = xml.SelectSingleNode(Log, _T("OperationLog"));
	MSXML2::IXMLDOMNodePtr JobLog = xml.SelectSingleNode(OperationLog, _T("JobLog"));
	MSXML2::IXMLDOMNodePtr CopyLog = xml.SelectSingleNode(JobLog, _T("CopyLog"));
	MSXML2::IXMLDOMNodeListPtr CopyLogEntryList = xml.SelectNodes(CopyLog, _T("CopyLogEntry"));
	if (!CopyLogEntryList)
	{
		theLog.Write(_T("##CSnmpJobMonitorDZ::ClearCopyLog,1,CopyLogEntryList=%p"), CopyLogEntryList);
		return;
	}
	long nCount = CopyLogEntryList->Getlength();
	theLog.Write(_T("CSnmpJobMonitorDZ::ClearCopyLog,2,nCount=%d"), nCount);
	for (long index = 0; index<nCount; index++)
	{
		CString szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages,
			szTotalPapers, szPrintedPages, szDocumentName;
		MSXML2::IXMLDOMNodePtr CopyLogEntry = CopyLogEntryList->Getitem(index);
		xml.GetNodeValue(CopyLogEntry, _T("Id"), szId);
		/*
		xml.GetNodeValue(CopyLogEntry, "JobType", szJobType);
		xml.GetNodeValue(CopyLogEntry, "ColorMode", szColorMode);
		xml.GetNodeValue(CopyLogEntry, "PaperSize", szPaperSize);
		xml.GetNodeValue(CopyLogEntry, "Sets", szSets);
		xml.GetNodeValue(CopyLogEntry, "OriginalPages", szOriginalPages);
		xml.GetNodeValue(CopyLogEntry, "TotalPapers", szTotalPapers);
		xml.GetNodeValue(CopyLogEntry, "PrintedPages", szPrintedPages);
		xml.GetNodeValue(CopyLogEntry, "DocumentName", szDocumentName);

		//��ȡBasicInfo
		CString szStartTime, szFinishTime, szStatus, szStatusCode;
		MSXML2::IXMLDOMNodePtr BasicInfo = xml.SelectSingleNode(CopyLogEntry, "BasicInfo");
		xml.GetNodeValue(BasicInfo, "StartTime", szStartTime);
		xml.GetNodeValue(BasicInfo, "FinishTime", szFinishTime);
		xml.GetNodeValue(BasicInfo, "Status", szStatus);
		xml.GetNodeValue(BasicInfo, "StatusCode", szStatusCode);

		theLog.Write("CSnmpJobMonitorDZ::ClearCopyLog,3,szId=%s,szJobType=%s,szColorMode=%s,"
		"szPaperSize=%s,szSets=%s,szOriginalPages=%s,szTotalPapers=%s,szPrintedPages=%s,"
		"szDocumentName=%s,szStartTime=%s,szFinishTime=%s,szStatus=%s,szStatusCode=%s"
		, szId, szJobType, szColorMode, szPaperSize, szSets, szOriginalPages, szTotalPapers
		, szPrintedPages, szDocumentName, szStartTime, szFinishTime, szStatus, szStatusCode);
		*/

		ClearJobLog(ToshibaLogType_Copy, _tstoi(szId));
	}

	theLog.Write(_T("CSnmpJobMonitorDZ::ClearCopyLog,100,end"));
}

//����ָ�����ͺ�ID����־��¼
void CSnmpJobMonitorDZ::ClearJobLog(ToshibaLogType nLogType, int nLogId)
{
	/* ͨ������һ���ⲿ������������־���ⲿ�����ǻ���.Net4.0���л����ġ�
	ʹ�������е��ã�����˳�򴫵�6��������
	ʾ����OperatePrinterLog.exe DeleteLog "192.168.4.180" "user" "password" 10 8024
	���У�
	����1��ClearLog����ʶ��������־
	����2����ӡ����IP
	����3����ӡ���ĵ�¼�û�,Ĭ��Ϊadmin
	����4����ӡ���ĵ�¼����,Ĭ��Ϊ123456
	����5���������־���ͣ�5 - ɨ�裬6 - ��ӡ��7 - ���棬8 - ������գ�9 - ���淢�ͣ�10 - ��ӡ��17 - Maintenance
	����6���������־ID
	*/

	theLog.Write(_T("CSnmpJobMonitorDZ::ClearJobLog,1,begin,nLogType=%d,nLogId=%d"), nLogType, nLogId);

	CString szPrinterIP = GetPrtIP();
	CString szUser = _T("admin");
	CString szPassword = _T("123456");
	CString szDefaultPath = CCommonFun::GetDefaultPath();
	CString szToolPath;
	szToolPath.Format(_T("%sToshiba\\OperatePrinterLog.exe"), szDefaultPath);

	CString szCmdLine;
	szCmdLine.Format(_T("\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %d %d ")
		, szToolPath, _T("ClearLog"), szPrinterIP, szUser, szPassword, nLogType, nLogId);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	// 	si.dwFlags = STARTF_USESHOWWINDOW;
	// 	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(0, szCmdLine.GetBuffer(), 0, 0, FALSE, 0, 0, 0, &si, &pi))
	{
		theLog.Write(_T("!!CSnmpJobMonitorDZ::ClearJobLog,2,CreateProcess fail,error=%u,szCmdLine=%s")
			, GetLastError(), szCmdLine);
		return;
	}

	BOOL bGetLogSucc = FALSE;
	//theLog.Write("CSnmpJobMonitorDZ::ClearJobLog,3,CreateProcess succ,dwProcessId=%u", pi.dwProcessId);
	DWORD dwTimeoutSecond = 60 * 3;
	DWORD dwRet = WaitForSingleObject(pi.hProcess, dwTimeoutSecond * 1000);
	if (dwRet == WAIT_TIMEOUT)
	{
		theLog.Write(_T("!!CSnmpJobMonitorDZ::ClearJobLog,4,WaitForSingleObject WAIT_TIMEOUT ��ҵ����ʱ(%d��)��ǿ�ƽ�����ӡ��ҵ,dwProcessId=%u")
			, dwTimeoutSecond, pi.dwProcessId);
		CCommonFun::TerminaPID(pi.dwProcessId);
	}
	else
	{
		//theLog.Write("CSnmpJobMonitorDZ::ClearJobLog,5,WaitForSingleObject succ,dwProcessId=%u", pi.dwProcessId);
		DWORD dwExitCode = 0;	//STILL_ACTIVE,EXCEPTION_INT_DIVIDE_BY_ZERO
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		if (dwExitCode == 1)
		{
			bGetLogSucc = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CSnmpJobMonitorDZ::ClearJobLog,6,fail,dwExitCode=%u(0x%X),dwProcessId=%u"), dwExitCode, dwExitCode, pi.dwProcessId);
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	theLog.Write(_T("CSnmpJobMonitorDZ::ClearJobLog,100,end"));
}
