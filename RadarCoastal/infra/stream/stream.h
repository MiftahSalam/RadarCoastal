#ifndef STREAM_H
#define STREAM_H

#include <QObject>

#include "mqttdevicewrapper.h"
#include "wsdevicewrapper.h"

class Stream : public QObject
{
    Q_OBJECT
public:    
    enum StreamType {
        MQTT,
        WS
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

    QString GetStreamStatusString() const;
    DeviceWrapper::DeviceStatus GetStreamStatus() const;
    void SendData(const QString& data);
    void SetConfig(const QString& config);
    void Reconnect();

protected:
    QString streamError;

signals:
    void SignalReceiveData(const QString& data);

public slots:

private:
    void generateConfig(const QString config);

    StreamConfig m_config;
    DeviceWrapper *m_streamDevice;
    bool m_stop;
};

#endif // STREAM_H
