#include "HUD.h"
#include <QDebug>

HUD::HUD(QWidget *parent): QWidget(parent)
{
    setStyleSheet("background-color: black");
    line_color = QColor(0, 0, 0);
    read_config();

    pix.load("Vizier.png");

    QTimer *timer = new QTimer(this);
    timer->start(33);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

HUD::~HUD()
{

}

void HUD::read_config()
{
    ini = new QSettings("hud_config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    {
        // Полноэкранный режим
        w = width();
        h = height();

        w_scale = ini->value("w_scale").toFloat();
        h_scale = ini->value("h_scale").toFloat();

        // Настройка сетки и линий
        line_width = ini->value("line_width").toFloat();
        line_color.setGreen(ini->value("color").toInt());
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
        vert_scale_offset = ini->value("scale_offset").toFloat();
        vert_lines_width = vert_speed_line_len * ini->value("vert_lines_width_coef").toFloat();

        // Настройки индикатора приборной воздушной скорости
        ias_line_len = w * ini->value("ias_width_coef").toFloat();
        ias_a_coef = ini->value("ias_acceleration_coef").toFloat();

        // Настройки шкалы дальности
        range_offset = ini->value("range_offset").toFloat();

        // Настройки визира
        viz_coef = ini->value("viz_coef").toFloat();
        viz_offset = ini->value("viz_offset").toFloat();

        // Настройка размеров виджета
        debug = ini->value("debug").toBool();

        if (debug)
        {
            w = ini->value("width").toInt();
            h = ini->value("height").toInt();
        }
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
    delete ini;
}

void HUD::calc_lines()
{
    // scope
    scope_lines[0] = QLineF(-scope_size, 0, -scope_space, 0);
    scope_lines[1] = QLineF(scope_size, 0, scope_space, 0);
    scope_lines[2] = QLineF(0, -scope_size, 0, -scope_space);
    scope_lines[3] = QLineF(0, scope_size, 0, scope_space);

    // roll
    R = roll_outer_radius;
    r = roll_inner_radius;
    dr = R - r;
    angle = -M_PI_2;
    deg_5 = M_PI / 36;
    s_len = scope_size + 2 * scope_space;   // Отступ от прицела

    roll_lines[0] = QLineF(                         // 0
        r * qSin(angle),
        r * qCos(angle),
        R * qSin(angle),
        R * qCos(angle)
    );
    roll_lines[1] = QLineF(                         // 5
        r * qSin(angle + deg_5),
        r * qCos(angle + deg_5),
        (R - dr / 2) * qSin(angle + deg_5),
        (R - dr / 2) * qCos(angle + deg_5)
    );
    roll_lines[2] = QLineF(                         // 10
        r * qSin(angle + deg_5 * 2),
        r * qCos(angle + deg_5 * 2),
        (R - dr / 2) * qSin(angle + deg_5 * 2),
        (R - dr / 2) * qCos(angle + deg_5 * 2)
    );
    roll_lines[3] = QLineF(                         // 15
        r * qSin(angle + deg_5 * 3),
        r * qCos(angle + deg_5 * 3),
        R * qSin(angle + deg_5 * 3),
        R * qCos(angle + deg_5 * 3)
    );
    roll_lines[4] = QLineF(                         // 30
        r * qSin(angle + deg_5 * 6),
        r * qCos(angle + deg_5 * 6),
        R * qSin(angle + deg_5 * 6),
        R * qCos(angle + deg_5 * 6)
    );
    roll_lines[5] = QLineF(                         // 60
        r * qSin(angle + deg_5 * 12),
        r * qCos(angle + deg_5 * 12),
        R * qSin(angle + deg_5 * 12),
        R * qCos(angle + deg_5 * 12)
    );

    // Приборная воздушная скорость
    ias_rect = QRectF(
        left_level - ias_line_len / 2,
        top_level - label_offset - ias_font_size,
        ias_line_len,
        ias_font_size
    );

    // Радиовысота
    r_rect = QRectF(
        right_level - radio_height_font_size,
        top_level + label_offset,
        height_font_size * 3 - radio_height_font_size,
        radio_height_font_size + label_offset
    );
}

void HUD::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (planeParams->device_on)
    {
        read_config();
        calc_lines();
        resize(w, h);
        line_color.setAlpha(*brightness);

        QPen pen;
        pen.setWidthF(line_width);
        pen.setColor(line_color);

        QPainter qp(this);
        qp.setPen(pen);

        qp.translate(w/2, h/2);  // Перенос 0,0 в центр экрана
        qp.scale(-w_scale, h_scale);

        if (!debug)
            qp.rotate(270);

        switch(*mode)
        {
        case NAV_MAINT: case NAV_TFIX:
            draw_nav(&qp);
            break;
        case APP:
            draw_app(&qp);
            break;
        case A_G_GND_BMB:
            draw_bomb(&qp);
            break;
        case A_G_GND_RKT:
            draw_rocket(&qp);
            break;
        case A_G_GND_GUN:
            draw_gun(&qp);
            break;
        case A_A_AIR_GUN:
            draw_air(&qp);
            break;
        case A_A_AIR_AIM:
            draw_vizier(&qp);
            break;
        default:
            break;
        }
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

    scope(qp);
    vert_speed_indicator(qp);
    waypoint_indicator(qp);
    overload_indicator(qp);
}

void HUD::draw_app(QPainter *qp)
{
    draw(qp, "APP", true);

    scope(qp);
    vert_speed_indicator(qp);
    waypoint_indicator(qp);
    glide_mark(qp);
    glide_symbols(qp);
}

void HUD::draw_bomb(QPainter *qp)
{
    draw(qp, "GND", false);

    scope(qp);
    bomb_mark(qp);
    launch_signal(qp);
    weapon_switch(qp, "B");
}

void HUD::draw_rocket(QPainter *qp)
{
    draw(qp, "GND", false);

    scope(qp);
    viz_mark(qp);
    launch_signal(qp);
    weapon_switch(qp, "S5M");
    range_indicator(qp);
}

void HUD::draw_gun(QPainter *qp)
{
    draw(qp, "GUN", false);

    scope(qp);
    viz_mark(qp);
    launch_signal(qp);
    range_indicator(qp);
}

void HUD::draw_air(QPainter *qp)
{
    draw(qp, "LNGT", false);

    viz_mark(qp);
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
        qp->drawLines(scope_lines, 4);
    }
    qp->restore();
}

void HUD::roll_indicator(QPainter *qp)
{
    qp->save();
    {
        // Шкала
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
        qp->rotate(planeParams->roll);

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
        qp->setFont(QFont("Helvetica", pitch_font_size));

        float pitch = planeParams->pitch;
        float line_pos = pitch * roll_outer_radius / 90;

        line_pos = pitch > 90 ? roll_outer_radius : line_pos;
        line_pos = pitch < -90 ? -roll_outer_radius : line_pos;

        qp->drawLine(-pitch_line_len/2, line_pos, pitch_line_len/2, line_pos);
        qp->drawText(QRectF(
            pitch_line_len/2 - pitch_font_size*4,
            line_pos - label_offset,
            pitch_font_size*4,
            -pitch_font_size
        ), Qt::AlignRight | Qt::AlignVCenter, QString::number(static_cast<int>(pitch)));
    }
    qp->restore();
}

void HUD::vert_speed_indicator(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", vert_speed_font_size));
        qp->translate(vert_scale_offset, 0);

        float v_speed = planeParams->vertical_speed;
        float arrow_pos = -v_speed * vert_speed_line_len/2 / 30;
        arrow_pos = v_speed > 30 ? -vert_speed_line_len/2 : arrow_pos;
        arrow_pos = v_speed < -30 ? vert_speed_line_len/2 : arrow_pos;

        // Шкала
        qp->drawLine(0, -vert_speed_line_len/2, 0, vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, 0, 0, 0);
        qp->drawLine(-vert_lines_width, -vert_speed_line_len/2, 0, -vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, vert_speed_line_len/2, 0, vert_speed_line_len/2);

        // Стрелка
        qp->drawText(vert_lines_width*2, arrow_pos - label_offset, QString::number(static_cast<int>(v_speed)));
        qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 = корень из 2 / 2
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);
    }
    qp->restore();
}

void HUD::overload_indicator(QPainter *qp)
{
    qp->save();
    {
        float overload = planeParams->overload;
        float arrow_pos = -overload * vert_speed_line_len/2 / 30;

        qp->setFont(QFont("Helvetica", vert_speed_font_size));
        qp->translate(-vert_scale_offset, 0);
        qp->scale(-1, 1);

        arrow_pos = overload > 10 ? -vert_speed_line_len/2 : arrow_pos;
        arrow_pos = overload < -10 ? vert_speed_line_len/2 : arrow_pos;

        // Шкала
        qp->drawLine(0, -vert_speed_line_len/2, 0, vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, 0, 0, 0);
        qp->drawLine(-vert_lines_width, -vert_speed_line_len/2, 0, -vert_speed_line_len/2);
        qp->drawLine(-vert_lines_width, vert_speed_line_len/2, 0, vert_speed_line_len/2);

        // Стрелка
        qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 = корень из 2 / 2
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);

        qp->scale(-1, 1);
        qp->drawText(-vert_lines_width*3, arrow_pos - label_offset, QString::number(overload, 'f', 1));
    }
    qp->restore();
}

void HUD::ias(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", ias_font_size));
        qp->translate(-w/2, -h/2);

        // Индикатор
        qp->drawLine(left_level-ias_line_len/2, top_level, left_level+ias_line_len/2, top_level);
        qp->drawText(ias_rect, Qt::AlignCenter, QString::number(static_cast<int>(planeParams->forward_speed)));

        // Тенденция ускорения (треугольник)
        qp->translate(planeParams->long_acceleration * ias_a_coef, 0);
        float a = ias_font_size/2;
        float h = qSqrt(3) * a /2;
        t1[0] = QPointF(left_level, top_level);
        t1[1] = QPointF(left_level-a/2, top_level+h);
        t1[2] = QPointF(left_level+a/2, top_level+h);
        qp->drawLine(t1[0], t1[1]);
        qp->drawLine(t1[1], t1[2]);
        qp->drawLine(t1[2], t1[0]);
    }
    qp->restore();
}

void HUD::course_incdicator(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", yaw_font_size));
        qp->translate(0, -h/2);

        float yaw = planeParams->yaw;

        // ОСТОРОЖНО, ГОВНОКОД!
        for (int i = 0; i < 360; i+=10)
        {
            if (yaw < 20)
            {
                float x = yaw_scale * (i - yaw * 2 - (360 - yaw));
                QRectF left_rect(
                    x - yaw_font_size*1.5,
                    top_level - label_offset - yaw_font_size - vert_lines_height,
                    yaw_font_size*3,
                    yaw_font_size
                );
                if (left_rect.x() + left_rect.width() > -yaw_line_len/2)
                {
                    QString yaw_str = i < 100 ? "0" + QString::number(i/10) : QString::number(i/10);
                    qp->drawText(left_rect, Qt::AlignCenter, yaw_str);
                    qp->drawLine(x, top_level, x, top_level-vert_lines_height);
                    qp->drawLine(
                        x + yaw_scale * 10 / 2,
                        top_level,
                        x + yaw_scale * 10 / 2,
                        top_level - vert_lines_height / 2
                    );
                }
            }
            else if (yaw > 340)
            {
                float x = yaw_scale * (i - (yaw - 360));
                QRectF right_rect(
                    x - yaw_font_size*1.5,
                    top_level - label_offset - yaw_font_size - vert_lines_height,
                    yaw_font_size*3,
                    yaw_font_size
                );
                if (right_rect.x() + right_rect.width()/2 < yaw_line_len/2)
                {
                    QString yaw_str = i < 100 ? "0" + QString::number(i/10) : QString::number(i/10);
                    qp->drawText(right_rect, Qt::AlignCenter, yaw_str);
                    qp->drawLine(x, top_level, x, top_level-vert_lines_height);
                    qp->drawLine(
                        x + yaw_scale * 10 / 2,
                        top_level,
                        x + yaw_scale * 10 / 2,
                        top_level - vert_lines_height / 2
                    );
                }
            }

            float x = yaw_scale * (i - yaw);
            if (x < yaw_line_len/2 && x > -yaw_line_len/2)
            {
                QRectF yaw_rect(
                    x - yaw_font_size*1.5,
                    top_level - label_offset - yaw_font_size - vert_lines_height,
                    yaw_font_size*3,
                    yaw_font_size
                );
                QString yaw_str = i < 100 ? "0" + QString::number(i/10) : QString::number(i/10);
                qp->drawText(yaw_rect, Qt::AlignCenter, yaw_str);
                qp->drawLine(x, top_level, x, top_level-vert_lines_height);
                qp->drawLine(
                    x + yaw_scale * 10 / 2,
                    top_level,
                    x + yaw_scale * 10 / 2,
                    top_level - vert_lines_height / 2
                );
            }
        }

        // Шкала
        qp->drawLine(-yaw_line_len/2, top_level, yaw_line_len/2, top_level);

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
        ), Qt::AlignCenter, QString::number(static_cast<int>(planeParams->bar_height)));

        if (radio && planeParams->radio_height < 750)
        {
            qp->setFont(r_font);
            qp->drawText(QRectF(
                right_level - height_font_size*2+radio_height_font_size,
                top_level+label_offset,
                radio_height_font_size,
                radio_height_font_size+label_offset
            ), Qt::AlignCenter, "R");

            qp->drawRect(r_rect);
            qp->drawText(r_rect, Qt::AlignCenter, QString::number(static_cast<int>(planeParams->radio_height)));
        }
    }
    qp->restore();
}

void HUD::mode_indicator(QPainter *qp, QString mode)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", mode_font_size));
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
        qp->setFont(QFont("Helvetica", range_font_size));
        qp->translate(0, -h/2);

        qp->drawText(QRectF(
            -range_font_size*2,
            bottom_level+label_offset,
            range_font_size*4,
            range_font_size
        ), Qt::AlignCenter, QString::number(planeParams->waypoint_dist));
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
    if (planeParams->launch_signal)
    {
        qp->save();
        {
            qp->setFont(QFont("Helvetica", launch_font_size));
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
        qp->setFont(QFont("Helvetica", range_scale_font_size));

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
        float range = planeParams->range ;
        float arrow_pos = (range > 5) ? (-range_line_len) : range_line_len - (range * range_line_len*2) / 5;

        qp->drawLine(0, arrow_pos, vert_lines_width*2, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*3, arrow_pos);
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos-vert_lines_width*0.707); // 0.707 - корень из 2 / 2
        qp->drawLine(0, arrow_pos, vert_lines_width*0.707, arrow_pos+vert_lines_width*0.707);

        // max/min
        QPen pen;
        pen.setWidthF(line_width*3);
        qp->setPen(pen);

        float max = range_line_len - (planeParams->max_range * range_line_len*2) / 5;
        float min = range_line_len - (planeParams->min_range * range_line_len*2) / 5;

        qp->drawLine(0, max, vert_lines_width, max);
        qp->drawLine(0, min, vert_lines_width, min);
    }
    qp->restore();
}

void HUD::weapon_switch(QPainter *qp, QString str)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", launch_font_size));
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
    qp->save();
    {
        qp->scale(viz_coef, viz_coef);
        QPainterPath p;
        p.addEllipse(QPointF(0, 0), scope_space, scope_space);
        qp->fillPath(p, QColor(line_color));
        qp->drawEllipse(QPointF(0, 0), scope_size * planeParams->scope_radius/255.f + scope_size, scope_size * planeParams->scope_radius/255.f + scope_size);
    }
    qp->restore();
}

void HUD::viz_mesh(QPainter *qp)
{
    qp->save();
    {
        qp->scale(1, 1);

        float _w = w*w_scale;
        float _h = h*h_scale;
        qp->setOpacity(static_cast<float>(*brightness)/255.f);
        qp->drawPixmap(-_w/2-viz_offset, -_h/2, _w, _h, pix);
    }
    qp->restore();
}

void HUD::keyPressEvent(QKeyEvent *event)
{
//    qDebug() << event->key();
    emit keyPressed(event);
}

void HUD::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closeEventSignal();
}
