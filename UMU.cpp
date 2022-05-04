#include "UMU.h"
#include "ui_UMU.h"

UMU::UMU(quint16 width, quint16 height, QWidget *parent) :
     QWidget(parent), ui(new Ui::UMU), w(width), h(height)
{
    // Форма и размеры
    ui->setupUi(this);
    resize(w, h);

    current_screen = Screens::Hud;

    // Таблица стилей
    QFile styleSheetFile("./styles.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    setStyleSheet(styleSheet);

    main_button = new QPushButton("MAIN", this);
    nav_button = new QPushButton("NAV", this);
    xpdr_button = new QPushButton("XPDR", this);
    radios_button = new QPushButton("RADIOS", this);
    misc_button = new QPushButton("MISC", this);
    hud_button = new QPushButton("HUD", this);

    connect(main_button, SIGNAL(clicked()), this, SLOT(main_button_clicked()));
    connect(nav_button, SIGNAL(released()), this, SLOT(nav_button_clicked()));
    connect(xpdr_button, SIGNAL(released()), this, SLOT(xpdr_button_clicked()));
    connect(radios_button, SIGNAL(released()), this, SLOT(radios_button_clicked()));
    connect(misc_button, SIGNAL(released()), this, SLOT(misc_button_clicked()));
    connect(hud_button, SIGNAL(released()), this, SLOT(hud_button_clicked()));

    QTimer *timer = new QTimer(this);
    timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    /*hud_button
    misc_button
    radios_button
    xpdr_button
    nav_button
    main_button*/

    hud = new HUD(625, 750, false);
    hud->move(1920/2, (1080-hud->height())/2);
    hud->show();
}

void UMU::rescale()
{
    read_config();

    w = width();
    h = height();

    int x = w/6, y = h/6;
    int b = h-y;
    QSize button_size(x, y);

    hud_button->setGeometry(QRect(QPoint(x*5, b), button_size));
    misc_button->setGeometry(QRect(QPoint(x*4, b), button_size));
    radios_button->setGeometry(QRect(QPoint(x*3, b), button_size));
    xpdr_button->setGeometry(QRect(QPoint(x*2, b), button_size));
    nav_button->setGeometry(QRect(QPoint(x, b), button_size));
    main_button->setGeometry(QRect(QPoint(0, b), button_size));
}

void UMU::read_config()
{
    QSettings *ini = new QSettings("umu_config.ini", QSettings::IniFormat);
    ini->beginGroup("GEOMETRY");
    {
        hud_brightness_line_len = w * ini->value("hud_brightness_line_len_koef").toFloat();;
    }
    ini->endGroup();

    ini->beginGroup("FONTS");
    {
        main_menu_font_size = ini->value("main_menu_font_size").toInt();;
        nav_menu_font_size = ini->value("nav_menu_font_size").toInt();;
        xpdr_menu_font_size = ini->value("xpdr_menu_font_size").toInt();;
        radios_menu_font_size = ini->value("radios_menu_font_size").toInt();;
        misc_menu_font_size = ini->value("misc_menu_font_size").toInt();;
        hud_menu_font_size = ini->value("hud_menu_font_size").toInt();;
    }
    ini->endGroup();
}

void UMU::paintEvent(QPaintEvent *event)
{
    rescale();

    Q_UNUSED(event);
    QPainter qp(this);

    QPen pen;
    pen.setColor(Qt::white);
    qp.setPen(pen);

    switch (current_screen)
    {
    case Main:
        draw_main_menu(&qp);
        break;
    case Nav:
        draw_nav_menu(&qp);
        break;
    case Xpdr:
        draw_xpdr_menu(&qp);
        break;
    case Radios:
        draw_radios_menu(&qp);
        break;
    case Misc:
        draw_misc_menu(&qp);
        break;
    case Hud:
        draw_hud_menu(&qp);
        break;
    default:
        break;
    }
}

void UMU::keyPressEvent(QKeyEvent *event)
{
    if (current_screen == Screens::Hud)
    {
        switch (event->key())
        {
        case Qt::Key_1:
            hud->select_mode(Modes::NAV);
            break;
        case Qt::Key_2:
            hud->select_mode(Modes::APP);
            break;
        case Qt::Key_3:
            hud->select_mode(Modes::BOMB);
            break;
        case Qt::Key_4:
            hud->select_mode(Modes::ROCKET);
            break;
        case Qt::Key_5:
            hud->select_mode(Modes::VIZIER);
            break;
        default:
            break;
        }
    }
}

void UMU::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    QApplication::closeAllWindows();
}

void UMU::set_color(QPushButton* btn)
{
    hud_button->setStyleSheet("color: white");
    misc_button->setStyleSheet("color: white");
    radios_button->setStyleSheet("color: white");
    xpdr_button->setStyleSheet("color: white");
    nav_button->setStyleSheet("color: white");
    main_button->setStyleSheet("color: white");
    btn->setStyleSheet("color: green");
}

void UMU::draw_main_menu(QPainter *qp)
{

}
void UMU::draw_nav_menu(QPainter *qp)
{

}
void UMU::draw_xpdr_menu(QPainter *qp)
{

}
void UMU::draw_radios_menu(QPainter *qp)
{

}
void UMU::draw_misc_menu(QPainter *qp)
{

}
void UMU::draw_hud_menu(QPainter *qp)
{
    qp->save();
    {
        QFont font("Helvetica", hud_menu_font_size);
        QPen w_pen(Qt::white);
        QPen wbpen(Qt::blue);
        qp->setFont(font);
        qp->setPen(w_pen);

        // Боковые надписи
        int h_rem = h-h/6;
        int dy = h_rem/9*2;
        QRectF r_left[4];
        QRectF r_right[4];

        for (int i = 0; i < 4; i++)
        {
            r_left[i].setCoords(0, i*dy, w/6, (i+1)*dy);
            r_right[i].setCoords(w-w/6, i*dy, w, (i+1)*dy);
        }

        qp->drawText(r_left[0],  Qt::AlignBottom | Qt::AlignLeft, " NAV");
        qp->drawText(r_left[1],  Qt::AlignBottom | Qt::AlignLeft, " APP");
        qp->drawText(r_left[2],  Qt::AlignBottom | Qt::AlignLeft, " A-A");
        qp->drawText(r_left[3],  Qt::AlignBottom | Qt::AlignLeft, " A-G");

        qp->drawText(r_right[0], Qt::AlignBottom | Qt::AlignRight, "TEST ");
        qp->drawText(r_right[1], Qt::AlignBottom | Qt::AlignRight, "DATA ");
        qp->drawText(r_right[2], Qt::AlignBottom | Qt::AlignRight, "GUN ");
        qp->drawText(r_right[3], Qt::AlignBottom | Qt::AlignRight, "BMB ");

        // Яркость ИЛС
        qp->drawText(QRectF(0, 0, w, h/6),  Qt::AlignTop | Qt::AlignCenter, "HUD BRIGHTNESS");
        qp->drawRoundedRect(
            (w - hud_brightness_line_len)/2,
            dy*0.75,
            hud_brightness_line_len,
            hud_menu_font_size/2,
            10, 10
        );
    }
    qp->restore();
}

void UMU::hud_button_clicked()
{
    set_color(hud_button);
    current_screen = Screens::Hud;
}
void UMU::misc_button_clicked()
{
    set_color(misc_button);
    current_screen = Screens::Misc;
}
void UMU::radios_button_clicked()
{
    set_color(radios_button);
    current_screen = Screens::Radios;
}
void UMU::xpdr_button_clicked()
{
    set_color(xpdr_button);
    current_screen = Screens::Xpdr;
}
void UMU::nav_button_clicked()
{
    set_color(nav_button);
    current_screen = Screens::Nav;
}
void UMU::main_button_clicked()
{
    set_color(main_button);
    current_screen = Screens::Main;
}

UMU::~UMU()
{
    delete ui;
    delete hud;

    delete hud_button;
    delete misc_button;
    delete radios_button;
    delete xpdr_button;
    delete nav_button;
    delete main_button;
}
