#include "stdafx.h"
#include "PrintFileTest.h"



PrintFileTest::PrintFileTest(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
}

PrintFileTest::~PrintFileTest()
{
	GdiplusShutdown(m_gdiplusToken);
}

void PrintFileTest::EnumeratePrinters(CStringArray& ary)
{
	DWORD dwNeeded = 0,
		dwReturned = 0;
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
	if (dwNeeded <= 0)
	{
		return;
	}

	LPBYTE lpBuffer = new BYTE[dwNeeded];
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, lpBuffer, dwNeeded, &dwNeeded, &dwReturned);

	PPRINTER_INFO_2 p2 = (PPRINTER_INFO_2)lpBuffer;

	for (DWORD x = 0; x < dwReturned; x++)
	{
		ary.Add(p2->pPrinterName);
		p2++;
	}

	delete[] lpBuffer;
}

CString PrintFileTest::GetPrinterPort(CString szPrnterName)
{
	CString szPort;
	HANDLE h = 0;
	PRINTER_DEFAULTS pd;
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	TCHAR cPrinter[MAX_PATH] = { 0 };
	_tcscpy_s(cPrinter, szPrnterName.GetString());

	if (OpenPrinter(cPrinter, &h, &pd) && h)
	{
		DWORD dwSize = 0;
		GetPrinter(h, 2, 0, 0, &dwSize);
		//theLog.Write(_T("GetPrinterStatus %d"),dwSize);
		if (dwSize > 0)
		{
			BYTE* pData = new BYTE[dwSize];
			if (GetPrinter(h, 2, pData, dwSize, &dwSize))
			{
				PRINTER_STATUS_BUSY;
				PRINTER_INFO_2* pInfo = (PRINTER_INFO_2*)pData;
				szPort = pInfo->pPortName;
			}
			else
			{
				theLog.Write(_T("!!GetPrinterPort,GetPrinter,3, err %d"), ::GetLastError());
			}
			delete[] pData;
		}
		else
		{
			theLog.Write(_T("!!GetPrinterPort,GetPrinter,2, err %d"), ::GetLastError());
		}
		ClosePrinter(h);
	}
	else
	{
		theLog.Write(_T("!!GetPrinterPort,OpenPrinter,1, err %d"), ::GetLastError());
	}

	return szPort;
}

void PrintFileTest::showEvent(QShowEvent *event)
{
	CStringArray szAry;
	EnumeratePrinters(szAry);
	for (int i = 0; i < szAry.GetCount();i++)
	{
		comboBox->addItem(QString::fromStdWString(szAry.GetAt(i).GetString()));
	}
}

void PrintFileTest::on_pushButton_browser_clicked()
{
	QString qsPath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Files(*.pdf *.spl *.tiff *.png *.jpg *.jpeg *.emf)"));
	lineEdit->setText(qsPath);
	CString szFilePath = qsPath.toStdWString().c_str();
}

void PrintFileTest::on_pushButton_print_clicked()
{
	CIOPrinter helper;
	CString szFilePath = lineEdit->text().toStdWString().c_str();
	CString szExt = CCommonFun::GetFileExt3(szFilePath);
	int nPagesCount = 0;

	helper.SetSelPrinter(comboBox->currentText().toStdWString().c_str());
	helper.EnableColor(TRUE);
	helper.SetDuplex(TRUE);
	BOOL bIsEmf = FALSE;
	BOOL bIsSpl = FALSE;
	BOOL bIsPdf = FALSE;
	BOOL bImage = FALSE;
// 	int a = (szExt.CompareNoCase(_T(".tiff")) == 0 || szExt.CompareNoCase(_T(".png")) == 0);
 	theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,szExt=%s"), szExt);
	BOOL bPrintSucc = FALSE;
	int nFrom = 0;
	int nTo = 0;
	if (szExt.CompareNoCase(_T(".pdf")) == 0)
	{
		CPdfHelper oHelper;
		if (oHelper.OpenFile(CCommonFun::UnicodeToMultiByte(szFilePath).GetString()))
		{
			nTo = oHelper.GetPageCount() - 1;
			oHelper.CloseFile();
			bIsPdf = TRUE;
		}
	}
	else if (szExt.CompareNoCase(_T(".tiff")) == 0 || szExt.CompareNoCase(_T(".png")) == 0 || szExt.CompareNoCase(_T(".emf")) == 0)
	{
		Image image(szFilePath);
		nPagesCount = image.GetFrameDimensionsCount();
		theLog.Write(_T("image,nPagesCount=%d"), nPagesCount);
		nTo = nPagesCount - 1;
		bImage = TRUE;
	}
	else if (szExt.CompareNoCase(_T(".spl")) == 0 || szExt.CompareNoCase(_T(".ps")) == 0 || szExt.CompareNoCase(_T(".pcl")) == 0)
	{
		bIsSpl = TRUE;
		CParseSpl ps;
		if (ps.InitParse(szFilePath)) {
			int nPageCountPerCopy = ps.GetPageCountPerCopy();
			if (nPageCountPerCopy > 0) {
				bIsEmf = TRUE;
				theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,3,nPageCountPerCopy=%d,bIsEmf=%d"), nPageCountPerCopy, bIsEmf);
				if (nPageCountPerCopy < (nFrom + 1))
				{
					theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,4,nPageCountPerCopy=%d,nFrom=%d,set new nFrom=%d"), nPageCountPerCopy, nFrom, 0);
					nFrom = 0;
				}
				if (nPageCountPerCopy > (nTo + 1))
				{
					theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,5,nPageCountPerCopy=%d,nTo=%d,set new nTo=%d"), nPageCountPerCopy, nTo, nPageCountPerCopy - 1);
					nTo = nPageCountPerCopy - 1;
				}
			}
		}
		

	}
	else if (szExt.CompareNoCase(_T(".emf")) == 0)
	{
		bIsEmf = TRUE;
		CParseSpl ps;
		if (ps.InitParse(szFilePath)) {
			int nPageCountPerCopy = ps.GetPageCountPerCopy();
			if (nPageCountPerCopy > 0) {
				bIsEmf = TRUE;
				theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,3,nPageCountPerCopy=%d,bIsEmf=%d"), nPageCountPerCopy, bIsEmf);
				if (nPageCountPerCopy < (nFrom + 1))
				{
					theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,4,nPageCountPerCopy=%d,nFrom=%d,set new nFrom=%d"), nPageCountPerCopy, nFrom, 0);
					nFrom = 0;
				}
				if (nPageCountPerCopy > (nTo + 1))
				{
					theLog.Write(_T("PrintFileTest::on_pushButton_print_clicked,5,nPageCountPerCopy=%d,nTo=%d,set new nTo=%d"), nPageCountPerCopy, nTo, nPageCountPerCopy - 1);
					nTo = nPageCountPerCopy - 1;
				}
			}
		}
	}
	else
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_print_clicked,szExt=%s"), szExt);
		
	}

	if (bImage)
	{
		BOOL bUsePhysicalArea = FALSE;
		bPrintSucc = helper.PrintTiffFile(szFilePath, this, nFrom, nTo, bUsePhysicalArea);
	}
	else if (bIsPdf)
	{
		bPrintSucc = helper.PrintPdfFile(szFilePath, this, nFrom, nTo);
	}
	else if (bIsSpl)
	{
		bPrintSucc = helper.PrintSplFile(szFilePath, this, nTo + 1,(BRAND_TYPE)0,TRUE);
	}
	else if (bIsEmf)
	{
		bPrintSucc = helper.PrintEmfFile(szFilePath, this,nFrom,nTo);
	}
	else
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_print_clicked,unkown ext"));
	}
}



void PrintFileTest::on_pushButton_paper_clicked()
{

	CString szPrinterName = comboBox->currentText().toStdWString().c_str();
	if (szPrinterName.IsEmpty())
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_paper_clicked,printer name is empty"));
		return;
	}

	BOOL bRet = FALSE;
	int nPaperCount = 0;
	CString szPaperInfo = _T("");
	szPaperInfo.Format(_T("Index\tPaperSize\tPoint\t\tPaperName\r\n"));

	do
	{
		CString szPort = GetPrinterPort(szPrinterName);
		theLog.Write(_T("PrintFileTest::on_pushButton_paper_clicked,szPort=%s"), szPort);
		if (szPort.IsEmpty())
		{
			break;
		}

		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERS, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_paper_clicked,nPaperCount=%d"), nPaperCount);
		WORD* pwdPapers = new WORD[nPaperCount];
		memset(pwdPapers, 0, sizeof(WORD)*nPaperCount);
		/*
		DC_PAPERS
		Retrieves a list of supported paper sizes.
		The pOutput buffer receives an array of WORD values that indicate the available paper sizes for the printer.
		The return value indicates the number of entries in the array.
		For a list of the possible array values, see the description of the dmPaperSize member of the DEVMODE structure.
		If pOutput is NULL, the return value indicates the required number of entries in the array.
		*/
		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERS, (LPTSTR)pwdPapers, NULL);

		if (nPaperCount <= 0)
		{
			break;
		}

		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERS, (LPTSTR)pwdPapers, NULL);
		if (nPaperCount <= 0)
		{
			break;
		}

		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERSIZE, NULL, NULL);

		if (nPaperCount <= 0)
		{
			break;
		}

		POINT *pPaperSize = new POINT[nPaperCount];	//array of POINT structures
		memset(pPaperSize, 0, sizeof(POINT)*nPaperCount);



		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERSIZE, (LPTSTR)pPaperSize, NULL);
		if (nPaperCount <= 0)
		{
			break;
		}

		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERNAMES, NULL, NULL);
		if (nPaperCount <= 0)
		{
			break;
		}

		TCHAR *pPaperName = new TCHAR[nPaperCount * 64];
		memset(pPaperName, 0x0, sizeof(TCHAR)*nPaperCount * 64);
		nPaperCount = DeviceCapabilities(szPrinterName, szPort, DC_PAPERNAMES, (LPTSTR)pPaperName, NULL);
		if (nPaperCount <= 0)
		{
			break;
		}

		for (int i = 0; i < nPaperCount; i++)
		{
			//theLog.Write(_T("PrintFileTest::on_pushButton_paper_clicked,i=%d,dd=%d,(%d,%d),name=%s")
			//	, i + 1, pwdPapers[i], pPaperSize[i].x, pPaperSize[i].y, pPaperName + (i * 64));

			szPaperInfo.AppendFormat(_T("i=%d\t%d\t(%d,%d)\t\t%s\r\n"), i + 1, pwdPapers[i], pPaperSize[i].x, pPaperSize[i].y, pPaperName + (i * 64));
		}




		bRet = TRUE;

		delete[] pPaperName;
		pPaperName = nullptr;
		delete[] pPaperSize;
		pPaperSize = nullptr;
		delete[] pwdPapers;
		pwdPapers = nullptr;

	} while (0);


	if (!bRet)
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_paper_clicked,获取纸张信息失败"));
		return;
	}

	textBrowser->setText(QString::fromStdWString(szPaperInfo.GetString()));


	theLog.Write(_T("PrintFileTest::on_pushButton_paper_clicked,end"));


}

void PrintFileTest::on_pushButton_input_clicked()
{

	CString szPrinterName = comboBox->currentText().toStdWString().c_str();
	if (szPrinterName.IsEmpty())
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_input_clicked,printer name is empty"));
		return;
	}

	BOOL bRet = FALSE;
	int nInputCount = 0;
	CString szInput = _T("");
	szInput.Format(_T("Index\tSourceSize\tSourceName\r\n"));

	do
	{
		CString szPort = GetPrinterPort(szPrinterName);
		theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,szPort=%s"), szPort);
		if (szPort.IsEmpty())
		{
			break;
		}

		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINS, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,nPaperCount=%d"), nInputCount);
 		WORD* pInputValue = new WORD[nInputCount];
 		memset(pInputValue, 0, sizeof(WORD)*nInputCount);

		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINS, (LPTSTR)pInputValue, NULL);

		if (nInputCount <= 0)
		{
			break;
		}

		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINNAMES, NULL, NULL);
		if (nInputCount <= 0)
		{
			break;
		}

		TCHAR *pInputName = new TCHAR[nInputCount * 24];
		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINNAMES, (LPTSTR)pInputName, NULL);
		if (nInputCount <= 0)
		{
			break;
		}
// 
// 
		for (int i = 0; i < nInputCount; i++)
		{
			//theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,i=%d,pInputValue=%d,name=%s")
			//	, i + 1, pInputValue[i], pInputName+24*i);
			szInput.AppendFormat(_T("i=%d\t%d\t%s\r\n"), i + 1, pInputValue[i], pInputName + 24 * i);
		}
 		bRet = TRUE;
		delete[] pInputValue;
		pInputValue = nullptr;
		delete pInputName;
		pInputName = nullptr;

	} while (0);
	if (!bRet)
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_input_clicked,获取纸盒信息失败"));
		return;
	}
	textBrowser->setText(QString::fromStdWString(szInput.GetString()));
	theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,end"));
}

void PrintFileTest::on_pushButton_other_clicked()
{
	CString szPrinterName = comboBox->currentText().toStdWString().c_str();
	if (szPrinterName.IsEmpty())
	{
		theLog.Write(_T("!!PrintFileTest::on_pushButton_other_clicked,printer name is empty"));
		return;
	}

	BOOL bRet = FALSE;
	int nCount = 0;
 	CString szOtherInfo = _T("");
// 	szInput.Format(_T("Index\tSourceSize\tSourceName\r\n"));

	do
	{
		CString szPort = GetPrinterPort(szPrinterName);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked,szPort=%s"), szPort);
		if (szPort.IsEmpty())
		{
			break;
		}

		nCount = DeviceCapabilities(szPrinterName, szPort, DC_COLLATE, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked, 逐份：%d"), nCount);

		nCount = DeviceCapabilities(szPrinterName, szPort, DC_COLORDEVICE, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked,彩色：%d"), nCount);

		nCount = DeviceCapabilities(szPrinterName, szPort, DC_COPIES, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked,支持份数：%d"), nCount);

		nCount = DeviceCapabilities(szPrinterName, szPort, DC_DRIVER, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked,驱动号：%d"), nCount);


		nCount = DeviceCapabilities(szPrinterName, szPort, DC_ENUMRESOLUTIONS, NULL, NULL);
		theLog.Write(_T("PrintFileTest::on_pushButton_other_clicked,分辨率：%d"), nCount);
		if (nCount <= 0)
		{
			break;
		}

		LONG *pResolutions = new LONG[nCount];
		memset(pResolutions, 0x0, sizeof(LONG)*nCount);

 		nCount = DeviceCapabilities(szPrinterName, szPort, DC_ENUMRESOLUTIONS, (LPTSTR)pResolutions, NULL);

		//theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,1,pResolutions=%ld,nCount=%d"), pResolutions[0], nCount);
	//	theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,2,pResolutions=%ld"), pResolutions[1]);

// 		if (nCount <= 0)
// 		{
// 			break;
// 		}


		


// 		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINNAMES, NULL, NULL);
// 		if (nInputCount <= 0)
// 		{
// 			break;
// 		}
// 
// 		TCHAR *pInputName = new TCHAR[nInputCount * 24];
// 		nInputCount = DeviceCapabilities(szPrinterName, szPort, DC_BINNAMES, (LPTSTR)pInputName, NULL);
// 		if (nInputCount <= 0)
// 		{
// 			break;
// 		}
// 		// 
// 		// 
		for (int i = 0; i < nCount; i++)
		{
			//theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,i=%d,pResolutions=%ld")
			//	, i + 1, pResolutions[i]);
			
		}
// 		bRet = TRUE;
 		delete[] pResolutions;
		pResolutions = nullptr;
// 		delete pInputName;
// 		pInputName = nullptr;

	} while (0);
// 	if (!bRet)
// 	{
// 		theLog.Write(_T("!!PrintFileTest::on_pushButton_input_clicked,获取纸盒信息失败"));
// 		return;
// 	}
//	textBrowser->setText(QString::fromStdWString(szInput.GetString()));
//	theLog.Write(_T("PrintFileTest::on_pushButton_input_clicked,end"));
}