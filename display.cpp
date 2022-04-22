#include "display.h"

Display::Display(const quint16 width, const quint16 height, QWidget *parent)
    : QWidget(parent), w(width), h(height), planeParams(client.displayPacket)
{
    rescale();
    resize(w, h);
    mode = Modes::NAV;

    QTimer *timer = new QTimer(this);
    timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update_params()));
    t.show();
}

void Display::update_params()
{
    planeParams.roll = t.getRoll();
}

void Display::rescale()
{
    // Конфиг
    QSettings *ini = new QSettings("config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    {
        // Настройки прицела
        scope_size = h * ini->value("scope_size_coef").toFloat();
        scope_space = scope_size * ini->value("scope_space_coef").toFloat();

        // Настройки шкалы крена
        roll_outer_radius = h * ini->value("roll_radius_coef").toFloat();
        roll_inner_radius = roll_outer_radius * ini->value("roll_radius_diff_coef").toFloat();
        radius_diff = roll_outer_radius - roll_inner_radius;
    }
    ini->endGroup();

    ini->beginGroup("NETWORK");
    {
        client.set_address(QHostAddress(ini->value("IP").toString()));
        client.set_port(ini->value("PORT").toInt());
    }
    ini->endGroup();
}

void Display::paintEvent(QPaintEvent *event)
{
    w = width();
    h = height();
    rescale();

    Q_UNUSED(event);
    QPainter qp(this);

    switch(mode)
    {
    case NAV:
        draw_nav(&qp);
        break;
    case APP:
        draw_app(&qp);
        break;
    case BOMB:
        draw_bomb(&qp);
        break;
    case ROCKET:
        draw_rocket(&qp);
        break;
    case GUN:
        draw_gun(&qp);
        break;
    case AIR:
        draw_air(&qp);
        break;
    case VIZIER:
        draw_vizier(&qp);
        break;
    default:
        break;
    }

    //draw_x(&qp);
}

void Display::draw_x(QPainter *qp)
{
    qp->drawLine(0, 0, width(), height());

}

void Display::draw_nav(QPainter *qp)
{
    quint16 wh = w/2;       // Половина ширины
    quint16 hh = h/2;       // Половина высоты

    qp->translate(wh, hh);  // Перенос 0,0 в центр экрана

    // Неподвижынй прицел
    scope(qp);

    // Шкала крена
    roll_scale(qp, planeParams.roll);









}

void Display::draw_app(QPainter *qp)
{

}

void Display::draw_bomb(QPainter *qp)
{

}
void Display::draw_rocket(QPainter *qp)
{

}
void Display::draw_gun(QPainter *qp)
{

}
void Display::draw_air(QPainter *qp)
{

}
void Display::draw_vizier(QPainter *qp)
{

}

void Display::scope(QPainter *s_qp)
{
    QLineF scope_lines[4] =
    {
        QLineF(0 - scope_size, 0, 0 - scope_space, 0),
        QLineF(0 + scope_size, 0, 0 + scope_space, 0),
        QLineF(0, 0 - scope_size, 0, 0 - scope_space),
        QLineF(0, 0 + scope_size, 0, 0 + scope_space)
    };
    s_qp->drawLines(scope_lines, 4);
}

void Display::roll_scale(QPainter *r_qp, float roll)
{
    float R = roll_outer_radius;
    float r = roll_inner_radius;

    float angle = -M_PI_2;
    float deg_5 = M_PI/36;

    // Шкала
    QLineF roll_lines[6] = {
        QLineF(                         // 0
            r*qSin(angle),
            r*qCos(angle),
            R*qSin(angle),
            R*qCos(angle)
        ),
        QLineF(                         // 5
            r*qSin(angle+deg_5),
            r*qCos(angle+deg_5),
            (R-radius_diff/2)*qSin(angle+deg_5),
            (R-radius_diff/2)*qCos(angle+deg_5)
        ),
        QLineF(                         // 10
            r*qSin(angle+deg_5*2),
            r*qCos(angle+deg_5*2),
            (R-radius_diff/2)*qSin(angle+deg_5*2),
            (R-radius_diff/2)*qCos(angle+deg_5*2)
        ),
        QLineF(                         // 15
            r*qSin(angle+deg_5*3),
            r*qCos(angle+deg_5*3),
            R*qSin(angle+deg_5*3),
            R*qCos(angle+deg_5*3)
        ),
        QLineF(                         // 30
            r*qSin(angle+deg_5*6),
            r*qCos(angle+deg_5*6),
            R*qSin(angle+deg_5*6),
            R*qCos(angle+deg_5*6)
        ),
        QLineF(                         // 60
            r*qSin(angle+deg_5*12),
            r*qCos(angle+deg_5*12),
            R*qSin(angle+deg_5*12),
            R*qCos(angle+deg_5*12)
        )
    };

    r_qp->drawLines(roll_lines, 6);

    for (int i = 0; i < 6; i++)
    {
        r_qp->drawLine(
            -roll_lines[i].x1(),
             roll_lines[i].y1(),
            -roll_lines[i].x2(),
             roll_lines[i].y2()
        );
    }

    // Линиия крена
    r_qp->rotate(roll);

    float s_len = scope_size + scope_space;
    r_qp->drawLine(-r+radius_diff/2, 0, -s_len, 0);
    r_qp->drawLine(s_len, 0, r-radius_diff/2, 0);
    r_qp->drawLine(0, -s_len, 0, -s_len-radius_diff/2);

}


void Display::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

