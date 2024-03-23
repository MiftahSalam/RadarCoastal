#ifndef PPIOBJECT_H
#define PPIOBJECT_H

#include <QObject>
#include <QPainter>

#include <RadarEngine/radarconfig.h>

class PPIObject : public QObject
{
    Q_OBJECT
public:
    PPIObject(QObject* parent);

    virtual void Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center) = 0;
};

#endif // PPIOBJECT_H
