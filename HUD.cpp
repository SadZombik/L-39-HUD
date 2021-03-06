#include "HUD.h"
#include <QDebug>

HUD::HUD(const quint16 width, const quint16 height, bool transparent, QWidget *parent)
    : QWidget(parent), planeParams(client.displayPacket), w(width), h(height)
{
    if (transparent)
    {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    resize(w, h);
    rescale();

    planeParams.hud_mode = Modes::VIZIER;

    pix.load(":/img/Vizier.png");

    QTimer *timer = new QTimer(this);
    timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    // TODO: удалить после настройки
    connect(timer, SIGNAL(timeout()), this, SLOT(update_params()));

    test.move(1650, (1080-test.height())/2);
    test.show();
}

// TODO: удалить после настройки
void HUD::update_params()
{
    // planeParams.hud_mode = test.getMode();
    planeParams.roll = test.getRoll();
    planeParams.pitch = test.getPitch();
    planeParams.yaw = test.getYaw();
    planeParams.vertical_speed = test.getVertSpeed();
    planeParams.forward_speed = test.getForwSpeed();
    planeParams.bar_height = test.getHeight();
    planeParams.radio_height = test.getRHeight();
    planeParams.launch_signal = true;
    planeParams.waypoint_dist = 0;
    planeParams.range = test.getTargetDist();
    planeParams.max_range = 4;
    planeParams.min_range = 3;

}

void HUD::rescale()
{
    w = width();
    h = height();

    // Конфиг
    QSettings *ini = new QSettings("hud_config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    {
        // Настройка сетки
        line_width = ini->value("line_width").toFloat();
        line_color = QColor(ini->value("color").toString());
        top_level = h * ini->value("h_level_coef").toFloat();
        bottom_level = h - h * ini->value("h_level_coef").toFloat();
        left_level = w * ini->value("w_level_coef").toFloat();
        right_level = w - w * ini->value("w_level_coef").toFloat();
        label_offset = ini->value("label_offset").toFloat();
        use_mesh = ini->value("mesh").toBool();

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

        // Настройки шкалы дальности
        range_offset = ini->value("range_offset").toFloat();

        // Настройки неподвижной сетки визира
        viz_offset = ini->value("viz_offset").toFloat();
    }
    ini->endGroup();

    ini->beginGroup("FONTS");
    {
        yaw_font_size = ini->value("yaw_font_size").toInt();
        pitch_font_size = ini->value("pitch_font_size").toInt();
        vert_speed_font_size = ini->value("vert_speed_font_size").toInt();
        ias_font_size = ini->value("ias_font_size").toInt();
        height_font_size = ini->value("height_font_size").toInt();
        radio_height_font_size = ini->value("radio_height_font_size").toInt();
        mode_font_size = ini->value("mode_font_size").toInt();
        range_font_size = ini->value("range_font_size").toInt();
        launch_font_size = ini->value("launch_font_size").toInt();
        range_scale_font_size = ini->value("range_scale_font_size").toInt();
    }
    ini->endGroup();

    ini->beginGroup("NETWORK");
    {
        client.set_address(QHostAddress(ini->value("IP").toString()));
        client.set_port(ini->value("PORT").toInt());
    }
    ini->endGroup();
}

void HUD::paintEvent(QPaintEvent *event)
{
    // TODO:
    rescale();

    QPen pen;
    pen.setWidthF(line_width);
    pen.setColor(line_color);
    // вынести в конструктор после настройки

    Q_UNUSED(event);
    QPainter qp(this);
    qp.setPen(pen);
    qp.translate(w/2, h/2);  // Перенос 0,0 в центр экрана

    switch(planeParams.hud_mode)
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
}

void HUD::mesh(QPainter *qp)
{
    if (use_mesh)
    {
        qp->save();
        {
            QPen ipen;
            ipen.setStyle(Qt::DashLine);
            ipen.setColor(line_color);
            qp->setPen(ipen);

            qp->translate(-w/2, -h/2);
            qp->drawLine(0, top_level, w, top_level);
            qp->drawLine(0, bottom_level, w, bottom_level);
            qp->drawLine(left_level, 0, left_level, h);
            qp->drawLine(right_level, 0, right_level, h);
        }
        qp->restore();
    }
}

void HUD::draw(QPainter *qp, QString mode, bool radio)
{
    mesh(qp);
    scope(qp);
    roll_indicator(qp);
    pitch_indicator(qp);
    course_incdicator(qp);
    ias(qp);
    height_indicator(qp, radio);
    mode_indicator(qp, mode);
}

void HUD::draw_nav(QPainter *qp)
{    
    draw(qp, "NAV", true);

    vert_speed_indicator(qp);
    waypoint_indicator(qp);
}

void HUD::draw_app(QPainter *qp)
{
    draw(qp, "APP", true);

    vert_speed_indicator(qp);
    waypoint_indicator(qp);
    glide_mark(qp);
    glide_symbols(qp);
}

void HUD::draw_bomb(QPainter *qp)
{
    draw(qp, "GND", false);

    bomb_mark(qp);
    launch_signal(qp);
    weapon_switch(qp, "B");
}

void HUD::draw_rocket(QPainter *qp)
{
    draw(qp, "GND", false);

    viz_mark(qp);
    launch_signal(qp);
    weapon_switch(qp, "S5M");
    range_indicator(qp);
}

void HUD::draw_gun(QPainter *qp)
{
    draw(qp, "GUN", false);

    viz_mark(qp);
    launch_signal(qp);
    range_indicator(qp);
}

void HUD::draw_air(QPainter *qp)
{
    draw(qp, "LNGT", false);

    launch_signal(qp);
    weapon_switch(qp, "R-3");
}

void HUD::draw_vizier(QPainter *qp)
{
    mesh(qp);
    viz_mesh(qp);
}

void HUD::scope(QPainter *qp)
{
    qp->save();
    {
        QLineF scope_lines[4] =
        {
            QLineF(-scope_size, 0, -scope_space, 0),
            QLineF(scope_size, 0, scope_space, 0),
            QLineF(0, -scope_size, 0, -scope_space),
            QLineF(0, scope_size, 0, scope_space)
        };
        qp->drawLines(scope_lines, 4);
    }
    qp->restore();
}

void HUD::roll_indicator(QPainter *qp)
{
    qp->save();
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

        qp->drawLines(roll_lines, 6);

        for (int i = 0; i < 6; i++)
        {
            qp->drawLine(
                -roll_lines[i].x1(),
                 roll_lines[i].y1(),
                -roll_lines[i].x2(),
                 roll_lines[i].y2()
            );
        }

        // Линиия крена
        qp->rotate(planeParams.roll);

        float s_len = scope_size + 2*scope_space;   // Отступ от прицела

        // Сама линия
        qp->drawLine(-r+dr/2, 0, -s_len, 0);
        qp->drawLine(s_len, 0, r-dr/2, 0);

        // Хвост
        qp->drawLine(0, -s_len, 0, -1.5*s_len);

        // Турбины
        qp->drawLine(-s_len-turbine_offset, 0, -s_len-turbine_offset, s_len/2);
        qp->drawLine(s_len+turbine_offset, 0, s_len+turbine_offset, s_len/2);
    }
    qp->restore();
}

void HUD::pitch_indicator(QPainter *qp)
{
    qp->save();
    {
        QFont p_font("Helvetica", pitch_font_size);

        float pitch = planeParams.pitch;
        float line_pos = pitch * roll_outer_radius / 90;
        line_pos = pitch > 90 ? roll_outer_radius : line_pos;
        line_pos = pitch < -90 ? -roll_outer_radius : line_pos;
        qp->setFont(p_font);
        qp->drawLine(-pitch_line_len/2, line_pos, pitch_line_len/2, line_pos);
        qp->drawText(pitch_line_len/2 - pitch_font_size, line_pos - label_offset, QString::number(pitch));
    }
    qp->restore();
}

void HUD::vert_speed_indicator(QPainter *qp)
{
    qp->save();
    {
        QFont vs_font("Helvetica", vert_speed_font_size);
        qp->setFont(vs_font);
        qp->translate(scale_offset, 0);

        float v_speed = planeParams.vertical_speed;
        float arrow_pos = -v_speed * vert_speed_line_len/2 / 30;
        arrow_pos = v_speed > 30 ? -vert_speed_line_len/2 : arrow_pos;
        arrow_pos = v_speed < -30 ? vert_speed_line_len/2 : arrow_pos;

        // Шкала
        qp->drawLine(0, -vert_speed_line_len/2, 0, vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, 0, 0, 0);
        qp->drawLine(-vert_lines_width, -vert_speed_line_len/2, 0, -vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, vert_speed_line_len/2, 0, vert_speed_line_len/2);

        // Стрелка
        qp->drawText(vert_lines_width*2.5, arrow_pos - label_offset, QString::number(v_speed));
        qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 - корень из 2 / 2
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);
    }
    qp->restore();
}

void HUD::ias(QPainter *qp)
{
    float speed = planeParams.forward_speed;

    qp->save();
    {
        QFont ias_font("Helvetica", ias_font_size);
        qp->setFont(ias_font);
        qp->translate(-w/2, -h/2);


        // Индикатор
        qp->drawLine(left_level-ias_line_len/2, top_level, left_level+ias_line_len/2, top_level);

        QRectF ias_rect(
            left_level-ias_line_len/2,
            top_level - label_offset - ias_font_size,
            ias_line_len,
            ias_font_size
        );
        qp->drawText(ias_rect, Qt::AlignCenter, QString::number(speed));

        // Тенденция ускорения (треугольник)
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

        qp->drawLine(t1[0], t1[1]);
        qp->drawLine(t1[1], t1[2]);
        qp->drawLine(t1[2], t1[0]);
    }
    qp->restore();

    prev_speed = speed;
}

void HUD::course_incdicator(QPainter *qp)
{
    qp->save();
    {
        QFont yaw_font("Helvetica", yaw_font_size);
        qp->setFont(yaw_font);
        qp->translate(0, -h/2);

        // Шкала
        qp->drawLine(-yaw_line_len/2, top_level, yaw_line_len/2, top_level);

        for (int i = 0; i < 360; i++)
        {
            float x = yaw_scale*(i - planeParams.yaw) ;
            if (x < yaw_line_len/2 && x > -yaw_line_len/2)
            {
                QRectF yaw_rect(
                    x - yaw_font_size*1.5,
                    top_level - label_offset - yaw_font_size - vert_lines_height,
                    yaw_font_size*3,
                    yaw_font_size
                );
                qp->drawText(yaw_rect, Qt::AlignCenter, QString::number(i));
//                c_qp->drawRect(yaw_rect); // debug
                qp->drawLine(x, top_level, x, top_level-vert_lines_height);
                qp->drawLine(x+yaw_scale/2, top_level, x+yaw_scale/2, top_level-vert_lines_height/2);
            }
        }

        // Треугольник
        float a = ias_font_size/2;
        float h = qSqrt(3) * a /2;
        t2[0] = QPointF(0, top_level);
        t2[1] = QPointF(-a/2, top_level+h);
        t2[2] = QPointF(a/2, top_level+h);
        qp->drawLine(t2[0], t2[1]);
        qp->drawLine(t2[1], t2[2]);
        qp->drawLine(t2[2], t2[0]);
    }
    qp->restore();
}

void HUD::height_indicator(QPainter *qp, bool radio)
{
    qp->save();
    {
        QFont h_font("Helvetica", height_font_size);
        QFont r_font("Helvetica", radio_height_font_size);
        qp->setFont(h_font);
        qp->translate(-w/2, -h/2);

        qp->drawText(QRectF(
            right_level - height_font_size*2,
            top_level - height_font_size - label_offset,
            height_font_size*4,
            height_font_size
        ), Qt::AlignCenter, QString::number(planeParams.bar_height));

        if (radio)
        {
            qp->setFont(r_font);
            qp->drawText(QRectF(
                right_level - height_font_size*2+radio_height_font_size,
                top_level+label_offset,
                radio_height_font_size,
                radio_height_font_size+label_offset
            ), Qt::AlignCenter, "R");

            QRectF r_rect(
                right_level - radio_height_font_size,
                top_level + label_offset,
                height_font_size*3-radio_height_font_size,
                radio_height_font_size+label_offset
            );

            qp->drawRect(r_rect);
            qp->drawText(r_rect, Qt::AlignCenter, QString::number(planeParams.radio_height));
        }
    }
    qp->restore();
}

void HUD::mode_indicator(QPainter *qp, QString mode)
{
    qp->save();
    {
        QFont m_font("Helvetica", mode_font_size);
        qp->setFont(m_font);
        qp->translate(-w/2, -h/2);

        qp->drawText(QRectF(
            left_level-mode_font_size*1.5,
            bottom_level-label_offset-mode_font_size,
            mode_font_size*3,
            mode_font_size
        ), Qt::AlignCenter, mode);
    }
    qp->restore();
}

void HUD::waypoint_indicator(QPainter *qp)
{
    qp->save();
    {
        QFont rn_font("Helvetica", range_font_size);
        qp->setFont(rn_font);
        qp->translate(0, -h/2);

        qp->drawText(QRectF(
            -range_font_size*2,
            bottom_level+label_offset,
            range_font_size*4,
            range_font_size
        ), Qt::AlignCenter, QString::number(planeParams.waypoint_dist));
    }
    qp->restore();
}

void HUD::glide_mark(QPainter *qp)
{

}

void HUD::glide_symbols(QPainter *qp)
{

}

void HUD::bomb_mark(QPainter *qp)
{

}

void HUD::launch_signal(QPainter *qp)
{
    if (planeParams.launch_signal)
    {
        QFont g_font("Helvetica", launch_font_size);

        qp->save();
        {
            qp->setFont(g_font);
            qp->translate(0, -h/2);
            qp->drawText(QRectF(
               -launch_font_size,
               bottom_level-2*mode_font_size-label_offset,
               launch_font_size*2,
               launch_font_size
            ), Qt::AlignCenter, "LA");
        }
        qp->restore();
    }
}

void HUD::range_indicator(QPainter *qp)
{
    qp->save();
    {
        QFont rs_font("Helvetica", range_scale_font_size);
        qp->setFont(rs_font);

        qp->translate(-range_offset, 0);
        range_line_len = roll_outer_radius; // ?

        // Шкала
        qp->drawLine(0, -range_line_len, 0, range_line_len);
        for (int i = 0; i < 6; i++)
        {
            float y = -range_line_len + range_line_len/2.5 * i;
            qp->drawLine(-vert_lines_width, y, 0, y);
        }
        qp->drawText(-vert_lines_width-range_scale_font_size*1.5, -range_line_len+range_scale_font_size/2, "5");

        // Стрелка
        float range = planeParams.range ;
        float arrow_pos = (range > 5) ? (-range_line_len) : range_line_len - (range * range_line_len*2) / 5;

        qp->drawLine(0, arrow_pos, vert_lines_width*2, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 - корень из 2 / 2
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);

        // max/min
        QPen pen;
        pen.setWidthF(line_width*3);
        qp->setPen(pen);

        float max = range_line_len - (planeParams.max_range * range_line_len*2) / 5;
        float min = range_line_len - (planeParams.min_range * range_line_len*2) / 5;

        qp->drawLine(0, max, vert_lines_width, max);
        qp->drawLine(0, min, vert_lines_width, min);
    }
    qp->restore();
}

void HUD::weapon_switch(QPainter *qp, QString str)
{
    qp->save();
    {
        QFont w_font("Helvetica", launch_font_size);
        qp->setFont(w_font);
        qp->translate(-w/2, -h/2);

        qp->drawText(QRectF(
            right_level-height_font_size*1.5,
            bottom_level-label_offset-launch_font_size,
            launch_font_size*3,
            launch_font_size
        ), Qt::AlignCenter, str);
    }
    qp->restore();
}

void HUD::viz_mark(QPainter *qp)
{

}

void HUD::viz_mesh(QPainter *qp)
{
    // Статичная сетка визирного прицела
    qp->drawPixmap(-w/2-viz_offset, -h/2, w, h, pix);
}
