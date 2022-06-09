#include "alarm.h"

Alarm::Alarm(QObject *parent, AlarmType type, QString id)
    : QObject{parent}, m_id(id), m_type(type)
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Alarm::trigger_timeOut);
}

QString Alarm::getId() const
{
    return m_id;
}

void Alarm::setCurrentMessage(const QString message)
{
    m_current_messsage = message;
}

QString Alarm::getCurrentMessage() const
{
    return m_current_messsage;
}

AlarmType Alarm::getType() const
{
    return m_type;
}

void Alarm::trigger_timeOut()
{
    checkAlarm();
}

void Alarm::init(int time)
{
    timer->start(time);
}
