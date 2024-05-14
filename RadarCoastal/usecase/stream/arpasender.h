#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <RadarEngine/radarconfig.h>

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"
#include "arpa_sender_model.h"
#include "shared/config/arpa_config.h"

class ArpaSender : public QObject, public ConfigListener
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void SendManyData(QList<TrackModel*> data);
    void SendOneData(TrackModel data);
    void SendOneData(
            long long ts,
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
    void Reconnect();

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    ArpaConfig *arpaConfig;
    Stream *m_stream_mqtt;
    Stream *m_stream_mqtt_spasi;

    QString m_topic, m_topic_spasi;

    void initConfigMqttSpasi();
    void initConfigMqtt();
    void sendMqtt(ArpaSenderDecoder* decoder);
    void sendMqttSpasi(ArpaSenderDecoder* data);
};

#endif // ARPASENDER_H
