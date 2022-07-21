
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPaintEvent>

#include "videoplayer/videoplayer.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();


private:
	Ui::MainWindow *ui;

    VideoPlayer *mPlayer;
    char playpress =0;
     QTcpSocket *client_fd;
private slots:
	void slotBtnClick();
    void on_pushButtoncnt_clicked();
    void on_pushButton_play_clicked();
    void on_pushButton_open_clicked();

    void mDisplay(int a);
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_stop_2_clicked();
    void on_pushButton_le_clicked();
    void on_pushButton_ri_clicked();
    void on_pushButton_6_clicked();
};
extern char takepic ;
extern char recordVideo ;
extern QString addr;
extern unsigned int port16;
extern unsigned int INPUT_IMAGE_WIDTH; //1280
extern unsigned int INPUT_IMAGE_HEIGHT;
#endif // MAINWINDOW_H
