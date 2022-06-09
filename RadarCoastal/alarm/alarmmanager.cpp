#include "alarmmanager.h"

#include <QDebug>

AlarmManager* AlarmManager::alarmManager{nullptr};

AlarmManager::AlarmManager(QObject *parent)
    : QObject{parent}
{
    addAlarm(ALARM_GZ,"GZ1");
}

void AlarmManager::confirm(const QString id)
{
    Alarm* alarm = listAlarms.value(id,nullptr);
    if(alarm) alarm->confirm();
}

void AlarmManager::trigger_alarm(const QString id, const QString message)
{
    qDebug()<<Q_FUNC_INFO<<id<<message;
    emit signal_alarm(id, message);
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
