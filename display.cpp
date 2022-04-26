#include "display.h"
#include <QDebug>

Display::Display(const quint16 width, const quint16 height, QWidget *parent)
    : QWidget(parent), w(width), h(height), planeParams(client.displayPacket)
{
    resize(w, h);
    rescale();
    mode = Modes::NAV;

    QTimer *timer = new QTimer(this);
    timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    // TODO: удалить после настройки
    connect(timer, SIGNAL(timeout()), this, SLOT(update_params()));
    test.show();
}

// TODO: удалить после настройки
void Display::update_params()
{
    planeParams.roll = test.getRoll();
    planeParams.pitch = test.getPitch();
    planeParams.yaw = test.getYaw();
    planeParams.vertical_speed = test.getVertSpeed();
    planeParams.forward_speed = test.getForwSpeed();
}

void Display::rescale()
{
    w = width();
    h = height();

    // Конфиг
    QSettings *ini = new QSettings("config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    {
        // Настройка сетки
        line_width = ini->value("line_width").toInt();
        line_color = QColor(ini->value("color").toString());
        top_level = h * ini->value("h_level_coef").toFloat();
        bottom_level = h - h * ini->value("h_level_coef").toFloat();
        left_level = w * ini->value("w_level_coef").toFloat();
        right_level = w - w * ini->value("w_level_coef").toFloat();
        label_offset = ini->value("label_offset").toFloat();

        // Настройки прицела
        scope_size = h * ini->value("scope_size_coef").toFloat();
        scope_space = scope_size * ini->value("scope_space_coef").toFloat();
        turbine_offset = ini->value("turbine_offset").toFloat();

        // Настройки шкалы крена
        roll_outer_radius = h * ini->value("roll_radius_coef").toFloat();
        roll_inner_radius = roll_outer_radius * ini->value("roll_radius_diff_coef").toFloat();

        // Настройки линии горизонта
        pitch_line_len = w * ini->value("pitch_width_coef").toFloat();

        // Настройки шкалы курса
        yaw_line_len = w * ini->value("yaw_width_coef").toFloat();
        yaw_scale = w * ini->value("yaw_scale").toFloat();
        vert_lines_height = yaw_font_size / 2;

        // Настройки шкалы вертикальной скорости
        vert_speed_line_len = h * ini->value("vert_speed_height_coef").toFloat();
        scale_offset = ini->value("scale_offset").toFloat();
        vert_lines_width = vert_speed_line_len * ini->value("vert_lines_width_coef").toFloat();

        // Настройки индикатора приборной воздушной скорости
        ias_line_len = w * ini->value("ias_width_coef").toFloat();
    }
    ini->endGroup();

    ini->beginGroup("FONTS");
    {
        yaw_font_size = ini->value("yaw_font_size").toInt();
        pitch_font_size = ini->value("pitch_font_size").toInt();
        vert_speed_font_size = ini->value("vert_speed_font_size").toInt();
        ias_font_size = ini->value("ias_font_size").toInt();
        height_font_size = ini->value("")
                radio_height_font_size
    }
    ini->endGroup();

    ini->beginGroup("NETWORK");
    {
        client.set_address(QHostAddress(ini->value("IP").toString()));
        client.set_port(ini->value("PORT").toInt());
    }
    ini->endGroup();

    delete ini;
}

void Display::paintEvent(QPaintEvent *event)
{
    // TODO: вынести в конструктор после настройки
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

void Display::mesh(QPainter *qp)
{
    QPen ipen;
    ipen.setStyle(Qt::DashLine);

    qp->save();
    {
        qp->setPen(ipen);
        qp->translate(-w/2, -h/2);

        qp->drawLine(0, top_level, w, top_level);
        qp->drawLine(0, bottom_level, w, bottom_level);

        qp->drawLine(left_level, 0, left_level, h);
        qp->drawLine(right_level, 0, right_level, h);
    }
    qp->restore();
}

void Display::draw_nav(QPainter *qp)
{
    // TODO:
    QPen pen;
    pen.setWidth(line_width);
    pen.setColor(line_color);
    // вынести в конструктор после настройки

    qp->setPen(pen);
    qp->translate(w/2, h/2);  // Перенос 0,0 в центр экрана

    // Сетка для позиционирования индикаторов
    mesh(qp);

    // Неподвижынй прицел
    scope(qp);

    // Шкала крена
    roll_indicator(qp, planeParams.roll);

    // Линия горизнта
    pitch_indicator(qp, planeParams.pitch);

    // Шкала курса
    course(qp, planeParams.yaw);

    // Шкала верткальной скорости
    vert_speed_indicator(qp, planeParams.vertical_speed);

    // Приборная воздушная скорость
    ias(qp, planeParams.forward_speed);
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

void Display::roll_indicator(QPainter *r_qp, float roll)
{
    r_qp->save();
    {
        float R = roll_outer_radius;
        float r = roll_inner_radius;
        float dr = R - r;

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
                (R-dr/2)*qSin(angle+deg_5),
                (R-dr/2)*qCos(angle+deg_5)
            ),
            QLineF(                         // 10
                r*qSin(angle+deg_5*2),
                r*qCos(angle+deg_5*2),
                (R-dr/2)*qSin(angle+deg_5*2),
                (R-dr/2)*qCos(angle+deg_5*2)
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

        float s_len = scope_size + 2*scope_space;   // Отступ от прицела

        // Сама линия
        r_qp->drawLine(-r+dr/2, 0, -s_len, 0);
        r_qp->drawLine(s_len, 0, r-dr/2, 0);

        // Хвост
        r_qp->drawLine(0, -s_len, 0, -1.5*s_len);

        // Турбины
        r_qp->drawLine(-s_len-turbine_offset, 0, -s_len-turbine_offset, s_len/2);
        r_qp->drawLine(s_len+turbine_offset, 0, s_len+turbine_offset, s_len/2);
    }
    r_qp->restore();
}

void Display::pitch_indicator(QPainter *p_qp, float pitch)
{
    QFont p_font("Helvetica", pitch_font_size);

    float line_pos = pitch * roll_outer_radius / 90;
    line_pos = pitch > 90 ? roll_outer_radius : line_pos;
    line_pos = pitch < -90 ? -roll_outer_radius : line_pos;

    p_qp->save();
    {
        p_qp->setFont(p_font);
        p_qp->drawLine(-pitch_line_len/2, line_pos, pitch_line_len/2, line_pos);
        p_qp->drawText(pitch_line_len/2 - pitch_font_size, line_pos - label_offset, QString::number(pitch));
    }
    p_qp->restore();
}

void Display::vert_speed_indicator(QPainter *vs_qp, float v_speed)
{
    QFont vs_font("Helvetica", vert_speed_font_size);

    float arrow_pos = -v_speed * vert_speed_line_len/2 / 30;
    arrow_pos = v_speed > 30 ? -vert_speed_line_len/2 : arrow_pos;
    arrow_pos = v_speed < -30 ? vert_speed_line_len/2 : arrow_pos;

    vs_qp->save();
    {
        vs_qp->setFont(vs_font);
        vs_qp->translate(scale_offset, 0);

        // Шкала
        vs_qp->drawLine(0, -vert_speed_line_len/2, 0, vert_speed_line_len/2);
        vs_qp->drawLine(-vert_lines_width, 0, 0, 0);
        vs_qp->drawLine(-vert_lines_width, -vert_speed_line_len/2, 0, -vert_speed_line_len/2);
        vs_qp->drawLine(-vert_lines_width, vert_speed_line_len/2, 0, vert_speed_line_len/2);

        // Стрелка
        vs_qp->drawText(vert_lines_width*2.5, arrow_pos - label_offset, QString::number(v_speed));
        vs_qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        vs_qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 - корень из 2 / 2
        vs_qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);
    }
    vs_qp->restore();
}

void Display::ias(QPainter *i_qp, float speed)
{
    QFont ias_font("Helvetica", ias_font_size);
    i_qp->save();
    {
        i_qp->translate(-w/2, -h/2);

        // Индикатор
        i_qp->setFont(ias_font);
        i_qp->drawLine(left_level-ias_line_len/2, top_level, left_level+ias_line_len/2, top_level);

        QRectF ias_rect(
            left_level-ias_line_len/2,
            top_level - label_offset - ias_font_size,
            ias_line_len,
            ias_font_size
        );
        i_qp->drawText(ias_rect, Qt::AlignCenter, QString::number(speed));

        // Тенденция ускорения
        float a = ias_font_size/2;
        float h = qSqrt(3) * a /2;

        if (speed > prev_speed)
        {
            t1[0] = QPointF(left_level, top_level);
            t1[1] = QPointF(left_level-a/2, top_level+h);
            t1[2] = QPointF(left_level+a/2, top_level+h);
        }
        else if (speed < prev_speed)
        {
            t1[0] = QPointF(left_level - a/2, top_level);
            t1[1] = QPointF(left_level + a/2, top_level);
            t1[2] = QPointF(left_level, top_level + h);
         }

        i_qp->drawLine(t1[0], t1[1]);
        i_qp->drawLine(t1[1], t1[2]);
        i_qp->drawLine(t1[2], t1[0]);
    }
    i_qp->restore();

    prev_speed = speed;
}

void Display::course(QPainter *c_qp, float yaw)
{
    QFont yaw_font("Helvetica", yaw_font_size);



    c_qp->save();
    {
        c_qp->setFont(yaw_font);
        c_qp->translate(0, -h/2);

        // Шкала
        c_qp->drawLine(-yaw_line_len/2, top_level, yaw_line_len/2, top_level);

        for (int i = 0; i < 360; i++)
        {
            float x = yaw_scale*(i - yaw) ;
            if (x < yaw_line_len/2 && x > -yaw_line_len/2)
            {
                QRectF yaw_rect(
                    x - yaw_font_size*1.5,
                    top_level - label_offset - yaw_font_size - vert_lines_height,
                    yaw_font_size*3,
                    yaw_font_size
                );
                c_qp->drawText(yaw_rect, Qt::AlignCenter, QString::number(i));
//                c_qp->drawRect(yaw_rect); // debug
                c_qp->drawLine(x, top_level, x, top_level-vert_lines_height);
                c_qp->drawLine(x+yaw_scale/2, top_level, x+yaw_scale/2, top_level-vert_lines_height/2);
            }


        }
        // Треугольник
        float a = ias_font_size/2;
        float h = qSqrt(3) * a /2;
        t2[0] = QPointF(0, top_level);
        t2[1] = QPointF(-a/2, top_level+h);
        t2[2] = QPointF(a/2, top_level+h);
        c_qp->drawLine(t2[0], t2[1]);
        c_qp->drawLine(t2[1], t2[2]);
        c_qp->drawLine(t2[2], t2[0]);
    }
    c_qp->restore();
}

void Display::flight_height(QPainter *h_qp, float height, float r_height)
{

}

void Display::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

