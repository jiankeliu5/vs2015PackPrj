#include "StdAfx.h"
#include "printusermgrtool.h"

PrintUserMgrTool::PrintUserMgrTool(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_szCurrentCardNo = "";

	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));

	readCardTimer = new QTimer(this);
	connect(readCardTimer, SIGNAL(timeout()), this, SLOT(on_readCardTimer_timeout()));

	InitializeCriticalSection(&m_cs);
}

PrintUserMgrTool::~PrintUserMgrTool()
{
	DeleteCriticalSection(&m_cs);
}

void PrintUserMgrTool::showEvent(QShowEvent* event)
{
	label_7->setText(tr("当前用户信息: (未先定用户)"));
	//初始化显示未绑定卡的用户
	comboBox->setCurrentIndex(2);
}

void PrintUserMgrTool::closeEvent(QCloseEvent *event)
{
	QMessageBox::StandardButton btn = QMessageBox::question(this, "提示", "确定要退出本工具吗?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

	if(btn==QMessageBox::Yes) 
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void PrintUserMgrTool::AddRow(UserInfo *pInfo, int nRow)
{
	if (pInfo)
	{
		tableWidget->insertRow(nRow);
		int nAccountId = pInfo->accountid();
		int nDeptId = pInfo->depid();

		//姓名
		QTableWidgetItem* pItem2 = new QTableWidgetItem(QString::fromStdString(pInfo->username()));
		pItem2->setData(Qt::UserRole, nAccountId);
		tableWidget->setItem(nRow, 0, pItem2);

		//账号
		QTableWidgetItem* pItem1 = new QTableWidgetItem(QString::fromStdString(pInfo->accountname()));
		pItem1->setData(Qt::UserRole, nDeptId);
		tableWidget->setItem(nRow, 1, pItem1);
		

		//卡号
		QTableWidgetItem* pItem3 = new QTableWidgetItem(QString::fromStdString(pInfo->card_no()));
		tableWidget->setItem(nRow, 2, pItem3);

		//Email
		QTableWidgetItem* pItem4 = new QTableWidgetItem(QString::fromStdString(pInfo->emailaddr()));
		tableWidget->setItem(nRow, 3, pItem4);

		//电话
		QTableWidgetItem* pItem5 = new QTableWidgetItem(QString::fromStdString(pInfo->telephone()));
		tableWidget->setItem(nRow, 4, pItem5);

		//部门
		QTableWidgetItem* pItem6 = new QTableWidgetItem(QString::fromStdString(pInfo->password()));
		tableWidget->setItem(nRow, 5, pItem6);

		//启用状态
		QTableWidgetItem* pItem7 = new QTableWidgetItem(pInfo->state()==1?tr("启用"):tr("禁用"));
		tableWidget->setItem(nRow, 6, pItem7);
	} 
	else
	{
		theLog.Write("!!PrintUserMgrTool::AddRow, nRow=%d", nRow);
	}

}

//开始发卡
void PrintUserMgrTool::on_pushButton_clicked()
{
	if (pushButton->text() == tr("开始发卡"))
	{
		tableWidget->setCurrentCell(0,0);
		pushButton->setText(tr("停止发卡"));
		readCardTimer->start(1000); //开始读取卡信息
	}
	else if (pushButton->text() == tr("停止发卡"))
	{
		pushButton->setText(tr("开始发卡"));
		readCardTimer->stop(); //停止读取卡信息
		QMessageBox::warning(this, tr("提示"), tr("发卡完成."));
	}
	EnableUI();
}

//取消卡绑定
void PrintUserMgrTool::on_pushButton_2_clicked()
{
	CLockCS ow(m_cs);
	int nRow = tableWidget->currentRow();
	if (nRow>=0)
	{
		int nAccountId = tableWidget->item(nRow,0)->data(Qt::UserRole).toInt();
		if (CUserAndDepartmentMgrSvc::GetInstance().BindUserIdAndCardId(nAccountId, ""))//设置绑定卡号为空
		{
			tableWidget->removeRow(nRow);
		}
		else
		{
			theLog.Write("!!PrintUserMgrTool::on_pushButton_2_clicked, BindUserIdAndCardId,fail,nAccountId=%d", nAccountId);
		}
	}
}

//刷新
void PrintUserMgrTool::on_pushButton_3_clicked()
{
	on_currentIndexChanged(comboBox->currentIndex());
}

//快速查找
void PrintUserMgrTool::on_pushButton_4_clicked()
{
	CLockCS ow(m_cs);
	QString sKey = lineEdit->text().trimmed();
	if (sKey.isEmpty())
	{
		return;
	}

	int nIndex = comboBox_2->currentIndex();
	if (nIndex < 0)
	{
		return;
	}

	while (tableWidget->rowCount() > 0)
		tableWidget->removeRow(0);

	int nCount = m_UserSelectList.list_size();
	UserInfo *pInfo = NULL;
	int nRow = 0;
	for (int i=0; i<nCount; i++)
	{
		pInfo = m_UserSelectList.mutable_list(i);
		if (pInfo)
		{
			switch (nIndex)
			{
			case 0:
				if (QString::fromStdString(pInfo->username()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			case 1:
				if (QString::fromStdString(pInfo->accountname()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			case 2:
				if (QString::fromStdString(pInfo->card_no()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			case 3:
				if (QString::fromStdString(pInfo->emailaddr()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			case 4:
				if (QString::fromStdString(pInfo->telephone()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			case 5:
				if (QString::fromStdString(pInfo->password()).indexOf(sKey)!=-1)
				{
					AddRow(pInfo, nRow++);
				}
				break;
			default:
				break;
			}
		}
	}
}

void PrintUserMgrTool::AddSelectList(UserInfo *pInfo)
{
	//将本用户记录添加到当前的选定类别列表中
	UserInfo *User = m_UserSelectList.add_list();
	User->CopyFrom(*pInfo);
}

void PrintUserMgrTool::on_currentIndexChanged(int index)
{
	CLockCS ow(m_cs);
	m_UserList.clear_list();
	m_UserSelectList.clear_list();
	if (CUserAndDepartmentMgrSvc::GetInstance().GetAllUserInfo(m_UserList))
	{
		while (tableWidget->rowCount() > 0)
			tableWidget->removeRow(0);

		int nCount = m_UserList.list_size();
		int j = 0;
		for (int i=0; i<nCount; i++)
		{
			UserInfo *pInfo = m_UserList.mutable_list(i);
			if (QString::fromStdString(pInfo->accountname()) == tr("Guest"))
			{
				continue;
			}

			if (index == 0)
			{
				AddSelectList(pInfo);
				AddRow(pInfo, j++);
			}
			else if (index == 1)
			{
				if (!QString::fromStdString(pInfo->card_no()).isEmpty())
				{
					AddSelectList(pInfo);
					AddRow(pInfo, j++);
				}
			}
			else if (index == 2)
			{
				if (QString::fromStdString(pInfo->card_no()).isEmpty())
				{
					AddSelectList(pInfo);
					AddRow(pInfo, j++);
				}
			}
		}

		if(tableWidget->rowCount() > 0)
			tableWidget->setCurrentCell(0,0);
		ShowSelectRowInfo();
		EnableUI();
	}
	else
	{
		theLog.Write("!!PrintUserMgrTool::on_currentIndexChanged,GetAllUserInfo fail,index=%d", index);
	}
}

void PrintUserMgrTool::on_tableWidget_clicked(const QModelIndex& index)
{
	ShowSelectRowInfo();
	EnableUI();
}

void PrintUserMgrTool::ShowSelectRowInfo()
{
	if(tableWidget->rowCount() == 0)
		return;

	int nIndex = tableWidget->currentRow();

	QString sInfo, sTemp;
	sTemp = "姓名:" + tableWidget->item(nIndex,0)->text() + "\t";
	sInfo += sTemp;
	sTemp = "账号:" + tableWidget->item(nIndex,1)->text() + "\t";
	sInfo += sTemp;
	sTemp = "卡号:" + tableWidget->item(nIndex,2)->text() + "\t\t\n";
	sInfo += sTemp;
	sTemp = "Email:" + tableWidget->item(nIndex,3)->text() + "\t";
	sInfo += sTemp;
	sTemp = "电话:" + tableWidget->item(nIndex,4)->text() + "\t";
	sInfo += sTemp;
	sTemp = "所属部门:" + tableWidget->item(nIndex,5)->text() + "\t";
	sInfo += sTemp;
	sTemp = "启用状态:" + tableWidget->item(nIndex,6)->text() + "\t";
	sInfo += sTemp;
	label_7->setText(sInfo);
}

void PrintUserMgrTool::EnableUI()
{
	int nIndex = comboBox->currentIndex();
	pushButton->setEnabled((nIndex!=2)?false:true);

	if (tableWidget->rowCount()>0)
	{
		int nRow = tableWidget->currentRow();
		bool bEnable = tableWidget->item(nRow,2)->text() != tr("");
		pushButton_2->setEnabled(bEnable);
	}

	if (pushButton->text() == tr("停止发卡"))
	{
		comboBox->setEnabled(false);
		pushButton_2->setEnabled(false);
		pushButton_3->setEnabled(false);
// 		pushButton_4->setEnabled(false);
	}
	else if (pushButton->text() == tr("开始发卡"))
	{
		comboBox->setEnabled(true);
		pushButton_2->setEnabled(true);
		pushButton_3->setEnabled(true);
// 		pushButton_4->setEnabled(true);
	}

}

void PrintUserMgrTool::on_readCardTimer_timeout()
{
	//获取一卡数据成功
	CString sNo;
	if (m_CardReader.ReadUsbCard(sNo) && !sNo.IsEmpty())
	{
		if (m_szCurrentCardNo != sNo)
		{
			int nRow = tableWidget->currentRow();
			int nAccountId = tableWidget->item(nRow,0)->data(Qt::UserRole).toInt();
			if (!CUserAndDepartmentMgrSvc::GetInstance().BindUserIdAndCardId(nAccountId, sNo))
			{
				theLog.Write("!!PrintUserMgrTool::on_readCardTimer_timeout, BindUserIdAndCardId,fail,nAccountId=%d,sNo=%s", nAccountId, sNo);
			}
			else
			{
				tableWidget->item(nRow, 2)->setText(sNo.GetString());
				m_szCurrentCardNo = sNo;
				if (nRow == tableWidget->rowCount()-1)
				{
					on_pushButton_clicked();
				}
				else
				{
					tableWidget->setCurrentCell(nRow+1,0);
				}
			}
		}
	}
}