#include "FireDogEditor.h"
#include <QtWidgets/QApplication>

#include <QFile>
#include <QTextStream>

//这个方法设置主题行不通，智能防盗mainwindow的构造函数里
//void initQss()
//{
//    QFile file(":/qss/css/qss.css");
//    if (!file.open(QIODevice::ReadOnly))
//        exit(0);
//
//    QTextStream in(&file);
//    QString css = in.readAll();
//    qApp->setStyleSheet(css);
//    qApp->setFont(QFont("微软雅黑", 9));
//
//    return;
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FireDogEditor w;

    //initQss();

    w.show();
    return a.exec();
}
