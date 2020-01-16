#include "StdAfx.h"
#include "ParsePCL.h"

// using namespace ParsePCL;
namespace ParsePCL
{//命名空间ParsePCL开始

CParsePCL::CParsePCL(void)
{
	memset(m_szPclFilePath, 0x0, sizeof(m_szPclFilePath));
	m_nFileLength = 0;
	m_nFilePos = 0;
	m_nPclPos = 0;
	m_pFile = NULL;
	m_nDataBuferLength = 1024;
	m_pDataBufer = new char[m_nDataBuferLength];

	memset(m_szProtocol, 0x0, sizeof(m_szProtocol));
	m_nMainProtocol = 0;
	m_nSubProtocol = 0;

	m_bIsBigEndianSystem = IsBigEndianSystem();

	m_bHaveGetJobName = false;
	m_strJobName = "";
	m_bHaveGetUserName = false;
	m_strUserName = "";
	m_bHaveGetPageCount = false;
	m_nPageCount = 0;
	m_nA3PageCount = 0;
	m_nA4PageCount = 0;
	m_nOtherPageCount = 0;
	m_bHaveGetCopies = false;
	m_nCopies = 1;
	m_bHaveGetCollate = false;
	m_nCollate = DMCOLLATE_TRUE;	//默认逐份打印
	m_bHaveGetOrientation = false;
	m_oOrientation = PORTRAIT;	//默认竖向
	m_bHaveGetBinding = false;
	m_oBinding = LONGEDGE;		//默认长边
	m_bHaveGetDuplex = false;
	m_bDuplex = false;	//默认关闭双面，即单面打印
	m_bHaveGetPageMode = false;
	m_oPageMode = Simplex;	//默认单面
	m_bHaveGetColor = false;
	m_nColor = DMCOLOR_MONOCHROME;	//默认黑白打印
	m_bHaveGetPaper = false;
	m_nPaper = DMPAPER_A4;
}

CParsePCL::~CParsePCL(void)
{
	CloseFile();
	if (m_pDataBufer)
	{
		delete[] m_pDataBufer;
		m_pDataBufer = NULL;
	}

	ClearParseStack();

	//清空
	m_oPageDuplexObjectList.clear();
}

bool CParsePCL::OpenFile(const TCHAR* pcszPclFilePath)
{
	theLog.Write(_T("CParsePCL::OpenFile,1,pcszPclFilePath=%s"), pcszPclFilePath);

	CloseFile();

	_tcscpy_s(m_szPclFilePath, pcszPclFilePath);

	//打开文件
	errno_t nError = _tfopen_s(&m_pFile, m_szPclFilePath, _T("rb"));
	if (!m_pFile)
	{
		theLog.Write(_T("!!CParsePCL::OpenFile,2,_tfopen fail,m_szPclFilePath=%s,nError=%d"), m_szPclFilePath, nError);
		return false;		
	}

	//获取文件大小
	_fseeki64(m_pFile, 0, SEEK_END);
	m_nFileLength = _ftelli64(m_pFile);
	if (m_nFileLength <= 0)
	{
		theLog.Write(_T("!!CParsePCL::OpenFile,2,_ftelli64 fail,m_nFileLength=%I64d,errno=%d"), m_nFileLength, errno);
		CloseFile();
		return false;		
	}

	//设置文件初始位置
	_fseeki64(m_pFile, 0, SEEK_SET);
	m_nFilePos = 0;

	return true;
}

void CParsePCL::CloseFile()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	m_nFileLength = 0;
	m_nFilePos = 0;
	memset(m_szPclFilePath, 0x0, sizeof(m_szPclFilePath));
	m_nPageCount = 0;
	m_nCopies = 1;
}

__int64 CParsePCL::GetFileLength()
{
	return m_nFileLength;
}

__int64 CParsePCL::GetPclPostion()
{
	return m_nPclPos;
}

std::string CParsePCL::GetJobName()
{
	return m_strJobName;
}

std::string CParsePCL::GetUserName()
{
	return m_strUserName;
}

bool CParsePCL::GetPageCount(int& nPageCount, int& nA3PageCount, int& nA4PageCount, int& nOtherPageCount)
{
	if (m_bHaveGetPageCount)
	{
		nPageCount = m_nPageCount;
		nA3PageCount = m_nA3PageCount;
		nA4PageCount = m_nA4PageCount;
		nOtherPageCount = m_nOtherPageCount;
	}
	else
	{
		nPageCount = 0;
		nA3PageCount = 0;
		nA4PageCount = 0;
		nOtherPageCount = 0;
	}
	return m_bHaveGetPageCount;
}

bool CParsePCL::GetCopies(int& nCopies)
{
	if (m_bHaveGetCopies)
	{
		nCopies = m_nCopies;
	}
	else
	{
		nCopies = 1;
	}
	return m_bHaveGetCopies;
}

bool CParsePCL::GetCollate(int& nCollate)
{
	if (m_bHaveGetCollate)
	{
		nCollate = m_nCollate;
	}
	else
	{
		nCollate = DMCOLLATE_TRUE;
	}
	return m_bHaveGetCollate;
}

bool CParsePCL::GetDuplex(int& nDuplex)
{
	if (m_bHaveGetDuplex && !m_bDuplex)
	{
		nDuplex = DMDUP_SIMPLEX;
	}
	else if (m_bHaveGetDuplex && m_bDuplex)
	{
		if (m_oBinding == LONGEDGE)
		{
			nDuplex = DMDUP_VERTICAL;	//双面长边
		}
		else if (m_oBinding == SHORTEDGE)
		{
			nDuplex = DMDUP_HORIZONTAL;	//双面短边
		}
		else
		{
			if (m_oBinding == LEFT || m_oBinding == RIGHT)
			{
				if (m_oOrientation == PORTRAIT)
				{
					nDuplex = DMDUP_VERTICAL;	//双面长边
				}
				else
				{
					nDuplex = DMDUP_HORIZONTAL;	//双面短边
				}
			}
			else
			{
				if (m_oOrientation == PORTRAIT)
				{
					nDuplex = DMDUP_HORIZONTAL;	//双面短边
				}
				else
				{
					nDuplex = DMDUP_VERTICAL;	//双面长边
				}
			}
		}
	}
	else if (m_bHaveGetPageMode)
	{
		if (m_oPageMode == Simplex)
		{
			nDuplex = DMDUP_SIMPLEX;
		}
		else if (m_oPageMode == DuplexVertical)
		{
			if (m_oOrientation == PORTRAIT)
			{
				nDuplex = DMDUP_VERTICAL;	//双面长边
			}
			else
			{
				nDuplex = DMDUP_HORIZONTAL;	//双面短边
			}
		}
		else
		{
			if (m_oOrientation == PORTRAIT)
			{
				nDuplex = DMDUP_HORIZONTAL;	//双面短边
			}
			else
			{
				nDuplex = DMDUP_VERTICAL;	//双面长边
			}
		}
	}

	return (m_bHaveGetDuplex || m_bHaveGetPageMode);
}

bool CParsePCL::GetColor(int& nColor)
{
	if (m_bHaveGetColor)
	{
		nColor = m_nColor;
	}
	else
	{
		nColor = DMCOLOR_MONOCHROME;
	}
	return m_bHaveGetColor;
}

bool CParsePCL::GetPaper(int& nPaper)
{
	if (m_bHaveGetPaper)
	{
		nPaper = m_nPaper;
	}
	else
	{
		nPaper = DMPAPER_A4;
	}
	return m_bHaveGetPaper;
}

bool CParsePCL::GetOrientation(int& nOrientation)
{
	if (m_bHaveGetOrientation)
	{
		if (m_oOrientation == PORTRAIT)
		{
			nOrientation = DMORIENT_PORTRAIT;
		}
		else
		{
			nOrientation = DMORIENT_LANDSCAPE;
		}
	}
	else
	{
		nOrientation = DMORIENT_PORTRAIT;
	}
	return m_bHaveGetOrientation;
}

bool CParsePCL::GetPageDuplexInfo(std::list<Object>& oPageDuplexObjectList)
{
	oPageDuplexObjectList = m_oPageDuplexObjectList;
	return true;
}

bool CParsePCL::Parse()
{
	theLog.Write(_T("CParsePCL::Parse,1,begin"));

	bool bSucc = false;

	//移到文件首部
	_fseeki64(m_pFile, 0, SEEK_SET);

	do 
	{
		//解析PJL
		if (!ParsePJL())
		{
			theLog.Write(_T("!!CParsePCL::Parse,2,ParsePJL fail"));
			break;
		}

		//解析PCL
		if (!ParsePCL())
		{
			theLog.Write(_T("!!CParsePCL::Parse,3,ParsePCL fail"));
			break;
		}

		bSucc = true;
	} while (false);

	theLog.Write(_T("CParsePCL::Parse,100,end"));

	return bSucc;
}

bool CParsePCL::ParsePJL()
{
	bool bSucc = true;
	bool bPJLEnd = false;

	//限制最大读取行数为200行。
	int nReadLine = 0;
	while (!bPJLEnd && ReadLine() && (nReadLine < 200))
	{
		nReadLine++;
		UByte ubData = m_pDataBufer[0];
		switch (ubData)
		{
		case PCL_ESC:	//ESC
			{
				//theLog.Write(_T("%S"), m_pDataBufer);
			}
			break;
		case PCL_At:	//@
			{
				//theLog.Write(_T("%S"), m_pDataBufer);
				GetJobNameByPJL();
				GetUserNameByPJL();
				GetCopiesByPJL();
				GetOrientationByPJL();
				GetBindingByPJL();
				GetDuplexByPJL();
				GetColorByPJL();
			}
			break;
		case PCL_RigthBracket:	//)
			{
				//theLog.Write(_T("%S"), m_pDataBufer);
				ReadProtocolInfo();
				m_nPclPos = m_nFilePos;	//记录PJL的结束位置，即PCL的起始位置
				bPJLEnd = true;
			}
			break;
		default:
			{
				//theLog.Write(_T("!!CParsePCL::ParsePJL,%S"), m_pDataBufer);
				bSucc = false;
			}
			break;
		}
	}

	return bSucc && bPJLEnd;
}

bool CParsePCL::ReadLine()
{
	memset(m_pDataBufer, 0x0, m_nDataBuferLength);
	if (fgets(m_pDataBufer, m_nDataBuferLength, m_pFile))
	{
		m_nFilePos = _ftelli64(m_pFile);
		return true;
	}
	else
	{
		//theLog.Write(_T("!!CParsePCL::ReadLine,ferror=%d,feof=%d"), ferror(m_pFile), feof(m_pFile));
		return false;
	}	
}

bool CParsePCL::ReadProtocolInfo()
{
	//一般为此格式：
	//) HP-PCL XL;3;0;Comment Copyright(c) 1999 Microsoft Corporation
	if ((m_pDataBufer[0] == PCL_RigthBracket) && (m_pDataBufer[1] == PCL_Space))
	{
		char* pSource = m_pDataBufer + 2;
		char* pTemp = strstr(pSource, ";");
		if (pTemp)
		{
			pTemp[0] = '\0';
			strcpy_s(m_szProtocol, pSource);
			pSource = pTemp + 1;

			pTemp = strstr(pSource, ";");
			if (pTemp)
			{
				pTemp[0] = '\0';
				m_nMainProtocol = atoi(pSource);
				pSource = pTemp + 1;

				pTemp = strstr(pSource, ";");
				if (pTemp)
				{
					pTemp[0] = '\0';
					m_nSubProtocol = atoi(pSource);
				}
			}
		}

		//theLog.Write(_T("CParsePCL::ReadProtocolInfo,%S %d.%d"), m_szProtocol, m_nMainProtocol, m_nSubProtocol);
		return true;
	}
	else
	{
		theLog.Write(_T("!!CParsePCL::ReadProtocolInfo,%s"), m_pDataBufer);
		return false;
	}
}

void CParsePCL::TrimPJLString(std::string& strString)
{
	//一般可能的格式为【 "test.doc" \r\n】

	//去除开头的单引号
	strString.erase(0, strString.find_first_not_of(" "));
	strString.erase(0, strString.find_first_not_of("\""));

	//去除结尾的单引号
	strString.erase(strString.find_last_not_of("\n") + 1);
	strString.erase(strString.find_last_not_of("\r") + 1);
	strString.erase(strString.find_last_not_of(" ") + 1);
	strString.erase(strString.find_last_not_of("\"") + 1);
}

bool CParsePCL::GetJobNameByPJL()
{
	if (m_bHaveGetJobName)
	{
		return true;
	}

	//一般格式为【@PJL SET JOBNAME="Microsoft Word - testG.doc"】
	std::string strTemp(m_pDataBufer);
	size_t nPos = strTemp.find("@PJL SET JOBNAME");
	if (nPos != std::string::npos)
	{
		m_bHaveGetJobName = true;
		nPos = strTemp.rfind("=");
		if (nPos != std::string::npos)
		{
			m_strJobName = strTemp.substr(nPos+1);
			TrimPJLString(m_strJobName);
		}
		//theLog.Write(_T("CParsePCL::GetCopiesByPJL,m_strJobName=[%s]"), CCommonFun::UTF8ToUnicode(m_strJobName.c_str()));
	}

	return m_bHaveGetJobName;
}

bool CParsePCL::GetUserNameByPJL()
{
	if (m_bHaveGetUserName)
	{
		return true;
	}

	//一般格式为【@PJL SET USERNAME="Administrator"】
	std::string strTemp(m_pDataBufer);
	size_t nPos = strTemp.find("@PJL SET USERNAME");
	if (nPos != std::string::npos)
	{
		m_bHaveGetUserName = true;
		nPos = strTemp.rfind("=");
		if (nPos != std::string::npos)
		{
			m_strUserName = strTemp.substr(nPos+1);
			TrimPJLString(m_strUserName);
		}
		//theLog.Write(_T("CParsePCL::GetCopiesByPJL,m_strUserName=[%s]"), CCommonFun::UTF8ToUnicode(m_strUserName.c_str()));
	}

	return m_bHaveGetUserName;
}

int CParsePCL::GetCopiesByPJL()
{
	//有些打印驱动生成的作业即有COPIES，又有QTY两个参数
	//则以最后一个出现的为准，所以每个PJL命令都要尝试识别。
	/*
	if (m_bHaveGetCopies)
	{
		return m_nCopies;
	}
	*/

	//一般格式为逐份打印【@PJL SET QTY =1】和【@PJL SET QTY = 1】两种
	//和非逐份打印【@PJL SET COPIES =1】和【@PJL SET COPIES = 1】两种
	std::string strTemp(m_pDataBufer);
	size_t nPos = strTemp.find("@PJL SET QTY");
	if (nPos != std::string::npos)
	{
		nPos = strTemp.rfind("=");
		if (nPos != std::string::npos)
		{
			std::string strTemp2 = strTemp.substr(nPos+1);
			m_nCopies = atoi(strTemp2.c_str());
			m_bHaveGetCopies = true;
			m_nCollate = DMCOLLATE_TRUE;
			m_bHaveGetCollate = true;
			theLog.Write(_T("CParsePCL::GetCopiesByPJL,m_nCopies=%d,m_nCollate=%d"), m_nCopies, m_nCollate);
		}
	}
	else
	{
		nPos = strTemp.find("@PJL SET COPIES");
		if (nPos != std::string::npos)
		{
			nPos = strTemp.rfind("=");
			if (nPos != std::string::npos)
			{
				std::string strTemp2 = strTemp.substr(nPos+1);
				m_nCopies = atoi(strTemp2.c_str());
				m_bHaveGetCopies = true;
				m_nCollate = DMCOLLATE_FALSE;
				m_bHaveGetCollate = true;
				theLog.Write(_T("CParsePCL::GetCopiesByPJL,m_nCopies=%d,m_nCollate=%d"), m_nCopies, m_nCollate);
			}
		}
	}

	return m_nCopies;
}

int CParsePCL::GetDuplexByPJL()
{
	if (m_bHaveGetDuplex)
	{
		return m_bDuplex;
	}

	std::string strTemp(m_pDataBufer);

	//判断是否开启双面打印
	//一般格式为【@PJL SET DUPLEX = OFF】或者【@PJL SET DUPLEX = ON】两种
	size_t nPos = strTemp.find("@PJL SET DUPLEX");
	if (nPos != std::string::npos)
	{
		m_bHaveGetDuplex = true;
		m_bDuplex = true;	//默认开启双面
		nPos = strTemp.find("OFF");
		if (nPos != std::string::npos)
		{
			m_bDuplex = false;
		}
		theLog.Write(_T("CParsePCL::GetDuplexByPJL,m_bDuplex=%d"), m_bDuplex);
	}

	return m_bDuplex;
}

//获取BINDING参数
int CParsePCL::GetBindingByPJL()
{
	if (m_bHaveGetBinding)
	{
		return m_oBinding;
	}

	std::string strTemp(m_pDataBufer);

	//判断双面打印的长边和短边
	//一般格式为【@PJL SET BINDING = LONGEDGE】【@PJL SET BINDING = SHORTEDGE】
	//【@PJL SET BINDING = LEFT】【@PJL SET BINDING = RIGHT】【@PJL SET BINDING = TOP】
	size_t nPos = strTemp.find("@PJL SET BINDING");
	if (nPos != std::string::npos)
	{
		m_bHaveGetBinding = true;
		m_oBinding = LONGEDGE;	//默认长边
		nPos = strTemp.find("LONGEDGE");
		if (nPos != std::string::npos)
		{
			m_oBinding = LONGEDGE;
		}
		else
		{
			nPos = strTemp.find("SHORTEDGE");
			if (nPos != std::string::npos)
			{
				m_oBinding = SHORTEDGE;
			}
			else
			{
				nPos = strTemp.find("LEFT");
				if (nPos != std::string::npos)
				{
					m_oBinding = LEFT;
				}
				else
				{
					nPos = strTemp.find("TOP");
					if (nPos != std::string::npos)
					{
						m_oBinding = TOP;
					}
					else
					{
						nPos = strTemp.find("RIGHT");
						if (nPos != std::string::npos)
						{
							m_oBinding = RIGHT;
						}
					}
				}
			}
		}
		theLog.Write(_T("CParsePCL::GetBindingByPJL,m_oBinding=%d"), m_oBinding);
	}

	return m_oBinding;
}

//获取打印内容方向参数
int CParsePCL::GetOrientationByPJL()
{
	if (m_bHaveGetOrientation)
	{
		return m_oOrientation;
	}

	std::string strTemp(m_pDataBufer);

	//判断是否开启双面打印
	//一般格式为【@PJL SET ORIENTATION = PORTRAIT】或者【@PJL SET ORIENTATION = LANDSCAPE】两种
	size_t nPos = strTemp.find("@PJL SET ORIENTATION");
	if (nPos != std::string::npos)
	{
		m_bHaveGetOrientation = true;
		m_oOrientation = PORTRAIT;	//默认竖向
		nPos = strTemp.find("LANDSCAPE");
		if (nPos != std::string::npos)
		{
			m_oOrientation = LANDSCAPE;
		}
		theLog.Write(_T("CParsePCL::GetOrientationByPJL,m_oOrientation=%d"), m_oOrientation);
	}

	return m_oOrientation;
}

int CParsePCL::GetColorByPJL()
{
	if (m_bHaveGetColor)
	{
		return m_nColor;
	}

	//参考【PJL_trm.pdf】。
	//通用格式为【@PJL SET PLANESINUSE=1】或者【@PJL SET RENDERMODE = GRAYSCALE】表示黑白打印
	//【@PJL SET PLANESINUSE=3】或者【@PJL SET RENDERMODE = COLOR】表示彩色打印
	std::string strTemp(m_pDataBufer);
	size_t nPos = strTemp.find("@PJL SET PLANESINUSE");
	if (nPos != std::string::npos)
	{
		m_bHaveGetColor = true;
		nPos = strTemp.rfind("=");
		if (nPos != std::string::npos)
		{
			std::string strTemp2 = strTemp.substr(nPos+1);
			int nPlanesInUse = atoi(strTemp2.c_str());
			//一般情况下，1-黑白，3-彩色
			if (nPlanesInUse == 1)
			{
				m_nColor = DMCOLOR_MONOCHROME;
			}
			else //(nPlanesInUse == 3)	
			{
				m_nColor = DMCOLOR_COLOR;
			}
		}
		theLog.Write(_T("CParsePCL::GetColorByPJL,1,m_nColor=%d"), m_nColor);			
		return m_nColor;
	}
	else
	{
		nPos = strTemp.find("@PJL SET RENDERMODE");
		if (nPos != std::string::npos)
		{
			m_bHaveGetColor = true;
			nPos = strTemp.find("GRAYSCALE", nPos);
			if (nPos != std::string::npos)
			{
				m_nColor = DMCOLOR_MONOCHROME;
			}
			else
			{
				m_nColor = DMCOLOR_COLOR;
			}
			theLog.Write(_T("CParsePCL::GetColorByPJL,2,m_nColor=%d"), m_nColor);			
			return m_nColor;
		}
	}

	//惠普一般格式为【@PJL SET GRAYSCALE=BLACKONLY】或者【@PJL SET GRAYSCALE=ON】表示黑白打印
	//【@PJL SET GRAYSCALE=OFF】或者【没有任何色彩控制的PJL参数】表示彩色打印
	nPos = strTemp.find("@PJL SET GRAYSCALE");
	if (nPos != std::string::npos)
	{
		m_bHaveGetColor = true;
		m_nColor = DMCOLOR_COLOR;
		nPos = strTemp.find("BLACKONLY", nPos);
		if (nPos != std::string::npos)
		{
			m_nColor = DMCOLOR_MONOCHROME;
		}
		else
		{
			nPos = strTemp.find("ON", nPos);
			if (nPos != std::string::npos)
			{
				m_nColor = DMCOLOR_MONOCHROME;
			}
		}
		theLog.Write(_T("CParsePCL::GetColorByPJL,3,m_nColor=%d"), m_nColor);			
		return m_nColor;
	}

	//柯美一般格式为【@PJL SET DRIVERCOLORSELECT = GRAYSCALE】表示黑白打印
	//【@PJL SET GRAYSCALE=COLOR】表示彩色打印
	nPos = strTemp.find("@PJL SET DRIVERCOLORSELECT");
	if (nPos != std::string::npos)
	{
		m_bHaveGetColor = true;
		m_nColor = DMCOLOR_COLOR;
		nPos = strTemp.find("GRAYSCALE", nPos);
		if (nPos != std::string::npos)
		{
			m_nColor = DMCOLOR_MONOCHROME;
		}
		else
		{
			m_nColor = DMCOLOR_COLOR;
		}
		theLog.Write(_T("CParsePCL::GetColorByPJL,4,m_nColor=%d"), m_nColor);			
		return m_nColor;
	}

	return m_nColor;
}

bool CParsePCL::ParsePCL()
{
	UByte ubCmd = 0;
	while (GetCmd(ubCmd))
	{
		if (IsDataTag(ubCmd))
		{
			if (!GetData(ubCmd))
			{
				theLog.Write(_T("!!CParsePCL::ParsePCL,GetData fail, ubCmd=0x%02X, m_nFilePos=%I64d"), ubCmd, m_nFilePos);
				break;
			}
		}
		else if (IsAttributeId(ubCmd))
		{
			if (!GetAttribute(ubCmd))
			{
				theLog.Write(_T("!!CParsePCL::ParsePCL,GetAttribute fail, ubCmd=0x%02X, m_nFilePos=%I64d"), ubCmd, m_nFilePos);
				break;
			}
		}
		else if (IsOperator(ubCmd))
		{
			if (!GetOperator(ubCmd))
			{
				theLog.Write(_T("!!CParsePCL::ParsePCL,GetOperator fail, ubCmd=0x%02X, m_nFilePos=%I64d"), ubCmd, m_nFilePos);
				break;
			}

			if (ubCmd == PCL_EndSession)
			{
				//此时，PCL数据解析结束
				theLog.Write(_T("CParsePCL::ParsePCL,end,(%I64d-%I64d=%I64d)"), m_nFileLength, m_nFilePos, m_nFileLength-m_nFilePos);
				break;
			}
		}
		else if (IsEmbeddedData(ubCmd))
		{
			if (!GetEmbeddedData(ubCmd))
			{
				theLog.Write(_T("!!CParsePCL::ParsePCL,GetOperator fail, ubCmd=0x%02X, m_nFilePos=%I64d"), ubCmd, m_nFilePos);
				break;
			}
		}
		else
		{
			theLog.Write(_T("!!CParsePCL::ParsePCL,UnKnow ubCmd=0x%02X, m_nFilePos=%I64d"), ubCmd, m_nFilePos);
			break;
		}
	}

	return true;
}

bool CParsePCL::GetCmd(UByte& ubCmd)
{
	return ReadData(&ubCmd, sizeof(UByte));
}

bool CParsePCL::GetDataValue(UByte& nValue)
{
	return ReadData(&nValue, sizeof(UByte));
}

//PCL数据是小端字节序的，如果在大端系统中，则要倒转字节序
//包括UInt16、UInt32、SInt16、SInt32和Real32。
bool CParsePCL::GetDataValue(UInt16& nValue)
{
	if (ReadData((UByte*)&nValue, sizeof(UInt16)))
	{
		if (m_bIsBigEndianSystem)
		{
			nValue = ((nValue&0xff00)>>8) + ((nValue&0x00ff)<<8);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::GetDataValue(UInt32& nValue)
{
	if (ReadData((UByte*)&nValue, sizeof(UInt32)))
	{
		if (m_bIsBigEndianSystem)
		{
			nValue = (((nValue&0x000000ff)<<24) + ((nValue&0x0000ff00)<<8) + ((nValue&0x00ff0000)>>8) + ((nValue&0xff000000)>>24));
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::GetDataValue(SInt16& nValue)
{
	if (ReadData((UByte*)&nValue, sizeof(SInt16)))
	{
		if (m_bIsBigEndianSystem)
		{
			nValue = ((nValue&0xff00)>>8) + ((nValue&0x00ff)<<8);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::GetDataValue(SInt32& nValue)
{
	if (ReadData((UByte*)&nValue, sizeof(SInt32)))
	{
		if (m_bIsBigEndianSystem)
		{
			nValue = (((nValue&0x000000ff)<<24) + ((nValue&0x0000ff00)<<8) + ((nValue&0x00ff0000)>>8) + ((nValue&0xff000000)>>24));
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::GetDataValue(Real32& nValue)
{
	if (ReadData((UByte*)&nValue, sizeof(Real32)))
	{
		if (m_bIsBigEndianSystem)
		{	
			//将Real32的内存使用UInt32来解析，然后再反转字节序
			UInt32 UInt32data = *((UInt32*)(&nValue));
			UInt32data = (((UInt32data&0x000000ff)<<24) + ((UInt32data&0x0000ff00)<<8) + ((UInt32data&0x00ff0000)>>8) + ((UInt32data&0xff000000)>>24));
			*((UInt32*)(&nValue)) = UInt32data;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::SkipData(__int64 nSkipLength)
{
	if ((m_nFilePos + nSkipLength) >= m_nFileLength)
	{
		return false;
	}

	_fseeki64(m_pFile, nSkipLength, SEEK_CUR);

	m_nFilePos += nSkipLength;

	return true;	
}

bool CParsePCL::ReadData(UByte* pData, size_t nDataLength)
{
	if ((m_nFilePos + nDataLength) >= m_nFileLength)
	{
		theLog.Write(_T("!!CParsePCL::ReadData,1,m_nFilePos=%I64d,nDataLength=%u,m_nFileLength=%I64d")
			, m_nFilePos, nDataLength, m_nFileLength);
		return false;
	}

	if (!pData || (nDataLength <= 0))
	{
		theLog.Write(_T("!!CParsePCL::ReadData,2,pData=%p,nDataLength=%u")
			, pData, nDataLength);
		return false;
	}

	size_t nRead = fread(pData, 1, nDataLength, m_pFile);
	if (nRead != nDataLength)
	{
		theLog.Write(_T("!!CParsePCL::ReadData,3,fread fail,ferror=%d,feof=%d")
			, ferror(m_pFile), feof(m_pFile));
		return false;
	}
	else
	{
		m_nFilePos = _ftelli64(m_pFile);
		return true;
	}
}


bool CParsePCL::IsOperator(UByte ubCmd)
{
	if ((ubCmd >= PCL_BeginSession) && (ubCmd <= PCL_PassThrough))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::IsDataTag(UByte ubCmd)
{
	if ((ubCmd >= PCL_UByteData) && (ubCmd <= PCL_Real32Box))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::IsAttributeId(UByte ubCmd)
{
	if ((ubCmd == PCL_AttributeIdByte) || (ubCmd == PCL_AttributeIdUInt16))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::IsEmbeddedData(UByte ubCmd)
{
	if ((ubCmd == PCL_EmbeddedData) || (ubCmd == PCL_EmbeddedDataByte))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CParsePCL::GetData(UByte ubCmd)
{
	bool bSucc = true;

	Object obj;
	obj.SetObjectType(Data);
	obj.SetDataTags((DataTags)ubCmd);
	obj.SetFileInfo(m_pFile, m_nFilePos-1);
	switch (ubCmd)
	{
	case PCL_UByteData:
		{
			UByte data = 0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;
	case PCL_UInt16Data:
		{
			UInt16 data = 0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;
	case PCL_UInt32Data:
		{
			UInt32 data = 0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;
	case PCL_SInt16Data:
		{
			SInt16 data = 0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;
	case PCL_SInt32Data:
		{
			SInt32 data = 0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;
	case PCL_Real32Data:
		{
			Real32 data = 0.0;
			bSucc = GetDataValue(data);
			obj.SetData(data);
		}
		break;

	case PCL_UByteArray:
	case PCL_UInt16Array:
	case PCL_UInt32Array:
	case PCL_SInt16Array:
	case PCL_SInt32Array:
	case PCL_Real32Array:
		{
			UByte ubDataType = 0;
			UInt32 nDataElements = 0;
			UInt32 nDataLength = 0;
			bSucc = GetArrayDataInfo(ubCmd, ubDataType, nDataElements, nDataLength);
			if (bSucc)
			{
				ArrayData array;
				array.oDataTags = (DataTags)ubCmd;
				array.nArrayLength = nDataElements;
				array.nDataLength = nDataLength;
				array.pArrayData = malloc(nDataLength);
				//!!重要提示：此处的数组数据未进行大小端转换，仅仅是原始数据（小端字节序）
				bSucc = ReadData((UByte*)array.pArrayData, nDataLength);
				if (bSucc)
				{
					//std::string strTemp((char*)array.pArrayData, array.nDataLength);
					//theLog.Write(_T("pArrayData=[%S]"), strTemp.c_str());
					obj.SetArrayData(array);
				}
			}
		}
		break;

	case PCL_UByteXy:
		{
			UByte dataX = 0;
			UByte dataY = 0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;
	case PCL_UInt16Xy:
		{
			UInt16 dataX = 0;
			UInt16 dataY = 0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;
	case PCL_UInt32Xy:
		{
			UInt32 dataX = 0;
			UInt32 dataY = 0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;
	case PCL_SInt16Xy:
		{
			SInt16 dataX = 0;
			SInt16 dataY = 0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;
	case PCL_SInt32Xy:
		{
			SInt32 dataX = 0;
			SInt32 dataY = 0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;
	case PCL_Real32Xy:
		{
			Real32 dataX = 0.0;
			Real32 dataY = 0.0;
			bSucc = GetDataValue(dataX);
			obj.SetData(dataX);
			bSucc = GetDataValue(dataY);
			obj.SetData(dataY, Second);
		}
		break;

	case PCL_UByteBox:
		{
			UByte dataBox1 = 0;
			UByte dataBox2 = 0;
			UByte dataBox3 = 0;
			UByte dataBox4 = 0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	case PCL_UInt16Box:
		{
			UInt16 dataBox1 = 0;
			UInt16 dataBox2 = 0;
			UInt16 dataBox3 = 0;
			UInt16 dataBox4 = 0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	case PCL_UInt32Box:
		{
			UInt32 dataBox1 = 0;
			UInt32 dataBox2 = 0;
			UInt32 dataBox3 = 0;
			UInt32 dataBox4 = 0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	case PCL_SInt16Box:
		{
			SInt16 dataBox1 = 0;
			SInt16 dataBox2 = 0;
			SInt16 dataBox3 = 0;
			SInt16 dataBox4 = 0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	case PCL_SInt32Box:
		{
			SInt32 dataBox1 = 0;
			SInt32 dataBox2 = 0;
			SInt32 dataBox3 = 0;
			SInt32 dataBox4 = 0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	case PCL_Real32Box:
		{
			Real32 dataBox1 = 0.0;
			Real32 dataBox2 = 0.0;
			Real32 dataBox3 = 0.0;
			Real32 dataBox4 = 0.0;
			bSucc = GetDataValue(dataBox1);
			obj.SetData(dataBox1);
			bSucc = GetDataValue(dataBox2);
			obj.SetData(dataBox2, Second);
			bSucc = GetDataValue(dataBox3);
			obj.SetData(dataBox3, Third);
			bSucc = GetDataValue(dataBox4);
			obj.SetData(dataBox4, Fourth);
		}
		break;
	}
	
	if (bSucc)
	{
		m_oParseStack.push(obj);
		return true;
	}
	else
	{
		theLog.Write(_T("!!CParsePCL::GetData,fail,m_nFilePos=%I64d"), m_nFilePos);
		return false;
	}
}

bool CParsePCL::GetAttribute(UByte ubCmd)
{
	if (ubCmd == PCL_AttributeIdByte)
	{
		Object obj;
		obj.SetObjectType(AttributeIdByte);
		obj.SetDataTags(UByteData);
		obj.SetFileInfo(m_pFile, m_nFilePos-1);
		UByte ubAttributeId;
		GetDataValue(ubAttributeId);
// 		theLog.Write(_T("##CParsePCL::GetAttribute,PCL_AttributeIdByte,(%d)%s")
// 			, ubAttributeId, gAttributeInfoTable[ubAttributeId].szAttribute);
		obj.SetData(ubAttributeId);
		m_oParseStack.push(obj);

		return true;		
	}
	else if (ubCmd == PCL_AttributeIdUInt16)
	{
		Object obj;
		obj.SetObjectType(AttributeIdUInt16);
		obj.SetDataTags(UInt16Data);
		obj.SetFileInfo(m_pFile, m_nFilePos-1);
		UInt16 ubAttributeId;
		GetDataValue(ubAttributeId);
		obj.SetData(ubAttributeId);
		m_oParseStack.push(obj);
		//theLog.Write(_T("##CParsePCL::GetAttribute,PCL_AttributeIdUInt16,0x04X"), ubAttributeId);
		return true;
	}
	else
	{
		theLog.Write(_T("!!CParsePCL::GetAttribute,未知的属性标识,ubCmd=0x%02X"), ubCmd);
		return false;
	}
}

bool CParsePCL::GetOperator(UByte ubCmd)
{
	//theLog.Write(_T("CParsePCL::GetOperator,[%s]"), gOperatorInfoTable[ubCmd].szOperator);

	//1.先将数据入栈
	Object obj;
	obj.SetObjectType(Operator);
	obj.SetDataTags(UByteData);
	obj.SetFileInfo(m_pFile, m_nFilePos-1);
	obj.SetData(ubCmd);
	m_oParseStack.push(obj);

	//2.对不同操作进行分类处理
	switch (ubCmd)
	{
	case PCL_BeginSession:
		BeginSession();
		break;
	case PCL_EndSession:
		EndSession();
		break;
	case PCL_BeginPage:
		BeginPage();
		break;
	case PCL_EndPage:
		EndPage();
		break;
	case PCL_SetColorSpace:
		SetColorSpace();
		break;
	default:
		break;;
	}

	//暂时在每次Operator之后，就清空栈。
	ClearParseStack();

	return true;
}

bool CParsePCL::GetEmbeddedData(UByte ubCmd)
{
#pragma message("==============CParsePCL::GetEmbeddedData接口未实现，数据未入栈，仅仅是简单跳过此段数据============")
	UInt32 nDataSize = 0;
	if (ubCmd == PCL_EmbeddedData)
	{
		if (!GetDataValue(nDataSize))
		{
			theLog.Write(_T("CParsePCL::GetOperator,1,PCL_EmbeddedData,GetDataValue fail"));
			return false;
		}

// 		Object obj;
// 		obj.SetObjectType(EmbeddedData);
// 		obj.SetDataTags(UByteData);
// 		obj.SetFileInfo(m_pFile, m_nFilePos-4);
// 		obj.SetArrayData();
// 		m_oParseStack.push(obj);
	}
	else if (ubCmd == PCL_EmbeddedDataByte)
	{
		UByte nSize = 0;
		if (!GetDataValue(nSize))
		{
			theLog.Write(_T("CParsePCL::GetOperator,2,PCL_EmbeddedDataByte,GetDataValue fail"));
			return false;
		}
		else
		{
			nDataSize = nSize;
		}
// 		Object obj;
// 		obj.SetObjectType(EmbeddedData);
// 		obj.SetDataTags(UByteData);
// 		obj.SetFileInfo(m_pFile, m_nFilePos-1);
// 		obj.SetArrayData();
// 		m_oParseStack.push(obj);
	}
	else
	{
		theLog.Write(_T("!!CParsePCL::GetAttribute,3,未知的嵌入数据标识,ubCmd=0x%02X"), ubCmd);
		return false;
	}

	return SkipData(nDataSize);
}

bool CParsePCL::GetArrayDataInfo(UByte ubCmd, UByte& ubDataType, UInt32& nDataElements, UInt32& nDataLength)
{
	bool bSucc = false;
	ubDataType = 0;
	nDataElements = 0;
	nDataLength = 0;

	//获取数组长度的o数据标签（类型）
	if (!GetDataValue(ubDataType))
	{
		theLog.Write(_T("!!CParsePCL::GetArrayDataSize,1,GetDataValue,fail"));
		return false;
	}

	switch (ubDataType)
	{
	case PCL_UByteData:
		{
			UByte nSize = 0;
			bSucc = GetDataValue(nSize);
			if (bSucc)
			{
				nDataElements = nSize;
			}
		}
		break;
	case PCL_UInt16Data:
		{
			UInt16 nSize = 0;
			bSucc = GetDataValue(nSize);
			if (bSucc)
			{
				nDataElements = nSize;
			}
		}
		break;
	case PCL_UInt32Data:
		{
			UInt32 nSize = 0;
			bSucc = GetDataValue(nSize);
			if (bSucc)
			{
				nDataElements = nSize;
			}
		}
		break;
	default:
		{
			bSucc = false;
			theLog.Write(_T("!!CParsePCL::GetArrayDataSize,数据类型异常,ubDataType=0x%02X"), ubDataType);
		}
		break;
	}

	if (!bSucc)
	{
		//获取数据类型失败，直接返回错误
		theLog.Write(_T("!!CParsePCL::GetArrayDataSize,2,fail"));
		return false;
	}

	switch (ubCmd)
	{
	case PCL_UByteArray:
		nDataLength = nDataElements * sizeof(UByte);
		break;
	case PCL_UInt16Array:
		nDataLength = nDataElements * sizeof(UInt16);
		break;
	case PCL_UInt32Array:
		nDataLength = nDataElements * sizeof(UInt32);
		break;
	case PCL_SInt16Array:
		nDataLength = nDataElements * sizeof(SInt16);
		break;
	case PCL_SInt32Array:
		nDataLength = nDataElements * sizeof(SInt32);
		break;
	case PCL_Real32Array:
		nDataLength = nDataElements * sizeof(Real32);
		break;
	default:
		assert(false);
		theLog.Write(_T("!!CParsePCL::GetArrayDataSize,3,数据类型异常,ubCmd=0x%02X"), ubCmd);
		break;
	}

	return true;
}

bool CParsePCL::IsBigEndianSystem()
{
	union ut
	{
		short s;
		char c[2];
	}u;

	u.s = 0x01;
	return (u.c[1] == 0x01);
}

//清空解析PCL时，保存的临时对象栈
void CParsePCL::ClearParseStack()
{
	while(!m_oParseStack.empty())
	{
		m_oParseStack.pop();
	}
}

void CParsePCL::BeginSession()
{
	//清空
	m_oPageDuplexObjectList.clear();

	//theLog.Write(_T("CParsePCL::BeginSession"));
	while(!m_oParseStack.empty())
	{
		Object obj = m_oParseStack.top();
		if (obj.IsAttributeId())
		{
			UByte ubAttribute = 0;
			obj.GetData(ubAttribute);
			switch (ubAttribute)
			{
			case PCL_Measure:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_eInch:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_Measure(%d=>eInch)"), data);
						break;
					case PCL_eMillimeter:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_Measure(%d=>eMillimeter)"), data);
						break;
					case PCL_eTenthsOfAMillimeter:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_Measure(%d=>eTenthsOfAMillimeter)"), data);
						break;
					default:
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginSession,PCL_Measure(%d=>UnKnwo)"), data);
						break;
					}
				}
				break;
			case PCL_UnitsPerMeasure:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UInt16 dataX, dataY;
					obj.GetData(dataX);
					obj.GetData(dataY, Second);
					//theLog.Write(_T("CParsePCL::BeginSession,PCL_UnitsPerMeasure(%dx%d)"), dataX, dataY);
				}
				break;
			case PCL_ErrorReport:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_eNoReporting:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eNoReporting)"), data);
						break;
					case PCL_eBackChannel:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eErrorPage)"), data);
						break;
					case PCL_eErrorPage:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eErrorPage)"), data);
						break;
					case PCL_eBackChAndErrPage:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eBackChAndErrPage)"), data);
						break;
						/*
					case PCL_eBackChanAndErrPage:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eBackChanAndErrPage)"), data);
						break;
						*/
					case PCL_eNWBackChannel:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eNWBackChannel)"), data);
						break;
					case PCL_eNWErrorPage:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eNWErrorPage)"), data);
						break;
					case PCL_eNWBackChAndErrPage:
						//theLog.Write(_T("CParsePCL::BeginSession,PCL_ErrorReport(%d=>eNWBackChAndErrPage)"), data);
						break;
					default:
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginSession,PCL_ErrorReport(%d=>UnKnow"), data);
						break;
					}
				}
				break;
			default:
				break;
			}
		}
		m_oParseStack.pop();
	}
}

void CParsePCL::EndSession()
{
	//theLog.Write(_T("CParsePCL::EndSession"));
	ClearParseStack();
}

void CParsePCL::BeginPage()
{
	//theLog.Write(_T("CParsePCL::BeginPage (%d)"), m_nPageCount+1);
	while(!m_oParseStack.empty())
	{
		Object obj = m_oParseStack.top();
		if (obj.IsAttributeId())
		{
			UByte ubAttribute = 0;
			obj.GetData(ubAttribute);
			switch (ubAttribute)
			{
			case PCL_Orientation:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_ePortraitOrientation:
						if (!m_bHaveGetOrientation)
						{
							m_bHaveGetOrientation = true;
							m_oOrientation = PORTRAIT;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_Orientation(%d=>竖向)"), data);
						break;
					case PCL_eLandscapeOrientation:
						if (!m_bHaveGetOrientation)
						{
							m_bHaveGetOrientation = true;
							m_oOrientation = LANDSCAPE;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_Orientation(%d=>横向)"), data);
						break;
					case PCL_eReversePortrait:
						if (!m_bHaveGetOrientation)
						{
							m_bHaveGetOrientation = true;
							m_oOrientation = PORTRAIT;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_Orientation(%d=>反转竖向)"), data);
						break;
					case PCL_eReverseLandscape:
						if (!m_bHaveGetOrientation)
						{
							m_bHaveGetOrientation = true;
							m_oOrientation = LANDSCAPE;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_Orientation(%d=>反转横向)"), data);
						break;
					case PCL_eDefaultOrientation:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_eDefaultOrientation(%d=>默认方向)"), data);
						break;
					default:
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginPage,PCL_Orientation(%d=>UnKnow)"), data);
						break;
					}
				}
				break;
			case PCL_MediaSize:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					//纸型大小有两种格式，
					//一种是直接是一个字符串，使用UByteArray保存
					//另一种是使用枚举表示

					if (m_bHaveGetPaper)
					{
						break;
					}
					if (obj.IsArray())
					{
						ArrayData oArrayData;
						obj.GetArrayData(oArrayData);
						std::string strTemp((char*)oArrayData.pArrayData, oArrayData.nDataLength);
						theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%S)"), strTemp.c_str());
						CString szMediaSize = CCommonFun::UTF8ToUnicode(strTemp.c_str());
						m_nPaper = CCommonFun::GetPagerSize(szMediaSize);
						if (m_nPaper <= 0)
						{
							if (szMediaSize.CompareNoCase(_T("EXEC")) == 0)
							{
								m_nPaper = DMPAPER_EXECUTIVE;
							}
							else if (szMediaSize.CompareNoCase(_T("JIS B5")) == 0)
							{
								m_nPaper = DMPAPER_B5;
							}
							else if (szMediaSize.CompareNoCase(_T("JIS B4")) == 0)
							{
								m_nPaper = DMPAPER_B4;
							}
							else if (szMediaSize.CompareNoCase(_T("C5")) == 0)
							{
								m_nPaper = DMPAPER_ENV_C5;
							}
							else if (szMediaSize.CompareNoCase(_T("DL")) == 0)
							{
								m_nPaper = DMPAPER_ENV_DL;
							}
							else if (szMediaSize.CompareNoCase(_T("COM10")) == 0)
							{
								m_nPaper = DMPAPER_ENV_10;
							}
							else if (szMediaSize.CompareNoCase(_T("MONARCH")) == 0)
							{
								m_nPaper = DMPAPER_ENV_MONARCH;
							}
							else if (szMediaSize.CompareNoCase(_T("B5 ENV")) == 0)
							{
								m_nPaper = DMPAPER_ENV_B5;
							}
							else if (szMediaSize.CompareNoCase(_T("JPOST")) == 0)
							{
								m_nPaper = DMPAPER_JAPANESE_POSTCARD;
							}
							else if (szMediaSize.CompareNoCase(_T("JPOSTD")) == 0)
							{
								m_nPaper = DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED;
							}
							else if (szMediaSize.CompareNoCase(_T("JISEXEC")) == 0)
							{
								m_nPaper = DMPAPER_EXECUTIVE;
							}
							else if (szMediaSize.CompareNoCase(_T("ROC16K")) == 0)
							{
								m_nPaper = DMPAPER_P16K;
							}
						}
					}
					else
					{
						UByte data;
						obj.GetData(data);
						switch (data)
						{
						case PCL_eA4Paper:
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eA4Paper)"), data);
							m_nPaper = DMPAPER_A4;
							break;
						case PCL_eA3Paper:
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eA3Paper)"), data);
							m_nPaper = DMPAPER_A3;
							break;
						case PCL_eLetterPaper:
							m_nPaper = DMPAPER_LETTER;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eLetterPaper)"), data);
							break;
						case PCL_eLegalPaper:
							m_nPaper = DMPAPER_LEGAL;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eLegalPaper)"), data);
							break;
						case PCL_eExecPaper:
							m_nPaper = DMPAPER_EXECUTIVE;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eExecPaper)"), data);
							break;
						case PCL_eLedgerPaper:
							m_nPaper = DMPAPER_LEDGER;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eLedgerPaper)"), data);
							break;
						case PCL_eCOM10Envelope:
							m_nPaper = DMPAPER_ENV_10;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eCOM10Envelope)"), data);
							break;
						case PCL_eMonarchEnvelope:
							m_nPaper = DMPAPER_ENV_MONARCH;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eMonarchEnvelope)"), data);
							break;
						case PCL_eC5Envelope:
							m_nPaper = DMPAPER_ENV_C5;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eC5Envelope)"), data);
							break;
						case PCL_eDLEnvelope:
							m_nPaper = DMPAPER_ENV_DL;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eDLEnvelope)"), data);
							break;
						case PCL_eJB4Paper:
							m_nPaper = DMPAPER_B4;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eJB4Paper)"), data);
							break;
						case PCL_eJB5Paper:
							m_nPaper = DMPAPER_B5;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eJB5Paper)"), data);
							break;
						case PCL_eB5Envelope:
							m_nPaper = DMPAPER_ENV_B5;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eB5Envelope)"), data);
							break;
						case PCL_eB5Paper:
							m_nPaper = DMPAPER_B5;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eB5Paper)"), data);
							break;
						case PCL_eJPostcard:
							m_nPaper = DMPAPER_JAPANESE_POSTCARD;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eJPostcard)"), data);
							break;
						case PCL_eJDoublePostcard:
							m_nPaper = DMPAPER_DBL_JAPANESE_POSTCARD;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eJDoublePostcard)"), data);
							break;
						case PCL_eA5Paper:
							m_nPaper = DMPAPER_A5;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eA5Paper)"), data);
							break;
						case PCL_eA6Paper:
							m_nPaper = DMPAPER_A6;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eA6Paper)"), data);
							break;
						case PCL_eJB6Paper:
							m_nPaper = DMPAPER_B6_JIS;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eJB6Paper)"), data);
							break;
						case PCL_JIS8K:
							m_nPaper = DMPAPER_A3;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>JIS8K)"), data);
							break;
						case PCL_JIS16K:
							m_nPaper = DMPAPER_A3;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>JIS16K)"), data);
							break;
						case PCL_JISExec:
							m_nPaper = DMPAPER_EXECUTIVE;
							//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>JISExec)"), data);
							break;
						case PCL_eDefaultPaperSize:
							m_nPaper = DMPAPER_A4;
							theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSize(%d=>eDefaultPaperSize)"), data);
							break;
						default:
							assert(false);
							m_nPaper = DMPAPER_A4;
							theLog.Write(_T("!!CParsePCL::BeginPage,PCL_MediaSize(%d=>UnKnow)"), data);
							break;
						}
					}

					if (m_nPaper == DMPAPER_A3)
					{
						m_nA3PageCount++;
					}
					else if (m_nPaper == DMPAPER_A4)
					{
						m_nA4PageCount++;
					}
					else
					{
						m_nOtherPageCount++;
					}
					m_bHaveGetPaper = true;
				}
				break;
			case PCL_CustomMediaSize:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					DataTags tags = obj.GetDataTags();
					if (tags == UInt16Xy)
					{
						UInt16 dataX, dataY;
						obj.GetData(dataX);
						obj.GetData(dataY);
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_CustomMediaSize(%dx%d)"), dataX, dataY);
					}
					else if (tags == Real32Xy)
					{
						Real32 dataX, dataY;
						obj.GetData(dataX);
						obj.GetData(dataY);
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_CustomMediaSize(%fx%f)"), dataX, dataY);
					}
					else
					{
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginPage,PCL_CustomMediaSize(%d=>UnKnow)"), tags);
					}
				}
				break;
			case PCL_CustomMediaSizeUnits:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_eInch:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_CustomMediaSizeUnits(%d=>eInch)"), data);
						break;
					case PCL_eMillimeter:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_CustomMediaSizeUnits(%d=>eMillimeter)"), data);
						break;
					case PCL_eTenthsOfAMillimeter:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_CustomMediaSizeUnits(%d=>eTenthsOfAMillimeter)"), data);
						break;
					default:
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginPage,PCL_CustomMediaSizeUnits(%d=>UnKnwo)"), data);
						break;
					}
				}
				break;
			case PCL_MediaType:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					//纸型大小,是一个字符串，使用UByteArray保存
					if (obj.IsArray())
					{
						//ArrayData oArrayData;
						//obj.GetArrayData(oArrayData);
						//std::string strTemp((char*)oArrayData.pArrayData, oArrayData.nDataLength);
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaType(%S)"), strTemp.c_str());
					}
					else
					{
						assert(false);
						theLog.Write(_T("!!CParsePCL::BeginPage,PCL_MediaType(UnKnow DataTags=%d)"), obj.GetDataTags());
					}
				}
				break;
			case PCL_MediaSource:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_eDefaultSource:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eDefaultSource)"), data);
						break;
					case PCL_eAutoSelect:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eAutoSelect)"), data);
						break;
					case PCL_eManualFeed:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eManualFeed)"), data);
						break;
					case PCL_eMultiPurposeTray:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eMultiPurposeTray)"), data);
						break;
					case PCL_eUpperCassette:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eUpperCassette)"), data);
						break;
					case PCL_eLowerCassette:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eLowerCassette)"), data);
						break;
					case PCL_eEnvelopeTray:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eEnvelopeTray)"), data);
						break;
					case PCL_eThirdCassette:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaSource(%d=>eThirdCassette)"), data);
						break;
					default:
						theLog.Write(_T("##CParsePCL::BeginPage,PCL_MediaSource(%d=>External Trays(1-248)(8-255)"), data);
						break;
					}
				}
				break;
			case PCL_MediaDestination:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					switch (data)
					{
					case PCL_eDefaultDestination:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaDestination(%d=>eDefaultDestination)"), data);
						break;
					case PCL_eFaceDownBin:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaDestination(%d=>eFaceDownBin)"), data);
						break;
					case PCL_eFaceUpBin:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaDestination(%d=>eFaceUpBin)"), data);
						break;
					case PCL_eJobOffsetBin:
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_MediaDestination(%d=>eJobOffsetBin)"), data);
						break;
					default:
						theLog.Write(_T("##CParsePCL::BeginPage,PCL_MediaDestination(%d=>External Bins(1-251)(5-255)"), data);
						break;
					}
				}
				break;
			case PCL_SimplexPageMode:
				{
					m_oPageDuplexObjectList.push_back(obj);	//添加到队列中
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					if (data == PCL_eSimplexFrontSide)
					{
						if (!m_bHaveGetPageMode)
						{
							m_bHaveGetPageMode = true;
							m_oPageMode = Simplex;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_SimplexPageMode(%d=>eSimplexFrontSide 单面打印)"), data);
					}
					else
					{
						assert(false);
						theLog.Write(_T("##CParsePCL::BeginPage,PCL_SimplexPageMode(%d=>UnKnow"), data);
					}
				}
				break;
			case PCL_DuplexPageMode:
				{
					m_oPageDuplexObjectList.push_back(obj);	//添加到队列中
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					//双面长边和双面短边参数要结合页面内容的实际方向，
					//当双面为eDuplexVerticalBinding和内容为ePortraitOrientation，表示双面长边，
					//当双面为eDuplexVerticalBinding和内容为eLandscapeOrientation，表示双面短边，
					//当双面为eDuplexHorizontalBinding和内容为eLandscapeOrientation，表示双面长边，
					//当双面为eDuplexHorizontalBinding和内容为ePortraitOrientation，表示双面短边，
					if (data == PCL_eDuplexHorizontalBinding)
					{
						if (!m_bHaveGetPageMode)
						{
							m_bHaveGetPageMode = true;
							m_oPageMode = DuplexHorizontal;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_DuplexPageMode(%d=>eDuplexHorizontalBinding 双面横边)"), data);
					}
					else if (data == PCL_eDuplexVerticalBinding)
					{
						if (!m_bHaveGetPageMode)
						{
							m_bHaveGetPageMode = true;
							m_oPageMode = DuplexVertical;
						}
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_DuplexPageMode(%d=>PCL_eDuplexVerticalBinding 双面竖边)"), data);
					}
					else
					{
						assert(false);
						theLog.Write(_T("##CParsePCL::BeginPage,PCL_DuplexPageMode(%d=>UnKnow"), data);
					}
				}
				break;
			case PCL_DuplexPageSide:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					if (data == PCL_eFrontMediaSide)
					{
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_DuplexPageSide(%d=>eFrontMediaSide)"), data);
					}
					else if (data == PCL_eBackMediaSide)
					{
						//theLog.Write(_T("CParsePCL::BeginPage,PCL_DuplexPageSide(%d=>eBackMediaSide)"), data);
					}
					else
					{
						assert(false);
						theLog.Write(_T("##CParsePCL::BeginPage,PCL_DuplexPageSide(%d=>UnKnow"), data);
					}
				}
				break;
			default:
				break;
			}
		}
		m_oParseStack.pop();
	}
}

void CParsePCL::EndPage()
{
	//theLog.Write(_T("CParsePCL::EndPage (%d)"), m_nPageCount+1);
	m_bHaveGetPageCount = true;
	m_nPageCount++;

	while(!m_oParseStack.empty())
	{
		Object obj = m_oParseStack.top();
		if (obj.IsAttributeId())
		{
			UByte ubAttribute = 0;
			obj.GetData(ubAttribute);
			switch (ubAttribute)
			{
			case PCL_PageCopies:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UInt16 data;
					obj.GetData(data);
					if (!m_bHaveGetCopies)
					{
						m_nCopies = data;
						m_bHaveGetCopies = true;

						//一般情况下在PJL命令中会指定是否逐份打印，
						//如果PJL没有指定，并且代码走到了此处，
						//则认为是非逐份打印，但是，如果份数为1，
						//则逐份打印参数没有意义，所以给默认为逐份打印。
						if (m_nCopies > 1)
						{
							//非逐份打印
							m_nCollate = DMCOLLATE_FALSE;
							m_bHaveGetCollate = true;
						}
						else
						{
							//份逐份打印
							m_nCollate = DMCOLLATE_TRUE;
							m_bHaveGetCollate = true;
						}

						theLog.Write(_T("CParsePCL::EndPage,PCL_PageCopies(%d)"), data);
					}
				}
				break;
			default:
				break;
			}
		}
		m_oParseStack.pop();
	}
}

void CParsePCL::SetColorSpace()
{
	//theLog.Write(_T("CParsePCL::SetColorSpace"));

	while(!m_oParseStack.empty())
	{
		Object obj = m_oParseStack.top();
		if (obj.IsAttributeId())
		{
			UByte ubAttribute = 0;
			obj.GetData(ubAttribute);
			switch (ubAttribute)
			{
			case PCL_ColorSpace:
				{
					m_oParseStack.pop();
					obj = m_oParseStack.top();
					UByte data;
					obj.GetData(data);
					if (data == PCL_eGray)
					{
						if (!m_bHaveGetColor)
						{
							m_bHaveGetColor = true;
							m_nColor = DMCOLOR_MONOCHROME;
							theLog.Write(_T("CParsePCL::SetColorSpace,PCL_ColorSpace(%d=>eGray 黑白)"), data);
						}
						//theLog.Write(_T("CParsePCL::SetColorSpace,PCL_ColorSpace(%d=>eGray 黑白)"), data);
					}
					else if (data == PCL_eRGB)
					{
						if (!m_bHaveGetColor)
						{
							m_bHaveGetColor = true;
							m_nColor = DMCOLOR_COLOR;
							theLog.Write(_T("CParsePCL::SetColorSpace,PCL_ColorSpace(%d=>eRGB 彩色)"), data);
						}
						//theLog.Write(_T("CParsePCL::SetColorSpace,PCL_ColorSpace(%d=>eRGB 彩色)"), data);
					}
					else
					{
						assert(false);
						theLog.Write(_T("##CParsePCL::SetColorSpace,PCL_ColorSpace(%d=>UnKnow)"), data);
					}
				}
				break;
			case PCL_PaletteDepth:
				{
					//theLog.Write(_T("CParsePCL::SetColorSpace,PCL_PaletteDepth"));
				}
				break;
			case PCL_PaletteData:
				{
					//theLog.Write(_T("CParsePCL::SetColorSpace,PCL_PaletteData"));
				}
				break;
			default:
				break;
			}
		}
		m_oParseStack.pop();
	}
}

}//命名空间ParsePCL结束
