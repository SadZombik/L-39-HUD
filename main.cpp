#include "HUD.h"
#include "test_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int size = 512;
    HUD hud(size, size * 1.2, true);
    hud.show();

    return a.exec();
}
