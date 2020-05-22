#ifndef OVERHEAD
#define OVERHEAD
#pragma once
//In the aim to use Windows thread message, here we use Qt Signal/Slot
#include<QtCharts/QChart>
#include <QTimer>
QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE
QT_CHARTS_USE_NAMESPACE

class Overhead :public QChart
{
	Q_OBJECT
public:
	Overhead(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
	virtual ~Overhead();

public slots:
	void handleTimeout();
	void keyPressEvent(QKeyEvent* event) override;
private:
	QTimer m_timer;
	QSplineSeries *m_series;
	QSplineSeries *consensus_overhead;
	QStringList m_titles;
	QValueAxis *m_axis;
	qreal m_step;
	qreal m_x;
	qreal m_y;
};



#endif
