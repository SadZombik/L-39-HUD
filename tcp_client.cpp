#include "tcp_client.h"

TCP_Client::TCP_Client()
{
    clientConnected = false;
    hostaddr = "";
    tcp_port = 0;
    TxInterval = 0;
    socketErrorString = "";

    TcpSocket = new QTcpSocket(this);
    TcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    TimerTCPSend = new QTimer(this);
    TimerTCPSend->setTimerType(Qt::PreciseTimer);

    connect(TcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(TcpSocket, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
    connect(TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotTcpError(QAbstractSocket::SocketError)));
}

TCP_Client::~TCP_Client()
{
    delete TimerTCPSend;
    TcpSocket->disconnectFromHost();
    delete TcpSocket;
}

void TCP_Client::ConnectToHost(QHostAddress addr, quint16 port)
{
    if(!(QAbstractSocket::ConnectedState == TcpSocket->state()))
    {
        TcpSocket->connectToHost(addr, port, QIODevice::ReadWrite);
    }
}

void TCP_Client::slotConnected()
{
    TcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    clientConnected = true;
    emit signalConnected();
}

void TCP_Client::slotDisconnected()
{
    clientConnected = false;
    emit signalDisconnected();
}

void TCP_Client::slotTcpReconnect()
{
    ConnectToHost(hostaddr, tcp_port);
}

void TCP_Client::slotTcpError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
        case 0:
            socketErrorString = "The connection was refused by the peer (or timed out)";
        break;
        case 1:
            socketErrorString = "The remote host closed the connection";
        break;
        case 2:
            socketErrorString = "The host address was not found";
        break;
        case 3:
            socketErrorString = "The socket operation failed because the application lacked the required privileges";
        break;
        case 4:
            socketErrorString = "The local system ran out of resources (e.g., too many sockets)";
        break;
        case 5:
            socketErrorString = "The socket operation timed out";
        break;
        case 6:
            socketErrorString = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)";
        break;
        case 7:
            socketErrorString = "An error occurred with the network (e.g., the network cable was accidentally plugged out)";
        break;
        case 8:
            socketErrorString = "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive";
        break;
        case 9:
            socketErrorString = "The address specified to QAbstractSocket::bind() does not belong to the host";
        break;
        case 10:
            socketErrorString = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)";
        break;
        case 11:
            socketErrorString = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)";
        break;
        case 12:
            socketErrorString = "The socket is using a proxy, and the proxy requires authentication";
        break;
        case 13:
            socketErrorString = "The SSL/TLS handshake failed, so the connection was closed";
        break;
        case 14:
            socketErrorString = "Could not contact the proxy server because the connection to that server was denied";
        break;
        case 15:
            socketErrorString = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
        break;
        case 16:
            socketErrorString = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase";
        break;
        case 17:
            socketErrorString = "The proxy address set with setProxy() (or the application proxy) was not found";
        break;
        case 18:
            socketErrorString = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood";
        break;
        case 19:
            socketErrorString = "An operation was attempted while the socket was in a state that did not permit it";
        break;
        case 20:
            socketErrorString = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library";
        break;
        case 21:
            socketErrorString = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library";
        break;
        case 22:
            socketErrorString = "A temporary error occurred (e.g., operation would block and socket is non-blocking)";
        break;
        case -1:
            socketErrorString = "An unidentified error occurred";
        break;
        default:
            socketErrorString = "";
        break;
    }
    emit signalError();
    TcpSocket->disconnectFromHost();
    // Try to reconnect after 3 seconds
    QTimer::singleShot(3000, this, SLOT(slotTcpReconnect()));
}
