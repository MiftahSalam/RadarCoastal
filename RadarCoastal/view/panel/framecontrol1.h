#ifndef FRAMECONTROL1_H
#define FRAMECONTROL1_H

#include <QFrame>
#include <QVariant>

#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

#include "infra/withconfig.h"
#include "infra/withradarengine.h"

using namespace RadarEngine;

namespace Ui {
class FrameControl1;
}

class FrameControl1 : public QFrame, public WithRadarEngine, protected WithConfig
{
    Q_OBJECT

public:
    explicit FrameControl1(QWidget *parent = nullptr, RadarEngine::RadarConfig* cfg = nullptr, RadarEngine::RadarEngine* re = nullptr);
    ~FrameControl1();

//    void setRangeRing(double ringWidth);
    void stateChange(int state);

signals:
    void signal_req_range(int range);
    void signal_req_Tx();
    void signal_req_Stby();
    void signal_req_shutdown();
    void signal_antena_man_switch();

private slots:
    void on_pushButtonZoomIn_clicked();

    void on_pushButtonZoomOut_clicked();

    void on_pushButtonTxStnb_clicked();

    void on_checkBoxShowRing_clicked(bool checked);

    void trigger_radarConfigChange(QString key, QVariant val);

    void on_pushButtonShutdown_clicked();

protected:
    void initConfig();

private:
    void handleRingRangeChange();

    Ui::FrameControl1 *ui;

    QString ringValue;
};

#endif // FRAMECONTROL1_H
