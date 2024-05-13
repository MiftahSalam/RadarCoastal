#include <QDebug>
#include <QString>
#include <QFile>
#include <QSettings>

#include "navigation_config.h"
//#include "src/shared/common/errors/err_open_file.h"

const QString NAV_MODE_GPS = "nav/mode/gps";
const QString NAV_MODE_HEADING = "nav/mode/heading";
const QString NAV_SPASI_MQTT = "nav/spasi/mqtt";
const QString NAV_INTERNAL_MQTT = "nav/internal/mqtt";

NavigationConfig* NavigationConfig::config = nullptr;

NavigationConfig::NavigationConfig()
{

}

quint8 NavigationConfig::getHeadingStatus() const
{
    return headingStatus;
}

void NavigationConfig::setHeadingStatus(quint8 newHeadingStatus)
{
    headingStatus = newHeadingStatus;
}

bool NavigationConfig::getHeadingModeAuto() const
{
    return headingModeAuto;
}

void NavigationConfig::setHeadingModeAuto(bool newHeadingModeAuto)
{
    headingModeAuto = newHeadingModeAuto;
}

QString NavigationConfig::getMqttInternal() const
{
    return mqttInternal;
}

void NavigationConfig::setMqttInternal(const QString &newMqttInternal)
{
    mqttInternal = newMqttInternal;
}

QString NavigationConfig::getMqttSpasi() const
{
    return mqttSpasi;
}

void NavigationConfig::setGpsModeAuto(bool newGpsModeAuto)
{
    gpsModeAuto = newGpsModeAuto;
}

void NavigationConfig::setGpsStatus(quint8 newGpsStatus)
{
    gpsStatus = newGpsStatus;
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
        if (!QFile::exists(path)) {
//            throw ErrFileNotFound();
            qFatal("file not found: %s", path.toUtf8().constData());
        }

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
    mqttSpasi = configFile.value(NAV_SPASI_MQTT, "mqtt;InOut;127.0.0.1:1883:user:pass:site_data").toString();
    mqttInternal = configFile.value(NAV_SPASI_MQTT, "mqtt;InOut;127.0.0.1:1883:gps").toString();
}
