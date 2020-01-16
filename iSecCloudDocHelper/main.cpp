#include "stdafx.h"
#include "iSecCloudDocHelper.h"
#include <QApplication>

#pragma data_seg("iSecCloudDocHelper_data") 
long g_lSession[100] = {0};
#pragma data_seg() 
#pragma comment(linker,"/Section:iSecCloudDocHelper_data,rws")

CFileLog theLog;
CIniFile theIniFile;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	CMiniDumper::Enable(_T("iSecCloudDocHelper"), false);	//����ʱ����dump

	theLog.SetLogName(_T("iSecCloudDocHelper"));
	theLog.EnableLog(IsDebug());

	theIniFile.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);

	//QTextCodec *codec = QTextCodec::codecForName(_T("GBK"));
	//QTextCodec::setCodecForCStrings(codec);
	//QTextCodec::setCodecForTr(codec);

	// QMessageBox�İ�ť���Ļ���Ҫ
	QTranslator translator(0);
	translator.load(":/iSecCloudDocHelper/Resources/qt_zh_CN.qm");
	a.installTranslator(&translator); 

	if (!RunOnce(GLOBAL_MUTEX_NAME))
	{
		theLog.Write(_T("iSecCloudDocHelper,�Ѿ�������ʵ�����У��˳������̡�"));
		return FALSE;
	}

	if(argc==2 && stricmp(argv[1], "/install")==0) 
	{
		CCommonFun::CreateShellLink(CCommonFun::GetDefaultPath()+_T("iSecCloudDocHelper.exe"), 
			CCommonFun::GetCommonDesktopDir()+_T("\\�ƴ�ӡ���ݿ�����.lnk"), CCommonFun::GetDefaultPath()+_T("res\\iSecCloudDocHelper.ico"), 0, _T(""));
		return 0;	
	} 
	else if(argc==2 && stricmp(argv[1], "/uninstall")==0) 
	{
		CString sdsk = CCommonFun::GetCommonDesktopDir();
		if(PathFileExists(sdsk+_T("\\�ƴ�ӡ���ݿ�����.lnk")))
			CFile::Remove(sdsk+_T("\\�ƴ�ӡ���ݿ�����.lnk"));

		return 0;
	}
	else if (argc==2 &&  stricmp(argv[1], "/auto")==0)
	{
		iSecCloudDocHelper w;
		w.hide();
		w.StartWork();
		return a.exec();
	}
	else/* if (argc==2 &&  stricmp(argv[1], "/config")==0)*/
	{
		iSecCloudDocHelper w;
		w.show();
		return a.exec();
	}
}
