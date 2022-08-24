#ifndef DISPLAYDATA_H
#define DISPLAYDATA_H

#include <QtGlobal>
// Заголовок пакета
struct DisplayTCPHeader
{
    quint32 PacketType;
    quint32 DataSize;
};

// Данные пакета
struct DisplayDataPacket
{
    bool PacketReady = 0;
    bool device_on = true;          // Вкл/Выкл
    float longitude = 0;            // Долгота
    float lattitude = 0;            // Широта
    float bar_height = 0;           // Барометрическая высота
    float radio_height = 0;         // Радио высота
    float roll = 0;                 // Крен
    float pitch = 0;                // Тангаж
    float yaw = 30;                 // Курс
    float engine_speed = 0;         // Скорость вращения двигателя
    float forward_speed = 0;        // Cкорость вперед
    float vertical_speed = 0;       // Cкорость вертикальная
    float sliding_angle = 0;        // Угол скольжения
    float long_acceleration = 0;    // Продольное ускорение
    float waypoint_dist = 0;        // Расстояние до выбранной путевой точки
    float fuel_quantity = 0;        // Количество топлива
    float fuel_consumption = 0;     // Расход топлива
    bool  launch_signal = 0;        // Сигнал разрешения сброса
    float range = 0;                // Текущая дальность (GUN , GND)
    float hit_point_x = 0;          // Точка попадания X
    float hit_point_y = 0;          // Точка попадания Y
    float hit_point_z = 0;          // Точка попадания Z
    float max_range = 0;            // Максимальная эффективная дальность стрельбы
    float min_range = 0;            // Минимальная эффективная дальность стрельбы
    float overload = 0;             // Нормальная перегрузка
    float scope_radius = 255;         // (0 - 255)
};

#endif // DISPLAYDATA_H
