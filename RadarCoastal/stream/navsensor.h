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

signals:

private slots:
    void trigger_receivedData(const QString data);

private:
    Stream *stream;
    QString topic;

    Stream::StreamConfig generateStreamConfig(const QString config);
};

#endif // NAVSENSOR_H
