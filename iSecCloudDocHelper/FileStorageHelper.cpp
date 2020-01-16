#include "stdafx.h"

CFileStorageHelper::CFileStorageHelper()
{
}

CFileStorageHelper& CFileStorageHelper::GetInstance()
{
	static CFileStorageHelper one ;
	return one;
}

void CFileStorageHelper::InitFileStorageList(FileStorageList& list)
{
	CCriticalSection2::Owner ow(m_cs4FileStorage);

	theLog.Write(_T("CFileStorageHelper::InitFileStorageList"));
	m_oFileStorageList.Clear();
	m_oFileStorageIPRangeList.RemoveAll();
	m_oFileStorageList.CopyFrom(list);

	int nListSize = m_oFileStorageList.list_size(); 
	for (int index=0; index<nListSize; index++)
	{
		AddFileStorage(m_oFileStorageList.mutable_list(index));
	}
}

void CFileStorageHelper::GetBestFileStorageList(CString szClientIP, CFileStorageList& list)
{
	CCriticalSection2::Owner ow(m_cs4FileStorage);

	DWORD dwClientIP = CCommonFun::ConverIPToDWORD(szClientIP);
	int nMaxCount = FILE_STORAGE_MAX_TRY_COUNT;	//最大选取存储个数

	POSITION pos = m_oFileStorageIPRangeList.GetHeadPosition();
	for (int i = 0; i < m_oFileStorageIPRangeList.GetCount(); i++)
	{
		const FileStorageIPRange& range = m_oFileStorageIPRangeList.GetNext(pos);
		if ((dwClientIP >= range.dwIPBegin) && (dwClientIP <= range.dwIPEnd))
		{
			nMaxCount--;
			FileStorage* pOne = new FileStorage;	//要求在外部delete，此处是从加锁保护的变量中取数据
			pOne->CopyFrom(*range.pFileStorage);
			if ((range.dwIPBegin == 0) && (range.dwIPEnd == 0xFFFFFFFF))
			{
				//这个是一个全范围的ip，默认添加到列表尾部
				list.AddTail(pOne);
			}
			else
			{
				//这个是一个合法范围的ip，默认添加到列表头部，这样能保证住优先使用此类存储服务
				list.AddHead(pOne);
			}
			CString szIP = CCommonFun::UTF8ToUnicode(range.pFileStorage->ip().c_str());
			CString szClientIpRange = CCommonFun::UTF8ToUnicode(range.pFileStorage->client_ip_range().c_str());
			theLog.Write(_T("CFileStorageHelper::GetBestFileStorageList,1,ip=%s,client_ip_range=[%s]")
				, szIP, szClientIpRange);
		}

		if (nMaxCount <= 0)
		{
			break;
		}
	}
}

void CFileStorageHelper::AddFileStorage(FileStorage* pFileStorage)
{
	if (!pFileStorage)
	{
		theLog.Write(_T("!!CFileStorageHelper::AddFileStorage,pFileStorage=%p"), pFileStorage);
		return;
	}

	//theLog.Write(_T("CFileStorageHelper::AddFileStorage,1,ip=%s,client_ip_range=[%s]")
	//	,CCommonFun::UTF8ToUnicode( pFileStorage->ip().c_str()), CCommonFun::UTF8ToUnicode(pFileStorage->client_ip_range().c_str()));

	CCriticalSection2::Owner ow(m_cs4FileStorage);

	int nFileStorageId = pFileStorage->id();
	CString szClientIpRange = CCommonFun::UTF8ToUnicode(pFileStorage->client_ip_range().c_str());
	szClientIpRange.Trim();
	CStringArray ary;
	CCommonFun::StringSplit(szClientIpRange, &ary, _T("|"), TRUE);
	int nClientIpRangeCount = ary.GetCount();
	if (nClientIpRangeCount <= 0)
	{
		FileStorageIPRange range;
		range.dwIPBegin = 0;
		range.dwIPEnd = 0xFFFFFFFF;
		range.pFileStorage = pFileStorage;
		//theLog.Write(_T("CFileStorageHelper::AddFileStorage,2,dwIPBegin=%u,dwIPEnd=%u"), range.dwIPBegin, range.dwIPEnd);
		m_oFileStorageIPRangeList.AddTail(range);
	}
	else
	{
		for (int i=0; i<nClientIpRangeCount; i++)
		{
			const CString& szIpRange = ary.GetAt(i);
			int nPos = szIpRange.Find(_T(","));
			FileStorageIPRange range;
			range.dwIPBegin = CCommonFun::ConverIPToDWORD(szIpRange.Left(nPos));
			range.dwIPEnd = CCommonFun::ConverIPToDWORD(szIpRange.Mid(nPos+1));
			//theLog.Write(_T("CFileStorageHelper::AddFileStorage,3,dwIPBegin=%u,dwIPEnd=%u,szIPBegin=%s,szIPEnd=%s")
			//	, range.dwIPBegin, range.dwIPEnd, szIpRange.Left(nPos), szIpRange.Mid(nPos+1));
			range.pFileStorage = pFileStorage;
			m_oFileStorageIPRangeList.AddTail(range);
		}
	}
}

void CFileStorageHelper::ReleaseFileStorageList(CFileStorageList& list)
{
	int nListCount = list.GetCount();
	POSITION pos = list.GetHeadPosition();
	for (int index=0; index<nListCount; index++)
	{
		FileStorage* pFileStorage = list.GetNext(pos);
		if (pFileStorage)
		{
			delete pFileStorage;
			pFileStorage = NULL;
		}
	}

	list.RemoveAll();
}
