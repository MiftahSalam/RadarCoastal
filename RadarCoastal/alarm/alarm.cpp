#include "alarm.h"

Alarm::Alarm(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Alarm::trigger_timeOut);
}

void Alarm::trigger_timeOut()
{
    checkAlarm();
}

void Alarm::init(int time)
{
    timer->start(time);
}
