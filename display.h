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

    void select_mode(Modes m) { mode = m; }

private:
    void rescale();

    // Размеры виджета
    quint16 w;
    quint16 h;

    // Текущий режим ИЛС
    Modes mode;

    // Прицел
    float scope_size;           // Размер сетки прицела
    float scope_space;          // Пространство в перекрестии

    // Крен
    float roll_scale_size;      // Размер шкалы крена
    float roll_outer_radius;    // Радиус внешней окружности
    float roll_inner_radius;    // Радиус внутренней окружности
    float radius_diff;          // Разница радиусов


    // Тангаж
    float pitch_line_size;      // Размер линии горизонта

    // Курс
    float course_scale_size;    // Размер шкалы курса

    Display_TCP_Client client;
    DisplayDataPacket &planeParams;

    test_widget t;

    // Рисование
    void draw_x(QPainter *qp);
    // По режимам работы ИЛС
    void draw_nav(QPainter *qp);
    void draw_app(QPainter *qp);
    void draw_bomb(QPainter *qp);
    void draw_rocket(QPainter *qp);
    void draw_gun(QPainter *qp);
    void draw_air(QPainter *qp);
    void draw_vizier(QPainter *qp);

    // Отдельные элементы
    void scope(QPainter *s_qp);
    void roll_scale(QPainter *r_qp, float roll);


protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void update_params();

};

#endif // DISPLAY_H
