#include "Overhead.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QMutex>
unsigned int dataMSGNumber=0;
unsigned int consensusMSGNumber=0;
Overhead::Overhead(QGraphicsItem* parent, Qt::WindowFlags wFlags) :
	QChart(QChart::ChartTypeCartesian, parent, wFlags), m_series(0),m_axis(new QValueAxis),m_step(0),m_x(60),m_y(1)
{
	QObject::connect(&m_timer, &QTimer::timeout, this, &Overhead::handleTimeout);
	m_timer.setInterval(1000);
	m_series = new QSplineSeries(this);
	consensus_overhead = new QSplineSeries(this);
	QPen pen1(Qt::red);
	pen1.setWidth(3);
	m_series->setPen(pen1);
	m_series->append(m_x, m_y);
	m_series->setUseOpenGL(true);
	QPen pen2(Qt::blue);
	pen2.setWidth(3);
	consensus_overhead->setPen(pen2);
	consensus_overhead->append(m_x, m_y);
	consensus_overhead->setUseOpenGL(true);

	addSeries(m_series);
	addSeries(consensus_overhead);
	createDefaultAxes();
	setAxisX(m_axis, m_series);
	setAxisX(m_axis, consensus_overhead);
	m_axis->setTickCount(60);
	axisX()->setRange(0, 60);
	axisY()->setRange(-10, 2000);
	m_timer.start();
}
Overhead::~Overhead()
{
	
}
void Overhead::handleTimeout()
{
	qreal x = plotArea().width() / m_axis->tickCount();
	qreal y = (m_axis->max() - m_axis->min()) / m_axis->tickCount();
	m_x += y;
	m_y = consensusMSGNumber;
	m_series->append(m_x, m_y);
	m_y += dataMSGNumber;
	consensus_overhead->append(m_x, m_y);
	scroll(x, 0);
	QMutex mutex;
	mutex.lock();
	dataMSGNumber = 0;
	consensusMSGNumber = 0;
	mutex.unlock();
}
void Overhead::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Plus:
		this->zoomIn();
		break;
	case Qt::Key_Minus:
		this->zoomOut();
		break;
	}
}


