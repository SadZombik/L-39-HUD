#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include "DisplayData.h"

class TCP_server: public QObject
{
    Q_OBJECT

public:
    explicit TCP_server(QObject *parent = nullptr);
    ~TCP_server();

    void setPort(int p) { port = p; }

    DisplayDataPacket displayPacket;

private:
    QTcpServer *server;
    QTcpSocket *socket;
    int port = 8003;

    void getDisplayDataPacket(QDataStream &);

private slots:
    void newConnection();
    void readCommonTcpData();

};

#endif // TCP_SERVER_H
