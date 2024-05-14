#ifndef SITEDATASENDER_H
#define SITEDATASENDER_H

#include <QObject>

#include "infra/stream/stream.h"
#include "shared/config/navigation_config.h"

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/global.h>

class SiteDataSender : public QObject, public ConfigListener
{
    Q_OBJECT
public:
    explicit SiteDataSender(QObject *parent = nullptr);

    void Reconnect();
    void SendSiteData();

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    NavigationConfig *navConfig;
    Stream *m_stream_mqtt_spasi;
    Stream *m_stream_mqtt;
    int m_site_data_count, max_site_data_count;

    QString m_topic, m_topic_spasi;

    void sendMqtt(QString data);
    void sendMqttSpasi(QString data);
    void initConfigMqttSpasi();
    void initConfigMqtt();
    int navStatusCode(const int status) const;
    int radarStatusCode(const RadarEngine::RadarState status) const;

};

#endif // SITEDATASENDER_H
