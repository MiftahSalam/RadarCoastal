#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <QObject>

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"
#include <RadarEngine/radarconfig.h>

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

    virtual ~ArpaSenderDecoder(){}

    virtual QString decode() = 0;

protected:
    ArpaSenderModel m_data;
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

    // ArpaSenderDecoder interface
    QString decode() override;
};

class ArpaSender : public QObject
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void SendData(TrackModel data);
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
