#include "test_widget.h"

test_widget::test_widget(QWidget *parent): QWidget(parent)
{
    layout = new QVBoxLayout(this);

    mode_label = new QLabel("Режим", this);
    mode = new QComboBox(this);
    mode->addItem("NAV");
    mode->addItem("APP");
    mode->addItem("BOMB");
    mode->addItem("ROCKET");
    mode->addItem("GUN");
    mode->addItem("AIR");
    mode->addItem("VIZ");

    roll_label = new QLabel("Roll", this);
    roll_slider = new QSlider(Qt::Horizontal, this);

    pitch_label = new QLabel("Pitch", this);
    pitch_slider = new QSlider(Qt::Vertical, this);

    v_speed_label = new QLabel("Vert Speed", this);
    v_speed_slider = new QSlider(Qt::Horizontal, this);

    f_speed_label = new QLabel("IAS", this);
    f_speed_slider = new QSlider(Qt::Horizontal, this);

    yaw_label = new QLabel("Yaw", this);
    yaw_slider = new QDoubleSpinBox();

    height_label = new QLabel("Height", this);
    height_slider = new QSlider(Qt::Horizontal, this);

    r_height_label = new QLabel("Radio Height", this);
    r_height_slider = new QSlider(Qt::Horizontal, this);

    yaw_slider->setMinimum(0);
    yaw_slider->setMaximum(360);
    yaw_slider->setSingleStep(0.3);
    roll_slider->setMinimum(-200);
    roll_slider->setMaximum(200);
    pitch_slider->setMinimum(-180);
    pitch_slider->setMaximum(180);

    v_speed_slider->setMinimum(-50);
    v_speed_slider->setMaximum(50);
    f_speed_slider->setMinimum(0);
    f_speed_slider->setMaximum(1000);
    height_slider->setMinimum(0);
    height_slider->setMaximum(5000);
    r_height_slider->setMinimum(0);
    r_height_slider->setMaximum(5000);

    layout->addWidget(mode_label);
    layout->addWidget(mode);

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
    layout->addWidget(height_label);
    layout->addWidget(height_slider);
    layout->addWidget(r_height_label);
    layout->addWidget(r_height_slider);


    setLayout(layout);

    connect(mode, SIGNAL(activated(int)), this, SLOT(select_mode(int)));
    connect(roll_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_roll(int)));
    connect(pitch_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_pitch(int)));
    connect(v_speed_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_v_speed(int)));
    connect(f_speed_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_f_speed(int)));
    connect(height_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_height(int)));
    connect(r_height_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_r_height(int)));
}

void test_widget::select_mode(int i)
{
    mode_label->setText(QString::number(i));
    index = i;
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

void test_widget::upd_height(int value)
{
    height_label->setText("Height " + QString::number(value));
}

void test_widget::upd_r_height(int value)
{
    r_height_label->setText("Radio Height " + QString::number(value));
}
