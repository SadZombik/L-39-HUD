#include "UMU.h"
#include "test_widget.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UMU umu1(896, 512);
    umu1.move(0, (1080-umu1.height())/2);
    umu1.show();

    return a.exec();
}


