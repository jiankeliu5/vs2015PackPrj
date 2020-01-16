#pragma once

#include "WordHeader.h"

class _AA_DLL_EXPORT_ CWordOperate 
{
public:
	CWordOperate(void);
	~CWordOperate(void);

public:  
    //����  
    //**********************�������ĵ�*******************************************  
    BOOL CreateApp();                    //����һ���µ�WORDӦ�ó���  
    BOOL CreateDocuments();                //����һ���µ�Word�ĵ�����  
    BOOL CreateDocument();                //����һ���µ�Word�ĵ�  
    BOOL Create();                        //�����µ�WORDӦ�ó��򲢴���һ���µ��ĵ�  
	void SetVisible(BOOL bVisible);     //����app��ʾ������״̬

	//��Ϣ
	CString GetVersion();				//��ȡWordVersion
  
    //**********************���ĵ�*********************************************  
    BOOL OpenDocument(CString fileName);//���Ѿ����ڵ��ĵ���  
    BOOL Open(CString fileName);        //�����µ�WORDӦ�ó��򲢴�һ���Ѿ����ڵ��ĵ���  
    BOOL SetActiveDocument(short i);    //���õ�ǰ������ĵ���  
  
    //**********************�����ĵ�*********************************************  
    BOOL SaveDocument();                //�ĵ����Դ���ʽ�����档  
    BOOL SaveDocumentAs(CString fileName);//�ĵ��Դ�����ʽ�����档  
    BOOL SaveDocumentAs(CString fileName,WdSaveFormat wdFormat);//�ĵ���ָ����ʽ�����档  
    BOOL ExportDocumentAs(CString fileName,WdExportFormat wdFormat);//�ĵ���ָ����ʽ��������  
    BOOL CloseDocument();  
    void CloseApp();   
  
    //**********************�ı���д����*****************************************  
    void WriteText(CString szText);        //��ǰ��괦д�ı�  
    void WriteNewLineText(CString szText, int nLineCount = 1); //��N��д��  
    void WriteEndLine(CString szText);    //�ĵ���β��д�ı�  
    void WholeStory();                    //ȫѡ�ĵ�����  
    void Copy();                        //�����ı����ݵ�������  
    void InsertFile(CString fileName);    //�����ص��ļ�ȫ������д�뵽��ǰ�ĵ��Ĺ�괦��  
    //----------------------add by zxx--------------------------------------  
    //***********************������********************************************  
    //���°���ѡ��  
    void SelectMoveDown(short lineCount, short unit);//��ѡ��������ƶ�  
    void NoneSelectMoveDown(short lineCount, short unit);//�����ƶ���꣬��ѡ��  
    void SelectMoveUp(short lineCount, short unit);//��ѡ��������ƶ�  
    void NoneSelectMoveUp(short lineCount, short unit);//�����ƶ���꣬��ѡ��  
    //���Ұ���ѡ��  
    void SelectMoveLeft(short charCount, short unit);//��ѡ��������ƶ�  
    void NoneSelectMoveLeft(short charCount, short unit);//  
    void SelectMoveRight(short charCount, short unit);//��ѡ��������ƶ�  
    void NoneSelectMoveRight(short charCount, short unit);//  
  
  
    void MoveToFirst();  
    void MoveToNextPage();  
    void TypeParagraph();  
    void PasteAndFormat();  
    void Paste();  
    void TypeBackspace(int count);  

	//��ӡ�ļ�
	BOOL Print();

private:  
	CApplication m_oApp;  
	CDocuments m_oDocuments;  
	CDocument0 m_oDocument;  
	CSelection m_oSelecton;  
	CRange     m_oRange;

	BOOL m_bInitOk;
};
