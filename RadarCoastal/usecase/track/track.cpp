#include "track.h"

#define MAX_UPDATE_NUMBER 2

Track* Track::m_track{nullptr};

Track::Track(QObject *parent)
    : QObject{parent}
{
    m_track_repo = TrackRepository::Create();
    m_instance_re = RadarEngine::RadarEngine::GetInstance();
    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_arpa_sender = new ArpaSender(this);
    m_model = new QStandardItemModel(this);
    m_model_view = new TrackModelView(m_model);
    m_timer = new QTimer(this);
    m_data_count_mqtt = 0;
    m_update_count = 0;
    m_cur_arpa_id_count = 0;

    initCfg();

    connect(m_instance_re->radarArpa,&RadarEngine::RadarArpa::Signal_LostTarget,
            this,&Track::trigger_LostTarget);
    connect(m_timer,&QTimer::timeout,this,&Track::timerTimeout);

    m_model->setColumnCount(8);

    m_timer->start(1000);

}

void Track::RemoveTrack(QString id_str)
{
    int id = id_str.toInt();
    for(int i=0;i<m_instance_re->radarArpa->targetNumber;i++)
        if(m_instance_re->radarArpa->targets[i]->targetId == id)
            m_instance_re->radarArpa->targets[i]->SetStatusLost();
}

void Track::RemoveAllTrack()
{
    m_instance_re->radarArpa->DeleteAllTargets();
}

QStandardItemModel* Track::GetModelView() const
{
    return m_model;
}

void Track::updateManyTarget(const int updateCount)
{
    qDebug()<<Q_FUNC_INFO;
    if(m_instance_re->radarArpa->targetNumber > 0)
    {
        int num_limit = updateCount;
        QList<TrackModel*> modelList;

        while ((m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber) && num_limit > 0)
        {
            if(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                modelList.append(trackModel);
            }
            m_cur_arpa_id_count++;
            num_limit--;
        }
        for (auto trk : modelList) {
            qDebug()<<Q_FUNC_INFO<<"id"<<trk->id<<"ts"<<trk->timestamp;
        }
        if (modelList.size() > 0) m_arpa_sender->SendManyData(modelList);

        if(m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

void Track::updateAllTarget()
{
    qDebug()<<Q_FUNC_INFO;
    if(m_instance_re->radarArpa->targetNumber > 0)
    {
        QList<TrackModel*> modelList;

        while (m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber)
        {
            if(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                modelList.append(trackModel);
            }
            m_cur_arpa_id_count++;
        }
        if (modelList.size() > 0) m_arpa_sender->SendManyData(modelList);

        if(m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

void Track::updateOneTarget()
{
    qDebug()<<Q_FUNC_INFO;
    if(m_instance_re->radarArpa->targetNumber > 0)
    {
        int num_limit = 1;

        while ((m_cur_arpa_id_count < m_instance_re->radarArpa->targetNumber) && num_limit > 0)
        {
            if(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]->targetId > 0)
            {
                TrackModel* trackModel = new TrackModel(arpaToTrackModel(m_instance_re->radarArpa->targets[m_cur_arpa_id_count]));
                updateModel(trackModel);
                m_arpa_sender->SendOneData(*trackModel);
            }
            m_cur_arpa_id_count++;
            num_limit--;
        }

        if(m_cur_arpa_id_count >= m_instance_re->radarArpa->targetNumber)
            m_cur_arpa_id_count = 0;
    }
}

void Track::updateModel(TrackModel* trackModel)
{
    if (m_track_repo->FindOne(trackModel->id) != nullptr) {
        m_track_repo->Update(*trackModel);
        m_model_view->UpdateModel(*trackModel);
    } else {
        trackModel->timestamp = QDateTime::currentMSecsSinceEpoch();

        m_track_repo->Insert(*trackModel);
        m_model_view->InsertModel(*trackModel);
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

    pol = Pos2Polar(target->position,own_pos,curRange);
    brn = SCALE_RAW_TO_DEGREES2048(pol.angle);
    //                brn -= 270;
    brn = heading_up ? brn+currentHeading : brn;
    while(brn>360 || brn<0)
    {
        if(brn>360)
            brn -= 360;
        if(brn<0)
            brn += 360;
    }

    double arpa_course = target->course;
    //                arpa_course -= 270;
    arpa_course = heading_up ? arpa_course+currentHeading : arpa_course;
    while(arpa_course>360 || arpa_course<0)
    {
        if(arpa_course>360)
            arpa_course -= 360;
        if(arpa_course<0)
            arpa_course += 360;
    }

    range = static_cast<double>(curRange*pol.r/RETURNS_PER_LINE)/1000.;

    const quint8 unit = static_cast<quint8>(RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());
    switch (unit) {
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

void Track::timerTimeout()
{
    m_arpa_sender->Reconnect();
    updateManyTarget(m_update_count);
//    updateOneTarget();
//    updateAllTarget();
}

void Track::trigger_LostTarget(int id)
{
    qDebug()<<Q_FUNC_INFO<<id;

    m_track_repo->Remove(id);
    m_model_view->Remove(id);
}

void Track::initCfg()
{
    QString config_ws_str = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_ARPA_NET_CONFIG_WS).toString();
    QStringList config_ws_str_list$ = config_ws_str.split("$");
    if(config_ws_str_list$.size() != 2)
    {
        qDebug()<<Q_FUNC_INFO<<"invalid config ws arpa max size"<<config_ws_str;
        exit(1);
    }

    bool ok;
    m_update_count = config_ws_str_list$.at(1).toInt(&ok);
    if (!ok) {
        m_update_count = 5;
        qWarning()<<Q_FUNC_INFO<<"invalid max_arpa_data_count"<<config_ws_str_list$.at(1)<<". will use default 5";
    }

}

Track* Track::GetInstance()
{
    if(!m_track) m_track = new Track();
    return m_track;
}

Track::~Track()
{
}
