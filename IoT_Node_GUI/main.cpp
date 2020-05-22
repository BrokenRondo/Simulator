
#include "IoT_Node_GUI.h"
#include <QtWidgets/QApplication>
#include<QgraphicsRectItem>
#include<QGraphicsScene>
#include<QGraphicsview>
#include <QThread>
#include <QTimer>
#include "IoT_Node_GUI.h"
#include"actThread.h"
#include "myitem.h"
#include "myline.h"
#include "IoT_Network_Scene.h"
#include "IoT_Node_GUI.h"
#include "InteractiveView.h"
#include "Overhead.h"
#include <QtCharts/QChartView>
#include "util.h"
#include <queue>

std::vector<MyItem*> GlobalItemVector;
std::vector<MyLine*> GlobalLineVector;
std::vector<TextItem*> GlobalTextVector;
std::vector<QString> GlobalLogVector;
std::queue<QTimer*>GlobalTimerVector;
const int ConsensusStartPort = 60000;
const int DataNetworkStartPort = 50000;
time_t start;
time_t end;
unsigned int ConsensusGap;
unsigned int node_index;
int fieldSize;
int rangeSize;
IoT_Node_GUI* GUI;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	Util util;
	node_index = 0;
	fieldSize = 800;
	rangeSize = 3000;
	ConsensusGap = 12 * 1000;
	start = 0;
	end = 0;
	end = util.getTimeStamp()-2000;
	QMainWindow window;
	//Overhead *chart=new Overhead();
	//chart->setTitle("Network Overhead");
	//chart->legend()->hide();
	//chart->setAnimationOptions(QChart::AllAnimations);
	//QChartView chartView(chart);
	//chartView.setRenderHint(QPainter::Antialiasing);
	//window.setCentralWidget(&chartView);
	//window.resize(500, 300);
	//window.show();

	IoT_Node_GUI w;
	GUI = &w;
	w.showMaximized();
	return a.exec();
}
