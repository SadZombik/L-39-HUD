#include <QApplication>
#include "device.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Device d;

    return a.exec();
}


