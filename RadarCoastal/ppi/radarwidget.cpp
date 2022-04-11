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
    rd = static_cast<RadarEngine::RDVert*>(m_re->radarDraw);

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

/*
void RadarWidget::trigger_ReqDelTrack(bool r1,int id)
{
    if(id>-10)
    {
        RA *cur_arpa = r1 ? arpa : arpa1;
        for(int i=0;i<cur_arpa->m_number_of_targets;i++)
            if(cur_arpa->m_target[i]->m_target_id == id)
                cur_arpa->m_target[i]->SetStatusLost();
    }
    else
    {
        arpa->DeleteAllTargets();
        arpa1->DeleteAllTargets();
    }
}
*/

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
    Q_UNUSED(event)

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


    /*
    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);

    GLfloat vertices[] = {
        0.0f, 0.407f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();
    */


    m_re->radarDraw->DrawRadarImage();

//    m_program->bind();
//    glEnableVertexAttribArray(GL_VERTEX_ARRAY);
//    glEnableVertexAttribArray(GL_COLOR_ARRAY);
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);

//    for (size_t i = 0; i < LINES_PER_ROTATION; i++)
//    {
//        RadarEngine::RDVert::VertexLine* line = &rd->m_vertices[i];

////        glVertexPointer(2, GL_FLOAT, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].x);
////        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].red);
////        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(line->count));
//        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].x);
//        glVertexAttribPointer(m_colAttr, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].red);
////        glVertexAttribPointer(GL_VERTEX_ARRAY_POINTER, 2, GL_FLOAT, GL_TRUE, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].x);
////        glVertexAttribPointer(GL_COLOR_ARRAY_POINTER, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RadarEngine::RDVert::VertexPoint), &line->points[0].red);
////        glEnableVertexAttribArray(0);
////        glEnableVertexAttribArray(1);
////        qDebug()<<Q_FUNC_INFO<<"line->count"<<line->count;
////        qDebug()<<Q_FUNC_INFO<<"line->point"<<line->points[0].x<<line->points[0].y;
//        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(line->count));
//    }

//    GLfloat vertices[] = {
//        0.0f, 0.0f,
//        -.7f, .7f,
//    };

//    GLfloat colors[] = {
//        0.0f, 1.0f, 0.0f,
//        0.0f, 1.0f, 0.0f,
//        0.0f, 1.0f, 0.0f
//    };

//    vertices[2] = sin(static_cast<float>(deg2rad(cur_radar_angle)));
//    vertices[3] = cos(static_cast<float>(deg2rad(cur_radar_angle)));
//    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
//    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

//    glDrawArrays(GL_LINES, 0, 2);

//    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
//    glDisableClientState(GL_COLOR_ARRAY);
//    glDisableVertexAttribArray(GL_VERTEX_ARRAY);  // disable vertex arrays
//    glDisableVertexAttribArray(GL_COLOR_ARRAY);
//    glDisableVertexAttribArray(1);  // disable vertex arrays
//    glDisableVertexAttribArray(0);
//    m_program->release();



    /*
    glBegin(GL_LINES);
    glColor3f(0,1,0);
    glVertex2f(0,0);
    glVertex2f(sin(static_cast<float>(deg2rad(cur_radar_angle))),
               cos(static_cast<float>(deg2rad(cur_radar_angle))));
    glEnd();
    */

//    m_re->radarDraw->DrawRadarImage();
//    rShader->DrawRadarImage();

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();


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
/*
void RadarWidget::createMARPA(QPoint pos)
{
    qDebug()<<Q_FUNC_INFO<<pos<<size()<<geometry().topLeft();

    int side = qMin(region.width(), region.height())/2;
    int centerX = pos.x()-(width()/2);
    int centerY = pos.y()-(height()/2);
    QPoint center_pos = QPoint(centerX,-centerY);
//    QPoint center_pos = QPoint(-centerY,-centerX);
    Polar pol;
    double deg = MOD_DEGREES(rad2deg(atan2(center_pos.x(),center_pos.y())));
//    double deg = MOD_DEGREES(rad2deg(atan2(-center_pos.y(),center_pos.x())));
    double range_scale = sqrt(pow(center_pos.x(),2)+pow(center_pos.y(),2))/side;
    pol.angle = SCALE_DEGREES_TO_RAW2048(deg);
    pol.r = range_scale*RETURNS_PER_LINE;
    qDebug()<<Q_FUNC_INFO<<"mouse pos"<<center_pos
           <<"polar angle"<<pol.angle
          <<"polar range"<<pol.r
         <<"side"<<side
        <<"range meter"<<range_scale*2*curRange
       <<"radar meter"<<2*curRange
      <<"degree"<<deg;
    if (pol.r >= RETURNS_PER_LINE || pol.r <= 0)
    {
        qDebug()<<"out of avail area";
        //        emit warning_MARPA_out_of_area();
        return;
    }

    Position own_pos;
    own_pos.lat = currentOwnShipLat;
    own_pos.lon = currentOwnShipLon;
    Position target_pos = Polar2Pos(pol,own_pos,curRange);
    qDebug()<<Q_FUNC_INFO<<target_pos.lat<<target_pos.lon;

    arpa->AcquireNewMARPATarget(target_pos);
}
*/
void RadarWidget::trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8 *data, size_t len)
{
//    qDebug()<<Q_FUNC_INFO<<angle;
    cur_radar_angle = SCALE_RAW_TO_DEGREES2048(angle);
    m_re->radarDraw->ProcessRadarSpoke(angle,data,len);
//    rShader->ProcessRadarSpoke(angle,data,len);
    update();
}

/*
void RadarWidget::setRange(int range)
{
    curRange = range;
    arpa->range_meters = 2*range;
    arpa1->range_meters = 2*range;
    computetRingWidth();
}
void RadarWidget::computetRingWidth()
{
    int side = region.width()/2;
    ringWidth = (qCeil(side/2)-20)*curRange/side/2;
    ringWidth /=1000;
}
int RadarWidget::getRange()
{
    return curRange;
}

double RadarWidget::getRingWidth()
{
    return ringWidth;
}
*/

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
/*
*/
static const char *vertexShaderSource =
      "attribute highp vec4 posAttr;\n"
      "attribute lowp vec4 colAttr;\n"
      "varying lowp vec4 col;\n"
//      "uniform highp mat4 matrix;\n"
      "void main() {\n"
      "   col = colAttr;\n"
      "   gl_Position = posAttr;\n"
//      "   gl_Position = matrix * posAttr;\n"
      "}\n";

  static const char *fragmentShaderSource =
      "varying lowp vec4 col;\n"
      "void main() {\n"
      "   gl_FragColor = col;\n"
      "}\n";

void RadarWidget::initializeGL()
{
    initializeOpenGLFunctions();
//    rShader = new RDShade(m_re);

    glEnable(GL_MULTISAMPLE);
//    rShader->init();
    m_re->radarDraw->init(this);

    /*
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    */
}

/*
void RadarWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<Q_FUNC_INFO<<event->pos()<<event->globalPos();
    if(event->button()==Qt::LeftButton && arpa_settings[0].create_arpa_by_click)
        createMARPA(event->pos());
    else if(event->button()==Qt::RightButton && arpa_settings[0].create_arpa_by_click)
        qDebug()<<Q_FUNC_INFO<<"right button";
}
*/

/*
void RadarWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    int side = qMin(region.width(), region.height())/2;
    QPoint os_pos(region.width()/2,region.height()/2);
    QPoint mouse_pos = mapFromGlobal(QCursor::pos());
    double range_pixel_x = os_pos.x()-mouse_pos.x();
    double range_pixel_y = os_pos.y()-mouse_pos.y();
    double bearing = atan2(range_pixel_y,range_pixel_x);
    bearing = (bearing*180/M_PI)-90;
    if(bearing<0)
        bearing+=360;

    double range = sqrt(pow(range_pixel_y,2)+pow(range_pixel_x,2)); //pixel
//    range = range*static_cast<double>(curRange)/static_cast<double>(side)/1000.;
    //    qDebug()<<Q_FUNC_INFO<<range<<bearing<<mouse_pos<<side;
    emit signal_cursorMove(range,bearing);
}
*/
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




static const char *VertexShaderText =
    "void main() \n"
    "{ \n"
    "   gl_TexCoord[0] = gl_MultiTexCoord0; \n"
    "   gl_Position = ftransform(); \n"
    "} \n";

static const char *FragmentShaderColorText =
    "uniform sampler2D tex2d; \n"
    "void main() \n"
    "{ \n"
    "   float d = length(gl_TexCoord[0].xy);\n"
    "   if (d >= 1.0) \n"
    "      discard; \n"
    "   float a = atan(gl_TexCoord[0].x, gl_TexCoord[0].y) / 6.28318; \n"
    "   gl_FragColor = texture2D(tex2d, vec2(d, a)); \n"
    "} \n";

RadarWidget::RDShade::RDShade(RadarEngine::RadarEngine *re):
    m_re(re)
{
}

void RadarWidget::RDShade::init()
{
    initializeOpenGLFunctions();

    m_start_line = -1;  // No spokes received since last draw
    m_lines = 0;
    m_format = GL_BGRA;
//    m_format = QOpenGLTexture::RGBA8_UNorm;
    m_channels = SHADER_COLOR_CHANNELS;

    m_vertex = new QOpenGLShader(QOpenGLShader::Vertex);
    m_vertex->compileSourceCode(VertexShaderText);

    m_fragment = new QOpenGLShader(QOpenGLShader::Fragment);
    m_fragment->compileSourceCode(FragmentShaderColorText);

    m_program = new QOpenGLShaderProgram;
    m_program->addShader(m_vertex);
    m_program->addShader(m_fragment);
//    m_program->bindAttributeLocation("gl_TexCoord[0]", 0);
    //          m_environmentProgram->bindAttributeLocation("aTexCoord", 1);
    m_program->link();
    m_program->bind();
    m_program->setUniformValue("tex2d", 0);

    glGenTextures(1,&m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(/* target          = */ GL_TEXTURE_2D,
                  /* level           = */ 0,
                  /* internal_format = */ m_format,
                  /* width           = */ RETURNS_PER_LINE,
                  /* heigth          = */ LINES_PER_ROTATION,
                  /* border          = */ 0,
                  /* format          = */ m_format,
                  /* type            = */ GL_UNSIGNED_BYTE,
                  /* data            = */ m_data);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*
    m_texture = new QOpenGLTexture(QImage(QDir::homePath()+QDir::separator()+"side1.png").mirrored());
    m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    m_texture->create();

    m_texture->setSize(100, 100, 1);
    m_texture->setFormat(m_format);
    m_texture->allocateStorage(QOpenGLTexture::BGRA,QOpenGLTexture::UInt8);
//    QImage image = QPixmap(10,10).toImage();
    QImage image(QDir::homePath()+QDir::separator()+"side1.png");
    m_texture->setData(0,0,0,100,100,1,0,QOpenGLTexture::BGRA,QOpenGLTexture::UInt8,static_cast<const void*>(image.bits()));
    */

    memset(&m_data,0,sizeof(unsigned char)*SHADER_COLOR_CHANNELS * LINES_PER_ROTATION * RETURNS_PER_LINE);

    /*

    float vertices[4][8] = {
        {-1.f,  -1.f, 0.0f,   -1.f, -1.f},
        {1.f, -1.f, 0.0f,   1.f, 1.0f},
        {1.f, 1.f, 0.0f,  1.0f, 1.0f},
        {-1.f,  1.f, 0.0f,  -1.0f, 1.f}
    };
    for(int j = 0; j < 4; ++j)
    {
        vertData.append(0.2*vertices[j][0]);
        vertData.append(0.2*vertices[j][1]);
        vertData.append(0.2*vertices[j][2]);

        vertData.append(vertices[j][3]);
        vertData.append(vertices[j][4]);
    }
    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
    */
}

void RadarWidget::RDShade::ProcessRadarSpoke(int angle, UINT8* data, size_t len)
{
    GLubyte alpha = 255;

    if (m_start_line == -1) m_start_line = angle;  // Note that this only runs once after each draw,
    if (m_lines < LINES_PER_ROTATION) m_lines++;
    if (m_channels == SHADER_COLOR_CHANNELS)
    {
        unsigned char *d = m_data + (angle * RETURNS_PER_LINE) * m_channels;
        for (size_t r = 0; r < len; r++)
        {
            GLubyte strength = data[r];
            RadarEngine::BlobColour colour = m_re->m_colour_map[strength];
            d[0] = static_cast<unsigned char>(m_re->m_colour_map_rgb[colour].red());
            d[1] = static_cast<unsigned char>(m_re->m_colour_map_rgb[colour].green());
            d[2] = static_cast<unsigned char>(m_re->m_colour_map_rgb[colour].blue());
            d[3] = colour != RadarEngine::BLOB_NONE ? alpha : 0;
            d += m_channels;
        }
    }
    else
    {
        unsigned char *d = m_data + (angle * RETURNS_PER_LINE);
        for (size_t r = 0; r < len; r++)
        {
            GLubyte strength = data[r];
            RadarEngine::BlobColour colour = m_re->m_colour_map[strength];
            *d++ = (static_cast<unsigned char>(m_re->m_colour_map_rgb[colour].red()) * alpha) >> 8;
        }
    }
}

void RadarWidget::RDShade::DrawRadarImage()
{
      if (!m_program || !m_texture) {
        return;
      }


      m_program->bind();
//      vbo.bind();
//      m_texture->bind();

      glPushAttrib(GL_TEXTURE_BIT);
      glBindTexture(GL_TEXTURE_2D, m_texture);

      if (m_start_line > -1) {
        // Since the last time we have received data from [m_start_line, m_end_line>
        // so we only need to update the texture for those data lines.
        if (m_start_line + m_lines > LINES_PER_ROTATION) {
          int end_line = MOD_ROTATION2048(m_start_line + m_lines);
          // if the new data partly wraps past the end of the texture
          // tell it the two parts separately
          // First remap [0, m_end_line>
//          m_texture->setData(0,0,0,RETURNS_PER_LINE,end_line,1,0,QOpenGLTexture::BGRA,QOpenGLTexture::UInt8,static_cast<const void*>(m_data));

          glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                          /* level =    */ 0,
                          /* x-offset = */ 0,
                          /* y-offset = */ 0,
                          /* width =    */ RETURNS_PER_LINE,
                          /* height =   */ end_line,
                          /* format =   */ m_format,
                          /* type =     */ GL_UNSIGNED_BYTE,
                          /* pixels =   */ m_data);
          // And then remap [m_start_line, LINES_PER_ROTATION>

//          qDebug()<<Q_FUNC_INFO<<"m_start_line * RETURNS_PER_LINE * m_channels"<<m_start_line * RETURNS_PER_LINE * m_channels;
//          qDebug()<<Q_FUNC_INFO<<"m_data"<<m_data[ m_start_line * RETURNS_PER_LINE * m_channels ];
//          m_texture->setData(0,m_start_line,0,RETURNS_PER_LINE,LINES_PER_ROTATION - m_start_line,1,0,QOpenGLTexture::BGRA,QOpenGLTexture::UInt8,static_cast<const void*>(m_data + m_start_line * RETURNS_PER_LINE * m_channels));


          glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                          /* level =    */ 0,
                          /* x-offset = */ 0,
                          /* y-offset = */ m_start_line,
                          /* width =    */ RETURNS_PER_LINE,
                          /* height =   */ LINES_PER_ROTATION - m_start_line,
                          /* format =   */ m_format,
                          /* type =     */ GL_UNSIGNED_BYTE,
                          /* pixels =   */ m_data + m_start_line * RETURNS_PER_LINE * m_channels);

        } else {
          // Map [m_start_line, m_end_line>
//            qDebug()<<Q_FUNC_INFO<<"m_start_line * RETURNS_PER_LINE * m_channels"<<m_start_line * RETURNS_PER_LINE * m_channels;
//            qDebug()<<Q_FUNC_INFO<<"m_data"<<m_data[ m_start_line * RETURNS_PER_LINE * m_channels ];
//            QImage image(100,100, QImage::Format_ARGB32);
//            QRgb value;

//             value = qRgba(189, 149, 39, 255); // 0xffbd9527
//             for(int i = 0; i < 25; i++) image.setPixel(i, i, value);

//             value = qRgba(122, 163, 39, 255); // 0xff7aa327
//             for(int i = 25; i < 50; i++)
//             {
//             image.setPixel(26, i, value);
//             image.setPixel(i, 26, value);
//             }

//             value = qRgba(237, 187, 51, 255); // 0xffedba31
//             for(int i = 50; i < 75; i++)
//             {
//             image.setPixel(50, i, value);
//             image.setPixel(i, 50, value);
//             }

//             m_texture->setData(0,0,0,RETURNS_PER_LINE,500,1,1,QOpenGLTexture::BGRA,QOpenGLTexture::UInt8,static_cast<const void*>(image.bits()));
//            m_texture->setData(0,m_start_line,0,RETURNS_PER_LINE,m_lines,1,0,QOpenGLTexture::BGRA,QOpenGLTexture::UInt8,static_cast<const void*>(m_data + m_start_line * RETURNS_PER_LINE * m_channels));


            glTexSubImage2D(/* target =   */ GL_TEXTURE_2D,
                          /* level =    */ 0,
                          /* x-offset = */ 0,
                          /* y-offset = */ m_start_line,
                          /* width =    */ RETURNS_PER_LINE,
                          /* height =   */ m_lines,
                          /* format =   */ m_format,
                          /* type =     */ GL_UNSIGNED_BYTE,
                          /* pixels =   */ m_data + m_start_line * RETURNS_PER_LINE * m_channels);

        }
        m_start_line = -1;
        m_lines = 0;
      }

      // We tell the GPU to draw a square from (-512,-512) to (+512,+512).
      // The shader morphs this into a circle.
      glDrawArrays(GL_QUADS,0,4);
      /*
      */
      float fullscale = 1;
      glBegin(GL_QUADS);
      glTexCoord2f(-1, -1);
      glVertex2f(-fullscale, -fullscale);
      glTexCoord2f(1, -1);
      glVertex2f(fullscale, -fullscale);
      glTexCoord2f(1, 1);
      glVertex2f(fullscale, fullscale);
      glTexCoord2f(-1, 1);
      glVertex2f(-fullscale, fullscale);
      glEnd();

      /*
      float fullscale = 1;
      glBegin(GL_QUADS);
      glTexCoord2f(0., 1.);
      glVertex2f(-fullscale, -fullscale);
      glTexCoord2f(1., 1.);
      glVertex2f(fullscale, -fullscale);
      glTexCoord2f(1., 0.);
      glVertex2f(fullscale, fullscale);
      glTexCoord2f(0., 0.);
      glVertex2f(-fullscale, fullscale);
      glEnd();
      */

//      m_texture->release();
//      vbo.release();
      m_program->release();
      glPopAttrib();
}
