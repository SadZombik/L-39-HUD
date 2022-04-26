#include "test_widget.h"

test_widget::test_widget(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);;
    roll_label = new QLabel("Roll", this);
    roll_slider = new QSlider(Qt::Horizontal, this);

    pitch_label = new QLabel("Pitch", this);
    pitch_slider = new QSlider(this);

    v_speed_label = new QLabel("Vert Speed", this);
    v_speed_slider = new QSlider(Qt::Horizontal, this);

    f_speed_label = new QLabel("IAS", this);
    f_speed_slider = new QSlider(Qt::Horizontal, this);

    yaw_label = new QLabel("Yaw", this);
    yaw_slider = new QDoubleSpinBox();

    roll_slider->setMinimum(-200);
    roll_slider->setMaximum(200);
    pitch_slider->setMinimum(-180);
    pitch_slider->setMaximum(180);
    yaw_slider->setMinimum(0);
    yaw_slider->setMaximum(360);
    yaw_slider->setSingleStep(0.3);

    v_speed_slider->setMinimum(-50);
    v_speed_slider->setMaximum(50);
    f_speed_slider->setMinimum(0);
    f_speed_slider->setMaximum(1000);

    layout->addWidget(roll_label);
    layout->addWidget(roll_slider);
    layout->addWidget(pitch_label);
    layout->addWidget(pitch_slider);
    layout->addWidget(yaw_label);
    layout->addWidget(yaw_slider);

    layout->addWidget(v_speed_label);
    layout->addWidget(v_speed_slider);
    layout->addWidget(f_speed_label);
    layout->addWidget(f_speed_slider);

    setLayout(layout);

    connect(roll_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_roll(int)));
    connect(pitch_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_pitch(int)));
    connect(v_speed_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_v_speed(int)));
    connect(f_speed_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_f_speed(int)));
}

void test_widget::upd_roll(int value)
{
    roll_label->setText("Roll " + QString::number(value));
}

void test_widget::upd_pitch(int value)
{
    pitch_label->setText("Pitch " + QString::number(value));
}

void test_widget::upd_v_speed(int value)
{
    v_speed_label->setText("Vert Speed " + QString::number(value));
}

void test_widget::upd_f_speed(int value)
{
    f_speed_label->setText("IAS " + QString::number(value));
}
