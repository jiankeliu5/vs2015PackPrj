#pragma once
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

// ������뽫λ������ָ��ƽ̨֮ǰ��ƽ̨��ΪĿ�꣬���޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "../Public/Global.h"
#include "../Public/CmmLibport.h"
#include "../Public/Include/glCmdAndStructDef.h"
#include "../Public/Include/SystemDef.h"
#include "../Public/eMsgTransExport.h"
#include "../Public/messageport.h"
using namespace isafetec;
#include "../Public/Include/NetCmdAndStructDef.h"
#include "../Public/Netport.h"
#include "../Public/Aaport.h"

#if 1
#include "../Public/Global.h"
#include "../Public/Include/glCmdAndStructDef.h"
#include "../Public/messageport.h"
using namespace isafetec;
#include "../Public/CmmLibport.h"
#include "../Public/DataSvcExport.h"
#endif

// header file
#include <activeds.h>
#include <Iads.h>
#include <Lm.h>
#include <Adshlp.h>

// lib
#pragma comment (lib, "Activeds")
#pragma comment(lib, "adsiid")

#pragma comment(lib, "Quartz.lib")

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mpr.lib")

#define ENABLE_PWD_CRY

#include "../encryptlib/PWDCry.h"
#ifdef _DEBUG
#pragma comment(lib,"../debug/encryptlib.lib")
#else
#pragma comment(lib,"../release/encryptlib.lib")
#endif

extern CFileLog theLog;

#include <QtGui>
#include <QMessageBox>