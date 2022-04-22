#include "display.h"
#include "test_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int size = 512;
    Display hud(size, size * 1.2);
    hud.show();

    return a.exec();
}
