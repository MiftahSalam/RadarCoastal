#ifndef TRACKREPOSITORY_MEM_H
#define TRACKREPOSITORY_MEM_H

#include <QStandardItemModel>

#include "domain/track/trackrepository.h"

class TrackRepository_Mem: public TrackRepository
{
public:
    TrackRepository_Mem();

    virtual ~TrackRepository_Mem(){ }
    virtual void Insert(const TrackModel& trackModel);
    virtual TrackModel* FindOne(const int& trackId);
    virtual std::list<TrackModel*> FindAll();
    virtual void Update(TrackModel& trackModel);
    virtual void Remove(const int& trackId);

    QMap<QString, TrackModel*> model;
};

#endif // TRACKREPOSITORY_MEM_H
