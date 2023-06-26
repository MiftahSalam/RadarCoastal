#ifndef FRAMECONTROL3_H
#define FRAMECONTROL3_H

#include <QFrame>

#include <RadarEngine/radarconfig.h>

namespace Ui {
class FrameControl3;
}

class FrameControl3 : public QFrame
{
    Q_OBJECT

public:
    explicit FrameControl3(QWidget *parent = nullptr);
    ~FrameControl3();

signals:
    void signal_PPIFullChanged();

private slots:
    void on_comboBoxMotion_currentIndexChanged(int index);

    void on_checkBoxShowCompass_clicked(bool checked);

    void on_checkBoxShowHM_clicked(bool checked);

    void on_checkBoxShowRSweep_clicked(bool checked);

    void on_comboBoxDisplayMode_currentIndexChanged(int index);

    void on_comboBoxDisplayUnit_currentIndexChanged(int index);

private:
    void initConfig();

    Ui::FrameControl3 *ui;

    RadarEngine::RadarConfig* m_instance_cfg;
    int prev_unit_idx;
};

#endif // FRAMECONTROL3_H
