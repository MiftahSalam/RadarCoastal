#ifndef ECHOSENDERCONFIG_H
#define ECHOSENDERCONFIG_H

#include "base_config.h"

const QString ECHO_SPASI_NET = "Echo/spasi/net";

class EchoSenderConfig: public BaseConfig
{
public:
    EchoSenderConfig(EchoSenderConfig &other) = delete;
    void operator=(const EchoSenderConfig&) = delete;
    ~EchoSenderConfig();

    static EchoSenderConfig* getInstance(const QString path);

    QString getMqttSpasi() const;

    // BaseConfig interface
    void setup(const QString path) override;
    void save(const QString path) override;

protected:
    EchoSenderConfig();

private:
    static EchoSenderConfig *config;
    QString mqttSpasi;

};

#endif // ECHOSENDERCONFIG_H
