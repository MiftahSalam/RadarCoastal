#include "arpasender.h"
#include "shared/utils.h"

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

void ArpaSender::SendData(const TrackModel &data)
{
    QString mq_data;
    QPointF gpsCorrection = Utils::GpsAbsolute(data.lat,data.lon);

    QString id_str = QString::number(data.id);
    QString lat_str = QString::number(gpsCorrection.y(),'f',Utils::FIVE_PRECISION);
    QString lon_str = QString::number(gpsCorrection.x(),'f',Utils::FIVE_PRECISION);
    QString rng_str = QString::number(data.rng,'f',Utils::ONE_PRECISION);
    QString brn_str = QString::number(data.brn,'f',Utils::ONE_PRECISION);
    QString spd_str = QString::number(data.spd,'f',Utils::ONE_PRECISION);
    QString crs_str = QString::number(data.crs,'f',Utils::ONE_PRECISION);
    QString alt_str =  QString::number(data.alt,'f',Utils::ONE_PRECISION);

//    lat_str.replace(".",",");
//    lon_str.replace(".",",");
//    alt_str.replace(".",",");
//    spd_str.replace(".",",");
//    crs_str.replace(".",",");

    mq_data = m_topic+":"+id_str+"#"+rng_str+"#"+brn_str+"#"+lat_str+"#"+lon_str+"#"+spd_str+"#"+crs_str;

    if(m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream->Reconnect();
    else m_stream->SendData(mq_data);

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
    QString mq_data;
    QPointF gpsCorrection = Utils::GpsAbsolute(lat,lon);

    QString id_str = QString::number(id);
    QString lat_str = QString::number(gpsCorrection.y(),'f',Utils::FIVE_PRECISION);
    QString lon_str = QString::number(gpsCorrection.x(),'f',Utils::FIVE_PRECISION);
    QString rng_str = QString::number(rng,'f',Utils::ONE_PRECISION);
    QString brn_str = QString::number(brn,'f',Utils::ONE_PRECISION);
    QString spd_str = QString::number(spd,'f',Utils::ONE_PRECISION);
    QString crs_str = QString::number(crs,'f',Utils::ONE_PRECISION);
    QString alt_str =  QString::number(alt,'f',Utils::ONE_PRECISION);

//    lat_str.replace(".",",");
//    lon_str.replace(".",",");
//    alt_str.replace(".",",");
//    spd_str.replace(".",",");
//    crs_str.replace(".",",");

    mq_data = m_topic+":"+id_str+"#"+rng_str+"#"+brn_str+"#"+lat_str+"#"+lon_str+"#"+spd_str+"#"+crs_str;

    if(m_stream->GetStreamStatus() == DeviceWrapper::NOT_AVAIL) m_stream->Reconnect();
    else m_stream->SendData(mq_data);

}
void ArpaSender::triggerConfigChange(const QString key, const QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        m_stream->SetConfig(val.toString());
    }
}
