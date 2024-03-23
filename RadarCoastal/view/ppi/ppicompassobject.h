#ifndef PPICOMPASSOBJECT_H
#define PPICOMPASSOBJECT_H

#include <QObject>

#include "ppiobject.h"

#include <RadarEngine/radarengine.h>

class PPICompassObject : public PPIObject
{
    Q_OBJECT
public:
    PPICompassObject(QObject* parent = nullptr);

    void Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center) override;

private:
    QPoint calculateGradien(const QPoint &p1, const QPoint &p2);
};

#endif // PPICOMPASSOBJECT_H
