#include "ppigzobject.h"
#include "qmath.h"

PPIGZObject::PPIGZObject(QObject *parent): PPIObject(parent)
{
    m_re = RadarEngine::RadarEngine::getInstance();
}

void PPIGZObject::draw(QPainter* painter, const int &side)
{
    const bool show_gz = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    if(show_gz)
    {
//        qDebug()<<Q_FUNC_INFO<<"bogey"<<m_re->guardZone->GetBogeyCount();

        const bool heading_up = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toInt();
//        const int preset_color = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt();
        const int inner_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_START_RANGE).toInt();
        const int outer_range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_END_RANGE).toInt();
        const int start_bearing = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_START_BEARING).toInt();
        const int end_bearing = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_END_BEARING).toInt();
        const int curRange = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
        const int mode = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_GZ_MODE).toInt();
        const double bearing = heading_up ? 0. : RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

        QPen curPen = painter->pen();

        //    painter.rotate(bearing-180);
        QPen pen(Qt::yellow);
        pen.setWidth(2);
        painter->setPen(pen);

        int inner_range_pix = static_cast<int>(static_cast<double>(side)*
                                               static_cast<double>(inner_range)/static_cast<double>(curRange));
        int outer_range_pix = static_cast<int>(static_cast<double>(side)*
                                               static_cast<double>(outer_range)/static_cast<double>(curRange));

        QRect rectangle;

        if(mode == 0)
        {
            /*
                        int inner_start_x = static_cast<int>(static_cast<double>(inner_range_pix)*
                                qSin(deg2rad(90-bearing+gz_sett.start_bearing)));
                        int inner_start_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
                                qCos(deg2rad(90-bearing+gz_sett.start_bearing)));
                        int outter_start_x = static_cast<int>(static_cast<double>(outer_range_pix)*
                                qSin(deg2rad(90-bearing+gz_sett.start_bearing)));
                        int outter_start_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
                                qCos(deg2rad(90-bearing+gz_sett.start_bearing)));
                        int inner_end_x = static_cast<int>(static_cast<double>(inner_range_pix)*
                                qSin(deg2rad(90-bearing+gz_sett.end_bearing)));
                        int inner_end_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
                                qCos(deg2rad(90-bearing+gz_sett.end_bearing)));
                        int outter_end_x = static_cast<int>(static_cast<double>(outer_range_pix)*
                                qSin(deg2rad(90-bearing+gz_sett.end_bearing)));
                        int outter_end_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
                                qCos(deg2rad(90-bearing+gz_sett.end_bearing)));
                        */
//            bearing = 0.; //always headup
            int inner_start_x = static_cast<int>(static_cast<double>(inner_range_pix)*
                                                 qSin(deg2rad(bearing+start_bearing)));
            int inner_start_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
                                                 qCos(deg2rad(bearing+start_bearing)));
            int outter_start_x = static_cast<int>(static_cast<double>(outer_range_pix)*
                                                  qSin(deg2rad(bearing+start_bearing)));
            int outter_start_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
                                                  qCos(deg2rad(bearing+start_bearing)));
            int inner_end_x = static_cast<int>(static_cast<double>(inner_range_pix)*
                                               qSin(deg2rad(bearing+end_bearing)));
            int inner_end_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
                                               qCos(deg2rad(bearing+end_bearing)));
            int outter_end_x = static_cast<int>(static_cast<double>(outer_range_pix)*
                                                qSin(deg2rad(bearing+end_bearing)));
            int outter_end_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
                                                qCos(deg2rad(bearing+end_bearing)));

            int startAngle;
            int spanAngle;

            //        qDebug()<<inner_range_pix<<outer_range_pix<<side<<gz_sett.inner_range<<gz_sett.outer_range<<curRange;
            painter->drawLine(inner_start_x,inner_start_y,outter_start_x,outter_start_y);
            painter->drawLine(inner_end_x,inner_end_y,outter_end_x,outter_end_y);

            if(start_bearing<end_bearing)
            {
                rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
                startAngle = static_cast<int>((90-(bearing+end_bearing)) * 16.);
                //                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16.);
                spanAngle = static_cast<int>((end_bearing-start_bearing) * 16.);

                painter->drawArc(rectangle, startAngle, spanAngle);

                rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
                startAngle = static_cast<int>((90-(bearing+end_bearing)) * 16.);
                //                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16.);
                spanAngle = static_cast<int>((end_bearing-start_bearing) * 16.);

                painter->drawArc(rectangle, startAngle, spanAngle);
            }
            else
            {
                rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
                startAngle = static_cast<int>((90-(bearing+end_bearing)) * 16);
                //                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16);
                spanAngle = static_cast<int>((end_bearing-start_bearing+360) * 16);

                painter->drawArc(rectangle, startAngle, spanAngle);

                rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
                startAngle = static_cast<int>((90-(bearing+end_bearing)) * 16);
                spanAngle = static_cast<int>((end_bearing-start_bearing+360) * 16);

                painter->drawArc(rectangle, startAngle, spanAngle);

            }
        }
        else
        {
            rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
            painter->drawEllipse(rectangle);

            rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
            painter->drawEllipse(rectangle);
        }

        painter->setPen(curPen);
    }

}
