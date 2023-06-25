#ifndef PPIARPAOBJECT_H
#define PPIARPAOBJECT_H

#include "usecase/ppi/arpa.h"
#include "ppiobject.h"

class PPIArpaObject : public PPIObject
{
    Q_OBJECT
public:
    PPIArpaObject(QObject* parent = nullptr, PPIArpa* arpa = nullptr);

    void Draw(QPainter* painter, const int &side) override;

    PPIArpa* m_ppi_arpa;
};

#endif // PPIARPAOBJECT_H
