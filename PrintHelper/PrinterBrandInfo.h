#pragma once

/*
enum BRAND_TYPE
{
	BT_SL,
	BT_DZ,
	BT_KM,
	BT_LM,
	BT_THIN_CLT,
	BT_LG,		//add by zxl,2013.03.08,����ӡ��
	BT_SL2,		//add by zxl,2013.07.01,ʩ�ֵ��µ�·��
	BT_JC,		//add by zxl,2013.09.16,���ɴ�ӡ��
	BT_ZD,		//add by zxl,2013.09.17,�𵩴�ӡ��
	BT_XP,		//add by zxl,2013.08.09,���մ�ӡ��
	BT_SX,		//add by zxl,2015.04.27,���Ǵ�ӡ��
	BT_HP,		//add by zxl,2015.07.16,���մ�ӡ��
	BT_EPSON,	//add by szy 2016.04.20,��������ӡ��
	BT_CANON,	//add by szy 2016.04.20,���ܴ�ӡ��
	BT_BROTHER, //add by zfq 2016.05.04,�ֵܴ�ӡ��
	BT_JT,		//add by zfq 2016.05.04,��ͼ��ӡ��
	BT_OKI,		//add by zfq 2016.05.04,OKI��ӡ��
	BT_SEIKO,	//add by zxl 2016.05.18,������ӡ��
	BT_PANTUM,		//add by zxl 2016.05.24,��ͼ
	BT_JOLIMARK,	//add by zxl 2016.05.24,ӳ��
	BT_GPRINTER,	//add by zxl 2016.05.24,�Ѳ�
	BT_PANASONIC,	//add by zxl 2016.05.24,����
	BT_LENOVO,		//add by zxl 2016.05.24,����
	BT_START,		//add by zxl 2016.05.24,ʵ��
	BT_XPRINTER,	//add by zxl 2016.05.24,о��
	BT_SONY,		//add by zxl 2016.05.24,����
	BT_DELL			//add by zxl 2016.05.24,����
};
*/

struct PrinterBrandInfo
{
	BRAND_TYPE nBrand;					//Ʒ���ڲ����룬�ο�BRAND_TYPE
	int nSnmpPrivateKey;		//snmp��Ʒ��˽��ʶ����
	TCHAR cManufacturer[100];	//Ʒ�����ƣ�Ӣ�ģ�
	TCHAR cManufacturer2[200];	//Ʒ�����ƣ����ģ�
};

static PrinterBrandInfo gPrinterBrandInfoTable[] = 
{
	{BT_THIN_CLT,	0,		_T("UnKnow"),		_T("δ֪")},
	{BT_HP,			11,		_T("HP"),			_T("����")},
	{BT_SX,			236,	_T("Samsung"),		_T("����")},
	{BT_SL2,		253,	_T("XEROX"),		_T("ʩ��")},
	{BT_SL2,		297,	_T("FUJI XEROX"),	_T("��ʿʩ��")},
	{BT_LG,			367,	_T("RICOH"),		_T("���")},
	{BT_LM,			641,	_T("Lexmark"),		_T("����")},
	{BT_DZ,			1129,	_T("TOSHIBA"),		_T("��֥")},
	{BT_EPSON,		1248,	_T("EPSON"),		_T("������")},
	{BT_JC,			1347,	_T("KYOCERA"),		_T("����")},
	{BT_KM,			18334,	_T("KONICA MINOLTA"), _T("���Ῠ���ܴ�")},
	{BT_XP,			2385,	_T("SHARP"),		_T("����")},
	{BT_CANON,		1602,	_T("Canon"),		_T("����")},
	{BT_BROTHER,	2435,	_T("Brother"),		_T("�ֵ�")},
	{BT_JT,			44966,	_T("Jingtu"),		_T("��ͼ")},
	{BT_ZD,			111,	_T("AURORA"),		_T("��")},
	{BT_OKI,		2001,	_T("Oki"),			_T("�����")},
	{BT_SEIKO,		263,	_T("Seiko"),		_T("����")},
	{BT_PANTUM,		40093,	_T("Pantum"),		_T("��ͼ")}
};

TCHAR* GetManufacturer(int nBrand)
{
	TCHAR* pManufacturer = gPrinterBrandInfoTable[0].cManufacturer;
	int nCount = (sizeof(gPrinterBrandInfoTable)/sizeof(gPrinterBrandInfoTable[0]));
	for (int nIndex=1; nIndex<nCount; nIndex++)
	{
		if (gPrinterBrandInfoTable[nIndex].nBrand == nBrand)
		{
			pManufacturer = gPrinterBrandInfoTable[nIndex].cManufacturer;
			break;
		}
	}
	return pManufacturer;
}

TCHAR* GetManufacturer2(int nBrand)
{
	TCHAR* pManufacturer2 = gPrinterBrandInfoTable[0].cManufacturer2;
	int nCount = (sizeof(gPrinterBrandInfoTable)/sizeof(gPrinterBrandInfoTable[0]));
	for (int nIndex=1; nIndex<nCount; nIndex++)
	{
		if (gPrinterBrandInfoTable[nIndex].nBrand == nBrand)
		{
			pManufacturer2 = gPrinterBrandInfoTable[nIndex].cManufacturer2;
			break;
		}
	}
	return pManufacturer2;
}

BRAND_TYPE GetBrandType(int nSnmpPrivateKey)
{
	BRAND_TYPE nBrand = gPrinterBrandInfoTable[0].nBrand;
	int nCount = (sizeof(gPrinterBrandInfoTable)/sizeof(gPrinterBrandInfoTable[0]));
	for (int nIndex=1; nIndex<nCount; nIndex++)
	{
		if (gPrinterBrandInfoTable[nIndex].nSnmpPrivateKey == nSnmpPrivateKey)
		{
			nBrand = gPrinterBrandInfoTable[nIndex].nBrand;
			break;
		}
	}
	return nBrand;
}
