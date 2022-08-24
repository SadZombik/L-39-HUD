#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QSettings>
#include <QtMath>
#include <QPixmap>
#include <QKeyEvent>
#include <QStaticText>
#include "DisplayData.h"
#include "Modes.h"

class HUD: public QWidget
{
    Q_OBJECT

public:
    explicit HUD(QWidget *parent = nullptr);
    ~HUD();

    void setPlaneParams(DisplayDataPacket *ddp) { planeParams = ddp; }
    void setMode(Modes *m) { mode = m; }
    void setBrightness(int *b) { brightness = b; }

private:
    // Эти переменные являются указателями так как ссылаются на поля класса УПУ
    DisplayDataPacket *planeParams;
    int *brightness;
    Modes *mode;

    QSettings *ini;
    void read_config();
    void calc_lines();

    // Размеры виджета
    int w;
    int h;
    float w_scale;
    float h_scale;

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
    void scope(QPainter*);
    QLineF scope_lines[4];

    // Крен
    float roll_outer_radius;    // Радиус внешней окружности
    float roll_inner_radius;    // Радиус внутренней окружности
    void roll_indicator(QPainter*);
    float R, r, dr, angle, deg_5, s_len;
    QLineF roll_lines[6];

    // Тангаж
    float pitch_line_len;       // Размер линии горизонта
    int pitch_font_size;        // Размер шрифта угла тангажа
    void pitch_indicator(QPainter*);

    // Курс
    QPointF t2[3];              // Координаты треугольника
    float yaw_line_len;         // Ширина шкалы курса
    float vert_lines_height;    // Высота линий шкалы
    float yaw_scale;            // Масштаб шкалы
    int yaw_font_size;          // Размер шрифта курса
    void course_incdicator(QPainter*);

    // Вертикальная скорость
    float vert_speed_line_len;  // Длина линии шкалы
    float vert_scale_offset;    // Отступ от шкалы крена
    float vert_lines_width;     // Ширина вертикальных линий
    int vert_speed_font_size;   // Размер шрифта вертикальной скорости
    void vert_speed_indicator(QPainter*);

    // Шкала перегрузки
    void overload_indicator(QPainter*);

    // Приборная воздушная скорость
    QPointF t1[3];              // Тееденция ускорения
    float ias_line_len;         // Ширина линии индикатора
    float ias_a_coef;           // Коэффициент смещения стрелки
    int ias_font_size;          // Размер шрифта скорости
    void ias(QPainter*);
    QRectF ias_rect;

    // Высота
    int height_font_size;       // Размер шрифта высоты
    int radio_height_font_size; // Размер шрифта радио высоты
    int range_font_size;        // Размер шрифта индикатора расстояния
    void height_indicator(QPainter*, bool);
    QRectF r_rect;

    // Шкала дальности
    float range_offset;         // Отступ шкалы от левого края
    float range_line_len;       // Высота шкалы
    int range_scale_font_size;  // Размер шрифта шкалы дальности
    void range_indicator(QPainter*);

    // Индикатор расстояния до выбранной точки маршрута
    void waypoint_indicator(QPainter*);

    // Метка отклонения от глиссады
    void glide_mark(QPainter*);

    // Символы курсового и луча глиссады
    void glide_symbols(QPainter*);

    // Визирная метка и траектория падения бомб
    void bomb_mark(QPainter*);

    // Сигнал разрешения сброса/пуска
    int launch_font_size;       // Размер шрифта разрешения сброса/пуска
    void launch_signal(QPainter*);

    // Выбор оружия
    void weapon_switch(QPainter*, QString);

    // Визирная метка
    float viz_coef;             // Коэффициент радиуса визирной метки
    void viz_mark(QPainter*);

    // Неподвижный визир
    float viz_offset;
    QPixmap pix;
    void viz_mesh(QPainter*);

    // Индикатор режима работы
    int mode_font_size;         // Размер шрифта индикатора режима работы
    void mode_indicator(QPainter*, QString);

    // Сетка для позиционирования индикаторов (опционально)
    void mesh(QPainter*);

    // Элементы которые есть на всех экранах
    void draw(QPainter *, QString, bool);

    // Рисовваие по режимам работы ИЛС
    void draw_nav(QPainter*);
    void draw_app(QPainter*);
    void draw_bomb(QPainter*);
    void draw_rocket(QPainter*);
    void draw_gun(QPainter*);
    void draw_air(QPainter*);
    void draw_vizier(QPainter*);

    // Для отладки на нормальном экране
    bool debug;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void keyPressed(QKeyEvent *event);
    void closeEventSignal();
};

#endif // DISPLAY_H
