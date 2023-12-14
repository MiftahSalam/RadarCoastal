#include "usecase/alarm/gzalarm.h"

#include <RadarEngine/radarconfig.h>

GZAlarm::GZAlarm(QObject *parent, QString id, RadarEngine::RadarEngine *re): Alarm(parent, AlarmType::ALARM_GZ, id),m_id(id)
{
    m_re = re;
    if(id == "GZ 1")
    {
        m_shown_key = RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ;
        m_enable_alarm_key = RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM;
        m_alarm_trhold_key = RadarEngine::NON_VOLATILE_GZ_NOTIF_THRESHOLD;
        m_confirmed_alarm_key = RadarEngine::VOLATILE_GZ_CONFIRMED;
        m_time_key = RadarEngine::VOLATILE_GZ_TIME;
        m_timeout_key = RadarEngine::NON_VOLATILE_GZ_TIMEOUT;
    }
    else if(id == "GZ 2")
    {
        m_shown_key = RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ1;
        m_enable_alarm_key = RadarEngine::NON_VOLATILE_GZ_ENABLE_ALARM1;
        m_alarm_trhold_key = RadarEngine::NON_VOLATILE_GZ_NOTIF_THRESHOLD1;
        m_confirmed_alarm_key = RadarEngine::VOLATILE_GZ_CONFIRMED1;
        m_time_key = RadarEngine::VOLATILE_GZ_TIME1;
        m_timeout_key = RadarEngine::NON_VOLATILE_GZ_TIMEOUT1;

    }
    init(1000);
}

void GZAlarm::checkAlarm()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarEngine::RadarConfig::getInstance("")->getConfig(m_enable_alarm_key).toBool();
    const bool gz_settings_confirmed = RadarEngine::RadarConfig::getInstance("")->getConfig(m_confirmed_alarm_key).toBool();
    const int gz_settings_notif_thr = RadarEngine::RadarConfig::getInstance("")->getConfig(m_alarm_trhold_key).toInt();
    const int gz_settings_time = RadarEngine::RadarConfig::getInstance("")->getConfig(m_time_key).toInt(); //s

    if(gz_settings_show)
    {
        if(gz_settings_enable_alarm)
        {
            qint64 now = QDateTime::currentSecsSinceEpoch();
            int bogey = m_re->guardZones[m_id]->GetBogeyCount();

            qDebug()<<Q_FUNC_INFO<<m_id<<gz_settings_notif_thr<<bogey<<gz_settings_time-now;

            if(bogey > gz_settings_notif_thr)
            {
                if(!gz_settings_confirmed || (gz_settings_confirmed && TIMED_OUT(now,(gz_settings_time))))
                {
                    qDebug()<<Q_FUNC_INFO<<m_id<<gz_settings_confirmed<<gz_settings_time;
                    if(TIMED_OUT(now,(gz_settings_time)))
                    {
                        qDebug()<<Q_FUNC_INFO<<m_id<<"timeout";

                        const int gz_settings_timeout = RadarEngine::RadarConfig::getInstance("")->getConfig(m_timeout_key).toInt(); //s
                        RadarEngine::RadarConfig::getInstance("")->setConfig(m_time_key,gz_settings_timeout+now);
                        RadarEngine::RadarConfig::getInstance("")->setConfig(m_confirmed_alarm_key,false);
                    }

                    SetCurrentMessage(QString("%1 alarm").arg(m_id));
                    emit SignalAlarmTriggered(GetId(), GetCurrentMessage());
                }
            }
        }
        else
        {
            if(m_re->guardZones[m_id]->GetBogeyCount()>0)
                m_re->guardZones[m_id]->ResetBogeys();

            RadarEngine::RadarConfig::getInstance("")->setConfig(m_confirmed_alarm_key,false);
        }
    }
    else
    {
        if(m_re->guardZones[m_id]->GetBogeyCount()>0)
            m_re->guardZones[m_id]->ResetBogeys();

        RadarEngine::RadarConfig::getInstance("")->setConfig(m_confirmed_alarm_key,false);
    }

}

bool GZAlarm::IsConfirmed()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarEngine::RadarConfig::getInstance("")->getConfig(m_enable_alarm_key).toBool();

    if(gz_settings_show && gz_settings_enable_alarm) return RadarEngine::RadarConfig::getInstance("")->getConfig(m_confirmed_alarm_key).toBool();
    else return false;
}

void GZAlarm::Confirm()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarEngine::RadarConfig::getInstance("")->getConfig(m_enable_alarm_key).toBool();

    if(gz_settings_show && gz_settings_enable_alarm)
    {
        const int now = static_cast<int>(QDateTime::currentSecsSinceEpoch());
        const int gz_settings_timeout = RadarEngine::RadarConfig::getInstance("")->getConfig(m_timeout_key).toInt(); //s
        const int gz_settings_time = gz_settings_timeout+now; //s

        RadarEngine::RadarConfig::getInstance("")->setConfig(m_time_key,gz_settings_time);
        RadarEngine::RadarConfig::getInstance("")->setConfig(m_confirmed_alarm_key,true);
    }
}

GZAlarm::~GZAlarm()
{
}
