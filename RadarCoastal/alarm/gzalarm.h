#ifndef GZALARM_H
#define GZALARM_H

#include <QObject>

#include <radarengine.h>
#include "alarm.h"

class GZAlarm : public Alarm
{
    Q_OBJECT
public:
    GZAlarm(QObject *parent = nullptr, QString id = "");
    ~GZAlarm() override;

    void confirm() override;
    bool isConfirmed() override;

protected:
    void checkAlarm() override;

private:
    RadarEngine::RadarEngine* m_re;
    QString m_id;
    QString time_key;
    QString timeout_key;
    QString shown_key;
    QString enable_alarm_key;
    QString confirmed_alarm_key;
    QString alarm_trhold_key;
};

#endif // GZALARM_H
