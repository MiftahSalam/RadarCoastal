#ifndef TRACK_H
#define TRACK_H

#include <QTimer>
#include <QStandardItemModel>

#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

#include "domain/track/trackrepository.h"
#include "shared/config/applicationconfig.h"
#ifdef DISPLAY_ONLY_MODE
#include "usecase/stream/arpareceiver.h"
#else
#include "usecase/stream/arpasender.h"
#endif
#include "usecase/track/trackmodelview.h"

class Track: public QObject
{
    Q_OBJECT

public:
    Track(Track& other) = delete;
    void operator=(const Track&) = delete;
    ~Track() override;

    QStandardItemModel *GetModelView() const;
    TrackRepository* GetRepo() const;
#ifndef DISPLAY_ONLY_MODE
    void RemoveTrack(QString id);
    void RemoveAllTrack();
#endif
    static Track* GetInstance();

protected:
    explicit Track(QObject *parent = nullptr);

private slots:
    void timerTimeout();
#ifdef DISPLAY_ONLY_MODE
    void triggerOnNewTrack(QList<TrackModel*> tracks);
#else
    void trigger_LostTarget(int id);
#endif

private:
    static Track* m_track;

    QTimer *m_timer;
#ifdef DISPLAY_ONLY_MODE
    ArpaReceiver *m_arpa_receiver;

    void updateAllTrack();
#else
    ArpaSender *m_arpa_sender;

    void updateManyTarget(const int updateCount);
    void updateOneTarget();
    void updateAllTarget();
    void initCfg();
    TrackModel arpaToTrackModel(const RadarEngine::ARPATarget* target);
    RadarEngine::RadarEngine* m_instance_re;
#endif
    RadarEngine::RadarConfig* m_instance_cfg;
    ApplicationConfig *appConfig;
    TrackRepository* m_track_repo;
    QStandardItemModel* m_model;
    TrackModelView* m_model_view;
    int m_data_count_mqtt, m_update_count, m_cur_arpa_id_count;

    void updateModel(TrackModel *trackModel);
   };

#endif // TRACK_H
