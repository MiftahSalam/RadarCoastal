#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "navigation_config.h"
//#include "src/shared/common/errors/err_open_file.h"

NavigationConfig* NavigationConfig::config = nullptr;

NavigationConfig::NavigationConfig()
    : BaseConfig()
{

}

quint8 NavigationConfig::getHeadingStatus() const
{
    return headingStatus;
}

void NavigationConfig::setHeadingStatus(quint8 newHeadingStatus)
{
    headingStatus = newHeadingStatus;
    changeConfig(NAV_STATUS_HEADING, newHeadingStatus);
}

bool NavigationConfig::getHeadingModeAuto() const
{
    return headingModeAuto;
}

void NavigationConfig::setHeadingModeAuto(bool newHeadingModeAuto)
{
    headingModeAuto = newHeadingModeAuto;
   changeConfig(NAV_MODE_HEADING, newHeadingModeAuto);
}

QString NavigationConfig::getMqttInternal() const
{
    return mqttInternal;
}

void NavigationConfig::setMqttInternal(const QString &newMqttInternal)
{
    mqttInternal = newMqttInternal;
    changeConfig(NAV_INTERNAL_NET, newMqttInternal);
}

QString NavigationConfig::getMqttSpasi() const
{
    return mqttSpasi;
}

void NavigationConfig::setGpsModeAuto(bool newGpsModeAuto)
{
    gpsModeAuto = newGpsModeAuto;
    changeConfig(NAV_MODE_GPS, newGpsModeAuto);
}

void NavigationConfig::setGpsStatus(quint8 newGpsStatus)
{
    gpsStatus = newGpsStatus;
    changeConfig(NAV_STATUS_GPS, newGpsStatus);
}

NavigationConfig::~NavigationConfig()
{

}

quint8 NavigationConfig::getGPSStatus() const
{
    return gpsStatus;
}

bool NavigationConfig::getGPSModeAuto() const
{
    return gpsModeAuto;
}

NavigationConfig* NavigationConfig::getInstance(const QString path) {
    qDebug()<<Q_FUNC_INFO<<"path"<<path;

    if(config == nullptr) {
        config = new NavigationConfig();
        config->setup(path);
    }

    return config;
}

void NavigationConfig::setup(const QString path)
{
    QSettings configFile(path,QSettings::IniFormat);

    gpsStatus = 0; //offline
    gpsModeAuto = configFile.value(NAV_MODE_GPS, true).toBool();
    headingModeAuto = configFile.value(NAV_MODE_HEADING, true).toBool();
    mqttSpasi = configFile.value(NAV_SPASI_NET, "mqtt;InOut;127.0.0.1:1883:user:pass:site_data").toString();
    mqttInternal = configFile.value(NAV_INTERNAL_NET, "mqtt;InOut;127.0.0.1:1883:gps").toString();
}

void NavigationConfig::save(const QString path)
{
    qDebug() << Q_FUNC_INFO << path;

    QSettings config(path, QSettings::IniFormat);

    config.setValue(NAV_MODE_GPS, gpsModeAuto);
    config.setValue(NAV_MODE_HEADING, headingModeAuto);
    config.setValue(NAV_INTERNAL_NET, mqttInternal);
}
