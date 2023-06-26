#ifndef FRAMETRACKDISPLAY_H
#define FRAMETRACKDISPLAY_H

#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>

#include "usecase/stream/arpasender.h"
#include <RadarEngine/radarengine.h>

namespace Ui {
class FrameTrackDisplay;
}

class FrameTrackDisplay : public QFrame
{
    Q_OBJECT

public:
    explicit FrameTrackDisplay(QWidget *parent = nullptr);
    ~FrameTrackDisplay() override;

private slots:
    void timerTimeout();
    void trigger_LostTarget(int id);

    void on_pushButtonDelSel_clicked();

    void on_pushButtonDelAll_clicked();

private:
    Ui::FrameTrackDisplay *ui;
    QTimer *timer;
    ArpaSender *arpaSender;
    RadarEngine::RadarEngine* m_re;

    int dataCount_mqtt, updateCount, cur_arpa_id_count;
    QStandardItemModel *model,*modelSend;

    void updateTarget();
    void removeTarget(int id);
    void insertList(
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
    void trigger_target_update(
            int id,
            double lat,
            double lon,
            double alt,
            double rng,
            double brn,
            double spd,
            double crs
            );
};

#endif // FRAMETRACKDISPLAY_H
