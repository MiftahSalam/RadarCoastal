#include "alarmmanager.h"

#include <QDebug>

AlarmManager* AlarmManager::alarmManager{nullptr};

AlarmManager::AlarmManager(QObject *parent)
    : QObject{parent}
{
    addAlarm(ALARM_GZ,"GZ 1");
    addAlarm(ALARM_GZ,"GZ 2");

    currentId = "";
    currentMsg = "";
    no_alarm_count = 5;
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&AlarmManager::trigger_timerTimeOut);
    timer->start(1000);
}

void AlarmManager::trigger_timerTimeOut()
{
    no_alarm_count++;
    if(no_alarm_count > 10) no_alarm_count = 10;
    if(no_alarm_count == 5)
    {
        currentId = "";
        currentMsg = "";
        emit signal_alarm("No Alarm", "No Alarm");
    }

    if(!currentId.isEmpty()) emit signal_alarm(currentId, currentMsg);
}

void AlarmManager::confirm(const QString id)
{
    currentId = "";
    currentMsg = "";

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
        const bool confirmed = alarm->isConfirmed();
        const QString curr_id = alarm->getId();
        if(!confirmed && curr_id == id)
        {
            currentId = id;
            currentMsg = message;
//            emit signal_alarm(id, message);
        }
//        else if(!alarm->isConfirmed()) emit signal_alarm(alarm->getId(), alarm->getCurrentMessage());
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
