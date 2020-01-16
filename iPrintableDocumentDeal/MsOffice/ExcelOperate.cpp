#include "StdAfx.h"
#include "ExcelOperate.h"

CExcelOperate::CExcelOperate(void)
{
	CloseApp();
}

CExcelOperate::~CExcelOperate(void)
{
	CloseApp();
}

BOOL CExcelOperate::CreateApp()
{
	if (m_bInitOk)
	{
		return TRUE;
	}

	CloseApp();

	try
	{
		if (!m_oApp.CreateDispatch(_T("Excel.Application")))  
		{  
			theLog.Write(_T("!!CExcelOperate::CreateApp,����ʧ��,1����ȷ����װ��Excel 2000�����ϰ汾!err=%d"), GetLastError());    
			return FALSE;  
		}

		m_oApp.put_DisplayAlerts(FALSE);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CreateApp,����ʧ��,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (m_oApp.m_lpDispatch)
	{
		theLog.Write(_T("CExcelOperate::CreateApp,�����ɹ�,3��Version=%s"), GetVersion());
		m_bInitOk = TRUE;
		return TRUE;  
	}
	else
	{
		theLog.Write(_T("!!CExcelOperate::CreateApp,����ʧ��,4"));
		return FALSE;  
	}	
}

//����һ������������(Workbooks),����N����������Workbook��
BOOL CExcelOperate::CreateWorkbooks()            
{
	if (!CreateApp())
	{
		theLog.Write(_T("!!CExcelOperate::CreateWorkbooks,1,CreateApp fail"));
		return FALSE;
	}

	if (m_oWorkbooks.m_lpDispatch)
	{
		theLog.Write(_T("##CExcelOperate::CreateWorkbooks,2,m_lpDispatch=%p"), m_oWorkbooks.m_lpDispatch);
		return TRUE;
	}

	try
	{
		m_oWorkbooks.AttachDispatch(m_oApp.get_Workbooks());
		if (!m_oWorkbooks.m_lpDispatch)
		{
			theLog.Write(_T("!!CExcelOperate::CreateWorkbooks,3,m_oWorkbooks fail"));
			return FALSE;
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CreateWorkbooks,4,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;
}

//����һ��������(Workbook)������һ��������sheets������ 
BOOL CExcelOperate::CreateWorkbook()
{
	if (!m_oWorkbooks.m_lpDispatch)
	{
		if (!CreateWorkbooks())
		{
			theLog.Write(_T("!!CExcelOperate::CreateWorkbook,1,m_oWorkbooks fail"));
			return FALSE;
		}
	}

	try
	{
		CComVariant Template(_T(""));    //û��ʹ��Excel���ĵ�ģ��  

		m_oWorkbook.AttachDispatch(m_oWorkbooks.Add(Template));
		if (!m_oWorkbook.m_lpDispatch)
		{
			theLog.Write(_T("!!CExcelOperate::CreateWorkbook,2,m_oWorkbook fail"));
			return FALSE;
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CreateWorkbook,3,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;
}

//����һ����������(Worksheets)������N��������(Worksheet)
BOOL CExcelOperate::CreateWorksheets()
{
	if (!m_oWorkbook.m_lpDispatch)
	{
		theLog.Write(_T("!!CExcelOperate::CreateWorksheets,1,m_oWorkbook fail"));
		return FALSE;
	}

	try
	{
		m_oWorksheets.AttachDispatch(m_oWorkbook.get_Worksheets());
		if (!m_oWorksheets.m_lpDispatch)
		{
			theLog.Write(_T("!!CExcelOperate::CreateWorksheets,2,m_oWorksheets fail"));
			return FALSE;
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CreateWorksheets,3,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;
}

//����һ��������(Worksheet)��Ҳ����������Excel�п�����Sheet1��Sheet2��Sheet3�����ǲ���Excel�Ļ�����λ
BOOL CExcelOperate::CreateWorksheet()
{
	if (!m_oWorksheets.m_lpDispatch)
	{
		theLog.Write(_T("!!CExcelOperate::CreateWorksheet,1,m_oWorksheets fail"));
		return FALSE;
	}

	try
	{
		CComVariant Template(_T("sheet1"));    //û��ʹ��Excel���ĵ�ģ��  
		CComVariant Count((long)1, VT_I4);    
		CComVariant Type(xlWorksheet);
		m_oWorksheet.AttachDispatch(m_oWorksheets.Add(Template, Template, Count, Type));
		if (!m_oWorksheet.m_lpDispatch)
		{
			theLog.Write(_T("!!CExcelOperate::CreateWorksheet,2,m_oWorksheet fail"));
			return FALSE;
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CreateWorksheet,3,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;
}

//����app��ʾ������״̬
void CExcelOperate::SetVisible(BOOL bVisible)
{
	try
	{
		m_oApp.put_Visible(bVisible);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::SetVisible,catch CException [%s],err=%d,bVisible=[%d]"), tcErrMsg, GetLastError(), bVisible);
	}
}

//����Excelҳ���С�ͷ���
BOOL CExcelOperate::SetPageParam(XlPaperSize lPaperSize, XlPageOrientation lPageOrientation)
{
	if (!m_oApp.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::SetPageParam,1,m_oApp fail"));  
		return FALSE;  
	}  

	if (!m_oWorkbook.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::SetPageParam,2,m_oWorkbook fail"));  
		return FALSE;  
	}  

	try
	{
		m_oWorksheets.AttachDispatch(m_oWorkbook.get_Worksheets());
		if (!m_oWorksheets.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CExcelOperate::SetPageParam,3,m_oWorksheets fail"));  
			return FALSE;  
		}  

		//��ȡsheet����
		long lCount = m_oWorksheets.get_Count();
		theLog.Write(_T("CExcelOperate::SetPageParam,4,Worksheets,lCount=%d"),lCount);
		for (int nIndex=0; nIndex<lCount; nIndex++)
		{
#if 0
			CString szSheetActivate;
			szSheetActivate.Format(_T("Sheet%d"), i+1);
			m_oApp.put_OnSheetActivate(szSheetActivate.GetString());
			m_oWorksheet.AttachDispatch(m_oApp.get_ActiveSheet());
			if (!m_oWorksheet.m_lpDispatch)   
			{  
				theLog.Write(_T("!!CExcelOperate::SetPageParam,5,m_oWorksheet fail,nIndex=%d"), nIndex);  
				return FALSE;  
			}
#else
			CComVariant Index(nIndex+1);
			m_oWorksheet.AttachDispatch(m_oWorksheets.get_Item(Index));
			if (!m_oWorksheet.m_lpDispatch)   
			{  
				theLog.Write(_T("!!CExcelOperate::SetPageParam,5,m_oWorksheet fail,nIndex=%d"), nIndex);  
				return FALSE;  
			}
#endif

			m_oPageSetup.AttachDispatch(m_oWorksheet.get_PageSetup());
			if (!m_oPageSetup.m_lpDispatch)   
			{  
				theLog.Write(_T("!!CExcelOperate::SetPageParam,7,m_oPageSetup fail,nIndex=%d"), nIndex);  
				return FALSE;  
			}

			//����ֽ��ʱ�����windowsû�а�װ��ӡ��������Spooler����δ������
			//��put_PaperSize��put_Orientation���ܻ�ʧ�ܣ������׳��쳣��
			m_oPageSetup.put_PaperSize(lPaperSize);
			m_oPageSetup.put_Orientation(lPageOrientation);
		}	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::SetPageParam,catch CException [%s],err=%d"), tcErrMsg, GetLastError());
		return FALSE;
	}

	return TRUE;
}

//��Ϣ
CString CExcelOperate::GetVersion()
{
	CString szVersion(_T("UnKnow"));
	try
	{
		szVersion = m_oApp.get_Version();	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::GetVersion,catch CException [%s],err=%d"), tcErrMsg, GetLastError());
	}

	return szVersion;
}

//**********************���ĵ�*********************************************  
BOOL CExcelOperate::OpenDocument(CString fileName)
{
	if (!m_oWorkbooks.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::OpenDocument,1,m_oWorkbooks fail"));  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vPassWord(_T("nullpwd"));	//����һ����������룬��ֹ������������򣬴Ӷ������̣߳�
												//Ч�����ǣ�һ������������ĵ���ֱ���׳��쳣����ʧ�ܣ�
												//������û��������ĵ���ֱ�Ӵ��ĵ���

		m_oWorkbook.AttachDispatch(m_oWorkbooks.Open(
			(LPCTSTR)fileName,	//LPCTSTR Filename
			vFalse,				//VARIANT& UpdateLinks
			vTrue,				//VARIANT& ReadOnly
			vOptional,			//VARIANT& Format
			vPassWord,			//VARIANT& Password
			vPassWord, 			//VARIANT& WriteResPassword
			vTrue, 				//VARIANT& IgnoreReadOnlyRecommended
			vOptional, 			//VARIANT& Origin
			vOptional, 			//VARIANT& Delimiter
			vOptional, 			//VARIANT& Editable
			vOptional, 			//VARIANT& Notify
			vOptional, 			//VARIANT& Converter
			vOptional, 			//VARIANT& AddToMru
			vOptional, 			//VARIANT& Local
			vOptional));		//VARIANT& CorruptLoad

		if (!m_oWorkbook.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CExcelOperate::OpenDocument,2,m_oWorkbook fail"));  
			return FALSE;  
		}  
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::OpenDocument,3,catch CException [%s],err=%d,file=[%s]"), tcErrMsg, GetLastError(), fileName);
		return FALSE;  
	}

	return TRUE; 
}

BOOL CExcelOperate::Open(CString fileName)
{
	if (!CreateWorkbooks())   
	{  
		theLog.Write(_T("!!CExcelOperate::Open,1,CreateWorkbooks fail"));
		return FALSE;  
	}

	return OpenDocument(fileName);  
}

BOOL CExcelOperate::SetActiveDocument(short i)
{
	theLog.Write(_T("##CExcelOperate::SetActiveDocument, no support method"));
	return FALSE;
}

//**********************�����ĵ�*********************************************  
BOOL CExcelOperate::SaveDocument()
{
	if (!m_oWorkbook.m_lpDispatch)
	{
		//theLog.Write(_T("!!CExcelOperate::SaveDocument,1,fail"));
		return TRUE;
	}

	try
	{
		m_oWorkbook.Save();
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::SaveDocument,2,catch CException [%s]"), tcErrMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CExcelOperate::SaveDocumentAs(CString fileName)
{
	if (!m_oWorkbook.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::SaveDocumentAs,1,m_oWorkbook fail,fileName=[%s]"), fileName);  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vFileName(fileName);

		m_oWorkbook.SaveAs(  
			vFileName,    //VARIANT& FileName  
			vOptional,    //VARIANT& FileFormat  
			vOptional,    //VARIANT& Password  
			vOptional,    //VARIANT& WritePassword  
			vOptional,    //VARIANT& ReadOnlyRecommended  
			vOptional,    //VARIANT& CreateBackup  
			xlNoChange,    //long AccessMode  //XlSaveAsAccessMode
			vOptional,    //VARIANT& ConflictResolution  
			vOptional,    //VARIANT& AddToMru  
			vOptional,    //VARIANT& TextCodepage  
			vOptional,    //VARIANT& TextVisualLayout  
			vOptional     //VARIANT& Local  
			);  	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::SaveDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;  
}

BOOL CExcelOperate::SaveDocumentAs(CString fileName,XlFileFormat xlFormat) 
{
	if (!m_oWorkbook.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::SaveDocumentAs,1,m_oWorkbook fail,fileName=[%s],wdFormat=%d"), fileName, xlFormat);  
		return FALSE;  
	}  

	try
	{
		COleVariant vFileName(fileName);  	
		COleVariant	vFileFormat((long)xlFormat, VT_I4);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

		m_oWorkbook.SaveAs(  
			vFileName,    //VARIANT& FileName  
			vFileFormat,  //VARIANT& FileFormat  
			vOptional,    //VARIANT& Password  
			vOptional,    //VARIANT& WritePassword  
			vOptional,    //VARIANT& ReadOnlyRecommended  
			vOptional,    //VARIANT& CreateBackup  
			xlNoChange,   //long AccessMode  //XlSaveAsAccessMode
			vOptional,    //VARIANT& ConflictResolution  
			vOptional,    //VARIANT& AddToMru  
			vOptional,    //VARIANT& TextCodepage  
			vOptional,    //VARIANT& TextVisualLayout  
			vOptional     //VARIANT& Local  
			);  	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::SaveDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (!PathFileExists(fileName))
	{
		theLog.Write(_T("!!CExcelOperate::SaveDocumentAs,3,PathFileExists fail,fileName=[%s]"), fileName);
		return FALSE;
	}

	return TRUE;  
}

BOOL CExcelOperate::ExportDocumentAs(CString fileName,XlFixedFormatType xlFormat)
{
	if (!m_oWorkbook.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CExcelOperate::ExportDocumentAs,1,m_oWorkbook fail,fileName=[%s],wdFormat=%d"), fileName, xlFormat);  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant vFileName(fileName);  	
		COleVariant vQuality((short)xlQualityStandard);    
		COleVariant	vtMissing((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

		m_oWorkbook.ExportAsFixedFormat(
			xlFormat,			//long Type
			vFileName,			//VARIANT& Filename
			vQuality,			//VARIANT& Quality
			vTrue,				//VARIANT& IncludeDocProperties
			vFalse,				//VARIANT& IgnorePrintAreas
			vtMissing,			//VARIANT& From
			vtMissing,			//VARIANT& To
			vFalse,				//VARIANT& OpenAfterPublish
			vtMissing);			//VARIANT& FixedFormatExtClassPtr
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::ExportDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (!PathFileExists(fileName))
	{
		theLog.Write(_T("!!CExcelOperate::ExportDocumentAs,3,PathFileExists fail,fileName=[%s]"), fileName);
		return FALSE;
	}

	return TRUE;  
}

BOOL CExcelOperate::CloseDocument()
{
	try
	{
		if (m_oWorkbook.m_lpDispatch)
		{
			COleVariant vTrue((short)TRUE);
			COleVariant	vFalse((short)FALSE);
			COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
			COleVariant vFileName(m_oWorkbook.get_Name());  
			m_oWorkbook.Close(vFalse, vFileName, vFalse);
			m_oWorkbook.ReleaseDispatch();
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CloseDocument,2,catch CException [%s]"), tcErrMsg);
		return FALSE;
	}

	return TRUE;
}

void CExcelOperate::CloseApp()
{
	m_bInitOk = FALSE;

	//�ͷ���Դ

	try
	{
		COleVariant vTrue((short)TRUE);
		COleVariant	vFalse((short)FALSE);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

		if (m_oWorksheet.m_lpDispatch)
		{
			m_oWorksheet.ReleaseDispatch();
		}

		if (m_oWorksheets.m_lpDispatch)
		{
			m_oWorksheets.ReleaseDispatch();
		}

		if (m_oWorkbook.m_lpDispatch)
		{
			COleVariant vFileName(m_oWorkbook.get_Name());  
			m_oWorkbook.Close(vFalse, vFileName, vFalse);
			m_oWorkbook.ReleaseDispatch();
		}

		if (m_oWorkbooks.m_lpDispatch)
		{
			m_oWorkbooks.Close();
			m_oWorkbooks.ReleaseDispatch();
		}

		if (m_oApp.m_lpDispatch)
		{
			m_oApp.Quit();
			m_oApp.ReleaseDispatch();
		}	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::CloseApp,catch CException [%s]"), tcErrMsg);
	}
}
 
//��ӡ
BOOL CExcelOperate::Print()
{
	if (!m_oWorkbook.m_lpDispatch)
	{
		theLog.Write(_T("!!CExcelOperate::Print,1,m_oWorkbook fail"));
		return FALSE;
	}

	try
	{
		// ��COleVariant��������ȽϷ���
		COleVariant covTrue((short)TRUE);
		COleVariant	covFalse((short)FALSE);
		COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant covCopies((long)2);
		COleVariant covOutputFileName(_T("C:\\excel.emf.spl"));

		m_oWorkbook.PrintOut(
			covOptional,		//VARIANT& From
			covOptional,		//VARIANT& To
			covCopies,			//VARIANT& Copies
			covOptional,		//VARIANT& Preview
			covOptional,		//VARIANT& ActivePrinter
			covTrue,			//VARIANT& PrintToFile
			covOptional,		//VARIANT& Collate
			covOutputFileName,	//VARIANT& PrToFileName
			covOptional			//VARIANT& IgnorePrintAreas
			);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CExcelOperate::Print,catch CException [%s]"), tcErrMsg);
		return FALSE;
	}

	return TRUE;
}
