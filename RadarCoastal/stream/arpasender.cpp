#include "arpasender.h"

ArpaSender::ArpaSender(QObject *parent)
    : QObject{parent}
{
    qDebug()<<Q_FUNC_INFO;
    QString config_str = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_ARPA_NET_CONFIG).toString();
    QStringList config_str_list = config_str.split(":");

    if(config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<config_str;
        exit(1);
    }

    topic = config_str_list.last();

    stream = new Stream(this,config_str);
    connect(RadarConfig::RadarConfig::getInstance(""),&RadarConfig::RadarConfig::configValueChange,
            this,&ArpaSender::trigger_configChange);

}

void ArpaSender::sendData(int id,
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

    QString id_str = QString::number(id);
    QString lat_str = QString::number(lat,'f',5);
    QString lon_str = QString::number(lon,'f',5);
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

    mq_data = topic+":"+id_str+"#"+rng_str+"#"+brn_str+"#"+lat_str+"#"+lon_str+"#"+spd_str+"#"+crs_str;

    if(stream->getStreamStatus() == DeviceWrapper::NOT_AVAIL) stream->reconnect();
    else stream->sendData(mq_data);

}
void ArpaSender::trigger_configChange(const QString key, const QVariant val)
{
//    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarConfig::NON_VOLATILE_ARPA_NET_CONFIG)
    {
        stream->setConfig(val.toString());
    }
}
