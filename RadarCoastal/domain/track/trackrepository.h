#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H

#include <list>

#include "domain/track/trackmodel.h"

class TrackRepository
{
public:
    static TrackRepository* Create();

    virtual ~TrackRepository(){}

    virtual void Insert(const TrackModel& TrackModel) = 0;
    virtual TrackModel* FindOne(const int& trackId) = 0;
    virtual std::list<TrackModel*> FindAll() = 0;
    virtual void Update(TrackModel& TrackModel) = 0;
    virtual void Remove(const int& trackId) = 0;

protected:
    TrackRepository() {}
    TrackRepository(const TrackRepository& track) {}

private:
    TrackRepository& operator=(
            const TrackRepository& track);
};

#endif // TRACKREPOSITORY_H
