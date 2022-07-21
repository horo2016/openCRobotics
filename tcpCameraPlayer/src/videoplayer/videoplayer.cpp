
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
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfilter.h"
#include "libpostproc/postprocess.h"

}

#include <QtNetwork>
#include <QTcpSocket>
#include <QHostAddress>
#include<stdio.h>                              // for printf
#include<stdlib.h>                             // for exit
#include<string.h>                             // for bzero
#include<unistd.h>
#define HELLO_WORLD_SERVER_PORT       7078
int BUFFER_SIZE= (INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT); //1280

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
void VideoPlayer::slotUpdateUi()
{
   //m_gui->Display();

}
int VideoPlayer::h264_decode(unsigned char *pic, unsigned char *buf, int *width,int *height)
{
    QImage Img;
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB, *pFrameJPG;
    AVPacket *packet;
    uint8_t *out_buffer;
    uint64_t frame_id = 0;

    QFile file("demo.h264");

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

    av_register_all(); //初始化FFMPEG  调用了这个才能正常适用编码器和解码器


    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    ///查找解码器
    pCodecCtx = avcodec_alloc_context3(pCodec);
        //初始化参数，下面的参数应该由具体的业务决定
    pCodecCtx->time_base.num = 1;
    pCodecCtx->frame_number = 1; //每包一个视频帧
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->bit_rate = 0;
    pCodecCtx->time_base.den = 30;//帧率
    pCodecCtx->width = *width;//视频宽
    pCodecCtx->height = *height;//视频高
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_YUYV422;//;

    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return 0;
    }
    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return 0;
    }
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    pFrameJPG = av_frame_alloc();


    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
            pCodecCtx->width, pCodecCtx->height);

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_init_packet(packet); //分配packet的数据
    int frameFinished = 200000;//这个是随便填入数字，没什么作用
    static char recflg =0;

    unsigned char  *buffer=NULL;
    buffer=(unsigned char *)malloc(BUFFER_SIZE);
    unsigned char *headh264 = buf;
    // 从服务器端接收数据到buffer中

    int length = 0, length_total = 0, length_flags = 0;

    int length_test = 0;

    /* socket recv h264 */
    while (1) {

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
                   memcpy(buf,buffer,length);
                   buf = buf + length;
                   length_flags =length_flags - length;
                   length_total +=length;
                   memset(buffer, 0,BUFFER_SIZE);
               }
               qDebug("decode size:%d!!!\n",length_total);
               buf = headh264;

               packet->data = buf;//这里填入一个指向完整H264数据帧的指针
               packet->size = 200000;//这个填入H264数据帧的大小
               ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

               if (ret < 0) {
                   printf("%s,%d,decode error.\n",__func__,__LINE__);
                 //  return 0;
               }

               if (got_picture) {
                   sws_scale(img_convert_ctx,
                           (uint8_t const * const *) pFrame->data,
                           pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                           pFrameRGB->linesize);
                   memcpy(pic, pFrameRGB->data[0], numBytes);
               }
               //把这个RGB数据 用QImage加载
               QImage tmpImg((uchar *)pic, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, QImage::Format_RGB32);
               Img = tmpImg;
               if(takepic == 1){
                   takepic =0;
                   Img.save("demo.bmp");
               }
               emit sig_GetOneFrame(Img);
               //int sleep_t = get_metadate_fps > 6 ? get_metadate_fps : 6;
               //msleep(1000/30);

           }
      }


    av_free_packet(packet);
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    return 0;
}

void VideoPlayer::run()
{
    //QImage Img;
    char cntflg =0;
    msleep(1000);
    int cnt = 0;
    unsigned char *pic =NULL;
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT);
    pic = (unsigned char *)malloc(numBytes);
    //memset(pic, 0x5a, INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT);
    int width = INPUT_IMAGE_WIDTH;
    int height = INPUT_IMAGE_HEIGHT;
    client = new QTcpSocket(this);
    qDebug("prepare connect!!!\n");

    client->connectToHost(QHostAddress(addr),port16);
    if (client->waitForConnected(1000))  // 连接则进入if{}
       {
           qDebug("链接成功\n");
              cntflg=1;
              emit show_fresh(1);
         //   this->ui->constatus->setText("连接成功");

       }
        // exit(1);

    unsigned char *buf = NULL;
     buf = (unsigned char *)malloc(BUFFER_SIZE);
     printf("h264 decode!!!\n");
     h264_decode(pic, buf, &width, &height);
     //h264_decode(pic, buf, &width, &height,0);

     qDebug("%s videoPlayer stop\n", __func__);
    free(pic);
    free(buf);
    return;
}

