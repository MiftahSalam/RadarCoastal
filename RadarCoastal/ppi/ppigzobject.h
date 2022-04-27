#ifndef PPIGZOBJECT_H
#define PPIGZOBJECT_H

#include <QObject>

#include "ppiobject.h"

#include <radarengine.h>

class PPIGZObject : public PPIObject
{
    Q_OBJECT
public:
    PPIGZObject(QObject* parent = nullptr);

    void draw(QPainter* painter, const int &side) override;

public slots:

private:
    RadarEngine::RadarEngine* m_re;
};

#endif // PPIGZOBJECT_H
