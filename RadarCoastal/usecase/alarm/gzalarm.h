#ifndef GZALARM_H
#define GZALARM_H

#include <QObject>

#include <radarengine.h>
#include "alarm.h"

class GZAlarm : public Alarm
{
    Q_OBJECT
public:
    GZAlarm(QObject *parent = nullptr, QString id = "", RadarEngine::RadarEngine* re = nullptr);
    ~GZAlarm() override;

    void Confirm() override;
    bool IsConfirmed() override;

protected:
    void checkAlarm() override;

private:
    RadarEngine::RadarEngine* m_re;
    QString m_id;
    QString m_time_key;
    QString m_timeout_key;
    QString m_shown_key;
    QString m_enable_alarm_key;
    QString m_confirmed_alarm_key;
    QString m_alarm_trhold_key;
};

#endif // GZALARM_H
