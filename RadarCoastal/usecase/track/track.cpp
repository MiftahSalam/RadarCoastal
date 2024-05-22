#include "track.h"

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, Track)
#else
#include <QDebug>
#endif

#define MAX_UPDATE_NUMBER 2

Track *Track::m_track{nullptr};

Track::Track(QObject *parent)
    : QObject{parent}
{
    m_track_repo = TrackRepository::Create();
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    appConfig = ApplicationConfig::getInstance();
#ifdef DISPLAY_ONLY_MODE
    m_arpa_receiver = new ArpaReceiver(this);
#else
    m_instance_re = RadarEngine::RadarEngine::GetInstance();
    m_arpa_sender = new ArpaSender(this);
#endif
    m_model = new QStandardItemModel(this);
    m_model_view = new TrackModelView(m_model);
    m_timer = new QTimer(this);
    m_data_count_mqtt = 0;
    m_update_count = 0;
    m_cur_arpa_id_count = 0;

    connect(m_timer, &QTimer::timeout, this, &Track::timerTimeout);
#ifdef DISPLAY_ONLY_MODE
    connect(m_arpa_receiver, &ArpaReceiver::signalNewTrack, this, &Track::triggerOnNewTrack);
#else
    connect(m_instance_re->radarArpa, &RadarEngine::RadarArpa::Signal_LostTarget,
            this, &Track::trigger_LostTarget);
#endif

    m_model->setColumnCount(8);

    m_timer->start(1000);
}

#ifdef DISPLAY_ONLY_MODE
void Track::triggerOnNewTrack(QList<TrackModel *> tracks)
{
    if (tracks.size() <= 0)
    {
        qWarning() << Q_FUNC_INFO << "new tracks is empty";
        return;
    }

    for (auto track : tracks)
    {
        updateModel(track);
    }
}

void Track::updateAllTrack()
{
    auto now = QDateTime::currentMSecsSinceEpoch();
    auto targets = m_track_repo->FindAll();
    for (auto target : targets)
    {
        qDebug() << Q_FUNC_INFO << "target id" << target->id;
        qDebug() << Q_FUNC_INFO << "now" << now;
        qDebug() << Q_FUNC_INFO << "timestamp" << target->timestamp;
        qDebug() << Q_FUNC_INFO << "timeout" << now - target->timestamp;

        if (now - target->timestamp > 10000)
        {
            if (m_track_repo->FindOne(target->id))
            {
                m_track_repo->Remove(target->id);
                m_model_view->Remove(target->id);
                qInfo() << Q_FUNC_INFO << "delete stale target" << target->id;
            }
            else
                qWarning() << Q_FUNC_INFO << "failed to delete stale target" << target->id << ". cannot find target";
        }
    }
}

#endif

void Track::timerTimeout()
{
#ifdef DISPLAY_ONLY_MODE
    updateAllTrack();
#else
    updateManyTarget(MAX_UPDATE_NUMBER);
    //    updateOneTarget();
    //    updateAllTarget();
#endif
}

void Track::updateModel(TrackModel* trackModel)
{
    if (m_track_repo->FindOne(trackModel->id) != nullptr)
    {
        m_track_repo->Update(*trackModel);
        m_model_view->UpdateModel(*trackModel);
    }
    else
    {
        trackModel->timestamp = QDateTime::currentMSecsSinceEpoch();

        m_track_repo->Insert(*trackModel);
        m_model_view->InsertModel(*trackModel);
    }
}

Track *Track::GetInstance()
{
    if (!m_track)
        m_track = new Track();
    return m_track;
}

Track::~Track()
{
}

QStandardItemModel *Track::GetModelView() const
{
    return m_model;
}

TrackRepository *Track::GetRepo() const
{
    return m_track_repo;
}

#ifndef DISPLAY_ONLY_MODE
void Track::RemoveTrack(QString id_str)
{
    int id = id_str.toInt();
    for (int i = 0; i < m_instance_re->radarArpa->targetNumber; i++)
        if (m_instance_re->radarArpa->targets[i]->targetId == id)
            m_instance_re->radarArpa->targets[i]->SetStatusLost();
}

void Track::RemoveAllTrack()
{
    m_instance_re->radarArpa->DeleteAllTargets();
}

void Track::updateManyTarget(const int updateCount)
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif
    if (m_instance_re->radarArpa->targetNumber > 0)
    {
        int num_limit = updateCount;
        QList<TrackModel *> modelList;

        while ((m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber) && num_limit > 0)
        {
            if (m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                modelList.append(trackModel);
            }
            m_cur_arpa_id_count++;
            num_limit--;
        }
#ifdef USE_LOG4QT
        for (auto trk : modelList)
        {
            logger()->trace() << Q_FUNC_INFO << "id" << trk->id << "ts" << trk->timestamp;
        }
#else
#endif
        if (modelList.size() > 0)
            m_arpa_sender->SendManyData(modelList);

        if (m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

void Track::updateAllTarget()
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif
    if (m_instance_re->radarArpa->targetNumber > 0)
    {
        QList<TrackModel *> modelList;

        while (m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber)
        {
            if (m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                modelList.append(trackModel);
            }
            m_cur_arpa_id_count++;
        }
        if (modelList.size() > 0)
            m_arpa_sender->SendManyData(modelList);

        if (m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

void Track::updateOneTarget()
{
#ifdef USE_LOG4QT
    logger()->trace() << Q_FUNC_INFO;
#else
    qDebug() << Q_FUNC_INFO;
#endif
    if (m_instance_re->radarArpa->targetNumber > 0)
    {
        int num_limit = 1;

        while ((m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber) && num_limit > 0)
        {
            if (m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                m_arpa_sender->SendOneData(*trackModel);
            }
            m_cur_arpa_id_count++;
            num_limit--;
        }

        if (m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

TrackModel Track::arpaToTrackModel(const RadarEngine::ARPATarget *target)
{
    RadarEngine::Position own_pos;
    Polar pol;
    double brn;
    double range;
    const bool heading_up = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
    const double currentHeading = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();

    own_pos.lat = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    own_pos.lon = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    //        const quint8 unit = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    //        switch (unit) {
    //        case 1:
    //            curRange *= KM_TO_NM;
    //            break;
    //        default:
    //            break;
    //        }

    pol = Pos2Polar(target->position, own_pos, curRange);
    brn = SCALE_RAW_TO_DEGREES2048(pol.angle);
    //                brn -= 270;
    brn = heading_up ? brn + currentHeading : brn;
    while (brn > 360 || brn < 0)
    {
        if (brn > 360)
            brn -= 360;
        if (brn < 0)
            brn += 360;
    }

    double arpa_course = target->course;
    //                arpa_course -= 270;
    arpa_course = heading_up ? arpa_course + currentHeading : arpa_course;
    while (arpa_course > 360 || arpa_course < 0)
    {
        if (arpa_course > 360)
            arpa_course -= 360;
        if (arpa_course < 0)
            arpa_course += 360;
    }

    range = static_cast<double>(curRange * pol.r / RETURNS_PER_LINE) / 1000.;

    const quint8 unit = static_cast<quint8>(appConfig->getUnit());
    switch (unit)
    {
    case 1:
        range *= KM_TO_NM;
        break;
    default:
        break;
    }

    TrackModel trackModel;
    trackModel.id = target->targetId;
    trackModel.lat = target->position.lat;
    trackModel.lon = target->position.lon;
    trackModel.rng = range;
    trackModel.brn = brn;
    trackModel.spd = target->speedKts;
    trackModel.crs = arpa_course;
    trackModel.alt = 0.;

    return trackModel;
    /* untuk menghitung posisi yang sudah dikoreksi
        pol.angle = SCALE_DEGREES_TO_RAW2048(brn);
        Position arpa_pos = Polar2Pos(pol,own_pos,curRange);
        */
    //                qDebug()<<Q_FUNC_INFO<<arpa->m_target[m_m_instance_re]->m_position.lat<<arpa->m_target[m_m_instance_re]->m_position.lon;
}

void Track::trigger_LostTarget(int id)
{
#ifdef USE_LOG4QT
    logger()->info() << Q_FUNC_INFO << id;
#else
    qDebug() << Q_FUNC_INFO << id;
#endif

    m_track_repo->Remove(id);
    m_model_view->Remove(id);
}
#endif
