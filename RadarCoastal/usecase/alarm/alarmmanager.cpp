#include "usecase/alarm/alarmmanager.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, AlarmManager)
#else
#include <QDebug>
#endif

AlarmManager* AlarmManager::m_alarm_manager{nullptr};
RadarEngine::RadarEngine* AlarmManager::m_re{nullptr};

AlarmManager::AlarmManager(QObject *parent, RadarEngine::RadarEngine *re)
    : QObject{parent}
{
    m_re = re;
    AddAlarm(ALARM_GZ,"GZ 1");
    AddAlarm(ALARM_GZ,"GZ 2");

    m_current_id = "";
    m_current_msg = "";
    m_no_alarm_count = 5;
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&AlarmManager::TriggerTimerTimeOut);
    m_timer->start(1000);
}

void AlarmManager::TriggerTimerTimeOut()
{
    m_no_alarm_count++;
    if(m_no_alarm_count > 10) m_no_alarm_count = 10;
    if(m_no_alarm_count == 5)
    {
        m_current_id = "";
        m_current_msg = "";
        emit SignalAlarm("No Alarm", "No Alarm");
    }

    if(!m_current_id.isEmpty()) emit SignalAlarm(m_current_id, m_current_msg);
}

void AlarmManager::Confirm(const QString id)
{
    m_current_id = "";
    m_current_msg = "";

    Alarm* alarm = m_list_alarms.value(id,nullptr);
    if(alarm) alarm->Confirm();
}

void AlarmManager::TriggerAlarm(const QString id, const QString message)
{
#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<id<<message;
#else
    qDebug()<<Q_FUNC_INFO<<id<<message;
#endif

    m_no_alarm_count = 0;

    QMapIterator<QString, Alarm*>i(m_list_alarms);
    while (i.hasNext()) {
        i.next();
        Alarm* alarm = i.value();
        const bool confirmed = alarm->IsConfirmed();
        const QString curr_id = alarm->GetId();
        if(!confirmed && curr_id == id)
        {
            m_current_id = id;
            m_current_msg = message;
//            emit signal_alarm(id, message);
        }
//        else if(!alarm->isConfirmed()) emit signal_alarm(alarm->getId(), alarm->getCurrentMessage());
    }
}

bool AlarmManager::AddAlarm(const AlarmType type, const QString id)
{
    if(id.isEmpty()) return false;

    Alarm* baseAlarm = nullptr;

    switch (type) {
    case ALARM_GZ:
    {
        GZAlarm* gzAlarm = new GZAlarm(this,id, m_alarm_manager->m_re);
        baseAlarm = dynamic_cast<Alarm*>(gzAlarm);
    }
        break;
    default:
        break;
    }

    connect(baseAlarm, &Alarm::SignalAlarmTriggered, this, &AlarmManager::TriggerAlarm);
    m_list_alarms.insert(id,baseAlarm);

    return true;
}

AlarmManager* AlarmManager::GetInstance(RadarEngine::RadarEngine *re)
{
    if(!m_alarm_manager) {
        m_alarm_manager = new AlarmManager(nullptr, re);
    }
    return m_alarm_manager;
}

AlarmManager::~AlarmManager()
{
}
