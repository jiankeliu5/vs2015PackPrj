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
	label_7->setText(tr("��ǰ�û���Ϣ: (δ�ȶ��û�)"));
	//��ʼ����ʾδ�󶨿����û�
	comboBox->setCurrentIndex(2);
}

void PrintUserMgrTool::closeEvent(QCloseEvent *event)
{
	QMessageBox::StandardButton btn = QMessageBox::question(this, "��ʾ", "ȷ��Ҫ�˳���������?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

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

		//����
		QTableWidgetItem* pItem2 = new QTableWidgetItem(QString::fromStdString(pInfo->username()));
		pItem2->setData(Qt::UserRole, nAccountId);
		tableWidget->setItem(nRow, 0, pItem2);

		//�˺�
		QTableWidgetItem* pItem1 = new QTableWidgetItem(QString::fromStdString(pInfo->accountname()));
		pItem1->setData(Qt::UserRole, nDeptId);
		tableWidget->setItem(nRow, 1, pItem1);
		

		//����
		QTableWidgetItem* pItem3 = new QTableWidgetItem(QString::fromStdString(pInfo->card_no()));
		tableWidget->setItem(nRow, 2, pItem3);

		//Email
		QTableWidgetItem* pItem4 = new QTableWidgetItem(QString::fromStdString(pInfo->emailaddr()));
		tableWidget->setItem(nRow, 3, pItem4);

		//�绰
		QTableWidgetItem* pItem5 = new QTableWidgetItem(QString::fromStdString(pInfo->telephone()));
		tableWidget->setItem(nRow, 4, pItem5);

		//����
		QTableWidgetItem* pItem6 = new QTableWidgetItem(QString::fromStdString(pInfo->password()));
		tableWidget->setItem(nRow, 5, pItem6);

		//����״̬
		QTableWidgetItem* pItem7 = new QTableWidgetItem(pInfo->state()==1?tr("����"):tr("����"));
		tableWidget->setItem(nRow, 6, pItem7);
	} 
	else
	{
		theLog.Write("!!PrintUserMgrTool::AddRow, nRow=%d", nRow);
	}

}

//��ʼ����
void PrintUserMgrTool::on_pushButton_clicked()
{
	if (pushButton->text() == tr("��ʼ����"))
	{
		tableWidget->setCurrentCell(0,0);
		pushButton->setText(tr("ֹͣ����"));
		readCardTimer->start(1000); //��ʼ��ȡ����Ϣ
	}
	else if (pushButton->text() == tr("ֹͣ����"))
	{
		pushButton->setText(tr("��ʼ����"));
		readCardTimer->stop(); //ֹͣ��ȡ����Ϣ
		QMessageBox::warning(this, tr("��ʾ"), tr("�������."));
	}
	EnableUI();
}

//ȡ������
void PrintUserMgrTool::on_pushButton_2_clicked()
{
	CLockCS ow(m_cs);
	int nRow = tableWidget->currentRow();
	if (nRow>=0)
	{
		int nAccountId = tableWidget->item(nRow,0)->data(Qt::UserRole).toInt();
		if (CUserAndDepartmentMgrSvc::GetInstance().BindUserIdAndCardId(nAccountId, ""))//���ð󶨿���Ϊ��
		{
			tableWidget->removeRow(nRow);
		}
		else
		{
			theLog.Write("!!PrintUserMgrTool::on_pushButton_2_clicked, BindUserIdAndCardId,fail,nAccountId=%d", nAccountId);
		}
	}
}

//ˢ��
void PrintUserMgrTool::on_pushButton_3_clicked()
{
	on_currentIndexChanged(comboBox->currentIndex());
}

//���ٲ���
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
	//�����û���¼��ӵ���ǰ��ѡ������б���
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
	sTemp = "����:" + tableWidget->item(nIndex,0)->text() + "\t";
	sInfo += sTemp;
	sTemp = "�˺�:" + tableWidget->item(nIndex,1)->text() + "\t";
	sInfo += sTemp;
	sTemp = "����:" + tableWidget->item(nIndex,2)->text() + "\t\t\n";
	sInfo += sTemp;
	sTemp = "Email:" + tableWidget->item(nIndex,3)->text() + "\t";
	sInfo += sTemp;
	sTemp = "�绰:" + tableWidget->item(nIndex,4)->text() + "\t";
	sInfo += sTemp;
	sTemp = "��������:" + tableWidget->item(nIndex,5)->text() + "\t";
	sInfo += sTemp;
	sTemp = "����״̬:" + tableWidget->item(nIndex,6)->text() + "\t";
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

	if (pushButton->text() == tr("ֹͣ����"))
	{
		comboBox->setEnabled(false);
		pushButton_2->setEnabled(false);
		pushButton_3->setEnabled(false);
// 		pushButton_4->setEnabled(false);
	}
	else if (pushButton->text() == tr("��ʼ����"))
	{
		comboBox->setEnabled(true);
		pushButton_2->setEnabled(true);
		pushButton_3->setEnabled(true);
// 		pushButton_4->setEnabled(true);
	}

}

void PrintUserMgrTool::on_readCardTimer_timeout()
{
	//��ȡһ�����ݳɹ�
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