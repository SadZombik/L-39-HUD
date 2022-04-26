#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>


class test_widget: public QWidget
{

    Q_OBJECT

public:
    test_widget(QWidget *parent = nullptr);
    float getRoll() { return roll_slider->value(); }
    float getPitch() { return pitch_slider->value(); }
    float getYaw() { return yaw_slider->value(); }
    float getVertSpeed() { return v_speed_slider->value(); }
    float getForwSpeed() { return f_speed_slider->value(); }

private:
    QLabel *roll_label;
    QSlider *roll_slider;

    QLabel *pitch_label;
    QSlider *pitch_slider;

    QLabel *yaw_label;
    QDoubleSpinBox *yaw_slider;

    QLabel *v_speed_label;
    QSlider *v_speed_slider;

    QLabel *f_speed_label;
    QSlider *f_speed_slider;

private slots:
    void upd_roll(int);
    void upd_pitch(int);
    void upd_v_speed(int);
    void upd_f_speed(int);
};

#endif // TEST_WIDGET_H
