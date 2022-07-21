#include "show_vediowidget.h"
#include "ui_show_vediowidget.h"

#include <QPainter>

show_vediowidget::show_vediowidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::show_vediowidget)
{
	ui->setupUi(this);
}

show_vediowidget::~show_vediowidget()
{
	delete ui;
}

void show_vediowidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
    painter.setBrush(Qt::black);
	painter.drawRect(0,0,this->width(),this->height()); //先画成黑色


	if (mImage.size().width() <= 0) return;

	///将图像按比例缩放成和窗口一样大小
	QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);

	int x = this->width() - img.width();
	int y = this->height() - img.height();

	x /= 2;
	y /= 2;

	painter.drawImage(QPoint(x,y),img); //画出图像

}

void show_vediowidget::slotGetOneFrame(QImage Img)
{
	mImage = Img;
	update(); //调用update将执行 paintEvent函数
}
