#include "usecase/alarm/alarm.h"

Alarm::Alarm(QObject *parent, AlarmType type, QString id)
    : QObject{parent}, m_id(id), m_type(type)
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Alarm::triggerTimeOut);
}

QString Alarm::GetId() const
{
    return m_id;
}

void Alarm::SetCurrentMessage(const QString message)
{
    m_current_messsage = message;
}

QString Alarm::GetCurrentMessage() const
{
    return m_current_messsage;
}

AlarmType Alarm::GetType() const
{
    return m_type;
}

void Alarm::triggerTimeOut()
{
    checkAlarm();
}

void Alarm::init(int time)
{
    timer->start(time);
}
