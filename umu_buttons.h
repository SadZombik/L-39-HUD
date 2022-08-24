#ifndef UMU_BUTTONS_H
#define UMU_BUTTONS_H

#include <QSettings>

class UMU_buttons {
public:
    static void read_btn_config()
    {
        QSettings *ini = new QSettings("buttons_config.ini", QSettings::IniFormat);
        ini->beginGroup("TOP");
        for (int i = 0; i < s_top; i++)
            top[i] = ini->value(QString::number(i)).toInt();
        ini->endGroup();

        ini->beginGroup("LEFT");
        for (int i = 0; i < s_left; i++)
            left[i] = ini->value(QString::number(i)).toInt();
        ini->endGroup();

        ini->beginGroup("RIGHT");
        for (int i = 0; i < s_right; i++)
            right[i] = ini->value(QString::number(i)).toInt();
        ini->endGroup();

        ini->beginGroup("BOTTOM");
        for (int i = 0; i < s_bot; i++)
            bot[i] = ini->value(QString::number(i)).toInt();

        bot[10] = ini->value("arr_0").toInt();
        bot[11] = ini->value("arr_1").toInt();
        bot[12] = ini->value("arr_2").toInt();
        bot[13] = ini->value("arr_3").toInt();
        bot[14] = ini->value("arr_4").toInt();
        bot[15] = ini->value("arr_5").toInt();
        ini->endGroup();
    }

    static int contains(int value)
    {
       for (int i = 0; i < s_top; i++)
           if (value == top[i])
               return 0;
       for (int i = 0; i < s_bot; i++)
           if (value == bot[i])
               return 1;
       for (int i = 0; i < s_left; i++)
           if (value == left[i])
               return 2;
       for (int i = 0; i < s_right; i++)
           if (value == right[i])
               return 3;
       return -1;
    }

    static const int s_top = 5;
    static const int s_bot = 18;
    static const int s_left = 4;
    static const int s_right = 4;

    static int top[s_top];
    static int bot[s_bot];
    static int left[s_left];
    static int right[s_right];

} UB;

int UMU_buttons::top[s_top];
int UMU_buttons::bot[s_bot];
int UMU_buttons::left[s_left];
int UMU_buttons::right[s_right];

#endif // UMU_BUTTONS_H
