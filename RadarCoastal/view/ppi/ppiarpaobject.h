#ifndef PPIARPAOBJECT_H
#define PPIARPAOBJECT_H

#include "usecase/ppi/arpa.h"
#include "ppiobject.h"

class PPIArpaObject : public PPIObject
{
    Q_OBJECT
public:
    PPIArpaObject(QObject* parent = nullptr, PPIArpa* arpa = nullptr);

    void Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center) override;

    PPIArpa* m_ppi_arpa;
};

#endif // PPIARPAOBJECT_H
