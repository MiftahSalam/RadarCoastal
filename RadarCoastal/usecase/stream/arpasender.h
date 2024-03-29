#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <RadarEngine/radarconfig.h>

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"
#include "usecase/stream/arpa_sender_model.h"

class ArpaSender : public QObject
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void SendManyData(QList<TrackModel *> data);
    void SendOneData(TrackModel data);
    void SendOneData(
            long long ts,
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs);
signals:

private slots:
    void triggerConfigChange(const QString key, const QVariant val);

private:
    Stream *m_stream;
    QString m_topic;

    void initConfig();
    void sendMqtt(ArpaSenderDecoder *decoder);
};

#endif // ARPASENDER_H
