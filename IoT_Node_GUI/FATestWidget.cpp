#include "FATestWidget.h"
#include "ui_FATestWidget.h"
#include <QMessageBox>
FATestWidget::FATestWidget(unsigned int index, QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::FATestWidget();
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(AddClicked()));
	this->index = index;
}

FATestWidget::~FATestWidget()
{
	delete ui;
}

void FATestWidget::AddClicked()
{
	unsigned int start=-1 ;
	unsigned int end=-1;
	unsigned type=-1;
	try
	{
		start = ui->lineEdit_2->text().toInt();
		end = ui->lineEdit->text().toInt();
		type = ui->comboBox->currentIndex();
	}
	catch (...)
	{
		QMessageBox::information(this, "Alarm", "Please Input Right Time Format!");
	}
	FATestStruct FAtestStruct;
	FAtestStruct.start = start;
	FAtestStruct.end = end;
	FAtestStruct.index = this->index;
	FAtestStruct.type = type;
	FAtestStruct.transRange = 1000;//this parameter is not used yet
	emit AddFATest(FAtestStruct);
	
}
void FATestWidget::setIndex(unsigned index)
{
	this->index = index;
}

