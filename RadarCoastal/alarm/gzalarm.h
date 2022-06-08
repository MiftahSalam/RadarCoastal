#ifndef GZALARM_H
#define GZALARM_H

#include <QObject>

#include <radarengine.h>
#include "alarm.h"

class GZAlarm : public Alarm
{
    Q_OBJECT
public:
    GZAlarm(GZAlarm& other) = delete;
    void operator=(const GZAlarm&) = delete;
    ~GZAlarm() override;

    static GZAlarm* getInstance(uint id);

protected:
    void checkAlarm() override;
    GZAlarm(QObject *parent = nullptr);

private:
    static QMap<uint, GZAlarm*> m_GZAlarms;

    RadarEngine::RadarEngine* m_re;

};

#endif // GZALARM_H
