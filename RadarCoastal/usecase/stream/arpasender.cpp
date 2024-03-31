#include "arpasender.h"
#include "qjsonarray.h"
#include "shared/utils.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, ArpaSender)
#else
#include <QDebug>
#endif

ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}, m_stream{nullptr}
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif

    initConfig();

    connect(RadarEngine::RadarConfig::getInstance(""), &RadarEngine::RadarConfig::configValueChange,
            this, &ArpaSender::triggerConfigChange);
}

void ArpaSender::initConfig()
{
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if (config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal() << Q_FUNC_INFO << " invalid config " << config_str;
#else
        qDebug() << Q_FUNC_INFO << "invalid config" << config_str;
        exit(1);
#endif
    }

    m_topic = config_str_list.last();
    if (!m_stream) {
        m_stream = new Stream(this, config_str);
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
    QString mq_data = m_topic + MQTT_MESSAGE_SEPARATOR + decoder->decode();

    qDebug()<<Q_FUNC_INFO<<" mq_data: "<<mq_data;

    if (m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL)
        m_stream->Reconnect();
    else
        m_stream->SendData(mq_data);
}

void ArpaSender::triggerConfigChange(const QString key, const QVariant val)
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO << "key" << key << "val" << val.toString();
#endif
    if (key == RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        initConfig();
        m_stream->SetConfig(val.toString());
    }
}
