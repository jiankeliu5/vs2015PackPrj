#include "StdAfx.h"
#include "CardAnalyst.h"

CCardAnalyst::CCardAnalyst(void)
{
}

CCardAnalyst::~CCardAnalyst(void)
{
}

CString CCardAnalyst::HandleCardNo( CString& szParam )
{
	CString szFun = _T("");
	CString szNewParam = _T("");
	if(Split(szParam, szFun, szNewParam))
	{
		if(szFun.CompareNoCase(_T("DecToHex")) == 0)
		{
			return DecToHex(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("HexToDec")) == 0)
		{
			return HexToDec(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("DecRevsToHex")) == 0)
		{
			return DecRevsToHex(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("DecRevsToDec")) == 0)
		{
			return DecRevsToDec(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("HexRevsToDec")) == 0)
		{
			return HexRevsToDec(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("HexRevsToHex")) == 0)
		{
			return HexRevsToHex(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("Intercept")) == 0)
		{
			return Intercept(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("InterceptLeft")) == 0)
		{
			return InterceptLeft(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("InterceptRight")) == 0)
		{
			return InterceptRight(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("Wiegand34To26")) == 0)
		{
			return Wiegand34To26(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("AddToLeft")) == 0)
		{
			return AddToLeft(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("AddToRight")) == 0)
		{
			return AddToRight(szNewParam);
		}
		else if(szFun.CompareNoCase(_T("Concat")) == 0)
		{
			return Concat(szNewParam);
		}
		else if (szFun.CompareNoCase(_T("StrRevsToStr")) == 0)
		{
			return StrRevsToStr(szNewParam);
		}
	}
	else
	{
		//_tprintf(_T("!!CAnalyst::Split,failed ,szParam=%s\n"), szParam);
	}
	return szParam;
}

BOOL CCardAnalyst::Split( CString& source, CString& funout, CString& paramout )
{
	int nFirstIndex = source.Find('(');
	int nLastIndex = source.ReverseFind(')');
	if (nFirstIndex > 0 && nLastIndex > 0)
	{
		funout = source.Mid(0, nFirstIndex);
		paramout = source.Mid(nFirstIndex+1, nLastIndex-nFirstIndex-1);
		//_tprintf(_T("CAnalyst::Split,funout=%s,paramout=%s\n"),funout,paramout);
		return TRUE;
	}
	return FALSE;
}

CString CCardAnalyst::DecToHex( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	//转换成16进制
	unsigned long nCardNo = _tstoi64(szCardNo);
	szCardNo.Format(_T("%08x"), nCardNo);
	szCardNo.MakeUpper();

	//_tprintf(_T("CAnalyst::DecToHex,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

CString CCardAnalyst::HexToDec( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	szCardNo.MakeUpper();
	int nLength = szCardNo.GetLength();
	unsigned __int64 value = 0;
	TCHAR c = '\0';
	unsigned __int64 temp = 0;
	for (int i=0; i<nLength; i++)
	{
		c = szCardNo.GetAt(i);
		if (c >= 'A')
		{
			temp = 10 + c - 'A';
		}
		else
		{
			temp = c - '0';
		}
		value = value * 16 + temp;
	}
	TCHAR chNum[21] = {0};
	_stprintf(chNum, _T("%I64u"), value);
	CString szOut(chNum);
	//_tprintf(_T("CAnalyst::HexToDec,szOut=%s\n"), szOut);
	return szOut;
}

//10进制字符串反转16进制字符串
CString CCardAnalyst::DecRevsToHex( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	unsigned long nCardNo = _tstoi64(szCardNo);
	nCardNo = ((nCardNo&0xFF000000)>>24) | ((nCardNo&0x00FF0000)>>8) | ((nCardNo&0x0000FF00)<<8) | ((nCardNo&0x000000FF)<<24);//反转
	szCardNo.Format(_T("%08x"), nCardNo);
	szCardNo.MakeUpper();
	//_tprintf(_T("CAnalyst::DecRevsToHex,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}
//10进制字符串反转10进制字符串
CString CCardAnalyst::DecRevsToDec( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	unsigned long nCardNo = _tstoi64(szCardNo);
	nCardNo = ((nCardNo&0xFF000000)>>24) | ((nCardNo&0x00FF0000)>>8) | ((nCardNo&0x0000FF00)<<8) | ((nCardNo&0x000000FF)<<24);//反转
	szCardNo.Format(_T("%08x"), nCardNo);
	szCardNo.MakeUpper();
	szCardNo = HexToDec(szCardNo);
	//_tprintf(_T("CAnalyst::DecRevsToDec,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}
//16进制字符串反转10进制字符串
CString CCardAnalyst::HexRevsToDec( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	szCardNo = DecRevsToDec(HexToDec(szCardNo));
	//_tprintf(_T("CAnalyst::HexRevsToDec,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}
//16进制字符串反转16进制字符串
CString CCardAnalyst::HexRevsToHex( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	CString lleft = szCardNo;
	if(lleft.GetLength() / 2 == 1)
	{//前面补0
		lleft = _T("0") + lleft;
	}
	CString szNew = _T("");
	while(lleft.GetLength() >= 2)
	{
		CString right2 = lleft.Right(2);
		lleft = lleft.Left(lleft.GetLength() - 2);
		szNew.Append(right2);
	}
	//_tprintf(_T("CAnalyst::HexRevsToHex,szNew=%s\n"), szNew);
	return szNew;
}
CString CCardAnalyst::Intercept( CString& szParam )
{
	//_tprintf(_T("CCardAnalyst::Intercept,1,szParam=%s\n"), szParam);
	CString szCardNo = szParam;
	int index = szCardNo.ReverseFind(',');
	if(index > 0)
	{
		CString card = szCardNo.Left(index);
		int nLen = _tstoi(szCardNo.Right(szCardNo.GetLength()-index-1));

		if(card.Find('(') > 0)
		{
			card = HandleCardNo(card);
		}
		szCardNo = card.Right(nLen);
		//_tprintf(_T("CCardAnalyst::Intercept,2,szCardNo=%s\n"), szCardNo);
		if(szCardNo.GetLength() < nLen)
		{
			CString szFormat;
			szFormat.Format(_T("%%0%dd%%s"),nLen-szCardNo.GetLength());
			CString tmp = szCardNo;
			szCardNo.Format(szFormat, 0, tmp);
		}
		//_tprintf(_T("CCardAnalyst::Intercept,3,szCardNo=%s\n"), szCardNo);
	}
	//_tprintf(_T("CAnalyst::Intercept,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

CString CCardAnalyst::Wiegand34To26( CString& szParam )
{
	CString szCardNo = szParam;
	if(szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	unsigned long lCardInt = _tcstoul(szCardNo,NULL,10);
	unsigned long lNewCard	= ((lCardInt % (256 * 256)) + (lCardInt / (256 * 256) % 256) * 100000);
	szCardNo.Format(_T("%08u"), lNewCard);
	return szCardNo;
}

BOOL CCardAnalyst::CheckValidate( CString& szParam )
{
	CString szFun = _T("");
	CString szNewParam = szParam;
	int leftkuohao = 0;
	int rightkuohao = 0;
	TCHAR pBuf[200] = {0};
	_tcscpy(pBuf, szParam.GetString());
	int nIndex = 0;
	//判断括顺序是否正确
	while(nIndex < 200)
	{
		if(*(pBuf+nIndex) == '(')
		{
			leftkuohao += 1;
		}
		else if(*(pBuf+nIndex) == ')')
		{
			rightkuohao += 1;
		}
		nIndex++;
		if(leftkuohao < rightkuohao)
		{
			//_tprintf(_T("括号顺序不对，左括号%d个，右括号%d个\n"), leftkuohao, rightkuohao);
			return FALSE;
		}
	}
	//判断括号个数是否匹配
	if(leftkuohao != rightkuohao || leftkuohao < 1)
	{
		//_tprintf(_T("括号个数不对，左括号%d个，右括号%d个\n"), leftkuohao, rightkuohao);
		return FALSE;
	}
	//判断函数名以及函数入参是否正确
	while(TRUE)
	{
		if(szNewParam.Find('(') > 0)
		{
			CString szNewParam2;
			if(!Split(szNewParam, szFun, szNewParam2))
			{
				//_tprintf(_T("参数分离错误，szNewParam=%s\n"), szNewParam);
				return FALSE;
			}
			if(szFun.CompareNoCase(_T("DecToHex")) != 0
				&& szFun.CompareNoCase(_T("HexToDec")) != 0
				&& szFun.CompareNoCase(_T("DecRevsToHex")) != 0
				&& szFun.CompareNoCase(_T("DecRevsToDec")) != 0
				&& szFun.CompareNoCase(_T("HexRevsToDec")) != 0
				&& szFun.CompareNoCase(_T("HexRevsToHex")) != 0
				&& szFun.CompareNoCase(_T("Intercept")) != 0
				&& szFun.CompareNoCase(_T("Wiegand34To26")) != 0)
			{
				//_tprintf(_T("未知的方法名 %s\n"), szFun);
				return FALSE;
			}
			if(szFun.CompareNoCase(_T("Intercept")) == 0)
			{
				int index = szNewParam2.Find(',');
				if(index > 0 && index < szNewParam2.GetLength())
				{
					szNewParam = szNewParam2.Left(index);
				}
				else
				{
					//_tprintf(_T("Intercept方法的参数个数不正确，szNewParam=%s\n"), szNewParam);
					return FALSE;
				}
			}
			szNewParam = szNewParam2;
		}
		{
			break;
		}
	}
	return TRUE;
}

CString CCardAnalyst::AddToLeft( CString& szParam )
{
	//_tprintf(_T("CCardAnalyst::AddToLeft,1,szParam=%s\n"), szParam);
	CString szCardNo = szParam;
	int index = szCardNo.ReverseFind(',');
	if(index > 0)
	{
		CString card = szCardNo.Left(index);
		CString part2 = szCardNo.Right(szCardNo.GetLength()-index-1);
		part2.Replace(_T("\""),_T(""));

		if(card.Find('(') > 0)
		{
			card = HandleCardNo(card);
		}
		szCardNo.Format(_T("%s%s"), part2, card);
		//_tprintf(_T("CCardAnalyst::AddToLeft,2,szCardNo=%s\n"), szCardNo);
	}
	//_tprintf(_T("CAnalyst::Intercept,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

CString CCardAnalyst::AddToRight( CString& szParam )
{
	//_tprintf(_T("CCardAnalyst::AddToRight,1,szParam=%s\n"), szParam);
	CString szCardNo = szParam;
	int index = szCardNo.ReverseFind(',');
	if(index > 0)
	{
		CString card = szCardNo.Left(index);
		CString part2 = szCardNo.Right(szCardNo.GetLength()-index-1);
		part2.Replace(_T("\""),_T(""));

		if(card.Find('(') > 0)
		{
			card = HandleCardNo(card);
		}
		szCardNo.Format(_T("%s%s"), card, part2);
		//_tprintf(_T("CCardAnalyst::AddToRight,2,szCardNo=%s\n"), szCardNo);
	}
	//_tprintf(_T("CAnalyst::Intercept,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

CString CCardAnalyst::Concat( CString& szParam )
{
	//算法有问题，暂不提倡使用
	CString szCardNo = szParam;
	TCHAR* cParam = szCardNo.GetBuffer();
	int nLeft = 0;
	int nRight = 0;
	int nIndex = 0;
	for(int i = 0; i < _tcslen(cParam); i++)
	{
		if(*(cParam+i) == '(')
		{
			nLeft++;
		}
		else if(*(cParam+i) == ')')
		{
			nRight++;
		}
		else if(*(cParam+i) == ',' && nLeft==nRight)
		{
			nIndex = i;
			break;
		}
	}
	szCardNo.ReleaseBuffer();
	//_tprintf(_T("CAnalyst::Concat,nIndex=%d\n"), nIndex);
	if(nIndex > 0)
	{
		CString str1 = szCardNo.Left(nIndex);
		CString str2 = szCardNo.Right(szCardNo.GetLength()-nIndex-1);
		if(str1.Find('(') > 0)
		{
			str1 = HandleCardNo(str1);
		}
		else
		{
			str1.Replace(_T("\""),_T(""));
		}
		if(str2.Find('(') > 0)
		{
			str2 = HandleCardNo(str2);
		}
		else
		{
			str2.Replace(_T("\""),_T(""));
		}
		szCardNo.Format(_T("%s%s"), str1, str2);
	}
	return szCardNo;
}

//截取左边位数，不够则在左侧补零
CString CCardAnalyst::InterceptLeft( CString& szParam )
{
	//_tprintf(_T("CCardAnalyst::Intercept,1,szParam=%s\n"), szParam);
	CString szCardNo = szParam;
	int index = szCardNo.ReverseFind(',');
	if(index > 0)
	{
		CString card = szCardNo.Left(index);
		int nLen = _tstoi(szCardNo.Right(szCardNo.GetLength()-index-1));

		if(card.Find('(') > 0)
		{
			card = HandleCardNo(card);
		}
		szCardNo = card.Left(nLen);
		//_tprintf(_T("CCardAnalyst::Intercept,2,szCardNo=%s\n"), szCardNo);
		if(szCardNo.GetLength() < nLen)
		{
			CString szFormat;
			szFormat.Format(_T("%%0%dd%%s"),nLen-szCardNo.GetLength());
			CString tmp = szCardNo;
			szCardNo.Format(szFormat, 0, tmp);
		}
		//_tprintf(_T("CCardAnalyst::Intercept,3,szCardNo=%s\n"), szCardNo);
	}
	_tprintf(_T("CAnalyst::Intercept,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

//截取右边位数，不够则在左侧补零
CString CCardAnalyst::InterceptRight( CString& szParam )
{
	//_tprintf(_T("CCardAnalyst::Intercept,1,szParam=%s\n"), szParam);
	CString szCardNo = szParam;
	int index = szCardNo.ReverseFind(',');
	if(index > 0)
	{
		CString card = szCardNo.Left(index);
		int nLen = _tstoi(szCardNo.Right(szCardNo.GetLength()-index-1));

		if(card.Find('(') > 0)
		{
			card = HandleCardNo(card);
		}
		szCardNo = card.Right(nLen);
		//_tprintf(_T("CCardAnalyst::Intercept,2,szCardNo=%s\n"), szCardNo);
		if(szCardNo.GetLength() < nLen)
		{
			CString szFormat;
			szFormat.Format(_T("%%0%dd%%s"),nLen-szCardNo.GetLength());
			CString tmp = szCardNo;
			szCardNo.Format(szFormat, 0, tmp);
		}
		//_tprintf(_T("CCardAnalyst::Intercept,3,szCardNo=%s\n"), szCardNo);
	}
	_tprintf(_T("CAnalyst::Intercept,szCardNo=%s\n"), szCardNo);
	return szCardNo;
}

CString CCardAnalyst::StrRevsToStr(CString& szParam)
{
	CString szCardNo = szParam;
	if (szParam.Find('(') > 0)
	{
		szCardNo = HandleCardNo(szParam);
	}
	if (szCardNo.GetLength() % 2 > 0)
	{
		szCardNo = _T("0") + szCardNo;
	}
	int nStrLen = szCardNo.GetLength();
	CString szTemp(_T(""));
	for (int i = nStrLen-2; i >= 0; i -= 2)
	{
		szTemp.AppendFormat(_T("%s"), szCardNo.Mid(i, 2));
	}
	//_tprintf(_T("CAnalyst::HexRevsToDec,szCardNo=%s\n"), szCardNo);
	return szTemp;
}
