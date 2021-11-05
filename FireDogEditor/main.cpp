#include "FireDogEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FireDogEditor w;
    w.show();
    return a.exec();
}
