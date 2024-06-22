#ifndef HEARBEATRADARCONFIG_H
#define HEARBEATRADARCONFIG_H

#include <QString>

#include "base_config.h"

const QString HEARTBEAT_NET_PUBLIC = "HeartBeat/net/public";
const QString HEARTBEAT_MESSAGE = "HeartBeat/message";

class HearBeatRadarConfig : public BaseConfig
{
public:
    HearBeatRadarConfig(HearBeatRadarConfig &other) = delete;
    void operator=(const HearBeatRadarConfig &) = delete;
    ~HearBeatRadarConfig();

    static HearBeatRadarConfig *getInstance(const QString path);

    // BaseConfig interface
    void setup(const QString path) override;
    void save(QString path) override;

    QString getMqttPublic() const;

    QString getHbMessage() const;

protected:
    HearBeatRadarConfig();

private:
    static HearBeatRadarConfig *config;
    QString mqttPublic;
    QString hbMessage;
};

#endif // HEARBEATRADARCONFIG_H
