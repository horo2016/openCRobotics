﻿

#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码格式为UTF-8
	QTextCodec::setCodecForLocale(codec);

	MainWindow w;
	w.show();

	return a.exec();
}

