#include "device.h"

Device::Device(QObject *parent): QObject(parent)
{
    server = new TCP_server();
    planeParams = &(server->displayPacket);

    umu = new UMU;
    umu->setPlaneParams(planeParams);
    umu->show();

    hud = new HUD;
    hud->move(800,0);
    hud->setPlaneParams(planeParams);
    hud->setMode(umu->Mode());
    hud->setBrightness(umu->Brightness());
    hud->showFullScreen();
    hud->show();

    connect(hud, SIGNAL(keyPressed(QKeyEvent*)), umu, SLOT(keyPressedSlot(QKeyEvent*)));
    connect(hud, SIGNAL(closeEventSignal()), umu, SLOT(closeEventSlot()));



//    connect(&timer, &QTimer::timeout, this, [this](){
//        t.planeParams.device_on = true;
//        t.planeParams.longitude = 0;
//        t.planeParams.lattitude = 0;
//        t.planeParams.bar_height = t.getHeight();
//        t.planeParams.radio_height = t.getRHeight();
//        t.planeParams.roll = t.getRoll();
//        t.planeParams.pitch = t.getPitch();
//        t.planeParams.yaw = t.getYaw();
//        t.planeParams.engine_speed = 0;
//        t.planeParams.forward_speed = t.getForwSpeed();
//        t.planeParams.vertical_speed = t.getVertSpeed();
//        t.planeParams.sliding_angle = 0;
//        t.planeParams.long_acceleration = t.getForwSpeed() - t.planeParams.forward_speed;
//        t.planeParams.waypoint_dist = 0;
//        t.planeParams.fuel_quantity = 0;
//        t.planeParams.fuel_consumption = 0;
//        t.planeParams.launch_signal = 0;
//        t.planeParams.range = 0;
//        t.planeParams.hit_point_x = 0;
//        t.planeParams.hit_point_y = 0;
//        t.planeParams.hit_point_z = 0;
//        t.planeParams.max_range = 0;
//        t.planeParams.min_range = 0;
//        t.planeParams.overload = 0;
//        t.planeParams.scope_radius = 0;
//    });
//    timer.start(33);
//    t.show();
}


Device::~Device()
{
    delete hud;
    delete umu;
    delete server;
}

