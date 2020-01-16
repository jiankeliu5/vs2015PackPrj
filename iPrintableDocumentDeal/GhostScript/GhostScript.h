#pragma once

/************************************************************************/
/* 
	CGhostScript����ܱ����̵߳��ã����Ա���GhostScript���ʱ��
	Ҫ�����ú꣺GS_THREADSAFE�����GhostScript�Ķ��߳�֧�֡�
	���ӿ��г��ֵġ�FilePath��·��Ҫ��ȫ��ʹ��UTF8���롣
*/
/************************************************************************/

class CGhostScript
{
public:
	CGhostScript(void);
	~CGhostScript(void);

public:
	bool RunGsApiWithArgs(int argc, char **argv);

	//��PostScript�ļ�ת����PDF�ļ���
	bool PsToPdf(char* pszPsFilePath, char* pszPdfFilePath);

	//��PDF�ļ�ת����PostScript�ļ���
	bool PdfToPs(char* pszPdfFilePath, char* pszPsFilePath);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡPDF��֧��PostScript��PDF���롣
	bool ExtractToPdf(char* pszFilePath, char* pszPdfFilePath, int nFirstPage = 0, int nLastPage = 0);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡPostScript��֧��PostScript��PDF���롣
	bool ExtractToPs(char* pszFilePath, char* pszPsFilePath, int nFirstPage = 0, int nLastPage = 0);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡTiff��֧��PostScript��PDF���롣
	bool ExtractToTiff(char* pszFilePath, char* pszTiffFilePath, bool bColor = true, int nDpi = 300, int nFirstPage = 0, int nLastPage = 0);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡPNG��֧��PostScript��PDF���롣
	//���ҳ�뷶Χ����1��������ļ��������ǹ̶��ģ���[output.png]��
	//���ҳ�뷶Χ����1ҳ��������ļ�������ʹ��%d�������ҳ��
	//	A:[output_%d.png],���ɴ�output_1.png,output_2.png,output_3.png,...���Ʊ�ŵ��ļ���
	//	B:[output_%03d.png],���ɴ�output_001.png,output_002.png,output_003.png,...���Ʊ�ŵ��ļ���
	bool ExtractToPng(char* pszFilePath, char* pszPngFilePath, bool bColor = true, int nDpi = 300, int nFirstPage = 0, int nLastPage = 0);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡJPEG��֧��PostScript��PDF���롣
	//���ҳ�뷶Χ����1��������ļ��������ǹ̶��ģ���[output.jpg]��
	//���ҳ�뷶Χ����1ҳ��������ļ�������ʹ��%d�������ҳ��
	//	A:[output_%d.jpg],���ɴ�output_1.jpg,output_2.jpg,output_3.jpg,...���Ʊ�ŵ��ļ���
	//	B:[output_%03d.jpg],���ɴ�output_001.jpg,output_002.jpg,output_003.jpg,...���Ʊ�ŵ��ļ���
	bool ExtractToJpeg(char* pszFilePath, char* pszJpegFilePath, bool bColor = true, int nDpi = 300, int nFirstPage = 0, int nLastPage = 0);

	//ָ����ʼ�ͽ���ҳ��(��1��ʼ,0��ʾ����ҳ��)��ȡTxt��֧��PostScript��PDF���룬
	//��PostScriptֻ֧�ֵ�ҳ����֧�ַ�Χ���磺֧��1-1��2-2����֧��0-0��1-2��
	bool ExtractToTxt(char* pszFilePath, char* pszTxtFilePath, int nFirstPage = 0, int nLastPage = 0);
};
