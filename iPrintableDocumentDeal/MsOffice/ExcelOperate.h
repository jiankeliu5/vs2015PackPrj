#pragma once

#include "ExcelHeader.h"

class _AA_DLL_EXPORT_ CExcelOperate
{
public:
	CExcelOperate(void);
	~CExcelOperate(void);

public:
    //����  
    //**********************�������ĵ�*******************************************  
    BOOL CreateApp();                   //����һ���µ�ExcelӦ�ó���  
    BOOL CreateWorkbooks();             //����һ������������(Workbooks),����N����������Workbook��
    BOOL CreateWorkbook();              //����һ��������(Workbook)������һ��������sheets������ 
	BOOL CreateWorksheets();			//����һ����������(Worksheets)������N��������(Worksheet)
	BOOL CreateWorksheet();			    //����һ��������(Worksheet)��Ҳ����������Excel�п�����Sheet1��Sheet2��Sheet3�����ǲ���Excel�Ļ�����λ
    BOOL Create();                      //�����µ�ExcelӦ�ó��򲢴���һ���µ��ĵ�  
    void SetVisible(BOOL bVisible);     //����app��ʾ������״̬
	BOOL SetPageParam(XlPaperSize lPaperSize, XlPageOrientation lPageOrientation);	//����Excelҳ���С�ͷ���

	//��Ϣ
	CString GetVersion();				//��ȡExcelVersion
  
    //**********************���ĵ�*********************************************  
    BOOL OpenDocument(CString fileName);//���Ѿ����ڵ��ĵ���  
    BOOL Open(CString fileName);        //�����µ�ExcelӦ�ó��򲢴�һ���Ѿ����ڵ��ĵ���  
    BOOL SetActiveDocument(short i);    //���õ�ǰ������ĵ���  
  
    //**********************�����ĵ�*********************************************  
    BOOL SaveDocument();                //�ĵ����Դ���ʽ�����档  
    BOOL SaveDocumentAs(CString fileName);//�ĵ��Դ�����ʽ�����档  
    BOOL SaveDocumentAs(CString fileName,XlFileFormat xlFormat);//�ĵ���ָ����ʽ�����档
    BOOL ExportDocumentAs(CString fileName,XlFixedFormatType xlFormat);//�ĵ���ָ����ʽ��������
    BOOL CloseDocument();  
    void CloseApp();  

	//��ӡ
	BOOL Print();

private:
	CApplication0 m_oApp;
	CWorkbooks m_oWorkbooks;
	CWorkbook m_oWorkbook;
	CWorksheets m_oWorksheets; 
	CWorksheet m_oWorksheet;
	CPageSetup0 m_oPageSetup;

	BOOL m_bInitOk;
};
