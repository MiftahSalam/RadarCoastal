#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>

#include "infra/stream/stream.h"
#include "shared/config/navigation_config.h"
#include "usecase/stream/nav_data_model.h"
#include <RadarEngine/radarconfig.h>

class NavSensor : public QObject, ConfigListener
{
    Q_OBJECT
public:
    explicit NavSensor(QObject *parent = nullptr);
    void Reconnect();
    void UpdateStatus();
#ifndef DISPLAY_ONLY_MODE
    void SendData();
#endif

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;

private slots:
    void triggerReceivedData(QString data);

private:
    RadarEngine::RadarConfig *m_instance_cfg;
    NavigationConfig *navConfig;
#ifndef DISPLAY_ONLY_MODE
    Stream *m_stream_mqtt_private;
    QString m_topic_private;
    void initConfigMqttPrivate();
#endif
    Stream *m_stream_mqtt_public;
    NavDataDecoder *decoder;
    QString m_topic_public;
    int m_no_osd_count;

    void sendMqtt(NavDataEncoder *encoder);
    void initConfigMqttPublic();
    void processNavData(QString data);
};

#endif // NAVSENSOR_H
