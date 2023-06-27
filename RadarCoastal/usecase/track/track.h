#ifndef TRACK_H
#define TRACK_H

#include <QTimer>
#include <QStandardItemModel>

#include "domain/track/trackrepository.h"
#include "usecase/stream/arpasender.h"
#include "usecase/track/trackmodelview.h"
#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

class Track: public QObject
{
    Q_OBJECT

public:
    Track(Track& other) = delete;
    void operator=(const Track&) = delete;
    ~Track() override;

    QStandardItemModel *GetModelView() const;
    void RemoveTrack(QString id);
    void RemoveAllTrack();

    static Track* GetInstance();

protected:
    explicit Track(QObject *parent = nullptr);

private slots:
    void timerTimeout();
    void trigger_LostTarget(int id);

private:
    static Track* m_track;

    QTimer *m_timer;
    ArpaSender *m_arpa_sender;
    RadarEngine::RadarEngine* m_instance_re;
    RadarEngine::RadarConfig* m_instance_cfg;
    TrackRepository* m_track_repo;
    QStandardItemModel* m_model;
    TrackModelView* m_model_view;
    int m_data_count_mqtt, m_update_count, m_cur_arpa_id_count;

    void updateTarget();
    TrackModel arpaToTrackModel(const RadarEngine::ARPATarget* target);
};

#endif // TRACK_H
