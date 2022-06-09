#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QTimer>

enum AlarmType
{
    ALARM_UNKNOWN = 0,
    ALARM_GZ
};

class Alarm : public QObject
{
    Q_OBJECT
public:
    explicit Alarm(QObject *parent = nullptr, AlarmType type = ALARM_UNKNOWN, QString id = "");

    QString getId() const;
    AlarmType getType() const;

signals:
    void signal_alarmTriggered(const QString id, const QString message);

protected:
    virtual void checkAlarm() = 0;
    void init(int time);
    QTimer *timer;

protected slots:
    void trigger_timeOut();

private:
    QString m_id;
    AlarmType m_type;
};

#endif // ALARM_H
