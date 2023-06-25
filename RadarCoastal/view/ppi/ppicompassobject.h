#ifndef PPICOMPASSOBJECT_H
#define PPICOMPASSOBJECT_H

#include <QObject>

#include "ppiobject.h"

#include <radarengine.h>

class PPICompassObject : public PPIObject
{
    Q_OBJECT
public:
    PPICompassObject(QObject* parent = nullptr);

    void Draw(QPainter* painter, const int &side) override;
};

#endif // PPICOMPASSOBJECT_H
