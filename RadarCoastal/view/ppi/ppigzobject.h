#ifndef PPIGZOBJECT_H
#define PPIGZOBJECT_H

#include <QObject>

#include "ppiobject.h"

#include <RadarEngine/radarengine.h>

class PPIGZObject : public PPIObject
{
    Q_OBJECT
public:
    PPIGZObject(QObject* parent = nullptr, QString id = "");

    void Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center) override;

public slots:

private:
    RadarEngine::RadarEngine* m_re;
    QString m_id;
    QString inner_range_key;
    QString outer_range_key;
    QString start_bearing_key;
    QString end_bearing_key;
    QString mode_key;
    QString shown_key;
};

#endif // PPIGZOBJECT_H
