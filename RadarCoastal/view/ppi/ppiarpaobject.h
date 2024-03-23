#ifndef PPIARPAOBJECT_H
#define PPIARPAOBJECT_H

#ifndef DISPLAY_ONLY_MODE
#include "usecase/ppi/arpa.h"
#else
#include "usecase/track/track.h"
#endif
#include "ppiobject.h"

class PPIArpaObject : public PPIObject
{
    Q_OBJECT
public:
#ifndef DISPLAY_ONLY_MODE
    PPIArpaObject(QObject* parent = nullptr, PPIArpa* arpa = nullptr);
    PPIArpa* m_ppi_arpa;
#else
    PPIArpaObject(QObject* parent = nullptr);
    Track* m_track;
#endif

    void Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center) override;

};

#endif // PPIARPAOBJECT_H
