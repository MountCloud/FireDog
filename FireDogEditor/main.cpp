#include "FireDogEditor.h"
#include <QtWidgets/QApplication>

#include <QFile>
#include <QTextStream>

//����������������в�ͨ�����ܷ���mainwindow�Ĺ��캯����
//void initQss()
//{
//    QFile file(":/qss/css/qss.css");
//    if (!file.open(QIODevice::ReadOnly))
//        exit(0);
//
//    QTextStream in(&file);
//    QString css = in.readAll();
//    qApp->setStyleSheet(css);
//    qApp->setFont(QFont("΢���ź�", 9));
//
//    return;
//}

int main(int argc, char *argv[])
{
    int nExitCode = 0;
    do {
		QApplication a(argc, argv);
		FireDogEditor w;
		w.show();
		nExitCode = a.exec();
	} while (nExitCode == EXIT_CODE_REBOOT);
	return nExitCode;
}
