#ifndef UMU_H
#define UMU_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QApplication>
#include <QTime>
#include "DisplayData.h"
#include "Modes.h"

class UMU : public QWidget
{
    Q_OBJECT

public:
    explicit UMU(QWidget *parent = nullptr);
    ~UMU();

    void setPlaneParams(DisplayDataPacket *ddp) { planeParams = ddp; }

    int *Brightness() { return &hud_brightness; }
    Modes *Mode() { return &hud_mode; }

private:
    // Параметры виджета
    int w;
    int h;
    int x_move, y_move;
    float line_width;
    void read_config();
    QSettings *ini;

    // Параметры самолета
    int code;
    DisplayDataPacket *planeParams;

    // Поля для ИЛС
    int hud_brightness;
    Modes hud_mode;

    // Указатели на функции отрисовки
    typedef void (UMU::*hud_menu)(QPainter *qp);
    typedef void (UMU::*umu_menu)(QPainter *qp);
    hud_menu hud_menu_ptr = nullptr;
    umu_menu draw_menu_ptr = nullptr;

    // Функции отрисовки HUD меню
    void hud_nav(QPainter *qp);
    void hud_app(QPainter *qp);
    void hud_a_a(QPainter *qp);
    void hud_a_g(QPainter *qp);
    float hud_brightness_line_len;

    // Функции отрисовки UMU меню
    void draw_main_menu(QPainter *qp);
    void draw_nav_menu(QPainter *qp);
    void draw_xpdr_menu(QPainter *qp);
    void draw_radios_menu(QPainter *qp);
    void draw_misc_menu(QPainter *qp);
    void draw_hud_menu(QPainter *qp);

    // Шрифты
    int hud_menu_font_size;
    int main_menu_font_size;
    int nav_menu_font_size;
    int xpdr_menu_font_size;
    int radios_menu_font_size;
    int misc_menu_font_size;
    int buttons_font_size;

    // Кнопки
    void draw_buttons(QPainter *qp);
    void bottom_buttons(int key);
    void left_buttons(int key);
    void right_buttons(int key);
    int b_index = 0;    // Индексы хранят
    int l_index = 0;    // в себе текущую
    int r_index = 0;    // выбранную кнопнку
    QRectF rects_left[4];
    QRectF rects_right[4];
    int dy;             // Высота кнопок

    void calc_rects();

    // MAIN меню
    QRectF COM2;
    QRectF NAV1;
    QRectF R_main[8];

    // NAV меню
    QRectF R_nav[4];
    QString degConvert(float const deg)
    {
        int d = static_cast<int>(deg);
        int m = (deg - d) * 60;
        float s = (deg - d - float(m)/60) * 3600;

        return QString("%1").arg(d, 3) + "°"
                + QString("%1").arg(m, 2) + "'"
                + QString("%1").arg(s, 2) + "\"";
    }

    QRectF ID[5], ETA[5], EFR[5];
    QRectF MC[6], DIST[6], ETE[6];
    QColor cl = QColor("#246FE0");

    float y;
    float _dy;
    float _dx;
    float mc_dx;
    float dist_dx;

    void subtable(QRectF (&arr)[6], float width, float prev_x, float prev_w)
    {
        arr[0] = QRectF(prev_x + prev_w, y, width, _dy);
        arr[1] = QRectF(arr[0].x(), y + _dy, width, _dy/2);
        arr[2] = QRectF(arr[0].x(), arr[1].y() + _dy/2, width, _dy);
        arr[3] = QRectF(arr[0].x(), arr[2].y() + _dy, width, _dy);
        arr[4] = QRectF(arr[0].x(), arr[3].y() + _dy, width, _dy);
        arr[5] = QRectF(arr[0].x(), arr[4].y() + _dy, width, _dy/2);
    }

    // RADIOS меню
    int COM2_1 = 07, COM2_2 = 131, COM2_3 = 550;
    int spacing = 25;

    // MISC меню
    QRectF R_misc[3];

    // HUD меню
    QRectF R_hud_l[4];
    QRectF R_hud_r[4];

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

public slots:
    void keyPressedSlot(QKeyEvent *event);
    void closeEventSlot();

};

#endif // UMU_H
