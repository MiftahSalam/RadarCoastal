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
    void SendData();

signals:

private slots:
    void triggerReceivedData(const QString data);
    void triggerConfigChange(const QString key, const QVariant val);

private:
    RadarEngine::RadarConfig *m_instance_cfg;
    Stream *m_stream;
    NavDataDecoder *decoder;
    QString m_topic;
    int m_no_osd_count;

    void sendMqtt(NavDataEncoder *encoder);
    void initConfig();
#ifdef DISPLAY_ONLY_MODE
    void processDisplayOnlyNav(const QString data);
#else
    void processRawNav(const QString data);
#endif
};

#endif // NAVSENSOR_H
