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

void ArpaSender::trigger_configChange(const QString key, const QVariant val)
{
    qDebug()<<Q_FUNC_INFO<<"key"<<key<<"val"<<val;
    if(key == RadarConfig::NON_VOLATILE_NAV_NET_CONFIG)
    {
        stream->setConfig(val.toString());
    }
}
