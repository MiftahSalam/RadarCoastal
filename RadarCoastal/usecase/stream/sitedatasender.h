#ifndef SITEDATASENDER_H
#define SITEDATASENDER_H

#include <QObject>

#include "infra/stream/stream.h"

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/global.h>

class SiteDataSender : public QObject
{
    Q_OBJECT
public:
    explicit SiteDataSender(QObject *parent = nullptr);

    void Reconnect();
    void SendSiteData();

signals:

private slots:
    void triggerConfigChange(const QString key, const QVariant val);

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    Stream *m_stream_ws;
    Stream *m_stream_mqtt;
    int m_site_data_count, max_site_data_count;

    QString m_topic;

    void sendMqtt(QString data);
    void initConfigWS();
    void initConfigMqtt();
    int navStatusCode(const int status) const;
    int radarStatusCode(const RadarEngine::RadarState status) const;
};

#endif // SITEDATASENDER_H
