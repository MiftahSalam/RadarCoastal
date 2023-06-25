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

    void SetCurrentMessage(const QString message);
    QString GetCurrentMessage() const;
    QString GetId() const;
    AlarmType GetType() const;
    virtual void Confirm() = 0;
    virtual bool IsConfirmed() = 0;

signals:
    void SignalAlarmTriggered(const QString id, const QString message);

protected:
    virtual void checkAlarm() = 0;
    void init(int time);
    QTimer *timer;

protected slots:
    void triggerTimeOut();

private:
    QString m_id;
    QString m_current_messsage;
    AlarmType m_type;
};

#endif // ALARM_H
