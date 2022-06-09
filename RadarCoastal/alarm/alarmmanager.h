#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>

//#include "alarm/alarm.h"
#include "alarm/gzalarm.h"

class AlarmManager : public QObject
{
    Q_OBJECT
public:
    AlarmManager(AlarmManager& other) = delete;
    void operator=(const AlarmManager&) = delete;
    ~AlarmManager() override;
    bool addAlarm(const AlarmType type, const QString id);

    static AlarmManager* getInstance();

signals:
    void signal_alarm(const QString message);

public slots:
    void trigger_alarm(const QString id, const QString message);

protected:
    explicit AlarmManager(QObject *parent = nullptr);

private:
    QMap<QString, Alarm*> listAlarms;
    static AlarmManager* alarmManager;
};

#endif // ALARMMANAGER_H
