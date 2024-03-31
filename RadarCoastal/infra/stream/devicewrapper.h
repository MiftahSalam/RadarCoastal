#ifndef DEVICEWRAPPER_H
#define DEVICEWRAPPER_H

#include <QObject>
#include <QDateTime>

class DeviceWrapper : public QObject
{
    Q_OBJECT
public:
    enum DeviceStatus
    {
        NOT_AVAIL = 0,
        NO_INPUT_DATA,
        INPUT_AVAIL
    };

    explicit DeviceWrapper(QObject *parent = nullptr);
    virtual void Write(const QString data) = 0;
    virtual DeviceStatus GetStatus() = 0;
    virtual void Reconnect() = 0;
    virtual void UpdateTimeStamp();
    virtual void ChangeConfig(const QString command) = 0;
    virtual bool InitConfig(const QString config) = 0;

signals:
    void ReadyRead(const QString data);

public slots:

protected:
    QString m_config;
//    QString _currentData;
    DeviceStatus m_currentStatus;
    qint64 m_last_data_time;
};

#endif // DEVICEWRAPPER_H
