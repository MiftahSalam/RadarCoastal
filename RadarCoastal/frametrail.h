#ifndef FRAMETRAIL_H
#define FRAMETRAIL_H

#include <QFrame>

namespace Ui {
class FrameTrail;
}

class FrameTrail : public QFrame
{
    Q_OBJECT

public:
    explicit FrameTrail(QWidget *parent = nullptr);
    ~FrameTrail();

private:
    Ui::FrameTrail *ui;
};

#endif // FRAMETRAIL_H
