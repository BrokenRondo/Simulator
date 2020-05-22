#include <QTimer>
#include "SimNodeRecvThread.h"
#include "jsoncpp/json.h"
#include <queue>
#include <mutex>
using namespace std;

extern std::vector<MyItem*> GlobalItemVector;
extern std::vector<MyLine*> GlobalLineVector;
extern std::vector<TextItem*> GlobalTextVector;
extern std::vector<QString> GlobalLogVector;
extern std::queue<QTimer*>GlobalTimerVector;
const int ConsensusStartPort = 60000;
const int DataNetworkStartPort = 50000;
extern unsigned int node_index;
extern unsigned int dataMSGNumber;
extern unsigned int consensusMSGNumber;

SimNodeRecvThread::SimNodeRecvThread(unsigned index, uint16_t port,NodeBlockchainDatabase* DB)
{
	this->index = index;
	this->port = port;
	receiver = new QUdpSocket(this);
	receiver->bind( (quint16)port,QUdpSocket::ShareAddress);
	connect(receiver, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
	localDatabase = DB;
	//localLog = new NodeLog(index);
	//QTimer *timer=new QTimer(this);
	//connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerUpdate()));
	//timer->setSingleShot(false);
	//timer->start(100);
	//GlobalTimerVector.enqueue(timer);
}
unsigned SimNodeRecvThread::getIndex()
{
	return this->index;
}
uint16_t SimNodeRecvThread::getPort()
{
	return this->port;
}
void SimNodeRecvThread::setIndex(unsigned index)
{
	this->index = index;
}

void SimNodeRecvThread::setPort(uint16_t port)
{
	this->port = port;
}

void SimNodeRecvThread::readPendingDatagrams()
{
	while (receiver->hasPendingDatagrams())
	{
		dataMSGNumber++;
		char buf[1024] = { 0 };
		QHostAddress addr;
		quint16 port;
		qint64 len;
		QString recv_addr;
		QString recv_port;
		QString recv_time;
		QString msgString = "";
		len = receiver->readDatagram(buf, sizeof(buf), &addr, &port);
		CheckandTerminateAct();
		if (Sys_Down)
		{
			continue;
		}
		
		if (len > 0)
		{
			recv_addr = addr.toString();
			recv_port = QString::number(port);
		}
		else
		{
			msgString = "Received Wrong Data, Error code:0x00000001.\n";
			emit toGUIAddLog(msgString);
			GlobalLogVector[index].append(msgString);
			return;
		}
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(buf, root, false))
		{

			msgString = tr("Received Wrong Data From %1:%2, Cannot Parse!\n").arg(addr.toString()).arg(port);
			emit toGUIAddLog(msgString);
			GlobalLogVector[index].append(msgString);
			return;
		}
		if (root["UPLOADER"].asInt() != port - DataNetworkStartPort || root["SIGNATURE"].asInt() != port)
		{
			msgString = tr("Received Bad Data! Signature not valid, from %1.").arg(port);
			GlobalLogVector[index].append( msgString);
			return;
		}
		recv_time = QString::fromStdString(root["TIME"].asString());
		msgString = tr("Received Data { %1 } From %2: %3\n").arg(QString::fromStdString( root["CONTEXT"].asString())).arg(port).arg(recv_time);
		//emit toGUIAddLog(msgString);
		//AddLog(msgString);
		if (GlobalLogVector.size()>0) 
		{
			if (GlobalLogVector[index].length() > 500)
			{
				GlobalLogVector[index].clear();//To avoid string too long
			}
		}
		src = root["UPLOADER"].asInt();
		
		GlobalLogVector[index].append(msgString);
		emit toSceneUpdateText(index, GlobalLogVector[index]);
		
		QPointF a = GlobalItemVector[src]->getCentre();
		emit toScene(1, nullptr, nullptr, a, GlobalItemVector[index]->getCentre(), a, a, Qt::blue,index);
		QTimer *timer = new QTimer();
		timer->setSingleShot(true);
		timer->start(200);
		GlobalTimerVector.push(timer);
		connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerUpdate()));
		//
		//QTimer::singleShot(100, this, SLOT(slotTimerUpdate()));
		emit toDBAddMsg(recv_time, recv_addr, recv_port, QString::fromStdString(root["CONTEXT"].asString()));
		AddMsg(recv_time, recv_addr, recv_port, QString::fromStdString(root["CONTEXT"].asString()));
		CheckandSetAction();
		
	}
}
void SimNodeRecvThread::slotTimerUpdate()
{
	emit toSceneDeleteLine(this->index);
}

void SimNodeRecvThread::run()
{

}
void SimNodeRecvThread::close()
{
	this->receiver->close();
}
void SimNodeRecvThread::AddMsg(QString timestamp, QString uploader, QString signature, QString context)
{
	this->localDatabase->UpdateMsgDB(timestamp,uploader,signature,context);
}
void SimNodeRecvThread::AddLocalData(QString timestamp, QString context)
{
	this->localDatabase->UpdateLocalDB(timestamp, context);
}
void SimNodeRecvThread::AddLog(QString str)
{
	this->localDatabase->UpdateLog(str);
}
void SimNodeRecvThread::showDB()
{
	//localDatabase->setWindowFlags(Qt::WindowStaysOnTopHint);
	this->localDatabase->show();
}
void SimNodeRecvThread::UpdateLocalDB(QString timestamp, QString context)
{
	this->localDatabase->UpdateLocalDB(timestamp, context);
}
void SimNodeRecvThread::toSceneDrawArrow(QPointF src, QPointF dst, QColor color, unsigned index)
{
	srand(time(nullptr) + index);
	QTimer *timer = new QTimer();
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerUpdate()));
	if (color==Qt::red)
	{
		timer->start(200);
	}
	else
	{
		timer->start( 200);
	}

	GlobalTimerVector.push(timer);

	//QTimer::singleShot(100, this, SLOT(slotTimerUpdate()));
	emit toScene(1, nullptr, nullptr, src, dst, src, src, color, index);
}


//先把所有的测试放入自己的vector里，然后自行结束。
//所有测试事件必须前后连接，不能同时发生
void SimNodeRecvThread::addFAAction(int type, unsigned startTime, unsigned endTime)
{
	FATimeItem *startitem = new FATimeItem;
	FATimeItem *enditem = new FATimeItem;
	startitem->type = type;
	enditem->type = type;
	startitem->time = startTime;
	FAStartVector.push_back(startitem);
	sort(FAStartVector.begin(), FAStartVector.end(), less_sort);
	enditem->time = endTime;
	FAEndVector.push_back(enditem);
	sort(FAEndVector.begin(), FAEndVector.end(), less_sort);
}
void SimNodeRecvThread::CheckandSetAction()
{
	if (!FAStartVector.empty())
	{
		//如果有需要开始的Action
		if (FAStartVector.front()->time<=viewNumber)
		{
			switch (FAStartVector.front()->type)
			{
			case 0:
				Sys_Down = true; break;
			case 1:
				Wrong_Fault = true; break;
			case 2:
				Random_Fault = true; break;
			}
			FAStartVector.pop_front();
		}
	
	}
}
void SimNodeRecvThread::CheckandTerminateAct()
{
	if (!this->FAEndVector.empty())
	{

		if (this->FAEndVector.front()->time <= viewNumber)
		{
			switch (FAEndVector.front()->type)
			{
			case 0:
				Sys_Down = false; break;
			case 1:
				Wrong_Fault = false; break;
			case 2:
				Random_Fault = false; break;
			}
			FAEndVector.pop_front();
		}
	}
}



