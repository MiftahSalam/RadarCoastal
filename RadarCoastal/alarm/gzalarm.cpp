#include "gzalarm.h"

GZAlarm::GZAlarm(QObject *parent, QString id): Alarm(parent, AlarmType::ALARM_GZ, id)
{
    m_re = RadarEngine::RadarEngine::getInstance();
    init(1000);
}

void GZAlarm::checkAlarm()
{
    const bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    const bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    const bool gz_settings_confirmed = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_GZ_CONFIRMED).toBool();
    const int gz_settings_notif_thr = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD).toInt();
    const int gz_settings_time = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_GZ_TIME).toInt(); //s

    if(gz_settings_show)
    {
        if(gz_settings_enable_alarm)
        {
            qint64 now = QDateTime::currentSecsSinceEpoch();
            int bogey = m_re->guardZone->GetBogeyCount();
            qDebug()<<gz_settings_notif_thr<<bogey<<gz_settings_time-now;
            if(bogey > gz_settings_notif_thr)
            {
                if(!gz_settings_confirmed || (gz_settings_confirmed && TIMED_OUT(now,(gz_settings_time))))
                {
//                    qDebug()<<gz_settings.confirmed<<gz_settings.time;
                    if(TIMED_OUT(now,(gz_settings_time)))
                        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_CONFIRMED,false);

                    emit signal_alarmTriggered(getId(), "Guarzone 1 alarm");
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
            if(m_re->guardZone->GetBogeyCount()>0)
                m_re->guardZone->ResetBogeys();
//            if(!dialAlarm->isHidden())
//            {
//                alarm->stop();
//                dialAlarm->hide();
//            }
            RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_CONFIRMED,false);
        }
    }
    else
    {
        if(m_re->guardZone->GetBogeyCount()>0)
            m_re->guardZone->ResetBogeys();
//        if(!dialAlarm->isHidden())
//        {
//            dialAlarm->hide();
//            alarm->stop();
//        }
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_CONFIRMED,false);
    }

}

void GZAlarm::confirm()
{
    bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();

    if(gz_settings_show && gz_settings_enable_alarm)
    {
        const int now = static_cast<int>(QDateTime::currentSecsSinceEpoch());
        const int gz_settings_timeout = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_TIMEOUT).toInt(); //s
        const int gz_settings_time = gz_settings_timeout+now; //s

        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_TIME,gz_settings_time);
        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_CONFIRMED,true);
    }
}

GZAlarm::~GZAlarm()
{
}
