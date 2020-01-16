#include "StdAfx.h"
#include "PsPclJobChange.h"

CPsPclJobChange::CPsPclJobChange(void)
{
}

CPsPclJobChange::~CPsPclJobChange(void)
{
}

BOOL CPsPclJobChange::ChangeJob(CString szSplPath, DWORD dwColor, DWORD dwDuplex, SplType nSplType, CString szDocName)
{
	theLog.Write(_T("CPsPclJobChange::ChangeJob,1,nSplType=%d,szSplPath=%s"), nSplType, szSplPath);

	BOOL bSucc = FALSE;

	if (szSplPath.IsEmpty() || !PathFileExists(szSplPath))
	{
		theLog.Write(_T("!!CPsPclJobChange::ChangeJob,2,szSplPath=%s"), szSplPath);
		return bSucc;
	}

	switch (nSplType)
	{
	case ST_PCL:
	case ST_PCL_5:
	case ST_PCL_6:
		{
			bSucc = ChangeJob_PCL(szSplPath, dwColor, dwDuplex, szDocName);
			if (!bSucc)
			{
				theLog.Write(_T("!!CPsPclJobChange::ChangeJob,3,ChangeJob_PCL fail"));
			}
		}
		break;
	case ST_PS:
	case ST_PS_2:
	case ST_PS_3:
		{
			bSucc = ChangeJob_PS(szSplPath, dwColor, dwDuplex, szDocName);
			if (!bSucc)
			{
				theLog.Write(_T("!!CPsPclJobChange::ChangeJob,ChangeJob_PS fail"));
			}
		}
		break;
	default:
		bSucc = FALSE;
		theLog.Write(_T("!!CPsPclJobChange::ChangeJob,100,fail,��֧�ֵĸ�ʽ����,nSplType=%d"), nSplType);
		break;
	}

	return bSucc;
}

BOOL CPsPclJobChange::ChangeJob_PS(CString szSplPath, DWORD dwColor, DWORD dwDuplex, CString szDocName)
{
	BOOL bSucc = FALSE;
	FILE *pFile = NULL;
	BYTE *pBuf = NULL;
	BYTE *pTmpBuf = NULL;

	do
	{
		BOOL bNeedChange = FALSE;
		DWORD dwNeedChangeEndPos = 0;
		CStringA szNeedChangeStr = "";
		CStdioFile oFile;
		if (oFile.Open(szSplPath, CFile::modeRead))
		{
			theLog.Write(_T("CPsPclJobChange::ChangeJob_PS,1,oFile FileLen=%d"), oFile.GetLength());
			DWORD dwReadLineCount = 0;	//����ȡ������Ҫ����10000
			CStringA szOneLine;
			CString szRead;
			while (oFile.ReadString(szRead) && dwReadLineCount < 10000)
			{
				dwReadLineCount++;
#ifdef UNICODE
				szOneLine = CCommonFun::UnicodeToMultiByte(szRead);
#else
				szOneLine = szRead;
#endif
				if (szOneLine.CompareNoCase("%%EndSetup") != 0)
				{
					szNeedChangeStr += szOneLine + "\n";
				}
				else
				{
					szNeedChangeStr += szOneLine;
					theLog.Write(_T("CPsPclJobChange::ChangeJob_PS,2"));
					bNeedChange = TRUE;
					break;
				}
			}

			dwNeedChangeEndPos = oFile.GetPosition();
			oFile.Close();
		}
		else
		{
			theLog.Write(_T("CPsPclJobChange::ChangeJob_PS,3,oFile.Open fail,error=%d"), GetLastError());
			break;
		}

		if (!bNeedChange || dwNeedChangeEndPos <= 0)
		{
			theLog.Write(_T("##CPsPclJobChange::ChangeJob_PS,4,bNeedChange=%d,dwNeedChangeEndPos=%d")
				, bNeedChange, dwNeedChangeEndPos);
			break;
		}

		pFile = ::_tfopen(szSplPath.GetString(), _T("rb"));
		if (!pFile)
		{
			theLog.Write(_T("##CPsPclJobChange::ChangeJob_PS,5,pFile=%p"), pFile);
			break;
		}

		DWORD dwTmpBufLen = dwNeedChangeEndPos + 1;
		pTmpBuf = new BYTE[dwTmpBufLen];
		memset(pTmpBuf, 0x0, dwTmpBufLen);
		if (!ReadToBuf(pFile, pTmpBuf, dwNeedChangeEndPos))
		{
			theLog.Write(_T("##CPsPclJobChange::ChangeJob_PS,6,ReadToBuf fail"));
			break;
		}

		char *pHasRead = (char*)pTmpBuf;
		char* cEndSetUp = "%%EndSetup";
		char* pIndex = strstr(pHasRead, cEndSetUp);
		DWORD dwNeedChangeEndPos2 = pIndex - pHasRead + strlen(cEndSetUp);

		theLog.Write(_T("CPsPclJobChange::ChangJob_PS,7,dwNeedChangeEndPos=%d,dwNeedChangeEndPos2=%d")
			, dwNeedChangeEndPos, dwNeedChangeEndPos2);

		BOOL bChangeToHB = FALSE;
		if (DMCOLOR_MONOCHROME == dwColor)
		{
			bChangeToHB = ChangeJobStrToHB_PS(szNeedChangeStr);
		}

		BOOL bChangeToDuplexV = FALSE;
		BOOL bChangeToDuplexH = FALSE;
		if (DMDUP_VERTICAL == dwDuplex)
		{
			bChangeToDuplexV = ChangeJobStrToDuplexV_PS(szNeedChangeStr);
		}
		else if (DMDUP_HORIZONTAL == dwDuplex)
		{
			bChangeToDuplexH = ChangeJobStrToDuplexH_PS(szNeedChangeStr);
		}

		BOOL bChangeJobDocName = ChangeJobDocName_PS(szNeedChangeStr, szDocName);

		//����ļ�����û���κ��޸ģ���Ҫ�������ļ�
		if (!bChangeToHB && !bChangeToDuplexV && !bChangeToDuplexH && !bChangeJobDocName)
		{
			theLog.Write(_T("##CPsPclJobChange::ChangJob_PS,8,Not Need Change Job"));
			break;
		}

		//�������ļ�
		CString szNewSplPath;
		szNewSplPath.Format(_T("%s_NewSpl.spl"), szSplPath);
		CFile oNewFile;
		if (!oNewFile.Open(szNewSplPath, CFile::modeCreate | CFile::modeWrite))
		{
			theLog.Write(_T("##CPsPclJobChange::ChangeJob_PS,9,oNewFile.Open fail,szNewSplPath=%s,error=%u")
				, szNewSplPath, GetLastError());
			break;
		}

		//���޸ĺ��ǰ�벿��д�뵽���ļ�
		oNewFile.Write(szNeedChangeStr.GetString(), szNeedChangeStr.GetLength());

		//����벿���ļ�д�뵽���ļ�
		fseek(pFile, dwNeedChangeEndPos2, SEEK_SET);

		DWORD dwBufLen = 1024 * 1024 * 10;//��벿�֣�����10M��С�ֶ�д�����ļ�
		pBuf = new BYTE[dwBufLen];
		while (TRUE)
		{
			DWORD dwRead = ReadToBuf(pFile, pBuf, dwBufLen);
			if (dwRead > 0)
			{
				oNewFile.Write(pBuf, dwRead);
			}
			else
			{
				break;
			}
		}

		oNewFile.Close();

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}

		//�����ļ�����ԭ�����ļ�
		if (!MoveFileEx(szNewSplPath, szSplPath, MOVEFILE_REPLACE_EXISTING))
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangJobToHB_PS,10,MoveFileEx fail,error=%u"), GetLastError());
			break;
		}

		theLog.Write(_T("CPsPclJobChange::ChangJobToHB_PS,11,succ"));

		bSucc = TRUE;

	} while (FALSE);

	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	if (pBuf)
	{
		delete[] pBuf;
		pBuf = NULL;
	}

	if (pTmpBuf)
	{
		delete[] pTmpBuf;
		pTmpBuf = NULL;
	}

	return bSucc;
}

BOOL CPsPclJobChange::ChangeJob_PCL(CString szSplPath, DWORD dwColor, DWORD dwDuplex, CString szDocName)
{
	BOOL bSucc = FALSE;
	FILE *pSourceFile = NULL;
	FILE *pDestFile = NULL;
	BYTE *pBuffer = NULL;
	BYTE *pTmpBuffer = NULL;

	do
	{
		ParsePCL::CParsePCL pcl;
		if (!pcl.OpenFile(szSplPath.GetString()))
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,1,pcl.OpenFile fail"));
			break;
		}

		if (!pcl.Parse())
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,2,pcl.Parse fail"));
			break;
		}

		//��ȡԭʼSPL����Ϣ
		int nColor = dwColor;
		pcl.GetColor(nColor);
		int nDuplex = dwDuplex;
		pcl.GetDuplex(nDuplex);
		__int64 nFileLength = pcl.GetFileLength();
		__int64 nPclPostion = pcl.GetPclPostion();
		std::list<ParsePCL::Object> oPageDuplexObjectList;
		pcl.GetPageDuplexInfo(oPageDuplexObjectList);

		//�ر�ԭʼSPL�ļ�
		pcl.CloseFile();

		DWORD dwPclPostion = nPclPostion;
		if (dwPclPostion <= 0)
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,4,dwPclPostion=%u"), dwPclPostion);
			break;
		}

		theLog.Write(_T("##CPsPclJobChange::ChangeJob_PCL,5,dwPclPostion=%u,duplexcount=%u"), dwPclPostion, oPageDuplexObjectList.size());

		//���´�ԭʼSPL�ļ�
		errno_t nError = _tfopen_s(&pSourceFile, szSplPath.GetString(), _T("rb"));
		if (!pSourceFile)
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,6,pSourceFile=%p,nError=%d"), pSourceFile, nError);
			break;
		}

		DWORD dwTmpBufferLen = dwPclPostion + 1;
		pTmpBuffer = new BYTE[dwTmpBufferLen];
		memset(pTmpBuffer, 0x0, dwTmpBufferLen);
		if (!ReadToBuf(pSourceFile, pTmpBuffer, dwPclPostion))
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,7,ReadToBuf fail"));
			break;
		}

		CStringA szNeedChangeStr = (char*)pTmpBuffer;
		if ((DMCOLOR_MONOCHROME == dwColor) && (DMCOLOR_COLOR == nColor))
		{
			ChangeJobStrToHB_PCL(szNeedChangeStr);
		}

		//��ʶ�Ƿ���Ҫ��PCL���������˫���ӡ������Ϣ
		BOOL bNeedClearPclDuplexInfo = FALSE;
		if ((DMDUP_VERTICAL == dwDuplex) && (DMDUP_SIMPLEX == nDuplex))
		{
			bNeedClearPclDuplexInfo = TRUE;
			ChangeJobStrToDuplexV_PCL(szNeedChangeStr);
		}
		else if ((DMDUP_HORIZONTAL == dwDuplex) && (DMDUP_SIMPLEX == nDuplex))
		{
			bNeedClearPclDuplexInfo = TRUE;
			ChangeJobStrToDuplexH_PCL(szNeedChangeStr);
		}

		ChangeJobDocName_PCL(szNeedChangeStr, szDocName);

		//�������ļ�
		CString szNewSplPath;
		szNewSplPath.Format(_T("%s_NewSpl.spl"), szSplPath);
		nError = _tfopen_s(&pDestFile, szNewSplPath.GetString(), _T("w+b"));
		if (!pDestFile)
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,8,pDestFile=%p,nError=%d"), pDestFile, nError);
			break;
		}

		//���޸ĺ��PJLд�뵽���ļ�
		fwrite(szNeedChangeStr.GetString(), 1, szNeedChangeStr.GetLength(), pDestFile);

		//��PCL���ݰ������1M��С�ֶ�д�����ļ�
		DWORD dwBufferLen = 1024 * 1024 * 1;
		pBuffer = new BYTE[dwBufferLen];

		__int64 nSourceFrom = dwPclPostion;
		__int64 nSourceTo = 0;
		while (bNeedClearPclDuplexInfo && !oPageDuplexObjectList.empty())
		{
			//˫���ӡ����Ϊ�ĸ��ֽڣ�1-���ݱ�ǩ�����ͣ���2-˫�������3-���Ա�ʶ��4-����ID
			//BYTE byte[4] = {PCL_UByteData, (PCL_eSimplexFrontSide|PCL_eDuplexHorizontalBinding|PCL_eDuplexVerticalBinding), PCL_AttributeIdByte, (PCL_SimplexPageMode|PCL_DuplexPageMode)}
			ParsePCL::Object obj = oPageDuplexObjectList.front();
			__int64 nFilePos = 0;
			obj.GetFileInfo(NULL, nFilePos);
			nSourceTo = (nFilePos - 2);

			WriteFile(pSourceFile, nSourceFrom, nSourceTo, pDestFile, pBuffer, dwBufferLen);
			nSourceFrom = (nSourceTo + 4);

			oPageDuplexObjectList.pop_front();
		}
		nSourceTo = nFileLength;

		WriteFile(pSourceFile, nSourceFrom, nSourceTo, pDestFile, pBuffer, dwBufferLen);

		//�ر�ԭ�ļ�
		if (pSourceFile)
		{
			fclose(pSourceFile);
			pSourceFile = NULL;
		}

		//�ر����ļ�
		if (pDestFile)
		{
			fclose(pDestFile);
			pDestFile = NULL;
		}

		//�����ļ�����ԭ�����ļ�
		if (!MoveFileEx(szNewSplPath, szSplPath, MOVEFILE_REPLACE_EXISTING))
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJob_PCL,10,MoveFileEx fail,error=%u"), GetLastError());
			break;
		}

		theLog.Write(_T("CPsPclJobChange::ChangeJob_PCL,11,succ"));

		bSucc = TRUE;

	} while (FALSE);

	if (pSourceFile)
	{
		fclose(pSourceFile);
		pSourceFile = NULL;
	}

	if (pDestFile)
	{
		fclose(pDestFile);
		pDestFile = NULL;
	}

	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = NULL;
	}

	if (pTmpBuffer)
	{
		delete[] pTmpBuffer;
		pTmpBuffer = NULL;
	}

	return bSucc;
}

//���нӿڵķ���ֵ����ʾ�����Ƿ����޸�
BOOL CPsPclJobChange::ChangeJobStrToHB_PS(CStringA& szSource)
{
	return FALSE;
}

BOOL CPsPclJobChange::ChangeJobStrToColor_PS(CStringA& szSource)
{
	return FALSE;
}

BOOL CPsPclJobChange::ChangeJobStrToDuplexV_PS(CStringA& szSource)
{
	return FALSE;
}

BOOL CPsPclJobChange::ChangeJobStrToDuplexH_PS(CStringA& szSource)
{
	return FALSE;
}

BOOL CPsPclJobChange::ChangeJobDocName_PS(CStringA& szSource, CString szDocName)
{
	return ChangeJobDocName(szSource, szDocName);
}

BOOL CPsPclJobChange::ChangeJobStrToHB_PCL(CStringA& szSource)
{
	return FALSE;
}

BOOL CPsPclJobChange::ChangeJobStrToColor_PCL(CStringA& szSource)
{
	return FALSE;
}

//һ��PJL�Ļ��������ָ�ʽ����\r\n���͡�\n����
BOOL CPsPclJobChange::ChangeJobStrToDuplexV_PCL(CStringA& szSource)
{
	//���Ҳ�ɾ��˫���ӡ���ز���
	int nDuplexBeginPos = szSource.Find("@PJL SET DUPLEX");
	if (nDuplexBeginPos > 0)
	{
		int nDuplexEndPos = szSource.Find("\n", nDuplexBeginPos);
		CStringA szDuplex = szSource.Mid(nDuplexBeginPos, nDuplexEndPos - nDuplexBeginPos + 1);
		szSource.Replace(szDuplex, "");
	}

	//���Ҳ�ɾ��˫���ӡ���߶̱߲���
	int nBindingBeginPos = szSource.Find("@PJL SET BINDING");
	if (nBindingBeginPos > 0)
	{
		int nBindingEndPos = szSource.Find("\n", nBindingBeginPos);
		CStringA szBinding = szSource.Mid(nBindingBeginPos, nBindingEndPos - nBindingBeginPos + 1);
		szSource.Replace(szBinding, "");
	}

	//@PJL ENTER LANGUAGE=PCLXL
	//@PJL ENTER LANGUAGE = PCLXL
	int nPos = szSource.Find("@PJL ENTER LANGUAGE");
	if (nPos >= 0)
	{
		//׷��˫�泤�ߴ�ӡ����
		szSource.Insert(nPos, "@PJL SET DUPLEX = ON\n");
		szSource.Insert(nPos, "@PJL SET BINDING = LONGEDGE\n");
	}
	else
	{
		theLog.Write(_T("!!CPsPclJobChange::ChangeJobStrToDuplexV_PCL,fail"));
	}
	return TRUE;
}

BOOL CPsPclJobChange::ChangeJobStrToDuplexH_PCL(CStringA& szSource)
{
	//���Ҳ�ɾ��˫���ӡ���ز���
	int nDuplexBeginPos = szSource.Find("@PJL SET DUPLEX");
	if (nDuplexBeginPos > 0)
	{
		int nDuplexEndPos = szSource.Find("\n", nDuplexBeginPos);
		CStringA szDuplex = szSource.Mid(nDuplexBeginPos, nDuplexEndPos - nDuplexBeginPos + 1);
		szSource.Replace(szDuplex, "");
	}

	//���Ҳ�ɾ��˫���ӡ���߶̱߲���
	int nBindingBeginPos = szSource.Find("@PJL SET BINDING");
	if (nBindingBeginPos > 0)
	{
		int nBindingEndPos = szSource.Find("\n", nBindingBeginPos);
		CStringA szBinding = szSource.Mid(nBindingBeginPos, nBindingEndPos - nBindingBeginPos + 1);
		szSource.Replace(szBinding, "");
	}

	//@PJL ENTER LANGUAGE=PCLXL
	//@PJL ENTER LANGUAGE = PCLXL
	int nPos = szSource.Find("@PJL ENTER LANGUAGE");
	if (nPos >= 0)
	{
		//׷��˫��̱ߴ�ӡ����
		szSource.Insert(nPos, "@PJL SET DUPLEX = ON\n");
		szSource.Insert(nPos, "@PJL SET BINDING = SHORTEDGE\n");
	}
	else
	{
		theLog.Write(_T("!!CPsPclJobChange::ChangeJobStrToDuplexH_PCL,fail"));
	}
	return TRUE;
}

BOOL CPsPclJobChange::ChangeJobDocName_PCL(CStringA& szSource, CString szDocName)
{
	return ChangeJobDocName(szSource, szDocName);
}

BOOL CPsPclJobChange::ChangeJobDocName(CStringA& szSource, CString szDocName)
{
	if (szDocName.GetLength() <= 0)
	{
		theLog.Write(_T("##CPsPclJobChange::ChangeJobDocName,�ĵ���Ϊ��,���޸�"));
		return FALSE;
	}

	//��ʶ�Ƿ��Ѿ��޸�
	BOOL bChange = FALSE;

	//UTF8������ĵ���
	CStringA szDocNameA = CCommonFun::UnicodeToUTF8(szDocName);

	//���Ҳ��滻�ĵ�����ʽһ
	int nJobNameBeginPos = szSource.Find("@PJL JOB NAME");
	if (nJobNameBeginPos > 0)
	{
		int nJobNameEndPos = szSource.Find("\n", nJobNameBeginPos);
		CStringA szJobName = szSource.Mid(nJobNameBeginPos, nJobNameEndPos - nJobNameBeginPos + 1);
		CStringA szJobNameNew;
		szJobNameNew.Format("@PJL JOB NAME = \"%s\"\n", szDocNameA);
		szSource.Replace(szJobName, szJobNameNew);
		bChange = TRUE;
	}

	//���Ҳ��滻�ĵ�����ʽ��
	nJobNameBeginPos = szSource.Find("@PJL SET JOBNAME");
	if (nJobNameBeginPos > 0)
	{
		int nJobNameEndPos = szSource.Find("\n", nJobNameBeginPos);
		CStringA szJobName = szSource.Mid(nJobNameBeginPos, nJobNameEndPos - nJobNameBeginPos + 1);
		CStringA szJobNameNew;
		szJobNameNew.Format("@PJL SET JOBNAME = \"%s\"\n", szDocNameA);
		szSource.Replace(szJobName, szJobNameNew);
		bChange = TRUE;
	}

	//���û���޸ģ����ֶ�����ĵ���
	if (!bChange)
	{
		//@PJL ENTER LANGUAGE=PCLXL
		//@PJL ENTER LANGUAGE = PCLXL
		int nPos = szSource.Find("@PJL ENTER LANGUAGE");
		if (nPos >= 0)
		{
			//׷���ĵ�������
			CStringA szJobNameNew;
			szJobNameNew.Format("@PJL SET JOBNAME = \"%s\"\n", szDocNameA);
			szSource.Insert(nPos, szJobNameNew);
			bChange = TRUE;
		}
		else
		{
			theLog.Write(_T("!!CPsPclJobChange::ChangeJobDocName,fail"));
		}
	}

	return bChange;
}

DWORD CPsPclJobChange::ReadToBuf(FILE* pFile, BYTE* pBuf, DWORD dwLen)
{
	if (!pFile || !pBuf || (dwLen<=0))
	{
		theLog.Write(_T("!!CPsPclJobChange::ReadToBuf,pFile=0x%x,pBuf=0x%x,dwLen=%u"), pFile, pBuf, dwLen);
		return 0;
	}

	DWORD dwHasRdLen = 0;
	DWORD dwNeedRdLen = 0;
	DWORD dwReadLen = 0;
	DWORD dwValidBufLen = dwLen;
	//theLog.Write(_T("CPsPclJobChange::ReadToBuf,1,dwValidBufLen=%d"),dwValidBufLen);
	while(dwHasRdLen < dwValidBufLen)
	{
		dwNeedRdLen = dwValidBufLen - dwHasRdLen;
		dwReadLen = fread(pBuf+dwHasRdLen, 1, dwNeedRdLen, pFile);
		dwHasRdLen += dwReadLen;
		if(0 >= dwReadLen)
		{
			break;
		}
		else if(dwReadLen < dwNeedRdLen)
		{
			//�ļ�û�н�β����Ҫ�������ȡ�ļ���
			if (!feof(pFile))
			{
				theLog.Write(_T("##CPsPclJobChange::ReadToBuf,2.1,dwReadLen=%d,dwNeedRdLen=%d"), dwReadLen, dwNeedRdLen);
			}
			else
			{
				break;
			}
		}
		else if(dwReadLen == dwNeedRdLen)
		{
			break;
		}
	}//while
	return dwHasRdLen;
}

BOOL CPsPclJobChange::WriteFile(FILE* pSourceFile, __int64 nSourceFrom, __int64 nSourceTo, FILE* pDestFile, void* pBuffer, size_t nBufferLength)
{
	BOOL bSucc = FALSE;
	if (pSourceFile && pDestFile && (nSourceTo > nSourceFrom) && (nSourceFrom >= 0) && pBuffer && (nBufferLength > 0))
	{
		_fseeki64(pSourceFile, nSourceFrom, SEEK_SET);
		size_t nLength = nSourceTo - nSourceFrom;
		while (nLength > 0)
		{
			if (nLength <= nBufferLength)
			{
				nBufferLength = nLength;
			}
			size_t nRead = fread(pBuffer, 1, nBufferLength, pSourceFile);
			if (nRead <= 0)
			{
				if (!feof(pSourceFile))
				{
					theLog.Write(_T("##CPsPclJobChange::WriteFile,fread fail,pBuffer=%p,nBufferLength=%u,pSourceFile=%p,errno=%d")
						, pBuffer, nBufferLength, pSourceFile, errno);
				}
				break;
			}

			size_t nWrite = fwrite(pBuffer, 1, nRead, pDestFile);
			if (nWrite != nRead)
			{
				theLog.Write(_T("##CPsPclJobChange::WriteFile,fwrite fail,pBuffer=%p,nBufferLength=%u,nRead=%u,nWrite=%u,pDestFile=%p,errno=%d")
					, pBuffer, nBufferLength, nRead, nWrite, pDestFile, errno);
				break;
			}
			nLength -= nRead;
		}
		bSucc = (nLength == 0) ? TRUE : FALSE;
	}

	return bSucc;
}