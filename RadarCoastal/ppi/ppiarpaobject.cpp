#include "ppiarpaobject.h"
#include "qmath.h"
#include "utils.h"

PPIArpaObject::PPIArpaObject(QObject *parent): PPIObject(parent)
{
    m_re = RadarEngine::RadarEngine::getInstance();
}

void PPIArpaObject::draw(QPainter* painter, const int &side)
{
    if(m_re->radarArpa->m_number_of_targets > 0)
    {
        m_re->radarArpa->RefreshArpaTargets();

        const int preset_color = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt();

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

        for(int i = 0;i<m_re->radarArpa->m_number_of_targets;i++)
        {
            int a_min = MOD_ROTATION2048(m_re->radarArpa->m_target[i]->m_min_angle_future.angle); //337
            a_min = static_cast<int>SCALE_RAW_TO_DEGREES2048(a_min)+1;
            int r_max = 2*side*m_re->radarArpa->m_target[i]->m_max_r_future.r/RETURNS_PER_LINE;
            r_max += 5;

            x2 = static_cast<int>(r_max*qSin(deg2rad(a_min)))/2;
            y2 = -static_cast<int>(r_max*qCos(deg2rad(a_min)))/2;

            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawRect(x2-10,y2-10,20,20);

            target_text = QString::number(m_re->radarArpa->m_target[i]->m_target_id);
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


        for (int i = 0; i < m_re->radarArpa->m_number_of_targets; i++)
        {
//            qDebug()<<Q_FUNC_INFO<<"target"<<i<<m_re->radarArpa->m_target[i]->m_position.lat<<m_re->radarArpa->m_target[i]->m_position.lon<<m_re->radarArpa->m_target[i]->m_speed_kn;
        }

        painter->setPen(curPen);
    }
}

void PPIArpaObject::createMARPA(const QPoint& pos, const int vp_width, const int vp_height)
{
    qDebug()<<Q_FUNC_INFO<<pos;

    RadarConfig::RadarConfig* instance = RadarConfig::RadarConfig::getInstance("");

    if(!instance->getConfig(RadarConfig::NON_VOLATILE_ARPA_CONTROL_CREATE_ARPA_BY_CLICK).toBool()) return;

    double const curLat = instance->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
    double const curLon = instance->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
    double curRange = instance->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
//    const quint8 unit = static_cast<quint8>(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

//    switch (unit) {
//    case 1:
//        curRange *= KM_TO_NM;
//        break;
//    default:
//        break;
//    }

    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = pixToGPS(pos.x()-screen_middle.x(),-pos.y()+screen_middle.y(),vp_width,vp_height,curRange,curLat,curLon);

    RadarEngine::Position target_pos;
    target_pos.lat = gps.y();
    target_pos.lon = gps.x();

    qDebug()<<Q_FUNC_INFO<<target_pos.lat<<target_pos.lon;

    m_re->radarArpa->AcquireNewMARPATarget(target_pos);
}
