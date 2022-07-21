
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QThread>
#include <QImage>
#include "show_vediowidget.h"
//#include "mainwindow.h"
#include <QtNetwork>
//extern "C"
//{
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libavutil/pixfmt.h"
//#include "libswscale/swscale.h"
//}


#define T02_VEDIO_INPUT_FORMAT	AV_PIX_FMT_NV12

//unsigned int INPUT_IMAGE_WIDTH=640; //1280
//unsigned int INPUT_IMAGE_HEIGHT=480; //720
#define VEDIO_LIST_NODES	10


enum PlayerState
{
	Playing,
	Pause,
	Stop
};



class VideoPlayer : public QThread
{
	Q_OBJECT

public:
    struct message_header{
        int type;
        int size;
    };
    explicit VideoPlayer();
	~VideoPlayer();

    int h264_decode(unsigned char *pic, unsigned char *buf, int *width,
                    int *height);
   int mapping_decode(int *width,int *height);
    void send_cmd_v_a_2stm32( char* da,int len);
	QWidget *getVideoWidget(){return mVideoWidget;}
	void startPlay();
	bool stop();
	bool play();
	bool pause();

    void setSocket(QTcpSocket *sock,QString addr,int port);

     QString _addr;
     unsigned int _port;
public slots:
    void slotUpdateUi();

signals:
	void sig_GetOneFrame(QImage); //没获取到一帧图像 就发送此信号
    void show_fresh(int);

protected:
	void run();
private:
 //   MainWindow *m_gui;
	show_vediowidget *mVideoWidget; //显示视频用的控件
	PlayerState mstate;
    QString currentForture;
    QTcpSocket *client;
    quint16 blockSize;

};


#endif // VIDEOPLAYER_H
