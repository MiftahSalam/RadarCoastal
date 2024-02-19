#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <RadarEngine/radarconfig.h>

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"

struct ArpaSenderModel
{
    long long timestamp;
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
    ArpaSenderDecoder(long long ts,
                      int id,
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
    bool isArray;
};

class ArpaSenderDecoderJson: public ArpaSenderDecoder
{
public:
    ArpaSenderDecoderJson(long long ts,
                          int id,
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
    QJsonDocument decodeJsonDoc();
};

class ArpaSenderDecoderNMEA: public ArpaSenderDecoder
{
public:
    ArpaSenderDecoderNMEA(long long ts,
                          int id,
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
            long long ts,
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
    void Reconnect();

signals:

private slots:
    void triggerConfigChange(const QString key, const QVariant val);

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    Stream *m_stream_mqtt;
    Stream *m_stream_ws;

    QString m_topic;

    void initConfigWS();
    void initConfigMqtt();
    void sendMqtt(ArpaSenderDecoder* decoder);
    void sendWS(ArpaSenderDecoder* data);
};

#endif // ARPASENDER_H
