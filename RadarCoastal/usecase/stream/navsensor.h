#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>

#include "qfuturewatcher.h"

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

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;

private slots:
    void triggerReceivedData(QString data);
    void triggerParseData();

private:
    QFutureWatcher<NavDataModel> watcherCapture;
    RadarEngine::RadarConfig* m_instance_cfg;
    NavigationConfig *navConfig;
    Stream *m_stream_mqtt;
    NavDataDecoder *decoder;
    QString m_topic;
    int m_no_osd_count;

    void sendMqtt(NavDataEncoder *encoder);
    void initConfigMqtt();

};

#endif // NAVSENSOR_H
