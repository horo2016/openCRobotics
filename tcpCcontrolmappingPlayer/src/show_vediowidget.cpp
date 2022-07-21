#include "show_vediowidget.h"
#include "ui_show_vediowidget.h"
#include <QMouseEvent>
#include <QPainter>

show_vediowidget::show_vediowidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::show_vediowidget)
{
	ui->setupUi(this);
    setMouseTracking(true);


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

    robot_position.setX(this->width()/2-7);
    robot_position.setY(this->height()/2-7);
    QMatrix matrix;     matrix.rotate(0.0);


	if (mImage.size().width() <= 0) return;

	///将图像按比例缩放成和窗口一样大小
	QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);
     QImage robot_img(":/image/robot.png");
     robot_img =robot_img.scaled(QSize(15,15));
     robot_img = robot_img.transformed(matrix,Qt::FastTransformation);
    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

	painter.drawImage(QPoint(x,y),img); //画出图像
     painter.drawImage(robot_position,robot_img); //画出图像
     if(this->goalIsValid ==1){
         QImage loc_img(":/image/loc_goal.png");
         loc_img =loc_img.scaled(QSize(10,15));

         painter.drawImage(goal_point,loc_img); //画出图像
     }
}

void show_vediowidget::slotGetOneFrame(QImage Img)
{
	mImage = Img;
    update(); //调用update执行paintEvent函数
}
void show_vediowidget::mouseMoveEvent(QMouseEvent *event)
{


 //qDebug(("坐标：（%d,%d）"),(event->pos().x()),(event->pos().y()));
 //lable->setText(QString("坐标：（%1,%2）").arg(event->pos().x()).arg(event->pos().y()));


}
void show_vediowidget::mousePressEvent(QMouseEvent *event)
{//单击
    // 如果是鼠标左键按下
    if (event->button() == Qt::LeftButton){
        qDebug( "left click") ;
        goal_point.setX(event->pos().x());
        goal_point.setY(event->pos().y());
        this->goalIsValid = 1;

        emit sig_position(goal_point);
        //setMouseState( MouseState::L_C, 0);
    }
    // 如果是鼠标右键按下
    else if (event->button() == Qt::RightButton){
        this->goalIsValid = 0;//清除显示
        //qDebug() << "right click";
        //setMouseState(MouseState::R_C, 0);
    }
    else if (event->button() == Qt::MidButton){
        //qDebug() << "mid click";
       // setMouseState(MouseState::M_C, 0);
    }

}
void show_vediowidget::mouseDoubleClickEvent(QMouseEvent *event)
{//双击
    // 如果是鼠标左键按下
    if (event->button() == Qt::LeftButton){
        //qDebug() << "left double click";

    }
    else if (event->button() == Qt::RightButton){
        //qDebug() << "right double click";

    }
}

void show_vediowidget::wheelEvent(QWheelEvent *event)
{//滚轮
    int wheel_val = event->delta();
   // setMouseState(MouseState::Wheel, wheel_val);
    // 当滚轮远离使用者时
    //if (wheel_val > 0){
    //	qDebug() << "wheel forward: " << wheel_val;
    //}
    //else{//当滚轮向使用者方向旋转时
    //	qDebug() << "wheel backward: " << wheel_val;
    //}
}
