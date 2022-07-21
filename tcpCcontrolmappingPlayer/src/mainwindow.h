
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPaintEvent>
#include "show_vediowidget.h"
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
    char connect_flg =0;
    char run_mode =0;//3种模式 探索、自主建图、手动建图
    char press_flg_expolre =0;
    char press_flg_automapping =0;
    char press_flg_manualmapping =0;
    char press_start =0;
    QPoint  target_position;
    char setGoalFlg =0;

private slots:
	void slotBtnClick();
    void on_pushButtoncnt_clicked();
    void slotGetPositon(QPoint);

    void mDisplay(int a);
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_stop_2_clicked();
    void on_pushButton_le_clicked();
    void on_pushButton_ri_clicked();
    void on_pushButton_explore_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_switch_clicked();
    void on_pushButton_automapping_clicked();
    void on_pushButton_manualmapping_clicked();
    void on_pushButton_loadmapping_clicked();
    void on_pushButton_setgoal_clicked();
    //void on_pushButton_pathplan_clicked();
    void on_pushButton_cleargoal_clicked();
    void on_pushButton_mapnavigation_clicked();
};
enum MODE_Item
{
    Stop_MODE=0,
    Explore_MODE,
    AutoMapping_MODE,//2
    Manual_MODE,
    SetGoal_CMD,//4
    Pathplan_CMD,
    Cleargoalplan_CMD
};
extern char takepic ;
extern char recordVideo ;
extern QString addr;
extern unsigned int port16;
extern   QTcpSocket *client_fd;
extern unsigned int INPUT_IMAGE_WIDTH; //1280
extern unsigned int INPUT_IMAGE_HEIGHT;
#endif // MAINWINDOW_H
