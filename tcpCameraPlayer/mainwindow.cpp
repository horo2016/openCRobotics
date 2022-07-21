

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
char filePath =0;//if  1  则打开本地文件 if 2 open internet stream
char takepic =0;
char recordVideo =0;
QString addr;
unsigned int port16;
unsigned int INPUT_IMAGE_WIDTH=640; //1280
unsigned int INPUT_IMAGE_HEIGHT=480;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);



    mPlayer = new VideoPlayer;

    ui->verticalLayout_show_video->addWidget(mPlayer->getVideoWidget()); //将显示视频的控件加入
    mPlayer->getVideoWidget()->show();

    usleep(50);



     connect(mPlayer, SIGNAL(show_fresh(int)), this, SLOT(mDisplay(int)));
}

MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::mDisplay(int a)
{
    if(a == 1){
        qDebug()<<"connected";
        this->ui->constatus->setText("已经连接");
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
        if(rate == "640x480"){
            INPUT_IMAGE_WIDTH=640; //1280
            INPUT_IMAGE_HEIGHT=480;
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
    }else if(this->ui->pushButtoncnt->text() == "断开"){
        QString addr = this->ui->lineEditaddr->text();
        QString port = this->ui->lineEditport->text();
        qDebug() << "ip addr port"<< addr << port;
        filePath =0;//代表文件来自网络
        this->ui->lineEditaddr->setDisabled(false);
         this->ui->lineEditport->setDisabled(false);
        this->ui->pushButtoncnt->setText("连接");
    }

}

void MainWindow::on_pushButton_play_clicked()
{   if(filePath == 1){//打开本地文件
        if(playpress ==0){
            mPlayer->startPlay();
            playpress = 1;
        }else {
            QMessageBox::warning(nullptr, "警告", "请先停止播放网络文件!", QMessageBox::Ok, QMessageBox::Ok);
        }
    }else if(filePath == 2)//打开网络文件
    {
        if(playpress ==0){
            mPlayer->startPlay();
            qDebug() << "启动播放";
            playpress = 1;
        }else {
            QMessageBox::warning(nullptr, "警告", "请先停止播放网络文件!", QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}

void MainWindow::on_pushButton_open_clicked()
{
    if(playpress == 0){
        QString s = QFileDialog::getOpenFileName(
                this, "选择要播放的文件",
                "/home/ppchen/Videos/8-16",//初始目录
                "视频文件 (*.flv *.rmvb *.avi *.MP4 *.wmv);; 所有文件 (*.*);; ");
        if (!s.isEmpty()) {
            filePath =1;
            mPlayer->startPlay();
            //sleep(1);
        }
        else {
            QMessageBox::warning(nullptr, "警告", "请选择输入文件!", QMessageBox::Ok, QMessageBox::Ok);
        }
    }else {
         QMessageBox::warning(nullptr, "警告", "请先停止播放网络文件!", QMessageBox::Ok, QMessageBox::Ok);
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

void MainWindow::on_pushButton_3_clicked()
{
    if(this->ui->pushButton_3->text() == "连接"){
        QString addr = this->ui->lineEditaddr->text();
        QString port = this->ui->lineEditp->text();


       unsigned int  port_16 =port.toUInt();

        client_fd = new QTcpSocket(this);
        qDebug("prepare connect!!!\n");

        client_fd->connectToHost(QHostAddress(addr),port_16);
        if (client_fd->waitForConnected(1000))  // 连接则进入if{}
           {
               qDebug("链接成功\n");
           }

        this->ui->lineEditp->setDisabled(true);
        this->ui->pushButton_3->setText("断开");
    }else if(this->ui->pushButton_3->text() == "断开"){

        client_fd->disconnectFromHost();
        this->ui->lineEditp->setDisabled(false);
        this->ui->pushButton_3->setText("连接");
    }
}
typedef union{
    unsigned char cvalue[4];
    float fvalue;
}float_union;
void MainWindow::on_pushButton_run_clicked()
{
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
   // qDebug()<<sendcheckcmd;



    client_fd->write((char*)sendcheckcmd,16);
}

void MainWindow::on_pushButton_le_clicked()
{




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




    client_fd->write((char*)sendcheckcmd,16);
}

void MainWindow::on_pushButton_stop_2_clicked()
{
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




    client_fd->write((char*)sendcheckcmd,16);
}

void MainWindow::on_pushButton_ri_clicked()
{
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




    client_fd->write((char*)sendcheckcmd,16);
}

void MainWindow::on_pushButton_6_clicked()
{
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




    client_fd->write((char*)sendcheckcmd,16);
}
