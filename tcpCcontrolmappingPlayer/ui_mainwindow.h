/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *widget_controller;
    QVBoxLayout *verticalLayout_7;
    QWidget *widget_video;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_open;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton_play;
    QPushButton *pushButton_pause;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton_stop;
    QWidget *widget_container;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_show_video;
    QLabel *label;
    QLabel *label_2;
    QLabel *constatus;
    QWidget *widget;
    QLabel *label_4;
    QLineEdit *lineEditaddr;
    QLabel *label_5;
    QLineEdit *lineEditport;
    QPushButton *pushButtoncnt;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QComboBox *comboBoxrate;
    QLabel *label_3;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QWidget *widget_2;
    QPushButton *pushButton_6;
    QPushButton *pushButton_stop_2;
    QPushButton *pushButton_run;
    QPushButton *pushButton_ri;
    QPushButton *pushButton_le;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QPushButton *pushButton_automapping;
    QPushButton *pushButton_manualmapping;
    QPushButton *pushButton_loadmapping;
    QPushButton *pushButton_setgoal;
    QPushButton *pushButton_cleargoal;
    QPushButton *pushButton_explore;
    QPushButton *pushButton_switch;
    QPushButton *pushButton_savmap;
    QLineEdit *lineEdit_a;
    QLineEdit *lineEdit_v;
    QLabel *label_6;
    QLabel *label_7;
    QPushButton *pushButton_mapnavigation;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1020, 626);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        widget_controller = new QWidget(centralwidget);
        widget_controller->setObjectName(QStringLiteral("widget_controller"));
        widget_controller->setGeometry(QRect(10, 550, 511, 70));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(18);
        sizePolicy.setVerticalStretch(29);
        sizePolicy.setHeightForWidth(widget_controller->sizePolicy().hasHeightForWidth());
        widget_controller->setSizePolicy(sizePolicy);
        widget_controller->setMinimumSize(QSize(0, 60));
        widget_controller->setStyleSheet(QLatin1String("QWidget#widget_controller\n"
"{\n"
"	background-color:rgb(27,100,100);\n"
"}"));
        verticalLayout_7 = new QVBoxLayout(widget_controller);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        widget_video = new QWidget(widget_controller);
        widget_video->setObjectName(QStringLiteral("widget_video"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget_video->sizePolicy().hasHeightForWidth());
        widget_video->setSizePolicy(sizePolicy1);
        widget_video->setStyleSheet(QStringLiteral(""));
        verticalLayout_10 = new QVBoxLayout(widget_video);
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        pushButton_open = new QPushButton(widget_video);
        pushButton_open->setObjectName(QStringLiteral("pushButton_open"));
        pushButton_open->setMinimumSize(QSize(36, 36));
        pushButton_open->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_open->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:image/open_normal.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:hover{ \n"
"image: url(:image/open_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:image/open_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_4->addWidget(pushButton_open);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        pushButton_play = new QPushButton(widget_video);
        pushButton_play->setObjectName(QStringLiteral("pushButton_play"));
        pushButton_play->setMinimumSize(QSize(50, 50));
        pushButton_play->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_play->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:image/start_normal.png);\n"
"border-radius:0px; \n"
"}  \n"
"QPushButton:hover{ \n"
"image: url(:image/start_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:image/start_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_4->addWidget(pushButton_play);

        pushButton_pause = new QPushButton(widget_video);
        pushButton_pause->setObjectName(QStringLiteral("pushButton_pause"));
        pushButton_pause->setMinimumSize(QSize(50, 50));
        pushButton_pause->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_pause->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:image/pause_normal.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:hover{ \n"
"image: url(:image/pause_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:image/pause_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_4->addWidget(pushButton_pause);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        pushButton_stop = new QPushButton(widget_video);
        pushButton_stop->setObjectName(QStringLiteral("pushButton_stop"));
        pushButton_stop->setMinimumSize(QSize(50, 50));
        pushButton_stop->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_stop->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:image/stop_normal.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:hover{ \n"
"image: url(:image/stop_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:image/stop_normal.png);\n"
"border-radius:0px; \n"
"}"));

        horizontalLayout_4->addWidget(pushButton_stop);


        verticalLayout_10->addLayout(horizontalLayout_4);


        verticalLayout_7->addWidget(widget_video);

        widget_container = new QWidget(centralwidget);
        widget_container->setObjectName(QStringLiteral("widget_container"));
        widget_container->setGeometry(QRect(20, 40, 500, 500));
        widget_container->setStyleSheet(QString::fromUtf8("QWidget#widget_container\n"
"{\n"
"	background-color: rgb(50,50, 86);\n"
"	border:1px solid rgba(0, 0, 0, 30);\n"
"	border-radius:2px;  /*\350\276\271\346\241\206\346\213\220\350\247\222*/\n"
"}\n"
""));
        verticalLayout_3 = new QVBoxLayout(widget_container);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_show_video = new QVBoxLayout();
        verticalLayout_show_video->setObjectName(QStringLiteral("verticalLayout_show_video"));

        verticalLayout_3->addLayout(verticalLayout_show_video);

        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 10, 451, 16));
        label->setStyleSheet(QStringLiteral("font-size:15px;color:blue"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(490, 10, 71, 21));
        label_2->setStyleSheet(QStringLiteral("font-size:15px;color:black"));
        constatus = new QLabel(centralwidget);
        constatus->setObjectName(QStringLiteral("constatus"));
        constatus->setGeometry(QRect(600, 10, 171, 21));
        QFont font;
        font.setPointSize(11);
        constatus->setFont(font);
        constatus->setStyleSheet(QStringLiteral(""));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(550, 40, 461, 141));
        widget->setStyleSheet(QStringLiteral("background-color:rgb(184,184,147);"));
        label_4 = new QLabel(widget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(0, 10, 71, 20));
        lineEditaddr = new QLineEdit(widget);
        lineEditaddr->setObjectName(QStringLiteral("lineEditaddr"));
        lineEditaddr->setGeometry(QRect(60, 10, 113, 20));
        label_5 = new QLabel(widget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(180, 10, 41, 21));
        lineEditport = new QLineEdit(widget);
        lineEditport->setObjectName(QStringLiteral("lineEditport"));
        lineEditport->setGeometry(QRect(220, 10, 61, 20));
        pushButtoncnt = new QPushButton(widget);
        pushButtoncnt->setObjectName(QStringLiteral("pushButtoncnt"));
        pushButtoncnt->setGeometry(QRect(224, 40, 51, 23));
        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(10, 62, 51, 31));
        pushButton_5 = new QPushButton(widget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setGeometry(QRect(10, 100, 61, 31));
        comboBoxrate = new QComboBox(widget);
        comboBoxrate->setObjectName(QStringLiteral("comboBoxrate"));
        comboBoxrate->setGeometry(QRect(140, 40, 81, 22));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(90, 40, 41, 21));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(900, 10, 75, 23));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(810, 10, 75, 23));
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(550, 190, 461, 401));
        widget_2->setStyleSheet(QStringLiteral("background-color:rgb(184,184,147);"));
        pushButton_6 = new QPushButton(widget_2);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setGeometry(QRect(200, 110, 51, 51));
        QFont font1;
        font1.setPointSize(12);
        pushButton_6->setFont(font1);
        pushButton_stop_2 = new QPushButton(widget_2);
        pushButton_stop_2->setObjectName(QStringLiteral("pushButton_stop_2"));
        pushButton_stop_2->setGeometry(QRect(200, 70, 51, 41));
        pushButton_stop_2->setFont(font);
        pushButton_run = new QPushButton(widget_2);
        pushButton_run->setObjectName(QStringLiteral("pushButton_run"));
        pushButton_run->setGeometry(QRect(200, 20, 51, 51));
        pushButton_run->setFont(font1);
        pushButton_ri = new QPushButton(widget_2);
        pushButton_ri->setObjectName(QStringLiteral("pushButton_ri"));
        pushButton_ri->setGeometry(QRect(260, 70, 61, 41));
        pushButton_ri->setFont(font1);
        pushButton_le = new QPushButton(widget_2);
        pushButton_le->setObjectName(QStringLiteral("pushButton_le"));
        pushButton_le->setGeometry(QRect(130, 70, 61, 41));
        pushButton_le->setFont(font);
        checkBox = new QCheckBox(widget_2);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(10, 20, 98, 16));
        checkBox->setChecked(true);
        checkBox_2 = new QCheckBox(widget_2);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        checkBox_2->setGeometry(QRect(10, 40, 111, 16));
        checkBox_2->setChecked(true);
        checkBox_3 = new QCheckBox(widget_2);
        checkBox_3->setObjectName(QStringLiteral("checkBox_3"));
        checkBox_3->setGeometry(QRect(10, 60, 121, 16));
        checkBox_3->setChecked(true);
        pushButton_automapping = new QPushButton(widget_2);
        pushButton_automapping->setObjectName(QStringLiteral("pushButton_automapping"));
        pushButton_automapping->setGeometry(QRect(110, 170, 71, 31));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_automapping->sizePolicy().hasHeightForWidth());
        pushButton_automapping->setSizePolicy(sizePolicy2);
        pushButton_manualmapping = new QPushButton(widget_2);
        pushButton_manualmapping->setObjectName(QStringLiteral("pushButton_manualmapping"));
        pushButton_manualmapping->setGeometry(QRect(200, 170, 101, 31));
        pushButton_loadmapping = new QPushButton(widget_2);
        pushButton_loadmapping->setObjectName(QStringLiteral("pushButton_loadmapping"));
        pushButton_loadmapping->setGeometry(QRect(320, 170, 121, 31));
        pushButton_setgoal = new QPushButton(widget_2);
        pushButton_setgoal->setObjectName(QStringLiteral("pushButton_setgoal"));
        pushButton_setgoal->setGeometry(QRect(20, 220, 161, 31));
        pushButton_cleargoal = new QPushButton(widget_2);
        pushButton_cleargoal->setObjectName(QStringLiteral("pushButton_cleargoal"));
        pushButton_cleargoal->setGeometry(QRect(200, 220, 121, 31));
        pushButton_explore = new QPushButton(widget_2);
        pushButton_explore->setObjectName(QStringLiteral("pushButton_explore"));
        pushButton_explore->setGeometry(QRect(20, 170, 81, 31));
        pushButton_switch = new QPushButton(widget_2);
        pushButton_switch->setObjectName(QStringLiteral("pushButton_switch"));
        pushButton_switch->setGeometry(QRect(20, 270, 75, 41));
        pushButton_savmap = new QPushButton(widget_2);
        pushButton_savmap->setObjectName(QStringLiteral("pushButton_savmap"));
        pushButton_savmap->setGeometry(QRect(110, 270, 75, 41));
        lineEdit_a = new QLineEdit(widget_2);
        lineEdit_a->setObjectName(QStringLiteral("lineEdit_a"));
        lineEdit_a->setGeometry(QRect(370, 140, 61, 20));
        lineEdit_v = new QLineEdit(widget_2);
        lineEdit_v->setObjectName(QStringLiteral("lineEdit_v"));
        lineEdit_v->setGeometry(QRect(90, 140, 71, 20));
        label_6 = new QLabel(widget_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 140, 51, 16));
        label_7 = new QLabel(widget_2);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(300, 140, 51, 16));
        pushButton_mapnavigation = new QPushButton(widget_2);
        pushButton_mapnavigation->setObjectName(QStringLiteral("pushButton_mapnavigation"));
        pushButton_mapnavigation->setGeometry(QRect(330, 220, 111, 31));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "h264Player", Q_NULLPTR));
        pushButton_open->setText(QString());
        pushButton_play->setText(QString());
        pushButton_pause->setText(QString());
        pushButton_stop->setText(QString());
        label->setText(QApplication::translate("MainWindow", "\346\254\242\350\277\216\344\275\277\347\224\250mapping\350\277\234\347\250\213\346\222\255\346\224\276\345\231\250 http://blog.cvosrobot.com/", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\347\212\266\346\200\201\357\274\232", Q_NULLPTR));
        constatus->setText(QApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:12pt;\">IP\345\234\260\345\235\200:</span></p></body></html>", Q_NULLPTR));
        lineEditaddr->setText(QApplication::translate("MainWindow", "192.168.31.135", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:12pt;\">\347\253\257\345\217\243\357\274\232</span></p></body></html>", Q_NULLPTR));
        lineEditport->setText(QApplication::translate("MainWindow", "5556", Q_NULLPTR));
        pushButtoncnt->setText(QApplication::translate("MainWindow", "\350\277\236\346\216\245", Q_NULLPTR));
        pushButton_4->setText(QApplication::translate("MainWindow", "\346\213\215\347\205\247", Q_NULLPTR));
        pushButton_5->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\345\275\225\345\210\266", Q_NULLPTR));
        comboBoxrate->clear();
        comboBoxrate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "300x300", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1280x720", Q_NULLPTR)
        );
        label_3->setText(QApplication::translate("MainWindow", "\345\210\206\350\276\250\347\216\207\357\274\232", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("MainWindow", "\345\270\256\345\212\251", Q_NULLPTR));
        pushButton_6->setText(QApplication::translate("MainWindow", "\345\220\216\351\200\200", Q_NULLPTR));
        pushButton_stop_2->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242", Q_NULLPTR));
        pushButton_run->setText(QApplication::translate("MainWindow", "\345\211\215\350\277\233", Q_NULLPTR));
        pushButton_ri->setText(QApplication::translate("MainWindow", "\345\217\263\350\275\254", Q_NULLPTR));
        pushButton_le->setText(QApplication::translate("MainWindow", "\345\267\246\350\275\254", Q_NULLPTR));
        checkBox->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\346\234\272\345\231\250\344\272\272", Q_NULLPTR));
        checkBox_2->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\350\275\250\350\277\271", Q_NULLPTR));
        checkBox_3->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\350\247\204\345\210\222\350\267\257\345\276\204", Q_NULLPTR));
        pushButton_automapping->setText(QApplication::translate("MainWindow", "\350\207\252\344\270\273\345\273\272\345\233\276", Q_NULLPTR));
        pushButton_manualmapping->setText(QApplication::translate("MainWindow", "\346\211\213\345\212\250\345\273\272\345\233\276", Q_NULLPTR));
        pushButton_loadmapping->setText(QApplication::translate("MainWindow", "\345\212\240\350\275\275\345\234\260\345\233\276", Q_NULLPTR));
        pushButton_setgoal->setText(QApplication::translate("MainWindow", "\350\256\276\345\256\232\347\233\256\346\240\207\347\202\271\345\271\266\350\247\204\345\210\222\350\267\257\345\276\204", Q_NULLPTR));
        pushButton_cleargoal->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\347\233\256\346\240\207\345\222\214\350\267\257\345\276\204", Q_NULLPTR));
        pushButton_explore->setText(QApplication::translate("MainWindow", "\350\207\252\344\270\273\346\216\242\347\264\242", Q_NULLPTR));
        pushButton_switch->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250", Q_NULLPTR));
        pushButton_savmap->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\234\260\345\233\276", Q_NULLPTR));
        lineEdit_a->setText(QApplication::translate("MainWindow", "0.0", Q_NULLPTR));
        lineEdit_v->setText(QApplication::translate("MainWindow", "0.05", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "\347\272\277\351\200\237\345\272\246\357\274\232", Q_NULLPTR));
        label_7->setText(QApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246\357\274\232", Q_NULLPTR));
        pushButton_mapnavigation->setText(QApplication::translate("MainWindow", "\345\234\260\345\233\276\350\207\252\344\270\273\345\257\274\350\210\252", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
