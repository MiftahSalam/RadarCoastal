#include "hear_beat_radar_config.h"

#include <QDebug>
#include <QSettings>

HearBeatRadarConfig* HearBeatRadarConfig::config = nullptr;

HearBeatRadarConfig::HearBeatRadarConfig()
    : BaseConfig()
{

}

QString HearBeatRadarConfig::getHbMessage() const
{
    return hbMessage;
}

QString HearBeatRadarConfig::getMqttPublic() const
{
    qDebug()<<Q_FUNC_INFO<<" mqttPublic: "<<mqttPublic;
    return mqttPublic;
}

HearBeatRadarConfig::~HearBeatRadarConfig()
{

}

HearBeatRadarConfig *HearBeatRadarConfig::getInstance(const QString path)
{
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        config = new HearBeatRadarConfig();
        config->setup(path);
    }

    return config;
}

void HearBeatRadarConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    mqttPublic = configFile.value(HEARTBEAT_NET_PUBLIC, "mqtt;InOut;127.0.0.1:1883:user:pass:radar_request:10").toString();
    hbMessage = configFile.value(HEARTBEAT_MESSAGE, "radar_request").toString();

    qDebug()<<Q_FUNC_INFO<<" mqttPublic: "<<mqttPublic;
}

void HearBeatRadarConfig::save(QString path)
{
    qDebug() << Q_FUNC_INFO << path;

    QSettings config(path, QSettings::IniFormat);

    config.setValue(HEARTBEAT_NET_PUBLIC, mqttPublic);
    config.setValue(HEARTBEAT_MESSAGE, hbMessage);
}
