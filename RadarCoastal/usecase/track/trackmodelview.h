#ifndef TRACKMODELVIEW_H
#define TRACKMODELVIEW_H

#include <QStandardItemModel>
#include <list>

#include "domain/track/trackmodel.h"

class TrackModelView
{
public:
    TrackModelView(QStandardItemModel* model);

    QStandardItemModel* GetModel();
    void UpdateModel(TrackModel trackModel);
    void InsertModel(TrackModel trackModel);
    void Remove(const int &trackId);

private:
    QStandardItemModel* m_model;
};

#endif // TRACKMODELVIEW_H
