#ifndef QCONFIGDLG_H
#define QCONFIGDLG_H

#include <QDialog>
#include "ui_QConfigDlg.h"

class QConfigDlg : public QDialog, public Ui::QConfigDlg
{
	Q_OBJECT

public:
	QConfigDlg(QWidget *parent = 0);
	~QConfigDlg();

	void accept();
	void showEvent(QShowEvent * event);

public:
	QString m_qsDbName;
	QString m_qsDbSvr;
	QString m_qsDbPort;
	QString m_qsDbUser;
	QString m_qsDbPwd;
};

#endif // QCONFIGDLG_H
