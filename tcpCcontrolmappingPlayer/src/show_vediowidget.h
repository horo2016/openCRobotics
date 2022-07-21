#ifndef SHOW_VEDIOWIDGET_H
#define SHOW_VEDIOWIDGET_H

#include <QWidget>

namespace Ui {
class show_vediowidget;
}

class show_vediowidget : public QWidget
{
	Q_OBJECT

public:
	explicit show_vediowidget(QWidget *parent = nullptr);
	~show_vediowidget();


    QPoint goal_point;
    char goalIsValid =0;
protected:
	void paintEvent(QPaintEvent *event);

	QImage mImage; //记录当前的图像
    QPoint robot_position;
protected:	//mouse
    void mouseMoveEvent(QMouseEvent *event);         //移动
    void mousePressEvent(QMouseEvent *event);        //单击
   // void mouseReleaseEvent(QMouseEvent *event);      //释放
    void mouseDoubleClickEvent(QMouseEvent *event);  //双击
    void wheelEvent(QWheelEvent *event);             //滑轮
signals:
     void sig_position(QPoint p);
private:
	Ui::show_vediowidget *ui;

public slots:
	///播放器相关的槽函数
	void slotGetOneFrame(QImage);

};

#endif // SHOW_VEDIOWIDGET_H
