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

};

#endif // GZALARM_H
