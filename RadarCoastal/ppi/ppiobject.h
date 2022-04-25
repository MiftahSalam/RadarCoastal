#ifndef PPIOBJECT_H
#define PPIOBJECT_H

#include <QObject>
#include <QPainter>

#include <radarconfig.h>

class PPIObject : public QObject
{
    Q_OBJECT
public:
    PPIObject(QObject* parent);

    virtual void draw(QPainter* painter, const int &side) = 0;
};

#endif // PPIOBJECT_H
