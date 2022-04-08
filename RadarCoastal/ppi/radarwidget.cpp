#include <QtGui>
#include <QtOpenGL>
#include <QtGlobal>
#include <stdlib.h>
#include <math.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include "radarwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

RadarWidget::RadarWidget(QWidget *parent, RadarEngine::RadarEngine *re)
    : QOpenGLWidget(parent),m_re(re)
{

//    counter = 0;
//    spokeDrawer = RD::make_Draw(m_ri,0);
//    cur_radar_angle = 0.;

//    arpa = new RA(this,re);

    timer  = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
    timer->start(100);

    setAutoFillBackground(false);
    setMinimumSize(200, 200);

//    ppiEvent = new PPIEvent(this);
//    installEventFilter(ppiEvent);
//    setMouseTracking(true);

//    old_motion_mode = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
//    curRange = 0;
//    cur_arpa_id_count = 0;
//    cur_arpa_number = 0;
//    arpa_measure_time = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch());
}


//void RadarWidget::trigger_ReqDelTrack(bool r1,int id)
//{
//    if(id>-10)
//    {
//        RA *cur_arpa = r1 ? arpa : arpa1;
//        for(int i=0;i<cur_arpa->m_number_of_targets;i++)
//            if(cur_arpa->m_target[i]->m_target_id == id)
//                cur_arpa->m_target[i]->SetStatusLost();
//    }
//    else
//    {
//        arpa->DeleteAllTargets();
//        arpa1->DeleteAllTargets();
//    }
//}

void RadarWidget::timeOut()
{
//    Position own_pos;
//    own_pos.lat = currentOwnShipLat;
//    own_pos.lon = currentOwnShipLon;
//    Polar pol;
//    double brn;
//    double range;

//    qDebug()<<Q_FUNC_INFO;
//    if(arpa->m_number_of_targets > 0)
//    {
//        int num_limit = 5;
//        while ((cur_arpa_id_count < arpa->m_number_of_targets) && num_limit > 0)
//        {
//            if(arpa->m_target[cur_arpa_id_count]->m_target_id > 0)
//            {
//                pol = Pos2Polar(arpa->m_target[cur_arpa_id_count]->m_position,own_pos,curRange);
//                brn = SCALE_RAW_TO_DEGREES2048(pol.angle);
////                brn -= 270;
//                brn = radar_settings.headingUp ? brn+currentHeading : brn;
//                while(brn>360 || brn<0)
//                {
//                    if(brn>360)
//                        brn -= 360;
//                    if(brn<0)
//                        brn += 360;
//                }

//                double arpa_course = arpa->m_target[cur_arpa_id_count]->m_course;
////                arpa_course -= 270;
//                arpa_course = radar_settings.headingUp ? arpa_course+currentHeading : arpa_course;
//                while(arpa_course>360 || arpa_course<0)
//                {
//                    if(arpa_course>360)
//                        arpa_course -= 360;
//                    if(arpa_course<0)
//                        arpa_course += 360;
//                }

//                range = static_cast<double>(curRange*pol.r/RETURNS_PER_LINE)/1000.;
//                /* untuk menghitung posisi yang sudah dikoreksi
//                pol.angle = SCALE_DEGREES_TO_RAW2048(brn);
//                Position arpa_pos = Polar2Pos(pol,own_pos,curRange);
//                */
////                qDebug()<<Q_FUNC_INFO<<arpa->m_target[cur_arpa_id_count]->m_position.lat<<arpa->m_target[cur_arpa_id_count]->m_position.lon;
//                emit signal_target_param(true,
//                                         arpa->m_target[cur_arpa_id_count]->m_target_id,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.lat,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.lon,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.alt, //temp
//                                         range,
//                                         brn,
//                                         arpa->m_target[cur_arpa_id_count]->m_speed_kn,
//                                         arpa_course
//                                         );
//            }
//            cur_arpa_id_count++;
//            num_limit--;
//        }
//        if(cur_arpa_id_count >= arpa->m_number_of_targets)
//            cur_arpa_id_count = 0;
//    }

//    if(old_motion_mode^radar_settings.headingUp)
//    {
//        arpa->DeleteAllTargets();
//        arpa1->DeleteAllTargets();
//        old_motion_mode = radar_settings.headingUp;
//    }

//    if(state_radar != RADAR_TRANSMIT)
//        cur_radar_angle = 0.;

//    if(state_radar1 != RADAR_TRANSMIT)
//        cur_radar_angle1 = 0.;

    update();
}

void RadarWidget::drawCompass(QPainter *painter, const int &side, const bool &heading_up, const double &currentHeading)
{
    /*
      compass ring text
*/
    int brnCor = 0;
    int brn;
    double buf_j;
    int margin_a = 5;
    int margin_b = 15;

    if(heading_up)
        brnCor =  -static_cast<int>(currentHeading);

    QString text;
    for(int j=0;j<12;j++)
    {
        /*
        brn = (j*30)+180;
        brn = brn == 360 ? 0 : brn;

        while(brn>360 || brn<0)
        {
            if(brn>360)
                brn -= 360;
            if(brn<0)
                brn += 360;
        }
        */

        if(j<9)
            brn = (j*30)+90;
        else
            brn = (j*30)-270;

        QTextStream(&text)<<brn;

        buf_j = (j*30)+brnCor;
        /*
        */
        while(buf_j>360 || buf_j<0)
        {
            if(buf_j>360)
                buf_j -= 360;
            if(buf_j<0)
                buf_j += 360;
        }

        QRect rect((side-20)*cos((buf_j)*M_PI/180)-margin_b,
                   ((side-20)*sin((buf_j)*M_PI/180)-margin_a),
                   30,
                   15);
        QTextOption opt;
        opt.setAlignment(Qt::AlignHCenter);
        QFont font;

        font.setPixelSize(12);
        painter->setFont(font);
        painter->drawText(rect,text,opt);
        text.clear();
    }

    /*
          compass ring
    */

    if(heading_up)
        painter->rotate(-currentHeading);

    for(int j=0;j<180;j++)
    {
        margin_a = 10;
        margin_b = 5;

        if(j%15==0)
            painter->drawLine(0,side,0,side-margin_a);
        else
            painter->drawLine(0,side,0,side-margin_b);

        painter->rotate(2);
    }

    if(heading_up)
        painter->rotate(currentHeading);

}

void RadarWidget::drawRings(QPainter *painter, const int &side)
{
    int ringCount = qCeil(side/2)-20;
    int bufRng = ringCount;

    painter->setPen(QColor(255,255,0,100));
    for(int i=0;i<RING_COUNT;i++)
    {
        painter->drawEllipse(-bufRng/2,-bufRng/2,bufRng,bufRng);
        bufRng += ringCount;
    }

}

void RadarWidget::drawHM(QPainter *painter, const int &side, const bool& heading_up, const double& currentHeading)
{
    double baringan = heading_up ? 0 : currentHeading;
    painter->rotate(baringan-90);
    //        painter.rotate(baringan-180);
    painter->setPen(QColor(255,255,0,255));
    painter->drawLine(0,0,side,0);
    //        painter.rotate(180-baringan);
    painter->rotate(90-baringan);
}

void RadarWidget::drawArpa(QPainter *painter)
{
    Q_UNUSED(painter)

    //    quint64 now = static_cast<quint64>(QDateTime::currentMSecsSinceEpoch());

    //    if(TIMED_OUT(now,arpa_measure_time+200))
    //    {
    //        arpa_measure_time = now;
    //        if(arpa->m_number_of_targets>0)
    //            arpa->RefreshArpaTargets();

    //    }

        /* test draw arpa
        QLine line1 = QLine(90,-424,92,-436);
        QLine line2 = QLine(92,-436,-7,-445);
        QLine line3 = QLine(-7,-445,-7,-443);
        QLine line4 = QLine(-7,-443,90,-424);
        QVector<QLine> lines;
        lines.append(line1);
        lines.append(line2);
        lines.append(line3);
        lines.append(line4);
        painter.drawLines(lines);
        */
    //    if(arpa->m_number_of_targets>0 && arpa_settings[0].show)
    //    {
    //        int x1,x2,x3,x4,y1,y2,y3,y4,txtX,txtY;
    //        QLine line1,line2,line3,line4;
    //        QVector<QLine> lines;
    //        QTextOption opt;
    //        QFont font;

    //        opt.setAlignment(Qt::AlignHCenter);
    //        font.setPixelSize(15);
    //        painter.setFont(font);

    //        QString target_text;
    //        QFontMetrics metric = QFontMetrics(font);
    //        QRect rect = metric.boundingRect(0,0,side, int(side*0.125),
    //                                         Qt::AlignCenter | Qt::TextWordWrap, target_text);

    //        QPen pen(QColor(255,255,255,255));
    //        pen.setWidth(2);
    //        painter.setPen(pen);

    //        for(int i = 0;i<arpa->m_number_of_targets;i++)
    //        {
    //            int a_max = MOD_ROTATION2048(arpa->m_target[i]->m_max_angle_future.angle); //363
    //            a_max = SCALE_RAW_TO_DEGREES2048(a_max);
    //            int a_min = MOD_ROTATION2048(arpa->m_target[i]->m_min_angle_future.angle); //337
    //            a_min = SCALE_RAW_TO_DEGREES2048(a_min)+1;
    //            int r_min = 2*side*arpa->m_target[i]->m_min_r_future.r/RETURNS_PER_LINE;
    //            int r_max = 2*side*arpa->m_target[i]->m_max_r_future.r/RETURNS_PER_LINE;
    //            r_max += 5;

    //            x1 = static_cast<int>(r_min*qSin(deg2rad(a_min)));
    //            x2 = static_cast<int>(r_max*qSin(deg2rad(a_min)));
    //            x3 = static_cast<int>(r_min*qSin(deg2rad(a_max)));
    //            x4 = static_cast<int>(r_max*qSin(deg2rad(a_max)));
    //            y1 = -static_cast<int>(r_min*qCos(deg2rad(a_min)));
    //            y2 = -static_cast<int>(r_max*qCos(deg2rad(a_min)));
    //            y3 = -static_cast<int>(r_min*qCos(deg2rad(a_max)));
    //            y4 = -static_cast<int>(r_max*qCos(deg2rad(a_max)));

    //            pen.setWidth(2);
    //            painter.setPen(pen);
    //            painter.drawRect(x2-10,y2-10,20,20);

    //            target_text = QString::number(arpa->m_target[i]->m_target_id);
    //            rect = metric.boundingRect(0,0,side, int(side*0.125),
    //                                       Qt::AlignCenter | Qt::TextWordWrap, target_text);
    //            txtX = x2 + 10;
    //            txtY = y2 + 10;
    //            /*
    //            txtX = x1 - (rect.width()*qSin(deg2rad(a_min))) - 5;
    //            txtY = y1 - (rect.height()*qSin(deg2rad(a_min/2))) -5;
    //            */
    //            pen.setWidth(1);
    //            painter.setPen(pen);
    //            painter.drawText(txtX,txtY,rect.width(), rect.height(),
    //                             Qt::AlignCenter | Qt::TextWordWrap, target_text);
    //        }
    //    }

}

void RadarWidget::drawGZ(QPainter *painter)
{
    Q_UNUSED(painter)


    //    double bearing = radar_settings.headingUp ? 0 : currentHeading;
    ////    painter.rotate(bearing-180);
    //    for(int gz_i=0; gz_i<3; gz_i++)
    //    {
    //        GZSettings gz_sett = gz_settings[gz_i];
    //        if(gz_sett.show)
    //        {
    //            QPen pen(Qt::yellow);
    //            pen.setWidth(2);
    //            painter.setPen(pen);

    //            int inner_range_pix = static_cast<int>(static_cast<double>(side)
    //                                                   *(gz_sett.inner_range/static_cast<double>(curRange)));
    //            int outer_range_pix = static_cast<int>(static_cast<double>(side)
    //                                                   *(gz_sett.outer_range/static_cast<double>(curRange)));

    //            QRect rectangle;

    //            if(!gz_sett.circle_type)
    //            {
    //                /*
    //                int inner_start_x = static_cast<int>(static_cast<double>(inner_range_pix)*
    //                        qSin(deg2rad(90-bearing+gz_sett.start_bearing)));
    //                int inner_start_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
    //                        qCos(deg2rad(90-bearing+gz_sett.start_bearing)));
    //                int outter_start_x = static_cast<int>(static_cast<double>(outer_range_pix)*
    //                        qSin(deg2rad(90-bearing+gz_sett.start_bearing)));
    //                int outter_start_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
    //                        qCos(deg2rad(90-bearing+gz_sett.start_bearing)));
    //                int inner_end_x = static_cast<int>(static_cast<double>(inner_range_pix)*
    //                        qSin(deg2rad(90-bearing+gz_sett.end_bearing)));
    //                int inner_end_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
    //                        qCos(deg2rad(90-bearing+gz_sett.end_bearing)));
    //                int outter_end_x = static_cast<int>(static_cast<double>(outer_range_pix)*
    //                        qSin(deg2rad(90-bearing+gz_sett.end_bearing)));
    //                int outter_end_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
    //                        qCos(deg2rad(90-bearing+gz_sett.end_bearing)));
    //                */
    //                bearing = 0.; //always headup
    //                int inner_start_x = static_cast<int>(static_cast<double>(inner_range_pix)*
    //                        qSin(deg2rad(bearing+gz_sett.start_bearing)));
    //                int inner_start_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
    //                        qCos(deg2rad(bearing+gz_sett.start_bearing)));
    //                int outter_start_x = static_cast<int>(static_cast<double>(outer_range_pix)*
    //                        qSin(deg2rad(bearing+gz_sett.start_bearing)));
    //                int outter_start_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
    //                        qCos(deg2rad(bearing+gz_sett.start_bearing)));
    //                int inner_end_x = static_cast<int>(static_cast<double>(inner_range_pix)*
    //                        qSin(deg2rad(bearing+gz_sett.end_bearing)));
    //                int inner_end_y = static_cast<int>(static_cast<double>(-inner_range_pix)*
    //                        qCos(deg2rad(bearing+gz_sett.end_bearing)));
    //                int outter_end_x = static_cast<int>(static_cast<double>(outer_range_pix)*
    //                        qSin(deg2rad(bearing+gz_sett.end_bearing)));
    //                int outter_end_y = static_cast<int>(static_cast<double>(-outer_range_pix)*
    //                        qCos(deg2rad(bearing+gz_sett.end_bearing)));

    //                int startAngle;
    //                int spanAngle;

    //                //        qDebug()<<inner_range_pix<<outer_range_pix<<side<<gz_sett.inner_range<<gz_sett.outer_range<<curRange;
    //                painter.drawLine(inner_start_x,inner_start_y,outter_start_x,outter_start_y);
    //                painter.drawLine(inner_end_x,inner_end_y,outter_end_x,outter_end_y);

    //                if(gz_sett.start_bearing<gz_sett.end_bearing)
    //                {
    //                    rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
    //                    startAngle = static_cast<int>((90-(bearing+gz_sett.end_bearing)) * 16.);
    ////                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16.);
    //                    spanAngle = static_cast<int>((gz_sett.end_bearing-gz_sett.start_bearing) * 16.);

    //                    painter.drawArc(rectangle, startAngle, spanAngle);

    //                    rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
    //                    startAngle = static_cast<int>((90-(bearing+gz_sett.end_bearing)) * 16.);
    ////                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16.);
    //                    spanAngle = static_cast<int>((gz_sett.end_bearing-gz_sett.start_bearing) * 16.);

    //                    painter.drawArc(rectangle, startAngle, spanAngle);
    //                 }
    //                else
    //                {
    //                    rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
    //                    startAngle = static_cast<int>((90-(bearing+gz_sett.end_bearing)) * 16);
    ////                    startAngle = static_cast<int>((-(bearing+gz_sett.end_bearing)) * 16);
    //                    spanAngle = static_cast<int>((gz_sett.end_bearing-gz_sett.start_bearing+360) * 16);

    //                    painter.drawArc(rectangle, startAngle, spanAngle);

    //                    rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
    //                    startAngle = static_cast<int>((90-(bearing+gz_sett.end_bearing)) * 16);
    //                    spanAngle = static_cast<int>((gz_sett.end_bearing-gz_sett.start_bearing+360) * 16);

    //                    painter.drawArc(rectangle, startAngle, spanAngle);

    //                }
    //            }
    //            else
    //            {
    //                rectangle = QRect(-inner_range_pix, -inner_range_pix, 2*inner_range_pix, 2*inner_range_pix);
    //                painter.drawEllipse(rectangle);

    //                rectangle = QRect(-outer_range_pix, -outer_range_pix, 2*outer_range_pix, 2*outer_range_pix);
    //                painter.drawEllipse(rectangle);
    //            }
    //        }
    //    }

}

void RadarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glClearColor(0.,0.,0.,1.);
    glEnable(GL_MULTISAMPLE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    setupViewport(width(), height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glScaled(.5, .5, .5);

//    qDebug()<<Q_FUNC_INFO;

    m_re->radarDraw->DrawRadarImage();

    /*
    glBegin(GL_LINES);
    glColor3f(0,1,0);
    glVertex2f(0,0);
    glVertex2f(sin(static_cast<float>(deg2rad(cur_radar_angle))),
               cos(static_cast<float>(deg2rad(cur_radar_angle))));
    glEnd();

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    */


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::SolidLine);
    painter.setPen(QColor(255,255,0,255));
    painter.translate(width()/2,height()/2);

    int side = region.width()/2;
    const bool show_compass = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool();
    const bool show_heading_marker = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool();
    const bool show_rings = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool();
    const bool show_arpa = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_ARPA).toBool();
    const bool show_gz = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_GZ).toBool();
    const bool heading_up = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
    const double currentHeading = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

    /*
    */
    if(show_compass) drawCompass(&painter,side,heading_up,currentHeading);

    /*
      heading marker
    */
    if(show_heading_marker) drawHM(&painter, side, heading_up, currentHeading);

    /*
      range ring
    */
    if(show_rings) drawRings(&painter,side);


    /*
     * arpa
    */
    if(show_arpa) drawArpa(&painter);


    /*
     * Guardzone
     */
    if(show_gz) drawGZ(&painter);

//    painter.rotate(-bearing+180);

//    painter.end();
}

//void RadarWidget::createMARPA(QPoint pos)
//{
//    qDebug()<<Q_FUNC_INFO<<pos<<size()<<geometry().topLeft();

//    int side = qMin(region.width(), region.height())/2;
//    int centerX = pos.x()-(width()/2);
//    int centerY = pos.y()-(height()/2);
//    QPoint center_pos = QPoint(centerX,-centerY);
////    QPoint center_pos = QPoint(-centerY,-centerX);
//    Polar pol;
//    double deg = MOD_DEGREES(rad2deg(atan2(center_pos.x(),center_pos.y())));
////    double deg = MOD_DEGREES(rad2deg(atan2(-center_pos.y(),center_pos.x())));
//    double range_scale = sqrt(pow(center_pos.x(),2)+pow(center_pos.y(),2))/side;
//    pol.angle = SCALE_DEGREES_TO_RAW2048(deg);
//    pol.r = range_scale*RETURNS_PER_LINE;
//    qDebug()<<Q_FUNC_INFO<<"mouse pos"<<center_pos
//           <<"polar angle"<<pol.angle
//          <<"polar range"<<pol.r
//         <<"side"<<side
//        <<"range meter"<<range_scale*2*curRange
//       <<"radar meter"<<2*curRange
//      <<"degree"<<deg;
//    if (pol.r >= RETURNS_PER_LINE || pol.r <= 0)
//    {
//        qDebug()<<"out of avail area";
//        //        emit warning_MARPA_out_of_area();
//        return;
//    }

//    Position own_pos;
//    own_pos.lat = currentOwnShipLat;
//    own_pos.lon = currentOwnShipLon;
//    Position target_pos = Polar2Pos(pol,own_pos,curRange);
//    qDebug()<<Q_FUNC_INFO<<target_pos.lat<<target_pos.lon;

//    arpa->AcquireNewMARPATarget(target_pos);
//}
void RadarWidget::trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8 *data, size_t len)
{
//    qDebug()<<Q_FUNC_INFO<<angle;
//    cur_radar_angle = SCALE_RAW_TO_DEGREES2048(angle);
    m_re->radarDraw->ProcessRadarSpoke(angle,data,len);
    update();
}
//void RadarWidget::setRange(int range)
//{
//    curRange = range;
//    arpa->range_meters = 2*range;
//    arpa1->range_meters = 2*range;
//    computetRingWidth();
//}
//void RadarWidget::computetRingWidth()
//{
//    int side = region.width()/2;
//    ringWidth = (qCeil(side/2)-20)*curRange/side/2;
//    ringWidth /=1000;
//}
//int RadarWidget::getRange()
//{
//    return curRange;
//}

//double RadarWidget::getRingWidth()
//{
//    return ringWidth;
//}

void RadarWidget::resizeGL(int width, int height)
{
    setupViewport(width, height);
}
void RadarWidget::setRectRegoin(QRect rect)
{
    region = QRect(0,0,rect.width(),rect.width());
}

RadarWidget::~RadarWidget()
{
}

void RadarWidget::initializeGL()
{
    glEnable(GL_MULTISAMPLE);
}

//void RadarWidget::mouseReleaseEvent(QMouseEvent *event)
//{
//    qDebug()<<Q_FUNC_INFO<<event->pos()<<event->globalPos();
//    if(event->button()==Qt::LeftButton && arpa_settings[0].create_arpa_by_click)
//        createMARPA(event->pos());
//    else if(event->button()==Qt::RightButton && arpa_settings[0].create_arpa_by_click)
//        qDebug()<<Q_FUNC_INFO<<"right button";
//}

//void RadarWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    Q_UNUSED(event);

//    int side = qMin(region.width(), region.height())/2;
//    QPoint os_pos(region.width()/2,region.height()/2);
//    QPoint mouse_pos = mapFromGlobal(QCursor::pos());
//    double range_pixel_x = os_pos.x()-mouse_pos.x();
//    double range_pixel_y = os_pos.y()-mouse_pos.y();
//    double bearing = atan2(range_pixel_y,range_pixel_x);
//    bearing = (bearing*180/M_PI)-90;
//    if(bearing<0)
//        bearing+=360;

//    double range = sqrt(pow(range_pixel_y,2)+pow(range_pixel_x,2)); //pixel
////    range = range*static_cast<double>(curRange)/static_cast<double>(side)/1000.;
//    //    qDebug()<<Q_FUNC_INFO<<range<<bearing<<mouse_pos<<side;
//    emit signal_cursorMove(range,bearing);
//}
void RadarWidget::setupViewport(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}
