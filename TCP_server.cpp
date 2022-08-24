#include "TCP_server.h"

TCP_server::TCP_server(QObject *parent):
    QObject(parent)
{
    server = new QTcpServer();
    server->setMaxPendingConnections(1);
    socket = new QTcpSocket();
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    server->listen(QHostAddress::Any, port);
}

TCP_server::~TCP_server()
{
    delete server;
}

void TCP_server::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readCommonTcpData()));
    qDebug() << "Connected";
}

void TCP_server::readCommonTcpData()
{
    //qDebug() << __FUNCTION__;
    bool processedHeader = false;
    DisplayTCPHeader displayHeader;

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        quint8 header_length = sizeof(DisplayTCPHeader); // 16 + 16 бит
        quint32 availabeBytes = socket->bytesAvailable();

        while(availabeBytes >= header_length)
        {
            if(!processedHeader)
            {
                QByteArray header_data = socket->read(header_length);
                QDataStream header_stream(header_data);
                header_stream.setByteOrder(QDataStream::LittleEndian);
                header_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

                header_stream >> displayHeader.PacketType;
                header_stream >> displayHeader.DataSize;
                processedHeader = true;
                availabeBytes = availabeBytes - header_length;
            }

            if((processedHeader) && (availabeBytes >= displayHeader.DataSize))
            {
                QByteArray packet_data = socket->read(displayHeader.DataSize);
                QDataStream data_stream(packet_data);
                data_stream.setByteOrder(QDataStream::LittleEndian);
                data_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                switch (displayHeader.PacketType)
                {
                case 430:
                    getDisplayDataPacket(data_stream);
                    break;
                default:
                    break;
                }
            }
            availabeBytes = socket->bytesAvailable();
            processedHeader = false;
        }
    }
}

void TCP_server::getDisplayDataPacket(QDataStream &_stream)
{
    displayPacket.PacketReady = true;
    _stream >> displayPacket.device_on;
    _stream >> displayPacket.longitude;
    _stream >> displayPacket.lattitude;
    _stream >> displayPacket.bar_height;
    _stream >> displayPacket.radio_height;
    _stream >> displayPacket.roll;
    _stream >> displayPacket.pitch;
    _stream >> displayPacket.yaw;
    _stream >> displayPacket.engine_speed;
    _stream >> displayPacket.forward_speed;
    _stream >> displayPacket.vertical_speed;
    _stream >> displayPacket.sliding_angle;
    _stream >> displayPacket.long_acceleration;
    _stream >> displayPacket.waypoint_dist;
    _stream >> displayPacket.fuel_quantity;
    _stream >> displayPacket.fuel_consumption;
    _stream >> displayPacket.launch_signal;
    _stream >> displayPacket.range;
    _stream >> displayPacket.hit_point_x;
    _stream >> displayPacket.hit_point_y;
    _stream >> displayPacket.hit_point_z;
    _stream >> displayPacket.max_range;
    _stream >> displayPacket.min_range;
    _stream >> displayPacket.overload;
    _stream >> displayPacket.scope_radius;
}
