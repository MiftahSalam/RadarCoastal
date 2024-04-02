#include "arpasender.h"
#include "baseresponsejson.h"

#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "shared/utils.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, ArpaSender)
#else
#include <QDebug>
#endif


ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}, m_stream_mqtt{nullptr}, m_stream_mqtt_spasi{nullptr}
{
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
#else
    qDebug()<<Q_FUNC_INFO;
#endif

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");

    initConfigMqtt();
    initConfigMqttSpasi();
}

void ArpaSender::sendMqtt(ArpaSenderDecoder *decoder)
{
    QString mq_data = m_topic+MQTT_MESSAGE_SEPARATOR+decoder->decode();

    if(m_stream_mqtt->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt->Reconnect();
    else m_stream_mqtt->SendData(mq_data);
}

void ArpaSender::sendMqttSpasi(ArpaSenderDecoder *decoder)
{
    ArpaSenderDecoderJson *decoderJson = dynamic_cast<ArpaSenderDecoderJson*>(decoder);
    QJsonDocument doc;
    auto decoderDoc = decoderJson->decodeJsonDoc();

    if(decoderDoc.isArray())
    {
        QJsonArray array(decoderDoc.array());
        BaseResponseJson<QJsonArray> resp(0, "ok", &array);
        doc = QJsonDocument(resp.build());
    }
    else if(decoderDoc.isObject())
    {
        QJsonObject obj(decoderDoc.object());
        BaseResponseJson<QJsonObject> resp(0, "ok", &obj);
        doc = QJsonDocument(resp.build());
    }

    QString mq_data = m_topic_spasi+MQTT_MESSAGE_SEPARATOR+doc.toJson(QJsonDocument::Compact);

    if(m_stream_mqtt_spasi->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt_spasi->Reconnect();
    m_stream_mqtt_spasi->SendData(mq_data);
}

void ArpaSender::SendManyData(QList<TrackModel *> data)
{
    foreach (auto m, data) {
        QPointF gpsCorrection = Utils::GpsAbsolute(m->lat,m->lon);

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

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(data));
    sendMqtt(decoder);
    sendMqttSpasi(decoder);

    delete decoder;
}

void ArpaSender::SendOneData(TrackModel data)
{
    QPointF gpsCorrection = Utils::GpsAbsolute(data.lat,data.lon);

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

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(data));
    sendMqtt(decoder);
    sendMqttSpasi(decoder);

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
                             double crs
                             )
{
    QPointF gpsCorrection = Utils::GpsAbsolute(lat,lon);

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

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(
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
    sendMqttSpasi(decoder);

    delete decoder;
}

void ArpaSender::Reconnect()
{
    if(m_stream_mqtt->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt->Reconnect();
    if(m_stream_mqtt_spasi->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream_mqtt_spasi->Reconnect();
}

void ArpaSender::initConfigMqttSpasi()
{
    QString config_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG_SPASI).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 6)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<<"invalid config mqtt spasi"<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt spasi"<<config_str;
        exit(1);
#endif
    }

    m_topic_spasi = config_str_list.at(4);
    if (!m_stream_mqtt_spasi) {
        m_stream_mqtt_spasi = new Stream(this,config_str);
    }
}

void ArpaSender::initConfigMqtt()
{
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
#ifdef USE_LOG4QT
        logger()->fatal()<<Q_FUNC_INFO<<"invalid config mqtt"<<config_str;
#else
        qDebug()<<Q_FUNC_INFO<<"invalid config mqtt"<<config_str;
        exit(1);
#endif
    }

    m_topic = config_str_list.last();
    if (!m_stream_mqtt) {
        m_stream_mqtt = new Stream(this,config_str);
        connect(RadarEngine::RadarConfig::getInstance(""),&RadarEngine::RadarConfig::configValueChange,
                this,&ArpaSender::triggerConfigChange);
    }
}

void ArpaSender::triggerConfigChange(const QString key, const QVariant val)
{
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val.toString();
#endif
    if(key == RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        initConfigMqtt();
        m_stream_mqtt->SetConfig(val.toString());
    }
}
