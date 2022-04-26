#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QSettings>
#include <QtMath>
#include "hud_tcp_client.h"
#include "test_widget.h"

class HUD: public QWidget
{
    Q_OBJECT

public:
    HUD(const quint16 width, const quint16 height, bool transparent = false, QWidget *parent = nullptr);

private:
    void rescale();

    // Размеры виджета
    quint16 w;
    quint16 h;

    // Линии и отступы
    QColor line_color;
    float line_width;
    float label_offset;
    bool use_mesh;

    // Выоста и ширина сетки индикаторов
    float top_level;
    float bottom_level;
    float left_level;
    float right_level;

    // Прицел
    float scope_size;           // Размер сетки прицела
    float scope_space;          // Пространство в перекрестии
    float turbine_offset;       // Отступ турбин от прицела

    // Крен
    float roll_outer_radius;    // Радиус внешней окружности
    float roll_inner_radius;    // Радиус внутренней окружности

    // Тангаж
    float pitch_line_len;       // Размер линии горизонта
    int pitch_font_size;        // Размер шрифта угла тангажа

    // Курс
    float yaw_line_len;         // Ширина шкалы курса
    float vert_lines_height;    // Высота линий шкалы
    float yaw_scale;            // Масштаб шкалы
    int yaw_font_size;          // Размер шрифта курса

    // Вертикальная скорость
    float vert_speed_line_len;  // Длина линии шкалы
    float scale_offset;         // Отступ от шкалы крена
    float vert_lines_width;     // Ширина вертикальных линий
    int vert_speed_font_size;   // Размер шрифта вертикальной скорости

    // Приборная воздушная скорость
    float ias_line_len;         // Ширина линии индикатора
    float prev_speed;           // Предыдущая скорость для сравнения ускорения
    int ias_font_size;          // Размер шрифта скорости

    // Точки для треугольников
    QPointF t1[3];
    QPointF t2[3];

    int height_font_size;       // Размер шрифта высоты
    int radio_height_font_size; // Размер шрифта радио высоты
    int mode_font_size;         // Размер шрифта индикатора режима работы
    int range_font_size;        // Размер шрифта индикатора расстояния
    int launch_font_size;       // Размер шрифта разрешения сброса/пуска

    HUD_TCP_client client;
    DisplayDataPacket &planeParams;

    test_widget test;

    // Рисовваие по режимам работы ИЛС
    void draw_nav(QPainter *qp);
    void draw_app(QPainter *qp);
    void draw_bomb(QPainter *qp);
    void draw_rocket(QPainter *qp);
    void draw_gun(QPainter *qp);
    void draw_air(QPainter *qp);
    void draw_vizier(QPainter *qp);

    // Элементы которые есть на всех экранах
    void draw(QPainter *qp, QString mode, bool radio);

    // Отдельные элементы
    // Сетка для позиционирования индикаторов
    void mesh(QPainter*);

    // Неподвижынй прицел
    void scope(QPainter*);

    // Шкала крена
    void roll_indicator(QPainter*);

    // Линия горизнта
    void pitch_indicator(QPainter*);

    // Шкала курса
    void vert_speed_indicator(QPainter*);

    // Шкала верткальной скорости
    void ias(QPainter*);

    // Приборная воздушная скорость
    void course(QPainter*);

    // Высота
    void height_indicator(QPainter*, bool);

    // Индикатор режима работы
    void mode_indicator(QPainter*, QString);

    // Индикатор расстояния до выбранной точки маршрута
    void waypoint_indicator(QPainter*);

    // Метка отклонения от глиссады
    void glide_mark(QPainter*);

    // Символы курсового и луча глиссады
    void glide_symbols(QPainter*);

    // Визирная метка и траектория падения бомб
    void bomb_mark(QPainter*);

    // Сигнал разрешения сброса/пуска
    void launch_signal(QPainter*);

    // Шкала дальности
    void range_scale(QPainter*);

    // Выбор оружия
    void weapon_switch(QPainter*, QString str);

    // Визирная метка
    void viz_mark(QPainter*);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void update_params();

};

#endif // DISPLAY_H
