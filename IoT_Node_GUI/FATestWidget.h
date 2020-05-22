#pragma once

#include <QWidget>
#include "Global.h"
namespace Ui { class FATestWidget; };

class FATestWidget : public QWidget
{
	Q_OBJECT

public:
	FATestWidget(unsigned int index, QWidget *parent = Q_NULLPTR);
	~FATestWidget();
signals:
	void AddFATest(FATestStruct);

private:
	Ui::FATestWidget *ui;
	unsigned int index;
	void setIndex(unsigned int index);
private slots:
	void AddClicked();
};
