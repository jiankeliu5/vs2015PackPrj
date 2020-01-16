#include "StdAfx.h"
#include "ImageDeal.h"

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken = 0;

CString GetFileExt(CString sFile)
{
	CString sTmp;
	int nPos = sFile.ReverseFind('.');
	sTmp = sFile.Right(sFile.GetLength()-nPos-1);
	return sTmp;
}

int GetEncoderClsid(WCHAR* format, CLSID* pClsid)
{
	if (!format || !pClsid)
	{
		return -1;
	}

	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

int GetEncoderClsid(CString tiffPath, CLSID* pClsid)
{
	WCHAR* format = NULL;
	CString szSuffix = GetFileExt(tiffPath);
	if (0 == szSuffix.CompareNoCase(_T("jpg"))
		||0 == szSuffix.CompareNoCase(_T("jpeg")))
	{
		format = L"image/jpeg";
	}
	else if (0 == szSuffix.CompareNoCase(_T("tif"))
		|| 0 == szSuffix.CompareNoCase(_T("tiff")))
	{
		format = L"image/tiff";
	}
	else if (0 == szSuffix.CompareNoCase(_T("png")))
	{
		format = L"image/png";
	}
	else if (0 == szSuffix.CompareNoCase(_T("bmp")))
	{
		format = L"image/bmp";
	}
	else if (0 == szSuffix.CompareNoCase(_T("gif")))
	{
		format = L"image/gif";
	}

	return GetEncoderClsid(format, pClsid);
}

int GetEncoderClsid(PrintFileFormat pf, CLSID* pClsid)
{
	WCHAR* format = NULL;
	switch (pf)
	{
	case PrintFileFormat_JPEG:
		format = L"image/jpeg";
		break;
	case PrintFileFormat_TIFF:
		format = L"image/tiff";
		break;
	case PrintFileFormat_PNG:
		format = L"image/png";
		break;
	case PrintFileFormat_BMP:
		format = L"image/bmp";
		break;
	case PrintFileFormat_GIF:
		format = L"image/gif";
		break;
	default:
		break;
	}
	
	return GetEncoderClsid(format, pClsid);
}

//颜色转灰度的矩阵
//亮度公式 gray = R * 0.299 + G * 0.587 + B * 0.144
ColorMatrix grayMatrix = {
	0.299f, 0.299f, 0.299f, 0, 0,
	0.587f, 0.587f, 0.587f, 0, 0,
	0.114f, 0.114f, 0.114f, 0, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 0, 1
};
