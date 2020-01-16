#include "stdafx.h"
#include "PrintFileTest.h"
#include <QtWidgets/QApplication>
CFileLog theLog;
CIniFile theIniFile;


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	CMiniDumper::Enable(_T("PrintFileTest"),true);
	theLog.SetLogName(_T("PrintFileTest"));
	theLog.EnableLog(TRUE);
	//theIniFile.m_sPath.Format(_T("%s%s"),CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);


	PrintFileTest w;
	w.show();
	return a.exec();
}
