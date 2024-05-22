#ifndef ARPACONFIG_H
#define ARPACONFIG_H

#include "base_config.h"

const QString ARPA_PUBLIC_MQTT = "arpa/public/mqtt";
const QString ARPA_INTERNAL_MQTT = "arpa/internal/mqtt";

class ArpaConfig: public BaseConfig
{
public:    
    ArpaConfig(ArpaConfig &other) = delete;
    void operator=(const ArpaConfig&) = delete;
    ~ArpaConfig();

    static ArpaConfig* getInstance(const QString path);

    QString getMqttPublic() const;

    QString getMqttInternal() const;
    void setMqttInternal(const QString &newMqttInternal);

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

protected:
    ArpaConfig();

private:
    static ArpaConfig *config;
    QString mqttPublic;
    QString mqttInternal;

};

#endif // ARPACONFIG_H
