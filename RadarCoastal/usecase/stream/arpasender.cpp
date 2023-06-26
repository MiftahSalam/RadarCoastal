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
    QString lat_str = QString::number(gpsCorrection.y(),'f',5);
    QString lon_str = QString::number(gpsCorrection.x(),'f',5);
    QString rng_str = QString::number(rng,'f',1);
    QString brn_str = QString::number(brn,'f',1);
    QString spd_str = QString::number(spd,'f',1);
    QString crs_str = QString::number(crs,'f',1);
    QString alt_str =  QString::number(alt,'f',1);

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
