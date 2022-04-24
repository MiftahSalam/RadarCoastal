#ifndef FRAMESTATUS_H
#define FRAMESTATUS_H

#include <QFrame>

namespace Ui {
class FrameStatus;
}

class FrameStatus : public QFrame
{
    Q_OBJECT

public:
    explicit FrameStatus(QWidget *parent = nullptr);
    ~FrameStatus();

private:
    Ui::FrameStatus *ui;
};

#endif // FRAMESTATUS_H
