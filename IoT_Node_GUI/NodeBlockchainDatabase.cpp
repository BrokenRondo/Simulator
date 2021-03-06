#include "NodeBlockchainDatabase.h"
#include "ui_NodeBlockchainDatabase.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include "jsoncpp/writer.h"
#include "util.h"
#include "openssl/md5.h"
#include <QCryptographicHash>
#include <QMutex>
#include <mutex>
#include "MerkleTree/merkletree.h"
std::mutex mtx;
NodeBlockchainDatabase::NodeBlockchainDatabase(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::NodeBlockchainDatabase();
	ui->setupUi(this);
	ui->tableWidget->setColumnCount(3);
	ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Previous Hash" << "BlockData" << "TimeStamp");
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//Can't Modify
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//
	ui->plainTextEdit->setMaximumBlockCount(2);
}
NodeBlockchainDatabase::NodeBlockchainDatabase(int index)
{
	ui = new Ui::NodeBlockchainDatabase();
	ui->setupUi(this);
	ui->tableWidget_2->show();
	ui->tableWidget->setColumnCount(3);
	ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Previous Hash" << "BlockData" << "TimeStamp");
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//Can't Modify
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//
	ui->label->setText(tr("Node Virtual Database: No.%1").arg(index));
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_2->setColumnCount(4);
	ui->tableWidget_2->setRowCount(1);
	ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "TimeStamp" << "Uploader" << "Signature"<<"Context");
	ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//Can't Modify
	ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);//
	ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_2->setRowCount(0);
	ui->tableWidget_3->setColumnCount(2);
	//ui->tableWidget_3->setRowCount(1);
	ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "TimeStamp" <<"Context");
	ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);//Can't Modify
	ui->tableWidget_3->setSelectionMode(QAbstractItemView::SingleSelection);//
	ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->plainTextEdit->setMaximumBlockCount(1000);
}

NodeBlockchainDatabase::~NodeBlockchainDatabase()
{
	delete ui;
}

void NodeBlockchainDatabase::UpdateMsgDB(QString timestamp, QString uploader, QString signature, QString context)
{
	mutex mtx;
	mtx.lock();

	//int i = ui->tableWidget_2->rowCount();
	int i = 0;
	ui->tableWidget_2->insertRow(i);
	ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(timestamp));
	ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(uploader));
	ui->tableWidget_2->setItem(i, 2, new QTableWidgetItem(signature));
	ui->tableWidget_2->setItem(i, 3, new QTableWidgetItem(context));
	


	mtx.unlock();
	//ui->tableWidget_2->scrollToBottom();
}
void NodeBlockchainDatabase::UpdateLocalDB(QString timestamp, QString context)
{
	mutex mtx;
	mtx.lock();
	ui->tableWidget_3->insertRow(ui->tableWidget_3->rowCount());
	ui->tableWidget_3->setItem(ui->tableWidget_3->rowCount()-1 , 0, new QTableWidgetItem(timestamp));
	ui->tableWidget_3->setItem(ui->tableWidget_3->rowCount()-1 , 1, new QTableWidgetItem(context));
	mtx.unlock();
}
void NodeBlockchainDatabase::UpdateLog(QString str)
{
	
	ui->plainTextEdit->appendPlainText(str);
	
}
std::string NodeBlockchainDatabase::GetData( time_t start, time_t end)
{

	Json::Value root;
	Json::FastWriter swriter;
	mutex mtx;
	//mtx.lock();
	int RowCount = ui->tableWidget_2->rowCount();
	for (int i = 0; i < RowCount; i++)
	{
		QString timestamp;
		try {
			QTableWidgetItem* p = ui->tableWidget_2->item(i, 0);
			if (p==nullptr)
			{
				continue;
			}
			timestamp = ui->tableWidget_2->item(i, 0)->text();//取当行时间戳
		}catch (...)
		{
			
		}
		
		Util util;
		time_t time_= util.utilTimeStr2LocalTime((char*)timestamp.toStdString().c_str());
		time_ *= 1000;//time_ accurate to seconds, start and end accurate to milliseconds
		if (time_ < end && time_ >= start)//判断当前时间戳是否属于筛选范围
		{
			//取当行数据并Json化

			std::string Timestamp = ui->tableWidget_2->item(i, 0)->text().toLocal8Bit();
			std::string Uploader = ui->tableWidget_2->item(i, 1)->text().toLocal8Bit();
			std::string Signature = ui->tableWidget_2->item(i, 2)->text().toLocal8Bit();
			std::string Context = ui->tableWidget_2->item(i, 3)->text().toLocal8Bit();
			root.clear();
			
			root["TIMESTAMP"] = Timestamp;
			root["UPLOADER"] = Uploader;
			root["SIGNATURE"] = Signature;
			root["CONTEXT"] = Context;
			std::string Data = swriter.write(root);
			char data [200]={0};
			memcpy(data, Data.c_str(), Data.length());
			result.push_back(data);
			index.push_back(i);
		}
	}
	//mtx.unlock();
	sort(index.begin(), index.end(),std::greater<int>());
	//reverse(index.begin(), index.end());
	if (result.size()<=0)
	{
		return "0000000000000000";
	}
	if (result.size()==1)
	{
		result.push_back(result[0]);//If only one node, the tree need to copy this node to make 2 nodes.
	}
	merkletree *mtree = new merkletree(result);
	std::string rootstr = mtree->root();
	qDebug("rootstr");
	qDebug(rootstr.c_str());
	delete mtree;
	mtree = nullptr;

	return rootstr;

	
}
void NodeBlockchainDatabase::clearDataifCorrect()
{
	mutex mtx;
	mtx.lock();
	for (unsigned int i = 0; i < index.size(); i++)
	{
		QTableWidgetItem* p1 = ui->tableWidget_2->item(index[i], 0);
		QTableWidgetItem* p2 = ui->tableWidget_2->item(index[i], 1);
		QTableWidgetItem* p3 = ui->tableWidget_2->item(index[i], 2);
		QTableWidgetItem* p4 = ui->tableWidget_2->item(index[i], 3);

		//ui->tableWidget_2->takeItem(index[i], 0);
		//ui->tableWidget_2->takeItem(index[i], 1);
		//ui->tableWidget_2->takeItem(index[i], 2);
		//ui->tableWidget_2->takeItem(index[i], 3);
		delete p1; p1 = nullptr;
		delete p2; p2 = nullptr;
		delete p3; p3 = nullptr;
		delete p4; p4 = nullptr;
		ui->tableWidget_2->removeRow(index[i]);

	}
	mtx.unlock();
	result.clear();
	index.clear();
}
void NodeBlockchainDatabase::addBlock(QString data)
{
	ui->tableWidget->insertRow(ui->tableWidget->rowCount());
	std::string previousBlock = "";
	if (ui->tableWidget->rowCount()>=2)
	{
		 previousBlock = ui->tableWidget->item(ui->tableWidget->rowCount() - 2, 1)->text().toStdString();
	}
	else
	{
		previousBlock = "0000000000000000";
	}
	

	QByteArray hashbuf;
	hashbuf = QCryptographicHash::hash(previousBlock.c_str(), QCryptographicHash::Md5);
	QString hashValue = hashbuf.toHex();
	time_t time_ = time(0);
	mutex mtx;
	mtx.lock();
	ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(hashValue));
	ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(data));
	ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, new QTableWidgetItem(QString::number(time_)));
	mtx.unlock();
}



