#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QSettings>
#include <QtMath>
#include <QPixmap>

#include "hud_tcp_client.h"
#include "test_widget.h"

class HUD: public QWidget
{
    Q_OBJECT

public:
    HUD(const quint16 width, const quint16 height, bool transparent = false, QWidget *parent = nullptr);

    void select_mode(int m) { planeParams.hud_mode = m; }

private:
    HUD_TCP_client client;
    DisplayDataPacket &planeParams;
    test_widget test;

    // Размеры виджета
    quint16 w;
    quint16 h;
    void rescale();

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
    void scope(QPainter*);
    float scope_size;           // Размер сетки прицела
    float scope_space;          // Пространство в перекрестии
    float turbine_offset;       // Отступ турбин от прицела

    // Крен
    void roll_indicator(QPainter*);
    float roll_outer_radius;    // Радиус внешней окружности
    float roll_inner_radius;    // Радиус внутренней окружности

    // Тангаж
    void pitch_indicator(QPainter*);
    float pitch_line_len;       // Размер линии горизонта
    int pitch_font_size;        // Размер шрифта угла тангажа

    // Курс
    void course_incdicator(QPainter*);
    QPointF t2[3];
    float yaw_line_len;         // Ширина шкалы курса
    float vert_lines_height;    // Высота линий шкалы
    float yaw_scale;            // Масштаб шкалы
    int yaw_font_size;          // Размер шрифта курса

    // Вертикальная скорость
    void vert_speed_indicator(QPainter*);
    float vert_speed_line_len;  // Длина линии шкалы
    float scale_offset;         // Отступ от шкалы крена
    float vert_lines_width;     // Ширина вертикальных линий
    int vert_speed_font_size;   // Размер шрифта вертикальной скорости

    // Приборная воздушная скорость
    void ias(QPainter*);
    QPointF t1[3];
    float ias_line_len;         // Ширина линии индикатора
    float prev_speed;           // Предыдущая скорость для сравнения ускорения
    int ias_font_size;          // Размер шрифта скорости

    // Высота
    void height_indicator(QPainter*, bool);
    int height_font_size;       // Размер шрифта высоты
    int radio_height_font_size; // Размер шрифта радио высоты
    int range_font_size;        // Размер шрифта индикатора расстояния

    // Шкала дальности
    void range_indicator(QPainter*);
    float range_offset;         // Отступ шкалы от левого края
    float range_line_len;       // Высота шкалы
    int range_scale_font_size;  // Размер шрифта шкалы дальности

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
    int launch_font_size;       // Размер шрифта разрешения сброса/пуска

    // Выбор оружия
    void weapon_switch(QPainter*, QString str);

    // Визирная метка
    void viz_mark(QPainter*);

    // Неподвижный визир
    void viz_mesh(QPainter*);
    float viz_offset;
    QPixmap pix;

    // Индикатор режима работы
    void mode_indicator(QPainter*, QString);
    int mode_font_size;         // Размер шрифта индикатора режима работы

    // Сетка для позиционирования индикаторов (опционально)
    void mesh(QPainter*);

    // Элементы которые есть на всех экранах
    void draw(QPainter *qp, QString mode, bool radio);

    // Рисовваие по режимам работы ИЛС
    void draw_nav(QPainter*);
    void draw_app(QPainter*);
    void draw_bomb(QPainter*);
    void draw_rocket(QPainter*);
    void draw_gun(QPainter*);
    void draw_air(QPainter*);
    void draw_vizier(QPainter*);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void update_params();

};

#endif // DISPLAY_H
