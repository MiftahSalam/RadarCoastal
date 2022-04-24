#ifndef ARPASENDER_H
#define ARPASENDER_H

#include <QObject>

#include <QObject>

#include "stream/stream.h"
#include <radarconfig.h>

class ArpaSender : public QObject
{
    Q_OBJECT
public:
    explicit ArpaSender(QObject *parent = nullptr);

    void sendData(const QString data);
signals:

private slots:
    void trigger_configChange(const QString key, const QVariant val);

private:
    Stream *stream;
    QString topic;

    Stream::StreamConfig generateStreamConfig(const QString config);

};

#endif // ARPASENDER_H
