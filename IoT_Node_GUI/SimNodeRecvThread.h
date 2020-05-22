#pragma once
#ifndef SIM_NODE_RECV_THREAD
#define SIM_NODE_RECV_THREAD
#include <QThread>
#include <QString>
#include <QtNetwork/QtNetwork>
#include "myitem.h"
#include "myline.h"
#include "NodeBlockchainDatabase.h"
#include "NodeLog.h"
#include <deque>
#include "Global.h"

class MyItem;
class MyLine;
struct FATimeItem;

//This Thread aims to simulate the receive action of a node, the send and receive communication
//is simulated by socket, the communication arrow showed in QGraphicsView will only be drawn when
//receives a message
class SimNodeRecvThread:public QThread
{
	Q_OBJECT
public:
	SimNodeRecvThread(unsigned int index, uint16_t port,NodeBlockchainDatabase* DB);
	void stop();
	void setPort(uint16_t port);
	void setIndex(unsigned int index);
	uint16_t getPort();
	unsigned int getIndex();
	void close();
	void AddMsg(QString, QString, QString, QString);
	void AddLocalData(QString, QString);
	void AddLog(QString str);
	void toSceneDrawArrow(QPointF src, QPointF dst, QColor color, unsigned int index);
	NodeBlockchainDatabase* localDatabase;
	void setSysDown(bool Sys_Down, unsigned int startTime, unsigned int endTime);
	void addFAAction(int type,unsigned int startTime, unsigned int endTime);
	void setWrong(bool WongFault, unsigned int startTime, unsigned int endTime);
	void setRandom(bool RandomFault, unsigned int startTime, unsigned int endTime);
	
signals:
	void toUpdateNodeLog(unsigned int, QString );
	void toScene(unsigned int, MyItem*, MyLine*, QPointF, QPointF, QPointF, QPointF, QColor,unsigned int index);
	void toSceneUpdateText(unsigned int, QString);
	void toSceneDeleteLines();
	void toSceneDeleteLine(unsigned int);
	void toGUIAddLog(QString);
	void toDBAddMsg(QString, QString, QString, QString);



protected:
	void run() override;
private:
	uint16_t port = 0;  //socket port of this thread
	unsigned int src = 0;// who sent the message
	unsigned int index = 0;//to match the same index of the node
	bool stopFlag = false;
	QUdpSocket *receiver;
	unsigned int frequency=100;
	bool Sys_Down = false;//The symbol of fault/attack test
	unsigned int SysDownStartTime = 0;
	unsigned int SysDownEndTime = 1;
	bool Wrong_Fault = false;
	unsigned int WrongFaultStartTime = 0;
	unsigned int NextActionEndTime = 1;
	bool Random_Fault = false;
	unsigned int RandomFaultStartTime = 0;
	std::deque<FATimeItem*> FAStartVector;
	std::deque<FATimeItem*> FAEndVector;

	void CheckandSetAction();
	void CheckandTerminateAct();
private slots:
	void readPendingDatagrams();
	void slotTimerUpdate();
	void showDB();
	void UpdateLocalDB(QString, QString);
	
};

#endif

