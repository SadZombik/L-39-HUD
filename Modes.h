#ifndef MODES_H
#define MODES_H

// Режимы работы ИЛС
enum Modes
{
    // Режим навигации (NAV)
    NAV_MAINT,
    NAV_TFIX,
    // Режим захода на посадку (APP)
    APP,
    // Воздух - Воздух
    A_A_AIR_GUN,
    A_A_AIR_AIM,
    A_A_SPAN,
    A_A_RANGE,
    // Воздух - Земля
    A_G_GND_BMB,    // Бомбометание
    A_G_GND_RKT,    // Неуправляемые ракеты
    A_G_GND_GUN,    // Cимулированная ПУШКА
    A_G_CCIP
};

#endif // MODES_H
