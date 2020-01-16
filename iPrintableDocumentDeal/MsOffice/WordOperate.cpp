#include "StdAfx.h"
#include "WordOperate.h"

CWordOperate::CWordOperate(void)
{
	CloseApp();
}

CWordOperate::~CWordOperate(void)
{
	CloseApp();
}

BOOL CWordOperate::CreateApp()  
{
	if (m_bInitOk)
	{
		return TRUE;
	}

	CloseApp();
	try
	{
		if (!m_oApp.CreateDispatch(_T("Word.Application")))  
		{  
			theLog.Write(_T("!!CWordOperate::CreateApp,����ʧ��,1����ȷ����װ��word 2000�����ϰ汾!err=%d"), GetLastError());    
			return FALSE;  
		}

		m_oApp.put_DisplayAlerts(FALSE);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::CreateApp,����ʧ��,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (m_oApp.m_lpDispatch)
	{
		theLog.Write(_T("CWordOperate::CreateApp,�����ɹ�,3��Version=%s"), GetVersion());
		m_bInitOk = TRUE;
		return TRUE;  
	}
	else
	{
		theLog.Write(_T("!!CWordOperate::CreateApp,����ʧ��,4"));
		return FALSE;  
	}	
}  

BOOL CWordOperate::CreateDocuments()  
{  
	if (!CreateApp())   
	{
		theLog.Write(_T("!!CWordOperate::CreateDocuments,1,CreateApp fail!"));  
		return FALSE;  
	}

	if (m_oDocuments.m_lpDispatch)
	{
		theLog.Write(_T("##CWordOperate::CreateDocuments,2,m_lpDispatch=%p"), m_oDocuments.m_lpDispatch);  
		return TRUE;  
	}

	try
	{
		m_oDocuments.AttachDispatch(m_oApp.get_Documents()); 
		if (!m_oDocuments.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CWordOperate::CreateDocuments,3,m_oDocuments fail!"));  
			return FALSE;  
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::CreateDocuments,4,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;  
}  

BOOL CWordOperate::CreateDocument()  
{  
	if (!m_oDocuments.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CWordOperate::CreateDocument,1,DocumentsΪ��!"));  
		return FALSE;  
	}  

	try
	{
		COleVariant varTrue(short(1),VT_BOOL);
		COleVariant vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		CComVariant Template(_T(""));    //û��ʹ��WORD���ĵ�ģ��  
		CComVariant NewTemplate(false);
		COleVariant DocumentType((long)0, VT_I4);
		COleVariant Visible;  

		m_oDocuments.Add(&Template,&NewTemplate,&DocumentType,&Visible);      

		//�õ�document����  
		m_oDocument = m_oApp.get_ActiveDocument();  
		if (!m_oDocument.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CWordOperate::CreateDocument,2,Document��ȡʧ��!"));  
			return FALSE;  
		}  
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::CreateDocument,5,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;  
}  

BOOL CWordOperate::Create()  
{  
	if (!CreateDocuments())   
	{
		theLog.Write(_T("!!CWordOperate::Create,1,CreateDocuments fail."));
		return FALSE;  
	}  
	return CreateDocument();  
}  

//����app��ʾ������״̬
void CWordOperate::SetVisible(BOOL bVisible)
{
	try
	{
		m_oApp.put_Visible(bVisible);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::SetVisible,catch CException [%s],err=%d,bVisible=[%d]"), tcErrMsg, GetLastError(), bVisible);
	}
}

CString CWordOperate::GetVersion()
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
		theLog.Write(_T("!!CWordOperate::GetVersion,catch CException [%s],err=%d"), tcErrMsg, GetLastError());
	}

	return szVersion;
}

BOOL CWordOperate::OpenDocument(CString fileName)  
{
	if (!m_oDocuments.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CWordOperate::OpenDocument,1,DocumentsΪ��!"));  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vFileName(fileName);  
		COleVariant vPassWord(_T("nullpwd"));	//����һ����������룬��ֹ������������򣬴Ӷ������̣߳�
												//Ч�����ǣ�һ������������ĵ���ֱ���׳��쳣����ʧ�ܣ�
												//������û��������ĵ���ֱ�Ӵ��ĵ���

		//�õ�document����  
		m_oDocument.AttachDispatch(m_oDocuments.Open(  
			vFileName,        // FileName  
			vFalse,            // Confirm Conversion.  
			vTrue,            // ReadOnly.  
			vFalse,            // AddToRecentFiles.  
			vPassWord,        // PasswordDocument.  
			vPassWord,        // PasswordTemplate.  
			vOptional,        // Revert.  
			vPassWord,        // WritePasswordDocument.  
			vPassWord,        // WritePasswordTemplate.  
			vOptional,        // Format. // Last argument for Word 97  
			vOptional,        // Encoding // New for Word 2000/2002  
			vOptional,        // Visible  
			//����4����word2003��Ҫ�Ĳ��������汾��word2000��  
			vOptional,    // OpenAndRepair  
			vOptional,            // DocumentDirection wdDocumentDirection LeftToRight  
			vOptional,    // NoEncodingDialog  
			vOptional  

			)                // Close Open parameters  
			);                    // Close AttachDispatch  

		if (!m_oDocument.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CWordOperate::OpenDocument,2,Document��ȡʧ��!"));  
			return FALSE;  
		}  
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::OpenDocument,5,catch CException [%s],err=%d,file=[%s]"), tcErrMsg, GetLastError(), fileName);
		return FALSE;  
	}

	return TRUE;  
}  

BOOL CWordOperate::Open(CString fileName)  
{  
	if (!CreateDocuments())   
	{  
		theLog.Write(_T("!!CWordOperate::Open,CreateDocuments fail"));
		return FALSE;  
	}  

	return OpenDocument(fileName);  
}  

BOOL CWordOperate::SetActiveDocument(short i)  
{  
	try
	{
		COleVariant vIndex(i);
		COleVariant vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

		m_oDocument.AttachDispatch(m_oDocuments.Item(vIndex));  
		m_oDocument.Activate();  
		if (!m_oDocument.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CWordOperate::SetActiveDocument,1,Document��ȡʧ��!"));  
			return FALSE;  
		}  
		//�õ�selection����  
		m_oSelecton = m_oApp.get_Selection();  
		if (!m_oSelecton.m_lpDispatch)   
		{  
			theLog.Write(_T("!!CWordOperate::SetActiveDocument,2,Select��ȡʧ��!"));  
			return FALSE;  
		}  
		//�õ�ȫ��DOC��Range����  
		m_oRange = m_oDocument.Range(vOptional,vOptional);  
		if(!m_oRange.m_lpDispatch)  
		{  
			theLog.Write(_T("!!CWordOperate::SetActiveDocument,3,Range��ȡʧ��!"));  
			return FALSE;  
		}  	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::SetActiveDocument,4,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;  
}  

BOOL CWordOperate::SaveDocument()  
{  
	if (!m_oDocument.m_lpDispatch)   
	{  
		//theLog.Write(_T("!!CWordOperate::SaveDocument,1,Document��ȡʧ��!"));  
		return TRUE;  
	}  
	try
	{
		m_oDocument.Save();  
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::SaveDocument,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}
	return TRUE;  
}  

BOOL CWordOperate::SaveDocumentAs(CString fileName)  
{  
	if (!m_oDocument.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CWordOperate::SaveDocumentAs,1,Document��ȡʧ��!fileName=[%s]"), fileName);  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vFileName(fileName);  

		m_oDocument.SaveAs(  
			vFileName,    //VARIANT* FileName  
			vOptional,    //VARIANT* FileFormat  
			vOptional,    //VARIANT* LockComments  
			vOptional,    //VARIANT* Password  
			vOptional,    //VARIANT* AddToRecentFiles  
			vOptional,    //VARIANT* WritePassword  
			vOptional,    //VARIANT* ReadOnlyRecommended  
			vOptional,    //VARIANT* EmbedTrueTypeFonts  
			vOptional,    //VARIANT* SaveNativePictureFormat  
			vOptional,    //VARIANT* SaveFormsData  
			vOptional,    //VARIANT* SaveAsAOCELetter  
			vOptional,    //VARIANT* ReadOnlyRecommended  
			vOptional,    //VARIANT* EmbedTrueTypeFonts  
			vOptional,    //VARIANT* SaveNativePictureFormat  
			vOptional,    //VARIANT* SaveFormsData  
			vOptional    //VARIANT* SaveAsAOCELetter  
			);  	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::SaveDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	return TRUE;  
}  

BOOL CWordOperate::SaveDocumentAs(CString fileName,WdSaveFormat wdFormat)  
{  
	if (!m_oDocument.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CWordOperate::SaveDocumentAs,1,Document��ȡʧ��!fileName=[%s],wdFormat=%d"), fileName, wdFormat);  
		return FALSE;  
	}  

	try
	{
		COleVariant vTrue((short)TRUE);    
		COleVariant	vFalse((short)FALSE);
		COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vFileName(fileName);  
		COleVariant	vFileFormat((long)wdFormat, VT_I4);

		//SaveAs������ʹ��word���һ�α���ʹ�õĲ�����ΪĬ�ϲ�����
		//�������������ǳ�����Ҫ�Ľ��������ʹ��ExportAsFixedFormat��
		m_oDocument.SaveAs(  
			vFileName,    //VARIANT* FileName  
			vFileFormat,    //VARIANT* FileFormat  
			vOptional,    //VARIANT* LockComments  
			vOptional,    //VARIANT* Password  
			vOptional,    //VARIANT* AddToRecentFiles  
			vOptional,    //VARIANT* WritePassword  
			vOptional,    //VARIANT* ReadOnlyRecommended  
			vOptional,    //VARIANT* EmbedTrueTypeFonts  
			vOptional,    //VARIANT* SaveNativePictureFormat  
			vOptional,    //VARIANT* SaveFormsData  
			vOptional,    //VARIANT* SaveAsAOCELetter  
			vOptional,    //VARIANT* ReadOnlyRecommended  
			vOptional,    //VARIANT* EmbedTrueTypeFonts  
			vOptional,    //VARIANT* SaveNativePictureFormat  
			vOptional,    //VARIANT* SaveFormsData  
			vOptional    //VARIANT* SaveAsAOCELetter  
			);	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::SaveDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (!PathFileExists(fileName))
	{
		theLog.Write(_T("!!CWordOperate::SaveDocumentAs,3,PathFileExists fail,fileName=[%s]"), fileName);
		return FALSE;
	}

	return TRUE;  
}  

BOOL CWordOperate::ExportDocumentAs(CString fileName,WdExportFormat wdFormat)
{
	if (!m_oDocument.m_lpDispatch)   
	{  
		theLog.Write(_T("!!CWordOperate::ExportDocumentAs,1,Document��ȡʧ��!fileName=[%s],wdFormat=%d"), fileName, wdFormat);  
		return FALSE;  
	}  

	try
	{
		COleVariant	vtMissing((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		m_oDocument.ExportAsFixedFormat(
			(LPCTSTR)fileName,				//LPCTSTR OutputFileName
			wdFormat,						//long ExportFormat
			FALSE,							//BOOL OpenAfterExport
			wdExportOptimizeForPrint,		//long OptimizeFor
			wdExportAllDocument,			//long Range
			1,								//long From
			1,								//long To
			wdExportDocumentContent,		//long Item
			FALSE,							//BOOL IncludeDocProps
			FALSE,							//BOOL KeepIRM	IRM��ϢȨ�޹���(Information Rights Management) 
			wdExportCreateHeadingBookmarks,	//long CreateBookmarks
			TRUE,							//BOOL DocStructureTags
			TRUE,							//BOOL BitmapMissingFonts
			FALSE,							//BOOL UseISO19005_1
			vtMissing						//VARIANT * FixedFormatExtClassPtr
			);
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::ExportDocumentAs,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}

	if (!PathFileExists(fileName))
	{
		theLog.Write(_T("!!CWordOperate::ExportDocumentAs,3,PathFileExists fail,fileName=[%s]"), fileName);
		return FALSE;
	}

	return TRUE;  
}

BOOL CWordOperate::CloseDocument()  
{  
	try
	{
		if (m_oDocument.m_lpDispatch)
		{
			COleVariant vTrue((short)TRUE);
			COleVariant	vFalse((short)FALSE);
			COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
			m_oDocument.Close(vFalse, vTrue, vFalse);
			m_oDocument.ReleaseDispatch();
		}
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::CloseDocument,2,catch CException [%s]"), tcErrMsg);
		return FALSE;  
	}
	return TRUE;  
}  

void CWordOperate::CloseApp()  
{ 
	m_bInitOk = FALSE;

	try
	{
		//�ͷ��ڴ�������Դ  
		if (m_oRange.m_lpDispatch)
		{
			m_oRange.ReleaseDispatch();  
		}

		if (m_oSelecton.m_lpDispatch)
		{
			m_oSelecton.ReleaseDispatch();
		}

		if (m_oDocument.m_lpDispatch)
		{
			CloseDocument();
		}

		if (m_oDocuments.m_lpDispatch)
		{
			m_oDocuments.ReleaseDispatch();  
		}

		if (m_oApp.m_lpDispatch)
		{
			COleVariant vTrue((short)TRUE);
			COleVariant	vFalse((short)FALSE);
			COleVariant	vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
			m_oApp.Quit(vFalse, vTrue,	vFalse);  
			m_oApp.ReleaseDispatch();  
		}	
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::CloseApp,2,catch CException [%s]"), tcErrMsg);
	}
}  

void CWordOperate::WriteText(CString szText)  
{  
	m_oSelecton.TypeText(szText);  
}  

void CWordOperate::WriteNewLineText(CString szText, int nLineCount /**//* = 1 */)  
{  
	int i;  
	if (nLineCount <= 0)  
	{  
		nLineCount = 0;  
	}  
	for (i = 0; i < nLineCount; i++)  
	{  
		m_oSelecton.TypeParagraph();  
	}  
	WriteText(szText);  
}  

void CWordOperate::WriteEndLine(CString szText)  
{  
	m_oRange.InsertAfter(szText);  
}  

void CWordOperate::WholeStory()  
{  
	m_oRange.WholeStory();  
}  

void CWordOperate::Copy()  
{  
	m_oSelecton.Copy();  
	//m_oSelecton.CopyFormat();  
}  

void CWordOperate::TypeParagraph()  
{  
	m_oSelecton.TypeParagraph();  
}  

void CWordOperate::PasteAndFormat()  
{  
	m_oSelecton.PasteAndFormat(0);  
}  

void CWordOperate::Paste()  
{  
	m_oSelecton.Paste();  
	//m_oSelecton.PasteFormat();  
}  

void CWordOperate::TypeBackspace(int count)  
{  
	for(int i = 0; i < count; i++)  
		m_oSelecton.TypeBackspace();  
} 

void CWordOperate::InsertFile(CString fileName)  
{  
    COleVariant     vFileName(fileName),  
                 vTrue((short)TRUE),  
                 vFalse((short)FALSE),  
                 vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR),  
                 vNull(_T(""));  
    /**//* 
    void InsertFile(LPCTSTR FileName, VARIANT* Range, VARIANT* ConfirmConversions, VARIANT* Link, VARIANT* Attachment); 
    */  
    m_oSelecton.InsertFile(  
                    fileName,  
                    vNull,  
                    vFalse,  
                    vFalse,  
                    vFalse  
                    );  
}  
  
void CWordOperate::SelectMoveDown(short lineCount, short unit)//��ѡ��������ƶ�  
{  
    m_oSelecton.MoveDown(COleVariant(unit), COleVariant((short)lineCount),COleVariant((short)wdExtend));  
}  
  
void CWordOperate::NoneSelectMoveDown(short lineCount, short unit)//�����ƶ���꣬��ѡ��  
{  
    m_oSelecton.MoveDown(COleVariant(unit), COleVariant((short)lineCount),COleVariant((short)wdMove));  
}  
  
void CWordOperate::SelectMoveUp(short lineCount, short unit)//��ѡ��������ƶ�  
{  
    m_oSelecton.MoveUp(COleVariant(unit), COleVariant((short)lineCount),COleVariant((short)wdExtend));  
}  
  
void CWordOperate::NoneSelectMoveUp(short lineCount, short unit)//�����ƶ���꣬��ѡ��  
{  
    m_oSelecton.MoveUp(COleVariant(unit), COleVariant((short)lineCount),COleVariant((short)wdMove));  
}  
  
void CWordOperate::SelectMoveLeft(short charCount, short unit)//��ѡ��������ƶ�  
{  
    m_oSelecton.MoveLeft(COleVariant(unit), COleVariant((short)charCount),COleVariant((short)wdExtend));  
}  
  
void CWordOperate::NoneSelectMoveLeft(short charCount, short unit)//  
{  
    m_oSelecton.MoveLeft(COleVariant(unit), COleVariant((short)charCount),COleVariant((short)wdMove));  
}  
void CWordOperate::SelectMoveRight(short charCount, short unit)//��ѡ��������ƶ�  
{  
    m_oSelecton.MoveRight(COleVariant(unit), COleVariant((short)charCount),COleVariant((short)wdExtend));  
}  
void CWordOperate::NoneSelectMoveRight(short charCount, short unit)//  
{  
    m_oSelecton.MoveRight(COleVariant(unit), COleVariant((short)charCount),COleVariant((short)wdMove));  
}  
void CWordOperate::MoveToFirst()  
{  
    m_oSelecton.GoTo(COleVariant((short)1), COleVariant((short)2), COleVariant((short)0), COleVariant(_T("1")));  
}  
  
void CWordOperate::MoveToNextPage()  
{  
    m_oSelecton.GoTo(COleVariant((short)1), COleVariant((short)2), COleVariant((short)1), COleVariant(_T("")));  
}

//��ӡ�ļ�
BOOL CWordOperate::Print()
{
	if (!m_oDocument.m_lpDispatch)
	{
		theLog.Write(_T("!!CWordOperate::Print,1,m_oDocument fail"));
		return FALSE;
	}

	try
	{
		//m_oDocument.PrintPreview();

		// ��COleVariant��������ȽϷ���
		COleVariant covTrue((short)TRUE);
		COleVariant	covFalse((short)FALSE);
		COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant covOutputFileName(_T("C:\\word.emf.spl"));
		m_oDocument.PrintOut(
			covFalse,              // Background.
			covOptional,           // Append.
			covOptional,           // Range.
			covOutputFileName,     // OutputFileName.
			covOptional,           // From.
			covOptional,           // To.
			covOptional,           // Item.
			COleVariant((long)2),  // Copies.
			covOptional,           // Pages.
			covOptional,           // PageType.
			covTrue,			   // PrintToFile.
			covOptional,           // Collate.
			covOptional,           // ActivePrinterMacGX.
			covOptional,            // ManualDuplexPrint.
			covOptional,           // PrintZoomColumn  New with Word 2002
			covOptional,           // PrintZoomRow          ditto
			covOptional,           // PrintZoomPaperWidth   ditto
			covOptional);          // PrintZoomPaperHeight  ditto
	}
	catch (CException* e)
	{
		TCHAR tcErrMsg[512] = {0};
		e->GetErrorMessage(tcErrMsg, 512);
		theLog.Write(_T("!!CWordOperate::Print,2,catch CException [%s]"), tcErrMsg);
		return FALSE;
	}

	return TRUE;
}