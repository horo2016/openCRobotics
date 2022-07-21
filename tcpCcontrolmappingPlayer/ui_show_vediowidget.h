/********************************************************************************
** Form generated from reading UI file 'show_vediowidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOW_VEDIOWIDGET_H
#define UI_SHOW_VEDIOWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_show_vediowidget
{
public:

    void setupUi(QWidget *show_vediowidget)
    {
        if (show_vediowidget->objectName().isEmpty())
            show_vediowidget->setObjectName(QStringLiteral("show_vediowidget"));
        show_vediowidget->resize(300, 300);

        retranslateUi(show_vediowidget);

        QMetaObject::connectSlotsByName(show_vediowidget);
    } // setupUi

    void retranslateUi(QWidget *show_vediowidget)
    {
        show_vediowidget->setWindowTitle(QApplication::translate("show_vediowidget", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class show_vediowidget: public Ui_show_vediowidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOW_VEDIOWIDGET_H
