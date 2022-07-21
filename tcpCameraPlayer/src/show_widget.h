#ifndef SHOW_WIDGET_H
#define SHOW_WIDGET_H

#include <QWidget>

namespace Ui {
class show_widget;
}

class show_widget : public QWidget
{
	Q_OBJECT

public:
	explicit show_widget(QWidget *parent = nullptr);
	~show_widget();

private:
	Ui::show_widget *ui;
};

#endif // SHOW_WIDGET_H
