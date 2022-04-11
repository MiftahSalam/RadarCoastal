#ifndef FRAMECURSOR_H
#define FRAMECURSOR_H

#include <QFrame>

namespace Ui {
class FrameCursor;
}

class FrameCursor : public QFrame
{
    Q_OBJECT

public:
    explicit FrameCursor(QWidget *parent = 0);
    ~FrameCursor();

private slots:
    void trigger_cursorMove(double range,double brn);

private:
    Ui::FrameCursor *ui;
};

#endif // FRAMECURSOR_H
