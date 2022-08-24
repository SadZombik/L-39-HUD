// Класс-контейнер для сервера, УПУ и ИЛС

#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "UMU.h"
#include "HUD.h"
#include "TCP_server.h"
#include "test_widget.h"

class Device: public QObject
{
    Q_OBJECT

public:
    explicit Device(QObject *parent = nullptr);
    ~Device();
    DisplayDataPacket *planeParams;

private:
    HUD *hud;
    UMU *umu;
    TCP_server *server;

    QTimer timer;
    //test_widget t;
};

#endif // DEVICE_H
