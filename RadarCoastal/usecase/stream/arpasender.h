#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <RadarEngine/radarconfig.h>

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"

struct ArpaSenderModel
{
    int id;
    double lat;
    double lon;
    double alt;
    double rng;
    double brn;
    double spd;
    double crs;
};

class ArpaSenderDecoder
{
public:
    ArpaSenderDecoder(int id,
                      double lat,
                      double lon,
                      double alt,
                      double rng,
                      double brn,
                      double spd,
                      double crs);
    ArpaSenderDecoder(TrackModel data);
    ArpaSenderDecoder(QList<TrackModel*> data);

    virtual ~ArpaSenderDecoder(){}

    virtual QString decode() = 0;

protected:
    QList<ArpaSenderModel*> m_data;
};

class ArpaSenderDecoderJson: public ArpaSenderDecoder
{
public:
    ArpaSenderDecoderJson(int id,
                          double lat,
                          double lon,
                          double alt,
                          double rng,
                          double brn,
                          double spd,
                          double crs);
    ArpaSenderDecoderJson(TrackModel data);
    ArpaSenderDecoderJson(QList<TrackModel*> data);

    // ArpaSenderDecoder interface
    QString decode() override;
};

class ArpaSenderDecoderNMEA: public ArpaSenderDecoder
{
public:
    ArpaSenderDecoderNMEA(int id,
                          double lat,
                          double lon,
                          double alt,
                          double rng,
                          double brn,
                          double spd,
                          double crs);
    ArpaSenderDecoderNMEA(TrackModel data);
    ArpaSenderDecoderNMEA(QList<TrackModel*> data);

    // ArpaSenderDecoder interface
    QString decode() override;
};

class ArpaSender : public QObject
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void SendManyData(QList<TrackModel*> data);
    void SendOneData(TrackModel data);
    void SendOneData(
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
