#ifndef FRAMESTATUS_H
#define FRAMESTATUS_H

#include <QFrame>

#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

#include "view/ppi/ppievent.h"
#include "usecase/alarm/alarmmanager.h"

namespace Ui {
class FrameStatus;
}

class FrameStatus : public QFrame
{
    Q_OBJECT

public:
    explicit FrameStatus(QWidget *parent = nullptr);
    ~FrameStatus() override;

private slots:
    void trigger_statusChange(const QString& key, const QVariant& val);
    void trigger_Alarm(const QString id, const QString msg);
    void on_alarmStatus_clicked(const QPoint &p);

private:
    Ui::FrameStatus *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    RadarEngine::RadarEngine* m_instance_re;
    PPIEvent *m_alarm_event;
    AlarmManager *m_alarm_manager;
    bool m_alarm_toggle;

    void updateRadarStatus(const RadarEngine::RadarState status);
    void updateNavStatus(const int status);
    void initStatus();
};

#endif // FRAMESTATUS_H
