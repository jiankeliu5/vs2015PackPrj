// PackMacPrinterClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "PackMacPrinterClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFileLog theLog;

// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		theLog.SetLogName(_T("PackMacPrinterClient"));
		theLog.EnableLog(CCommonFun::IsDebug());

		theLog.Write(_T("PackMacPrinterClient,begin"));
		
		//mac�ͻ��˴��
		CString szPackPath;
		szPackPath.Format(_T("%sPrinterClient.zip"), CCommonFun::GetDefaultPath());
		CString szOrgFilePath = szPackPath.Left(szPackPath.GetLength() - 4);
#if 0
		if (CFileHelper::UnCompressZipFile(szPackPath, szOrgFilePath, FALSE))
		{
			//���ô�ӡ����

			CIniFile file;
			file.m_sPath.Format(_T("%sconfig.ini"), CCommonFun::GetDefaultPath());
			CString szPrinterName = file.GetVal(_T("ClientConfig"), _T("PrinterName"), _T("XabVPrinter"));
			CString sWebDomainName = file.GetVal(_T("ClientConfig"),_T("urldomainname"),_T("")); //��ӡ��ѯ�ķ���������
			CString sWebAppName = file.GetVal(_T("ClientConfig"),_T("WebAppName"),_T("XabPrint")); //��ӡ��ѯ�ķ�����Ӧ�ó�����
			CString sWebIP = file.GetVal(_T("ClientConfig"), _T("SvrIP"), CCommonFun::GetLocalIP());
			BOOL bHttps = file.GetVal(_T("ClientConfig"), _T("Https"), FALSE);				 //��ӡ��ѯ�ķ������Ƿ�����Https
			int nWebPort = file.GetVal(_T("ClientConfig"), _T("urlport"), (bHttps ? 443 : 80));

			CString szUrl;	//http://192.168.2.15:80/XabPrint/WebService/iSecPrint.aspx
			if(sWebDomainName.GetLength()>0)
			{
				szUrl.Format(_T("%s://%s:%d"), (bHttps ? _T("https") : _T("http")), sWebDomainName, nWebPort);
			}
			else
			{
				szUrl.Format(_T("%s://%s:%d"), (bHttps ? _T("https") : _T("http")), sWebIP, nWebPort);
			}

			if (sWebAppName.GetLength()>0)
			{
				szUrl = szUrl + _T("/") + sWebAppName;
			}
			else
			{
				szUrl = szUrl;
			}

			CStringArray ary;
			CString szPrinterNameSet;szPrinterNameSet.Format(_T("PrinterName=%s\n"), szPrinterName);
			CString szHostNameSet;szHostNameSet.Format(_T("HostName=%s/WebService/iSecPrint.aspx\n"), szUrl);
			CString szCloudPrintSet;szCloudPrintSet.Format(_T("CloudPrint=0\n"));	//0-��ʾ��ҵ��ӡ

			CString szSetupPath;
			szSetupPath.Format(_T("%s\\PrinterClient.pkg\\Contents\\Resources\\setup.ini"), szOrgFilePath);
			FILE* pFile = _tfopen(szSetupPath.GetString(), _T("rt"));
			if (pFile != NULL)
			{
				int nMaxCount = 1024;
				char* pReadBuf = new char[nMaxCount];
				CString szRead;
				while (fgets(pReadBuf, nMaxCount, pFile))
				{
					//�ļ�������utf8�����
					szRead = CCommonFun::UTF8ToUnicode(pReadBuf);
					if (szRead.Find(_T("PrinterName=")) >= 0)
					{
						ary.Add(szPrinterNameSet);
					}
					else if (szRead.Find(_T("HostName=")) >= 0)
					{
						ary.Add(szHostNameSet);
					}
					else if (szRead.Find(_T("CloudPrint=")) >= 0)
					{
						ary.Add(szCloudPrintSet);
					}
					else
					{
						ary.Add(szRead);
					}
				}

				//�ر��ļ�
				fclose(pFile);
				delete[] pReadBuf;
			}
			else
			{
				theLog.Write(_T("!!,2,Open fail,err=%u, szSetupPath=%s"), GetLastError(), szSetupPath);
			}

			//���´��ļ�,�Զ����Ʒ�ʽд�룬�����з�������MACϵͳ������
			CFile oFile2;
			if (oFile2.Open(szSetupPath, CFile::modeWrite))
			{
				//��д�������ļ�
				for (int i=0; i<ary.GetCount(); i++)
				{
					CStringA szLine = CCommonFun::UnicodeToUTF8(ary.GetAt(i));
					oFile2.Write(szLine.GetString(), szLine.GetLength());
				}

				//�ر��ļ�
				oFile2.Close();
			}
			else
			{
				theLog.Write(_T("!!,3,Open fail,err=%u, szSetupPath=%s"), GetLastError(), szSetupPath);
			}

			CString szZipFilePath;
			szZipFilePath.Format(_T("%sMacPrinterClient.zip"), CCommonFun::GetDefaultPath());
			CString szPrinterClientPkg;
			szPrinterClientPkg.Format(_T("%s\\PrinterClient.pkg"), szOrgFilePath);
			if (!PathFileExists(szPrinterClientPkg))
			{
				szPrinterClientPkg = szOrgFilePath;
			}
			if (CFileHelper::CompressZipFile(szPrinterClientPkg, szZipFilePath, FALSE))
			{
				//C:\WorkSpace\iSecPrint5.5-20171226\iSecStar\WebServer\bin\Client_Setup\MacPrinterClient.zip
				//C:\WorkSpace\iSecPrint5.5-20171226\iSecStar\WebServer\down\MacPrinterClient.zip

				CString szDownZipFilePath = szZipFilePath;
				szDownZipFilePath.Replace(_T("bin\\Client_Setup"), _T("down"));
				if (!MoveFileEx(szZipFilePath, szDownZipFilePath, MOVEFILE_REPLACE_EXISTING))
				{
					theLog.Write(_T("!!,4,MoveFileEx fail,error=%u,szOrgFilePath=%s,szZipFilePath=%s")
						, GetLastError(), szOrgFilePath, szZipFilePath);
				}
			}
			else
			{
				theLog.Write(_T("!!,5,CompressZipFile fail,szOrgFilePath=%s,szZipFilePath=%s"), szOrgFilePath, szZipFilePath);
			}

			CCommonFun::DeleteDir(szOrgFilePath);
		}
		else
		{
			theLog.Write(_T("!!,6,UnCompressZipFile fail,szPackPath=%s,szOrgFilePath=%s"), szPackPath, szOrgFilePath);
		}
#endif
	}

	theLog.Write(_T("PackMacPrinterClient,end"));

	return nRetCode;
}
