#ifndef PPIARPAOBJECT_H
#define PPIARPAOBJECT_H

#include "ppiobject.h"

#include <radarengine.h>

class PPIArpaObject : public PPIObject
{
    Q_OBJECT
public:
    PPIArpaObject(QObject* parent = nullptr);

    void draw(QPainter* painter, const int &side) override;

public slots:
    void createMARPA(const QPoint &pos, const int vp_width, const int vp_height);

private:
    RadarEngine::RadarEngine* m_re;
};

#endif // PPIARPAOBJECT_H
