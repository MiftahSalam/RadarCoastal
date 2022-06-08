#ifndef FRAMESTATUS_H
#define FRAMESTATUS_H

#include <QFrame>

#include <radarengine.h>
#include <alarm/gzalarm.h>

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
    void trigger_GZAlarmTrigger(const QString msg);

private:
    Ui::FrameStatus *ui;
    RadarEngine::RadarEngine* m_re;

private:
    GZAlarm* gzAlarm;

    void updateRadarStatus(const RadarEngine::RadarState status);
    void updateNavStatus(const int status);
};

#endif // FRAMESTATUS_H
