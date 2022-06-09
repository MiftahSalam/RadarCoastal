#include "alarmmanager.h"

#include <QDebug>

AlarmManager* AlarmManager::alarmManager{nullptr};

AlarmManager::AlarmManager(QObject *parent)
    : QObject{parent}
{
    addAlarm(ALARM_GZ,"GZ1");

    no_alarm_count = 5;
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&AlarmManager::trigger_timerTimeOut);
    timer->start(1000);
}

void AlarmManager::trigger_timerTimeOut()
{
    no_alarm_count++;
    if(no_alarm_count > 10) no_alarm_count = 10;
    if(no_alarm_count == 5) emit signal_alarm("No Alarm", "No Alarm");
}

void AlarmManager::confirm(const QString id)
{
    Alarm* alarm = listAlarms.value(id,nullptr);
    if(alarm) alarm->confirm();
}

void AlarmManager::trigger_alarm(const QString id, const QString message)
{
    qDebug()<<Q_FUNC_INFO<<id<<message;

    no_alarm_count = 0;

    QMapIterator<QString, Alarm*>i(listAlarms);
    while (i.hasNext()) {
        i.next();
        Alarm* alarm = i.value();
        if(!alarm->isConfirmed() && alarm->getId() == id) emit signal_alarm(id, message);
        else if(!alarm->isConfirmed()) emit signal_alarm(alarm->getId(), alarm->getCurrentMessage());
    }
}

bool AlarmManager::addAlarm(const AlarmType type, const QString id)
{
    if(id.isEmpty()) return false;

    Alarm* baseAlarm = nullptr;

    switch (type) {
    case ALARM_GZ:
    {
        GZAlarm* gzAlarm = new GZAlarm(this,id);
        baseAlarm = dynamic_cast<Alarm*>(gzAlarm);
    }
        break;
    default:
        break;
    }

    connect(baseAlarm, &Alarm::signal_alarmTriggered, this, &AlarmManager::trigger_alarm);
    listAlarms.insert(id,baseAlarm);

    return true;
}

AlarmManager* AlarmManager::getInstance()
{
    if(!alarmManager) alarmManager = new AlarmManager();
    return alarmManager;
}

AlarmManager::~AlarmManager()
{
}
