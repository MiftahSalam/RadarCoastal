#ifndef FRAMESTATUS_H
#define FRAMESTATUS_H

#include <QFrame>

#include <radarengine.h>
#include "ppi/ppievent.h"
#include "alarm/alarmmanager.h"

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
    void trigger_Alarm(const QString msg);
    void on_alarmStatus_clicked(const QPoint &p);

private:
    Ui::FrameStatus *ui;
    RadarEngine::RadarEngine* m_re;
    PPIEvent *alarmEvent;
    AlarmManager *alarmManager;
    bool alarmToggle;

    void updateRadarStatus(const RadarEngine::RadarState status);
    void updateNavStatus(const int status);
    void initStatus();
};

#endif // FRAMESTATUS_H
