#ifndef _Cloud_Doc_Def_H_D82B4162_CC81_47e0_A799_EB3B16D3405C_
#define _Cloud_Doc_Def_H_D82B4162_CC81_47e0_A799_EB3B16D3405C_

typedef struct tag_CloudJobInfo
{
	int nJobId;
	int nJobState;
	int nFileState;
	int nFileDeleted;
	int nDocSize;				//���ĵ�ԭʼ�ĵ���С����test.doc
	TCHAR cDocName[MAX_PATH];	//���ĵ�ԭʼ�ĵ�������test.doc
	int nPaperSize;
	TCHAR cPageType[50];
	int nOrientation;
	int nCopies;
	int nColor;
	int nDoublePrint;
	int nPaperSource;
	int nPageCount;				//�ĵ�ҳ��
	int nTotalPage;
	int nOrgJobSize;			//���ĵ�����ǰ���ĵ���С��������gzѹ������������δѹ���ģ���test.doc.gz����test.doc
	TCHAR cOrgJobName[MAX_PATH];	//���ĵ�����ǰ���ĵ�����������gzѹ������������δѹ���ģ���test.doc.gz����test.doc
	int nOrgJobIsGz;			//���ĵ�����ǰ���ĵ������Ƿ�Ϊgzѹ����
	int nNewJobSize;			//���ĵ��������ĵ���С��������gzѹ������������δѹ���ģ���test.doc.tif.gz����test.doc.tif
	TCHAR cNewJobName[MAX_PATH];	//���ĵ��������ĵ�����������gzѹ������������δѹ���ģ���test.doc.tif.gz����test.doc.tif������ȡ�ļ���������ļ�Ϊ׼
	int nNewJobIsGz;			//���ĵ��������ĵ������Ƿ���gzѹ����
	TCHAR cJobDir[MAX_PATH];		//���ĵ�����·�����������ļ���
	int nUserId;
	TCHAR cAccount[100];
	double fSubmitTime;
	int nFileStorageId;			//���ĵ����յĴ洢������
	TCHAR cIP[50];				//���ĵ�����ԴIP
	TCHAR cEnSerialNumber[50];
	int nOcrState;
	int nAuditTacticsId;
}CloudJobInfo, *PCloudJobInfo;

typedef CList<PCloudJobInfo, PCloudJobInfo> CListCloudJobInfo;

typedef enum CloudPrintType {CPT_Normal,CPT_Cloud} Cloud_Type;


//����״̬
enum PROCESS_STATE {
	PROCESS_STATE_NOT_EXIST = 0,		//������
	PROCESS_STATE_RUNING = 1			//������
};

//���ĵ����������Ϣ
typedef struct tag_CloudJobDealProcessInfo
{
	PROCESSENTRY32 oProcessEntry32;		//������Ϣ
	PROCESS_STATE oProcessState;	//����״̬
	double fProcessBeginTime;	//���̿�ʼʱ��
	char cJobPath[MAX_PATH];	//��ҵ·��
}CloudJobDealProcessInfo, *PCloudJobDealProcessInfo;

//���ڽ�����Ϣ
typedef struct tag_WindowProcessInfo
{
	HWND hWnd;			//���ھ��
	DWORD dwProcessID;	//����ID
	DWORD dwThreadID;	//�߳�ID
	TCHAR cClassName[MAX_PATH];	//��������
	TCHAR cWindowText[MAX_PATH];	//���ڱ���
}WindowProcessInfo, *PWindowProcessInfo;

#define MS_Office_Word			TEXT("WINWORD.EXE")
#define MS_Office_PowerPoint	TEXT("POWERPNT.EXE")
#define MS_Office_Excel			TEXT("EXCEL.EXE")
#define MS_Office_ProcessExe	TEXT("WINWORD.EXE;POWERPNT.EXE;EXCEL.EXE")		//΢��Office�칫����Ľ�����
#define MS_Office_ErrorExe		TEXT("DWWIN.EXE;WERFAULT.EXE")	//΢��Office�칫����������ʱ�Ĵ��������

typedef struct tag_FtpInfo
{
	TCHAR szFtpIP[50];		//ftp��������IP
	int nFtpPort;			//ftp�Ķ˿ں�
	TCHAR szFtpUser[50];		//ftp�ĵ�¼�û���
	TCHAR szFtpPwd[100];		//ftp�ĵ�¼����
	TCHAR szFtpDir[300];		//ftpĬ��Ŀ¼
	TCHAR szFtpDiskDir[300];	//ftpĬ��Ŀ¼ָ���ftp��������Ӳ��Ŀ¼�����ftp���������ⲿ������˵û�����塣
}FtpInfo, *PFtpInfo;

#define Db_Name_Normal TEXT("iSec_Print56")	//��ͨ��ӡ���ƴ�ӡ���ݿ�������У԰�����ҵ���
#define Db_Name_Cloud TEXT("iSecCloud")		//�����ƴ�ӡ���ƴ�ӡ���ݿ���

#define Conn_DataBase_TimeOut_MS	(1000 * 30)		//�������ݿ��ʱ����,����
#define Check_Cloud_Doc_TimeOut_MS	(1000 * 5)		//���������Ҫ�������ĵ���ʱ����������
#define Check_Cloud_Doc_Deal_TimeOut_MS	(1000 * 1)	//�����һ����Ҫ�������ĵ���ʱ����������
#define Check_Cloud_Config_Change_TimeOut_MS	(1000 * 60 * 30 * 1)		//��⴦�����ĵ����Ը��ĵ�ʱ����������
#define Check_Cloud_Db_Conn_TimeOut_MS	(1000 * 60 * 8)		//����Ʒ��������ݿ�����״̬��ʱ����������

#define Max_Wait_Cloud_Doc_Deal_Count	(1000)		//�ȴ�������ƴ�ӡ�ĵ����е�������
#define Min_Cloud_Doc_Deal_Thread_Count	1			//���ĵ�������С�߳���

//���������ĵ��Ĳ���
#define Check_Blocked_Cloud_Doc_TimeOut_MS	(1000 * 10)		//�����Ҫ�������������ĵ���ʱ����������
#define Max_Wait_Blocked_Cloud_Doc_Time_S	(60 * 6)		//���������ĵ������ʱ�䣬��

//����ɨ���ĵ��Ĳ���
#define Check_Scan_Doc_TimeOut_MS		(1000 * 60)	//���������Ҫ����ɨ���ĵ���ʱ����������
#define Check_Scan_Doc_Deal_TimeOut_MS	(1000 * 1)	//�����һ����Ҫ�������ĵ���ʱ����������
#define Min_Scan_Doc_Deal_Thread_Count	2			//ɨ���ĵ�������С�߳���
#define Max_Wait_Scan_Doc_Deal_Count	(100)		//�ȴ������ɨ���ĵ����е�������

//��������ĵ��Ĳ���
#define Check_OutDate_Doc_TimeOut_MS		(1000 * 60)	//���������Ҫ��������ĵ���ʱ����������

//�����ɴ�ӡ�ĵ���ʽ��ʶ��
#define Cloud_Doc_Format_UnKnow		0		//δ֪�ɴ�ӡ�ĵ�
#define Cloud_Doc_Format_Word		1		//office word
#define Cloud_Doc_Format_PowerPoint	2		//office PowerPoint
#define Cloud_Doc_Format_Excel		3		//office Excel
#define Cloud_Doc_Format_PDF		4		//Adobe Portable Document Format (PDF)
#define Cloud_Doc_Format_XPS		5		//Microsoft XML Paper Specification (XPS)
#define Cloud_Doc_Format_WMF		6		//Windows Metafile Format (WMF)
#define Cloud_Doc_Format_EMF		7		//Windows Enhanced MetaFile format (EMF)
#define Cloud_Doc_Format_PS			8		//Adobe PostScript (PS)
#define Cloud_Doc_Format_PCL		9		//Printer Control Language (PCL)
#define Cloud_Doc_Format_SPL		10		//Microsoft? Windows Spool File Format (SPL) (������pcl/ps/emf/xps�ȸ�ʽ)
#define Cloud_Doc_Format_JPEG		11		//Joint Photographic Experts Group������ͼ��ר��С��/JPEG��
#define Cloud_Doc_Format_TIFF		12		//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF��
#define Cloud_Doc_Format_PNG		13		//Portable Network Graphic Format (����ֲ����ͼ�θ�ʽ/PNG)
#define Cloud_Doc_Format_BMP		14		//Bitmap (λͼ)
#define Cloud_Doc_Format_GIF		15		//Graphics Interchange Format (ͼ�񻥻���ʽ/GIF)

//�����ɴ�ӡ�ĵ���ʽ����
#define Cloud_Doc_Format_Name_UnKnow				TEXT("δ֪��ʽ")
#define Cloud_Doc_Format_Name_Word					TEXT("Word �ĵ� (*.doc,*.docx)")
#define Cloud_Doc_Format_Name_PowerPoint			TEXT("PowerPoint ��ʾ�ĸ� (*.ppt,*.pptx)")
#define Cloud_Doc_Format_Name_Excel					TEXT("Excel ������ (*.xls,*.xlsx)")
#define Cloud_Doc_Format_Name_PDF					TEXT("PDF (*.pdf)")
#define Cloud_Doc_Format_Name_XPS					TEXT("XPS �ĵ� (*.xps)")
#define Cloud_Doc_Format_Name_WMF					TEXT("Windows Metafile Format (*.wmf)")
#define Cloud_Doc_Format_Name_EMF					TEXT("Windows Enhanced MetaFile format (*.emf)")
#define Cloud_Doc_Format_Name_PS					TEXT("Adobe PostScript (*.ps)")
#define Cloud_Doc_Format_Name_PCL					TEXT("Printer Control Language (*.pcl)")
#define Cloud_Doc_Format_Name_SPL					TEXT("Windows Spool File Format (*.spl)")
#define Cloud_Doc_Format_Name_JPEG					TEXT("JPEG (*.jpg,*.jpeg)")
#define Cloud_Doc_Format_Name_TIFF					TEXT("TIFF (*.tif,*.tiff)")
#define Cloud_Doc_Format_Name_PNG					TEXT("PNG (*.png)")
#define Cloud_Doc_Format_Name_BMP					TEXT("Bitmap (*.bmp)")
#define Cloud_Doc_Format_Name_GIF					TEXT("GIF (*.gif)")

//�����ɴ�ӡ�ĵ���ʽ
#define Cloud_Doc_Format_Suffix_Word				TEXT(".doc")		//office word 97-2003
#define Cloud_Doc_Format_Suffix_Word_GZ				TEXT(".doc.gz")		//office word 97-2003 ѹ����
#define Cloud_Doc_Format_Suffix_WordX				TEXT(".docx")		//office word 2007-later
#define Cloud_Doc_Format_Suffix_WordX_GZ			TEXT(".docx.gz")	//office word 2007-later ѹ����
#define Cloud_Doc_Format_Suffix_PowerPoint			TEXT(".ppt")		//office PowerPoint 97-2003
#define Cloud_Doc_Format_Suffix_PowerPoint_GZ		TEXT(".ppt.gz")		//office PowerPoint 97-2003 ѹ����
#define Cloud_Doc_Format_Suffix_PowerPointX			TEXT(".pptx")		//office PowerPoint 2007-later
#define Cloud_Doc_Format_Suffix_PowerPointX_GZ		TEXT(".pptx.gz")	//office PowerPoint 2007-later ѹ����
#define Cloud_Doc_Format_Suffix_Excel				TEXT(".xls")		//office Excel 97-2003
#define Cloud_Doc_Format_Suffix_Excel_GZ			TEXT(".xls.gz")		//office Excel 97-2003 ѹ����
#define Cloud_Doc_Format_Suffix_ExcelX				TEXT(".xlsx")		//office Excel 2007-later
#define Cloud_Doc_Format_Suffix_ExcelX_GZ			TEXT(".xlsx.gz")	//office Excel 2007-later ѹ����
#define Cloud_Doc_Format_Suffix_PDF					TEXT(".pdf")		//Adobe Portable Document Format (PDF)
#define Cloud_Doc_Format_Suffix_PDF_GZ				TEXT(".pdf.gz")		//Adobe Portable Document Format (PDF) ѹ����
#define Cloud_Doc_Format_Suffix_XPS					TEXT(".xps")		//Microsoft XML Paper Specification (XPS)
#define Cloud_Doc_Format_Suffix_XPS_GZ				TEXT(".xps.gz")		//Microsoft XML Paper Specification (XPS) ѹ����
#define Cloud_Doc_Format_Suffix_WMF					TEXT(".wmf")		//Windows Metafile Format (WMF)
#define Cloud_Doc_Format_Suffix_WMF_GZ				TEXT(".wmf.gz")		//Windows Metafile Format (WMF) ѹ����
#define Cloud_Doc_Format_Suffix_EMF					TEXT(".emf")		//Windows Enhanced MetaFile format (EMF)
#define Cloud_Doc_Format_Suffix_EMF_GZ				TEXT(".emf.gz")		//Windows Enhanced MetaFile format (EMF) ѹ����
#define Cloud_Doc_Format_Suffix_PS					TEXT(".ps")			//Adobe PostScript (PS)
#define Cloud_Doc_Format_Suffix_PS_GZ				TEXT(".ps.gz")		//Adobe PostScript (PS) ѹ����
#define Cloud_Doc_Format_Suffix_PCL					TEXT(".pcl")		//Printer Control Language (PCL)
#define Cloud_Doc_Format_Suffix_PCL_GZ				TEXT(".pcl.gz")		//Printer Control Language (PCL) ѹ����
#define Cloud_Doc_Format_Suffix_SPL					TEXT(".spl")		//Microsoft? Windows Spool File Format (SPL) (������pcl/ps/emf/xps�ȸ�ʽ)
#define Cloud_Doc_Format_Suffix_SPL_GZ				TEXT(".spl.gz")		//Microsoft? Windows Spool File Format (SPL) (������pcl/ps/emf/xps�ȸ�ʽ) ѹ����
#define Cloud_Doc_Format_Suffix_JPG					TEXT(".jpg")		//Joint Photographic Experts Group������ͼ��ר��С��/JPEG��
#define Cloud_Doc_Format_Suffix_JPG_GZ				TEXT(".jpg.gz")		//Joint Photographic Experts Group������ͼ��ר��С��/JPEG�� ѹ����
#define Cloud_Doc_Format_Suffix_JPEG				TEXT(".jpeg")		//Joint Photographic Experts Group������ͼ��ר��С��/JPEG��
#define Cloud_Doc_Format_Suffix_JPEG_GZ				TEXT(".jpeg.gz")	//Joint Photographic Experts Group������ͼ��ר��С��/JPEG�� ѹ����
#define Cloud_Doc_Format_Suffix_TIF					TEXT(".tif")		//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF��
#define Cloud_Doc_Format_Suffix_TIF_GZ				TEXT(".tif.gz")		//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF�� ѹ����
#define Cloud_Doc_Format_Suffix_TIFF				TEXT(".tiff")		//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF��
#define Cloud_Doc_Format_Suffix_TIFF_GZ				TEXT(".tiff.gz")	//Tagged Image File Format����ǩͼ���ļ���ʽ/TIFF�� ѹ����
#define Cloud_Doc_Format_Suffix_PNG					TEXT(".png")		//Portable Network Graphic Format (����ֲ����ͼ�θ�ʽ/PNG)
#define Cloud_Doc_Format_Suffix_PNG_GZ				TEXT(".png.gz")		//Portable Network Graphic Format (����ֲ����ͼ�θ�ʽ/PNG) ѹ����
#define Cloud_Doc_Format_Suffix_BMP					TEXT(".bmp")		//Bitmap (λͼ)
#define Cloud_Doc_Format_Suffix_BMP_GZ				TEXT(".bmp.gz")		//Bitmap (λͼ) ѹ����
#define Cloud_Doc_Format_Suffix_GIF					TEXT(".gif")		//Graphics Interchange Format (ͼ�񻥻���ʽ/GIF)
#define Cloud_Doc_Format_Suffix_GIF_GZ				TEXT(".gif.gz")		//Graphics Interchange Format (ͼ�񻥻���ʽ/GIF) ѹ����

#define Cloud_Doc_Format_Suffix_GZ					TEXT(".gz")			//���ύ�ĵ���ѹ����ʽ�ĺ�׺

#define Cloud_Doc_Path_Slash_Windows_Double			TEXT("\\\\")		//Windws�ĵ�·����˫��б�ܡ�\\��
#define Cloud_Doc_Path_Slash_Linux_Double			TEXT("//")			//Linux�ĵ�·����˫б�ܡ�//��
#define Cloud_Doc_Path_Slash_Windows				TEXT("\\")			//Windws�ĵ�·���ķ�б�ܡ�\��
#define Cloud_Doc_Path_Slash_Linux					TEXT("/")			//Linux�ĵ�·����б�ܡ�/��
#ifdef WIN32
#define Cloud_Doc_Path_Slash_Current			Cloud_Doc_Path_Slash_Windows	//��ǰϵͳҪ��ʹ���ĵ�·����б��		
#define Cloud_Doc_Path_Slash_Current_Reverse	Cloud_Doc_Path_Slash_Linux		//��ǰϵͳҪ��ʹ���ĵ�·���ķ���б��		
#else
#define Cloud_Doc_Path_Slash_Current			Cloud_Doc_Path_Slash_Linux		//��ǰϵͳҪ��ʹ���ĵ�·����б��		
#define Cloud_Doc_Path_Slash_Current_Reverse	Cloud_Doc_Path_Slash_Windows	//��ǰϵͳҪ��ʹ���ĵ�·���ķ���б��		
#endif

#endif //_Cloud_Doc_Def_H_D82B4162_CC81_47e0_A799_EB3B16D3405C_
