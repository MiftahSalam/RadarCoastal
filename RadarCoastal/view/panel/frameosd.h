#ifndef FRAMEOSD_H
#define FRAMEOSD_H

#include <QFrame>
#include <QTimer>

#include <RadarEngine/radarconfig.h>

#include "usecase/stream/navsensor.h"

namespace Ui {
class FrameOSD;
}

class FrameOSD : public QFrame
{
    Q_OBJECT

public:
    explicit FrameOSD(QWidget *parent = nullptr);
    ~FrameOSD() override;

private slots:
    void on_timeout();

    void on_pushButtonApply_clicked();

private:
    Ui::FrameOSD *ui;

    QTimer *timer;
    NavSensor* sensor;
    RadarEngine::RadarConfig* m_instance_cfg;

    quint8 no_hdg_count,no_gps_count;
    quint8 hdg_col_normal,gps_col_normal;

    void initConfig();
    void updateGPSData();
    void updateHDGData();
    void updateGPSColor(const int status);
    void updateHDGColor(const int status);

};

#endif // FRAMEOSD_H
