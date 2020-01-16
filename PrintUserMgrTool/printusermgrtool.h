#ifndef PRINTUSERMGRTOOL_H
#define PRINTUSERMGRTOOL_H

#include "StdAfx.h"
#include <QDialog>
#include "ui_printusermgrtool.h"
#include "UsbMinHuaCardReader.h"

class CLockCS
{
public:
	explicit CLockCS(CRITICAL_SECTION& cs): m_cs(cs)
	{
		EnterCriticalSection(&m_cs);
	}
	~CLockCS()
	{
		LeaveCriticalSection(&m_cs);
	}
	CRITICAL_SECTION&  m_cs;
};

class PrintUserMgrTool : public QDialog, public Ui::PrintUserMgrToolClass
{
	Q_OBJECT

public:
	PrintUserMgrTool(QWidget *parent = 0);
	~PrintUserMgrTool();

protected:
	void showEvent(QShowEvent* event);
	void closeEvent(QCloseEvent *event);
	void AddRow(UserInfo *pInfo, int nRow);
	void AddSelectList(UserInfo *pInfo);
	void ShowSelectRowInfo();
	void EnableUI();

protected slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_currentIndexChanged(int index);
	void on_tableWidget_clicked(const QModelIndex& index);
	void on_readCardTimer_timeout();

protected:
	int m_bConnected;
	CUsbMinHuaCardReader m_CardReader;
	QTimer* readCardTimer;
	CString m_szCurrentCardNo;
	UserInfoList m_UserList;
	UserInfoList m_UserSelectList;
	CRITICAL_SECTION m_cs;
};

#endif // PRINTUSERMGRTOOL_H
