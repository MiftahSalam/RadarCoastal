#ifndef NAVSENSOR_H
#define NAVSENSOR_H

#include <QObject>

#include "stream/stream.h"
#include <radarconfig.h>

class NavSensor : public QObject
{
    Q_OBJECT
public:
    explicit NavSensor(QObject *parent = nullptr);
    void reconnect();
    void updateStatus();

signals:

private slots:
    void trigger_receivedData(const QString data);
    void trigger_configChange(const QString key, const QVariant val);

private:
    Stream *stream;
    QString topic;
    QString append_data_osd;
    int no_osd_count;
    quint8 no_hdg_count,no_gps_count;

    Stream::StreamConfig generateStreamConfig(const QString config);
    bool isGPSDataValid(const QString lat_str, const QString lon_str);
    bool isHDGDataValid(const QString data);
};

#endif // NAVSENSOR_H
