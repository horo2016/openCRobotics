

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <unistd.h>

#include <QtNetwork>
#include <QTcpSocket>
#include <QHostAddress>

//#define STOP_MODE 0

char filePath =0;//if  1  则打开本地文件 if 2 open internet stream
char takepic =0;
char recordVideo =0;
QString addr;
unsigned int port16;
QTcpSocket *client_fd;
unsigned int INPUT_IMAGE_WIDTH=300; //1280
unsigned int INPUT_IMAGE_HEIGHT=300;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);



    mPlayer = new VideoPlayer;

    ui->verticalLayout_show_video->addWidget(mPlayer->getVideoWidget()); //将显示视频的控件加入
    mPlayer->getVideoWidget()->show();

    usleep(50);
    //widget 的 信号 emit触发,本mainwidonw 接收
    QObject::connect(mPlayer->getVideoWidget(), SIGNAL(sig_position(QPoint)), this, SLOT(slotGetPositon(QPoint)));

    //发送方信号-->接收方的槽
     connect(mPlayer, SIGNAL(show_fresh(int)), this, SLOT(mDisplay(int)));
}

MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::mDisplay(int a)
{
    if(a == 1){
        qDebug()<<"yijign ddd";
        this->ui->constatus->setText("已经连接");
        qDebug("链接成功\n");
        qDebug() << "启动播放";
        playpress = 1;
        connect_flg =1;
    }else{
                    this->ui->pushButtoncnt->setText("连接");
                    this->ui->lineEditaddr->setDisabled(false);
                    this->ui->lineEditport->setDisabled(false);
                    qDebug("链接超时 \n");
                    connect_flg =0;
        }

}

void MainWindow::slotBtnClick()
{
	if (QObject::sender() == ui->pushButton_open) {
		QString s = QFileDialog::getOpenFileName(
				this, "选择要播放的文件",
				"/home/ppchen/Videos/8-16",//初始目录
				"视频文件 (*.flv *.rmvb *.avi *.MP4 *.wmv);; 所有文件 (*.*);; ");
		if (!s.isEmpty()) {

			mPlayer->startPlay();
			//sleep(1);
		}
		else {
			QMessageBox::warning(nullptr, "Warning", "please input vedio path!", QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else if (QObject::sender() == ui->pushButton_stop) {


	}
        else if (QObject::sender() == ui->pushButton_pause) {

        }
        else if (QObject::sender() == ui->pushButton_play) {

        }


}

void MainWindow::on_pushButtoncnt_clicked()
{
    if(this->ui->pushButtoncnt->text() == "连接"){
        addr = this->ui->lineEditaddr->text();
        QString port = this->ui->lineEditport->text();
        QString rate = this->ui->comboBoxrate->currentText();
        if(rate == "300x300"){
            INPUT_IMAGE_WIDTH=300;
            INPUT_IMAGE_HEIGHT=300;
        }else if(rate == "1280x720")
        {
            INPUT_IMAGE_WIDTH=1280; //1280
            INPUT_IMAGE_HEIGHT=720;

        }
        port16 =port.toUInt();
        qDebug() << "ip addr port"<< addr << port <<rate;
        filePath =2;//代表文件来自网络
        this->ui->lineEditaddr->setDisabled(true);
         this->ui->lineEditport->setDisabled(true);
        this->ui->pushButtoncnt->setText("断开");
        mPlayer->setSocket(client_fd,addr,port16);
         mPlayer->startPlay();

    }else if(this->ui->pushButtoncnt->text() == "断开"){
        QString addr = this->ui->lineEditaddr->text();
        QString port = this->ui->lineEditport->text();

        filePath =0;//代表文件来自网络
        this->ui->lineEditaddr->setDisabled(false);
         this->ui->lineEditport->setDisabled(false);
        this->ui->pushButtoncnt->setText("连接");
      //  client_fd->close();

    }

}




void MainWindow::on_pushButton_4_clicked()
{
    takepic =1;
}

void MainWindow::on_pushButton_5_clicked()
{
     if(this->ui->pushButton_5->text() == "开始录制"){
        recordVideo =1;
        this->ui->pushButton_5->setText("停止录制");
     }else  if(this->ui->pushButton_5->text() == "停止录制"){
         recordVideo =0;
          this->ui->pushButton_5->setText("开始录制");
      }

}

void MainWindow::slotGetPositon(QPoint P)
{
    qDebug(("receive1 坐标：（%d,%d）"),(P.x()),(P.y()));

    target_position.setX(P.x());
    target_position.setY(P.y());
    setGoalFlg =1;
}
typedef union{
    unsigned char cvalue[4];
    float fvalue;
}float_union;
//前进指令
void MainWindow::on_pushButton_run_clicked()
{
    if(connect_flg==0)
    {
            QMessageBox::information(NULL, "提醒", "请先连接设备！");
            return;
            }
    float_union  vel_float ,vely_float,vela_float;
    QString vel = this->ui->lineEdit_v->text();//线速度

    vel_float.fvalue = vel.toFloat();
    QString ang = this->ui->lineEdit_a->text();
    vely_float.fvalue =0;vela_float.fvalue =0;

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xff;
    sendcheckcmd[2]=15;
    memcpy(sendcheckcmd+3,vel_float.cvalue,4);
    memcpy(sendcheckcmd+7,vely_float.cvalue,4);
    memcpy(sendcheckcmd+11,vela_float.cvalue,4);
    unsigned char crc = sendcheckcmd[3]^sendcheckcmd[4]^sendcheckcmd[5]^sendcheckcmd[6]^
            sendcheckcmd[7]^sendcheckcmd[8]^sendcheckcmd[9]^sendcheckcmd[10]^
            sendcheckcmd[11]^sendcheckcmd[12]^sendcheckcmd[13]^sendcheckcmd[14];
    sendcheckcmd[15]=crc;
    qDebug()<<sendcheckcmd;


    mPlayer->send_cmd_v_a_2stm32(( char*)sendcheckcmd,16);

}
//左转
void MainWindow::on_pushButton_le_clicked()
{
    if(connect_flg==0)
        return;

    float_union  vel_float ,vely_float,vela_float;
    QString vel = this->ui->lineEdit_v->text();//线速度

    vel_float.fvalue = vel.toFloat();
    QString ang = this->ui->lineEdit_a->text();

    vela_float.fvalue = ang.toFloat();
    vela_float.fvalue =   vela_float.fvalue ;

    vely_float.fvalue =0;
    vel_float.fvalue =0;

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xff;
    sendcheckcmd[2]=15;
    memcpy(sendcheckcmd+3,vel_float.cvalue,4);
    memcpy(sendcheckcmd+7,vely_float.cvalue,4);
    memcpy(sendcheckcmd+11,vela_float.cvalue,4);
    unsigned char crc = sendcheckcmd[3]^sendcheckcmd[4]^sendcheckcmd[5]^sendcheckcmd[6]^
            sendcheckcmd[7]^sendcheckcmd[8]^sendcheckcmd[9]^sendcheckcmd[10]^
            sendcheckcmd[11]^sendcheckcmd[12]^sendcheckcmd[13]^sendcheckcmd[14];
    sendcheckcmd[15]=crc;



    mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,16);

}
//停止
void MainWindow::on_pushButton_stop_2_clicked()
{
    if(connect_flg==0)
        return;
    float_union  vel_float ,vely_float,vela_float;
    QString vel = this->ui->lineEdit_v->text();//线速度

    vel_float.fvalue = vel.toFloat();
    QString ang = this->ui->lineEdit_a->text();

    vela_float.fvalue = ang.toFloat();
    vela_float.fvalue = 0 ;

    vely_float.fvalue =0;
    vel_float.fvalue =0;

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xff;
    sendcheckcmd[2]=15;
    memcpy(sendcheckcmd+3,vel_float.cvalue,4);
    memcpy(sendcheckcmd+7,vely_float.cvalue,4);
    memcpy(sendcheckcmd+11,vela_float.cvalue,4);
    unsigned char crc = sendcheckcmd[3]^sendcheckcmd[4]^sendcheckcmd[5]^sendcheckcmd[6]^
            sendcheckcmd[7]^sendcheckcmd[8]^sendcheckcmd[9]^sendcheckcmd[10]^
            sendcheckcmd[11]^sendcheckcmd[12]^sendcheckcmd[13]^sendcheckcmd[14];
    sendcheckcmd[15]=crc;


     mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,16);

}
//右转
void MainWindow::on_pushButton_ri_clicked()
{
    if(connect_flg==0)
        return;
    float_union  vel_float ,vely_float,vela_float;
    QString vel = this->ui->lineEdit_v->text();//线速度

    vel_float.fvalue = vel.toFloat();
    QString ang = this->ui->lineEdit_a->text();

    vela_float.fvalue = ang.toFloat();
    vela_float.fvalue =  0-vela_float.fvalue ;

    vely_float.fvalue =0;
    vel_float.fvalue =0;

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xff;
    sendcheckcmd[2]=15;
    memcpy(sendcheckcmd+3,vel_float.cvalue,4);
    memcpy(sendcheckcmd+7,vely_float.cvalue,4);
    memcpy(sendcheckcmd+11,vela_float.cvalue,4);
    unsigned char crc = sendcheckcmd[3]^sendcheckcmd[4]^sendcheckcmd[5]^sendcheckcmd[6]^
            sendcheckcmd[7]^sendcheckcmd[8]^sendcheckcmd[9]^sendcheckcmd[10]^
            sendcheckcmd[11]^sendcheckcmd[12]^sendcheckcmd[13]^sendcheckcmd[14];
    sendcheckcmd[15]=crc;

     mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,16);

}
//后退
void MainWindow::on_pushButton_6_clicked()
{
    if(connect_flg==0)
    {
            QMessageBox::information(NULL, "提醒", "请先连接设备！");
            return;
            }
    float_union  vel_float ,vely_float,vela_float;
    QString vel = this->ui->lineEdit_v->text();//线速度

    vel_float.fvalue = vel.toFloat();
    QString ang = this->ui->lineEdit_a->text();

    vela_float.fvalue = ang.toFloat();
    vela_float.fvalue = 0 ;

    vely_float.fvalue =0;
    vel_float.fvalue =0 -vel_float.fvalue ;

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xff;
    sendcheckcmd[2]=15;
    memcpy(sendcheckcmd+3,vel_float.cvalue,4);
    memcpy(sendcheckcmd+7,vely_float.cvalue,4);
    memcpy(sendcheckcmd+11,vela_float.cvalue,4);
    unsigned char crc = sendcheckcmd[3]^sendcheckcmd[4]^sendcheckcmd[5]^sendcheckcmd[6]^
            sendcheckcmd[7]^sendcheckcmd[8]^sendcheckcmd[9]^sendcheckcmd[10]^
            sendcheckcmd[11]^sendcheckcmd[12]^sendcheckcmd[13]^sendcheckcmd[14];
    sendcheckcmd[15]=crc;

     mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,16);

}

void MainWindow::on_pushButton_explore_clicked()
{

    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
        }
    if(press_flg_expolre == 0){
        press_flg_automapping =0;
        press_flg_manualmapping =0;
      ui->pushButton_explore->setStyleSheet("background-color: rgb(175,238,238)");
      ui->pushButton_manualmapping->setStyleSheet("");
      ui->pushButton_automapping->setStyleSheet("");
    }
    else {
        ui->pushButton_explore->setStyleSheet("");

    }
    press_flg_expolre=!press_flg_expolre;
    if(connect_flg==0)
    {
            QMessageBox::information(NULL, "提醒", "请先连接设备！");
            return;
            }

    run_mode = Explore_MODE;
   //f1 f1 1


}
void MainWindow::on_pushButton_automapping_clicked()
{

    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
        }
    if(press_flg_automapping ==0){
        press_flg_expolre = 0;

       press_flg_manualmapping =0;
      ui->pushButton_automapping->setStyleSheet("background-color: rgb(175,238,238)");
      ui->pushButton_explore->setStyleSheet("");
      ui->pushButton_manualmapping->setStyleSheet("");
    }
    else ui->pushButton_automapping->setStyleSheet("");
    press_flg_automapping=!press_flg_automapping;
    if(connect_flg==0)
        return;

    run_mode = AutoMapping_MODE;
}
void MainWindow::on_pushButton_manualmapping_clicked()
{

    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
        }
    if(press_flg_manualmapping ==0){
        press_flg_expolre = 0;
        press_flg_automapping =0;

        ui->pushButton_explore->setStyleSheet("");
        ui->pushButton_automapping->setStyleSheet("");
      ui->pushButton_manualmapping->setStyleSheet("background-color: rgb(175,238,238)");
    }
    else ui->pushButton_manualmapping->setStyleSheet("");
    press_flg_manualmapping=!press_flg_manualmapping;
    if(connect_flg==0)
        return;

    run_mode = Manual_MODE;

}

void MainWindow::on_pushButton_loadmapping_clicked()
{
    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
        }
}


void MainWindow::on_pushButton_setgoal_clicked()
{


    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
        }
    if(setGoalFlg ==0)
    {
            QMessageBox::information(NULL, "提醒", "请先设置目标点！");
            return;
     }
    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xf1;
    sendcheckcmd[2]=5;//length
    sendcheckcmd[3]=SetGoal_CMD;//包括规划路径
    sendcheckcmd[4]=target_position.x()>>8;//
    sendcheckcmd[5]=target_position.x();//
    sendcheckcmd[6]=target_position.y()>>8;//
    sendcheckcmd[7]=target_position.y();//
    for(int i=0;i<8;i++)
        printf("0x%x ",sendcheckcmd[i]);
    if(connect_flg==0)
    {
            QMessageBox::information(NULL, "提醒", "请先连接设备！");
            return;
    }
    //设置目标会直接让机器人按目标规划路径
    mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,8);
}
//void MainWindow::on_pushButton_pathplan_clicked()
//{
//    if(press_start ==1){
//        QMessageBox::information(NULL, "提醒", "请先停止运行！");
//        return;
//        }
//    unsigned char sendcheckcmd[16]={0x00};
//    sendcheckcmd[0]=sendcheckcmd[1]=0xf1;
//    sendcheckcmd[2]=1;//length
//    sendcheckcmd[3]=Pathplan_CMD;//


//    if(connect_flg==0)
//        return;
//    mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,4);
//}

void MainWindow::on_pushButton_cleargoal_clicked()
{
    if(press_start ==1){
        QMessageBox::information(NULL, "提醒", "请先停止运行！");
        return;
      }
    setGoalFlg =0;
    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xf1;
    sendcheckcmd[2]=1;//length
    sendcheckcmd[3]=Cleargoalplan_CMD;//
    if(connect_flg==0)
        return;
    mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,4);
}
void MainWindow::on_pushButton_switch_clicked()
{

    unsigned char sendcheckcmd[16]={0x00};
    sendcheckcmd[0]=sendcheckcmd[1]=0xf1;
    sendcheckcmd[2]=1;//length

    if(this->ui->pushButton_switch->text() == "启动"){
        press_start =1;
        sendcheckcmd[3]=run_mode;//
         ui->pushButton_switch->setStyleSheet("background-color: rgb(180,224,126)");
       // mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,4);
       this->ui->pushButton_switch->setText("停止");
    }else  if(this->ui->pushButton_switch->text() == "停止"){
        press_start =0;
         sendcheckcmd[3]= Stop_MODE;
         this->ui->pushButton_switch->setText("启动");
          ui->pushButton_switch->setStyleSheet("");
     }
    if(connect_flg==0)
        return;
    mPlayer->send_cmd_v_a_2stm32((char*)sendcheckcmd,4);
}



void MainWindow::on_pushButton_mapnavigation_clicked()
{

}
