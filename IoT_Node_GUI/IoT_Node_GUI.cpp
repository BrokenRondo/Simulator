#include "IoT_Node_GUI.h"
#include<QPainter>
#include <QVector>
#include <QString>
#include <QTimer>
#include "myline.h"
#include "actThread.h"
#include "IoT_Network_Scene.h"
#include "ChangeRangeDialog.h"
#include "Overhead.h"
#include "FATestWidget.h"
#include "Global.h"
#include "FaultAttackTest.h"
#include <queue>
extern std::vector<MyItem*> GlobalItemVector;
extern std::vector<MyLine*> GlobalLineVector;
extern std::vector<TextItem*> GlobalTextVector;
extern std::vector<QString> GlobalLogVector;
extern std::queue<QTimer*>GlobalTimerVector;
const int ConsensusStartPort = 60000;
const int DataNetworkStartPort = 50000;
extern unsigned int node_index;
extern int fieldSize;
extern int rangeSize;
extern time_t start;
extern time_t end;
extern unsigned int ConsensusGap;

IoT_Node_GUI::IoT_Node_GUI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	IoT_Network_Scene* scene = new IoT_Network_Scene();
	scene->setSceneRect(-250, -250, 500, 500);
	this->Scene = scene;
	MyItem *myitem = new MyItem(node_index++,(unsigned int)12);
	myitem->setPos(0, 0);
	myitem->set_r(10);
	myitem->setType(1);
	scene->addItem(myitem->range);
	myitem->updatePos(0, 0);
	GlobalItemVector.push_back(myitem);
	scene->addItem(myitem);
	scene->add_an_item(myitem);
	scene->addItem(myitem->Text);
	
	ui.graphicsView->setScene(scene);
	ui.graphicsView->resize(1000, 800);
	ui.graphicsView->show();
	ui.horizontalSlider->setRange(0, 88);
	ui.horizontalSlider->setSliderPosition(65);
	ui.ConsensusSpeedSlider->setRange(0, 67);
	ui.ConsensusSpeedSlider->setSliderPosition(10);
	actThread* act_thread = new actThread();
	this->thread = act_thread;
	act_thread->setMainView(ui.graphicsView);
	connect(ui.startSim, SIGNAL(clicked()), this, SLOT(StartClicked()));
	connect(ui.Clear, SIGNAL(clicked()), this, SLOT(ClearClicked()));
	connect(ui.AddItem, SIGNAL(clicked()), this, SLOT(AddItemClicked()));
	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(ChangeSpeed(int)));
	connect(ui.ConsensusSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(ChangeConsensusSpeed(int)));
	connect(ui.StopSim, SIGNAL(clicked()), this, SLOT(StopClicked()));
	connect(ui.HideLog, SIGNAL(clicked()), this, SLOT(HideLog()));
	connect(ui.ShowLog, SIGNAL(clicked()), this, SLOT(ShowLog()));
	connect(ui.setRange, SIGNAL(clicked()), this, SLOT(setRange()));
	connect(ui.setField, SIGNAL(clicked()), this, SLOT(setField()));
	//QTimer *timer = new QTimer();
	//timer->setSingleShot(false);
	//timer->start(ConsensusGap);
	Overhead *chart = new Overhead();
	chart->setTitle("Network Overhead");
	chart->legend()->hide();
	//chart->setAnimationOptions(QChart::AllAnimations);
	ui.widget->setChart(chart);
	ui.widget->setRubberBand(QChartView::HorizontalRubberBand);
	ui.widget->setRenderHint(QPainter::Antialiasing);
	ui.widget->resize(700, 500);

	
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	
}


void IoT_Node_GUI::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	//painter.drawLine(QPoint(0, 0), QPoint(100, 100));
}
void IoT_Node_GUI::setField()
{
	ChangeRangeDialog changeRangeDialog(this);
	changeRangeDialog.exec();
	int newRangeRadius = changeRangeDialog.getInput();
	fieldSize = newRangeRadius;
}
void IoT_Node_GUI::setRange()
{
	ChangeRangeDialog changeRangeDialog(this);
	changeRangeDialog.exec();
	int newRangeRadius = changeRangeDialog.getInput();
	rangeSize = newRangeRadius;
}

void IoT_Node_GUI::AddItemClicked()
{
	this->Scene->draw_an_item();
	if (started)
	{
		GlobalItemVector.back()->sender->start();
		GlobalItemVector.back()->consensus->Run();
		GlobalItemVector.back()->consensus->broadcast_->start();
	}
	this->update();
	ui.graphicsView->update();
}
void IoT_Node_GUI::StartClicked()
{

	//connect(act_thread, SIGNAL(toScene(unsigned int, MyItem*, MyLine*, QPointF, QPointF, QPointF, QPointF, QColor)), this->Scene, SLOT(Scene_act_as_asked(unsigned int, MyItem*, MyLine*, QPointF, QPointF, QPointF, QPointF, QColor)));
	//connect(act_thread, SIGNAL(toSceneUpdateText(unsigned int, QString)), this->Scene, SLOT(SlotUpdateLog(unsigned int, QString)));
	//connect(act_thread, SIGNAL(toSceneDeleteLines()), this->Scene, SLOT(SlotDeleteLineVector()));


	//在开始的时候需要把所有的攻击测试全部加载完成，将所有的单元格全部整理成结构体数组，然后作为参数传给FaultAttackTest
	std::vector<FAItem>FAItemVector = ReadTableWidget();
	FaultAttackTest FTest;
	FTest.addFAItemVector(FAItemVector);
	started = true;
	this->thread->start();
	for (int i=1;i<GlobalItemVector.size();i++)
	{
		GlobalItemVector[i]->sender->start();
		//GlobalItemVector[i]->receiver->start();
		GlobalItemVector[i]->consensus->Run();
		GlobalItemVector[i]->consensus->broadcast_->start();
	}
}
void IoT_Node_GUI::StopClicked()
{
	started = false;
	for (int i = 0; i < GlobalItemVector.size(); i++)
	{
		GlobalItemVector[i]->sender->stop();
		GlobalItemVector[i]->consensus->stop();
		GlobalItemVector[i]->consensus->broadcast_->stop();
	}
	_sleep(1100);
	this->thread->setStop(true);
	this->Scene->SlotDeleteLineVector();
	GlobalLineVector.clear();
	//GlobalItemVector.clear();
	while (!GlobalTimerVector.empty())
	{
		GlobalTimerVector.pop();
	}
	
	//GlobalLogVector.clear();
}

void IoT_Node_GUI::ClearClicked()
{
	StopClicked();
	for (int i= GlobalItemVector.size() - 1;i>0;i--)
	{
		this->Scene->removeItem(GlobalItemVector[i]);
		this->Scene->removeItem(GlobalTextVector[i]);
		this->Scene->removeItem(GlobalItemVector[i]->Text);
		this->Scene->removeItem(GlobalItemVector[i]->range);
		GlobalItemVector[i]->nodelog->close();
		GlobalItemVector[i]->nodeBlockchainDatabase->close();
		GlobalItemVector[i]->sender->close();
		GlobalItemVector[i]->receiver->close();
		delete GlobalItemVector[i]->nodeBlockchainDatabase;
		delete GlobalItemVector[i]->nodelog;
		delete GlobalItemVector[i];
		GlobalItemVector.erase(GlobalItemVector.begin()+i);
	}
	for (int i = 0; i < GlobalLineVector.size(); i++)
	{
		this->Scene->removeItem(GlobalLineVector[i]);
		delete GlobalLineVector[i];
	}
	//for (int i=GlobalTimerVector.size()-1; i>=0;i--)
	//{
	//	delete GlobalTimerVector[i];
	//	GlobalTimerVector.remove(i);
	//}

	GlobalLogVector.clear();
	GlobalLineVector.clear();
	GlobalItemVector.clear();
	node_index = 0;
	this->Scene->clear();
	this->Scene->draw_an_item(QPointF(0, 0));
	qApp->closeAllWindows();

	QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}
void IoT_Node_GUI::ChangeSpeed(int speed)
{
	for (int i=0;i<GlobalItemVector.size();i++)
	{
		GlobalItemVector[i]->sender->setFrequency(100 - speed);
		GlobalItemVector[i]->sender->setSpeed(1000 - 10 * speed);
	}
}

void IoT_Node_GUI::ChangeConsensusSpeed(int speed)
{
	ConsensusGap = 8000 + speed*speed*speed;
}

void IoT_Node_GUI::HideLog()
{
	for (int i=0;i<GlobalTextVector.size();i++)
	{
		GlobalTextVector[i]->setVisible(false);
	}
}
void IoT_Node_GUI::ShowLog()
{
	for (int i = 0; i < GlobalTextVector.size(); i++)
	{
		GlobalTextVector[i]->setVisible(true);
	}
}
void IoT_Node_GUI::slotTimerUpdate()
{
	start = end;
	end = end + ConsensusGap;
}

void IoT_Node_GUI::insertTest(FATestStruct FATest)
{

	int rowcount = ui.tableWidget->rowCount();
	QString typestr = "";
	ui.tableWidget->insertRow(rowcount);
	ui.tableWidget->setItem(rowcount, 0, new QTableWidgetItem(QString::fromStdString( std::to_string(FATest.index))));
	ui.tableWidget->setItem(rowcount, 2, new QTableWidgetItem(QString::fromStdString(std::to_string(FATest.start))));
	ui.tableWidget->setItem(rowcount, 3, new QTableWidgetItem(QString::fromStdString(std::to_string(FATest.end))));
	ui.tableWidget->setItem(rowcount, 4, new QTableWidgetItem(QString::fromLocal8Bit("×")));
	switch (FATest.type)
	{
	case 0:
		typestr = "STOP/OFFLINE";break;
	case 1:
		typestr = "Broadcast wrong data if being main node";break;
	case 2:
		typestr = "Broadcast random data if being main node";break;
	default:
		typestr = "Default";
	}
	ui.tableWidget->setItem(rowcount, 1, new QTableWidgetItem(typestr));
}
std::vector<FAItem> IoT_Node_GUI::ReadTableWidget()
{
	std::vector<FAItem>FAItems;
	int tableCount = ui.tableWidget->rowCount();
	for (int i=0;i<tableCount;i++)
	{
		FAItem item;
		item.index = static_cast<unsigned>(ui.tableWidget->item(i, 0)->text().toInt());
		QString action = ui.tableWidget->item(i, 1)->text();
		if (action=="STOP/OFFLINE")
		{
			item.action = 0;
		}
		else if (action=="Broadcast wrong data if being main node")
		{
			item.action = 1;
		}
		else if (action=="Broadcast random data if being main node")
		{
			item.action = 2;
		}
		item.startTime = static_cast<unsigned>(ui.tableWidget->item(i, 2)->text().toInt());
		item.endTime =static_cast<unsigned>( ui.tableWidget->item(i, 3)->text().toInt());
		FAItems.push_back(item);
	}
	return  FAItems;
}
