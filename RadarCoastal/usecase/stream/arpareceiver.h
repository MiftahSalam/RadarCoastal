#ifndef ARPARECEIVER_H
#define ARPARECEIVER_H

#include "domain/track/trackmodel.h"
#include "infra/stream/stream.h"
#include "shared/config/arpa_config.h"

#include <QObject>

#include <RadarEngine/radarconfig.h>

class ArpaReceiverDecoder
{
public:
    ArpaReceiverDecoder(QString data);

    virtual ~ArpaReceiverDecoder(){}

    virtual QList<TrackModel*> decode() = 0;
    virtual bool isDataValid() = 0;

protected:
    QString m_data;
    bool m_parse_error;
};

class ArpaReceiverDecoderJson: public ArpaReceiverDecoder
{
public:
    ArpaReceiverDecoderJson(QString data);

    // ArpaSenderDecoder interface
    QList<TrackModel*> decode() override;
    bool isDataValid() override;
};

class ArpaReceiver : public QObject
{
    Q_OBJECT
public:
    explicit ArpaReceiver(QObject *parent = nullptr);

    void Reconnect();
    void UpdateStatus();

signals:
    void signalNewTrack(QList<TrackModel*> tracks);

private slots:
    void triggerReceivedData(const QString data);

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    ArpaConfig *arpaConfig;
    Stream *m_stream;
    QString m_append_data;
    QString m_topic;
    int m_no_data_count;

    void initConfig();

};

#endif // ARPARECEIVER_H
