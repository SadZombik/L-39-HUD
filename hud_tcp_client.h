#ifndef DISPLAY_TCP_CLIENT_H
#define DISPLAY_TCP_CLIENT_H

#include <QObject>
#include <QDataStream>
#include <QSettings>
#include "tcp_client.h"

// Режимы работы ИЛС
enum Modes
{
    NAV,    // Режим навигации (NAV)
    APP,    // Режим захода на посадку (APP)
    BOMB,   // Воздух – Земля - Бомбометание
    ROCKET, // Воздух – Земля Неуправляемые ракеты
    GUN,    // Воздух – Земля симулированная ПУШКА
    AIR,    // Воздух - Воздух
    VIZIER  // Режим запасной неподвижный визир
};

struct DisplayTCPHeader
{
    quint16 PacketType;
    quint16 DataSize;
};

struct DisplayDataPacket
{
    bool PacketReady;

    float bar_height;           // Барометрическая высота
    float radio_height;         // Радио высота
    float roll;                 // Крен
    float pitch;                // Тангаж
    float yaw;                  // Курс
    float engine_speed;         // Скорость вращения двигателя
    float forward_speed;        // Cкорость вперед
    float vertical_speed;       // Cкорость вертикальная
    float sliding_angle;        // Угол скольжения
    float long_acceleration;    // Продольное ускорение
    quint8 hud_mode;            // Режим ИЛС
    float waypoint_dist;        // Расстояние до выбранной путевой точки
    bool launch_signal;         // Сигнал разрешения сброса
    float range;                // Текущая дальность (GUN , GND)
    float hit_point_x;          // Точка попадания X
    float hit_point_y;          // Точка попадания Y
    float hit_point_z;          // Точка попадания Z
    float max_range;            // Максимальная эффективная дальность стрельбы
    float min_range;            // Минимальная эффективная дальность стрельбы

};

class HUD_TCP_client: public TCP_Client
{
    Q_OBJECT

public:
    HUD_TCP_client();

    void set_address(QHostAddress address) { hostaddr = address; }
    void set_port(quint16 port) { tcp_port = port; }

    DisplayDataPacket displayPacket;

private:
    void ReadCommonTcpData();
    void GetDisplayDataPacket(QDataStream &_stream);

private slots:
    void OnReadTcpData();
};

#endif // DISPLAY_TCP_CLIENT_H
