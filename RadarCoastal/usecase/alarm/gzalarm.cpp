#include "usecase/alarm/gzalarm.h"
#include "shared/config/applicationconfig.h"

#include <RadarEngine/radarconfig.h>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, GZAlarm)
#else
#include <QDebug>
#endif

GZAlarm::GZAlarm(QObject *parent, QString id, RadarEngine::RadarEngine *re): Alarm(parent, AlarmType::ALARM_GZ, id),m_id(id)
{
    m_re = re;
    if(m_re == nullptr)
    {
#ifdef USE_LOG4QT
    logger()->fatal()<<Q_FUNC_INFO<<"m_re is null";
#else
        qDebug()<<Q_FUNC_INFO<<"m_re is null";
        exit(-1);
#endif
    }

    gzConfig = ApplicationConfig::getInstance()->getGzConfig();

    if(id == "GZ 1")
    {
        m_shown_key = RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ;
    }
    else if(id == "GZ 2")
    {
        m_shown_key = RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ1;

    }
    init(1000);
}

void GZAlarm::checkAlarm()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    bool gz_settings_enable_alarm;
    bool gz_settings_confirmed;
    int gz_settings_notif_thr;
    int gz_settings_time; //s
    if(m_id == "GZ 1")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm();
        gz_settings_confirmed = gzConfig->getConfirmed();
        gz_settings_notif_thr = gzConfig->getNotifThreshold();
        gz_settings_time = gzConfig->getTime();
    }
    else if(m_id == "GZ 2")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm1();
        gz_settings_confirmed = gzConfig->getConfirmed1();
        gz_settings_notif_thr = gzConfig->getNotifThreshold1();
        gz_settings_time = gzConfig->getTime1();
    }

    if(gz_settings_show)
    {
        if(gz_settings_enable_alarm)
        {
            qint64 now = QDateTime::currentSecsSinceEpoch();
            int bogey = m_re->guardZones[m_id]->GetBogeyCount();

#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<" id: "<<m_id<<", notif: "<<gz_settings_notif_thr<<", bogey: "<<bogey<<", timeout: "<<gz_settings_time-now;
#else
            qDebug()<<Q_FUNC_INFO<<m_id<<gz_settings_notif_thr<<bogey<<gz_settings_time-now;
#endif

            if(bogey > gz_settings_notif_thr)
            {
                if(!gz_settings_confirmed || (gz_settings_confirmed && TIMED_OUT(now,(gz_settings_time))))
                {
#ifdef USE_LOG4QT
                    logger()->debug()<<Q_FUNC_INFO<<" id: "<<m_id<<", confirmed: "<<gz_settings_confirmed<<", time: "<<gz_settings_time;
#else
                    qDebug()<<Q_FUNC_INFO<<m_id<<gz_settings_confirmed<<gz_settings_time;
#endif
                    if(TIMED_OUT(now,(gz_settings_time)))
                    {
#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<" id: "<<m_id<<" timeout";
#else
                        qDebug()<<Q_FUNC_INFO<<m_id<<"timeout";
#endif
                        int gz_settings_timeout; //s
                        if(m_id == "GZ 1")
                        {
                            gz_settings_timeout = gzConfig->getTimeout(); //s
                            gzConfig->setTimeout(gz_settings_timeout+now);
                            gzConfig->setConfirmed(false);
                        }
                        else if(m_id == "GZ 2")
                        {
                            gz_settings_timeout = gzConfig->getTimeout1(); //s
                            gzConfig->setTimeout1(gz_settings_timeout+now);
                            gzConfig->setConfirmed1(false);
                        }
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

            if(m_id == "GZ 1")
            {
                gzConfig->setConfirmed(false);
            }
            else if(m_id == "GZ 2")
            {
                gzConfig->setConfirmed1(false);
            }
        }
    }
    else
    {
        if(m_re->guardZones[m_id]->GetBogeyCount()>0)
            m_re->guardZones[m_id]->ResetBogeys();

        if(m_id == "GZ 1")
        {
            gzConfig->setConfirmed(false);
        }
        else if(m_id == "GZ 2")
        {
            gzConfig->setConfirmed1(false);
        }
    }
}

bool GZAlarm::IsConfirmed()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    bool gz_settings_enable_alarm;
    if(m_id == "GZ 1")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm();
        if(gz_settings_show && gz_settings_enable_alarm) return gzConfig->getConfirmed();
        else return false;
    }
    else if(m_id == "GZ 2")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm1();
        if(gz_settings_show && gz_settings_enable_alarm) return gzConfig->getConfirmed1();
        else return false;
    }

    return false;
}

void GZAlarm::Confirm()
{
    const bool gz_settings_show = RadarEngine::RadarConfig::getInstance("")->getConfig(m_shown_key).toBool();
    bool gz_settings_enable_alarm;
    const int now = static_cast<int>(QDateTime::currentSecsSinceEpoch());

    if(m_id == "GZ 1")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm();
        if(gz_settings_show && gz_settings_enable_alarm)
        {
            const int gz_settings_timeout = gzConfig->getTimeout(); //s
            const int gz_settings_time = gz_settings_timeout+now; //s

            gzConfig->setTime(gz_settings_time);
            gzConfig->setConfirmed(true);
        }
    }
    else if(m_id == "GZ 2")
    {
        gz_settings_enable_alarm = gzConfig->getEnableAlarm1();
        if(gz_settings_show && gz_settings_enable_alarm)
        {
            const int gz_settings_timeout = gzConfig->getTimeout1(); //s
            const int gz_settings_time = gz_settings_timeout+now; //s

            gzConfig->setTime1(gz_settings_time);
            gzConfig->setConfirmed1(true);
        }
    }
}

GZAlarm::~GZAlarm()
{
}
