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

protected:
	void paintEvent(QPaintEvent *event);

	QImage mImage; //记录当前的图像

private:
	Ui::show_vediowidget *ui;

public slots:
	///播放器相关的槽函数
	void slotGetOneFrame(QImage);
};

#endif // SHOW_VEDIOWIDGET_H
