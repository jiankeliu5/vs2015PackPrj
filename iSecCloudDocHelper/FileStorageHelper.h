#pragma once

#define FILE_STORAGE_MAX_TRY_COUNT	1024	//�ļ��洢����Ը���

struct FileStorageIPRange
{
	DWORD dwIPBegin;
	DWORD dwIPEnd;
	FileStorage* pFileStorage;	//�ļ��洢ָ��
};

typedef CList<FileStorageIPRange, FileStorageIPRange> CFileStorageIPRangeList;
typedef CList<FileStorage*, FileStorage*> CFileStorageList;

class CFileStorageHelper
{
protected:
	CFileStorageHelper();
public:
	static CFileStorageHelper& GetInstance();
	void InitFileStorageList(FileStorageList& list);
	void GetBestFileStorageList(CString szClientIP, CFileStorageList& list);
	void ReleaseFileStorageList(CFileStorageList& list);

private:
	void AddFileStorage(FileStorage* pFileStorage);
	FileStorageList m_oFileStorageList;
	CFileStorageIPRangeList m_oFileStorageIPRangeList;
	CCriticalSection2 m_cs4FileStorage;
};
