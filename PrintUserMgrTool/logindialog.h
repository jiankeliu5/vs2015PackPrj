#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "stdafx.h"
#include <QDialog>
#include "ui_logindialog.h"

class LoginDialog : public QDialog, public Ui::LoginDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = 0);
	~LoginDialog();

protected slots:
	void on_labelConn_linkActivated(const QString & link);
	void on_pushOk_clicked();
	void showEvent(QShowEvent * event);
	void OnbeginConn();
	void on_connTimer_timeout();

protected:
	QTimer* connTimer;
	int m_bConnected;

	BOOL ConnDB();
	void enableControls(bool enable);

signals:
	void beginConn();
};

#endif // LOGINDIALOG_H
