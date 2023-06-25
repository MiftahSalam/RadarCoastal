#ifndef PPIEVENT_H
#define PPIEVENT_H

#include <QObject>

class PPIEvent : public QObject
{
    Q_OBJECT
public:
    explicit PPIEvent(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void send_rightButtonClicked(const QPoint &p);
    void send_leftButtonClicked(const QPoint &p);
    void send_leftButtonReleased(const QPoint &p);
    void move_mouse(const QPoint &p);
    void hover_enter();
    void hover_leave();
};

#endif // PPIEVENT_H
