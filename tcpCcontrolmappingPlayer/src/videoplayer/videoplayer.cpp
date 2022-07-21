
#include "videoplayer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QImage>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
//extern "C"
//{
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libavutil/pixfmt.h"
//#include "libswscale/swscale.h"
//#include "libavfilter/avfilter.h"
//#include "libpostproc/postprocess.h"

//}

#include <QtNetwork>
#include <QTcpSocket>
#include <QHostAddress>
#include<stdio.h>                              // for printf
#include<stdlib.h>                             // for exit
#include<string.h>                             // for bzero
#include<unistd.h>
#define HELLO_WORLD_SERVER_PORT       7078
int BUFFER_SIZE= (INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT);

#define FILE_NAME_MAX_SIZE            512


VideoPlayer::VideoPlayer()
{
   // m_gui = gui;
	mVideoWidget = new show_vediowidget;
	QObject::connect(this, SIGNAL(sig_GetOneFrame(QImage)), mVideoWidget, SLOT(slotGetOneFrame(QImage)));

//


}

VideoPlayer::~VideoPlayer()
{

}

bool VideoPlayer::stop()
{
	mstate = Stop;
	if (mstate != Stop)
		return  false;
	return true;
}

bool VideoPlayer::play()
{
        mstate = Playing;
        if (mstate != Playing)
                return false;
        return true;
}

bool VideoPlayer::pause()
{
        mstate = Pause;
        if (mstate != Pause)
                return false;
        return true;
}

void VideoPlayer::startPlay()
{
	///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
	this->start();
}
void VideoPlayer::setSocket(QTcpSocket *sock,QString addr,int port)
{
 // client = sock;
  _addr =addr;
  _port =port;
}

void VideoPlayer::slotUpdateUi()
{
   //m_gui->Display();

}
void VideoPlayer::send_cmd_v_a_2stm32( char* da,int len)
{

  if(da[0]==0xf1)
  {
     // if(da[2]==AutoMapping_MODE);
  }
  client->write(da,len);
  client->flush();

}

int VideoPlayer::h264_decode(unsigned char *pic, unsigned char *buf, int *width,int *height)
{
    return 0;
}
int VideoPlayer::mapping_decode(int *width,int *height)
{
    unsigned char  *buffer=NULL;
    buffer=(unsigned char *)malloc(BUFFER_SIZE);
    QFile file("demo.mp4");
    QImage Img;
    int length = 0, length_total = 0, length_flags = 0;
 static char recflg =0;

    unsigned char Buff[300*300] = {0};
    /* socket recv h264 */
    while (1) {

           memset(Buff, 0,BUFFER_SIZE);
             memset(buffer, 0,BUFFER_SIZE);
           length_total = 0;
           if (length_flags == 0){
              if(client->waitForReadyRead()){

                     length =   client->read((char *)buffer,8);
               }
               if (length < 0)
               {
                   qDebug("Recieve Data From Server Failed!\n");
                   break;
               }
               qDebug("recv size:%d!!!\n",length);
               struct message_header *message = (struct message_header*)buffer;
               if ((message->type == 0x5a5a5a5a)||(message->type ==0xc9a4e3f7))
               {
                   length_flags = message->size;
                   qDebug("message size:%d!!!\n",length_flags);
               }
           }
           else {
               while(length_flags){
                    if(client->waitForReadyRead())
                    length =   client->read((char *)buffer,length_flags);
                   if (length < 0)
                   {
                       qDebug("Recieve Data From Server Failed!\n");
                       break;
                   }
                   if(recordVideo ==1){//开始 录制

                       if(recflg == 0){
                           recflg =1;
                           file.open(QIODevice::WriteOnly | QIODevice::Append);
                       }
                       file.write((char *)buffer,length_flags);

                   }else if(recordVideo ==0){
                            file.close();
                          }
                   memcpy(Buff+length_total,buffer,length);
                 //  Buff = Buff + length;
                   length_flags =length_flags - length;
                   length_total +=length;
                   memset(buffer, 0,BUFFER_SIZE);
               }
               qDebug("decode size:%d!!!\n",length_total);
              // buf = headh264;

               //把这个RGB数据 用QImage加载
            //   QImage tmpImg((uchar *)Buff, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, QImage::Format_RGB888);


                  QImage imageData(INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, QImage::Format_RGB888);
                  for (int x = 0;x < INPUT_IMAGE_WIDTH;x++) {
                      for (int y = 0;y < INPUT_IMAGE_HEIGHT;y++) {
                         unsigned char rgb =  Buff[x*300+y];
                              //宽高，rgb三色
                         if(rgb>150)
                             imageData.setPixel(y, x, QColor(235, 222,180).rgb());
                         else
                            imageData.setPixel(y, x, QColor(rgb, rgb,rgb).rgb());
                      }
                  }

               Img = imageData;
               if(takepic == 1){
                   takepic =0;
                   Img.save("demo.bmp");
               }
               emit sig_GetOneFrame(Img);
               //int sleep_t = get_metadate_fps > 6 ? get_metadate_fps : 6;
              msleep(49);

           }
      }
   return 0;
}

void VideoPlayer::run()
{
    //QImage Img;
    char cntflg =0;
    msleep(1000);
    int cnt = 0;

    //= avpicture_get_size(AV_PIX_FMT_RGB32, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT);

    //memset(pic, 0x5a, INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT);
    int width = INPUT_IMAGE_WIDTH;
    int height = INPUT_IMAGE_HEIGHT;
    int numBytes =width*height*3;
    while(1){
        client = new QTcpSocket(this);
    //    qDebug("prepare connect!!!\n");

        client->connectToHost(QHostAddress(_addr),_port);
        if (client->waitForConnected(5000))  // 连接则进入if{}
           {
               qDebug("链接成功\n");
                  cntflg=1;
                  emit show_fresh(1);
           }else {
            emit show_fresh(0);
        }


         printf("h264 decode!!!\n");
       //  h264_decode(pic, buf, &width, &height);
         mapping_decode( &width, &height);
         qDebug("%s videoPlayer stop\n", __func__);
    }
    return;
}

