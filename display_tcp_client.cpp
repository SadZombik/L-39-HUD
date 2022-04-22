#include "display_tcp_client.h"

Display_TCP_Client::Display_TCP_Client()
{    
    connect(TcpSocket, SIGNAL(readyRead()), this, SLOT(OnReadTcpData()));
}

void Display_TCP_Client::OnReadTcpData()
{
    ReadCommonTcpData();
}

void Display_TCP_Client::ReadCommonTcpData()
{
    bool processedHeader = false;
    DisplayTCPHeader displayHeader;

    if(TcpSocket->state() == QAbstractSocket::ConnectedState)
    {
       quint8 header_length = sizeof(DisplayTCPHeader); // 16 + 16 бит
       quint32 availabeBytes = TcpSocket->bytesAvailable();

       while(availabeBytes >= header_length)
       {
           if(!processedHeader)
           {
               QByteArray header_data = TcpSocket->read(header_length);

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
               QByteArray packet_data = TcpSocket->read(displayHeader.DataSize);
               QDataStream data_stream(packet_data);
               data_stream.setByteOrder(QDataStream::LittleEndian);
               data_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
               switch (displayHeader.PacketType)
               {
               case 431:
                   GetDisplayDataPacket(data_stream);
                   break;
               default:
                   break;
               }
           }
           availabeBytes = TcpSocket->bytesAvailable();
           processedHeader = false;
       }
    }
}


void Display_TCP_Client::GetDisplayDataPacket(QDataStream &_stream)
{
    displayPacket.PacketReady = true;
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
    _stream >> displayPacket.hud_mode;
    _stream >> displayPacket.waypoint_dist;
    _stream >> displayPacket.reset_signal;
    _stream >> displayPacket.range;
    _stream >> displayPacket.hit_point_x;
    _stream >> displayPacket.hit_point_y;
    _stream >> displayPacket.hit_point_z;
    _stream >> displayPacket.max_range;
    _stream >> displayPacket.min_range;

}
