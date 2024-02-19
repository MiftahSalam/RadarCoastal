#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>

#include "infra/stream/stream.h"
#include <RadarEngine/radarconfig.h>

class NavSensor : public QObject
{
    Q_OBJECT
public:
    explicit NavSensor(QObject *parent = nullptr);
    void Reconnect();
    void UpdateStatus();
    void SendData(QString lat, QString lon, QString hdt);

signals:

private slots:
    void triggerReceivedData(const QString data);
    void triggerConfigChange(const QString key, const QVariant val);

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    Stream *m_stream_mqtt;
    QString m_topic;
    QString m_append_data_osd;
    int m_no_osd_count;
    quint8 m_no_hdg_count,m_no_gps_count;

    Stream::StreamConfig generateStreamConfig(const QString config);
    bool isGPSDataValid(const QString lat_str, const QString lon_str);
    bool isHDGDataValid(const QString data);
    void initConfigMqtt();
};

#endif // NAVSENSOR_H
