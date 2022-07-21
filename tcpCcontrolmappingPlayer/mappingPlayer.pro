#-------------------------------------------------
#
# Project created by QtCreator 2019-06-01T16:10:47
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slam
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp \
    src/show_vediowidget.cpp \
    src/mainwindow.cpp \
    src/videoplayer/videoplayer.cpp

HEADERS  += \
    src/image_draw.h \
    src/show_vediowidget.h \
    src/mainwindow.h \
    src/videoplayer/videoplayer.h


FORMS    += \
    src/mainwindow.ui \
    src/show_vediowidget.ui
INCLUDEPATH += src/

#INCLUDEPATH +=  $$PWD/ffmpeg/include
#LIBS +=        -L$$PWD/ffmpeg/lib   -lavcodec  -lavformat -lavdevice -lavutil -lswresample -lswscale
#LIBS +=              -L E:\T20\AkubelliPalyer-aiqi\AkubelliPalyer-aiqi\ffmpeg\lib\libavcodec.dll.a
#LIBS +=                -L$$PWD/ffmpeg/lib/libavdevice.dll.a
#LIBS +=                $$PWD/ffmpeg/lib/libavfilter.dll.a
#LIBS +=                $$PWD/ffmpeg/lib/libavformat.dll.a
#LIBS +=                $$PWD/ffmpeg/lib/libavutil.dll.a
#LIBS +=                $$PWD/ffmpeg/lib/libswresample.dll.a
#LIBS +=                $$PWD/ffmpeg/lib/libswscale.dll.a
#LIBS += D:\opencv3\build\install\x86\mingw\lib\libopencv_calib3d320.dll.a

#LIBS += -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib/avformat.lib    \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lavdevice \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lavfilter \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lavformat \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lavutil \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lpostproc \
#        -LE:/T20/AkubelliPalyer-aiqi/AkubelliPalyer-aiqi/ffmpeg/lib -lswscale


DISTFILES +=

RESOURCES += \
    image.qrc

