#ifndef FRAMECONTROL1_H
#define FRAMECONTROL1_H

#include <QFrame>
#include <QVariant>

#include <radarengine.h>

namespace Ui {
class FrameControl1;
}

class FrameControl1 : public QFrame
{
    Q_OBJECT

public:
    explicit FrameControl1(QWidget *parent = nullptr);
    ~FrameControl1();

//    void setRangeRing(double ringWidth);
    void setRadarEnginePtr(RadarEngine::RadarEngine* re = nullptr);
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

private:
    void handleRingRangeChange();

    Ui::FrameControl1 *ui;
    RadarEngine::RadarEngine* m_re;

    QString ringValue;
};

#endif // FRAMECONTROL1_H
