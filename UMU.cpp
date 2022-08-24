#include "UMU.h"
#include "umu_buttons.h"
#include <QDebug>

UMU::UMU(QWidget *parent): QWidget(parent)
{
    read_config();
    resize(w, h);
    move(x_move, y_move);
    setStyleSheet("background-color: black;");

    calc_rects();

    QTimer *timer = new QTimer(this);
    timer->start(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    b_index = 5;
    l_index = 0;
    r_index = 0;
    hud_brightness = 128;

    UMU_buttons::read_btn_config();
    draw_menu_ptr = &UMU::draw_hud_menu;
    hud_menu_ptr = &UMU::hud_nav;
    hud_mode = NAV_MAINT;

}

void UMU::read_config()
{
    ini = new QSettings("umu_config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    { 
        w = ini->value("width").toInt();
        h = ini->value("height").toInt();
        x_move = ini->value("x_move").toInt();
        y_move = ini->value("y_move").toInt();

        line_width = ini->value("line_width").toFloat();
        hud_brightness_line_len = w * ini->value("hud_brightness_line_len_koef").toFloat();
    }
    ini->endGroup();

    ini->beginGroup("FONTS");
    {
        buttons_font_size = ini->value("buttons_font_size").toInt();
        main_menu_font_size = ini->value("main_menu_font_size").toInt();
        nav_menu_font_size = ini->value("nav_menu_font_size").toInt();
        xpdr_menu_font_size = ini->value("xpdr_menu_font_size").toInt();
        radios_menu_font_size = ini->value("radios_menu_font_size").toInt();
        misc_menu_font_size = ini->value("misc_menu_font_size").toInt();
        hud_menu_font_size = ini->value("hud_menu_font_size").toInt();
    }
    ini->endGroup();

    ini->beginGroup("PARAMS");
    {
        code = ini->value("code").toInt();
    }
    ini->endGroup();

    delete ini;
}

void UMU::calc_rects()
{
    dy = (h-h/6)/4;

    for (int i = 0; i < 4; i++)
    {
        rects_left[i].setCoords(0, i*dy, w/2, (i+1)*dy);
        rects_right[i].setCoords(w/2, i*dy, w, (i+1)*dy);
    }

    for (int i = 0; i < 4; i++)
    {
        R_hud_l[i].setCoords(0, i*dy, w/9, (i+1)*dy);
        R_hud_r[i].setCoords(w-w/6, i*dy, w, (i+1)*dy);
    }

    // MAIN
    COM2 = QRectF(0, 0, w/2, dy);
    NAV1 = QRectF(0, dy, w/2, dy);

    R_main[0] = QRectF(w/2, 0.5*dy, w/2, -main_menu_font_size*2);
    R_main[1] = QRectF(w/2, 0.5*dy, w/2, +main_menu_font_size*2);
    R_main[2] = QRectF(w/2, 1.5*dy, w/2, -main_menu_font_size*2);
    R_main[3] = QRectF(w/2, 1.5*dy, w/2, +main_menu_font_size*2);
    R_main[4] = QRectF(w/2, 2.5*dy, w/2, -main_menu_font_size*2);
    R_main[5] = QRectF(w/2, 2.5*dy, w/2, +main_menu_font_size*2);
    R_main[6] = QRectF(w/2, 3.5*dy, w/2, -main_menu_font_size*2);
    R_main[7] = QRectF(w/2, 3.5*dy, w/2, +main_menu_font_size*2);

    // NAV
    R_nav[0] = QRectF(w/2, 0.5*dy, w/2, -main_menu_font_size*2);
    R_nav[1] = QRectF(w/2, 0.5*dy, w/2, +main_menu_font_size*2);
    R_nav[2] = QRectF(w/2, 1.5*dy, w/2, -main_menu_font_size*2);
    R_nav[3] = QRectF(w/2, 1.5*dy, w/2, +main_menu_font_size*2);

    y = rects_left[2].y();
    _dy = (h - h/6 - rects_left[2].y()) / 5;
    _dx = w/4.8;
    mc_dx = w/9.67;
    dist_dx = w/7.3;

    ID[0] = QRectF(0, y, _dx, _dy);
    ETA[0] = QRectF(w/1.52, y, _dx, _dy);
    EFR[0] = QRectF(ETA[0].x() + _dx, y, w/7.5, _dy);

    for (int i = 1; i < 5; i++)
    {
        ID[i] = QRectF(ID[0].x(), ID[i-1].y() + _dy, _dx, _dy);
        ETA[i] = QRectF(ETA[0].x(), ETA[i-1].y() + _dy, _dx, _dy);
        EFR[i] = QRectF(ETA[0].x() + _dx, EFR[i-1].y() + _dy, w/7.5, _dy);
    }

    subtable(MC, mc_dx, ID[0].x(), _dx);
    subtable(DIST, dist_dx, MC[0].x(), mc_dx);
    subtable(ETE, _dx, DIST[0].x(), dist_dx);

    // MISC
    for (int i = 0; i < 3; i++)
        R_misc[i].setCoords(w/2, dy*i, w, dy*(i+1));

}

void UMU::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (planeParams->device_on)
    {
        read_config();

        QPainter qp(this);
        QPen pen;
        pen.setColor(Qt::white);
        pen.setWidthF(line_width);
        qp.setPen(pen);

        QFont font("Monospace", main_menu_font_size);
        font.setStyleHint(QFont::TypeWriter);
        qp.setFont(font);

        (this->*draw_menu_ptr)(&qp);
        draw_buttons(&qp);
    }
}

void UMU::keyPressedSlot(QKeyEvent *event)
{
    this->keyPressEvent(event);
}

void UMU::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    switch (UMU_buttons::contains(key))
    {
    case 0:
        // top_buttons
        break;
    case 1:
        bottom_buttons(key);
        break;
    case 2:
        left_buttons(key);
        break;
    case 3:
        right_buttons(key);
        break;
    case -1:
        qDebug() << "No such key";
        break;
    }

    if (key == 91)
        hud_brightness = (hud_brightness - 10 < 0) ? 0 : hud_brightness - 10;
    else if (key == 93)
        hud_brightness = (hud_brightness + 10 > 255) ? 255 : hud_brightness + 10;
}

void UMU::bottom_buttons(int key)
{
    if (key == UB.bot[10])
    {
        draw_menu_ptr = &UMU::draw_main_menu;
        b_index = 0;
    }
    else if (key == UB.bot[11])
    {
        draw_menu_ptr = &UMU::draw_nav_menu;
        b_index = 1;
    }
    else if (key == UB.bot[12])
    {
        draw_menu_ptr = &UMU::draw_xpdr_menu;
        b_index = 2;
    }
    else if (key == UB.bot[13])
    {
        draw_menu_ptr = &UMU::draw_radios_menu;
        b_index = 3;
    }
    else if (key == UB.bot[14])
    {
        draw_menu_ptr = &UMU::draw_misc_menu;
        b_index = 4;
    }
    else if (key == UB.bot[15])
    {
        draw_menu_ptr = &UMU::draw_hud_menu;
        b_index = 5;
    }
}

void UMU::left_buttons(int key)
{
    if (draw_menu_ptr == &UMU::draw_hud_menu)
    {
        r_index = 0;

        if (key == UB.left[0])
        {
            hud_mode = NAV_MAINT;
            l_index = 0;
            hud_menu_ptr = &UMU::hud_nav;
        }
        else if (key == UB.left[1])
        {
            hud_mode = APP;
            l_index = 1;
            hud_menu_ptr = &UMU::hud_app;
        }
        else if (key == UB.left[2])
        {
            hud_mode = A_A_AIR_GUN;
            l_index = 2;
            hud_menu_ptr = &UMU::hud_a_a;
        }
        else if (key == UB.left[3])
        {
            hud_mode = A_G_GND_RKT;
            l_index = 3;
            hud_menu_ptr = &UMU::hud_a_g;
        }
    }
}

void UMU::right_buttons(int key)
{
    if (draw_menu_ptr == &UMU::draw_hud_menu)
    {
        if (key == UB.right[0]) r_index = 0;
        else if (key == UB.right[1]) r_index = 1;
        else if (key == UB.right[2]) r_index = 2;
        else if (key == UB.right[3]) r_index = 3;
    }
}

void UMU::draw_main_menu(QPainter *qp)
{
    qp->save();
    {
        qp->drawText(COM2, Qt::AlignLeft | Qt::AlignVCenter, " COM2 CH "
                     + QString("%1").arg(COM2_1, 3) + " "
                     + QString("%1").arg(COM2_2, 3) + "."
                     + QString("%1").arg(COM2_3, 3)
        );
        qp->drawText(NAV1, Qt::AlignLeft | Qt::AlignVCenter, " NAV1 CH M 113.70");

        QString hud_mode, del_mode;
        switch (l_index)
        {
        case 0:
            hud_mode = "NAV";
            del_mode = " - ";
            break;
        case 1:
            hud_mode = "APP";
            del_mode = " - ";
            break;
        case 2:
            hud_mode = "A-A";
            switch (r_index)
            {
            case 0:
                del_mode = "GUN";
                break;
            case 1:
                del_mode = "AIM";
                break;
            default:
                del_mode = " - ";
                break;
            }
            break;
        case 3:
            hud_mode = "A-G";
            switch (r_index)
            {
            case 0:
                del_mode = "BMB";
                break;
            case 1:
                del_mode = "RKT";
                break;
            case 2:
                del_mode = "GUN";
                break;
            default:
                del_mode = " - ";
                break;
            }
            break;
        default:
            hud_mode = "NAV";
            del_mode = " - ";
            break;
        }

        qp->drawText(R_main[0], Qt::AlignRight | Qt::AlignVCenter, "HUD MODE:  " + hud_mode + " ");
        qp->drawText(R_main[2], Qt::AlignRight | Qt::AlignVCenter, "TO WPT:        N/A ");
        qp->drawText(R_main[4], Qt::AlignRight | Qt::AlignVCenter, "ZT     " + QDateTime::currentDateTime().toUTC().time().toString()+ " ");
        qp->drawText(R_main[5], Qt::AlignRight | Qt::AlignVCenter, "LT     " + QTime::currentTime().toString()+ " ");
        qp->drawText(R_main[6], Qt::AlignRight | Qt::AlignVCenter, "IDENT CODE: " + QString::number(code) + " ");

        qp->setPen(Qt::gray);
        qp->drawText(R_main[1], Qt::AlignRight | Qt::AlignVCenter, "DEL MODE:  " + del_mode + " ");
        qp->drawText(R_main[3], Qt::AlignRight | Qt::AlignVCenter, "DIST      --  ETE    ------ ");
        qp->drawText(R_main[7], Qt::AlignRight | Qt::AlignVCenter, "MODE: ALT ");
    }
    qp->restore();
}

void UMU::draw_nav_menu(QPainter *qp)
{
    qp->save();
    {
        rects_left[0].setWidth(w/2);
        rects_left[1].setWidth(w/2);

        qp->drawText(rects_left[0], Qt::AlignLeft | Qt::AlignVCenter, " STEERPOINT:     N/A");
        qp->drawText(rects_left[1], Qt::AlignLeft | Qt::AlignVCenter, " EDIT FLIGHT PLAN");

        // TODO: только наше полушарие N W
        // Добавить обратку отрицательных долготы и широты
        qp->drawText(R_nav[0], Qt::AlignRight | Qt::AlignVCenter, "FUEL QTY:    " + QString("%1").arg(static_cast<int>(planeParams->fuel_quantity), 4) + " LBS ");
        qp->drawText(R_nav[2], Qt::AlignRight | Qt::AlignVCenter, "LAT: N " + degConvert(planeParams->lattitude));
        qp->drawText(R_nav[3], Qt::AlignRight | Qt::AlignVCenter, "LON: W " + degConvert(planeParams->longitude));

        // Таблица
        for (int i = 0; i < 5; i+=2)
        {
            qp->fillRect(ID[i], cl);
            qp->fillRect(ETA[i], cl);
            qp->fillRect(EFR[i], cl);
        }

        qp->fillRect(MC[0], cl);
        qp->fillRect(DIST[0], cl);
        qp->fillRect(ETE[0], cl);

        qp->drawText(ID[0], Qt::AlignCenter, "ID");
        qp->drawText(MC[0], Qt::AlignCenter, "MC");
        qp->drawText(DIST[0], Qt::AlignCenter, "DIST");
        qp->drawText(ETE[0], Qt::AlignCenter, "ETE");
        qp->drawText(ETA[0], Qt::AlignCenter, "ETA");
        qp->drawText(EFR[0], Qt::AlignCenter, "EFR");

        qp->drawRects(ID, 5);
        qp->drawRects(MC, 6);
        qp->drawRects(DIST, 6);
        qp->drawRects(ETE, 6);
        qp->drawRects(ETA, 5);
        qp->drawRects(EFR, 5);

        qp->setPen(Qt::gray);
        qp->drawText(R_nav[1], Qt::AlignRight | Qt::AlignVCenter, "FUEL FLOW: " + QString("%1").arg(static_cast<int>(planeParams->fuel_consumption), 3) + " LBS/hr ");
    }
    qp->restore();
}

void UMU::draw_xpdr_menu(QPainter *qp)
{
    qp->save();
    {
        rects_left[0].setWidth(w/2);
        rects_left[1].setWidth(w/2);

        qp->drawText(rects_left[0], Qt::AlignLeft | Qt::AlignVCenter, " MODE: ALT");
        qp->drawText(rects_left[1], Qt::AlignLeft | Qt::AlignVCenter, " CODE: " + QString::number(code) + " ");
        qp->drawText(rects_left[2], Qt::AlignLeft | Qt::AlignVCenter, " IDENT");
        qp->drawText(rects_right[0], Qt::AlignRight | Qt::AlignVCenter, "EMERG ");
    }
    qp->restore();

}

void UMU::draw_radios_menu(QPainter *qp)
{
    qp->save();
    {
        qp->drawText(rects_left[0], Qt::AlignLeft | Qt::AlignVCenter, " COM2");
        qp->drawText(rects_left[1], Qt::AlignLeft | Qt::AlignVCenter, " NAV1");
    }
    qp->restore();
}

//void UMU::radios_draw_com2(QPainter *qp)
//{
//    qp->save();
//    {
//        rects_left[0].setWidth(w/2);
//        rects_left[1].setWidth(w/2);
//        qp->drawText(rects_left[0], Qt::AlignLeft | Qt::AlignVCenter, " PRESETS: CH "
//                + QString("%1").arg(COM2_1, 3) + " "
//                + QString("%1").arg(COM2_2, 3) + "."
//                + QString("%1").arg(COM2_3, 3)
//        );
//        qp->drawText(rects_left[1], Qt::AlignLeft | Qt::AlignVCenter, " CHANNEL SPACING: " + QString::number(spacing) + "kHz");
//    }
//    qp->restore();
//}

void UMU::draw_misc_menu(QPainter *qp)
{
    qp->save();
    {
        rects_left[0].setWidth(w/2);
        rects_left[1].setWidth(w/2);
        rects_left[2].setWidth(w/2);

        qp->drawText(rects_left[0], Qt::AlignLeft | Qt::AlignVCenter, " LAT: N " + degConvert(0));
        qp->drawText(rects_left[1], Qt::AlignLeft | Qt::AlignVCenter, " LON: N " + degConvert(0));
        qp->drawText(rects_left[2], Qt::AlignLeft | Qt::AlignVCenter, " ELEV:       0 m");

        qp->drawText(R_misc[0], Qt::AlignRight| Qt::AlignVCenter, "JOKER:  0 KG ");
        qp->drawText(R_misc[1], Qt::AlignRight| Qt::AlignVCenter, "BINGO:  0 KG ");
        qp->drawText(R_misc[2], Qt::AlignRight| Qt::AlignVCenter, "BIT ");
    }
    qp->restore();
}

void UMU::draw_hud_menu(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", hud_menu_font_size));

        // Яркость ИЛС
        qp->setPen(QPen(Qt::green));
        QRectF brightness_rect(
            (w - hud_brightness_line_len)/2,
            dy*0.75,
            hud_brightness_line_len*hud_brightness/255.f,
            hud_menu_font_size/2
        );

        qp->fillRect(brightness_rect, QColor(Qt::green));
        qp->drawRoundedRect(brightness_rect, 10, 10);

        qp->setPen(QPen(Qt::white));
        qp->drawRoundedRect(
            (w - hud_brightness_line_len)/2,
            dy*0.75,
            hud_brightness_line_len,
            hud_menu_font_size/2,
            10, 10
        );

        qp->drawText(QRectF(0, 0, w, h/6),  Qt::AlignTop | Qt::AlignCenter, "HUD BRIGHTNESS");

        // Кнопки
        qp->drawText(R_hud_l[0],  Qt::AlignCenter, "NAV");
        qp->drawText(R_hud_l[1],  Qt::AlignCenter, "APP");
        qp->drawText(R_hud_l[2],  Qt::AlignCenter, "A-A");
        qp->drawText(R_hud_l[3],  Qt::AlignCenter, "A-G");
    }
    qp->restore();
    (this->*hud_menu_ptr)(qp);
}

void UMU::hud_nav(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", hud_menu_font_size));

        qp->drawText(R_hud_r[0], Qt::AlignCenter, "MAINT");
        qp->drawText(R_hud_r[3], Qt::AlignCenter, "TFIX");

        qp->setPen(Qt::black);
        qp->fillRect(R_hud_l[l_index], QColor(Qt::white));
        qp->drawText(R_hud_l[l_index], Qt::AlignCenter, "NAV");

        if (r_index == 0)
        {
            hud_mode = NAV_MAINT;
            qp->fillRect(R_hud_r[r_index], QColor(Qt::white));
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "MAINT");
        }
        else if (r_index == 3)
        {
            hud_mode = NAV_TFIX;
            qp->fillRect(R_hud_r[r_index], QColor(Qt::white));
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "TFIX");
        }
    }
    qp->restore();
}

void UMU::hud_app(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", hud_menu_font_size));

        qp->setPen(Qt::black);
        qp->fillRect(R_hud_l[l_index], QColor(Qt::white));
        qp->drawText(R_hud_l[l_index], Qt::AlignCenter, "APP");

        hud_mode = APP;
    }
    qp->restore();
}

void UMU::hud_a_a(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", hud_menu_font_size));

        qp->drawText(R_hud_r[0], Qt::AlignCenter, "GUN");
        qp->drawText(R_hud_r[1], Qt::AlignCenter, "AIM");

        qp->setPen(Qt::black);
        qp->fillRect(R_hud_l[l_index], QColor(Qt::white));
        qp->drawText(R_hud_l[l_index], Qt::AlignCenter, "A-A");

        qp->fillRect(R_hud_r[r_index], QColor(Qt::white));
        switch (r_index)
        {
        case 0:
            hud_mode = A_A_AIR_GUN;
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "GUN");
            break;
        case 1:
            hud_mode = A_A_AIR_AIM;
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "AIM");
            break;
        case 2:

            break;
        case 3:

            break;
        }
    }
    qp->restore();
}

void UMU::hud_a_g(QPainter *qp)
{
    qp->save();
    {
        qp->setFont(QFont("Helvetica", hud_menu_font_size));

        qp->drawText(R_hud_r[0], Qt::AlignCenter, "BMB");
        qp->drawText(R_hud_r[1], Qt::AlignCenter, "RKT");
        qp->drawText(R_hud_r[2], Qt::AlignCenter, "GUN");

        qp->setPen(Qt::black);
        qp->fillRect(R_hud_l[l_index], QColor(Qt::white));
        qp->drawText(R_hud_l[l_index], Qt::AlignCenter, "A-G");

        qp->fillRect(R_hud_r[r_index], QColor(Qt::white));
        switch (r_index)
        {
        case 0:
            hud_mode = A_G_GND_BMB;
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "BMB");
            break;
        case 1:
            hud_mode = A_G_GND_RKT;
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "RKT");
            break;
        case 2:
            hud_mode = A_G_GND_GUN;
            qp->drawText(R_hud_r[r_index], Qt::AlignCenter, "GUN");
            break;
        case 3:

            break;
        }
    }
    qp->restore();
}

void UMU::draw_buttons(QPainter *qp)
{
    qp->save();
    {
        // Нижние кнопки
        qp->setFont(QFont("Helvetica", buttons_font_size));

        float button_h = h - h / 6;
        QRectF buttons[6];
        QString titles[6] = { "MAIN", "NAV", "XPDR", "RADIOS", "MISC", "HUD" };

        for (int i = 0; i < 6; i++)
        {
            buttons[i] = QRectF(QPointF(w/6*i, button_h), QSizeF(w/6, h/6-line_width));
            qp->drawRect(buttons[i]);
            qp->drawText(buttons[i], Qt::AlignCenter, titles[i]);
        }

        qp->setPen(Qt::black);
        qp->fillRect(buttons[b_index], QColor(Qt::white));
        qp->drawText(buttons[b_index], Qt::AlignCenter, titles[b_index]);
    }
    qp->restore();
}

void UMU::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    closeEventSlot();
}


void UMU::closeEventSlot()
{
    QApplication::closeAllWindows();
}

UMU::~UMU()
{

}
