#include "arpasender.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "shared/utils.h"

ArpaSenderDecoder::ArpaSenderDecoder(int id,
                                     double lat,
                                     double lon,
                                     double alt,
                                     double rng,
                                     double brn,
                                     double spd,
                                     double crs)
{
    m_data.id = id;
    m_data.lat = lat;
    m_data.lon = lon;
    m_data.alt = alt;
    m_data.rng = rng;
    m_data.brn = brn;
    m_data.spd = spd;
    m_data.crs = crs;
}

ArpaSenderDecoder::ArpaSenderDecoder(TrackModel data)
{
    m_data.id = data.id;
    m_data.lat = data.lat;
    m_data.lon = data.lon;
    m_data.alt = data.alt;
    m_data.rng = data.rng;
    m_data.brn = data.brn;
    m_data.spd = data.spd;
    m_data.crs = data.crs;
}

ArpaSenderDecoderJson::ArpaSenderDecoderJson(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(id, lat, lon, alt, rng, brn, spd, crs)
{}

ArpaSenderDecoderJson::ArpaSenderDecoderJson(TrackModel data): ArpaSenderDecoder(data)
{}

QString ArpaSenderDecoderJson::decode()
{
    QJsonObject obj;

    obj["id"] = m_data.id;
    obj["lat"] = m_data.lat;
    obj["lon"] = m_data.lon;
    obj["alt"] = m_data.alt;
    obj["rng"] = m_data.rng;
    obj["brn"] = m_data.brn;
    obj["spd"] = m_data.spd;
    obj["crs"] = m_data.crs;

    QJsonDocument doc = QJsonDocument(obj);

    return QString(doc.toJson(QJsonDocument::Compact));
}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(
        int id,
        double lat,
        double lon,
        double alt,
        double rng,
        double brn,
        double spd,
        double crs) : ArpaSenderDecoder(id, lat, lon, alt, rng, brn, spd, crs)
{}

ArpaSenderDecoderNMEA::ArpaSenderDecoderNMEA(TrackModel data): ArpaSenderDecoder(data)
{}

QString ArpaSenderDecoderNMEA::decode()
{
    QString id_str = QString::number(m_data.id);
    QString lat_str = QString::number(m_data.lat,'f',Utils::FIVE_PRECISION);
    QString lon_str = QString::number(m_data.lon,'f',Utils::FIVE_PRECISION);
    QString rng_str = QString::number(m_data.rng,'f',Utils::ONE_PRECISION);
    QString brn_str = QString::number(m_data.brn,'f',Utils::ONE_PRECISION);
    QString spd_str = QString::number(m_data.spd,'f',Utils::ONE_PRECISION);
    QString crs_str = QString::number(m_data.crs,'f',Utils::ONE_PRECISION);
//    QString alt_str =  QString::number(m_data.alt,'f',Utils::ONE_PRECISION);

    //    lat_str.replace(".",",");
    //    lon_str.replace(".",",");
    //    alt_str.replace(".",",");
    //    spd_str.replace(".",",");
    //    crs_str.replace(".",",");

    return id_str+"#"+rng_str+"#"+brn_str+"#"+lat_str+"#"+lon_str+"#"+spd_str+"#"+crs_str;
}

ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}
{
    qDebug()<<Q_FUNC_INFO;
    QString config_str = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<config_str;
        exit(1);
    }

    m_topic = config_str_list.last();
    m_stream = new Stream(this,config_str);
    connect(RadarEngine::RadarConfig::getInstance(""),&RadarEngine::RadarConfig::configValueChange,
            this,&ArpaSender::triggerConfigChange);

}

void ArpaSender::SendData(TrackModel data)
{
    QPointF gpsCorrection = Utils::GpsAbsolute(data.lat,data.lon);

    data.lat = gpsCorrection.y();
    data.lon = gpsCorrection.x();

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(data));
    QString mq_data = m_topic+MQTT_MESSAGE_SEPARATOR+decoder->decode();

    if(m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream->Reconnect();
    else m_stream->SendData(mq_data);

    delete decoder;
}

void ArpaSender::SendData(int id,
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

    ArpaSenderDecoder *decoder = dynamic_cast<ArpaSenderDecoder*>(new ArpaSenderDecoderJson(
                                                                      id,
                                                                      lat,
                                                                      lon,
                                                                      alt,
                                                                      rng,
                                                                      brn,
                                                                      spd,
                                                                      crs));
    QString mq_data = m_topic+MQTT_MESSAGE_SEPARATOR+decoder->decode();

    if(m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream->Reconnect();
    else m_stream->SendData(mq_data);

    delete decoder;
}

void ArpaSender::triggerConfigChange(const QString key, const QVariant val)
{
    //    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        m_stream->SetConfig(val.toString());
    }
}
