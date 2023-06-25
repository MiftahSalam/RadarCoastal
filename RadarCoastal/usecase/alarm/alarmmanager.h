#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QTimer>

#include "usecase/alarm/gzalarm.h"

class AlarmManager : public QObject
{
    Q_OBJECT
public:
    AlarmManager(AlarmManager& other) = delete;
    void operator=(const AlarmManager&) = delete;
    ~AlarmManager() override;

    bool AddAlarm(const AlarmType type, const QString id);
    void Confirm(const QString id);

    static AlarmManager* GetInstance();

signals:
    void SignalAlarm(const QString id, const QString message);

public slots:
    void TriggerAlarm(const QString id, const QString message);

private slots:
    void TriggerTimerTimeOut();

protected:
    explicit AlarmManager(QObject *parent = nullptr);

private:
    QMap<QString, Alarm*> m_list_alarms;
    static AlarmManager* m_alarm_manager;
    QTimer *m_timer;
    QString m_current_id, m_current_msg;
    int m_no_alarm_count;
};

#endif // ALARMMANAGER_H
