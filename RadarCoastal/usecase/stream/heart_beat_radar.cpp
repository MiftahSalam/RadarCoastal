#include "heart_beat_radar.h"
#include "shared/config/applicationconfig.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, HeartBeatRadar)
#else
#include <QDebug>
#endif

HeartBeatRadar::HeartBeatRadar(QObject *parent)
    : QObject(parent), m_stream{nullptr}

{
    hbRadarConfig = ApplicationConfig::getInstance()->getHbConfig();

    initConfigMqttPublic();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HeartBeatRadar::triggerTimerTimeout);

    timer->start(timeout * 1000);
}

void HeartBeatRadar::triggerTimerTimeout()
{
    reconnect();
    sendHeartBeat();
}

void HeartBeatRadar::setTimeout(const QString timeoutCfg)
{
    bool ok;
    timeout = timeoutCfg.toInt(&ok);
    if (!ok) {
        qWarning()<<Q_FUNC_INFO<<" invalid heart beat timeout config: "<<timeoutCfg<<". will use default 10s";
        timeout = 10;
    }
}

void HeartBeatRadar::reconnect()
{
    m_stream->Reconnect();

}

void HeartBeatRadar::sendHeartBeat()
{
    QString mq_data = m_topic_hb + MQTT_MESSAGE_SEPARATOR + hbRadarConfig->getHbMessage();

#ifdef USE_LOG4QT
    logger()->debug()<<Q_FUNC_INFO<<" mq_data: "<<mq_data;
#else
    qDebug()<<Q_FUNC_INFO<<" mq_data: "<<mq_data;
#endif

    if (m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream->Reconnect();
    else
        m_stream->SendData(mq_data);
}

void HeartBeatRadar::initConfigMqttPublic()
{
    QString config_str = hbRadarConfig->getMqttPublic();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 6)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<< "invalid config mqtt public: "<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt public"<<config_str;
        exit(1);
#endif
    }

    setTimeout(config_str_list.at(5));

    m_topic_hb = config_str_list.at(4);
    if (!m_stream) {
        m_stream = new Stream(this,config_str);
    }
}
