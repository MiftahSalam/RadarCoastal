#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>

#include "infra/stream/stream.h"
#include "usecase/stream/nav_data_model.h"
#include <RadarEngine/radarconfig.h>

class NavSensor : public QObject
{
    Q_OBJECT
public:
    explicit NavSensor(QObject *parent = nullptr);
    void Reconnect();
    void UpdateStatus();
#ifndef DISPLAY_ONLY_MODE
    void SendData();
#endif

signals:

private slots:
    void triggerReceivedData(QString data);
    void triggerConfigChange(const QString key, const QVariant val);

private:
    RadarEngine::RadarConfig *m_instance_cfg;
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
