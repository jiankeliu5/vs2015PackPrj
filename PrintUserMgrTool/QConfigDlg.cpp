#include "stdafx.h"
#include "QConfigDlg.h"

QConfigDlg::QConfigDlg(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	m_qsDbName = "";
	m_qsDbSvr = "";
	m_qsDbPort = "";
	m_qsDbUser = "";
	m_qsDbPwd = "";
}

QConfigDlg::~QConfigDlg()
{

}

void QConfigDlg::accept()
{
	m_qsDbName = lineEdit_5->text().trimmed();
	m_qsDbSvr = lineEdit->text().trimmed();
	m_qsDbPort = lineEdit_3->text().trimmed();
	m_qsDbUser = lineEdit_2->text().trimmed();
	m_qsDbPwd = lineEdit_4->text().trimmed();
	QDialog::accept();
}

void QConfigDlg::showEvent(QShowEvent * event)
{
	CIniFile theIniFile;
	theIniFile.m_sPath.Format("%s%s",CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
	CString sTemp = "";

	sTemp = theIniFile.GetVal("DbConfig","DbName","iSec_Print5");
	lineEdit_5->setText(QString::fromLocal8Bit(sTemp.GetString()));

	sTemp = theIniFile.GetVal("DbConfig","DbSvr","192.168.2.15");
	lineEdit->setText(QString::fromLocal8Bit(sTemp.GetString()));

	sTemp = theIniFile.GetVal("DbConfig","DbPort","1433");
	lineEdit_3->setText(QString::fromLocal8Bit(sTemp.GetString()));

	sTemp = theIniFile.GetVal("DbConfig","DbUser","sa");
	lineEdit_2->setText(QString::fromLocal8Bit(sTemp.GetString()));

	sTemp = theIniFile.GetVal("DbConfig","DbPwd","sa");
#ifdef ENABLE_PWD_CRY
	char szPwd[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	CPWDCry cry;
	cry.UnCryptPWD(sTemp.GetBuffer(),szPwd,nSize);
	sTemp.ReleaseBuffer();
	sTemp = szPwd;
#endif
	lineEdit_4->setText(QString::fromLocal8Bit(sTemp.GetString()));
}

