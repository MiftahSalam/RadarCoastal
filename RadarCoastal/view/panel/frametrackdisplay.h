#ifndef FRAMETRACKDISPLAY_H
#define FRAMETRACKDISPLAY_H

#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>

#include "usecase/track/track.h"

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
#ifndef DISPLAY_ONLY_MODE
    void on_pushButtonDelSel_clicked();

    void on_pushButtonDelAll_clicked();
#endif
private:
    Ui::FrameTrackDisplay *ui;

    QTimer *timer;
    Track* track;
};

#endif // FRAMETRACKDISPLAY_H
