#ifndef PPIARPAOBJECT_H
#define PPIARPAOBJECT_H

#include "ppiobject.h"

#include <radarengine.h>

class PPIArpaObject : public PPIObject
{
    Q_OBJECT
public:
    PPIArpaObject(QObject* parent = nullptr);

    void draw(QPainter* painter) override;

public slots:
    void createMARPA(QPoint pos);

private:
    RadarEngine::RadarEngine* m_re;
};

#endif // PPIARPAOBJECT_H
