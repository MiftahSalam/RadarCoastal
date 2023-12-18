#ifndef FRAMETRAIL_H
#define FRAMETRAIL_H

#include <QFrame>
#include <QMap>

#include <RadarEngine/radarengine.h>

namespace Ui {
class FrameTrail;
}

class FrameTrail : public QFrame
{
    Q_OBJECT
public:
    explicit FrameTrail(QWidget *parent = nullptr);
    ~FrameTrail() override;

private slots:
    void on_checkBoxTrailOff_clicked(bool checked);

    void on_pushButtonClearTrail_clicked();

    void on_comboBoxTargetTail_currentIndexChanged(int index);

private:
    Ui::FrameTrail *ui;
    RadarEngine::RadarEngine* m_re;
};

#endif // FRAMETRAIL_H
