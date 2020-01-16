// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <math.h>
#include <string>
#include <map>
#include <list>
using namespace std;
#include <Winspool.h>


#define _AA_DLL_
#include "../Public/Global.h"
#include "..\Public\include\glCmdAndStructDef.h"
#include "../Public/CloudPrintCommDef.h"
#include "../Public/CmmLibport.h"
#include "../Public/FileOperateHeader.h"

#include "../Public/PdfHelperDef.h"

#include "..\Public\SplHelperDef.h"
#include "PCLXL/ParsePCL.h"
#include "../Public/CurlWrapPort.h"
/*#include "../Public/messageport.h"*/
#include "HwConfig.h"
#include "CopyInterface.h"
#include "SerialCommHelper.h"
#include "COMPortHelper.h"
#include <gdiplus.h>


using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

extern CFileLog theLog;
extern CIniFile g_oIniFile;
extern int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

#ifndef HIGH32
#define HIGH32(param)	((UINT32)(param >> 32)) 
#endif

#ifndef LOW32
#define LOW32(param)	((UINT32)param)
#endif

#ifndef MAKE64
#define MAKE64(high32, low32)	(((UINT64)high32 << 32) + low32)
#endif
