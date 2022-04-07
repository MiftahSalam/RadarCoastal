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

    explicit DeviceWrapper(QObject *parent = nullptr, QString config = "");
    virtual void write(const QString data) = 0;
    virtual DeviceStatus getStatus() = 0;
    virtual void changeConfig(const QString command) = 0;

signals:
    void readyRead(const QString data);

public slots:

protected:
    virtual void initConfig(const QString config) = 0;

    QString _config;
//    QString _currentData;
    DeviceStatus _currentStatus;
    qint64 last_data_time;
};

#endif // DEVICEWRAPPER_H
