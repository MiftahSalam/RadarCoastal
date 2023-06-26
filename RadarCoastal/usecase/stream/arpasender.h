#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <QObject>

#include "infra/stream/stream.h"
#include <RadarEngine/radarconfig.h>

class ArpaSender : public QObject
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void SendData(const QString data);
    void SendData(
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
signals:

private slots:
    void triggerConfigChange(const QString key, const QVariant val);

private:
    Stream *m_stream;
    QString m_topic;

    Stream::StreamConfig generateStreamConfig(const QString config);

};

#endif // ARPASENDER_H
