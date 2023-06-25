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
    explicit FrameCursor(QWidget *parent = nullptr);
    ~FrameCursor() override;

public slots:
    void trigger_cursorMove(const QPoint pos, const int vp_width, const int vp_height);

private:
    Ui::FrameCursor *ui;
};

#endif // FRAMECURSOR_H
