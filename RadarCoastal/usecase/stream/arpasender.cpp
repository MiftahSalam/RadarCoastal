#include "arpasender.h"
#include "qjsonarray.h"
#include "shared/utils.h"
#include "shared/config/applicationconfig.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, ArpaSender)
#else
#include <QDebug>
#endif

ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}, m_stream_mqtt_private{nullptr}, m_stream_mqtt_public{nullptr}
{
    arpaConfig = ApplicationConfig::getInstance()->getArpaConfig();

#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif

    initConfigMqttPrivate();
    initConfigMqttPublic();
}

void ArpaSender::initConfigMqttPublic()
{
    QString config_str = arpaConfig->getMqttPublic();
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

    m_topic_public = config_str_list.at(4);
    if (!m_stream_mqtt_public) {
        m_stream_mqtt_public = new Stream(this,config_str);
    }
}

void ArpaSender::initConfigMqttPrivate()
{
    QString config_str = arpaConfig->getMqttInternal();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal() << Q_FUNC_INFO << " invalid config private: " << config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt private:"<<config_str;
        exit(1);
#endif
    }

    m_topic_private = config_str_list.last();
    if (!m_stream_mqtt_private) {
        m_stream_mqtt_private = new Stream(this,config_str);
        arpaConfig->attach(this);
    }

 }

void ArpaSender::SendManyData(QList<TrackModel *> data)
{
    foreach (auto m, data)
    {
        QPointF gpsCorrection = Utils::GpsAbsolute(m->lat, m->lon);

        m->lat = gpsCorrection.y();
        m->lon = gpsCorrection.x();

        switch (Utils::unit)
        {
        case 1:
            m->rng /= KM_TO_NM;
            break;
        default:
            break;
        }
        m->rng *= 1000.;
    }

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder *>(new ArpaSenderDecoderJson(data));

    sendMqtt(decoder);

    delete decoder;
}

void ArpaSender::SendOneData(TrackModel data)
{
    QPointF gpsCorrection = Utils::GpsAbsolute(data.lat, data.lon);

    data.lat = gpsCorrection.y();
    data.lon = gpsCorrection.x();
    switch (Utils::unit)
    {
    case 1:
        data.rng /= KM_TO_NM;
        break;
    default:
        break;
    }
    data.rng *= 1000.;

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder *>(new ArpaSenderDecoderJson(data));

    sendMqtt(decoder);

    delete decoder;
}

void ArpaSender::SendOneData(long long ts,
                             int id,
                             double lat,
                             double lon,
                             double alt,
                             double rng,
                             double brn,
                             double spd,
                             double crs)
{
    QPointF gpsCorrection = Utils::GpsAbsolute(lat, lon);

    lat = gpsCorrection.y();
    lon = gpsCorrection.x();
    switch (Utils::unit)
    {
    case 1:
        rng /= KM_TO_NM;
        break;
    default:
        break;
    }
    rng *= 1000.;

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder *>(new ArpaSenderDecoderJson(
                                                                       ts,
                                                                       id,
                                                                       lat,
                                                                       lon,
                                                                       alt,
                                                                       rng,
                                                                       brn,
                                                                       spd,
                                                                       crs));
    sendMqtt(decoder);
    delete decoder;
}

void ArpaSender::sendMqtt(ArpaSenderDecoder *decoder)
{
    QString mq_data_pub = m_topic_public + MQTT_MESSAGE_SEPARATOR + decoder->decode();
    QString mq_data_prv = m_topic_private + MQTT_MESSAGE_SEPARATOR + decoder->decode();

#ifdef USE_LOG4QT
        logger()->debug()<<Q_FUNC_INFO<<" mq_data_pub: "<<mq_data_pub;
        logger()->debug()<<Q_FUNC_INFO<<" mq_data_prv: "<<mq_data_prv;
#else
    qDebug()<<Q_FUNC_INFO<<" mq_data_pub: "<<mq_data_pub;
    qDebug()<<Q_FUNC_INFO<<" mq_data_prv: "<<mq_data_prv;
#endif

    if (m_stream_mqtt_public->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream_mqtt_public->Reconnect();
    else
        m_stream_mqtt_public->SendData(mq_data_pub);

    if (m_stream_mqtt_private->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)

        m_stream_mqtt_private->Reconnect();
    else
        m_stream_mqtt_private->SendData(mq_data_prv);
}

void ArpaSender::configChange(const QString key, const QVariant val)
{
    if(key == ARPA_NET_INTERNAL)
    {
        initConfigMqttPrivate();
        m_stream_mqtt_private->SetConfig(val.toString());
    }
}
