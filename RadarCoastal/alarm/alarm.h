#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QTimer>

class Alarm : public QObject
{
    Q_OBJECT
public:
    explicit Alarm(QObject *parent = nullptr);

signals:
    void signal_alarmTriggered(const QString message);

protected:
    virtual void checkAlarm() = 0;
    void init(int time);
    QTimer *timer;

protected slots:
    void trigger_timeOut();

};

#endif // ALARM_H
