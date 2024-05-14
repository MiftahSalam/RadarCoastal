#ifndef NAVIGATIONCONFIG_H
#define NAVIGATIONCONFIG_H

#include <QString>

#include "base_config.h"

const QString NAV_MODE_GPS = "nav/mode/gps";
const QString NAV_MODE_HEADING = "nav/mode/heading";
const QString NAV_STATUS_GPS = "nav/status/gps";
const QString NAV_STATUS_HEADING = "nav/status/heading";
const QString NAV_SPASI_MQTT = "nav/spasi/mqtt";
const QString NAV_INTERNAL_MQTT = "nav/internal/mqtt";

class NavigationConfig: public BaseConfig
{
public:    
    NavigationConfig(NavigationConfig &other) = delete;
    void operator=(const NavigationConfig&) = delete;
    ~NavigationConfig();

    static NavigationConfig* getInstance(const QString path);

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

    quint8 getGPSStatus() const;
    void setGpsStatus(quint8 newGpsStatus);

    bool getGPSModeAuto() const;
    void setGpsModeAuto(bool newGpsModeAuto);

    QString getMqttSpasi() const;

    QString getMqttInternal() const;
    void setMqttInternal(const QString &newMqttInternal);

    bool getHeadingModeAuto() const;
    void setHeadingModeAuto(bool newHeadingModeAuto);

    quint8 getHeadingStatus() const;
    void setHeadingStatus(quint8 newHeadingStatus);

protected:
    NavigationConfig();

private:
    static NavigationConfig *config;
    QString mqttSpasi;
    QString mqttInternal;
    quint8 gpsStatus;
    quint8 headingStatus;
    bool gpsModeAuto;
    bool headingModeAuto;
};

#endif // NAVIGATIONCONFIG_H
