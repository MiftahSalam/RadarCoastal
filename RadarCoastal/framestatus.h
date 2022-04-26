#ifndef FRAMESTATUS_H
#define FRAMESTATUS_H

#include <QFrame>

#include <radarengine.h>

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

private:
    Ui::FrameStatus *ui;
    RadarEngine::RadarEngine* m_re;
};

#endif // FRAMESTATUS_H
