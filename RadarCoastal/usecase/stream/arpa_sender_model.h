#ifndef ARPA_SENDER_MODEL_H
#define ARPA_SENDER_MODEL_H

#include <QList>
#include <QJsonDocument>

#include "domain/track/trackmodel.h"

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

#endif // ARPA_SENDER_MODEL_H
