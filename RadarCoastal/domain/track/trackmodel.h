#ifndef TRACKMODEL_H
#define TRACKMODEL_H


class TrackModel
{
public:
    TrackModel(){}
    TrackModel(const TrackModel& track);

    int id;
    double lat;
    double lon;
    double alt;
    double rng;
    double brn;
    double spd;
    double crs;
};

#endif // TRACKMODEL_H
