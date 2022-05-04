#ifndef UMU_H
#define UMU_H

#include <QMainWindow>
#include <QFile>
#include <QPushButton>
#include <QKeyEvent>

#include "HUD.h"

enum Screens
{
    Hud,
    Misc,
    Radios,
    Xpdr,
    Nav,
    Main
};

namespace Ui {
class UMU;
}

class UMU : public QWidget
{
    Q_OBJECT

public:
    explicit UMU(quint16 width, quint16 height, QWidget *parent = nullptr);
    ~UMU();

private:
    HUD *hud;
    Ui::UMU *ui;

    void rescale();
    void read_config();

    quint16 w;
    quint16 h;

    // Кнопки
    QPushButton *main_button;
    QPushButton *nav_button;
    QPushButton *xpdr_button;
    QPushButton *radios_button;
    QPushButton *misc_button;
    QPushButton *hud_button;
    void set_color(QPushButton* btn);

    // Шрифты
    int main_menu_font_size;
    int nav_menu_font_size;
    int xpdr_menu_font_size;
    int radios_menu_font_size;
    int misc_menu_font_size;
    int hud_menu_font_size;

    Screens current_screen;
    void draw_main_menu(QPainter *qp);
    void draw_nav_menu(QPainter *qp);
    void draw_xpdr_menu(QPainter *qp);
    void draw_radios_menu(QPainter *qp);
    void draw_misc_menu(QPainter *qp);
    void draw_hud_menu(QPainter *qp);

    // Параметры экранов
    float hud_brightness_line_len;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void hud_button_clicked();
    void misc_button_clicked();
    void radios_button_clicked();
    void xpdr_button_clicked();
    void nav_button_clicked();
    void main_button_clicked();

};

#endif // UMU_H

//hud_button
//misc_button
//radios_button
//xpdr_button
//nav_button
//main_button
