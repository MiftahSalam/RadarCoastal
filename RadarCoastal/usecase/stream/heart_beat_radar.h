#ifndef HEARTBEATRADAR_H
#define HEARTBEATRADAR_H

#include "infra/stream/stream.h"
#include "shared/config/hear_beat_radar_config.h"

#include <QObject>
#include <QTimer>

class HeartBeatRadar : public QObject
{
    Q_OBJECT
public:
    HeartBeatRadar(QObject *parent);

private slots:
    void triggerTimerTimeout();

private:
    HearBeatRadarConfig *hbRadarConfig;
    QString m_topic_hb;
    QTimer *timer;
    Stream *m_stream;
    int timeout;

    void setTimeout(const QString timeoutCfg);
    void reconnect();
    void sendHeartBeat();
    void initConfigMqttPublic();
};

#endif // HEARTBEATRADAR_H
