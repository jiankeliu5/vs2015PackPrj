#include "StdAfx.h"
#include "logindialog.h"
#include "QConfigDlg.h"

LoginDialog::LoginDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	connect(this, SIGNAL(beginConn()), this, SLOT(OnbeginConn()), Qt::QueuedConnection);

	connTimer = new QTimer(this);
	connect(connTimer, SIGNAL(timeout()), this, SLOT(on_connTimer_timeout()));
}

LoginDialog::~LoginDialog()
{
	if (m_bConnected)
		CAdoFactory::Release();
	if (connTimer)
	{
		connTimer->stop();
		delete connTimer;
	}
}

void LoginDialog::showEvent(QShowEvent * event)
{
	labelLogin->setText(tr("正在连接..."));
	if (connTimer)
	{
		connTimer->start(500);
	}
}

void LoginDialog::on_labelConn_linkActivated(const QString & link)
{
	QConfigDlg dlg;
	if(dlg.exec()==QDialog::Accepted) 
	{
		CIniFile theIniFile;
		theIniFile.m_sPath.Format("%s%s",CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
		theIniFile.SetVal("DbConfig","DbName",dlg.m_qsDbName.toStdString().c_str());
		theIniFile.SetVal("DbConfig","DbSvr",dlg.m_qsDbSvr.toStdString().c_str());
		theIniFile.SetVal("DbConfig","DbPort",dlg.m_qsDbPort.toStdString().c_str());
		theIniFile.SetVal("DbConfig","DbUser",dlg.m_qsDbUser.toStdString().c_str());
#ifdef ENABLE_PWD_CRY
		CPWDCry cry;
		CString sPwd(dlg.m_qsDbPwd.toStdString().c_str());
		char* pPsw  = cry.CryptPWD(sPwd.GetBuffer());
		sPwd.ReleaseBuffer();
		sPwd.Format("%s",pPsw);
		delete pPsw;
		theIniFile.SetVal("DBConfig","DbPwd",sPwd);
#else
		theIniFile.SetVal("DbConfig","DbPwd",dlg.m_qsDbPwd.toStdString().c_str());
#endif
		labelLogin->setText(tr("正在连接..."));
		if (connTimer)
		{
			connTimer->start(500);
		}
	}
}

void LoginDialog::on_pushOk_clicked()
{
	CString szUserName = comboName->currentText().toStdString().c_str();
	if(szUserName == "") 
	{
		QMessageBox::warning(this, "登录", "用户名不能为空。");
		return;
	}

	CString szPwd = editPwd->text().toStdString().c_str();
	pushOk->setEnabled(false);
	theLog.Write("LoginDialog::on_pushOk_clicked,1,user=%s", szUserName);
	if (CUserAndDepartmentMgrSvc::GetInstance().CheckUser(szUserName, szPwd))
	{
		accept();
	}
	else
	{
		theLog.Write("!!LoginDialog::on_pushOk_clicked, CheckUser fail, szUserName=%s", szUserName);
		labelLogin->setText(tr("登录失败!"));
		pushOk->setEnabled(true);
	}
}

void LoginDialog::enableControls(bool enable)
{
	comboName->setEnabled(enable);
	editPwd->setEnabled(enable);
	pushOk->setEnabled(enable);
}


BOOL LoginDialog::ConnDB()
{
	ADO_INFO ado;
	CIniFile theIniFile;
	theIniFile.m_sPath.Format("%s%s",CCommonFun::GetDefaultPath(),SYS_CONFIG_FILE);
	strcpy(ado.szDbName,theIniFile.GetVal("DbConfig","DbName","iSec_Print5"));
	strcpy(ado.szIP,theIniFile.GetVal("DbConfig","DbSvr","192.168.2.15"));
	strcpy(ado.szPort,theIniFile.GetVal("DbConfig","DbPort","1433"));
	strcpy(ado.szUser,theIniFile.GetVal("DbConfig","DbUser","sa"));
	CString sDbPwd = theIniFile.GetVal("DbConfig","DbPwd","sa");

#ifdef ENABLE_PWD_CRY
	char szPwd[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	CPWDCry cry;
	cry.UnCryptPWD(sDbPwd.GetBuffer(),szPwd,nSize);
	sDbPwd.ReleaseBuffer();
	strcpy(ado.szPwd,szPwd);
#else
	strcpy(ado.szPwd,sDbPwd);
#endif
	m_bConnected = (CAdoFactory::CreateAdo(&ado) != NULL);

	if(!m_bConnected)
	{
		labelLogin->setText(tr("连接失败!"));
		enableControls(false);
		theLog.Write("!!LoginDialog::ConnDB,1,m_bConnected=%d", m_bConnected);
	}
	else
	{
		enableControls(true);
		labelLogin->setText(tr("已连接."));
	}
	return m_bConnected;
}

void LoginDialog::OnbeginConn()
{
	ConnDB();	//连接数据库
}

void LoginDialog::on_connTimer_timeout()
{
	connTimer->stop();
	emit beginConn();
}