#include "stdafx.h"
#include "printusermgrtool.h"
#include "logindialog.h"
#include <QApplication>

CFileLog theLog;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//QTextCodec *codec = QTextCodec::codecForName("GBK");
	//QTextCodec::setCodecForCStrings(codec);
	//QTextCodec::setCodecForTr(codec);

	// QMessageBox的按钮中文化需要
	QTranslator translator(0);
	translator.load(":/PrintUserMgrTool/Resources/qt_zh_CN.qm");
	a.installTranslator(&translator); 

	if(argc==2 && stricmp(argv[1], "/install")==0) 
	{
		CCommonFun::CreateShellLink(CCommonFun::GetDefaultPath()+"PrintUserMgrTool.exe", 
			CCommonFun::GetCommonDesktopDir()+"\\用户卡管理工具.lnk", CCommonFun::GetDefaultPath()+"res\\PrintUserMgrTool.ico", 0, "");
		return 0;	
	} 
	else if(argc==2 && stricmp(argv[1], "/uninstall")==0) 
	{
		CString sdsk = CCommonFun::GetCommonDesktopDir();
		if(PathFileExists(sdsk+"\\用户卡管理工具.lnk"))
			CFile::Remove(sdsk+"\\用户卡管理工具.lnk");

		return 0;
	}

	theLog.SetLogName("PrintUserMgrTool");

	LoginDialog dlg;
	if(dlg.exec() != QDialog::Accepted)
		return 1;

	PrintUserMgrTool w;
	w.show();
	return a.exec();
}
