#include "ppievent.h"

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QDebug>

PPIEvent::PPIEvent(QObject *parent) : QObject(parent)
{

}
bool PPIEvent::eventFilter(QObject *obj, QEvent *event)
{
     if (event->type() == QEvent::ContextMenu) {
        qDebug()<<Q_FUNC_INFO<<"contex menu";
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
        send_rightButtonClicked(mouseEvent->globalPos());
        return true;
    }
    else if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent=static_cast<QMouseEvent*> (event);
        if(mouseEvent->button()==Qt::LeftButton){
            send_leftButtonClicked(mouseEvent->pos());
        }
        return true;
    }
    else if(event->type() == QEvent::MouseButtonRelease){
         QMouseEvent *mouseEvent=static_cast<QMouseEvent*> (event);
         send_leftButtonReleased(mouseEvent->pos());

         return true;
    }
    else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
        move_mouse(mouseEvent->pos());
        return true;
    }
    else if (event->type() == QEvent::Enter) { //30 jan
        //        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
        hover_enter();
        return true;
    }
    else if (event->type() == QEvent::Leave) {
        //        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
        hover_leave();
        return true;
    }
    else
        return QObject::eventFilter(obj, event);
}
