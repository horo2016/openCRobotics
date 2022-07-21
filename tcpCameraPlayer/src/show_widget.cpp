#include "show_widget.h"
#include "ui_show_widget.h"

show_widget::show_widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::show_widget)
{
	ui->setupUi(this);
}

show_widget::~show_widget()
{
	delete ui;
}
