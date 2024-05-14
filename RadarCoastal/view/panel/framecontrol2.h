#ifndef FRAMECONTROL2_H
#define FRAMECONTROL2_H

#include "shared/config/ppi_config.h"
#include <QFrame>

#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

namespace Ui {
class FrameControl2;
}

class FrameControl2 : public QFrame, public ConfigListener
{
    Q_OBJECT

public:
    explicit FrameControl2(QWidget *parent = nullptr);
    ~FrameControl2();

    // ConfigListener interface
    void configChange(const QString key, const QVariant val) override;

signals:
    void signal_change_gain_req(int value);
    void signal_change_sea_req(int value);
    void signal_change_rain_req(int value);

private slots:
    void on_pushButtonGain_clicked();

    void on_horizontalSliderGain_valueChanged(int value);

    void on_lineEditGain_textChanged(const QString &arg1);

    void on_pushButtonSea_clicked();

    void on_horizontalSliderSea_valueChanged(int value);

    void on_lineEditSea_textChanged(const QString &arg1);

    void on_horizontalSliderRain_valueChanged(int value);

    void on_lineEditRain_textChanged(const QString &arg1);

    void on_checkBoxMTI_clicked(bool checked);

    void on_lineEditMTI_textChanged(const QString &arg1);

    void on_horizontalSliderMTI_valueChanged(int value);

    void on_checkBoxEBL_clicked(bool checked);

    void on_checkBoxVRM_clicked(bool checked);

    void on_horizontalSlideEBL_valueChanged(int value);

    void on_horizontalSliderVRM_valueChanged(int value);

private:
    void initConfig();

    Ui::FrameControl2 *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    RadarEngine::RadarEngine* m_instance_re;
    PPIConfig *ppiConfig;
};

#endif // FRAMECONTROL2_H
