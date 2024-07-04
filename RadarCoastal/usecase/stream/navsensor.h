#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>
#ifndef DISPLAY_ONLY_MODE
#include "qfuturewatcher.h"
#endif

#include "infra/stream/stream.h"
#include "shared/config/navigation_config.h"
#include "usecase/stream/nav_data_model.h"
#include <RadarEngine/radarconfig.h>

#ifndef DISPLAY_ONLY_MODE
class NavSensor : public QObject, ConfigListener
        #else
class NavSensor : public QObject
        #endif
{
    Q_OBJECT
public:
    explicit NavSensor(QObject *parent = nullptr);
    void Reconnect();
    void UpdateStatus();
#ifndef DISPLAY_ONLY_MODE
    void SendData();

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;
#endif


private slots:
    void triggerReceivedData(QString data);
#ifndef DISPLAY_ONLY_MODE
    void triggerParseData();
#endif

private:
    RadarEngine::RadarConfig *m_instance_cfg;
    NavigationConfig *navConfig;
#ifndef DISPLAY_ONLY_MODE
    QFutureWatcher<NavDataModel> watcherCapture;
    Stream *m_stream_mqtt_private;
    QString m_topic_private;
    bool pendingDataProcess;

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
