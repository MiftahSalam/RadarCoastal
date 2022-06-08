#include "gzalarm.h"

QMap<uint, GZAlarm*> GZAlarm::m_GZAlarms{};

GZAlarm::GZAlarm(QObject *parent): Alarm(parent)
{
    m_re = RadarEngine::RadarEngine::getInstance();
}

void GZAlarm::checkAlarm()
{
    bool gz_settings_show = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    bool gz_settings_enable_alarm = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    bool gz_settings_confirmed = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_GZ_CONFIRMED).toBool();
    int gz_settings_notif_thr = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_NOTIF_THRESHOLD).toInt();
    int gz_settings_time = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_TIMEOUT).toInt();

    if(gz_settings_show)
    {
        if(gz_settings_enable_alarm)
        {
            qint64 now = QDateTime::currentSecsSinceEpoch();
            int bogey = m_re->guardZone->GetBogeyCount();
            qDebug()<<gz_settings_notif_thr<<bogey;
            if(bogey > gz_settings_notif_thr)
            {
                if(!gz_settings_confirmed || (gz_settings_confirmed && TIMED_OUT(now,gz_settings_time)))
                {
//                    qDebug()<<gz_settings.confirmed<<gz_settings.time;
                    if(TIMED_OUT(now,gz_settings_time))
                        RadarConfig::RadarConfig::getInstance("")->setConfig(RadarConfig::VOLATILE_GZ_CONFIRMED,false);

                    emit signal_alarmTriggered("Guarzone 1 alarm");
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

GZAlarm::~GZAlarm()
{
    m_GZAlarms.clear();
}

GZAlarm* GZAlarm::getInstance(uint id)
{
    qDebug()<<Q_FUNC_INFO;
    if(!m_GZAlarms.contains(id))
    {
        GZAlarm* alarm = new GZAlarm(nullptr);
        alarm->init(1000);
        m_GZAlarms.insert(id,alarm);
    }

    return m_GZAlarms.value(id);
}

