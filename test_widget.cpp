#include "test_widget.h"

test_widget::test_widget(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);;
    roll_slider = new QSlider(Qt::Horizontal, this);
    pitch_slider = new QSlider(this);
    roll_label = new QLabel("Roll", this);
    pitch_label = new QLabel("Pitch", this);

    roll_slider->setMinimum(-33);
    roll_slider->setMaximum(33);
    pitch_slider->setMinimum(-33);
    pitch_slider->setMaximum(33);

    layout->addWidget(roll_label);
    layout->addWidget(roll_slider);
    layout->addWidget(pitch_label);
    layout->addWidget(pitch_slider);

    setLayout(layout);

    connect(roll_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_roll(int)));
    connect(pitch_slider, SIGNAL(valueChanged(int)), this, SLOT(upd_pitch(int)));
}

void test_widget::upd_roll(int value)
{
    roll_label->setText("Roll " + QString::number(value));
}

void test_widget::upd_pitch(int value)
{
    pitch_label->setText("Pitch " + QString::number(value));
}
