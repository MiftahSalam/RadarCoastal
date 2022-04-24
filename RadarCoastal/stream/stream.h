#ifndef STREAM_H
#define STREAM_H

#include <QObject>

#include "mqttdevicewrapper.h"

class Stream : public QObject
{
    Q_OBJECT
public:    
    enum StreamType {
        MQTT
    };
    enum StreamMode {
        STREAM_IN,
        STREAM_OUT,
        STREAM_IN_OUT
    };
    struct StreamConfig
    {
        StreamType type;
        StreamMode mode;
        QString config;
    };

    Stream(QObject *parent = nullptr, QString config = "" );
    ~Stream() override;

    QString getStreamError() const;
    void sendData(const QString& data);
    void setConfig(const QString& config);

protected:
    QString streamError;

signals:
    void signal_receiveData(const QString& data);

public slots:

private:
    void generateConfig(const QString config);

    StreamConfig m_config;
    DeviceWrapper *m_streamDevice;
    bool stop;
};

#endif // STREAM_H
