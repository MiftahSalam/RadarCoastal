#include "gzalarm.h"

GZAlarm::GZAlarm(QObject *parent, QString id): Alarm(parent, AlarmType::ALARM_GZ, id),m_id(id)
{
    m_re = RadarEngine::RadarEngine::getInstance();
    if(id == "GZ 1")
    {
        shown_key = RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ;
        enable_alarm_key = RadarConfig::NON_VOLATILE_GZ_ENABLE_ALARM;
        alarm_trhold_key = RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD;
        confirmed_alarm_key = RadarConfig::VOLATILE_GZ_CONFIRMED;
        time_key = RadarConfig::VOLATILE_GZ_TIME;
        timeout_key = RadarConfig::NON_VOLATILE_GZ_TIMEOUT;
    }
    else if(id == "GZ 2")
    {
        shown_key = RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ1;
        enable_alarm_key = RadarConfig::NON_VOLATILE_GZ_ENABLE_ALARM1;
        alarm_trhold_key = RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD1;
        confirmed_alarm_key = RadarConfig::VOLATILE_GZ_CONFIRMED1;
        time_key = RadarConfig::VOLATILE_GZ_TIME1;
        timeout_key = RadarConfig::NON_VOLATILE_GZ_TIMEOUT1;

    }
    init(1000);
}

void GZAlarm::checkAlarm()
{
    const bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(enable_alarm_key).toBool();
    const bool gz_settings_confirmed = RadarConfig::RadarConfig::getInstance("")->getConfig(confirmed_alarm_key).toBool();
    const int gz_settings_notif_thr = RadarConfig::RadarConfig::getInstance("")->getConfig(alarm_trhold_key).toInt();
    const int gz_settings_time = RadarConfig::RadarConfig::getInstance("")->getConfig(time_key).toInt(); //s

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
                        RadarConfig::RadarConfig::getInstance("")->setConfig(confirmed_alarm_key,false);
                    }

                    setCurrentMessage(QString("%1 alarm").arg(m_id));
                    emit signal_alarmTriggered(getId(), getCurrentMessage());
//                    char d=(char)(7);
//                    printf("%c\n",d);
//                    alarm->play();

//                    dialAlarm->move((width()/2)-(dialAlarm->width()/2),(height()/2)-(dialAlarm->height()/2));
//                    dialAlarm->setSpotCount(bogey);
                }
//                if((gz_settings.confirmed && !TIMED_OUT(now,gz_settings.time)))
//                    if(!dialAlarm->isHidden())
//                    {
//                        dialAlarm->hide();
//                        alarm->stop();
//                    }
            }
//            else
//            {
//                if(!dialAlarm->isHidden())
//                {
//                    alarm->stop();
//                    dialAlarm->hide();
//                }
//            }
        }
        else
        {
            if(m_re->guardZones[m_id]->GetBogeyCount()>0)
                m_re->guardZones[m_id]->ResetBogeys();
//            if(!dialAlarm->isHidden())
//            {
//                alarm->stop();
//                dialAlarm->hide();
//            }
            RadarConfig::RadarConfig::getInstance("")->setConfig(confirmed_alarm_key,false);
        }
    }
    else
    {
        if(m_re->guardZones[m_id]->GetBogeyCount()>0)
            m_re->guardZones[m_id]->ResetBogeys();
//        if(!dialAlarm->isHidden())
//        {
//            dialAlarm->hide();
//            alarm->stop();
//        }
        RadarConfig::RadarConfig::getInstance("")->setConfig(confirmed_alarm_key,false);
    }

}

bool GZAlarm::isConfirmed()
{
    const bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(enable_alarm_key).toBool();

    if(gz_settings_show && gz_settings_enable_alarm) return RadarConfig::RadarConfig::getInstance("")->getConfig(confirmed_alarm_key).toBool();
    else return false;
}

void GZAlarm::confirm()
{
    const bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(shown_key).toBool();
    const bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(enable_alarm_key).toBool();

    if(gz_settings_show && gz_settings_enable_alarm)
    {
        const int now = static_cast<int>(QDateTime::currentSecsSinceEpoch());
        const int gz_settings_timeout = RadarConfig::RadarConfig::getInstance("")->getConfig(timeout_key).toInt(); //s
        const int gz_settings_time = gz_settings_timeout+now; //s

        RadarConfig::RadarConfig::getInstance("")->setConfig(time_key,gz_settings_time);
        RadarConfig::RadarConfig::getInstance("")->setConfig(confirmed_alarm_key,true);
    }
}

GZAlarm::~GZAlarm()
{
}
