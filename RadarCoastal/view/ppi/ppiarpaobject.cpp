#include "ppiarpaobject.h"
#include "qmath.h"
#include "shared/utils.h"

#ifndef DISPLAY_ONLY_MODE
PPIArpaObject::PPIArpaObject(QObject *parent,PPIArpa *arpa): PPIObject(parent)
{
    m_ppi_arpa = arpa;
}
#else
PPIArpaObject::PPIArpaObject(QObject *parent): PPIObject(parent)
{
    m_track = Track::GetInstance();
}
#endif

void PPIArpaObject::Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(off_center)

#ifndef DISPLAY_ONLY_MODE
    if(m_ppi_arpa->m_re->radarArpa->targetNumber > 0)
    {
        m_ppi_arpa->m_re->radarArpa->RefreshArpaTargets();
#else
    if(m_track->GetRepo()->FindAll().size() > 0)
    {
#endif
        const int preset_color = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR).toInt();

        QPen curPen = painter->pen();

        if(preset_color == 0)
            painter->setPen(QColor(255,255,255,255));
        else if(preset_color == 1)
            painter->setPen(QColor(0,0,0,255));


        int x2,y2,txtX,txtY;
        QTextOption opt;
        QFont font;

        opt.setAlignment(Qt::AlignHCenter);
        font.setPixelSize(15);
        painter->setFont(font);

        QString target_text;
        QFontMetrics metric = QFontMetrics(font);
        QRect rect = metric.boundingRect(0,0,side, int(side*0.125),
                                         Qt::AlignCenter | Qt::TextWordWrap, target_text);

        QPen pen(QColor(255,255,255,255));
        pen.setWidth(2);
        painter->setPen(pen);

#ifndef DISPLAY_ONLY_MODE
        for(int i = 0;i<m_ppi_arpa->m_re->radarArpa->targetNumber;i++)
        {
            int a_min = MOD_ROTATION2048(m_ppi_arpa->m_re->radarArpa->targets[i]->minAngleFuture.angle); //337
            a_min = static_cast<int>SCALE_RAW_TO_DEGREES2048(a_min)+1;
            int r_max = 2*side*m_ppi_arpa->m_re->radarArpa->targets[i]->maxAngleFuture.r/RETURNS_PER_LINE;
            r_max += 5;

            x2 = static_cast<int>(r_max*qSin(deg2rad(a_min)))/2;
            y2 = -static_cast<int>(r_max*qCos(deg2rad(a_min)))/2;

            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawRect(x2-10,y2-10,20,20);

            target_text = QString::number(m_ppi_arpa->m_re->radarArpa->targets[i]->targetId);
            rect = metric.boundingRect(0,0,side, int(side*0.125),
                                       Qt::AlignCenter | Qt::TextWordWrap, target_text);
            txtX = x2 + 10;
            txtY = y2 + 10;
            /*
                    txtX = x1 - (rect.width()*qSin(deg2rad(a_min))) - 5;
                    txtY = y1 - (rect.height()*qSin(deg2rad(a_min/2))) -5;
                    */
            pen.setWidth(1);
            painter->setPen(pen);
            painter->drawText(txtX,txtY,rect.width(), rect.height(),
                             Qt::AlignCenter | Qt::TextWordWrap, target_text);
        }


        for (int i = 0; i < m_ppi_arpa->m_re->radarArpa->targetNumber; i++)
        {
//            qDebug()<<Q_FUNC_INFO<<"target"<<i<<m_re->radarArpa->m_target[i]->m_position.lat<<m_re->radarArpa->m_target[i]->m_position.lon<<m_re->radarArpa->m_target[i]->m_speed_kn;
        }
#else
        const double range = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
        const double own_lat = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
        const double own_lon = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
        const double hdt = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
        const bool hup = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toDouble();
        foreach (auto track, m_track->GetRepo()->FindAll())
        {
            auto pixPos = Utils::GPSToPix(track->lon, track->lat, 2*side, 2*side, range, own_lat, own_lon).toPoint();

            if(hup)
            {
                QTransform transform;
                transform.rotate(hdt);
                pixPos = transform.map(pixPos);
            }

            x2 = pixPos.x();
            y2 = -pixPos.y();


            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawRect(x2-10, y2-10, 20, 20);

            target_text = QString::number(track->id);
            rect = metric.boundingRect(0,0,side, int(side*0.125),
                                       Qt::AlignCenter | Qt::TextWordWrap, target_text);
            txtX = x2 + 10;
            txtY = y2 + 10;

            pen.setWidth(1);
            painter->setPen(pen);
            painter->drawText(txtX,txtY,rect.width(), rect.height(), Qt::AlignCenter | Qt::TextWordWrap, target_text);

//            qDebug()<<Q_FUNC_INFO<<"target"<<track->id<<track->lat<<track->lon<<track->spd;
//            qDebug()<<Q_FUNC_INFO<<"position:"<<x2<<y2;
        }
#endif
        painter->setPen(curPen);
    }
}
