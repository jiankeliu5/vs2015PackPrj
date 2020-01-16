#pragma once

#include <QtWidgets/QWidget>
#include "ui_PrintFileTest.h"

class PrintFileTest : public QWidget,public Ui::PrintFileTestClass,public IPrtStationPrintCallback
{
	Q_OBJECT

public:
	PrintFileTest(QWidget *parent = Q_NULLPTR);
	~PrintFileTest();

	void showEvent(QShowEvent *event);
	void EnumeratePrinters(CStringArray& ary);


	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
protected:
	CString GetPrinterPort(CString szPrnterName);
private slots:
	void on_pushButton_browser_clicked();
	void on_pushButton_print_clicked();
	void on_pushButton_paper_clicked();
	void on_pushButton_input_clicked();
	void on_pushButton_other_clicked();

	
};
