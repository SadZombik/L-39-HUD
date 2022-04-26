#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QSettings>
#include <QtMath>
#include "display_tcp_client.h"
#include "test_widget.h"

class Display: public QWidget
{
    Q_OBJECT

public:
    Display(const quint16 width, const quint16 height, QWidget *parent = nullptr);

private:
    void rescale();

    // Размеры виджета
    quint16 w;
    quint16 h;

    // Линии и отступы
    quint16 line_width;
    QColor line_color;
    quint16 label_offset;

    // Текущий режим ИЛС
    Modes mode;

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
    float course_scale_size;    // Размер шкалы курса

    // Вертикальная скорость
    float vert_speed_line_len;  // Длина линии шкалы
    float scale_offset;         // Отступ от шкалы крена
    float vert_lines_width;     // Ширина вертикальных линий
    int vert_speed_font_size;   // Размер шрифта вертикальной скорости

    // Выоста и ширина сетки индикаторов
    float top_level;
    float bottom_level;
    float left_level;
    float right_level;

    // Приборная воздушная скорость
    float ias_line_len;         // Ширина линии индикатора
    float prev_speed;           // Предыдущая скорость для сравнения ускорения
    int ias_font_size;          // Размер шрифта скорости

    // Точки для треугольников
    QPointF t1[3];
    QPointF t2[3];

    // Шкала курса
    float yaw_line_len;         // Ширина шкалы курса
    float vert_lines_height;    // Высота линий шкалы
    float yaw_scale;            // Масштаб шкалы
    int yaw_font_size;          // Размер шрифта курса

    // Высота

    int height_font_size;
    int radio_height_font_size;

    Display_TCP_Client client;
    DisplayDataPacket &planeParams;

    test_widget test;

    // Рисование
    // По режимам работы ИЛС
    void draw_nav(QPainter *qp);
    void draw_app(QPainter *qp);
    void draw_bomb(QPainter *qp);
    void draw_rocket(QPainter *qp);
    void draw_gun(QPainter *qp);
    void draw_air(QPainter *qp);
    void draw_vizier(QPainter *qp);

    // Отдельные элементы
    void mesh(QPainter *qp);
    void scope(QPainter *s_qp);
    void roll_indicator(QPainter *r_qp, float roll);
    void pitch_indicator(QPainter *p_qp, float pitch);
    void vert_speed_indicator(QPainter *vs_qp, float v_speed);
    void ias(QPainter *i_qp, float speed);
    void course(QPainter *c_qp, float yaw);
    void flight_height(QPainter *h_qp, float height, float r_height);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void update_params();

};

#endif // DISPLAY_H
