#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QtNetwork>

class TCP_Client : public QObject
{
    Q_OBJECT

public:
    TCP_Client();
    ~TCP_Client();
    bool ClientConnected() { return clientConnected; }
protected:
    QHostAddress hostaddr;
    quint16 tcp_port;
    quint16 TxInterval;            // интервал в мс передачи TCP пакетов
    QString socketErrorString;
    QTcpSocket *TcpSocket;
    QTimer *TimerTCPSend;
    void ConnectToHost(QHostAddress addr, quint16 port);
private:
    bool clientConnected;          // флаг содинения с TCP сервером
private slots:
    void slotTcpError    (QAbstractSocket::SocketError);
    void slotConnected   ();
    void slotDisconnected();
    void slotTcpReconnect();
signals:
    void signalConnected();
    void signalDisconnected();
    void signalError();
};

#endif // TCP_CLIENT_H
