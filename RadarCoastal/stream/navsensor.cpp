#include "navsensor.h"

NavSensor::NavSensor(QObject *parent) : QObject(parent)
{
    qDebug()<<Q_FUNC_INFO;
    QString nav_config_str = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_NET_CONFIG).toString();
    QStringList nav_config_str_list = nav_config_str.split(":");

    if(nav_config_str_list.size() != 3)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config"<<nav_config_str;
        exit(1);
    }

    topic = nav_config_str_list.last();

    stream = new Stream(this,nav_config_str);
    connect(stream,&Stream::signal_receiveData,this,&NavSensor::trigger_receivedData);
}

void NavSensor::trigger_receivedData(const QString data)
{
    stream->sendData("radar:tespublish");
    qDebug()<<Q_FUNC_INFO<<data;
}
