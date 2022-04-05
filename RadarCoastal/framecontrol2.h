#ifndef FRAMECONTROL2_H
#define FRAMECONTROL2_H

#include <QFrame>

#include <radarengine.h>

namespace Ui {
class FrameControl2;
}

class FrameControl2 : public QFrame
{
    Q_OBJECT

public:
    explicit FrameControl2(QWidget *parent = nullptr);
    ~FrameControl2();

    void setRadarEnginePtr(RadarEngine::RadarEngine* re = nullptr);

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

private:
    Ui::FrameControl2 *ui;
    RadarEngine::RadarEngine* m_re;
};

#endif // FRAMECONTROL2_H
