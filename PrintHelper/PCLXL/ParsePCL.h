#pragma once

#include "PCL_Def.h"
#include "Object.h"

namespace ParsePCL
{//�����ռ�ParsePCL��ʼ

class CParsePCL
{
public:
	CParsePCL(void);
	~CParsePCL(void);

	bool OpenFile(const TCHAR* pcszPclFilePath);
	void CloseFile();
	__int64 GetFileLength();	//��ȡ�ļ���С
	__int64 GetPclPostion();	//��ȡPCL���ݵ���ʼλ��
	std::string GetJobName();	//utf-8����
	std::string GetUserName();	//utf-8����
	bool GetPageCount(int& nPageCount, int& nA3PageCount, int& nA4PageCount, int& nOtherPageCount);
	bool GetCopies(int& nCopies);
	bool GetCollate(int& nCollate);
	bool GetDuplex(int& nDuplex);
	bool GetColor(int& nColor);
	bool GetPaper(int& nPaper);
	bool GetOrientation(int& nOrientation);

	bool GetPageDuplexInfo(std::list<Object>& oPageDuplexObjectList);

	bool Parse();

protected:
	/******* ����PJL��ؽӿڣ�begin *******/
	//����PJLָ��
	bool ParsePJL();
	
	//��ȡһ������
	bool ReadLine();

	//��ȡPCLXLЭ����Ϣ
	bool ReadProtocolInfo();

	//ɾ��PJL�ַ�����ǰ����������
	void TrimPJLString(std::string& strString);

	//��ȡ��ҵ��
	bool GetJobNameByPJL();

	//��ȡ�û���
	bool GetUserNameByPJL();

	//��ȡ��ӡ����
	int GetCopiesByPJL();

	//��ȡ˫���ӡ���ز���
	int GetDuplexByPJL();

	//��ȡBINDING����
	int GetBindingByPJL();

	//��ȡ��ӡ���ݷ������
	int GetOrientationByPJL();

	//��ȡ��ɫ��ӡ����
	int GetColorByPJL();
	/******* ����PJL��ؽӿڣ�end *******/


	/******* ����PCL��ؽӿڣ�begin *******/
	//����PCLָ��
	bool ParsePCL();

	//��ȡ��ǰλ�õ�ָ��
	bool GetCmd(UByte& ubCmd);

	//��ȡ��ֵ
	bool GetDataValue(UByte& nValue);
	bool GetDataValue(UInt16& nValue);
	bool GetDataValue(UInt32& nValue);
	bool GetDataValue(SInt16& nValue);
	bool GetDataValue(SInt32& nValue);
	bool GetDataValue(Real32& nValue);

	//���ļ�λ�ôӵ�ǰλ������ָ������(nSkipLength)��������
	bool SkipData(__int64 nSkipLength);

	//��ȡ�ļ��ӵ�ǰλ�������ָ������(nDataLength)������
	bool ReadData(UByte* pData, size_t nDataLength);

	//����������ж�
	bool IsDataTag(UByte ubCmd);
	bool IsAttributeId(UByte ubCmd);
	bool IsOperator(UByte ubCmd);
	bool IsEmbeddedData(UByte ubCmd);

	bool GetData(UByte ubCmd);
	bool GetAttribute(UByte ubCmd);
	bool GetOperator(UByte ubCmd);
	bool GetEmbeddedData(UByte ubCmd);

	//��ȡ�������ݵ���Ϣ
	bool GetArrayDataInfo(UByte ubCmd, UByte& ubDataType, UInt32& nDataElements, UInt32& nDataLength);

	//�ж�ϵͳ���Ƿ�Ϊ����ֽ���
	bool IsBigEndianSystem();

	//��ս���PCLʱ���������ʱ����ջ
	void ClearParseStack();

	//ʵ�ֲ���Operator
	void BeginSession();
	void EndSession();
	void BeginPage();
	void EndPage();
	void SetColorSpace();
	/******* ����PCL��ؽӿڣ�end *******/

protected:
	TCHAR m_szPclFilePath[300];
	__int64 m_nFileLength;
	__int64 m_nFilePos;
	__int64 m_nPclPos;
	FILE* m_pFile;
	char* m_pDataBufer;
	int m_nDataBuferLength;

	//PCLXLЭ����Ϣ
	char m_szProtocol[32];
	int m_nMainProtocol;
	int m_nSubProtocol;

	//��ʶϵͳ�Ƿ�Ϊ����ֽ���
	bool m_bIsBigEndianSystem;

	//��ҵ��Ϣ
	bool m_bHaveGetJobName;
	std::string m_strJobName;	//��ҵ����utf-8����
	bool m_bHaveGetUserName;
	std::string m_strUserName;	//�û�����utf-8����
	bool m_bHaveGetPageCount;
	int m_nPageCount;	//����ҳ��
	int m_nA3PageCount;
	int m_nA4PageCount;
	int m_nOtherPageCount;
	bool m_bHaveGetCopies;
	int m_nCopies;		//��ӡ����
	bool m_bHaveGetCollate;
	int m_nCollate;		//��ݴ�ӡ��0-DMCOLLATE_FALSE������ݴ�ӡ����1-DMCOLLATE_TRUE����ݴ�ӡ��
	bool m_bHaveGetOrientation;
	enum ORIENTATION { PORTRAIT, LANDSCAPE };	//ֽ�����ݷ���PORTRAIT-����LANDSCAPE-����
	int m_oOrientation;	//ֽ�����ݵķ���
	bool m_bHaveGetBinding;	//˫��ĳ��߻��߶̱�
	enum BINDING { LONGEDGE, SHORTEDGE, LEFT, RIGHT, TOP };	//���ߣ�LONGEDGE���̱ߣ�SHORTEDGE��LEFT��RIGHT��ʾ���ߣ�TOP��ʾ���
	BINDING m_oBinding;	//װ��ѡ��
	bool m_bHaveGetDuplex;
	bool m_bDuplex;		//˫�濪��
	bool m_bHaveGetPageMode;
	enum PageMode { Simplex, DuplexVertical, DuplexHorizontal };	//Simplex-���棬DuplexVertical-˫�����ߣ�DuplexHorizontal-˫����
	PageMode m_oPageMode;	//ҳ���ӡģʽ
	bool m_bHaveGetColor;
	int m_nColor;		//��ɫ��ӡ��1-DMCOLOR_MONOCHROME, 2-DMCOLOR_COLOR
	bool m_bHaveGetPaper;
	int m_nPaper;		//ֽ�ͣ�8-DMPAPER_A3��9-DMPAPER_A4


	//����PCLʱ���������ʱ����ջ
	std::stack<Object> m_oParseStack;

	//����ÿҳ��˫���ӡ�������ã��ر�˵��������ÿҳ����˫���ӡ����
	std::list<Object> m_oPageDuplexObjectList;
};

}//�����ռ�ParsePCL����
