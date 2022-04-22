#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QLabel>

class test_widget: public QWidget
{

    Q_OBJECT

public:
    test_widget(QWidget *parent = nullptr);
    float getRoll() { return roll_slider->value(); }

private:
    QLabel *roll_label;
    QLabel *pitch_label;
    QSlider *roll_slider;
    QSlider *pitch_slider;

private slots:
    void upd_roll(int);
    void upd_pitch(int);
};

#endif // TEST_WIDGET_H
