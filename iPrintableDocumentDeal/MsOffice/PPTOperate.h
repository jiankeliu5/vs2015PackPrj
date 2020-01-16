#pragma once

#include "PPTHeader.h"

class _AA_DLL_EXPORT_ CPPTOperate
{
public:
	CPPTOperate(void);
	~CPPTOperate(void);

public:
	   //����  
    //**********************�������ĵ�*******************************************  
    BOOL CreateApp();                    //����һ���µ�PPTӦ�ó���  
    BOOL CreatePresentations();          //����һ���µ�PPT�ĵ�����  
    BOOL CreatePresentation();           //����һ���µ�PPT�ĵ�  
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
    BOOL SaveDocumentAs(CString fileName,PpSaveAsFileType ppFormat);//�ĵ���ָ����ʽ�����档  
    BOOL ExportDocumentAs(CString fileName,PpFixedFormatType ppFormat);//�ĵ���ָ����ʽ��������  
    BOOL CloseDocument();  
    void CloseApp();   

	//��ӡ�ļ�
	BOOL Print();

private:
	CApplication1 m_oApp;
	CPresentations m_oPresentations;
	CPresentation m_oPresentation;
	CProtectedViewWindow m_oProtectedViewWindow;
	CProtectedViewWindows m_oProtectedViewWindows;
// 	CSlideShowView m_oSlideShowView;
// 	CSlideShowWindow m_oSlideShowWindow;
// 	CSlideShowSettings m_oSlideShowSettings;
// 	CSlides m_oSlides;
// 	CSlide m_oSlide;
// 	CDocumentWindow m_oDocumentwindow;
// 	CView1 m_oView;

	BOOL m_bInitOk;
};
